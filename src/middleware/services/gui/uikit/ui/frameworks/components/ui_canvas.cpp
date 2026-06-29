/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "components/ui_canvas.h"
#include "common/image.h"
#include "draw/draw_arc.h"
#include "draw/draw_image.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "gfx_utils/graphic_log.h"
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
#include "common/graphic_hardware_types.h"
#include "draw/hw_draw_utils.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "hals/gralloc_engines.h"
#include "hals/display_dev.h"
#endif
namespace OHOS {
UICanvas::UICanvasPath::~UICanvasPath()
{
    points_.Clear();
    cmd_.Clear();
    arcParam_.Clear();
}

BufferInfo* UICanvas::gfxMapBuffer_ = nullptr;
void UICanvas::BeginPath()
{
    /* If the previous path is not added to the drawing linked list, it should be destroyed directly. */
    if (path_ != nullptr && path_->strokeCount_ == 0) {
        delete path_;
        path_ = nullptr;
    }

    path_ = new UICanvasPath();
    if (path_ == nullptr) {
        GRAPHIC_LOGE("new UICanvasPath fail");
        return;
    }
}

void UICanvas::MoveTo(const Point& point)
{
    if (path_ == nullptr) {
        return;
    }

    path_->startPos_ = point;
    /* If the previous command is also CMD_MOVE_TO, the previous command is overwritten. */
    if ((path_->cmd_.Size() != 0) && (path_->cmd_.Tail()->data_ == SWPathCmd::SW_CMD_MOVE_TO)) {
        path_->points_.Tail()->data_ = point;
        return;
    }
    path_->points_.PushBack(point);
    path_->cmd_.PushBack(SWPathCmd::SW_CMD_MOVE_TO);
}

void UICanvas::LineTo(const Point& point)
{
    if (path_ == nullptr) {
        return;
    }

    path_->points_.PushBack(point);
    if (path_->cmd_.Size() == 0) {
        path_->startPos_ = point;
        path_->cmd_.PushBack(SWPathCmd::SW_CMD_MOVE_TO);
    } else {
        path_->cmd_.PushBack(SWPathCmd::SW_CMD_LINE_TO);
    }
}

void UICanvas::ArcTo(const Point& center, uint16_t radius, int16_t startAngle, int16_t endAngle)
{
    if (path_ == nullptr) {
        return;
    }

    /*
     * If there is no command before SWPathCmd::SW_CMD_ARC, only the arc is drawn. If there is a command in front of
     * SWPathCmd::SW_CMD_ARC, the start point of arc must be connected to the end point of the path.
     */
    float sinma = radius * Sin(startAngle);
    float cosma = radius * Sin(QUARTER_IN_DEGREE - startAngle);
    if (path_->cmd_.Size() != 0) {
        path_->points_.PushBack({MATH_ROUND(center.x + sinma), MATH_ROUND(center.y - cosma)});
        path_->cmd_.PushBack(SWPathCmd::SW_CMD_LINE_TO);
    } else {
        path_->startPos_ = {MATH_ROUND(center.x + sinma), MATH_ROUND(center.y - cosma)};
    }

    /* If the ARC scan range exceeds 360 degrees, the end point of the path is the position of the start angle. */
    if (MATH_ABS(startAngle - endAngle) < CIRCLE_IN_DEGREE) {
        sinma = radius * Sin(endAngle);
        cosma = radius * Sin(QUARTER_IN_DEGREE - endAngle);
    }
    path_->points_.PushBack({MATH_ROUND(center.x + sinma), MATH_ROUND(center.y - cosma)});
    path_->cmd_.PushBack(SWPathCmd::SW_CMD_ARC);

    int16_t start;
    int16_t end;
    if (startAngle > endAngle) {
        start = endAngle;
        end = startAngle;
    } else {
        start = startAngle;
        end = endAngle;
    }

    DrawArc::GetInstance()->GetDrawRange(start, end);
    ArcParam param;
    param.center = center;
    param.radius = radius;
    param.startAngle = start;
    param.endAngle = end;
    path_->arcParam_.PushBack(param);
}

void UICanvas::AddRect(const Point& point, int16_t height, int16_t width)
{
    if (path_ == nullptr) {
        return;
    }

    MoveTo(point);
    LineTo({static_cast<int16_t>(point.x + width), point.y});
    LineTo({static_cast<int16_t>(point.x + width), static_cast<int16_t>(point.y + height)});
    LineTo({point.x, static_cast<int16_t>(point.y + height)});
    ClosePath();
}

void UICanvas::ClosePath()
{
    if ((path_ == nullptr) || (path_->cmd_.Size() == 0)) {
        return;
    }

    path_->points_.PushBack(path_->startPos_);
    path_->cmd_.PushBack(SWPathCmd::SW_CMD_CLOSE);
}

UICanvas::~UICanvas()
{
    if ((path_ != nullptr) && (path_->strokeCount_ == 0)) {
        delete path_;
        path_ = nullptr;
    }
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
    LiteMGfxEngine::GetInstance()->SyncHwDraw();
    matrices_.Clear();
#endif

    void* param = nullptr;
    ListNode<DrawCmd>* curDraw = drawCmdList_.Begin();
    for (; curDraw != drawCmdList_.End(); curDraw = curDraw->next_) {
        param = curDraw->data_.param;
        curDraw->data_.DeleteParam(param);
        curDraw->data_.param = nullptr;
    }
    drawCmdList_.Clear();
    DestroyMapBufferInfo();
}

void UICanvas::Clear()
{
    if ((path_ != nullptr) && (path_->strokeCount_ == 0)) {
        delete path_;
        path_ = nullptr;
    }
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
    LiteMGfxEngine::GetInstance()->SyncHwDraw();
    matrices_.Clear();
#endif
    void* param = nullptr;
    ListNode<DrawCmd>* curDraw = drawCmdList_.Begin();
    for (; curDraw != drawCmdList_.End(); curDraw = curDraw->next_) {
        param = curDraw->data_.param;
        curDraw->data_.DeleteParam(param);
        curDraw->data_.param = nullptr;
    }
    drawCmdList_.Clear();
    path_ = nullptr;
    Invalidate();
}

void UICanvas::DrawLine(const Point& endPoint, const Paint& paint)
{
    DrawLine(startPoint_, endPoint, paint);
}

void UICanvas::DrawLine(const Point& startPoint, const Point& endPoint, const Paint& paint)
{
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
    CanvasDrawParam* pathParam = CanvasHwDrawer::CreateCanvasDrawParam(PARAM_TYPE_PATH, &paint, this);
    if (pathParam == nullptr) {
        return;
    }

    if (!HWDrawUtils::GetInstance()->ObtainLinePath(startPoint, endPoint, pathParam->object.path)) {
        CanvasHwDrawer::DestoryCanvasDrawParam(pathParam);
        return;
    }
    AddHwDrawCmdToList(pathParam);
#else
    LineParam* lineParam = new LineParam;
    if (lineParam == nullptr) {
        GRAPHIC_LOGE("new LineParam fail");
        return;
    }
    lineParam->start = startPoint;
    lineParam->end = endPoint;

    DrawCmd cmd;
    cmd.paint = paint;
    cmd.param = lineParam;
    cmd.DeleteParam = DeleteLineParam;
    cmd.DrawGraphics = DoDrawLine;
    drawCmdList_.PushBack(cmd);

    Invalidate();
#endif
    SetStartPosition(endPoint);
}

void UICanvas::DrawCurve(const Point& control1, const Point& control2, const Point& endPoint, const Paint& paint)
{
    DrawCurve(startPoint_, control1, control2, endPoint, paint);
}

void UICanvas::DrawCurve(const Point& startPoint,
                         const Point& control1,
                         const Point& control2,
                         const Point& endPoint,
                         const Paint& paint)
{
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
    CanvasDrawParam* pathParam = CanvasHwDrawer::CreateCanvasDrawParam(PARAM_TYPE_PATH, &paint, this);
    if (pathParam == nullptr) {
        return;
    }

    if (!HWDrawUtils::GetInstance()->ObtainCurvePath(startPoint, control1, control2, endPoint, pathParam->object.path)) {
        CanvasHwDrawer::DestoryCanvasDrawParam(pathParam);
        return;
    }

    AddHwDrawCmdToList(pathParam);
#else
    CurveParam* curveParam = new CurveParam;
    if (curveParam == nullptr) {
        GRAPHIC_LOGE("new CurveParam fail");
        return;
    }
    curveParam->start = startPoint;
    curveParam->control1 = control1;
    curveParam->control2 = control2;
    curveParam->end = endPoint;

    DrawCmd cmd;
    cmd.paint = paint;
    if (paint.GetStrokeWidth() > MAX_CURVE_WIDTH) {
        cmd.paint.SetStrokeWidth(MAX_CURVE_WIDTH);
    }
    cmd.param = curveParam;
    cmd.DeleteParam = DeleteCurveParam;
    cmd.DrawGraphics = DoDrawCurve;
    drawCmdList_.PushBack(cmd);

    Invalidate();
#endif
    SetStartPosition(endPoint);
}

void UICanvas::DeleteRectParam(void* param)
{
    RectParam* rectParam = static_cast<RectParam*>(param);
    delete rectParam;
}

void UICanvas::DrawRect(const Point& startPoint, int16_t height, int16_t width, const Paint& paint)
{
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
    CanvasDrawParam* pathParam = CanvasHwDrawer::CreateCanvasDrawParam(PARAM_TYPE_PATH, &paint, this);
    if (pathParam == nullptr) {
        return;
    }

    if (!HWDrawUtils::GetInstance()->ObtainRectPath(startPoint, height, width, 0, pathParam->object.path)) {
        CanvasHwDrawer::DestoryCanvasDrawParam(pathParam);
        return;
    }

    AddHwDrawCmdToList(pathParam);
#else
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::STROKE_STYLE) {
        RectParam* rectParam = new RectParam;
        if (rectParam == nullptr) {
            GRAPHIC_LOGE("new RectParam fail");
            return;
        }
        rectParam->start = startPoint;
        rectParam->height = height;
        rectParam->width = width;

        DrawCmd cmd;
        cmd.paint = paint;
        cmd.param = rectParam;
        cmd.DeleteParam = DeleteRectParam;
        cmd.DrawGraphics = DoDrawRect;
        drawCmdList_.PushBack(cmd);
    }

    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
        RectParam* rectParam = new RectParam;
        if (rectParam == nullptr) {
            GRAPHIC_LOGE("new RectParam fail");
            return;
        }
        rectParam->start = startPoint;
        rectParam->height = height;
        rectParam->width = width;

        DrawCmd cmd;
        cmd.paint = paint;
        cmd.param = rectParam;
        cmd.DeleteParam = DeleteRectParam;
        cmd.DrawGraphics = DoFillRect;
        drawCmdList_.PushBack(cmd);
    }

    Invalidate();
#endif
}

void UICanvas::StrokeRect(const Point& startPoint, int16_t height, int16_t width, const Paint& paint)
{
    if (!paint.GetChangeFlag()) {
        RectParam* rectParam = new RectParam;
        if (rectParam == nullptr) {
            GRAPHIC_LOGE("new RectParam fail");
            return;
        }
        rectParam->start = startPoint;
        rectParam->height = height;
        rectParam->width = width;

        DrawCmd cmd;
        cmd.paint = paint;
        cmd.param = rectParam;
        cmd.DeleteParam = DeleteRectParam;
        cmd.DrawGraphics = DoDrawRect;
        drawCmdList_.PushBack(cmd);
    } else {
        BeginPath();
        MoveTo(startPoint);
        LineTo({static_cast<int16_t>(startPoint.x + width), startPoint.y});
        LineTo({static_cast<int16_t>(startPoint.x + width), static_cast<int16_t>(startPoint.y + height)});
        LineTo({startPoint.x, static_cast<int16_t>(startPoint.y + height)});
        ClosePath();
        DrawPath(paint);
    }
    SetStartPosition(startPoint);
}

void UICanvas::ClearRect(const Point& startPoint, int16_t height, int16_t width)
{
    Paint paint;
    paint.SetFillColor(this->style_->bgColor_);
    paint.SetStyle(Paint::FILL_STYLE);
    BeginPath();
    MoveTo(startPoint);
    LineTo({static_cast<int16_t>(startPoint.x + width), startPoint.y});
    LineTo({static_cast<int16_t>(startPoint.x + width), static_cast<int16_t>(startPoint.y + height)});
    LineTo({startPoint.x, static_cast<int16_t>(startPoint.y + height)});
    ClosePath();
    FillPath(paint);
}

void UICanvas::DrawCircle(const Point& center, uint16_t radius, const Paint& paint)
{
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
    CanvasDrawParam* pathParam = CanvasHwDrawer::CreateCanvasDrawParam(PARAM_TYPE_PATH, &paint, this);
    if (pathParam == nullptr) {
        return;
    }

    Path* path = pathParam->object.path;
    ArcInfo arcInfo = {center, {0, 0}, radius, 0, 0, nullptr};
    if (!HWDrawUtils::GetInstance()->ObtainCirclePath(arcInfo, path->cmdNum, path->cmds, path->dataNum, path->data)) {
        CanvasHwDrawer::DestoryCanvasDrawParam(pathParam);
        return;
    }

    AddHwDrawCmdToList(pathParam);
#else
    CircleParam* circleParam = new CircleParam;
    if (circleParam == nullptr) {
        GRAPHIC_LOGE("new CircleParam fail");
        return;
    }
    circleParam->center = center;
    circleParam->radius = radius;

    DrawCmd cmd;
    cmd.paint = paint;
    cmd.param = circleParam;
    cmd.DeleteParam = DeleteCircleParam;
    cmd.DrawGraphics = DoDrawCircle;
    drawCmdList_.PushBack(cmd);

    Invalidate();
#endif
}

void UICanvas::DrawSector(const Point& center,
                          uint16_t radius,
                          float startAngle,
                          float endAngle,
                          const Paint& paint)
{
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
    if (FloatEqual(startAngle, endAngle)) {
        return;
    }

    CanvasDrawParam* pathParam = CanvasHwDrawer::CreateCanvasDrawParam(PARAM_TYPE_PATH, &paint, this);
    if (pathParam == nullptr) {
        return;
    }

    Path* path = pathParam->object.path;
    ArcInfo arcInfo = {center, {0, 0}, radius, startAngle, endAngle, nullptr};
    if (!HWDrawUtils::GetInstance()->ObtainSectorPath(arcInfo, path->cmdNum, path->cmds, path->dataNum, path->data)) {
        CanvasHwDrawer::DestoryCanvasDrawParam(pathParam);
        return;
    }

    AddHwDrawCmdToList(pathParam);
#else
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
        Paint innerPaint = paint;
        innerPaint.SetStyle(Paint::PaintStyle::STROKE_STYLE);
        innerPaint.SetStrokeWidth(radius);
        innerPaint.SetStrokeColor(paint.GetFillColor());
        radius >>= 1;
        DrawArc(center, radius, startAngle, endAngle, innerPaint);
    }
#endif
}

void UICanvas::DrawArc(const Point& center, uint16_t radius, float startAngle, float endAngle, const Paint& paint)
{
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
        if (FloatEqual(startAngle, endAngle)) {
            return;
        }

        if (paint.GetStyle() != Paint::PaintStyle::STROKE_STYLE) {
            return DrawSector(center, radius, startAngle, endAngle, paint);
        }

        CanvasDrawParam* pathParam = CanvasHwDrawer::CreateCanvasDrawParam(PARAM_TYPE_PATH, &paint, this);
        if (pathParam == nullptr) {
            return;
        }

        Path* path = pathParam->object.path;
        ArcInfo arcInfo = {center, {0, 0}, radius, startAngle, endAngle, nullptr};
        if (!HWDrawUtils::GetInstance()->ObtainArcPath(arcInfo, path->cmdNum, path->cmds, path->dataNum, path->data)) {
            CanvasHwDrawer::DestoryCanvasDrawParam(pathParam);
            return;
        }

        AddHwDrawCmdToList(pathParam);
#else
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::STROKE_STYLE) {
        ArcParam* arcParam = new ArcParam;
        if (arcParam == nullptr) {
            GRAPHIC_LOGE("new ArcParam fail");
            return;
        }
        arcParam->center = center;
        arcParam->radius = radius;

        int16_t start;
        int16_t end;
        if (startAngle > endAngle) {
            start = endAngle;
            end = startAngle;
        } else {
            start = startAngle;
            end = endAngle;
        }

        DrawArc::GetInstance()->GetDrawRange(start, end);
        arcParam->startAngle = start;
        arcParam->endAngle = end;

        DrawCmd cmd;
        cmd.paint = paint;
        cmd.param = arcParam;
        cmd.DeleteParam = DeleteArcParam;
        cmd.DrawGraphics = DoDrawArc;
        drawCmdList_.PushBack(cmd);

        Invalidate();
    }
#endif
}

void UICanvas::DrawLabel(const Point& startPoint,
                         const char* text,
                         uint16_t maxWidth,
                         const FontStyle& fontStyle,
                         const Paint& paint)
{
    if (text == nullptr) {
        return;
    }
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
        CanvasDrawParam* param = CanvasHwDrawer::CreateCanvasDrawParam(PARAM_TYPE_LABEL, &paint, this);
        if (param == nullptr || param->object.label == nullptr) {
            return;
        }
        UILabel* label = param->object.label;
#else
        UILabel* label = new UILabel();
        if (label == nullptr) {
            GRAPHIC_LOGE("new UILabel fail");
            return;
        }
#endif
        label->SetLineBreakMode(UILabel::LINE_BREAK_CLIP);
        label->SetPosition(startPoint.x, startPoint.y);
        label->SetWidth(maxWidth);
        label->SetHeight(GetHeight());
        label->SetText(text);
        label->SetFont(fontStyle.fontName, fontStyle.fontSize);
        label->SetAlign(fontStyle.align, fontStyle.verticalAlign);
        label->SetDirect(fontStyle.direct);
        label->SetStyle(STYLE_LETTER_SPACE, fontStyle.letterSpace);
        label->SetStyle(STYLE_TEXT_COLOR, paint.GetFillColor().full);

#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
        AddHwDrawCmdToList(param);
#else
        DrawCmd cmd;
        cmd.param = label;
        cmd.DeleteParam = DeleteLabel;
        cmd.DrawGraphics = DoDrawLabel;
        drawCmdList_.PushBack(cmd);

        Invalidate();
#endif
    }
}
#if defined(GRAPHIC_ENABLE_DRAW_IMAGE_FLAG) && GRAPHIC_ENABLE_DRAW_IMAGE_FLAG
UIImageView* UICanvas::DrawImageInternal(const Point& startPoint, const Paint& paint)
{
    UIImageView* ret = nullptr;
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_ALPHA_BITMAP
        CanvasDrawParam* param = CanvasHwDrawer::CreateCanvasDrawParam(PARAM_TYPE_IMAGE, &paint, this);
        if (param == nullptr || param->object.imageView == nullptr) {
            return nullptr;
        }
        param->object.imageView->SetPosition(startPoint.x, startPoint.y);

        AddHwDrawCmdToList(param);
        ret = param->object.imageView;
#else
        UIImageView* imageView = new UIImageView();
        if (imageView == nullptr) {
            GRAPHIC_LOGE("new UIImageView fail");
            return nullptr;
        }
        imageView->SetPosition(startPoint.x, startPoint.y);
        DrawCmd cmd;
        cmd.param = imageView;
        cmd.paint = paint;
        cmd.DeleteParam = DeleteImageView;
        cmd.DrawGraphics = DoDrawImage;
        drawCmdList_.PushBack(cmd);

        Invalidate();
        ret = imageView;
#endif
        SetStartPosition(startPoint);
    }
    return ret;
}

void UICanvas::DrawImage(const Point& startPoint, const char* image, const Paint& paint)
{
    if (image == nullptr) {
        return;
    }

    UIImageView* imageView = DrawImageInternal(startPoint, paint);
    if (imageView == nullptr) {
        return;
    }
    imageView->SetSrc(image);
}

void UICanvas::DrawImage(const Point& startPoint, ImageInfo* image, const Paint& paint)
{
    if (image == nullptr) {
        return;
    }

    UIImageView* imageView = DrawImageInternal(startPoint, paint);
    if (imageView == nullptr) {
        return;
    }
    imageView->SetSrc(image);
}

void UICanvas::DrawImage(const Point& startPoint, const char* image,
                         const Paint& paint, int16_t width, int16_t height)
{
    if (image == nullptr) {
        return;
    }
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_ALPHA_BITMAP
        CanvasDrawParam* param = CanvasHwDrawer::CreateCanvasDrawParam(PARAM_TYPE_IMAGE, &paint, this);
        if (param == nullptr || param->object.imageView == nullptr) {
            return;
        }
        UIImageView* imageView = param->object.imageView;
#else
        UIImageView* imageView = new UIImageView();
        if (imageView == nullptr) {
            GRAPHIC_LOGE("new UIImageView fail");
            return;
        }
#endif
        imageView->SetPosition(startPoint.x, startPoint.y);
        imageView->SetSrc(image);
        float scaleX = 1.0;
        float scaleY = 1.0;
        if (width > 0 && imageView->GetWidth() > 0) {
            scaleX = (float)width / (float)imageView->GetWidth();
        }
        if (height > 0 && imageView->GetHeight() > 0) {
            scaleY = (float)height / (float)imageView->GetHeight();
        }
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_ALPHA_BITMAP
        param->paint->Scale(scaleX, scaleY);
        AddHwDrawCmdToList(param);
#else
        DrawCmd cmd;
        cmd.param = imageView;
        cmd.paint = paint;
        cmd.paint.Scale(scaleX, scaleY);
        cmd.DeleteParam = DeleteImageView;
        cmd.DrawGraphics = DoDrawImage;
        drawCmdList_.PushBack(cmd);

        Invalidate();
#endif
        SetStartPosition(startPoint);
    }
}
#endif

void UICanvas::DrawPath(const Paint& paint)
{
    if ((path_ == nullptr) || (path_->cmd_.Size() == 0)) {
        return;
    }

    path_->strokeCount_++;
    PathParam* param = new PathParam;
    if (param == nullptr) {
        GRAPHIC_LOGE("new PathParam fail");
        return;
    }
    param->path = path_;
    param->count = path_->cmd_.Size();

    DrawCmd cmd;
    cmd.paint = paint;
    cmd.param = param;
    cmd.DeleteParam = DeletePathParam;
    cmd.DrawGraphics = DoDrawPath;
    drawCmdList_.PushBack(cmd);
    Invalidate();
}

void UICanvas::FillPath(const Paint& paint)
{
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
    if ((path_ == nullptr) || (path_->cmd_.Size() == 0)) {
        return;
    }

    HWPathParam* pathParam = new HWPathParam;
    if (pathParam == nullptr) {
        GRAPHIC_LOGE("new PathParam fail");
        return;
    }

    Path* path = new Path();
    (void)memset_s(path, sizeof(Path), 0, sizeof(Path));
    if (!Convert2VGUPath(path_, path)) {
        GRAPHIC_LOGE("Convert2VGUPath fail");
        delete path;
        delete pathParam;
        return;
    }

    path_->strokeCount_++;
    pathParam->canvasPath = path_;
    pathParam->path = path;

    DrawCmd cmd;
    cmd.paint = paint;
    cmd.param = pathParam;
    cmd.DeleteParam = DeletePathParam;
    cmd.DrawGraphics = DoFillPath;
    drawCmdList_.PushBack(cmd);
    Invalidate();
#endif
}

void UICanvas::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    Rect rect = GetOrigRect();
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, rect, invalidatedArea, *style_, GetMixOpaScale());

    void* param = nullptr;
    ListNode<DrawCmd>* curDraw = drawCmdList_.Begin();
    Rect coords = GetOrigRect();
    Rect trunc = invalidatedArea;
    if (trunc.Intersect(trunc, coords)) {
        for (; curDraw != drawCmdList_.End(); curDraw = curDraw->next_) {
            param = curDraw->data_.param;
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
            if (curDraw->data_.DrawGraphics == CanvasHwDrawer::DoDraw) {
                CanvasDrawParam* drawParam = static_cast<CanvasDrawParam*>(param);
                if (drawParam->index < matrices_.Size() && drawParam->index >= 0) {
                    drawParam->matrix = matrices_[drawParam->index];
                }
                drawParam->mixOpa = GetMixOpaScale();
                trunc = invalidatedArea;
            }
#endif
            curDraw->data_.DrawGraphics(gfxDstBuffer, param, curDraw->data_.paint, rect, trunc, *style_);
        }
    }
}

void UICanvas::GetAbsolutePosition(const Point& prePoint, const Rect& rect, const Style& style, Point& point)
{
    point.x = prePoint.x + rect.GetLeft() + style.paddingLeft_ + style.borderWidth_;
    point.y = prePoint.y + rect.GetTop() + style.paddingTop_ + style.borderWidth_;
}

void UICanvas::DoDrawLine(BufferInfo& gfxDstBuffer,
                          void* param,
                          const Paint& paint,
                          const Rect& rect,
                          const Rect& invalidatedArea,
                          const Style& style)
{
    if (param == nullptr) {
        return;
    }
    LineParam* lineParam = static_cast<LineParam*>(param);
    Point start;
    Point end;
    GetAbsolutePosition(lineParam->start, rect, style, start);
    GetAbsolutePosition(lineParam->end, rect, style, end);

    BaseGfxEngine::GetInstance()->DrawLine(gfxDstBuffer, start, end, invalidatedArea, paint.GetStrokeWidth(),
                                           paint.GetStrokeColor(), paint.GetOpacity());
}

void UICanvas::DoDrawCurve(BufferInfo& gfxDstBuffer,
                           void* param,
                           const Paint& paint,
                           const Rect& rect,
                           const Rect& invalidatedArea,
                           const Style& style)
{
    if (param == nullptr) {
        return;
    }
    CurveParam* curveParam = static_cast<CurveParam*>(param);
    Point start;
    Point end;
    Point control1;
    Point control2;
    GetAbsolutePosition(curveParam->start, rect, style, start);
    GetAbsolutePosition(curveParam->end, rect, style, end);
    GetAbsolutePosition(curveParam->control1, rect, style, control1);
    GetAbsolutePosition(curveParam->control2, rect, style, control2);

    BaseGfxEngine::GetInstance()->DrawCubicBezier(gfxDstBuffer, start, control1, control2, end, invalidatedArea,
                                                  paint.GetStrokeWidth(), paint.GetStrokeColor(), paint.GetOpacity());
}

void UICanvas::DoDrawRect(BufferInfo& gfxDstBuffer,
                          void* param,
                          const Paint& paint,
                          const Rect& rect,
                          const Rect& invalidatedArea,
                          const Style& style)
{
    if (param == nullptr) {
        return;
    }
    RectParam* rectParam = static_cast<RectParam*>(param);
    Style drawStyle = StyleDefault::GetDefaultStyle();
    drawStyle.bgColor_ = paint.GetStrokeColor();
    drawStyle.bgOpa_ = paint.GetOpacity();
    drawStyle.borderRadius_ = 0;

    int16_t lineWidth = static_cast<int16_t>(paint.GetStrokeWidth());
    Point start;
    GetAbsolutePosition(rectParam->start, rect, style, start);

    int16_t x = start.x - lineWidth / 2; // 2: half
    int16_t y = start.y - lineWidth / 2; // 2: half
    Rect coords;
    if ((rectParam->height <= lineWidth) || (rectParam->width <= lineWidth)) {
        coords.SetPosition(x, y);
        coords.SetHeight(rectParam->height + lineWidth);
        coords.SetWidth(rectParam->width + lineWidth);
        BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, drawStyle, OPA_OPAQUE);
        return;
    }

    coords.SetPosition(x, y);
    coords.SetHeight(lineWidth);
    coords.SetWidth(rectParam->width);
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, drawStyle, OPA_OPAQUE);

    coords.SetPosition(x + rectParam->width, y);
    coords.SetHeight(rectParam->height);
    coords.SetWidth(lineWidth);
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, drawStyle, OPA_OPAQUE);

    coords.SetPosition(x, y + lineWidth);
    coords.SetHeight(rectParam->height);
    coords.SetWidth(lineWidth);
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, drawStyle, OPA_OPAQUE);

    coords.SetPosition(x + lineWidth, y + rectParam->height);
    coords.SetHeight(lineWidth);
    coords.SetWidth(rectParam->width);
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, drawStyle, OPA_OPAQUE);
}

void UICanvas::DoFillRect(BufferInfo& gfxDstBuffer,
                          void* param,
                          const Paint& paint,
                          const Rect& rect,
                          const Rect& invalidatedArea,
                          const Style& style)
{
    if (param == nullptr) {
        return;
    }
    RectParam* rectParam = static_cast<RectParam*>(param);
    uint8_t enableStroke = static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::STROKE_STYLE;
    int16_t lineWidth = enableStroke ? paint.GetStrokeWidth() : 0;
    if ((rectParam->height <= lineWidth) || (rectParam->width <= lineWidth)) {
        return;
    }
    Point start;
    GetAbsolutePosition(rectParam->start, rect, style, start);

    Rect coords;
    coords.SetPosition(start.x + (lineWidth + 1) / 2, start.y + (lineWidth + 1) / 2); // 2: half
    coords.SetHeight(rectParam->height - lineWidth);
    coords.SetWidth(rectParam->width - lineWidth);

    Style drawStyle = StyleDefault::GetDefaultStyle();
    drawStyle.bgColor_ = paint.GetFillColor();
    drawStyle.bgOpa_ = paint.GetOpacity();
    drawStyle.borderRadius_ = 0;
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, drawStyle, OPA_OPAQUE);
}

void UICanvas::DoDrawCircle(BufferInfo& gfxDstBuffer,
                            void* param,
                            const Paint& paint,
                            const Rect& rect,
                            const Rect& invalidatedArea,
                            const Style& style)
{
    if (param == nullptr) {
        return;
    }
    CircleParam* circleParam = static_cast<CircleParam*>(param);

    Style drawStyle = StyleDefault::GetDefaultStyle();
    drawStyle.lineOpa_ = paint.GetOpacity();

    ArcInfo arcInfo = {{0}};
    arcInfo.imgPos = Point{0, 0};
    arcInfo.startAngle = 0;
    arcInfo.endAngle = CIRCLE_IN_DEGREE;
    GetAbsolutePosition(circleParam->center, rect, style, arcInfo.center);
    uint8_t enableStroke = static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::STROKE_STYLE;
    uint16_t halfLineWidth = enableStroke ? (paint.GetStrokeWidth() >> 1) : 0;
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
        arcInfo.radius = circleParam->radius - halfLineWidth;
        drawStyle.lineWidth_ = arcInfo.radius;
        drawStyle.lineColor_ = paint.GetFillColor();
        BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfo, invalidatedArea, drawStyle, OPA_OPAQUE,
                                              CapType::CAP_NONE);
    }

    if (enableStroke) {
        arcInfo.radius = circleParam->radius + halfLineWidth - 1;
        drawStyle.lineWidth_ = static_cast<int16_t>(paint.GetStrokeWidth());
        drawStyle.lineColor_ = paint.GetStrokeColor();
        BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfo, invalidatedArea, drawStyle, OPA_OPAQUE,
                                              CapType::CAP_NONE);
    }
}

void UICanvas::DoDrawArc(BufferInfo& gfxDstBuffer,
                         void* param,
                         const Paint& paint,
                         const Rect& rect,
                         const Rect& invalidatedArea,
                         const Style& style)
{
    if (param == nullptr) {
        return;
    }
    ArcParam* arcParam = static_cast<ArcParam*>(param);

    ArcInfo arcInfo = {{0}};
    arcInfo.imgPos = Point{0, 0};
    arcInfo.startAngle = arcParam->startAngle;
    arcInfo.endAngle = arcParam->endAngle;
    Style drawStyle = StyleDefault::GetDefaultStyle();
    drawStyle.lineWidth_ = static_cast<int16_t>(paint.GetStrokeWidth());
    drawStyle.lineColor_ = paint.GetStrokeColor();
    drawStyle.lineOpa_ = paint.GetOpacity();
    arcInfo.radius = arcParam->radius + ((paint.GetStrokeWidth() + 1) >> 1);

    GetAbsolutePosition(arcParam->center, rect, style, arcInfo.center);
    BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfo, invalidatedArea, drawStyle, OPA_OPAQUE,
                                          CapType::CAP_NONE);
}

#if defined(GRAPHIC_ENABLE_DRAW_IMAGE_FLAG) && GRAPHIC_ENABLE_DRAW_IMAGE_FLAG
void UICanvas::DoDrawImage(BufferInfo& gfxDstBuffer,
                           void* param,
                           const Paint& paint,
                           const Rect& rect,
                           const Rect& invalidatedArea,
                           const Style& style)
{
    if (param == nullptr) {
        return;
    }

    UIImageView* imageView = static_cast<UIImageView*>(param);
    Point startPos = {imageView->GetX(), imageView->GetY()};
    Point start;
    GetAbsolutePosition({startPos.x, startPos.y}, rect, style, start);
    imageView->SetPosition(start.x, start.y);
    if (!paint.GetTransAffine().IsIdentity()) {
        float angle = paint.GetRotateAngle();
        imageView->Rotate(MATH_ROUND(angle), Vector2<float>(0, 0));
        imageView->Translate(Vector3<int16_t>(paint.GetTranslateX(), paint.GetTranslateY(), 1));
        Vector2<float> scale(static_cast<float>(paint.GetScaleX()), static_cast<float>(paint.GetScaleY()));
        imageView->Scale(scale, Vector2<float>(0, 0));
    }
    imageView->OnDraw(gfxDstBuffer, invalidatedArea);
    imageView->SetPosition(startPos.x, startPos.y);
}
#endif

void UICanvas::DoDrawLabel(BufferInfo& gfxDstBuffer,
                           void* param,
                           const Paint& paint,
                           const Rect& rect,
                           const Rect& invalidatedArea,
                           const Style& style)
{
    if (param == nullptr) {
        return;
    }
    UILabel* label = static_cast<UILabel*>(param);
    Point startPos = {label->GetX(), label->GetY()};
    Point start;
    GetAbsolutePosition({startPos.x, startPos.y}, rect, style, start);
    label->SetPosition(start.x, start.y);
    label->OnDraw(gfxDstBuffer, invalidatedArea);
    label->SetPosition(startPos.x, startPos.y);
}

void UICanvas::DoDrawLineJoin(BufferInfo& gfxDstBuffer,
                              const Point& center,
                              const Rect& invalidatedArea,
                              const Paint& paint)
{
    ArcInfo arcinfo = {{0}};
    arcinfo.center = center;
    arcinfo.imgPos = Point{0, 0};
    arcinfo.radius = (paint.GetStrokeWidth() + 1) >> 1;
    arcinfo.startAngle = 0;
    arcinfo.endAngle = CIRCLE_IN_DEGREE;

    Style style;
    style.lineColor_ = paint.GetStrokeColor();
    style.lineWidth_ = static_cast<int16_t>(paint.GetStrokeWidth());
    style.lineOpa_ = OPA_OPAQUE;
    BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcinfo, invalidatedArea, style, OPA_OPAQUE,
                                          CapType::CAP_NONE);
}

void UICanvas::DoDrawPath(BufferInfo& gfxDstBuffer,
                          void* param,
                          const Paint& paint,
                          const Rect& rect,
                          const Rect& invalidatedArea,
                          const Style& style)
{
    if (param == nullptr) {
        return;
    }
    PathParam* pathParam = static_cast<PathParam*>(param);
    const UICanvasPath* path = pathParam->path;
    if (path == nullptr) {
        return;
    }
    Point pathEnd = {COORD_MIN, COORD_MIN};

    ListNode<Point>* pointIter = path->points_.Begin();
    ListNode<ArcParam>* arcIter = path->arcParam_.Begin();
    ListNode<SWPathCmd>* iter = path->cmd_.Begin();
    for (uint16_t i = 0; (i < pathParam->count) && (iter != path->cmd_.End()); i++, iter = iter->next_) {
        switch (iter->data_) {
            case SWPathCmd::SW_CMD_MOVE_TO: {
                pointIter = pointIter->next_;
                break;
            }
            case SWPathCmd::SW_CMD_LINE_TO: {
                Point start = pointIter->prev_->data_;
                Point end = pointIter->data_;
                pointIter = pointIter->next_;
                if ((start.x == end.x) && (start.y == end.y)) {
                    break;
                }

                GetAbsolutePosition(start, rect, style, start);
                GetAbsolutePosition(end, rect, style, end);
                BaseGfxEngine::GetInstance()->DrawLine(gfxDstBuffer, start, end, invalidatedArea,
                                                       paint.GetStrokeWidth(), paint.GetStrokeColor(), OPA_OPAQUE);
                if ((pathEnd.x == start.x) && (pathEnd.y == start.y)) {
                    DoDrawLineJoin(gfxDstBuffer, start, invalidatedArea, paint);
                }
                pathEnd = end;
                break;
            }
            case SWPathCmd::SW_CMD_ARC: {
                ArcInfo arcInfo = {{0}};
                arcInfo.imgPos = Point{0, 0};
                arcInfo.startAngle = arcIter->data_.startAngle;
                arcInfo.endAngle = arcIter->data_.endAngle;
                Style drawStyle = StyleDefault::GetDefaultStyle();
                drawStyle.lineWidth_ = static_cast<int16_t>(paint.GetStrokeWidth());
                drawStyle.lineColor_ = paint.GetStrokeColor();
                drawStyle.lineOpa_ = OPA_OPAQUE;
                arcInfo.radius = arcIter->data_.radius + ((paint.GetStrokeWidth() + 1) >> 1);

                GetAbsolutePosition(arcIter->data_.center, rect, style, arcInfo.center);
                BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfo, invalidatedArea, drawStyle, OPA_OPAQUE,
                                                      CapType::CAP_NONE);
                if (pointIter != path->points_.Begin()) {
                    DoDrawLineJoin(gfxDstBuffer, pathEnd, invalidatedArea, paint);
                }

                GetAbsolutePosition(pointIter->data_, rect, style, pathEnd);
                pointIter = pointIter->next_;
                arcIter = arcIter->next_;
                break;
            }
            case SWPathCmd::SW_CMD_CLOSE: {
                Point start = pointIter->prev_->data_;
                Point end = pointIter->data_;
                GetAbsolutePosition(start, rect, style, start);
                GetAbsolutePosition(end, rect, style, end);
                if ((start.x != end.x) || (start.y != end.y)) {
                    BaseGfxEngine::GetInstance()->DrawLine(gfxDstBuffer, start, end, invalidatedArea,
                                                           paint.GetStrokeWidth(), paint.GetStrokeColor(), OPA_OPAQUE);
                    if ((pathEnd.x == start.x) && (pathEnd.y == start.y)) {
                        DoDrawLineJoin(gfxDstBuffer, start, invalidatedArea, paint);
                    }
                    pathEnd = end;
                }

                if ((pathEnd.x == end.x) && (pathEnd.y == end.y)) {
                    DoDrawLineJoin(gfxDstBuffer, end, invalidatedArea, paint);
                }
                pointIter = pointIter->next_;
                break;
            }
            default:
                break;
        }
    }
}

#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
static void ClearArcPath(List<Path*>& arcPath)
{
    ListNode<Path*>* iter = arcPath.Begin();
    for (; iter != arcPath.End(); iter = iter->next_) {
        GrallocEngines::GetInstance()->FreeBuffer(iter->data_->cmds);
        GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t*>(iter->data_->data));
        delete iter->data_;
    }
    arcPath.Clear();
}

static bool AllocBufferForPath(Path* path)
{
    AllocInfo cmdInfo;
    cmdInfo.expectedSize = sizeof(uint8_t) * path->cmdNum;
    cmdInfo.usage = HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_PATH;
    GrallocBuffer cmdBuffer;
    if (!GrallocEngines::GetInstance()->AllocBuffer(cmdInfo, cmdBuffer)) {
        return false;
    }
    path->cmds = (uint8_t *)cmdBuffer.virAddr;

    if (!HWDrawUtils::GetInstance()->AllocBufferForData(path->dataNum, path->data)) {
        GrallocEngines::GetInstance()->FreeBuffer(path->cmds);
        path->cmds = nullptr;
        return false;
    }

    return true;
}

bool UICanvas::PrepareVGUPath(const UICanvasPath* canvasPath, List<Path*>& arcPath, uint32_t& dataNum, uint32_t& cmdNum)
{
    ListNode<SWPathCmd>* swCmd = canvasPath->cmd_.Begin();
    ListNode<ArcParam>* arc = canvasPath->arcParam_.Begin();
    for (; swCmd != canvasPath->cmd_.End(); swCmd = swCmd->next_) {
        switch (swCmd->data_) {
            case SWPathCmd::SW_CMD_MOVE_TO:
                cmdNum++;
                dataNum += 2; // 2: x,y coordinate
                break;
            case SWPathCmd::SW_CMD_LINE_TO:
                cmdNum++;
                dataNum += 2; // 2: x,y coordinate
                break;
            case SWPathCmd::SW_CMD_ARC: {
                ArcInfo arcInfo = {{arc->data_.center}};
                arcInfo.startAngle = arc->data_.startAngle;
                arcInfo.endAngle = arc->data_.endAngle;
                arcInfo.radius = arc->data_.radius;
                Path* path = new Path();
                if (!HWDrawUtils::GetInstance()->ObtainArcPath(arcInfo, path->cmdNum, path->cmds,
                    path->dataNum, path->data)) {
                    delete path;
                    return false;
                }
                dataNum += path->dataNum - 2; // 2: move to point
                cmdNum += path->cmdNum - 1;
                arcPath.PushBack(path);
                arc = arc->next_;
                break;
            }
            case SWPathCmd::SW_CMD_CLOSE:
                cmdNum++;
                break;
            default:
                break;
        }
    }
    return true;
}

bool UICanvas::Convert2VGUPath(const UICanvasPath* canvasPath, Path* path)
{
    if (canvasPath->cmd_.Size() == 0 || canvasPath->points_.Size() == 0) {
        return false;
    }

    List<Path*> arcPath;
    if (!PrepareVGUPath(canvasPath, arcPath, path->dataNum, path->cmdNum)) {
        ClearArcPath(arcPath);
        return false;
    }

    if (!AllocBufferForPath(path)) {
        ClearArcPath(arcPath);
        return false;
    }

    ListNode<SWPathCmd>* swCmd = canvasPath->cmd_.Begin();
    ListNode<Point>* point = canvasPath->points_.Begin();
    ListNode<Path*>* arc = arcPath.Begin();
    for (uint16_t i = 0, j = 0; swCmd != canvasPath->cmd_.End(); swCmd = swCmd->next_) {
        switch (swCmd->data_) {
            case SWPathCmd::SW_CMD_MOVE_TO:
                path->cmds[i++] = CMD_MOVE_TO;
                path->data[j++] = static_cast<float>(point->data_.x);
                path->data[j++] = static_cast<float>(point->data_.y);
                point = point->next_;
                break;
            case SWPathCmd::SW_CMD_LINE_TO:
                path->cmds[i++] = CMD_LINE_TO;
                path->data[j++] = static_cast<float>(point->data_.x);
                path->data[j++] = static_cast<float>(point->data_.y);
                point = point->next_;
                break;
            case SWPathCmd::SW_CMD_ARC:
                memcpy_s(&path->cmds[i], arc->data_->cmdNum - 1, &(arc->data_->cmds[1]), arc->data_->cmdNum - 1);
                memcpy_s(&path->data[j], (arc->data_->dataNum - 2) * sizeof(float), // 2: move to point
                    &(arc->data_->data[2]), (arc->data_->dataNum - 2) * sizeof(float)); // 2: move to point
                i += arc->data_->cmdNum - 1;
                j += arc->data_->dataNum - 2; // 2: move to point
                arc = arc->next_;
                point = point->next_;
                break;
            case SWPathCmd::SW_CMD_CLOSE:
                path->cmds[i++] = CMD_CLOSE;
                break;
            default:
                break;
        }
    }

    ClearArcPath(arcPath);
    return true;
}

void UICanvas::DoFillPath(BufferInfo& gfxDstBuffer,
                          void* param,
                          const Paint& paint,
                          const Rect& rect,
                          const Rect& invalidatedArea,
                          const Style& style)
{
    HWPathParam* pathParam = static_cast<HWPathParam*>(param);
    if (pathParam == nullptr || pathParam->path == nullptr) {
        return;
    }

    Path* path = pathParam->path;
    if (path->cmdNum < 1) {
        GRAPHIC_LOGE("path cmdNum less than 1");
        return;
    }

    if (path->cmds[path->cmdNum - 1] != CMD_CLOSE) {
        GRAPHIC_LOGE("Last cmd must be CMD_CLOSE");
        return;
    }

    Point start;
    GetAbsolutePosition({0, 0}, rect, style, start);
    VGUMatrix3 matrix = {{1, 0, start.x, 0, 1, start.y, 0, 0, 1}};

    uintptr_t vguPath = LiteMGfxEngine::GetInstance()->InitPath(true);
    LiteMGfxEngine::GetInstance()->SetPathData(vguPath, path->cmdNum, path->cmds, path->dataNum, path->data);
    LiteMGfxEngine::GetInstance()->FillPath(gfxDstBuffer, vguPath, invalidatedArea, paint.GetFillColor(),
        paint.GetOpacity(), &matrix);
}

void UICanvas::DeleteHWPathParam(void* param)
{
    LiteMGfxEngine::GetInstance()->SyncHwDraw();

    HWPathParam* pathParam = static_cast<HWPathParam*>(param);
    Path* path = pathParam->path;
    if (path != nullptr) {
        if (path->cmds != nullptr) {
            GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t*>(path->cmds));
        }
        if (path->data != nullptr) {
            GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t*>(path->data));
        }
        delete path;
    }

    pathParam->canvasPath->strokeCount_--;
    if (pathParam->canvasPath->strokeCount_ == 0) {
        delete pathParam->canvasPath;
    }

    delete pathParam;
}
#endif

#if defined(GRAPHIC_ENABLE_DRAW_TEXT_FLAG) && GRAPHIC_ENABLE_DRAW_TEXT_FLAG
void UICanvas::StrokeText(const char* text, const Point& point, const FontStyle& fontStyle, const Paint& paint)
{
    if (text == nullptr) {
        return;
    }
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
        TextParam* textParam = new TextParam;
        if (textParam == nullptr) {
            GRAPHIC_LOGE("new TextParam fail");
            return;
        }
        textParam->text = text;
        textParam->fontStyle = fontStyle;
        textParam->fontOpa = paint.GetOpacity();
        textParam->fontColor = paint.GetFillColor();
        textParam->position = point;
        DrawCmd cmd;
        cmd.param = textParam;
        cmd.DeleteParam = DeleteTextParam;
        cmd.DrawGraphics = DoDrawText;
        cmd.paint = paint;
        drawCmdList_.PushBack(cmd);
        Invalidate();
        SetStartPosition(point);
    }
}
#endif

Point UICanvas::MeasureText(const char* text, const FontStyle& fontStyle)
{
    Text* textCompent = new Text;
    textCompent->SetText(text);
    textCompent->SetFont(fontStyle.fontName, fontStyle.fontSize);
    textCompent->SetDirect(static_cast<UITextLanguageDirect>(fontStyle.direct));
    textCompent->SetAlign(static_cast<UITextLanguageAlignment>(fontStyle.align),
        static_cast<UITextLanguageAlignment>(fontStyle.verticalAlign));
    Style drawStyle;
    drawStyle.SetStyle(STYLE_LETTER_SPACE, fontStyle.letterSpace);
    textCompent->ReMeasureTextSize(this->GetRect(), drawStyle);
    Point textSize = textCompent->GetTextSize();
    delete textCompent;
    return textSize;
}

void UICanvas::BlitMapBuffer(BufferInfo &gfxDstBuffer, BufferInfo& gfxMapBuffer, Rect& textRect,
                             TransformMap& transMap, const Rect& invalidatedArea)
{
    Rect invalidRect = textRect;
    transMap.SetTransMapRect(textRect);
    if (invalidRect.Intersect(invalidRect, transMap.GetBoxRect())) {
        uint8_t pxSize = DrawUtils::GetPxSizeByColorMode(gfxDstBuffer.mode);
        ImageInfo imageInfo;
        imageInfo.header.colorMode = gfxDstBuffer.mode;
        imageInfo.dataSize = gfxMapBuffer.width * gfxMapBuffer.height *
                DrawUtils::GetByteSizeByColorMode(gfxDstBuffer.mode);
        imageInfo.header.width = gfxMapBuffer.width;
        imageInfo.header.height = gfxMapBuffer.height;
        imageInfo.header.reserved = 0;
        uint8_t* addr = reinterpret_cast<uint8_t*>(gfxMapBuffer.virAddr);
        imageInfo.data = addr;
        TransformDataInfo imageTranDataInfo = {imageInfo.header, imageInfo.data, pxSize,
                                               BlurLevel::LEVEL0, TransformAlgorithm::BILINEAR};
        BaseGfxEngine::GetInstance()->DrawTransform(gfxDstBuffer, invalidatedArea, {0, 0}, Color::Black(),
                                                    OPA_OPAQUE, transMap, imageTranDataInfo);
    }
}

#if defined(GRAPHIC_ENABLE_DRAW_TEXT_FLAG) && GRAPHIC_ENABLE_DRAW_TEXT_FLAG
void UICanvas::DoDrawText(BufferInfo& gfxDstBuffer,
                          void* param,
                          const Paint& paint,
                          const Rect& rect,
                          const Rect& invalidatedArea,
                          const Style& style)
{
    TextParam* textParam = static_cast<TextParam*>(param);
    if (textParam == nullptr) {
        return;
    }
    if (textParam->fontStyle.fontSize <= 0) {
        return;
    }
    Text* text = textParam->textComment;
    text->SetText(textParam->text);
    text->SetFont(textParam->fontStyle.fontName, textParam->fontStyle.fontSize);
    text->SetDirect(static_cast<UITextLanguageDirect>(textParam->fontStyle.direct));
    text->SetAlign(static_cast<UITextLanguageAlignment>(textParam->fontStyle.align),
        static_cast<UITextLanguageAlignment>(textParam->fontStyle.verticalAlign));

    Point start;
    Rect textRect = invalidatedArea;
    GetAbsolutePosition(textParam->position, rect, style, start);
    textRect.SetPosition(start.x, start.y);
    Style drawStyle = style;
    drawStyle.textColor_ = textParam->fontColor;
    drawStyle.lineColor_ = textParam->fontColor;
    drawStyle.bgColor_ = textParam->fontColor;
    drawStyle.SetStyle(STYLE_LETTER_SPACE, textParam->fontStyle.letterSpace);
    text->ReMeasureTextSize(textRect, drawStyle);
    if (text->GetTextSize().x == 0 || text->GetTextSize().y == 0) {
        return;
    }
    textRect.SetWidth(text->GetTextSize().x + 1);
    textRect.SetHeight(text->GetTextSize().y + 1);
    OpacityType opa = DrawUtils::GetMixOpacity(textParam->fontOpa, style.bgOpa_);
    if (!paint.GetTransAffine().IsIdentity()) {
        Rect textImageRect(0, 0, textRect.GetWidth(), textRect.GetHeight());
        BufferInfo* mapBufferInfo = UpdateMapBufferInfo(gfxDstBuffer, textImageRect);
        text->OnDraw(*mapBufferInfo, textImageRect, textImageRect, textImageRect, 0, drawStyle,
                     Text::TEXT_ELLIPSIS_END_INV, opa);
        TransformMap trans;
        trans.SetTransMapRect(textRect);
        trans.Scale(Vector2<float>(static_cast<float>(paint.GetScaleX()), static_cast<float>(paint.GetScaleY())),
                    Vector2<float>(0, 0));
        float angle = paint.GetRotateAngle();
        trans.Rotate(MATH_ROUND(angle),  Vector2<float>(0, 0));
        trans.Translate(Vector2<int16_t>(paint.GetTranslateX(), paint.GetTranslateY()));
        BlitMapBuffer(gfxDstBuffer, *mapBufferInfo, textRect, trans, invalidatedArea);
    } else {
        text->OnDraw(gfxDstBuffer, invalidatedArea, textRect, textRect, 0,
                     drawStyle, Text::TEXT_ELLIPSIS_END_INV, opa);
    }
}
#endif
void UICanvas::InitGfxMapBuffer(const BufferInfo& srcBuff, const Rect& rect)
{
    gfxMapBuffer_ = new BufferInfo();
    gfxMapBuffer_->rect = rect;
    gfxMapBuffer_->mode = srcBuff.mode;
    gfxMapBuffer_->color = srcBuff.color;
    gfxMapBuffer_->width = static_cast<uint16_t>(rect.GetWidth());
    gfxMapBuffer_->height = static_cast<uint16_t>(rect.GetHeight());
    uint8_t destByteSize = DrawUtils::GetByteSizeByColorMode(srcBuff.mode);
    gfxMapBuffer_->stride = static_cast<int32_t>(gfxMapBuffer_->width) * static_cast<int32_t>(destByteSize);
    uint32_t buffSize = gfxMapBuffer_->height * gfxMapBuffer_->stride;
    BaseGfxEngine* baseGfxEngine = BaseGfxEngine::GetInstance();
    gfxMapBuffer_->virAddr = baseGfxEngine->AllocBuffer(buffSize, BUFFER_MAP_SURFACE);
    gfxMapBuffer_->phyAddr = gfxMapBuffer_->virAddr;

    errno_t err = memset_s(gfxMapBuffer_->virAddr, buffSize, 0, buffSize);
    if (err != EOK) {
        baseGfxEngine->FreeBuffer(static_cast<uint8_t*>(gfxMapBuffer_->virAddr), BUFFER_MAP_SURFACE);
        GRAPHIC_LOGE("memset_s gfxMapBuffer_ fail");
        return;
    }
}

BufferInfo* UICanvas::UpdateMapBufferInfo(const BufferInfo& srcBuff, const Rect& rect)
{
    if (gfxMapBuffer_ == nullptr) {
        InitGfxMapBuffer(srcBuff, rect);
        return gfxMapBuffer_;
    }

    if (rect.GetWidth() != gfxMapBuffer_->width ||
        rect.GetHeight() != gfxMapBuffer_->height ||
        srcBuff.mode != gfxMapBuffer_->mode) {
        DestroyMapBufferInfo();
        InitGfxMapBuffer(srcBuff, rect);
    } else {
        uint32_t buffSize = gfxMapBuffer_->height * gfxMapBuffer_->stride;
        errno_t err = memset_s(gfxMapBuffer_->virAddr, buffSize, 0, buffSize);
        if (err != EOK) {
            BaseGfxEngine::GetInstance()->FreeBuffer(static_cast<uint8_t*>(gfxMapBuffer_->virAddr), BUFFER_MAP_SURFACE);
            GRAPHIC_LOGE("memset_s gfxMapBuffer_ fail");
        }
    }
    return gfxMapBuffer_;
}

void UICanvas::DestroyMapBufferInfo()
{
    if (gfxMapBuffer_ != nullptr) {
        BaseGfxEngine::GetInstance()->FreeBuffer(static_cast<uint8_t*>(gfxMapBuffer_->virAddr), BUFFER_MAP_SURFACE);
        gfxMapBuffer_->virAddr = nullptr;
        gfxMapBuffer_->phyAddr = nullptr;
        delete gfxMapBuffer_;
        gfxMapBuffer_ = nullptr;
    }
}

#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
void UICanvas::AddHwDrawCmdToList(CanvasDrawParam* param)
{
    matrices_.PushBack(Matrix3<float>());

    DrawCmd cmd;
    param->index = matrices_.Size() - 1;
    cmd.param = param;
    cmd.DeleteParam = CanvasHwDrawer::DeleteParam;
    cmd.DrawGraphics = CanvasHwDrawer::DoDraw;
    drawCmdList_.PushBack(cmd);

    Invalidate();
}
#endif
} // namespace OHOS

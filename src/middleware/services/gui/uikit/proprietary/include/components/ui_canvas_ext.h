/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd. All rights reserved
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

/**
 * @file ui_canvas_ext.h
 *
 * @brief Defines the class to extend canvas/paint ability.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_CANVAS_EXT_H
#define GRAPHIC_LITE_UI_CANVAS_EXT_H

#include "securec.h"
#include "components/ui_canvas.h"
#include "components/svg_data.h"
#include "gfx_utils/graphic_types.h"
#include "gfx_utils/list.h"
#include "gfx_utils/vector.h"
#include "securec.h"
#include "common/graphic_hardware_types.h"
#include "common/paint_ext.h"
#include "gfx_utils/diagram/common/paint.h"

namespace OHOS {
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
/**
 * @brief Class to edtend UILabel ability.
 *
 * @since 1.0
 * @version 1.0
 */
class UICanvasExt : public UICanvas {
public:
    /**
     * @brief A constructor used to create an <b>AbstractAdapter</b> instance.
     *
     */
    UICanvasExt();

    /**
     * @brief A destructor used to delete the <b>UICanvasExt</b> instance.
     *
     */
    ~UICanvasExt() override;

    /**
     * @brief Get current view type.
     *
     */
    UIViewType GetViewType() const override
    {
        return UI_CANVAS_EXT;
    }

    /**
     * @brief Begin a path.
     *        This method would init a new path in the context.
     * @param paint Indicates the path style of the newly-created path in the context.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t BeginPath(const PaintExt &paint);

    /**
     * @brief Set path data to the current path in the context.
     */
    void SetPathData(uint32_t cmdNum, const uint8_t* cmd, uint32_t dataNum, const float* data);

    /**
     * @brief Clear all paths, paints, and matrices, and invalidate the view.
     */
    void Clear();

    /**
     * @brief Set transformation matrix of a path object.
     * @param matrix Indicates a transformation matrix.
     * @param index Indicates a value that relates a matrix to a path object.
     *        This is the returned value of BeginPath or any DrawGraphics method(DrawLine, DrawCurve, etc.).
     *        Default value is -1, which relates to the last created path object.
     */
    void SetMatrix(Matrix3<float> matrix, int16_t index = -1);

    /**
     * @brief Set transformation matrix of a svg object.
     * @param matrix Indicates a transformation matrix.
     * @param index Indicates a value that relates a matrix to a svg object.
     *        This is the returned value of DrawSvg.
     */
    void SetSvgMatrix(Matrix3<float> matrix, int16_t index);

    /**
     * @brief Set drawing flag to true.
     */
    void DrawPath();

    /**
     * @brief Draws svg data.
     * @param path svg bin path.
     * @param path len
     */
    int16_t DrawSvg(const char* path);

    /**
     * @brief Draws a straight line from the coordinates of the start point.
     *
     * @param startPoint Indicates the coordinates of the start point.
     * @param endPoint   Indicates the coordinates of the end point.
     * @param paint      Indicates the straight line style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawLine(const Point& startPoint, const Point& endPoint, const PaintExt& paint);

    /**
     * @brief Draws a polyline described by points.
     *
     * @param points     Indicates the waypoints of the polyline.
     * @param pointNum   Indicates the number of waypoints.
     * @param paint      Indicates the polyline style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawPolyline(const Point *points, uint32_t pointNum, const PaintExt& paint);

    /**
     * @brief Draws a polygon described by points.
     *
     * @param points     Indicates the waypoints of the polygon.
     * @param pointNum   Indicates the number of waypoints.
     * @param paint      Indicates the polygon style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawPolygon(const Point *points, uint32_t pointNum, const PaintExt& paint);

    /**
     * @brief Draws a cubic Bezier curve from the start point coordinates.
     *
     * @param startPoint Indicates the coordinates of the start point of the cubic Bezier curve.
     * @param control1   Indicates the coordinates of the first control point of the cubic Bezier curve.
     * @param control2   Indicates the coordinates of the second control point of the cubic Bezier curve.
     * @param endPoint   Indicates the coordinates of the end point of the cubic Bezier curve.
     * @param paint      Indicates the curve style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawCurve(const Point& startPoint, const Point& control1, const Point& control2,
        const Point& endPoint, const PaintExt& paint);

    /**
     * @brief Draws a rectangle.
     *
     * @param startPoint Indicates the coordinates of the point at the upper left corner of the rectangle.
     * @param height     Indicates the height of the rectangle.
     * @param width      Indicates the width of the rectangle.
     * @param paint      Indicates the rectangle style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawRect(const Point& startPoint, int16_t height, int16_t width, const PaintExt& paint,
        uint16_t radius = 0);

   /**
     * @brief Draws a circle.
     *
     * @param center Indicates the coordinates of the circle center.
     * @param radius Indicates the radius of the circle.
     * @param paint  Indicates the circle style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawCircle(const Point& center, uint16_t radius, const PaintExt& paint);

    /**
     * @brief Draws a sector.
     *
     * When the start angle is smaller than the end angle, the sector is drawn clockwise.
     * Otherwise, the sector is drawn counterclockwise.
     *
     * @param center     Indicates the coordinates of the sector's center.
     * @param radius     Indicates the radius of the sector.
     * @param startAngle Indicates the start angle of the sector. Value <b>0</b> indicates the 12-o'clock direction,
     *                   and <b>90</b> indicates the 3-o'clock direction.
     * @param endAngle   Indicates the end angle of the sector. Value <b>0</b> indicates the 12-o'clock direction,
     *                   and <b>90</b> indicates the 3-o'clock direction.
     * @param paint      Indicates the sector style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawSector(const Point& center, uint16_t radius, float startAngle, float endAngle, const PaintExt& paint);

    /**
     * @brief Draws an arc.
     *
     * When the start angle is smaller than the end angle, the sector is drawn clockwise.
     * Otherwise, the sector is drawn counterclockwise.
     *
     * @param center     Indicates the coordinates of the arc's center.
     * @param radius     Indicates the radius of the arc.
     * @param startAngle Indicates the start angle of the arc. Value <b>0</b> indicates the 12-o'clock direction,
     *                   and <b>90</b> indicates the 3-o'clock direction.
     * @param endAngle   Indicates the end angle of the arc. Value <b>0</b> indicates the 12-o'clock direction,
     *                   and <b>90</b> indicates the 3-o'clock direction.
     * @param paint      Indicates the arc style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawArc(const Point& center, uint16_t radius, float startAngle, float endAngle, const PaintExt& paint);

    /**
     * @brief Draws an image.
     *
     * @param startPoint   Indicates the coordinates of the point at the upper left corner of the image.
     * @param image        Indicates the image bin path.
     * @param paint        Indicates the image style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawImage(const Point& startPoint, const char* image, const PaintExt& paint);

    /**
     * @brief Draws an image.
     *
     * @param startPoint   Indicates the coordinates of the point at the upper left corner of the image.
     * @param image        Indicates the info of the image.
     * @param paint        Indicates the image style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawImage(const Point& startPoint, ImageInfo *image, const PaintExt& paint);

    /**
     * @brief Draws text.
     *
     * @param startPoint Indicates the coordinates of the start point.
     * @param text       Indicates the pointer to the text content.
     * @param maxWidth   Indicates the maximum width of the text that can be displayed. If the maximum width is
     *                   exceeded, the text is truncated.
     * @param fontStyle  Indicates the text layout and font style. For details, see {@link FontStyle}.
     * @param paint      Indicates the text style. For details, see {@link Paint}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawLabel(const Point& startPoint, const char* text, uint16_t maxWidth, const FontStyle& fontStyle,
        const PaintExt& paint);

    /**
     * @brief Draws text.
     *
     * @param rect       Indicates the area to draw the text.
     * @param text       Indicates the pointer to the text content.
     * @param fontStyle  Indicates the text layout and font style. For details, see {@link FontStyle}.
     * @param paint      Indicates the text style. For details, see {@link Paint}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawLabel(const Rect& rect, const char* text, const FontStyle& fontStyle, const PaintExt& paint);

    /**
     * @brief Draws an bitmap.
     *
     * @param bitmap        Indicates the buffer info of the bitmap.
     * @param paint        Indicates the image style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawBitmap(const BufferInfo *bitmap, const PaintExt& paint);

    /**
     * @brief Draws a render Cache .
     *
     * @param cacheKey     Indicates the key of the render cache.
     * @param paint        Indicates the render cache paint style. For details, see {@link PaintExt}.
     * @return Returns index value that relates to an object. Returns -1 when any error occurs.
     */
    int16_t DrawRenderCache(uint32_t cacheKey, const PaintExt& paint);

private:
    Path* curPath_;
    Graphic::Vector<SvgData*> svgObjList_;
    Graphic::Vector<SvgPath> svgPaths_;
    bool isPathDrawable_;

    void ClearSvgData();
    SvgData* GetSvgData(const char* path);
    void PaintSetGradient(SvgData* svg, int16_t gradientIndex, PaintExt& paint, uint16_t index);
    void PaintSetClipPath(SvgData* svg, int16_t clipPathIndex, PaintExt& paint);
    ColorStop* OverlayOpacity(SvgData* svg, uint16_t stopIndex, uint16_t stopCount, uint16_t index);
    void PaintSetParam(SvgData* svg, PaintExt& paint, uint16_t index);
};
#endif
}
#endif
/**
 * @}
 */

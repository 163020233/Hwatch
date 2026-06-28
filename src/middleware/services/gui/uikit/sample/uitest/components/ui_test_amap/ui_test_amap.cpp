/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: UIAMapView
 * Author:
 * Create: 2026-01
 */

#include "ui_test_amap.h"
#include <algorithm>
#include "common/image_cache_manager.h"
#include "common/screen.h"
#include "components/root_view.h"
#include "font/ui_font.h"
#include "hals/gralloc_engines.h"
#include "animator/animator.h"
#include "hal_tick.h"

namespace OHOS {
static constexpr int16_t MAP_WIDTH = 512;
static constexpr int16_t MAP_HEIGHT = 256;
static constexpr int16_t MAP_GAP = 50;
static constexpr int16_t MAP_FONT_SIZE = 15;
static constexpr int16_t TILE_WIDTH = 256;
static constexpr int16_t TILE_HEIGHT = 128;
static constexpr int16_t BIT_MAP_WIDTH = 24;
static constexpr int16_t BIT_MAP_HEIGHT = 24;
static constexpr int16_t DIVISOR_TWO = 2;
static constexpr int16_t DIVISOR_THREE = 3;
static constexpr int16_t LABEL_BTN_WIDTH = 80;
static constexpr int16_t LABEL_BTN_HEIGHT = 40;
static constexpr int16_t LABEL_FONT_SIZE = 18;
static constexpr float MAP_ROTATE = 45.0;
static constexpr uint8_t SVG_MAX_NUM = 4;
const char* g_svgPath[SVG_MAX_NUM][SVG_MAX_NUM] = {
    {RES_PATH"842_284_10.fb", RES_PATH"842_285_10.fb", RES_PATH"843_284_10.fb", RES_PATH"843_285_10.fb"},
    {RES_PATH"3429_1341_12.fb", RES_PATH"3427_1338_12.fb", RES_PATH"3427_1339_12.fb", RES_PATH"3428_1339_12.fb"},
    {RES_PATH"3427_1340_12.fb", RES_PATH"3427_1341_12.fb", RES_PATH"3428_1338_12.fb", RES_PATH"3428_1340_12.fb"},
    {RES_PATH"3428_1341_12.fb", RES_PATH"3429_1338_12.fb", RES_PATH"3429_1339_12.fb", RES_PATH"3429_1340_12.fb"}
};

const char* g_imgPath[SVG_MAX_NUM][SVG_MAX_NUM] = {
    {RES_PATH"842_284_10.JPG", RES_PATH"842_285_10.JPG", RES_PATH"843_284_10.JPG", RES_PATH"843_285_10.JPG"},
    {RES_PATH"3429_1341_12.JPG", RES_PATH"3427_1338_12.JPG", RES_PATH"3427_1339_12.JPG", RES_PATH"3428_1339_12.JPG"},
    {RES_PATH"3427_1340_12.JPG", RES_PATH"3427_1341_12.JPG", RES_PATH"3428_1338_12.JPG", RES_PATH"3428_1340_12.JPG"},
    {RES_PATH"3428_1341_12.JPG", RES_PATH"3429_1338_12.JPG", RES_PATH"3429_1339_12.JPG", RES_PATH"3429_1340_12.JPG"}
};
uint32_t* g_bitmap = nullptr;

UITestAMap::~UITestAMap()
{
    TearDown();
}

void UITestAMap::SetUp()
{
    UIFont::GetInstance()->RegisterFontInfo(DEFAULT_VECTOR_FONT_FILENAME);
    if (g_bitmap == nullptr) {
        g_bitmap = (uint32_t*)GrallocEngines::GetInstance()->AllocBufferBySize(sizeof(uint32_t) *
            BIT_MAP_HEIGHT * BIT_MAP_WIDTH);
        if (g_bitmap == nullptr) {
            GRAPHIC_LOGE("ImageCacheMalloc bitmap failed.");
            return;
        }
        for (uint32_t w = 0; w < BIT_MAP_WIDTH; w++) {
            for (uint32_t h = 0; h < BIT_MAP_HEIGHT; h++) {
                g_bitmap[w * BIT_MAP_HEIGHT + h] = Color::Blue().full;
            }
        }
    }

    if (container_ == nullptr) {
        container_ = new(std::nothrow) UIScrollView();
        if (container_ == nullptr) {
            return;
        }
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        container_->SetScrollBlankSize(200); // 200: blank size
    }

    SetUpAmap();
    SetUpControlBtn();
}

void UITestAMap::SetUpAmap()
{
    if (amapSvg_ == nullptr) {
        amapSvg_ = new(std::nothrow) UIAMapView();
        if (amapSvg_ == nullptr) {
            return;
        }
        amapSvg_->SetPosition(0, 0, MAP_WIDTH, MAP_HEIGHT);
        amapSvg_->SetTouchable(false);
        container_->Add(amapSvg_);
    }

    if (amapImage_ == nullptr) {
        amapImage_ = new(std::nothrow) UIAMapView();
        if (amapImage_ == nullptr) {
            return;
        }
        amapImage_->SetPosition(0, MAP_HEIGHT + MAP_GAP, MAP_WIDTH, MAP_HEIGHT);
        amapImage_->SetTouchable(false);
        container_->Add(amapImage_);
    }
}

void UITestAMap::SetUpControlBtn()
{
    if (animator_ == nullptr) {
        animator_ = new(std::nothrow) AmapAnimator(this);
        if (animator_ != nullptr && startAniBtn_ == nullptr) {
            startAniBtn_ = new(std::nothrow) UILabelButton();
            if (startAniBtn_ != nullptr) {
                startAniBtn_->SetText("Animator");
                startAniBtn_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT_SIZE);
                startAniBtn_->SetPosition(0, MAP_HEIGHT, LABEL_BTN_WIDTH, LABEL_BTN_HEIGHT);
                startAniBtn_->SetViewIndex(static_cast<int16_t>(ControlIndex::START_TILE_MAP_ANIMAT));
                startAniBtn_->SetOnClickListener(this);
                container_->Add(startAniBtn_);
            }

            startAniBtn2_ = new(std::nothrow) UILabelButton();
            if (startAniBtn2_ != nullptr) {
                startAniBtn2_->SetText("Animator");
                startAniBtn2_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT_SIZE);
                startAniBtn2_->SetPosition(LABEL_BTN_WIDTH + MAP_GAP, MAP_HEIGHT,
                    LABEL_BTN_WIDTH, LABEL_BTN_HEIGHT);
                startAniBtn2_->SetViewIndex(static_cast<int16_t>(ControlIndex::START_TILE_MAP_ANIMAT_NO_CHANGE));
                startAniBtn2_->SetOnClickListener(this);
                container_->Add(startAniBtn2_);
            }
        }
    }

    if (changeSvgBtn_ == nullptr) {
        changeSvgBtn_ = new(std::nothrow) UILabelButton();
        if (changeSvgBtn_ != nullptr) {
            changeSvgBtn_->SetText("ChangeMap");
            changeSvgBtn_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT_SIZE);
            changeSvgBtn_->SetPosition(2 * (LABEL_BTN_WIDTH + MAP_GAP), MAP_HEIGHT, // 2: x
                LABEL_BTN_WIDTH, LABEL_BTN_HEIGHT);
            changeSvgBtn_->SetViewIndex(static_cast<int16_t>(ControlIndex::CHANGE_TILE_MAP));
            changeSvgBtn_->SetOnClickListener(this);
            container_->Add(changeSvgBtn_);
        }
    }
}

void UITestAMap::TearDown()
{
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }

    if (amapSvg_ != nullptr) {
        delete amapSvg_;
        amapSvg_ = nullptr;
    }
    if (amapImage_ != nullptr) {
        delete amapImage_;
        amapImage_ = nullptr;
    }

    if (g_bitmap != nullptr) {
        GrallocEngines::GetInstance()->FreeBuffer((uint8_t*)g_bitmap);
        g_bitmap = nullptr;
    }

    if (animator_ != nullptr) {
        if (animator_->GetState() == Animator::START) {
            animator_->Stop();
        }
        delete animator_;
        animator_ = nullptr;
    }

    if (startAniBtn_ != nullptr) {
        delete startAniBtn_;
        startAniBtn_ = nullptr;
    }

    if (startAniBtn2_ != nullptr) {
        delete startAniBtn2_;
        startAniBtn2_ = nullptr;
    }

    if (changeSvgBtn_ != nullptr) {
        delete changeSvgBtn_;
        changeSvgBtn_ = nullptr;
    }
    return;
}

void UITestAMap::AMapTestDrawSvg(UIAMapView *amap, const char *svgFile, Rect rect)
{
    if (container_ == nullptr || amap == nullptr) {
        return;
    }

    FILE* fp = fopen(svgFile, "rb");
    if (fp == nullptr) {
        GRAPHIC_LOGE("fopen %s failed.", svgFile);
        return;
    }
    fseek(fp, 0, SEEK_END);
    uint32_t  fileLen = static_cast<uint32_t>(ftell(fp));
    uint8_t* fileData = GrallocEngines::GetInstance()->AllocBufferBySize(fileLen,
        HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_JPEG_DATA);
    if (fileData == nullptr) {
        fclose(fp);
        GRAPHIC_LOGE("Alloc buffer for data failed.");
        return;
    }
    fseek(fp, 0, SEEK_SET);
    if (fread(fileData, 1, fileLen, fp) != fileLen) {
        fclose(fp);
        GrallocEngines::GetInstance()->FreeBuffer(fileData);
        GRAPHIC_LOGE("Read data failed.");
        return;
    }
    fclose(fp);
    IMapRender::SvgBufferInfo svg;
    svg.buffer = fileData;
    svg.bufferSize = fileLen;
    svg.svgType = IMapRender::SvgType::SVG_TYPE_SERIALIZED;
    svg.uuid = (char *)svgFile;
    amap->DrawSvg(&svg, rect, MAP_ROTATE);

    GrallocEngines::GetInstance()->FreeBuffer(fileData);
}

void UITestAMap::AMapTestDrawImage(UIAMapView *amap, const char *jpegFile, Rect rect)
{
    if (container_ == nullptr || amap == nullptr) {
        return;
    }

    FILE* fp = fopen(jpegFile, "rb");
    if (fp == nullptr) {
        GRAPHIC_LOGE("fopen %s failed.", jpegFile);
        return;
    }
    fseek(fp, 0, SEEK_END);
    uint32_t  fileLen = static_cast<uint32_t>(ftell(fp));
    uint8_t* fileData = GrallocEngines::GetInstance()->AllocBufferBySize(fileLen,
        HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_JPEG_DATA);
    if (fileData == nullptr) {
        fclose(fp);
        GRAPHIC_LOGE("Alloc buffer for data failed.");
        return;
    }
    fseek(fp, 0, SEEK_SET);
    if (fread(fileData, 1, fileLen, fp) != fileLen) {
        fclose(fp);
        GrallocEngines::GetInstance()->FreeBuffer(fileData);
        GRAPHIC_LOGE("Read data failed.");
        return;
    }
    fclose(fp);
    IMapRender::ImageBufferInfo imageBuffer;
    imageBuffer.buffer = fileData;
    imageBuffer.bufferSize = fileLen;
    imageBuffer.imageType = IMapRender::ImageType::IMG_TYPE_JPEG;
    imageBuffer.uuid = (char *)jpegFile;

    amap->DrawImage(&imageBuffer, rect, MAP_ROTATE);
    GrallocEngines::GetInstance()->FreeBuffer(fileData);
}

void UITestAMap::AMapTestDrawPoints(UIAMapView *amap)
{
    if (amap != nullptr) {
        IMapRender::PointStyle pointStyle = {0};
        pointStyle.color = Color::Maroon();
        pointStyle.radius = 30; // 30: radius
        Point points[4] = { // 4: index
            {TILE_WIDTH, 0},
            {MAP_WIDTH, TILE_HEIGHT},
            {TILE_WIDTH, MAP_HEIGHT},
            {0, TILE_HEIGHT}
        };
        amap->DrawPoints(points, 4, &pointStyle); // 4: index
    }
}

void UITestAMap::AMapTestDrawPolyline(UIAMapView *amap)
{
    if (amap != nullptr) {
        IMapRender::StrokeStyle strokeStyle = {0};
        strokeStyle.strokeColor = Color::Green();
        strokeStyle.strokeWidth = 10; // 10: line width
        Point points[3] = {{0, 0}, // 3: index
            {TILE_WIDTH / DIVISOR_TWO, TILE_HEIGHT + TILE_HEIGHT / DIVISOR_TWO}, {MAP_WIDTH, MAP_HEIGHT}};
        amap->DrawPolyline(points, 3, &strokeStyle); // 3: index

        strokeStyle.strokeColor = Color::Cyan();
        Point points1[3] = {{0, 0}, // 3: index
            {TILE_WIDTH + TILE_WIDTH / DIVISOR_TWO, TILE_HEIGHT / DIVISOR_TWO}, {MAP_WIDTH, MAP_HEIGHT}};
        amap->DrawPolyline(points1, 3, &strokeStyle); // 3: index
    }
}

void UITestAMap::AMapTestDrawPolygon(UIAMapView *amap)
{
    if (amap != nullptr) {
        IMapRender::MapPaintStyle paintStyle = {0};
        paintStyle.paintType = Paint::PaintStyle::STROKE_STYLE;
        paintStyle.strokeStyle.strokeColor = Color::Magenta();
        paintStyle.strokeStyle.strokeWidth = 10; // 10: line width
        Point points[4] = { // 4: index;
            {TILE_WIDTH / DIVISOR_TWO, 0},
            {MAP_WIDTH, TILE_HEIGHT / DIVISOR_TWO},
            {TILE_WIDTH + TILE_WIDTH / DIVISOR_TWO, MAP_HEIGHT},
            {0, TILE_HEIGHT + TILE_HEIGHT / DIVISOR_TWO}
        };
        amap->DrawPolygon(points, 4, &paintStyle); // 4: index

        paintStyle.strokeStyle.strokeColor = Color::Purple();
        Point points1[4] = { // 4: index
            {TILE_WIDTH + TILE_WIDTH / DIVISOR_TWO, 0},
            {MAP_WIDTH, TILE_HEIGHT + TILE_HEIGHT / DIVISOR_TWO},
            {TILE_WIDTH / DIVISOR_TWO, MAP_HEIGHT},
            {0, TILE_HEIGHT / DIVISOR_TWO}
        };
        amap->DrawPolygon(points1, 4, &paintStyle); // 4: index
    }
}

void UITestAMap::AMapTestDrawText(UIAMapView *amap)
{
    if (amap != nullptr) {
        IMapRender::TextStyle textStyle = {0};
        textStyle.color = Color::Olive();
        textStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
        textStyle.fontSize = MAP_FONT_SIZE;
        amap->DrawText({10, 30}, "北海公园", &textStyle, MAP_ROTATE); // 10, 30: x, y
        amap->DrawText({360, 90}, "南锣鼓巷", &textStyle, MAP_ROTATE); // 360, 90: x, y
        amap->DrawText({90, 190}, "画舫斋", &textStyle, MAP_ROTATE); // 90, 190: x, y
        amap->DrawText({95, 120}, "崇椒室", &textStyle, MAP_ROTATE); // 95, 120: x, y
        amap->DrawText({50, 253}, "北海公园", &textStyle, MAP_ROTATE); // 50, 253: x, y
        textStyle.color = Color::Blue();
        amap->DrawText({400, 230}, "北海", &textStyle, MAP_ROTATE); // 400, 230: x, y
        textStyle.color = Color::Aqua();
        amap->DrawText({240, 230}, "东城", &textStyle, MAP_ROTATE); // 240, 230: x, y
    }
}

void UITestAMap::AMapTestMeasureText(UIAMapView *amap)
{
    if (amap != nullptr) {
        IMapRender::TextStyle textStyle = {0};
        textStyle.fontName = DEFAULT_VECTOR_FONT_FILENAME;
        textStyle.fontSize = MAP_FONT_SIZE;
        int32_t width = 0;
        int32_t height = 0;
        bool ret = amap->MeasureText("北海公园", &textStyle, &width, &height);
        if (ret) {
            char text[32] = {0}; // 32: text length
            sprintf_s(text, sizeof(text), "%d_%d", width, height);
            textStyle.color = Color::Red();
            amap->DrawText({static_cast<int16_t>(Screen::GetInstance().GetWidth() / DIVISOR_TWO),
                static_cast<int16_t>(Screen::GetInstance().GetHeight() / DIVISOR_THREE)},
                text, &textStyle, MAP_ROTATE);
        }
    }
}

void UITestAMap::AMapTestDrawBitmap(UIAMapView *amap)
{
    if (amap != nullptr) {
        IMapRender::BitmapBufferInfo bitmap = {ColorMode::ARGB8888};
        bitmap.buffer = (uint8_t *)g_bitmap;
        bitmap.bufferSize = BIT_MAP_WIDTH * BIT_MAP_HEIGHT * 4; // 4: bytes per pixel
        bitmap.width = BIT_MAP_WIDTH;
        bitmap.height = BIT_MAP_HEIGHT;
        bitmap.stride = BIT_MAP_WIDTH * 4; // 4: bytes per pixel
        amap->DrawBitmap(&bitmap,
            {static_cast<int16_t>(MAP_WIDTH / DIVISOR_TWO - BIT_MAP_WIDTH),
            static_cast<int16_t>(MAP_HEIGHT / DIVISOR_TWO - BIT_MAP_HEIGHT),
            static_cast<int16_t>(MAP_WIDTH / DIVISOR_TWO - 1),
            static_cast<int16_t>(MAP_HEIGHT / DIVISOR_TWO - 1)}, MAP_ROTATE);

        IMapRender::BitmapBufferInfo bitmapBorder = {ColorMode::ARGB8888};
        bitmapBorder.buffer = (uint8_t *)g_bitmap;
        bitmapBorder.bufferSize = BIT_MAP_WIDTH * BIT_MAP_HEIGHT * 4; // 4: bytes per pixel
        bitmapBorder.width = BIT_MAP_WIDTH;
        bitmapBorder.height = BIT_MAP_HEIGHT;
        bitmapBorder.stride = BIT_MAP_WIDTH * 4; // 4: bytes per pixel
        amap->DrawBitmap(&bitmapBorder,
            {static_cast<int16_t>(MAP_WIDTH / DIVISOR_TWO - BIT_MAP_WIDTH / DIVISOR_TWO),
            static_cast<int16_t>(MAP_HEIGHT - BIT_MAP_HEIGHT / DIVISOR_TWO),
            static_cast<int16_t>(MAP_WIDTH / DIVISOR_TWO + BIT_MAP_WIDTH / DIVISOR_TWO),
            static_cast<int16_t>(MAP_HEIGHT + BIT_MAP_HEIGHT / DIVISOR_TWO)}, MAP_ROTATE);
    }
}

void UITestAMap::AMapTestDrawColor(UIAMapView *amap)
{
    if (amap != nullptr) {
        IMapRender::MapPaintStyle paintStyle = {0};
        paintStyle.fillColor = Color::Yellow();
        paintStyle.paintType = Paint::PaintStyle::FILL_STYLE;
        amap->DrawRect({MAP_WIDTH / DIVISOR_TWO, MAP_HEIGHT / DIVISOR_TWO, MAP_WIDTH / DIVISOR_TWO + BIT_MAP_WIDTH - 1,
            MAP_HEIGHT / DIVISOR_TWO + BIT_MAP_HEIGHT - 1}, &paintStyle, MAP_ROTATE);
    }
}

const UIView* UITestAMap::GetTestView()
{
    ShowAmapByIndex(curIndex_);
    return container_;
}

void UITestAMap::ShowAmapByIndex(int16_t index)
{
    if (amapSvg_ != nullptr) {
        amapSvg_->BeginDraw(MAP_WIDTH, MAP_HEIGHT, {0, 0, MAP_WIDTH, MAP_HEIGHT});
        AMapTestDrawSvg(amapSvg_, g_svgPath[index][0],
            {0, 0, TILE_WIDTH - 1, TILE_HEIGHT - 1});
        AMapTestDrawSvg(amapSvg_, g_svgPath[index][1],
            {TILE_WIDTH, 0, TILE_WIDTH * DIVISOR_TWO - 1, TILE_HEIGHT - 1});
        AMapTestDrawSvg(amapSvg_, g_svgPath[index][2],  // 2：index
            {0, TILE_HEIGHT, TILE_WIDTH - 1, TILE_HEIGHT * DIVISOR_TWO - 1});
        AMapTestDrawSvg(amapSvg_, g_svgPath[index][3],  // 3：index
            {TILE_WIDTH, TILE_HEIGHT, TILE_WIDTH * DIVISOR_TWO - 1, TILE_HEIGHT * DIVISOR_TWO - 1});
        AMapTestDrawPoints(amapSvg_);
        AMapTestDrawPolyline(amapSvg_);
        AMapTestDrawPolygon(amapSvg_);
        AMapTestDrawText(amapSvg_);
        AMapTestMeasureText(amapSvg_);
        AMapTestDrawBitmap(amapSvg_);
        AMapTestDrawColor(amapSvg_);
        amapSvg_->CommitDraw();
    }

    if (amapImage_ != nullptr) {
        amapImage_->BeginDraw(MAP_WIDTH, MAP_HEIGHT, {0, 0, MAP_WIDTH, MAP_HEIGHT});
        AMapTestDrawImage(amapImage_, g_imgPath[index][0],
            {0, 0, TILE_WIDTH - 1, TILE_HEIGHT - 1});
        AMapTestDrawImage(amapImage_, g_imgPath[index][1],
            {TILE_WIDTH, 0, TILE_WIDTH * DIVISOR_TWO - 1, TILE_HEIGHT - 1});
        AMapTestDrawImage(amapImage_, g_imgPath[index][2], // 2：index
            {0, TILE_HEIGHT, TILE_WIDTH - 1, TILE_HEIGHT * DIVISOR_TWO - 1});
        AMapTestDrawImage(amapImage_, g_imgPath[index][3], // 3：index
            {TILE_WIDTH, TILE_HEIGHT, TILE_WIDTH * DIVISOR_TWO - 1, TILE_HEIGHT * DIVISOR_TWO - 1});
        AMapTestDrawPoints(amapImage_);
        AMapTestDrawPolyline(amapImage_);
        AMapTestDrawPolygon(amapImage_);
        AMapTestDrawText(amapImage_);
        AMapTestMeasureText(amapImage_);
        AMapTestDrawBitmap(amapImage_);
        AMapTestDrawColor(amapImage_);
        amapImage_->CommitDraw();
    }
}

void UITestAMap::AmapAnimator::Callback(UIView* view)
{
    if (HALTick::GetInstance().GetElapseTime(lastTime_) < 1000) { // 1000: 1s
        return;
    }

    lastTime_ = HALTick::GetInstance().GetTime();
    if (testAmap_ != nullptr) {
        if (isChangeMapRes_) {
            testAmap_->curIndex_ =
                (testAmap_->curIndex_ + 1) < SVG_MAX_NUM ? testAmap_->curIndex_ + 1 : 0;
        }
        testAmap_->ShowAmapByIndex(testAmap_->curIndex_);
    }
    return;
}

bool UITestAMap::OnClick(UIView& view, const ClickEvent& event)
{
    if (view.GetViewType() != UIViewType::UI_LABEL_BUTTON) {
        return false;
    }

    int16_t index = static_cast<UILabelButton*>(&view)->GetViewIndex();
    if (index == static_cast<int16_t>(ControlIndex::CHANGE_TILE_MAP)) {
        curIndex_ = (curIndex_ + 1) < SVG_MAX_NUM ? curIndex_ + 1 : 0;
        ShowAmapByIndex(curIndex_);
    } else if (index == static_cast<int16_t>(ControlIndex::START_TILE_MAP_ANIMAT) ||
        index == static_cast<int16_t>(ControlIndex::START_TILE_MAP_ANIMAT_NO_CHANGE)) {
        if (animator_ == nullptr) {
            return false;
        }

        if (animator_->GetState() == Animator::START) {
            animator_->Stop();
        } else {
            if (index == static_cast<int16_t>(ControlIndex::START_TILE_MAP_ANIMAT)) {
                animator_->SetChangeMapRes(true);
            } else {
                animator_->SetChangeMapRes(false);
            }
            animator_->Start();
        }
    }
    return true;
}
}

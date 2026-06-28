/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd..
 * Description: simulator display driver implements
 */

#include "monitor.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <windows.h>
#include <cstdlib>
#include <cstdbool>
#include <cstdint>
#include <mutex>
#include "SDL.h"
#include "mouse_input.h"
#include "securec.h"
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/graphic_types.h"
#include "gfx_utils/color.h"
#include "graphic_service.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "dock/screen_device_proxy.h"
#include "components/root_view.h"
#include "components/ui_label.h"

using namespace std;
using namespace OHOS;

static SDL_Window *g_sdlWindow;
static SDL_Renderer *g_sdlRenderer;
static SDL_Texture *g_sdlTexture;
bool g_sdlInited = false;
bool g_sdlQuitQry = false;
static uint32_t g_contentHeight = 0;
static uint32_t g_contentWidth = 0;
static uint32_t g_monitorHeight = 0;
static uint32_t g_monitorWidth = 0;
static int16_t g_pointX = 0;
static int16_t g_pointY = 0;
static float g_scale = 0.0;
static DeviceShape g_monitorShape = DeviceShape::SHAPE_CIRCLE;
static bool g_sdlExitStatus = false;

static constexpr uint16_t DEFAULT_CIRCLE_RESOLUTION = 454;      // 454 : max resolution
static constexpr uint16_t DEFAULT_RECT_RESOLUTION = 454;        // 454 : max resolution
static constexpr uint16_t DEFAULT_BLANK_SIZE = 20;              // 20  : BLANK_SIZE
static constexpr uint16_t RECT_BORDER_WIDTH = 5;                // 5   : BORDER_WIDTH
static constexpr uint32_t RECT_BORDER_COLOR = 0xff839192;       // 0xff839192  : BORDER_COLOR
static constexpr uint16_t SDL_REFR_PERIOD = 16;                 // 16 ms
static constexpr uint16_t BACKGROUND_HORIZONTAL = 750;         // 750 : Height Pixels
static constexpr uint16_t BACKGROUND_VERTICAL = 598;           // 598 : Width Pixels
static constexpr uint16_t BACKGROUND_RECT_HORIZONTAL = 780;         // 750 : Height Pixels
static constexpr uint16_t BACKGROUND_RECT_VERTICAL = 900;           // 800 : Width Pixels
static constexpr uint32_t DEFAULT_COLOR = 0xff000000;           // 0xff000000 : Black color
static uint32_t g_tftFb[ALIGN_BYTE(HORIZONTAL_RESOLUTION, BYTE_ALIGNMENT) * VERTICAL_RESOLUTION];
static uint32_t *g_windowsFb = nullptr;
static uint32_t *g_scaleBackGround = nullptr;
static uint32_t *g_backgroudBuffer = nullptr;

static void InitBackGround(const char* resPath, uint16_t backGroundVertical)
{
    int32_t fd = open(resPath, O_RDONLY | O_BINARY);
    if (fd == -1) {
        printf("open file Error \n");
        return;
    }

    ImageHeader* header = new ImageHeader;
    lseek(fd, 0, SEEK_SET);
    int32_t readCount = read(fd, header, sizeof(ImageHeader));
    if (readCount != sizeof(ImageHeader)) {
        printf("read header error \n");
        return;
    }
    struct stat info;
    int32_t ret = fstat(fd, &info);
    if (ret != 0) {
        printf("read file info error \n");
        return;
    }
    uint32_t pxCount = info.st_size - readCount;
    if (!(static_cast<int32_t>(pxCount) == read(fd, static_cast<void*>(g_backgroudBuffer), pxCount))) {
        printf("Init BackGround error \n");
    }
    close(fd);
    if (header != nullptr) {
        delete header;
        header = nullptr;
    }

    for (int i = 0; i < g_monitorHeight; i++) {
        for (int  j = 0; j < g_monitorWidth; j++) {
            uint32_t index = i * g_monitorWidth + j;
            uint32_t orgIndex = uint32_t(j / g_scale) + uint32_t(i / g_scale) * backGroundVertical;
            *(g_scaleBackGround + index) = *(g_backgroudBuffer + orgIndex);
        }
    }
}

static void DrawLabel()
{
    BufferInfo buf;
    buf.virAddr = g_windowsFb;
    buf.phyAddr = g_windowsFb;
    buf.width = g_monitorWidth;
    buf.height = g_monitorHeight;
    buf.stride = g_monitorWidth * sizeof(uint32_t);
    buf.compressMode = 0;
    buf.mode = ARGB8888;
    buf.rect = {0, 0, g_monitorWidth - 1, g_monitorHeight - 1};

    UILabel label;
    label.SetPosition(0, 0, 200, 200); // 0, 0, 200, 200: left, right, width, height
    std::string PointStr = "X :" + std::to_string(g_pointX) + " Y : " + std::to_string(g_pointY);
    label.SetText(PointStr.c_str());
    label.SetFont(DEFAULT_VECTOR_FONT_FILENAME, 20); // 20: font size
    label.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    label.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    label.OnDraw(buf, label.GetRect());
    LiteMGfxEngine::GetInstance()->SyncHwDraw();
}

static uint8_t GetAlpha(uint32_t x)
{
    return uint8_t((x & 0xff000000) >> 24);  // 24 : alpha offset
}

static uint32_t AlphaBlend(uint8_t a, uint32_t p)
{
    uint32_t r = (((p & 0x00ff0000) * a / 0xff) & 0x00ff0000);
    uint32_t g = (((p & 0x0000ff00) * a / 0xff) & 0x0000ff00);
    uint32_t b = (((p & 0x000000ff) * a / 0xff) & 0x000000ff);

    return uint32_t(((0x000000ff & a) << 24) + r + g + b);    // 24 : alpha offset
}

static void RefreshWinFB()
{
    uint32_t yoffset = (g_monitorHeight - g_contentHeight) / 2;      // 2 : half
    uint32_t xoffset = (g_monitorWidth - g_contentWidth) / 2;      // 2 : half
    uint32_t index = yoffset * g_monitorWidth + xoffset;
    uint32_t stride = ALIGN_BYTE(HORIZONTAL_RESOLUTION, BYTE_ALIGNMENT);

    if (memcpy_s(g_windowsFb, g_monitorHeight * g_monitorWidth * sizeof(uint32_t), g_scaleBackGround,
                 g_monitorHeight * g_monitorWidth * sizeof(uint32_t)) != EOK) {
        return;
    }

    for (int i = 0; i < g_contentHeight; i++) {
        for (int j = 0; j < g_contentWidth; j++) {
            if (g_windowsFb[index + i * g_monitorWidth + j] == 0) {
                g_windowsFb[index + i * g_monitorWidth + j] = g_tftFb[i * stride + j];
            } else if (GetAlpha(g_windowsFb[index + i * g_monitorWidth + j]) != 0xff) {  // 0xff : alpha value
                uint32_t pixels = g_windowsFb[index + i * g_monitorWidth + j];
                uint32_t pixeld = g_tftFb[i * stride + j];
                g_windowsFb[index + i * g_monitorWidth + j] =
                    AlphaBlend((0xff - GetAlpha(pixels)), pixeld);  // 0xff : alpha value
            }
        }
    }
    DrawLabel();
}

static void UpdateSDL()
{
    static std::mutex g_mutex;
    g_mutex.lock();
    SDL_UpdateTexture(g_sdlTexture, NULL, g_windowsFb, g_monitorWidth * sizeof(uint32_t));
    SDL_RenderClear(g_sdlRenderer);
    /* Update the g_sdlRenderer with the g_sdlTexture containing the rendered image */
    SDL_RenderCopy(g_sdlRenderer, g_sdlTexture, NULL, NULL);
    SDL_RenderPresent(g_sdlRenderer);
    g_mutex.unlock();
}

static int quit_filter(void * userdata, SDL_Event * event)
{
    (void)userdata;

    if (event->type == SDL_QUIT) {
        g_sdlQuitQry = true;
    }
    return 1;
}

namespace OHOS {
unsigned int* GetFramBuff()
{
    return g_tftFb;
}

unsigned int GetContentHeight()
{
    return g_contentHeight;
}

unsigned int GetContentWidth()
{
    return g_contentWidth;
}

unsigned int GetMonitorHeight()
{
    return g_monitorHeight;
}

unsigned int GetMonitorWidth()
{
    return g_monitorWidth;
}

void MonitorInit(uint32_t width, uint32_t height, DeviceShape shape)
{
    if (g_sdlInited || (shape >= DeviceShape::SHAPE_UNSUPPORT)) {
        return;
    }
    if (width > HORIZONTAL_RESOLUTION || height > VERTICAL_RESOLUTION) {
        printf("error: bad params [%d, %d]\n", width, height);
        return;
    }
    g_contentWidth = width;
    g_contentHeight = height;
    g_monitorShape = shape;
    if (g_monitorShape == DeviceShape::SHAPE_CIRCLE) {
        g_scale = float(g_contentWidth) / DEFAULT_CIRCLE_RESOLUTION;
        g_monitorHeight = uint32_t(g_scale * BACKGROUND_HORIZONTAL);
        g_monitorWidth = uint32_t(g_scale * BACKGROUND_VERTICAL);
        BaseGfxEngine::GetInstance()->SetScreenShape(CIRCLE);
        g_backgroudBuffer = (uint32_t *)malloc(sizeof(uint32_t) * BACKGROUND_VERTICAL * BACKGROUND_HORIZONTAL);
    } else {
        g_scale = float(g_contentWidth) / DEFAULT_RECT_RESOLUTION;
        g_monitorHeight = uint32_t(g_scale * BACKGROUND_RECT_HORIZONTAL);
        g_monitorWidth = uint32_t(g_scale * BACKGROUND_RECT_VERTICAL);
        BaseGfxEngine::GetInstance()->SetScreenShape(RECTANGLE);
        g_backgroudBuffer = (uint32_t *)malloc(sizeof(uint32_t) * BACKGROUND_RECT_VERTICAL * BACKGROUND_RECT_HORIZONTAL);
    }
    g_windowsFb = (uint32_t *)malloc(sizeof(uint32_t) * g_monitorHeight * g_monitorWidth);
    g_scaleBackGround = (uint32_t *)malloc(sizeof(uint32_t) * g_monitorHeight * g_monitorWidth);
    BaseGfxEngine::GetInstance()->SetScreenWidth(g_contentWidth);
    BaseGfxEngine::GetInstance()->SetScreenHeight(g_contentHeight);
    if (g_monitorShape == DeviceShape::SHAPE_CIRCLE) {
        InitBackGround(BACKGROUND_CIRCLE_RES_PATH, BACKGROUND_VERTICAL);
    } else {
        InitBackGround(BACKGROUND_RECT_RES_PATH, BACKGROUND_RECT_VERTICAL);
    }
    SDL_CreateThread(OHOS::SdlRefresh, "sdl_refr", NULL);
    while (!g_sdlInited) {} /* Wait until 'sdl_refr' initializes the SDL */
}

void MonitorFlush()
{
    RefreshWinFB();
    UpdateSDL();
}

/**
 * SDL main thread. All SDL related task have to be handled here!
 * It initializes SDL, handles drawing and the mouse.
 */

int SdlRefresh(void * param)
{
    (void)param;
    OHOS::InitSdl();
    /* Run until quit event not arrives */
    while (!g_sdlQuitQry) {
        /* Refresh handling */
        OHOS::SdlEventDeal();
    }
    OHOS::DeinitSdl();
    g_sdlExitStatus = true;
    return 0;
}

bool SdlExitStatus(void)
{
    return g_sdlExitStatus;
}

void DeinitSdl(void)
{
    SDL_DestroyTexture(g_sdlTexture);
    SDL_DestroyRenderer(g_sdlRenderer);
    SDL_DestroyWindow(g_sdlWindow);
    SDL_Quit();
}

void InitSdl(void)
{
    /* Initialize the SDL */
    SDL_Init(SDL_INIT_VIDEO);

    SDL_SetEventFilter(quit_filter, NULL);

    g_sdlWindow = SDL_CreateWindow("UIKit Simulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   g_monitorWidth, g_monitorHeight,  0);

    g_sdlRenderer = SDL_CreateRenderer(g_sdlWindow, -1, 0);     // -1 : param

    g_sdlTexture = SDL_CreateTexture(g_sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC,
                                     g_monitorWidth, g_monitorHeight);

    SDL_SetTextureBlendMode(g_sdlTexture, SDL_BLENDMODE_BLEND);

    /* Initialize the frame buffer to DEFAULT_COLOR */
    for (uint32_t i = 0; i < g_contentHeight * g_contentWidth; i++) {
        g_tftFb[i] = DEFAULT_COLOR;
    }

    /* Initialize the frame buffer to DEFAULT_COLOR */
    for (uint32_t i = 0; i < g_monitorHeight * g_monitorWidth; i++) {
        g_windowsFb[i] = DEFAULT_COLOR;
    }

    SDL_UpdateTexture(g_sdlTexture, NULL, g_windowsFb, g_monitorWidth * sizeof(uint32_t));
    g_sdlInited = true;
}

void SdlEventDeal(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
#if USE_MOUSE != 0
        MouseHandler(&event);
#endif
        if ((&event)->type == SDL_WINDOWEVENT) {
            switch ((&event)->window.event) {
#if SDL_VERSION_ATLEAST(2, 0, 5)  // 2, 0, 5 :  version
                case SDL_WINDOWEVENT_TAKE_FOCUS:
#endif
                case SDL_WINDOWEVENT_EXPOSED:
                    UpdateSDL();
                    break;
                default:
                    break;
            }
        }
    }
    /* Sleep some time */
    SDL_Delay(SDL_REFR_PERIOD);
}

void UpdatePoint(int16_t x, int16_t y)
{
    g_pointX = x;
    g_pointY = y;
    OHOS::GraphicEventHandler::GetInstance()->PostGraphicEvent(
        [] {
            GraphicMutex lock_;
            lock_.Lock();
            // 选择一块小区域进行刷新，保证在静态屏幕下也能触发坐标刷新
            RootView::GetInstance()->InvalidateRect(Rect(0, 0, 1, 1));
            lock_.Unlock();
        },
        false);
}

float GetScale()
{
    return g_scale;
}
}  // namespace OHOS
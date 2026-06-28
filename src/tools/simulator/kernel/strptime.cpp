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
#include "engines/gfx/lite_m_gfx_engine.h"
#include "dock/screen_device_proxy.h"
#include "components/root_view.h"

using namespace std;
using namespace OHOS;

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
        g_monitorHeight = uint32_t(g_scale * BACKGROUNDL_HORIZONTAL);
        g_monitorWidth = uint32_t(g_scale * BACKGROUNDL_VERTICAL);
        BaseGfxEngine::GetInstance()->SetScreenShape(CIRCLE);
    } else {
        g_monitorHeight = g_contentHeight + (DEFAULT_BLANK_SIZE + RECT_BORDER_WIDTH) * 2;   // 2 : left and right
        g_monitorWidth = g_contentWidth + (DEFAULT_BLANK_SIZE + RECT_BORDER_WIDTH) * 2;   // 2 : top and bottom
    }
    g_windowsFb = (uint32_t *)malloc(sizeof(uint32_t) * g_monitorHeight * g_monitorWidth);
    g_scaleBackGround = (uint32_t *)malloc(sizeof(uint32_t) * g_monitorHeight * g_monitorWidth);
    BaseGfxEngine::GetInstance()->SetScreenWidth(g_contentWidth);
    BaseGfxEngine::GetInstance()->SetScreenHeight(g_contentHeight);
    if (g_monitorShape == DeviceShape::SHAPE_CIRCLE) {
        InitCirclBackGround();
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
}  // namespace OHOS
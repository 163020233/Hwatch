/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022. All rights reserved.
 * Description: main file
 * Create: 2022-09-27
 */

#if BUILD_IN_LINUX
#include <windows.h>
#else
#include <Windows.h>
#endif
#include <cstdlib>
#ifdef USE_MINGW
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif
#include <string>
#include <iostream>
#include <cstdio>
#include "common/graphic_startup.h"
#include "common/input_device_manager.h"
#include "common/task_manager.h"
#include "font/ui_font_header.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "components/root_view.h"
#include "hal_tick.h"
#include "mouse_input.h"
#include "monitor.h"
#include "uisample/UISampleView.h"
#include "NativeLauncher.h"
#include "graphic_service.h"
#include "StartNative.h"

using namespace std;
using namespace OHOS;

static MousePointerInput* g_pointer = nullptr;
static MouseWheelInput* g_wheel = nullptr;
static MouseKeyInput* g_key = nullptr;

static void InitHal()
{
    MonitorInit(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION, static_cast<OHOS::DeviceShape>(DEVICE_SHAPE));
#if USE_MOUSE
    g_pointer = new MousePointerInput();
    g_wheel = new MouseWheelInput();
    g_key = new MouseKeyInput();
    InputDeviceManager::GetInstance()->Add(g_pointer);
    InputDeviceManager::GetInstance()->Add(g_wheel);
    InputDeviceManager::GetInstance()->Add(g_key);
#endif
}

// assuming below are the memory pool
static uint8_t g_fontPsramBaseAddr[OHOS::MIN_FONT_PSRAM_LENGTH];

int main(int argc, char* argv[])
{
    if (!GraphicService::GetInstance()->InitGraphicService()) {
        GRAPHIC_LOGE("InitGraphicService fail!!");
        return -1;
    }
    GRAPHIC_LOGE("InitGraphicService success!!");
    GraphicStartUp::InitFontEngine(reinterpret_cast<uintptr_t>(g_fontPsramBaseAddr), OHOS::MIN_FONT_PSRAM_LENGTH,
                                   VECTOR_FONT_DIR, "HarmonyOS_Sans_SC_Regular.ttf");
    InitHal();
    StartNative();
    GraphicService::GetInstance()->NotifyScreenOn();
    while (!SdlExitStatus()) {
        /* Periodically call TaskHandler(). It could be done in a timer interrupt or an OS task too. */
        GraphicService::GetInstance()->AsyncTriggerNativeGraphicTask();
        Sleep(16); /* 16 ms : Just to let the system breathe */
    }
    if (g_pointer != nullptr) {
        delete g_pointer;
        g_pointer = nullptr;
    }
    if (g_wheel != nullptr) {
        delete g_wheel;
        g_wheel = nullptr;
    }
    return 0;
}


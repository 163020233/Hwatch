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

#include <cmath>
#include "graphic_service_wrapper.h"
#if !defined(_WIN32) && !VERSION_SOCMN2
#include "soc_lcd_api.h"
#endif
#include "graphic_service.h"
#include "gfx_utils/graphic_log.h"
#include "common/low_power_manager.h"
#include "common/image_cache_manager.h"
#include "common/screen.h"
#include "components/root_view.h"
#include "components/ui_image_view.h"

#ifdef __cplusplus
extern "C" {
#endif
static const uint32_t SECOND_TO_MILLISECOND = 1000;
static const int32_t FULL_FREQUENCE = 60;
#if !defined(_WIN32) && !VERSION_SOCMN2
static int32_t g_cycleNum = -1;
#endif
static int32_t g_freq = 0;
static bool g_initialized = false;
bool g_isScreenOn = false;
static bool g_showLogo = false;
static OHOS::UIImageView* g_logo = nullptr;
static OHOS::ImageInfo* g_imgInfo = nullptr;

#ifdef QSPI_DISPLAY
#define IMAGE_OPEN_LOGO_PATH "/boot/data/OPEN_LOGO_QSPI.bin"
#else
#define IMAGE_OPEN_LOGO_PATH "/boot/data/OPEN_LOGO.bin"
#endif

static void UikitTriggerCallBack(void)
{
    if (!g_initialized || !g_isScreenOn) {
        GRAPHIC_LOGE("vsync should not be started when g_isScreenOn:%d g_initialized:%d", g_isScreenOn, g_initialized);
        return;
    }
    GraphicService::GetInstance()->UpdateGraphicEvent();
}

void SetTriggerFreq(int freq)
{
#if !defined(_WIN32) && !VERSION_SOCMN2
    int32_t cycleNum = 0;
    cycleNum = static_cast<int32_t>(round(FULL_FREQUENCE / float(freq)));
    g_freq = FULL_FREQUENCE / cycleNum;
    g_cycleNum = cycleNum;

    GRAPHIC_LOGD("set freq: %d, cycleNum: %d\n", freq, g_cycleNum);
    uapi_lcd_set_mcu_int_cycle_num(g_cycleNum);
    uapi_lcd_register_mcu_int_callback(UikitTriggerCallBack);
#endif
}

void SetFrequenceInner(int freq)
{
    if (!g_isScreenOn || freq == 0) {
        return;
    }

    if (freq > FULL_FREQUENCE) {
        freq = FULL_FREQUENCE;
    }

    if (g_freq != freq) {
        if (freq == FULL_FREQUENCE) {
#if ENABLE_DYNAMIC_FRAME_RATE
            if (!OHOS::LowPowerManager::GetInstance()->IsLiteSurfaceInRootView()) {
                GraphicService::GetInstance()->EnableAsyncMode(true);
            }
#endif
        } else {
            GraphicService::GetInstance()->EnableAsyncMode(false);
        }
        SetTriggerFreq(freq);
    }
}

int GetFrequence(void)
{
    return g_freq;
}

int IsNativeRunning(void)
{
    return GraphicService::GetInstance()->IsNativeRunning();
}

void SetFrequence(int freq)
{
    if (freq <= 0 || freq > FULL_FREQUENCE) {
        GRAPHIC_LOGE("freq %d should be in (0, 60]", freq);
        return;
    }
    GraphicService::GetInstance()->PostGraphicEvent([=] {
        OHOS::LowPowerManager::GetInstance()->SetMaxFrequence(freq);
    });
}

void NotifyScreenOn(void)
{
    GraphicService::GetInstance()->NotifyScreenOn();
    if (!g_isScreenOn) {
        g_isScreenOn = true;
#if ENABLE_DYNAMIC_FRAME_RATE
        OHOS::LowPowerManager::GetInstance()->ExitLowPower();
#endif
    }
}

void NotifyScreenOff(void)
{
#if !defined(_WIN32) && !VERSION_SOCMN2
    uapi_lcd_set_mcu_int_cycle_num(0);
    g_cycleNum = -1;
#endif
    g_freq = 0;
    g_isScreenOn = false;
    GraphicService::GetInstance()->NotifyScreenOff();
#if ENABLE_DYNAMIC_FRAME_RATE
    // When the screen is off, enter low power mode directly.
    OHOS::LowPowerManager::GetInstance()->EnterLowPower();
#endif
}

void InitGraphicService(void)
{
    if (g_initialized) {
        return;
    }

    if (!GraphicService::GetInstance()->InitGraphicService()) {
        GRAPHIC_LOGE("InitGraphicService fail!!");
        return;
    }
    NotifyScreenOn();
    g_initialized = true;
}

void ShowGraphicLogo()
{
#if ENABLE_IMAGE_PACKER
    if (g_showLogo) {
        return;
    }
    g_showLogo = true;
    g_logo = new OHOS::UIImageView();
    g_logo->SetPosition(0, 0, OHOS::Screen::GetInstance().GetWidth(), OHOS::Screen::GetInstance().GetHeight());
    g_logo->SetAutoEnable(false);
    g_logo->SetResizeMode(OHOS::UIImageView::CENTER);
    g_imgInfo = OHOS::ImageCacheManager::GetInstance().LoadSingleRes(IMAGE_OPEN_LOGO_PATH);
    g_logo->SetSrc(g_imgInfo);
    GraphicService::GetInstance()->PostGraphicEvent(
        [] {
            OHOS::RootView::GetInstance()->Add(g_logo);
            OHOS::RootView::GetInstance()->Invalidate();
        });
#endif
}

void HideGraphicLogo()
{
#if ENABLE_IMAGE_PACKER
    if (!g_showLogo) {
        return;
    }
    g_showLogo = false;
    GraphicService::GetInstance()->PostGraphicEvent(
        [] {
            if (g_logo != nullptr) {
                OHOS::RootView::GetInstance()->Remove(g_logo);
                delete g_logo;
                g_logo = nullptr;
                OHOS::ImageCacheManager::GetInstance().UnloadSingleRes(IMAGE_OPEN_LOGO_PATH);
                g_imgInfo = nullptr;
            }
        });
#endif
}

void PostGraphicEvent(GraphicEventCb eventCb, void* userData)
{
    GraphicService::GetInstance()->PostGraphicEvent(
        [=] {
            eventCb(userData);
            });
}

void GpuResetStart(void)
{
    GraphicService::GetInstance()->GpuResetStart();
}

void GpuResetEnd(void)
{
    GraphicService::GetInstance()->GpuResetEnd();
}

#ifdef __cplusplus
};
#endif

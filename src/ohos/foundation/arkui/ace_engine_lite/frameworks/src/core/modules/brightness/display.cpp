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

#include "display.h"
#include "ace_log.h"
#include "errcode.h"
#if defined(SUPPORT_POWER_MANAGER)
#include "power_display_service.h"
#include "soc_lcd_api.h"
#endif

constexpr char KEEPSCREENON[] = "keepScreenOn";
constexpr char MODE[] = "mode";
constexpr char VALUE[] = "value";
// The available brightness is 0-100 in ACE, which corresponds to 0-255 in drv_lcd.
const int ACE_MAX_BRIGHTNESS = 100;
// The max timeout of keepScreenOn 1000 * 60 * 60 ms. See POWER_DISPLAY_MAX_SET_TIME in power_manager.
const int ACE_MAX_KEEP_SCREEN_ON_TIMEOUT = (1000 * 60 * 60);
// The backup of keepScreenOn timeout. The default value is 5000ms.
uint32_t g_keepScreenOnTimeoutBackup = UINT32_MAX;
// The backup of system brightness mode
int g_brightnessModeBackup = -1;
// The backup of system brightness value.
uint8_t g_brightnessValueBackup = UINT8_MAX;
const char *ERR_GENERAL = "display error";
const char *ERR_NOT_SUPPORT = "display set is not support";
const int DISPLAY_SUCCESS = 0;
const int DISPLAY_FAIL = -1;
const int DISPLAY_NOT_SUPPORT = -2;

namespace OHOS {
namespace ACELite {
static void FailCallBack(const JSIValue thisVal, const JSIValue args, int ret)
{
    JSIValue errCode = JSI::CreateNumber(ret);
    JSIValue errInfo;
    if (ret == DISPLAY_FAIL) {
        errInfo = JSI::CreateString(ERR_GENERAL);
    } else {
        errInfo = JSI::CreateString(ERR_NOT_SUPPORT);
    }
    JSIValue fail = JSI::GetNamedProperty(args, CB_FAIL);
    JSIValue complete = JSI::GetNamedProperty(args, CB_COMPLETE);
    JSIValue argv[ARGC_TWO] = {errInfo, errCode};
    if (!JSI::ValueIsUndefined(fail)) {
        JSI::CallFunction(fail, thisVal, argv, ARGC_TWO);
    }
    if (!JSI::ValueIsUndefined(complete)) {
        JSI::CallFunction(complete, thisVal, nullptr, 0);
    }
    JSI::ReleaseValueList(fail, complete, errInfo, errCode, ARGS_END);
}

static void SuccessCallBack(const JSIValue thisVal, const JSIValue args, JSIValue jsiValue)
{
    if (JSI::ValueIsUndefined(args)) {
        return;
    }
    JSIValue success = JSI::GetNamedProperty(args, CB_SUCCESS);
    JSIValue complete = JSI::GetNamedProperty(args, CB_COMPLETE);
    if (!JSI::ValueIsUndefined(success)) {
        if (JSI::ValueIsUndefined(jsiValue)) {
            JSI::CallFunction(success, thisVal, nullptr, 0);
        } else {
            JSI::CallFunction(success, thisVal, &jsiValue, ARGC_ONE);
        }
    }
    if (!JSI::ValueIsUndefined(complete)) {
        JSI::CallFunction(complete, thisVal, nullptr, 0);
    }
    JSI::ReleaseValueList(success, complete, ARGS_END);
}

JSIValue DisplayModule::SetKeepScreenOn(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    int ret = DISPLAY_NOT_SUPPORT;
#if defined(SUPPORT_POWER_MANAGER)
    if (g_keepScreenOnTimeoutBackup == UINT32_MAX) {
        // Get the default value of keep_on_timeout in the power_manger.
        screen_context_t *curScreenCtx = power_display_get_screen_context();
        if (curScreenCtx != nullptr) {
            g_keepScreenOnTimeoutBackup = curScreenCtx->keep_on_timeout;
        }
    }
    errcode_t result = ERRCODE_NOT_SUPPORT;
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api != nullptr) {
        bool keepScreenOn = JSI::GetBooleanProperty(args[0], KEEPSCREENON);
        if (keepScreenOn) {
            if (display_api->get_screen_state() != SCREEN_ON) {
                display_api->turn_on_screen();
            }
            result = display_api->set_screen_set_keepon_timeout(0);
            result = display_api->set_screen_set_keepon_timeout(ACE_MAX_KEEP_SCREEN_ON_TIMEOUT);
        } else {
            result = display_api->set_screen_set_keepon_timeout(0);
            result = display_api->set_screen_set_keepon_timeout(g_keepScreenOnTimeoutBackup);
        }
    }
    ret = (result == ERRCODE_SUCC) ? DISPLAY_SUCCESS : DISPLAY_FAIL;
#endif
    if (ret == DISPLAY_SUCCESS) {
        SuccessCallBack(thisVal, args[0], JSI::CreateUndefined());
    } else {
        FailCallBack(thisVal, args[0], ret);
    }
    return JSI::CreateUndefined();
}

JSIValue DisplayModule::SetMode(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    int ret = DISPLAY_NOT_SUPPORT;
#if defined(SUPPORT_POWER_MANAGER)
    errcode_t result;
    brightness_mode_t mode = static_cast<brightness_mode_t>(JSI::GetNumberProperty(args[0], MODE));
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (g_brightnessModeBackup == -1) {
        // To backup the system value.
        g_brightnessModeBackup = static_cast<int>(display_api->get_brightness_mode());
    }
    if (mode <= brightness_mode::AUTO) {
        if (display_api->get_screen_state() != screen_state::SCREEN_ON) {
            display_api->turn_on_screen();
        }
        result = display_api->set_brightness_mode(mode);
    } else {
        result = display_api->set_brightness_mode(static_cast<brightness_mode_t>(g_brightnessModeBackup));
    }
    if (result == ERRCODE_SUCC) {
        ret = DISPLAY_SUCCESS;
    } else {
        ret = DISPLAY_FAIL;
    }
#endif
    if (ret == DISPLAY_SUCCESS) {
        SuccessCallBack(thisVal, args[0], JSI::CreateUndefined());
    } else {
        FailCallBack(thisVal, args[0], ret);
    }
    return JSI::CreateUndefined();
}

JSIValue DisplayModule::GetMode(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    int ret = DISPLAY_NOT_SUPPORT;
    JSIValue result = JSI::CreateObject();
#if defined(SUPPORT_POWER_MANAGER)
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api->get_screen_state() != screen_state::SCREEN_ON) {
        display_api->turn_on_screen();
    }
    brightness_mode_t mode = display_api->get_brightness_mode();
    if (mode >= brightness_mode::MANUAL) {
        JSI::SetNumberProperty(result, MODE, static_cast<double>(mode));
        ret = DISPLAY_SUCCESS;
    }
#endif
    if (ret == DISPLAY_SUCCESS) {
        SuccessCallBack(thisVal, args[0], result);
    } else {
        FailCallBack(thisVal, args[0], ret);
    }
    JSI::ReleaseValue(result);
    return JSI::CreateUndefined();
}

JSIValue DisplayModule::SetValue(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    int ret = DISPLAY_NOT_SUPPORT;
#if defined(SUPPORT_POWER_MANAGER)
    errcode_t result = ERRCODE_FAIL;
    uint8_t brightnessValue = static_cast<uint8_t>(JSI::GetNumberProperty(args[0], VALUE));
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (g_brightnessValueBackup == UINT8_MAX) {
        // To backup the system value.
        g_brightnessValueBackup = display_api->get_brightness();
    }
    if (brightnessValue >= 0 && brightnessValue <= ACE_MAX_BRIGHTNESS) {
        if (display_api->get_screen_state() != screen_state::SCREEN_ON) {
            display_api->turn_on_screen();
        }
        display_api->set_brightness_mode(brightness_mode::MANUAL);
        // CN: LCD普通模式亮度范围0-255，不允许JSApp设置HDR高亮模式.
        result = display_api->set_brightness(brightnessValue * (LCD_NORMAL_BRIGHT_MAX / ACE_MAX_BRIGHTNESS));
    } else {
        HILOG_WARN(HILOG_MODULE_ACE, "It's invalid brightness value: %u. The available range: 0-100.", brightnessValue);
    }
    if (result == ERRCODE_SUCC) {
        ret = DISPLAY_SUCCESS;
    } else {
        ret = DISPLAY_FAIL;
    }
#endif
    if (ret == DISPLAY_SUCCESS) {
        SuccessCallBack(thisVal, args[0], JSI::CreateUndefined());
    } else {
        FailCallBack(thisVal, args[0], ret);
    }
    return JSI::CreateUndefined();
}

JSIValue DisplayModule::GetValue(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    int ret = DISPLAY_NOT_SUPPORT;
    JSIValue result = JSI::CreateObject();
#if defined(SUPPORT_POWER_MANAGER)
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api->get_screen_state() != SCREEN_ON) {
        display_api->turn_on_screen();
    }
    uint8_t brightnessValue = (ACE_MAX_BRIGHTNESS / LCD_NORMAL_BRIGHT_MAX) * (display_api->get_brightness());
    JSI::SetNumberProperty(result, VALUE, static_cast<double>(brightnessValue));
    if (brightnessValue >= 0 && brightnessValue <= ACE_MAX_BRIGHTNESS) {
        SuccessCallBack(thisVal, args[0], result);
    } else {
        FailCallBack(thisVal, args[0], ret);
    }
#else
    FailCallBack(thisVal, args[0], ret);
#endif
    JSI::ReleaseValue(result);
    return JSI::CreateUndefined();
}

void DisplayModule::OnTerminate()
{
#if defined(SUPPORT_POWER_MANAGER)
    errcode_t result;
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api == nullptr) {
        HILOG_DEBUG(HILOG_MODULE_ACE, "Failed to get power display api.");
        return;
    }
    // resume brightness mode.
    if (g_brightnessModeBackup != -1) {
        result = display_api->set_brightness_mode(static_cast<brightness_mode_t>(g_brightnessModeBackup));
        if (result != ERRCODE_SUCC) {
            HILOG_DEBUG(HILOG_MODULE_ACE, "Failed to resume the system brightness mode.");
        }
    }
    // resume brightness value.
    if (g_brightnessValueBackup != UINT8_MAX) {
        result = display_api->set_brightness(g_brightnessValueBackup);
    }
    if (result != ERRCODE_SUCC) {
        HILOG_DEBUG(HILOG_MODULE_ACE, "Failed to resume the system brightness value.");
    }
    // resume screen keepon timeout.
    if (g_keepScreenOnTimeoutBackup != UINT32_MAX) {
        result = display_api->set_screen_set_keepon_timeout(g_keepScreenOnTimeoutBackup);
    }
    if (result != ERRCODE_SUCC) {
        HILOG_DEBUG(HILOG_MODULE_ACE, "Failed to resume the system screen keepon timeout.");
    }
#endif
}

void InitBrightnessModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "setKeepScreenOn", DisplayModule::SetKeepScreenOn);
    JSI::SetModuleAPI(exports, "setMode", DisplayModule::SetMode);
    JSI::SetModuleAPI(exports, "getMode", DisplayModule::GetMode);
    JSI::SetModuleAPI(exports, "setValue", DisplayModule::SetValue);
    JSI::SetModuleAPI(exports, "getValue", DisplayModule::GetValue);
    JSI::SetOnTerminate(exports, DisplayModule::OnTerminate);
}
}  // namespace ACELite
}  // namespace OHOS
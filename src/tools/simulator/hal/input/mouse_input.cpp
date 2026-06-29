/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd..
 * Description: simulator mouse driver definition
 */

#include "mouse_input.h"
#include "monitor.h"
#include "gfx_utils/list.h"
#include "NativeAbility.h"
#include "AppViewIDs.h"
#include "common/key_code.h"

using namespace OHOS;

#if USE_MOUSE != 0
static bool g_leftButtonDown = false;
static int16_t g_lastX = 0;
static int16_t g_lastY = 0;
static int16_t g_lastRotate = 0;
static List<int16_t> g_rotateList;
const uint8_t MAX_LIST_SIZE = 100;
static bool g_pressPowerFlag = false;
static bool g_pressFuncFlag = false;

/**
 * Get the current position and state of the mouse
 * @param data store the mouse data here
 * @return false: because the points are not buffered, so no more data to be read
 */
bool MousePointerInput::Read(OHOS::DeviceData& data)
{
    /* Store the collected data */
    data.point.x = g_lastX;
    data.point.y = g_lastY;
    data.state = g_leftButtonDown ? STATE_PRESS : STATE_RELEASE;
    return false;
}

/**
 * Get the current position and state of the mouse
 * @param data store the mouse data here
 * @return false: because the points are not buffered, so no more data to be read
 */
bool MouseWheelInput::Read(OHOS::DeviceData& data)
{
    if (g_rotateList.IsEmpty()) {
        g_lastRotate = 0;
    }
    if (g_rotateList.Size() > 0) {
        g_lastRotate = g_rotateList.Front();
        g_rotateList.PopFront();
    }
    data.rotate = g_lastRotate;
    return false;
}

MouseKeyInput::MouseKeyInput()
{
    if (static_cast<DeviceShape>(DEVICE_SHAPE) == DeviceShape::SHAPE_CIRCLE) {
        buttonPowerXStart_ = 460 * GetScale();
        buttonPowerXEnd_ = 510 * GetScale();
        buttonPowerYStart_ = 38 * GetScale();
        buttonPowerYEnd_ = 105 * GetScale();
        buttonFuncXStart_ = 460 * GetScale();
        buttonFuncXEnd_ = 510 * GetScale();
        buttonFuncYStart_ = 345 * GetScale();
        buttonFuncYEnd_ = 415 * GetScale();
    } else {
        buttonPowerXStart_ = 590 * GetScale();
        buttonPowerXEnd_ = 655 * GetScale();
        buttonPowerYStart_ = 0 * GetScale();
        buttonPowerYEnd_ = 120 * GetScale();
        buttonFuncXStart_ = 590 * GetScale();
        buttonFuncXEnd_ = 655 * GetScale();
        buttonFuncYStart_ = 310 * GetScale();
        buttonFuncYEnd_ = 450 * GetScale();
    }
}

/**
 * Get the current position and state of the mouse
 * @param data store the mouse data here
 * @return false: because the points are not buffered, so no more data to be read
 */
bool MouseKeyInput::Read(OHOS::DeviceData& data)
{
    /* Store the collected data */
    data.point.x = g_lastX;
    data.point.y = g_lastY;
    data.state = g_leftButtonDown ? STATE_PRESS : STATE_RELEASE;
    if (data.point.x >= buttonPowerXStart_ && data.point.x <= buttonPowerXEnd_ &&
        data.point.y >= buttonPowerYStart_ && data.point.y <= buttonPowerYEnd_ &&
        data.state == STATE_PRESS) {
        g_pressPowerFlag = true;
    }
    if (data.point.x >= buttonFuncXStart_ && data.point.x <= buttonFuncXEnd_ &&
        data.point.y >= buttonFuncYStart_ && data.point.y <= buttonFuncYEnd_ &&
        data.state == STATE_PRESS) {
        g_pressFuncFlag = true;
    }
    if (g_pressPowerFlag && data.state == STATE_RELEASE) {
        data.keyId = static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_POWER);
        g_pressPowerFlag = false;
    }
    if (g_pressFuncFlag && data.state == STATE_RELEASE) {
        data.keyId = static_cast<uint16_t>(ZliteKeyCode::ZLITE_KEY_FUNC);
        g_pressFuncFlag = false;
    }
    return false;
}

/**
 * It will be called from the main SDL thread
 */
void MouseHandler(SDL_Event *event)
{
    uint32_t yoffset = (OHOS::GetMonitorHeight() - OHOS::GetContentHeight()) / 2;  // 2 : half
    uint32_t xoffset = (OHOS::GetMonitorWidth() - OHOS::GetContentWidth()) / 2;     // 2 : half

    switch (event->type) {
        case SDL_MOUSEBUTTONUP:
            if (event->button.button == SDL_BUTTON_LEFT) {
                g_leftButtonDown = false;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event->button.button == SDL_BUTTON_LEFT) {
                g_leftButtonDown = true;
                g_lastX = event->button.x - xoffset;
                g_lastY = event->button.y - yoffset;
            }
            break;
        case SDL_MOUSEWHEEL:
            if (g_rotateList.Size() >= MAX_LIST_SIZE) {
                GRAPHIC_LOGI("SDL_MOUSEWHEEL data list is full.");
                break;
            }
            g_rotateList.PushBack(event->wheel.y);
            break;
        case SDL_MOUSEMOTION:
            if (event->motion.x < xoffset || event->motion.x > (xoffset + OHOS::GetContentWidth()) ||
                event->motion.y < yoffset || event->motion.y > (yoffset + OHOS::GetContentHeight())) {
                g_leftButtonDown = false;
                return;
            }
            g_lastX = event->motion.x - xoffset;
            g_lastY = event->motion.y - yoffset;
            UpdatePoint(g_lastX, g_lastY);
            break;
    }
}

#endif

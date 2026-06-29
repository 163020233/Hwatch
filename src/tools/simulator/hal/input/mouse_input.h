/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd..
 * Description: simulator mouse driver definition
 */

#ifndef MOUSE_INPUT_H
#define MOUSE_INPUT_H

#include <cstdint>
#include "config.h"
#include "dock/pointer_input_device.h"
#include "dock/rotate_input_device.h"
#include "dock/key_input_device.h"
#include "SDL.h"

#if USE_MOUSE
class MousePointerInput : public OHOS::PointerInputDevice {
public:
    bool Read(OHOS::DeviceData& data) override;
};

class MouseWheelInput : public OHOS::RotateInputDevice {
public:
    bool Read(OHOS::DeviceData& data) override;
};

class MouseKeyInput : public OHOS::KeyInputDevice {
public:
    MouseKeyInput();
    bool Read(OHOS::DeviceData& data) override;

private:
    int16_t buttonPowerXStart_ = 0;
    int16_t buttonPowerXEnd_ = 0;
    int16_t buttonPowerYStart_ = 0;
    int16_t buttonPowerYEnd_ = 0;
    int16_t buttonFuncXStart_ = 0;
    int16_t buttonFuncXEnd_ = 0;
    int16_t buttonFuncYStart_ = 0;
    int16_t buttonFuncYEnd_ = 0;
};
/**
 * It will be called from the main SDL thread
 */
void MouseHandler(SDL_Event *event);
#endif /* USE_MOUSE */
#endif /* MOUSE_INPUT_H */

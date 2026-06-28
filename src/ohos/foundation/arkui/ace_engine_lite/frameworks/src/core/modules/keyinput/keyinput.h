/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_KEYINPUT_MODULE_H
#define OHOS_ACELITE_KEYINPUT_MODULE_H

#include "jsi.h"
#include "events/key_event.h"
#include "events/rotate_event.h"
#include "events/gesture_event.h"
#include "dock/input_device.h"
#include "common/key_code.h"
typedef bool (*KeyNotifyCallback)(uint16_t keyID, uint8_t state);

namespace OHOS {
namespace ACELite {
class KeyInputModule : public MemoryHeap {
public:
    KeyInputModule() = default;
    ~KeyInputModule() = default;
    static JSIValue SetKeyListener(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue SetRotateListener(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue SetGestureListener(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static void SetNotifyKeyListener(KeyNotifyCallback callback);
    static KeyNotifyCallback GetNotifyKeyListener(void);
    static void DefineProperty(
        JSIValue target, const char *propName, JSIFunctionHandler getter, JSIFunctionHandler setter);
    static JSIValue OnClickGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue OnClickSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue OnPressGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue OnPressSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static void OnDestroy();

private:
    static KeyNotifyCallback notifyCallback_;
    static void ReleaseGestureCallback();
    static void ReleaseKeyCallback();
    static void ReleaseRotateCallbacks();
    static void ReleaseOnClickCallbacks();
    static void ReleaseOnPressCallbacks();
};

void InitKeyInputModule(JSIValue exports);

class KeyListener {
public:
    static KeyListener *GetInstance()
    {
        static KeyListener instance;
        return &instance;
    }

    void OnKeyEvent(const KeyEvent &event);

    bool HasRegister();

    void SendToJS(const KeyEvent &event);

private:
    KeyListener() : isPress_(false), isLongPress_(false){};
    ~KeyListener() = default;
    void OnRelease(const KeyEvent &event);
    void OnKeyLongPress(const KeyEvent &event);
    bool isPress_;
    bool isLongPress_;
};

class RotateListener {
public:
    static RotateListener *GetInstance()
    {
        static RotateListener instance;
        return &instance;
    }

    void OnRotateEvent(const RotateEvent &event);

    bool HasRegister()
    {
        return isRegister_;
    }
    void SetRegister(bool isRegister)
    {
        isRegister_ = isRegister;
    }

private:
    RotateListener() : isRegister_(false){};
    ~RotateListener() = default;
    bool isRegister_;
};


class GestureListener {
public:
    static GestureListener *GetInstance()
    {
        static GestureListener instance;
        return &instance;
    }

    void OnGestureEvent(const GestureEvent &event);

    bool HasRegister()
    {
        return isRegister_;
    }
    void SetRegister(bool isRegister)
    {
        isRegister_ = isRegister;
    }

private:
    GestureListener() : isRegister_(false){};
    ~GestureListener() = default;
    bool isRegister_;
};


}  // namespace ACELite
}  // namespace OHOS
int32_t KeySendMsgToJS(const char *msgBody, uint16_t type);
int32_t RotateSendMsgToJS(int16_t rotate);
int32_t GestureSendMsgToJS(int16_t gesture);
#endif

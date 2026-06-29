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

#include <cstdlib>
#include "graphic_config.h"
#include "dfx/dfx_dom_dump.h"
#include "graphic_service.h"
#include "dfx/dfx_cmd.h"
#if ENABLE_DFX_CMD
#include <unistd.h>
#include "components/root_view.h"
#include "components/ui_view.h"
#include "common/screen.h"
#include "dfx/fps_task.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "gfx_utils/mem_check.h"
#include "gfx_utils/image_info.h"
#include "securec.h"
#include "dfx/dfx_record.h"
#include "dfx/dfx_frame_trace.h"
#include "dfx/dfx_traceui.h"
#include "dfx/dfx_img.h"
#include "common/low_power_manager.h"
#include "common/image_cache_manager.h"
#include "common/input_device_manager.h"
#include "common/task_manager.h"
#include "graphic_service_wrapper.h"
#include "graphic_event_handler.h"
#include "animator/animator_manager.h"
#include "imgdecode/image_load.h"
#ifdef SUPPORT_OHOSFWK
#include "RotationManager.h"
#endif
#if ENABLE_DEBUG
#include "common/key_code.h"
#include "dfx/event_injector.h"
#include "dock/input_device.h"
#include "events/key_event.h"
#include "security_hash.h"
#endif
#include <unordered_map>

namespace OHOS {
constexpr int DEFAULT_FILE_PERMISSION = 0666;
char* g_filePath = nullptr;
UIView* g_capView = nullptr;
Rect g_cropRect{0, 0, 0, 0};
uint8_t *g_traceuiBuf = nullptr;

void DumpViewId(UIView* view)
{
    if (view == nullptr) {
        return;
    }

    if (view->GetViewId() != nullptr) {
        Rect rect = view->GetRect();
        printf("   [%s]: %d, %d, %d, %d\n", view->GetViewId(),
            rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
    }

    if (view->IsViewGroup()) {
        UIView* child = static_cast<UIViewGroup*>(view)->GetChildrenHead();
        while (child != nullptr) {
            DumpViewId(child);
            child = child->GetNextSibling();
        }
    }
}

void DumpViewIdCmd(int argc, char *argv[])
{
    printf("=== Begin Dump Widget ViewId. Format: [viewId]: x, y, w, h ===\n");
    DumpViewId(RootView::GetInstance());
    printf("=== Dump Widget ViewId End===\n");
}

void DumpRootView(int argc, char *argv[])
{
    printf("=== Begin Dump RootView ===\n");
    DfxDomDump::GetInstance().DumpRootView();
    printf("=== Dump RootView End===\n");
}

#if ENABLE_MEMORY_CHECK
void DumpMemoryCmd(int argc, char *argv[])
{
    MemCheck::GetInstance()->DumpMemInfo();
}

void DumpState(int argc, char *argv[])
{
    printf("=== Dump State ===\n");
    printf("   NativeRunninng: %d\n", GraphicService::GetInstance()->IsNativeRunning());
    printf("   IsScreenOn: %d\n", GraphicService::GetInstance()->IsScreenOn());
    printf("   GraphicTaskTriggeredCount: %d\n", GraphicService::GetInstance()->GetGraphicTaskTriggeredCount());
    printf("   LowPower:\n");
    printf("       Frequence: %d\n", GetFrequence());
    printf("       Task: total num: %u\n", TaskManager::GetInstance()->GetTotalTaskNum());
    printf("       Animator: total num: %u, active num: %u\n",
        AnimatorManager::GetInstance()->GetTotalAnimatorNum(),
        AnimatorManager::GetInstance()->GetActiveAnimatorNum());
    printf("       InputEvent Exist: %d\n", InputDeviceManager::GetInstance()->IsEventExist());
    printf("       PendingFrame: %d\n", LiteMGfxEngine::GetInstance()->HasPendingFrame());
    printf("       SufaceView Exist: %d\n", LowPowerManager::GetInstance()->IsLiteSurfaceInRootView());
}

void LeakPanic(int argc, char *argv[])
{
    if (argc != 2) { // 2: param num
        GRAPHIC_LOGE("param err.");
        return;
    }
    int16_t cnt = 0;
    int32_t sz = 0;
    char* end = nullptr;
    cnt = strtol(argv[0], &end, 0);
    if (end == argv[0]) {
        GRAPHIC_LOGE("first para is not a number.");
        return;
    }
    sz = strtol(argv[1], &end, 0);
    if (end == argv[1]) {
        GRAPHIC_LOGE("second para is not a number.");
        return;
    }
    MemCheck::GetInstance()->Crash(cnt, sz);
}

void DumpImgCmd(int argc, char *argv[])
{
    ImageCacheManager::GetInstance().Dump();

    printf("\n\n");
    printf("========================DumpImgInfoRecursive========================\n");
    RootView::GetInstance()->DumpImgInfoRecursive();
}

void SetMemMonitorCmd(int argc, char *argv[])
{
    if (argc != 1) {
        GRAPHIC_LOGE("param err.\n");
        return;
    }
    if (strcmp(argv[0], "0") == 0) {
        GrallocEngines::GetInstance()->SetMemMonitor(false);
    } else if (strcmp(argv[0], "1") == 0) {
        GrallocEngines::GetInstance()->SetMemMonitor(true);
    } else {
        GRAPHIC_LOGE("param err.\n");
        return;
    }
}
#endif

static bool ConvertToInt(const char* input, uint16_t& value)
{
    if (input == nullptr) {
        return false;
    }

    char* end = nullptr;
    int temp = strtol(input, &end, 0);
    if ((end == input) || (temp < 0)) {
        GRAPHIC_LOGE("Invalid input value: %s", input);
        return false;
    }
    value = static_cast<uint16_t>(temp);
    return true;
}

static void PrintScreencapInfo()
{
    printf("---GUI Screencap CMD---\n");
    printf("   -h: help, GUI screencap descriptions.\n");
    printf("   -v: viewId, obtains the capture of the view defined by viewId. eg. screencap -v viewId\n");
    printf("   -c: capture rect, obtains the capture of the input capture rect. eg. screencap -c x,y,w,h\n");
    printf("   -f: output file name, saves capture data to the file. eg. screencap -f /bin/vs/sd0p0/s.bin\n");
    printf("Note: -f should always be the last opt.\n");
}

bool HandleRectCapture(char* capRect)
{
    uint16_t left = 0;
    uint16_t top = 0;
    uint16_t width = 0;
    uint16_t height = 0;
    char* temp = nullptr;
    if (!ConvertToInt(strtok_r(capRect, ",", &temp), left) ||
        !ConvertToInt(strtok_r(nullptr, ",", &temp), top) ||
        !ConvertToInt(strtok_r(nullptr, ",", &temp), width) ||
        !ConvertToInt(strtok_r(nullptr, ",", &temp), height)) {
        GRAPHIC_LOGE("Please input capture rect in correct format: x,y,w,h");
        return false;
    }
    if (((left + width) > Screen::GetInstance().GetWidth()) ||
        ((top + height) > Screen::GetInstance().GetHeight())) {
        GRAPHIC_LOGE("Input rect exceeds the screen size. Screen: width = %u, height = %u.",
            Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        return false;
    }
    g_cropRect = {left, top, left + width - 1, top + height - 1};
    RootView::GetInstance()->SetScreencapFlag();
    return true;
}

bool SaveScreencap()
{
    if (g_filePath == nullptr) {
        GRAPHIC_LOGE("Filename is empty.");
        return false;
    }
    unlink(g_filePath);

#ifdef _WIN32
    char absolutePath[PATH_MAX] = {0};
    if (_fullpath(absolutePath, g_filePath, PATH_MAX) == nullptr) {
#else
    char* absolutePath = realpath(g_filePath, nullptr);
    bool checkOK = true;
    if (absolutePath == nullptr) {
#endif
        absolutePath = g_filePath;
        checkOK = false;
    }
    int32_t fd = open(absolutePath, O_RDWR | O_CREAT, DEFAULT_FILE_PERMISSION);
    UIFree(g_filePath);
    g_filePath = nullptr;
#ifndef _WIN32
    if (checkOK) {
        UIFree(absolutePath);
        absolutePath = nullptr;
    }
#endif
    ImageInfo info = {{0}};

    bool ret = false;
    if (fd < 0) {
        GRAPHIC_LOGE("open file failed!\n");
        goto END;
    }

    if (g_capView != nullptr) {
        if (g_capView->GetBitmap(info)) {
            ret = true;
        }
        g_capView = nullptr;
        goto END;
    }

    if (g_cropRect.IsValid()) {
        if (Screen::GetInstance().GetScreenBitmapByRect(info, &g_cropRect)) {
            ret = true;
        }
        g_cropRect = {0, 0, 0, 0};
    } else {
        if (Screen::GetInstance().GetScreenBitmapByRect(info, nullptr)) {
            ret = true;
        }
    }

END:
    if (ret && (write(fd, info.data, info.dataSize) != static_cast<int32_t>(info.dataSize))) {
        ret = false;
    }
    ImageCacheFree(info);

    if (fd >= 0) {
        close(fd);
    }
    printf("screencap result: %d.\n", ret);
    return ret;
}

void SetFilePath(const char* path)
{
    if (path == nullptr) {
        return;
    }
    if (g_filePath != nullptr) {
        UIFree(g_filePath);
        g_filePath = nullptr;
    }
    uint32_t pathLength = strlen(path);
    char* destPath = static_cast<char*>(UIMalloc(pathLength + 1));
    if (destPath == nullptr) {
        return;
    }

    if (memcpy_s(destPath, pathLength + 1, path, pathLength) != EOK) {
        UIFree(reinterpret_cast<void*>(destPath));
        return;
    }
    destPath[pathLength] = '\0';
    g_filePath = destPath;
}

void ScreencapCmd(int argc, char *argv[])
{
    if ((argc == 1) && (strcmp(argv[0], "-h") == 0)) {
        PrintScreencapInfo();
        return;
    }

    if ((argc == 2) && (strcmp(argv[0], "-f") == 0)) { // 2: param num
        SetFilePath(argv[1]);
        RootView::GetInstance()->SetScreencapFlag();
        return;
    }

    if ((argc == 4) && (strcmp(argv[2], "-f") == 0)) { // 4: param num, 2: index
        if ((strcmp(argv[0], "-v") == 0)) {
            if (strcmp(argv[1], "NULL") == 0) {
                g_capView = RootView::GetInstance();
                RootView::GetInstance()->SetScreencapFlag();
                SetFilePath(argv[3]); // 3: index
                return;
            } else if (RootView::GetInstance()->GetChildById(argv[1]) != nullptr) {
                g_capView = RootView::GetInstance()->GetChildById(argv[1]);
                RootView::GetInstance()->SetScreencapFlag();
                SetFilePath(argv[3]); // 3: index
                return;
            }
        }

        if (strcmp(argv[0], "-c") == 0) {
            char* capRect = argv[1];
            if (HandleRectCapture(capRect)) {
                SetFilePath(argv[3]); // 3: index
                return;
            }
        }
    }

    GRAPHIC_LOGE("Screencap failed. Please input valid string.");
    PrintScreencapInfo();
    return;
}

void ShowFPS(int argc, char *argv[])
{
    if (argc == 0) {
        FPSTask::GetInstance()->ShowFPS();
        return;
    }

    if ((argc == 1) && (strcmp(argv[0], "-h") == 0)) {
        printf("---GUI showfps [-p] [position]---\n");
        printf("   -h: help, GUI showfps descriptions.\n");
        printf("   -p: position, where fps are shown on screen, eg. showfps -p x,y\n");
        return;
    }

    if ((argc == 2) && (strcmp(argv[0], "-p") == 0)) { // 2: param num
        uint16_t left = 0;
        uint16_t top = 0;
        char* temp = nullptr;
        printf("ShowFPS %s", temp);
        if (!ConvertToInt(strtok_r(argv[1], ",", &temp), left) ||
            !ConvertToInt(strtok_r(nullptr, ",", &temp), top)) {
            GRAPHIC_LOGE("Please input position in correct value: x,y");
            return;
        }
        FPSTask::GetInstance()->ShowFPS(left, top);
    }
}

void ShowTime(int argc, char *argv[])
{
    if (argc == 1) {
        if (strcmp(argv[0], "tp") == 0) {
            DfxShowType(ShowType::DFX_SHOW_TP);
        } else if (strcmp(argv[0], "point") == 0) {
            DfxShowType(ShowType::DFX_SHOW_POINT);
        } else if (strcmp(argv[0], "close") == 0) {
            DfxShowType(ShowType::DFX_SHOW_DISABLE);
        }
    } else if (argc == 3) { // 3: argc
        if (strcmp(argv[0], "render") != 0) {
            printf("param err.");
            return;
        }
        uint16_t cnt;
        uint16_t invalid;
        if (!ConvertToInt(argv[1], cnt) || !ConvertToInt(argv[2], invalid)) {
            printf("param err.");
            return;
        }
        if (invalid != 0) {
            GraphicService::GetInstance()->PostGraphicEvent([]() {  RootView::GetInstance()->Invalidate(); });
        }
        DfxFrameTrace::GetInstance().EnableFrameTrace(static_cast<int>(cnt));
    } else {
        printf("param err.");
    }
}

void ForceRefresh(int argc, char *argv[])
{
    if (argc == 0) {
        GraphicService::GetInstance()->PostGraphicEvent([]() {  RootView::GetInstance()->Invalidate(); });
    } else {
        printf("param err.\n");
    }
}

void HideFPS(int argc, char *argv[])
{
    FPSTask::GetInstance()->HideFPS();
}

void PrintFPS(int argc, char *argv[])
{
    if (argc == 0) {
        FPSTask::GetInstance()->PrintFPS();
        return;
    }

    if (argc == 1) {
        if (strcmp(argv[0], "-h") == 0) {
            printf("---GUI printfps [duration]---\n");
            printf("   -h: help, GUI printfps descriptions.\n");
            printf("   duration: how long (in second) will be fps printed\n");
            return;
        }

        uint16_t duration = 0;
        if (ConvertToInt(argv[0], duration)) {
            FPSTask::GetInstance()->PrintFPS(duration);
        }
    }
}

static void PrintShowImageInfo()
{
        printf("---GUI showimg -r [rect] -f [res path] -i [resId] -m [mode]---\n");
        printf("---showimg,-r,0 0 100 100,-f,/user/res/xxx.bin,-i,0x100001,-m,1\n");
        printf("   -h: help, GUI showimg descriptions.\n");
        printf("   -r: [Required] img show rect, when the rect num is set to 4, disable the image view autosize.\n");
        printf("   -f: [Required] img res file.\n");
        printf("   -i: [Non-essential] img resID in package, use hexadecimal notation.\n");
        printf("   -m: [Non-essential] image view resize mode.\n");
}

void ShowImage(int argc, char *argv[])
{
    if (argc == 1 && strcmp(argv[0], "-h") == 0) {
        return PrintShowImageInfo();
    }

    std::unordered_map<std::string, std::string> argsMap;

    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg[0] == '-') {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                argsMap[arg] = argv[++i];
            } else {
                argsMap[arg] = "";
            }
        }
    }

    if (argsMap.find("-r") == argsMap.end()) {
        printf("param err,The -r parameter is missing.\n");
        return;
    }
    if (argsMap.find("-f") == argsMap.end()) {
        printf("param err,The -f parameter is missing.\n");
        return;
    }

    Graphic::Vector<std::string> tokens;
    std::string token;
    std::string rect = argsMap["-r"];
    for (char c : rect) {
        if (c == ',') {
            if (!token.empty()) {
                tokens.PushBack(token);
                token.clear();
            }
        } else {
            token += c;
        }
    }
    if (!token.empty()) {
        tokens.PushBack(token);
    }
    std::string file = argsMap["-f"];
    uint32_t resId = static_cast<uint32_t>(std::stoul(argsMap.find("-i") != argsMap.end() ? argsMap["-i"] : "0",
        nullptr, 16)); // 16:hexadecimal notation
    uint8_t mode = static_cast<uint8_t>(std::stoul(argsMap.find("-m") != argsMap.end() ? argsMap["-m"] : "0"));

    Rect imgRect;
    if (tokens.Size() == 4) { // 4: parameter number
        imgRect.SetPosition(std::stoi(tokens[0]), std::stoi(tokens[1]));
        imgRect.Resize(std::stoi(tokens[2]), std::stoi(tokens[3])); // 2, 3:index
        GraphicService::GetInstance()->PostGraphicEvent([imgRect, resId, file, mode]() {
            DfxImg::GetInstance()->Show(imgRect, resId, file, false, mode);});
    } else if (tokens.Size() == 2) { // 2: parameter number
        imgRect.SetPosition(std::stoi(tokens[0]), std::stoi(tokens[1]));
        GraphicService::GetInstance()->PostGraphicEvent([imgRect, resId, file, mode]() {
            DfxImg::GetInstance()->Show(imgRect, resId, file, true, mode);});
    } else {
        printf("param err,The -r parameter is error, need 2 or 4 num.\n");
        return;
    }
}

void HideImage(int argc, char *argv[])
{
    GraphicService::GetInstance()->PostGraphicEvent([]() {
        DfxImg::GetInstance()->Hide();});
}

void SetFreq(int argc, char *argv[])
{
    if (argc != 1) {
        printf("Invalid format!");
        return;
    }

    uint16_t freq = 0;
    if (ConvertToInt(argv[0], freq)) {
        SetFrequence(freq);
        printf("Set Frequence: %u", freq);
    }
}

#if SUPPORT_OHOSFWK
void RotateScreen(int argc, char *argv[])
{
    if (argc == 1) {
        AppRotationType rotation = AppRotationType::ROTATION_0;
        if (strcmp(argv[0], "0") == 0) {
            rotation = AppRotationType::ROTATION_0;
        } else if (strcmp(argv[0], "1") == 0) {
            rotation = AppRotationType::ROTATION_90;
        } else if (strcmp(argv[0], "2") == 0) {
            rotation = AppRotationType::ROTATION_180;
        } else if (strcmp(argv[0], "3") == 0) {
            rotation = AppRotationType::ROTATION_270;
        } else {
            printf("Invalid value. Valid range should be [0, 3].");
            return;
        }
        RotationManager::GetInstance().SetDisplayRotation(rotation);
    } else {
        printf("param err.\n");
    }
}
#endif

#if ENABLE_DEBUG
void InjectRotateEvent(int argc, char *argv[])
{
    if (argc != 1) {
        printf("Invalid input. Please input in correct format: rotate.");
        return;
    }

    char* end = nullptr;
    int rotate = strtol(argv[0], &end, 0);
    if (end == argv[0]) {
        GRAPHIC_LOGE("Invalid input value: %s", argv[0]);
        return;
    }

    if (!OHOS::EventInjector::GetInstance()->IsEventInjectorRegistered(OHOS::EventDataType::ROTATE_TYPE)) {
        OHOS::EventInjector::GetInstance()->RegisterEventInjector(OHOS::EventDataType::ROTATE_TYPE);
    }
    OHOS::EventInjector::GetInstance()->SetRotateEvent(rotate);
}

void InjectKeyEvent(int argc, char *argv[])
{
    if (argc != 2) { // 2: param num
        printf("Invalid Input.Please input in correct format: keyId state.\n\
            Valid Options for keyId is [power, func].\n\
            Valid Options for state is [0, 1, 2]. 0: release. 1: press. 2: long press.\n");
        return;
    }

    ZliteKeyCode keyId;
    if (strcmp(argv[0], "power") == 0) {
        keyId = ZliteKeyCode::ZLITE_KEY_POWER;
    } else if (strcmp(argv[0], "func") == 0) {
        keyId = ZliteKeyCode::ZLITE_KEY_FUNC;
    } else {
        printf("Invalid KeyId. Valid Options [power, func].\n");
        return;
    }

    int16_t state;
    if (strcmp(argv[1], "0") == 0) {
        state = InputDevice::STATE_RELEASE;
    } else if (strcmp(argv[1], "1") == 0) {
        state = InputDevice::STATE_PRESS;
    } else if (strcmp(argv[1], "2") == 0) {
        state = InputDevice::STATE_LONG_PRESS;
    } else {
        printf("Invalid State. Valid Options [0, 1, 2]. 0: release. 1: press. 2: long press.\n");
        return;
    }

    if (!OHOS::EventInjector::GetInstance()->IsEventInjectorRegistered(OHOS::EventDataType::KEY_TYPE)) {
        OHOS::EventInjector::GetInstance()->RegisterEventInjector(OHOS::EventDataType::KEY_TYPE);
    }
    OHOS::EventInjector::GetInstance()->SetKeyEvent(static_cast<uint16_t>(keyId), state);
}

void InjectClickEvent(int argc, char *argv[])
{
    if (argc != 2) { // 2: param num
        printf("Invalid Input. Please input in correct format: x y.\n");
        return;
    }

    uint16_t x;
    uint16_t y;
    if (!ConvertToInt(argv[0], x) || !ConvertToInt(argv[1], y)) {
        printf("Invalid Input. Please input a non-zero coordinate values.\n");
        return;
    }
    Point point = {x, y};

    if (!OHOS::EventInjector::GetInstance()->IsEventInjectorRegistered(OHOS::EventDataType::POINT_TYPE)) {
        OHOS::EventInjector::GetInstance()->RegisterEventInjector(OHOS::EventDataType::POINT_TYPE);
    }
    OHOS::EventInjector::GetInstance()->SetClickEvent(point);
}

void InjectDragEvent(int argc, char *argv[])
{
    if (argc != 5) { // 5: param num
        printf("Invalid Input. Please input in correct format: StartX StartY EndX EndY duration[>=20].\n");
        return;
    }

    uint16_t startX;
    uint16_t startY;
    uint16_t endX;
    uint16_t endY;
    uint16_t time;
    if (!ConvertToInt(argv[0], startX) || !ConvertToInt(argv[1], startY) ||
        !ConvertToInt(argv[2], endX) || !ConvertToInt(argv[3], endY) || // 2, 3: index
        !ConvertToInt(argv[4], time)) { // 4: index
        printf("Invalid Input. Please input a non-zero coordinate values.\n");
        return;
    }
    Point start = {startX, startY};
    Point end = {endX, endY};

    if (!OHOS::EventInjector::GetInstance()->IsEventInjectorRegistered(OHOS::EventDataType::POINT_TYPE)) {
        OHOS::EventInjector::GetInstance()->RegisterEventInjector(OHOS::EventDataType::POINT_TYPE);
    }
    OHOS::EventInjector::GetInstance()->SetDragEvent(start, end, time);
}

void InjectLongPressEvent(int argc, char *argv[])
{
    if (argc != 2) { // 2: param num
        printf("Invalid Input. Please input in correct format: x y.\n");
        return;
    }

    uint16_t x;
    uint16_t y;
    if (!ConvertToInt(argv[0], x) || !ConvertToInt(argv[1], y)) {
        printf("Invalid Input. Please input a non-zero coordinate values.\n");
        return;
    }
    Point point = {x, y};

    if (!OHOS::EventInjector::GetInstance()->IsEventInjectorRegistered(OHOS::EventDataType::POINT_TYPE)) {
        OHOS::EventInjector::GetInstance()->RegisterEventInjector(OHOS::EventDataType::POINT_TYPE);
    }
    OHOS::EventInjector::GetInstance()->SetLongPressEvent(point);
}

void CalcSampleUiHash(int argc, char *argv[])
{
    if (argc < 1) {
        printf("Invalid Input. Please input a coordinate values.\n");
        return;
    }
    std::string viewId(argv[0]);
    GraphicService::GetInstance()->PostGraphicEvent([] {
        OHOS::RootView::GetInstance()->Invalidate();
    });
    GraphicService::GetInstance()->PostGraphicEvent([viewId] {
        OHOS::ImageInfo info = {0};
        OHOS::Screen::GetInstance().GetScreenBitmapByRect(info, nullptr, true); // 1.0: scale factor
        
        // mbedtls_sha256 方法要求32位
        unsigned char* hash = new unsigned char[32];
        uapi_drv_cipher_sha256(info.data, info.dataSize, hash);
        ImageCacheFree(info);

        // 65: 32*2+1，32位hash数组每位按照16进制输出长度为2,再加上一个结束符
        char* output =  new char[65];
        for (int i = 0; i < 32; i++) { // 32: 长度
            sprintf_s(output + (i * 2), 3, "%02x", hash[i]); // 2:位; 3:多一个结束符
        }
        printf("screen hash value of %s=[%s] \n", viewId.c_str(), output);
        
        delete[] hash;
        delete[] output;
    });
   
    return;
}
#endif

#if ENABLE_DFX_TRACEUI
#define TRACEUI_RECORDING_DEFAULT_SIZE (128 * 1024)
#define TRACEUI_OUTPUT_TRACE_PATH "/user/res/1.trace"

static bool CheckWriteFilePathValid(const char *path)
{
    if (path == NULL) {
        return false;
    }
    if (path[0] != '/') {
        return false;
    }
    if (strstr(path, "/../") != NULL) {
        return false;
    }
    return true;
}

/*
 * argv[0]: size of recording.
 */
void TraceuiStart(int argc, char *argv[])
{
    uint32_t bufSize = TRACEUI_RECORDING_DEFAULT_SIZE;
    if (argc >= 1) {
        uint16_t kb;
        if (ConvertToInt(argv[0], kb)) {
            bufSize = kb * 1024; // 1024: 1kb
        } else {
            printf("Parsing record size filed! %s\n", argv[0]);
            return;
        }
    }

    printf("traceui begin (bufSize: %u) ...\n", bufSize);
    g_traceuiBuf = static_cast<uint8_t *>(UIMalloc(bufSize));
    if (g_traceuiBuf == NULL) {
        printf("malloc(%u) error!\n", bufSize);
        return;
    }
    if (!TraceuiRecordStart(TRACEUI_EVENTS_ALL, g_traceuiBuf, bufSize)) {
        printf("traceui start error!\n");
        return;
    }
}

/*
 * argv[0]: output path.
 */
void TraceuiStop(int argc, char *argv[])
{
    uint32_t size;
    bool isSuccess;
    const char *outPath = TRACEUI_OUTPUT_TRACE_PATH;
    if (g_traceuiBuf == NULL) {
        printf("traceui not start!\n");
        return;
    }
    size = TraceuiRecordStop();
    if (argc >= 1) {
        outPath = argv[0];
        if (!CheckWriteFilePathValid(outPath)) {
            printf("path check error: %s, please stop again!\n", outPath);
            return;
        }

        int32_t fd = open(outPath, O_RDWR | O_CREAT, DEFAULT_FILE_PERMISSION);
        if (fd < 0) {
            printf("file path error, please stop again!\n");
            return;
        }
        close(fd);
    }

    printf("record size: %u, start convert ...\n", size);
    isSuccess = TraceuiConvertToPerfettoProto(g_traceuiBuf, size, outPath);
    UIFree(reinterpret_cast<void*>(g_traceuiBuf));
    g_traceuiBuf = NULL;
    if (isSuccess) {
        printf("traceui record success: %s\n", outPath);
    }
}
#endif // ENABLE_DFX_TRACEUI
}

struct DfxCmd {
    const char* cmdType;
    void (*func)(int argc, char *argv[]);
};

struct DfxCmd g_dfxCmdList[] = {
    {"screencap", OHOS::ScreencapCmd},
    {"dump_view_id", OHOS::DumpViewIdCmd},
    {"dump_root_view", OHOS::DumpRootView},
    {"setfreq", OHOS::SetFreq},
    {"printfps", OHOS::PrintFPS},
    {"hidefps", OHOS::HideFPS},
    {"showfps", OHOS::ShowFPS},
    {"showtime", OHOS::ShowTime},
    {"refresh", OHOS::ForceRefresh},
    {"showimg", OHOS::ShowImage},
    {"hideimg", OHOS::HideImage},
#if ENABLE_MEMORY_CHECK
    {"dumpstate", OHOS::DumpState},
    {"dumpmem", OHOS::DumpMemoryCmd},
    {"mem_monitor", OHOS::SetMemMonitorCmd},
    {"dumpimg", OHOS::DumpImgCmd},
    {"leak_panic", OHOS::LeakPanic},
#endif
#if ENABLE_DEBUG
    {"inject_rotate", OHOS::InjectRotateEvent},
    {"inject_key", OHOS::InjectKeyEvent},
    {"inject_click", OHOS::InjectClickEvent},
    {"inject_drag", OHOS::InjectDragEvent},
    {"inject_lp", OHOS::InjectLongPressEvent},
    {"calc_hash", OHOS::CalcSampleUiHash},
#endif
#if ENABLE_DFX_TRACEUI
    {"traceui_start", OHOS::TraceuiStart},
    {"traceui_stop", OHOS::TraceuiStop},
#endif
#ifdef SUPPORT_OHOSFWK
    {"rotate", OHOS::RotateScreen},
#endif
};

bool ExecUikitDfxByCmdType(int argc, char *argv[])
{
    uint16_t i;

    if (argc < 1) {
        printf("ExecGuiDfx Invalid Input, argc: %d\n", argc);
        return false;
    }

    for (i = 0; i < sizeof(g_dfxCmdList) / sizeof(g_dfxCmdList[0]); i++) {
        if (!strcmp(g_dfxCmdList[i].cmdType, argv[0])) {
            if (strcmp(argv[0], "dumpstate") != 0) {
                OHOS::LowPowerManager::GetInstance()->ExitLowPower();
            }
            g_dfxCmdList[i].func(argc - 1, &argv[1]);
            return true;
        }
    }

    printf("ExecRegDfxCmd cmd not found, argv[0]: %s\n", argv[0]);
    return false;
}

void InitDfxCmd()
{
#if SUPPORT_OS_CMD
    OsCmdReg("guidfx", "gui dfx", (CmdCallBackFunc)ExecUikitDfxByCmdType);
#endif
    printf("====InitDfxCmd===\n");
    return;
}

bool SaveScreencapToFile()
{
    return OHOS::SaveScreencap();
}

#else // ENABLE_DFX_CMD
void InitDfxCmd()
{
    return;
}

bool SaveScreencapToFile()
{
    return true;
}

bool ExecUikitDfxByCmdType(int argc, char *argv[])
{
    return true;
}
#endif // ENABLE_DFX_CMD


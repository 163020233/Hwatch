/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "camera_test.h"
#include "camera_sample.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::Media;

constexpr char CAMERA_VIDEO_PATH[] = "/user/cam_record.mjpg";
constexpr char CAMERA_CAPTURE_PATH[] = "/user/cam_capture.jpg";

namespace {

CameraTest *g_CameraTest;
int g_CameraTestInitCount = 0;

void CameraTestInit(void)
{
    if (g_CameraTestInitCount == 0 || g_CameraTest == nullptr) {
        g_CameraTest = new CameraTest();
        if (g_CameraTest == nullptr) {
            CAM_ERR("no memory!\n");
            return;
        }
        g_CameraTest->CameraInit();
        g_CameraTestInitCount = 0;
    }
    g_CameraTestInitCount++;
}

void CameraTestDeinit(void)
{
    g_CameraTestInitCount--;
    if (g_CameraTestInitCount <= 0) {
        g_CameraTest->CameraDeinit();
        delete g_CameraTest;
        g_CameraTest = nullptr;
        g_CameraTestInitCount = 0;
    }
}

int CameraTestStart(int argc, char *argv[])
{
    CameraTestInit();
    if (g_CameraTest == nullptr) {
        CAM_ERR("camera not start!\n");
        return -1;
    }
    g_CameraTest->StartPreview();
    return 0;
}

int CameraTestStop(int argc, char *argv[])
{
    if (g_CameraTest == nullptr) {
        CAM_ERR("camera not start!\n");
        return -1;
    }
    g_CameraTest->StopPreview();
    CameraTestDeinit();
    return 0;
}

int CameraTestRecordStart(int argc, char *argv[])
{
    CameraTestInit();
    if (g_CameraTest == nullptr) {
        CAM_ERR("camera not start!\n");
        return -1;
    }

    const char *path = CAMERA_VIDEO_PATH;
    if (argc > 1) {
        if (strcmp(argv[0], "notsave") == 0) {
            path = nullptr;
            CAM_INFO("record not save\n");
        }
    }
    g_CameraTest->StartRecord(path);
    return 0;
}

int CameraTestRecordStop(int argc, char *argv[])
{
    if (g_CameraTest == nullptr) {
        CAM_ERR("camera not start!\n");
        return -1;
    }

    g_CameraTest->StopRecord();
    CameraTestDeinit();
    CAM_INFO("output file: %s\n", CAMERA_VIDEO_PATH);
    return 0;
}

int CameraTestCapture(int argc, char *argv[])
{
    CameraTestInit();
    if (g_CameraTest == nullptr) {
        CAM_ERR("camera not start!\n");
        return -1;
    }

    g_CameraTest->Capture(CAMERA_CAPTURE_PATH);
    CameraTestDeinit();
    CAM_INFO("output file: %s\n", CAMERA_CAPTURE_PATH);
    return 0;
}

using CameraSampleCmdFunc = int (*)(int argc, char *argv[]);

typedef struct {
    const char *name;
    CameraSampleCmdFunc func;
} CameraSampleCmd;

CameraSampleCmd g_Cmds[] = {
    {"preview_start", CameraTestStart},
    {"preview_stop", CameraTestStop},
    {"video_start", CameraTestRecordStart},
    {"video_stop", CameraTestRecordStop},
    {"capture", CameraTestCapture},
};

void CameraSampleHelp(void)
{
    CAM_INFO("support list:\n");
    for (unsigned int i = 0; i < sizeof(g_Cmds) / sizeof(g_Cmds[0]); i++) {
        CAM_INFO(" %s\n", g_Cmds[i].name);
    }
}

} // namespace


int32_t CameraSample(uint8_t argc, char *argv[])
{
    if (argc < 1) {
        CAM_ERR("argc error: %d\n", argc);
        CameraSampleHelp();
        return -1;
    }
    for (unsigned int i = 0; i < sizeof(g_Cmds) / sizeof(g_Cmds[0]); i++) {
        if (strcmp(g_Cmds[i].name, argv[0]) != 0) {
            continue;
        }
        if (argc - 1 == 0) {
            return g_Cmds[i].func(0, nullptr);
        } else {
            return g_Cmds[i].func(argc - 1, &argv[1]);
        }
    }
    CAM_ERR("unknown cmd: %s\n", argv[0]);
    CameraSampleHelp();
    return -1;
}

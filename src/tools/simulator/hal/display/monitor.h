/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd..
 * Description: simulator display driver implements
 */

#ifndef MONITOR_H
#define MONITOR_H

#include "config.h"

namespace OHOS {
enum class DeviceShape {
    SHAPE_RECT = 0,
    SHAPE_CIRCLE,
    SHAPE_UNSUPPORT,
};

int SdlRefresh(void *param);
bool SdlExitStatus(void);
void DeinitSdl(void);
void InitSdl(void);
void SdlEventDeal(void);
void MonitorInit(uint32_t width, uint32_t height, DeviceShape shape);
void MonitorFlush();
void UpdatePoint(int16_t x, int16_t y);
float GetScale();
unsigned int* GetFramBuff();
unsigned int GetContentHeight();
unsigned int GetContentWidth();
unsigned int GetMonitorHeight();
unsigned int GetMonitorWidth();
}  // namespace OHOS
#endif /* MONITOR_H */

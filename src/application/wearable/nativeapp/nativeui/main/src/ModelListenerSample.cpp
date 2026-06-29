/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PullUpDownControls
 * Created: 2025-11-05
 */

#include "main/ModelListenerSample.h"

#include "wearable_log.h"
namespace OHOS {
ModelListenerSample::ModelListenerSample()
{
    Init();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ModelListenerSample::ModelListenerSample");
}
ModelListenerSample::~ModelListenerSample()
{
    Deinit();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ModelListenerSample::~ModelListenerSample");
}

void ModelListenerSample::Callback()
{
    Notify();
    OnTickEvent();
}
}

/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
* Description: PullUpDownModel
* Create: 2026-01-08
*/
#include "securec.h"
#include "wearable_log.h"
#include "iostream"
#include "graphic_mutex.h"
#include "main/PullUpDownModel.h"

namespace OHOS {
PullUpDownModel::PullUpDownModel()
{
}

PullUpDownModel::~PullUpDownModel()
{
}

PullUpDownModel *PullUpDownModel::GetInstance(void)
{
    static PullUpDownModel PullUpDownModel;
    return &PullUpDownModel;
}

int PullUpDownModel::GetBriScrStatus(void)
{
    return briScreenStatus;
}

int PullUpDownModel::GetDntDistStatus(void)
{
    return dntDisturbStatus;
}

void PullUpDownModel::UpdateBriScrStatus(uint16_t status)
{
    briScreenStatus = status;
}

void PullUpDownModel::UpdateDntDistStatus(uint16_t status)
{
    dntDisturbStatus = status;
}
}

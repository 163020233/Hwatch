/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Reboot Model
 * Create: 2022-04-28
 */

#include "rebootdevice/RebootModel.h"

#define REBOOT_WAIT_SCREEN_OFF_TIME 200
namespace OHOS {
RebootModel::RebootModel() {}

RebootModel::~RebootModel() {}

RebootModel *RebootModel::GetInstance(void)
{
    static RebootModel instance;
    return &instance;
}

int16_t RebootModel::GetSelectView(void)
{
    return selectView;
}

void RebootModel::SetSelectViewId(int16_t value)
{
    selectView = value;
}

bool RebootModel::GetExitsView(void)
{
    return exitsView;
}

void RebootModel::SetExitsView(bool value)
{
    exitsView = value;
}

void RebootModel::SoftRebootDevice(void)
{
}

void RebootModel::ShutdownDevice(void)
{
}

}
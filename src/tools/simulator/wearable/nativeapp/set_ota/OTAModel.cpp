/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * Description: ota local update
 * Author: wangle
 * Create: 2025-5-6
 */
#include "set_ota/OTACommon.h"
#include "set_ota/OTAModel.h"
#include "set_ota/OTAPresenter.h"
#include "tcxo.h"
#include "wearable_log.h"


namespace OHOS {
OTAModel::OTAModel()
{
    isIdel = true;
    wait.wait = nullptr;
}
OTAModel::~OTAModel()
{
}

int OTAModel::WaitCondition(const void *data)
{
    OTAModel *model = (OTAModel *)data;
    return model->isIdel ? 0 : 1;  // 不空闲说明有任务
}

int OTAModel::OSALKthreadCallback(void *data)
{
    return 0;
}
bool OTAModel::ModelRunTask(TaskType tasktype, void *taskdata)
{
    return true;
}
void OTAModel::SwitchTask()
{
}

void OTAModel::UpdateInfo(void *data)
{
}
void OTAModel::LocalUpdate(void *data)
{
}
void OTAModel::DelayRebootFor5S()
{
}
}  // namespace OHOS

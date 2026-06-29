/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2021. All rights reserved.
 * Description: camera device base impl
 * Author: media_develop team
 * Create: 20211117
 */

#include "camera_device_base.h"
using namespace std;

namespace OHOS {
namespace Camera {

CameraDeviceBase::CameraDeviceBase(uint32_t id)
    : m_id(id)
{
    HwCameraDeviceOpen(m_id);
}

CameraDeviceBase::~CameraDeviceBase()
{
    HAL_LOGI("\n");
    HwCameraDeviceClose(m_id);
}

RetCode CameraDeviceBase::GetStreamMng(std::shared_ptr<StreamMng> &streamMng)
{
    streamMng = make_shared<StreamMng>(m_id);
    return SUCCESS;
}

RetCode CameraDeviceBase::UpdateSettings(const void *value, int size)
{
    (void)size;
    DeviceInfo info;
    info.type = CAMERA_CONTROL_PRIVATE;
    info.u.privateData = value;
    HwCameraDeviceSetInfo(m_id, &info);
    return SUCCESS;
}

} // namespace Camera
} // namespace OHOS
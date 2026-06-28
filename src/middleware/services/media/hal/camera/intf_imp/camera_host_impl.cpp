/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2026. All rights reserved.
 * Description: camera host impl
 * Create: 2025-08-09
 */

#include "camera_host_impl.h"
#include "camera_device_impl.h"
using namespace std;

namespace OHOS {
namespace Camera {
class ICameraDevice;
class ICameraHostCallback;
class ICameraDeviceCallback;

CameraHostImpl::CameraHostImpl()
{
    static CameraDeviceMng devMng;
    m_devMng = &devMng;
    Init();
}

CameraHostImpl::~CameraHostImpl()
{
    HAL_LOGI("");
}

CamRetCode CameraHostImpl::Init()
{
    return (CamRetCode)(m_devMng->Init());
}

sptr<ICameraHost> ICameraHost::Get(const char *serviceName)
{
    (void)serviceName;
    static shared_ptr<ICameraHost> host = make_shared<CameraHostImpl>();
    return host;
}

CamRetCode CameraHostImpl::SetCallback(const OHOS::sptr<ICameraHostCallback> &callback)
{
    HW_HAL_UNUSED(callback);
    return (CamRetCode)(m_devMng->SetDeviceStatusCb());
}

CamRetCode CameraHostImpl::GetCameraIds(std::vector<std::string> &cameraIds)
{
    HW_HAL_UNUSED(cameraIds);
    return (CamRetCode)(m_devMng->GetCameraIds(cameraIds));
}

CamRetCode CameraHostImpl::GetCameraAbility(const std::string &cameraId,
    std::shared_ptr<CameraAbility> &ability)
{
    HW_HAL_UNUSED(cameraId);
    HW_HAL_UNUSED(ability);
    return NO_ERROR;
}

CamRetCode CameraHostImpl::OpenCamera(const std::string &cameraId,
    const OHOS::sptr<ICameraDeviceCallback> &callback,
    OHOS::sptr<ICameraDevice> &device)
{
    HW_HAL_UNUSED(callback);
    std::shared_ptr<CameraDeviceBase> devBase = nullptr;
    m_devMng->OpenCamera(cameraId, devBase);
    device = make_shared<CameraDeviceImpl>(cameraId, devBase);
    return NO_ERROR;
}

CamRetCode CameraHostImpl::SetFlashlight(const std::string &cameraId, bool &isEnable)
{
    HW_HAL_UNUSED(cameraId);
    HW_HAL_UNUSED(isEnable);
    return NO_ERROR;
}
} // namespace Camera
} // namespace OHOS
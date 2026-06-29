/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Native Start
 * Author: Hisi Graphic Team
 * Created: 2025-4
 */
#include "NativeAbility.h"
#include "NativeLauncher.h"
#include "StartNative.h"
#include "graphic_service_wrapper.h"
#include "adapter.h"

void StartNative()
{
    OHOS::NativeLauncher::GetInstance().InitNativeLauncher();
    HideGraphicLogo();
    Want *info = static_cast<Want *>(AdapterMalloc(sizeof(Want)));
    memset_s(info, sizeof(Want), 0, sizeof(Want));
    OHOS::NativeAbility::GetInstance().OnActive(*info);
    AdapterFree(info);
}

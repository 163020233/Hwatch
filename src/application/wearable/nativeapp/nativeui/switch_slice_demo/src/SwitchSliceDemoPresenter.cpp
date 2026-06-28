/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: SwitchSliceDemoPresenter
 * Create: 2026-02-12
 */

#include "NativeRegisterManager.h"
#include "switch_slice_demo/SwitchSliceDemoView.h"
#include "switch_slice_demo/SwitchSliceDemoPresenter.h"

namespace OHOS {
REGIST_MENU(VIEW_SWITCH_SLICE_DEMOA, SwitchSliceDemoView, SwitchSliceDemoPresenter, EMPTY_ICON, EMPTY_ICON, "Slice跳转测试");
REGIST_SLICE(VIEW_SWITCH_SLICE_DEMOB, SwitchSliceDemoView, SwitchSliceDemoPresenter);
REGIST_SLICE(VIEW_SWITCH_SLICE_DEMOC, SwitchSliceDemoView, SwitchSliceDemoPresenter);
REGIST_SLICE(VIEW_SWITCH_SLICE_DEMOD, SwitchSliceDemoView, SwitchSliceDemoPresenter);

}

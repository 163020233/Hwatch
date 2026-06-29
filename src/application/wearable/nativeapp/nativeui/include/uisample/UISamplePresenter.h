/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#ifndef UISAMPLE_PRESENTER_H
#define UISAMPLE_PRESENTER_H

#include "Presenter.h"
#include "components/ui_view.h"
#include "uisample/UISampleView.h"
#include "uisample/UISampleModel.h"
#include "uiservice/ui_service.h"
#include "NativeRegisterManager.h"

namespace OHOS {
class UISampleView;
class UISampleModel;
class UISamplePresenter : public Presenter<UISampleView> {
public:
    UISamplePresenter();

    void OnResume() override
    {
        SendMsgToUIService(UI_ADD_SLEEP_VETO, 0, nullptr, 0);
        return;
    }
    void OnPause() override
    {
        SendMsgToUIService(UI_RM_SLEEP_VETO, 0, nullptr, 0);
        return;
    }

private:
    UISampleModel *uisampleModel_;
};
}

#endif // UISAMPLE_PRESENTER_H
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: voice assistant
 * Author: CompanyName
 * Create: 2022-01-18
 */

#include "offlinevoice/MainVocassistView.h"
#include "offlinevoice/VocassistPresenter.h"

namespace OHOS {
static MainVocassistView *g_pMainVocassistView = nullptr;

MainVocassistView::MainVocassistView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[MainVocassistView] MainVocassistView");
    g_pMainVocassistView = this;
}

MainVocassistView::~MainVocassistView()
{
#ifdef TEMP_DEPENDENCY
    LocalAsrStop();
#endif
    if (vocassistance != nullptr) {
        delete vocassistance;
        vocassistance = nullptr;
    }
    g_pMainVocassistView = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[MainVocassistView] ~MainVocassistView");
}

MainVocassistView *MainVocassistView::GetInstance()
{
    return g_pMainVocassistView;
}

void MainVocassistView::OnStart()
{
    vocassistance = new VocassistView();
    if (vocassistance == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "VocassistView::GetInstance is nullptr!!");
        return;
    }
    if (VocassistPresenter::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "VocassistPresenter::GetInstance is nullptr!!");
        return;
    }
    draglistener = static_cast<UIView::OnDragListener*>(VocassistPresenter::GetInstance());
    if (draglistener == nullptr) {
        return;
    }
    vocassistance->SetViewId(VOC_ID);
    vocassistance->SetDraggable(true);
    vocassistance->SetTouchable(true);
#ifdef TEMP_DEPENDENCY
    vocassistance->SetOnDragListener(draglistener);
#endif
    vocassistance->InitView(this);
    AddViewToRootContainer(vocassistance);

#ifdef TEMP_DEPENDENCY
    if (VocassistModel::GetInstance()->GetPhoneDetailStatus() != ONE_OR_MORE_ROAD_AND_OTHER_STATUS) {
        AsrWorkMode curMode = LocalAsrGetMode();
        LocalAsrSetMode(curMode);
    }
#endif

    // 六秒钟未操作分两步退出语音助手，波形图标先转化为语音助手图标，再退出语音助手
    if (VocassistView::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "VocassistView GetInstance() is nullptr!!");
        return;
    }

    VocassistModel::GetInstance()->SetExitStatus(CONTINUTE_EXIT_MODE2);
    VocassistView::GetInstance()->DelayDisplayTime(DELAY_SIX_SECONDS);
}

void MainVocassistView::OnStop()
{
    VocassistModel::GetInstance()->UnInit();
    return;
}

VocassistView *MainVocassistView::GetVocassist()
{
    return vocassistance;
}

}
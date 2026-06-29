/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: IntelligentVoiceView.cpp
 * Create: 2025-06
 */

#include "intelligent/IntelligentVoiceView.h"

namespace OHOS {

IntelligentVoiceView::IntelligentVoiceView()
{
    for (int i = 0; i < INANI_TOTAL_NUM; i++) {
        imageAnimatorInfo[i].imagePath = nullptr;
        imageAnimatorInfo[i].pos.x = 0;
        imageAnimatorInfo[i].pos.y = 0;
        imageAnimatorInfo[i].width = 0;
        imageAnimatorInfo[i].height = 0;
        imageAnimatorInfo[i].imageType = OHOS::ImageType::IMG_SRC_FILE_PATH;
    }
}

IntelligentVoiceView *IntelligentVoiceView::GetInstance()
{
    static IntelligentVoiceView instance;
    return &instance;
}

void IntelligentVoiceView::InitView()
{
    SetStyle(STYLE_BACKGROUND_OPA, 100); // opa 100
    SetDraggable(false);
    voicePhoto = new UIImageAnimatorView();
    if (voicePhoto == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "VocassistView : failed to Create vociePhoto!!");
        return;
    }
    voicePhoto->SetPosition(0, 0, INVOCASSIST_WIDTH, INVOCASSIST_HEIGHTH);
    voicePhoto->SetRepeat(true);
    voicePhoto->SetVisible(true);
    voicePhoto->SetSizeFixed(true);
    voicePhoto->SetViewId(INVOCPHOTO_ID);
    int ret;
    (void)memset_s(storPicPathBuf, sizeof(storPicPathBuf), 0, sizeof(storPicPathBuf));
    for (int i = 0; i < INANI_TOTAL_NUM; i++) {
        ret = sprintf_s(storPicPathBuf[i], sizeof(storPicPathBuf[i]), RES_PATH "/VOICE_ASSISTANT_%02d.bin", i + 1);
        if (ret == ERROR_RETVAL) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Splice failed!!");
            return;
        }
        imageAnimatorInfo[i].imagePath = (const char *)storPicPathBuf[i];
        imageAnimatorInfo[i].pos.x = INANI_X;
        imageAnimatorInfo[i].pos.y = INANI_Y;
        imageAnimatorInfo[i].width = INANI_SL;
        imageAnimatorInfo[i].height = INANI_SL;
        imageAnimatorInfo[i].imageType = IMG_SRC_FILE_PATH;
    }
    Add(voicePhoto);
    SetVisible(true);
    SetPosition(INVOCASSIST_XLABEL, INVOCASSIST_YLABEL, INVOCASSIST_WIDTH, INVOCASSIST_HEIGHTH);
    voicePhoto->SetVisible(true);
    isInitView_ = true;
}

void IntelligentVoiceView::ShowView()
{
    WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Show View");
    if (!isInitView_) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Init View");
        InitView();
    }
    isShowView_ = true;
    if (voicePhoto == nullptr) {
        return;
    }
    voicePhoto->SetImageAnimatorSrc(imageAnimatorInfo + INWAVEFORM_CHANGE_START,
        INWAVEFORM_CHANGE_PICTURE_TOTAL,
        INWAVEFORM_CHANGE_PICTURE_INTERVAL);
    voicePhoto->SetRepeat(true);
    voicePhoto->Start();
    RootView::GetInstance()->AddSystemView(this);
    Invalidate();
}

void IntelligentVoiceView::ExitView()
{
    if (voicePhoto == nullptr) {
        return;
    }
    WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Exit View");
    if (voicePhoto != nullptr) {
        voicePhoto->Stop();
    }
    RootView::GetInstance()->RemoveSystemView(this);
    RemoveAll();
    isInitView_ = false;
    isShowView_ = false;
    if (voicePhoto != nullptr) {
        delete voicePhoto;
        voicePhoto = nullptr;
    }
}

IntelligentVoiceView::~IntelligentVoiceView()
{
    ExitView();
}
}  // namespace OHOS

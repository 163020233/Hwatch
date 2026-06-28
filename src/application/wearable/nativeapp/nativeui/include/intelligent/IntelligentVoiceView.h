/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: IntelligentVoiceView.h
 * Create: 2025-06
 */

#ifndef INTELLIGENT_VOICE_VIEW
#define INTELLIGENT_VOICE_VIEW
#include <string>
#include "components/ui_image_animator.h"
#include "UiConfig.h"
#include "wearable_log.h"
#include "main/AppGroupView.h"
#include "components/root_view.h"

namespace OHOS {
static constexpr const char INVOCPHOTO_ID[] = "INVOCPIC";
static constexpr uint16_t INSWITCH_MAP_NUM = 16;
static constexpr uint16_t INANI_X = 84;
static constexpr uint16_t INANI_Y = 108;
static constexpr uint16_t INANI_SL = 116;

static constexpr uint16_t INWAVEFORM_CHANGE_START = 9;
static constexpr uint16_t INWAVEFORM_CHANGE_PICTURE_TOTAL = 34;
static constexpr uint16_t INWAVEFORM_CHANGE_PICTURE_INTERVAL = 50;

static constexpr uint16_t INWAVEFORM_CHANGE_END = 51;
static constexpr uint16_t INACTIVE_VOCASSIST_PICTURE_TOTAL = 20;
static constexpr uint16_t INACTIVE_VOCASSIST_PICTURE_INTERVAL = 100;

static constexpr uint16_t INVOCASSIST_XLABEL = 180;
static constexpr uint16_t INVOCASSIST_YLABEL = 60;
static constexpr uint16_t INVOCASSIST_WIDTH = 80;
static constexpr uint16_t INVOCASSIST_HEIGHTH = 80;

constexpr int INANI_TOTAL_NUM = 71;
static constexpr int INPATH_LENGTH = 60;
static constexpr int16_t ERROR_RETVAL = -1;

class IntelligentVoiceView : public UIViewGroup {
public:
    ~IntelligentVoiceView() override;
    static IntelligentVoiceView *GetInstance();
    void ShowView();
    void ExitView();
    IntelligentVoiceView(const IntelligentVoiceView &) = delete;
    IntelligentVoiceView &operator=(const IntelligentVoiceView &) = delete;
    IntelligentVoiceView(IntelligentVoiceView &&) = delete;
    IntelligentVoiceView &operator=(IntelligentVoiceView &&) = delete;
private:
    IntelligentVoiceView();
    void InitView();
    bool isInitView_{false};
    bool isShowView_{false};
    UIImageAnimatorView *voicePhoto = nullptr;
    char storPicPathBuf[INANI_TOTAL_NUM][INPATH_LENGTH];
    ImageAnimatorInfo imageAnimatorInfo[INANI_TOTAL_NUM];
};

}  // namespace OHOS
#endif /* INTELLIGENT_VOICE_VIEW */

/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingLanguageModel
 * Created: 2025-12-15
 */

#ifndef SET_LANGUAGE_MODEL_H
#define SET_LANGUAGE_MODEL_H

#include <cstdint>
#include <map>
#include "UiConfig.h"
#include "settings/common/SettingCommon.h"

typedef enum {
    CHINESE,
    ENGLISH,
} LanguageStyle;

namespace OHOS {
    static const uint16_t SETTING_LANGUAGE_COUNT = 10;
    class  SettingLanguageModel {
    public:
        void InitSetLanguages();
        static  SettingLanguageModel &GetInstance(void);
        LanguageStyle GetLanguageStyle(void);
        void SetLanguageStyle(LanguageStyle style);
        static void ScanPackagesResultCallback(const uint8_t isSuccess, const void* resultMessage);
        const SetLanguages *GetSetLanguagelistItems() const;
        uint8_t GetSettingListNum() const;
    private:
         SettingLanguageModel();
         SettingLanguageModel(const  SettingLanguageModel &);
         SettingLanguageModel &operator=(const  SettingLanguageModel &);
        virtual ~ SettingLanguageModel();
        SetLanguages languageSettings[SETTING_LANGUAGE_COUNT] = { INVALID_ID, nullptr };
        int usedLen = 0;
        int nativeLanguage = 0;
    };
}
#endif // SET_Language_MODEL_H
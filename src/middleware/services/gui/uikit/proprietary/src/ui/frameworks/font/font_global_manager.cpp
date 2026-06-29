/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd. All rights reserved
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "font/font_global_manager.h"
#include <cstdlib>
#include "securec.h"
#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/mem_api.h"
#include "common/graphic_hardware_types.h"

#if ENABLE_FONT_VECTOR_GLOBAL
namespace OHOS {
StringsInfo::StringsInfo() {}

bool StringsInfo::Load()
{
    bool result = false;
    if (binInfo_ != nullptr) {
        delete binInfo_;
    }
    binInfo_ = new LangBinInfo();
    if (binInfo_ == nullptr) {
        GRAPHIC_LOGE("new LangBinInfo failed");
        return false;
    }

    if (fileName_ != nullptr) {
        result = binInfo_->Init(fileName_);
    } else {
#ifdef VERSION_SOCMN2
        result = binInfo_->Init(langId_);
#endif
    }

    if (result == false) {
        UnLoad();
    }

    return result;
}

#ifdef VERSION_SOCMN2
bool StringsInfo::SetLangId(uint8_t langId)
{
    langId_ = langId;
    return true;
}
#endif

bool StringsInfo::SetFileName(const char* fileName)
{
    uint16_t fileNameLen = strlen(fileName);
    if (fileNameLen <= 0) {
        GRAPHIC_LOGE("the filename of language resource is invalid");
        return false;
    }
    if (fileName_ != nullptr) {
        UIFree(fileName_);
    }
    fileName_ = static_cast<char*>(UIMalloc(fileNameLen + 1));
    if (fileName_ == nullptr) {
        GRAPHIC_LOGE("memory is not enough.");
        return false;
    }
    if (memcpy_s(fileName_, fileNameLen, fileName, fileNameLen) != EOK) {
        GRAPHIC_LOGE("memcpy filename error!");
        UIFree(fileName_);
        fileName_ = nullptr;
        return false;
    }
    fileName_[fileNameLen] = '\0';
    return true;
}

void StringsInfo::UnLoad()
{
    if (binInfo_ != nullptr) {
        delete binInfo_;
        binInfo_ = nullptr;
    }
}

LangBinInfo* StringsInfo::GetBinInfo()
{
    return binInfo_;
}

StringsInfo::~StringsInfo()
{
    if (fileName_ != nullptr) {
        UIFree(fileName_);
    }
    if (binInfo_ != nullptr) {
        delete binInfo_;
    }
    fileName_ = nullptr;
    binInfo_ = nullptr;
}

LangBinInfo::LangBinInfo() : hasDefaultFontName(false) {}

LangBinInfo::~LangBinInfo()
{
    if (binHead_ != nullptr) {
        UIFree(binHead_);
    }
}

#ifdef VERSION_SOCMN2
bool LangBinInfo::Init(uint8_t langId)
{
    constexpr uint8_t STR_NUM_INDEX = 20;
    constexpr uint8_t STR_OFFSETS_INDEX = 22;

    /*
 * The resource mem layout is like this:

  -----------------------------------  begin  -----------------------------------

    MEM_RESOURCE_START   -----------------------------------------------------
        imageOffset                 uint32_t
        fontOffset                  uint32_t
        langOffset                  uint32_t
    imgPackHead   ------------------------------------------------------------
        ....                        ....
        ....                        ....
    fontPackHead  ------------------------------------------------------------
        ....                        ....
        ....                        ....
    langPackHead  ------------------------------------------------------------
        langNum                     uint8_t
        langResOffset[langNum]      uint32_t
        langSize[langNum]           uint32_t
            1.lang                      sizeof(1.lang)
            ....                        ....
            langNum.lang                sizeof(langNum.lang)

  ---------------------------------  end  ------------------------------------
 */

    uint8_t* binHead = static_cast<uint8_t*>((void *)MEM_RESOURCE_START());
    uint32_t langOffset = reinterpret_cast<uint32_t*>(binHead)[static_cast<int>(ResourceType::RESOURCE_LANGUAGE)];

    uint8_t langNum = *reinterpret_cast<uint8_t*>(binHead + langOffset);
    if (langId >= langNum) {
        GRAPHIC_LOGE("langId %u is invalid, langNum = %u", langId, langNum);
        return false;
    }
    
    uint32_t langResOffset = reinterpret_cast<uint32_t*>(binHead + langOffset + sizeof(uint8_t))[langId];
    totalStrsNum_ = (*reinterpret_cast<uint16_t*>(binHead + langResOffset + STR_NUM_INDEX)) & TOTAL_NUM_MASK;
    hasDefaultFontName = ((*reinterpret_cast<uint16_t*>(binHead + langResOffset + STR_NUM_INDEX)) &
        DEFAULT_FONTNAME_MASK) != 0;
    strOffsets_ = reinterpret_cast<uint32_t*>(binHead + langResOffset + STR_OFFSETS_INDEX);
    strData_ = reinterpret_cast<char*>(strOffsets_ + totalStrsNum_);

    return true;
}
#endif

bool LangBinInfo::Init(const char* fileName)
{
    bool result = false;
    constexpr uint8_t STR_NUM_INDEX = 20;
    constexpr uint8_t STR_OFFSETS_INDEX = 22;
    if (fileName == nullptr) {
        GRAPHIC_LOGE("the langbin filename is null");
        return result;
    }
#ifdef _WIN32
    char absolutePath[PATH_MAX] = {0};
    if (_fullpath(absolutePath, fileName, PATH_MAX) == nullptr) {
#else
    char* absolutePath = realpath(fileName, nullptr);
    if (absolutePath == nullptr) {
#endif
        GRAPHIC_LOGE("invalid path.");
        return result;
    }
    FILE* fp = fopen(absolutePath, "rb");
#ifndef _WIN32
    free(absolutePath);
    absolutePath = nullptr;
#endif
    if (fp == nullptr) {
        GRAPHIC_LOGE("file %s open failed", fileName);
        return result;
    }
    // get file length
    fseek(fp, 0, SEEK_END);
    uint32_t fileLen = static_cast<uint32_t>(ftell(fp));
    fseek(fp, 0, SEEK_SET);
    binHead_ = static_cast<uint8_t*>(UIMalloc(fileLen + 1));
    if (binHead_ == nullptr) {
        GRAPHIC_LOGE("can not malloc memory");
        result = false;
        goto RET;
    }
    if (fread(binHead_, 1, fileLen, fp) != fileLen) {
        GRAPHIC_LOGE("fread failed errno : %d", errno);
        result = false;
        goto RET;
    }
    binHead_[fileLen] = '\0';
    totalStrsNum_ = (*reinterpret_cast<uint16_t*>(binHead_ + STR_NUM_INDEX)) & TOTAL_NUM_MASK;
    hasDefaultFontName = ((*reinterpret_cast<uint16_t*>(binHead_ + STR_NUM_INDEX)) & DEFAULT_FONTNAME_MASK) != 0;
    strOffsets_ = reinterpret_cast<uint32_t*>(binHead_ + STR_OFFSETS_INDEX);
    strData_ = reinterpret_cast<char*>(strOffsets_ + totalStrsNum_);
    result = true;
RET:
    fclose(fp);
    if ((!result) && (binHead_ != nullptr)) {
        UIFree(binHead_);
        binHead_ = nullptr;
    }
    return result;
}

char* LangBinInfo::GetText(uint16_t textId)
{
    if (textId >= totalStrsNum_) {
        GRAPHIC_LOGE("textId %u is invalid, totalStrsNum_ = %u", textId, totalStrsNum_);
        return nullptr;
    }
    uint32_t strOffset = strOffsets_[textId];
    char* textAddr = strData_ + (strOffset & FONT_STR_MASK);
    return textAddr;
}

void LangBinInfo::GetFontParam(uint16_t textId, FontParam& prm)
{
    prm.direct = TEXT_DIRECT_MIXED + 1;
    if (textId >= totalStrsNum_) {
        GRAPHIC_LOGE("textId %u is invalid", textId);
        return;
    }

    uint32_t strOffset = strOffsets_[textId];
    prm.fontSize = (strOffset & FONT_SIZE_MASK) >> FONT_SIZE_BITS_OFFSET;
    prm.direct = (strOffset & FONT_DIRECT_MASK) >> FONT_DIRECT_OFFSET;
    if ((strOffset & FONTNAME_FLAG_MASK) != 0) {
        char* textAddr = strData_ + (strOffset & FONT_STR_MASK);
        uint32_t strLen = strlen(textAddr);
        prm.fontName = textAddr + strLen + 1;
    } else if (hasDefaultFontName) {
        prm.fontName = strData_;
    }
    return;
}

FontGlobalManager* FontGlobalManager::GetInstance()
{
    static FontGlobalManager instance;
    return &instance;
}

LangBinInfo* FontGlobalManager::GetLangBinInfo()
{
    if (currentLangId_ >= langNum_) {
        GRAPHIC_LOGE("language has not set");
        return nullptr;
    }
    LangBinInfo* langInfo = stringsInfos_[currentLangId_].GetBinInfo();
    return langInfo;
}

char* FontGlobalManager::GetText(uint16_t textId)
{
    LangBinInfo* langInfo = GetLangBinInfo();
    if (langInfo == nullptr) {
        GRAPHIC_LOGE("can not get the langbininfo");
        return nullptr;
    }
    return langInfo->GetText(textId);
}

FontParam FontGlobalManager::GetFontParam(uint16_t textId)
{
    FontParam prm = { 0 };
    prm.direct = TEXT_DIRECT_MIXED + 1;
    LangBinInfo* langInfo = GetLangBinInfo();
    if (langInfo == nullptr) {
        GRAPHIC_LOGE("can not get the langbininfo");
        return prm;
    }
    langInfo->GetFontParam(textId, prm);
    return prm;
}

bool FontGlobalManager::ReallocStringInfos(uint8_t langNum)
{
    if (stringsInfos_ != nullptr) {
        delete[] stringsInfos_;
    }
    stringsInfos_ = new StringsInfo[langNum];
    if (stringsInfos_ == nullptr) {
        GRAPHIC_LOGE("can not malloc memory %s", __FUNCTION__);
        return false;
    }
    langNum_ = langNum;
    return true;
}

uint8_t FontGlobalManager::GetTotalLangId()
{
    return langNum_;
}

FontGlobalManager::~FontGlobalManager()
{
    if (stringsInfos_ != nullptr) {
        delete[] stringsInfos_;
        stringsInfos_ = nullptr;
    }
}

void FontGlobalManager::UnRegisterLanguageInfo()
{
    if (stringsInfos_ != nullptr) {
        delete[] stringsInfos_;
        stringsInfos_ = nullptr;
    }
    langNum_ = 0;
    currentLangId_ = INVALID_LANG_ID;
}

#ifdef VERSION_SOCMN2
bool FontGlobalManager::RegisterLanguageInfo(uint8_t* langIds, uint8_t langNum, uint8_t defaultLangId)
{
    bool ret = true;
    if (langIds == nullptr) {
        GRAPHIC_LOGE("the resIdArray is nullptr");
        return false;
    }
    if (defaultLangId >= langNum) {
        GRAPHIC_LOGE("the default is bigger than the total");
        return false;
    }
    if (!ReallocStringInfos(langNum)) {
        return false;
    }
    // init the stringsInfos_
    for (uint8_t i = 0; i < langNum; i++) {
        stringsInfos_[i].SetLangId(langIds[i]);
        if (i == defaultLangId) {
            ret &= stringsInfos_[i].Load();
        }
    }
    currentLangId_ = defaultLangId;
    return ret;
}
#endif

bool FontGlobalManager::RegisterLanguageInfo(const char** langFile, uint8_t langNum, uint8_t defaultLangId)
{
    bool ret = true;
    if (langFile == nullptr) {
        GRAPHIC_LOGE("the resFile is nullptr");
        return false;
    }
    if (defaultLangId >= langNum) {
        GRAPHIC_LOGE("the default is bigger than the total");
        return false;
    }
    if (!ReallocStringInfos(langNum)) {
        return false;
    }
    // init the stringsInfos_
    for (uint8_t i = 0; i < langNum; i++) {
        stringsInfos_[i].SetFileName(langFile[i]);
        if (i == defaultLangId) {
            ret &= stringsInfos_[i].Load();
        }
    }
    currentLangId_ = defaultLangId;
    return ret;
}

void FontGlobalManager::SetCurrentLangId(uint8_t langId)
{
    if ((langId == currentLangId_) || (langId >= langNum_)) {
        return;
    }

    if (!stringsInfos_[langId].Load()) {
        GRAPHIC_LOGE("load langId %u failed", langId);
        return;
    }

    stringsInfos_[currentLangId_].UnLoad();
    currentLangId_ = langId;
    RefreshViews(RootView::GetInstance());
}

void FontGlobalManager::RefreshViews(UIView* rootView)
{
    if (!rootView->IsViewGroup()) {
        GRAPHIC_LOGW("not view group");
        return;
    }
    int16_t stackCount = 1;
    UIView* currentView = static_cast<UIViewGroup*>(rootView)->GetChildrenHead();
    UIView* parent = rootView;

    while (stackCount > 0) {
        if (currentView == nullptr) {
            currentView = parent->GetNextSibling();
            parent = parent->GetParent();
            stackCount--;
        } else if (currentView->IsViewGroup()) {
            parent = currentView;
            currentView = static_cast<UIViewGroup*>(currentView)->GetChildrenHead();
            stackCount++;
        } else {
            if (currentView->GetViewType() == UI_LABEL_EXT) {
                static_cast<UILabel*>(currentView)->ForceResetText();
            }
            if (currentView->GetViewType() == UI_LABEL_BUTTON_EXT) {
                static_cast<UILabelButton*>(currentView)->ForceResetText();
            }
            LangChangeListener *listener = dynamic_cast<LangChangeListener*>(currentView);
            if (listener != nullptr) {
                listener->OnLangChange(currentLangId_);
            }
            currentView = currentView->GetNextSibling();
        }
    }
    return;
}
} // OHOS

#endif // ENABLE_FONT_VECTOR_GLOBAL

/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingCardModel
 * Create: 2025-06-01
 */
#ifndef SETTING_CARD_MODEL_H
#define SETTING_CARD_MODEL_H

#include <cstdint>
#include <map>
#include "settings/common/SettingCardEffectSample.h"
#include "settings/common/SettingUnionItemGroup.h"
#include "settings/common/SettingCardAddItemGroup.h"
#include "settings/common/SettingCardMgrItemGroup.h"
#include "settings/common/SettingCommon.h"
#include "settings/common/SettingRefSingleton.h"

namespace OHOS {
class SettingCardModel : public SettingRefSingleton<SettingCardModel> {
public:
    SettingCardModel();
    std::vector<SettingUnionItemData>& GetCardUnionData();
    std::vector<SettingCardItemData>& GetCardData();
    void RefreshCardData();
    void AddCard(uint8_t cardId);
    void DeleteCard(uint8_t cardId);
    void SetSelectedCard(uint8_t cardId);
    uint8_t GetSelectedCard(void);
    // 卡片动效
    CardEffectID GetSwipeEffectStyle(void);
    void SetSwipeEffectStyle(CardEffectID style);
    void InitEffectItem(void);
    std::map<CardEffectID, SettingCardEffectSample>& GetEffectListItems(void);
    void UpdateCardUnionData();
    std::map<CardEffectID, SettingCardEffectSample> effects_;
    std::vector<SettingCardItemData> cardData_;
    std::vector<SettingUnionItemData> cardUnionData_;
private:
    uint8_t selectedCardId_ = MAX_CARD;
    CardEffectID selectStyle_;
};
} // OHOS
#endif
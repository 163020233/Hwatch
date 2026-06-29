/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: voice assistant
 * Author: CompanyName
 * Create: 2022-01-18
 */
#ifndef MAIN_VOCASSIST_VIEW_H
#define MAIN_VOCASSIST_VIEW_H

#include "View.h"
#include "offlinevoice/VocassistModel.h"
#include "offlinevoice/VocassistView.h"

namespace OHOS {
class VocassistPresenter;
class MainVocassistView : public View<VocassistPresenter> {
public:
    MainVocassistView();
    MainVocassistView(const MainVocassistView&) = delete;
    MainVocassistView& operator=(const MainVocassistView&) = delete;
    ~MainVocassistView() override;
    void OnStart() override;
    void OnStop() override;
    static MainVocassistView *GetInstance();
    VocassistView *GetVocassist();
private:
    VocassistView* vocassistance{nullptr};
    UIView::OnDragListener* draglistener{nullptr};
};
}
#endif
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SamplePage — template for native app development
 * Create: 2025-06-08
 */

#ifndef SAMPLE_PAGE_H
#define SAMPLE_PAGE_H

#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "samplepage/SamplePresenter.h"

namespace OHOS {

class SamplePage : public SlicePage<SamplePresenter>,
    public UIView::OnDragListener {
public:
    SamplePage();
    ~SamplePage();

    /* lifecycle — add your UI setup here */
    void OnStart(void* data) override;
    void OnResume() override;
    void OnPause(PauseReason reason) override;
    void OnStop() override;

    /* drag-left-to-right returns to app list */
    bool OnDrag(UIView& view, const DragEvent& event) override;

private:
    SamplePage(const SamplePage&) = delete;
    SamplePage& operator=(const SamplePage&) = delete;
    SamplePage(SamplePage&&) = delete;
    SamplePage& operator=(SamplePage&&) = delete;

    UIViewGroup* container_ = nullptr;
};

}  // namespace OHOS
#endif /* SAMPLE_PAGE_H */

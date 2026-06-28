/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UITestMesh
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#ifndef UI_TEST_MESH_H
#define UI_TEST_MESH_H

#include "ui_test.h"
#include "components/ui_view.h"
#include "components/ui_label_button.h"
#include "components/ui_3d_mesh.h"
#include "gfx_utils/image_info.h"

namespace OHOS {
class UITestMesh : public UITest, public UIView::OnClickListener,
    public UI3DMesh::MeshAnimStopListener, public AnimatorCallback {
public:
    UITestMesh() {}
    ~UITestMesh() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    void OnMeshAnimStop(UIView& view) override;
    void Callback(UIView* view) override;

private:
    UIViewGroup* container_ = nullptr;
    UILabelButton* switchBtn_ = nullptr;
    UILabelButton* textureBtn_ = nullptr;
    UILabelButton* realTimeSceencapBtn_ = nullptr;
    UILabelButton* reverseBtn_ = nullptr;
    UI3DMesh* mesh_ = nullptr;
    ImageInfo* texture_ = nullptr;
    VGUMeshType meshType_ = VGU_MESH_ARRAY;
    uint8_t value_ = 'A';
    std::string str_ = "A";
    Animator* anim_ = nullptr;
    void SetUpButton(UIViewGroup* parent, UILabelButton*& btn, int16_t x, int16_t y, const char* text);
};
}
#endif
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UITestSphere
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#include "ui_test_mesh.h"
#include "common/image_cache_manager.h"
#include "components/ui_image_view.h"

namespace OHOS {
constexpr uint8_t MESH_MODEL_MAX = 5;
static const char* g_meshModelPath[MESH_MODEL_MAX] = {
    RES_PATH"mesh_model_convex.bin",
    RES_PATH"mesh_model_concave.bin",
    RES_PATH"mesh_model_vortex.bin",
    RES_PATH"mesh_model_wave.bin",
    RES_PATH"mesh_model_morie.bin",
};

void UITestMesh::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIViewGroup();
    }
    container_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);

    texture_ = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"forest.bin");
    if (texture_ == nullptr) {
        GRAPHIC_LOGE("Load texture failed!");
        return;
    }

    if (mesh_ == nullptr) {
        mesh_ = new UI3DMesh();
    }

    UIImageView* bgImg = new UIImageView();
    ImageInfo* bgInfo = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"kaleidoscope.bin");
    bgImg->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    bgImg->SetAutoEnable(false);
    bgImg->SetResizeMode(UIImageView::ImageResizeMode::FILL);
    bgImg->SetSrc(bgInfo);
    container_->Add(bgImg);

    UILabel* label = new UILabel();
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 100); // 100: font size
    // 2: divider, 50: offset
    label->SetPosition(Screen::GetInstance().GetWidth() / 2 - 50, Screen::GetInstance().GetHeight() / 2 - 50,
        100, 100); // 100: width and height
    label->SetText("A");
    label->SetStyle(STYLE_TEXT_COLOR, Color::Yellow().full);
    container_->Add(label);

    mesh_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    mesh_->SetTouchable(true);
    mesh_->SetDraggable(true);
    mesh_->SetOnClickListener(this);
    mesh_->SetMeshAnimStopListener(this);
    mesh_->SetTexture(texture_);
    mesh_->SetMeshType(meshType_);

    mesh_->SetMeshPosition(texture_->header.width / 2, texture_->header.height / 2); // 2: divider
    mesh_->SetTime(5000); // 5000: Anim Time
    mesh_->Start();
    container_->Add(mesh_);

    SetUpButton(container_, switchBtn_, 128, 350, "switch"); // 128, 350: x, y
    SetUpButton(container_, textureBtn_, 198, 350, "texture"); // 198, 350: x, y
    SetUpButton(container_, reverseBtn_, 268, 350, "reverse"); // 268, 350: x, y
    SetUpButton(container_, realTimeSceencapBtn_, 338, 350, "realTime"); // 338, 350: x, y

    if (anim_ == nullptr) {
        anim_ = new Animator(this, label, 0, true);
    }
    anim_->Start();
}

void UITestMesh::SetUpButton(UIViewGroup* parent, UILabelButton*& btn, int16_t x, int16_t y, const char* text)
{
    if (btn == nullptr) {
        btn = new UILabelButton();
    }
    btn->SetPosition(x, y, 60, 50); // 60, 50: width and height
    btn->SetText(text);
    btn->SetOnClickListener(this);
    parent->Add(btn);
}

void UITestMesh::OnMeshAnimStop(UIView& view)
{
    if (&view == mesh_) {
        bool textureSet = mesh_->GetTexture() != nullptr;
        printf("Mesh Anim Stopped! Type: %d. TextureSet: %d\n", mesh_->GetMeshType(), textureSet);
    }
}

void UITestMesh::Callback(UIView* view)
{
    value_ += 1;
    if (value_ == 'Z') {
        value_ = 'A';
    }
    str_.assign(1, value_);
    UILabel* label = dynamic_cast<UILabel*>(view);
    if (label == nullptr) {
        return;
    }

    label->SetText(str_.c_str());
}

bool UITestMesh::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == switchBtn_) {
        meshType_ = static_cast<VGUMeshType>(static_cast<int>(meshType_) + 1);
        if (meshType_ > VGU_MESH_RIPPLE) {
            meshType_ = VGU_MESH_ARRAY;
        }
        if (meshType_ >= VGU_MESH_CONVEX && meshType_ <= VGU_MESH_MORIE) {
            mesh_->SetModel(g_meshModelPath[meshType_ - 1]);
        }
        mesh_->SetMeshType(meshType_);
        mesh_->Start();
        printf("Switch to MeshType: %d\n", meshType_);
    } else if (&view == textureBtn_) {
        if (mesh_->GetTexture() != nullptr) {
            mesh_->ClearTexture();
            printf("Clear Texture!\n");
        } else {
            mesh_->SetTexture(texture_);
            printf("Set Texture!\n");
        }
    }  else if (&view == realTimeSceencapBtn_) {
        bool realTimeSceencap = !mesh_->GetRealTimeScreencap();
        mesh_->SetRealTimeScreencap(realTimeSceencap);
        printf("SetRealTimeScreencap: %d", realTimeSceencap);
    } else if (&view == reverseBtn_) {
        mesh_->SetReverse(!mesh_->GetReverse());
        mesh_->Start();
        printf("SetReverse: %d\n", mesh_->GetReverse());
    } else if (&view == mesh_) {
        if ((meshType_ == VGU_MESH_ARRAY) || (meshType_ == VGU_MESH_RIPPLE)) {
            int16_t x = event.GetCurrentPos().x;
            int16_t y = event.GetCurrentPos().y;
            mesh_->SetMeshPosition(x, y);
            mesh_->Start();
            printf("Set mesh position: [%d, %d]. Current mesh type: %d\n", x, y, meshType_);
        } else {
            printf("Current mesh type: %d does not support chaging mesh position\n", meshType_);
        }
    }
    return true;
}

void UITestMesh::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    switchBtn_ = nullptr;
    textureBtn_ = nullptr;
    realTimeSceencapBtn_ = nullptr;
    reverseBtn_ = nullptr;
    mesh_ = nullptr;
    texture_ = nullptr;
    meshType_ = VGU_MESH_ARRAY;
    if (anim_ != nullptr) {
        delete anim_;
        anim_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadSingleRes(RES_PATH"forest.bin");
    ImageCacheManager::GetInstance().UnloadSingleRes(RES_PATH"kaleidoscope.bin");
}

const UIView* UITestMesh::GetTestView()
{
    return container_;
}
}
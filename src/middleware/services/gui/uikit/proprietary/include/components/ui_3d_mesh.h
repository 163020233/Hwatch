/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UI3DMesh
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#ifndef UI_3D_MESH_H
#define UI_3D_MESH_H

#include "components/ui_3d_view.h"
#include "animator/animator.h"
#include "gfx_utils/graphic_math.h"

namespace OHOS {
class UI3DMesh : public UI3DView {
public:
    /**
     * @brief  Constructor of UI3DMesh. This must be set to full-screen size.
     */
    UI3DMesh();

    /**
     * @brief  Destructor of UI3DMesh.
     */
    virtual ~UI3DMesh() override;

    /**
     * @brief  Set mesh type.
     *         For strong model types(VGU_MESH_ARRAY and VGU_MESH_RIPPLE), model file is not used.
     *         Memory will be allocated for these types, Call ClearStrongModel to release memory manually.
     *         For other weak model types, model file is required and must be set.
     */
    bool SetMeshType(VGUMeshType meshType);

    /**
     * @brief  Get mesh type.
     */
    VGUMeshType GetMeshType() const;

    /**
     * @brief  Set mesh anim position relative to this widget.
     *         The position is only valid for strong model types(VGU_MESH_ARRAY and VGU_MESH_RIPPLE).
     *         For other types, mesh position is defined in model file.
     */
    void SetMeshPosition(int16_t x, int16_t y);

    /**
     * @brief  Set mesh animator time.
     */
    void SetTime(uint32_t time);

    /**
     * @brief  Start mesh animation.
     */
    void Start();

    /**
     * @brief  Pause mesh animation.
     */
    void Pause();

    /**
     * @brief  Resume mesh animation.
     */
    void Resume();

    /**
     * @brief  Stop mesh animation.
     */
    void Stop();

    /**
     * @brief  Get mesh animation's state.
     */
    uint8_t GetState();

    /**
     * @brief  Set whether reverse the mesh animation.
     */
    void SetReverse(bool reverse);

    /**
     * @brief  Get whether the mesh animation is reversed.
     */
    bool GetReverse() const;

    /**
     * @brief  Set whether takes real-time screencap to be used as texture when texture is not set.
               True: take screencap everyframe.
               False: take screencap once and use it for an animation period.
     */
    void SetRealTimeScreencap(bool realTime);

    bool GetRealTimeScreencap();

    class MeshAnimStopListener : public HeapBase {
    public:
        virtual ~MeshAnimStopListener() {}

        /**
         * @brief Called when mesh animator stops.
         *        You need to inherit from the <b> MeshAnimStopListener</b> class and implement this function.
         */
        virtual void OnMeshAnimStop(UIView& view) {}
    };

    /**
     * @brief Set the listener for the stop of mesh animator.
     */
    void SetMeshAnimStopListener(MeshAnimStopListener* listener);

    /**
     * @brief Get the listener for the stop of mesh animator.
     */
    MeshAnimStopListener* GetMeshAnimStopListener();

    bool SetTexture(const ImageInfo* info) override;

    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    UIViewType GetViewType() const override
    {
        return UI_3D_MESH;
    }

private:
    class MeshAnimCb : public AnimatorCallback {
    public:
        MeshAnimCb() {}
        virtual ~MeshAnimCb() {}
        void Callback(UIView* view) override;
        void OnStop(UIView& view) override;
        void BindAnimator(Animator* anim);
        void SetReverse(bool reverse);
        bool GetReverse() const;
        uint16_t GetMeshAnimValue() const;

    private:
        const uint16_t MAX_MESH_VALUE = 50;
        uint16_t beginValue_ = 0;
        uint16_t endValue_ = MAX_MESH_VALUE;
        uint16_t playBeginValue_ = beginValue_;
        uint16_t playEndValue_ = endValue_;
        uint16_t meshAnimValue_ = 0;
        bool reverse_ = false;
        bool isFirstFrame_ = true;
        Animator* anim_ = nullptr;
    };

    friend class MeshAnimCb;
    MeshAnimCb meshAnimCb_;
    Animator anim_;
    MeshAnimStopListener* listener_ = nullptr;

    int16_t meshPosX_ = 0;
    int16_t meshPosY_ = 0;
    bool realTimeScreencap_ = false;
    bool needScreencap_ = false;
    ImageInfo* screencap_ = nullptr;
    VGUModelSurface strongModel_ = {0};
    VGUMeshAttr meshAttr_ = {VGU_MESH_INVALID, 0, 0, 0};

    void SetScreencapFlag(bool flag);
    void ClearScreencapData();
    void ClearStrongModel();
};
}
#endif

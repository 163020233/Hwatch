/*
 * Copyright (c) 2026. All rights reserved.
 * Description: Touch-reactive particle overlay for the main watch dial.
 */

#ifndef PARTICLE_DIAL_VIEW_H
#define PARTICLE_DIAL_VIEW_H

#include <cstdint>

#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
#include "animator/animator.h"
#include "components/ui_canvas_ext.h"
#include "events/cancel_event.h"
#include "events/drag_event.h"
#include "events/press_event.h"
#include "events/release_event.h"

namespace OHOS {
class ParticleDialView : public UICanvasExt,
                         public UIView::OnTouchListener,
                         public UIView::OnDragListener {
public:
    ParticleDialView();
    ~ParticleDialView() override;
    ParticleDialView(const ParticleDialView&) = delete;
    ParticleDialView& operator=(const ParticleDialView&) = delete;

    void Start();
    void Pause();
    void Resume();
    void Stop();
    uint8_t GetState() const
    {
        return animator_.GetState();
    }

    bool OnPress(UIView& view, const PressEvent& event) override;
    bool OnRelease(UIView& view, const ReleaseEvent& event) override;
    bool OnCancel(UIView& view, const CancelEvent& event) override;
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;

private:
    static constexpr uint16_t PARTICLE_COUNT = 48;

    struct Particle {
        float x = 0.0f;
        float y = 0.0f;
        float vx = 0.0f;
        float vy = 0.0f;
        float homeX = 0.0f;
        float homeY = 0.0f;
        float phase = 0.0f;
        float radius = 1.0f;
    };

    class ParticleAnimatorCallback : public AnimatorCallback {
    public:
        explicit ParticleAnimatorCallback(ParticleDialView* owner) : owner_(owner) {}
        ~ParticleAnimatorCallback() override = default;
        void Callback(UIView* view) override;

    private:
        ParticleDialView* owner_;
    };

    void InitParticles();
    void BeginGather(const Point& position);
    void UpdateTarget(const Point& position);
    void ReleaseParticles();
    void OnAnimatorTick();
    void UpdateParticles(float deltaSeconds);
    void DrawParticles();
    Color GetParticleColor(uint16_t index) const;
    float NextRandom();

    ParticleAnimatorCallback callback_{this};
    Animator animator_;
    Particle particles_[PARTICLE_COUNT] = {};
    uint32_t randomState_ = 0x5A17C9E3;
    uint32_t lastTick_ = 0;
    float timeSeconds_ = 0.0f;
    float touchX_ = 0.0f;
    float touchY_ = 0.0f;
    float previousTouchX_ = 0.0f;
    float previousTouchY_ = 0.0f;
    bool dragging_ = false;
};
} // namespace OHOS
#endif
#endif // PARTICLE_DIAL_VIEW_H

/*
 * Copyright (c) 2026. All rights reserved.
 * Description: Touch-reactive particle overlay for the main watch dial.
 */

#include "main/dial/ParticleDialView.h"

#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
#include <cmath>

#include "hal_tick.h"
#include "UiConfig.h"

namespace OHOS {
namespace {
constexpr int16_t SCREEN_CENTER_X = HORIZONTAL_RESOLUTION / 2;
constexpr int16_t SCREEN_CENTER_Y = VERTICAL_RESOLUTION / 2;
constexpr float MIN_HOME_RADIUS = 72.0f;
constexpr float HOME_RADIUS_RANGE = 92.0f;
constexpr float MAX_DELTA_SECONDS = 0.05f;
constexpr float AMBIENT_FREQUENCY = 0.8f;
constexpr float AMBIENT_RADIUS = 4.0f;
constexpr float GATHER_SPRING = 34.0f;
constexpr float HOME_SPRING = 7.5f;
constexpr float DRAG_DAMPING = 5.8f;
constexpr float RELEASE_DAMPING = 1.65f;
constexpr float SCATTER_SPEED = 155.0f;
constexpr float SCATTER_TANGENT_SPEED = 34.0f;
constexpr float MIN_SCATTER_DISTANCE = 4.0f;
constexpr float MAX_SPEED = 420.0f;
constexpr float PARTICLE_PI = 3.14159265f;
} // namespace

ParticleDialView::ParticleDialView() : animator_(&callback_, this, 0, true)
{
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    SetTouchable(true);
    SetDraggable(true);
    SetIntercept(false);
    SetOnTouchListener(this);
    SetOnDragListener(this);
    InitParticles();
    DrawParticles();
}

ParticleDialView::~ParticleDialView()
{
    Stop();
}

void ParticleDialView::ParticleAnimatorCallback::Callback(UIView* view)
{
    (void)view;
    if (owner_ != nullptr) {
        owner_->OnAnimatorTick();
    }
}

void ParticleDialView::Start()
{
    if (animator_.GetState() != Animator::STOP) {
        return;
    }
    lastTick_ = HALTick::GetInstance().GetTime();
    animator_.Start();
}

void ParticleDialView::Pause()
{
    if (animator_.GetState() == Animator::START) {
        animator_.Pause();
    }
    dragging_ = false;
}

void ParticleDialView::Resume()
{
    if (animator_.GetState() != Animator::PAUSE) {
        return;
    }
    lastTick_ = HALTick::GetInstance().GetTime();
    animator_.Resume();
}

void ParticleDialView::Stop()
{
    if (animator_.GetState() != Animator::STOP) {
        animator_.Stop();
    }
    dragging_ = false;
}

float ParticleDialView::NextRandom()
{
    randomState_ = randomState_ * 1664525U + 1013904223U;
    return static_cast<float>((randomState_ >> 8U) & 0x00FFFFFFU) /
        static_cast<float>(0x01000000U);
}

Color ParticleDialView::GetParticleColor(uint16_t index) const
{
    static const Color colors[] = {
        Color::GetColorFromRGB(0x9D, 0xD7, 0xFF),
        Color::GetColorFromRGB(0x69, 0xB5, 0xFF),
        Color::GetColorFromRGB(0xFF, 0xC8, 0x6A),
        Color::GetColorFromRGB(0xFF, 0xFF, 0xFF)
    };
    return colors[index % (sizeof(colors) / sizeof(colors[0]))];
}

void ParticleDialView::InitParticles()
{
    for (uint16_t i = 0; i < PARTICLE_COUNT; ++i) {
        float phase = NextRandom() * 2.0f * PARTICLE_PI;
        float radius = MIN_HOME_RADIUS + NextRandom() * HOME_RADIUS_RANGE;
        Particle& particle = particles_[i];
        particle.homeX = static_cast<float>(SCREEN_CENTER_X) + cosf(phase) * radius;
        particle.homeY = static_cast<float>(SCREEN_CENTER_Y) + sinf(phase) * radius;
        particle.x = particle.homeX;
        particle.y = particle.homeY;
        particle.phase = phase;
        particle.radius = 1.2f + NextRandom() * 2.4f;
    }
}

bool ParticleDialView::OnPress(UIView& view, const PressEvent& event)
{
    (void)view;
    BeginGather(event.GetCurrentPos());
    return false;
}

bool ParticleDialView::OnRelease(UIView& view, const ReleaseEvent& event)
{
    (void)view;
    (void)event;
    ReleaseParticles();
    return false;
}

bool ParticleDialView::OnCancel(UIView& view, const CancelEvent& event)
{
    (void)view;
    (void)event;
    ReleaseParticles();
    return false;
}

bool ParticleDialView::OnDragStart(UIView& view, const DragEvent& event)
{
    (void)view;
    BeginGather(event.GetCurrentPos());
    return false;
}

bool ParticleDialView::OnDrag(UIView& view, const DragEvent& event)
{
    (void)view;
    UpdateTarget(event.GetCurrentPos());
    return false;
}

bool ParticleDialView::OnDragEnd(UIView& view, const DragEvent& event)
{
    (void)view;
    (void)event;
    ReleaseParticles();
    return false;
}

void ParticleDialView::BeginGather(const Point& position)
{
    dragging_ = true;
    touchX_ = static_cast<float>(position.x);
    touchY_ = static_cast<float>(position.y);
    previousTouchX_ = touchX_;
    previousTouchY_ = touchY_;
    if (animator_.GetState() == Animator::STOP) {
        Start();
    } else if (animator_.GetState() == Animator::PAUSE) {
        Resume();
    }
}

void ParticleDialView::UpdateTarget(const Point& position)
{
    previousTouchX_ = touchX_;
    previousTouchY_ = touchY_;
    touchX_ = static_cast<float>(position.x);
    touchY_ = static_cast<float>(position.y);
    dragging_ = true;
}

void ParticleDialView::ReleaseParticles()
{
    if (!dragging_) {
        return;
    }
    dragging_ = false;
    for (uint16_t i = 0; i < PARTICLE_COUNT; ++i) {
        Particle& particle = particles_[i];
        float dx = particle.x - touchX_;
        float dy = particle.y - touchY_;
        float distance = sqrtf(dx * dx + dy * dy);
        if (distance < MIN_SCATTER_DISTANCE) {
            float angle = NextRandom() * 2.0f * PARTICLE_PI;
            dx = cosf(angle);
            dy = sinf(angle);
            distance = 1.0f;
        }
        float normalX = dx / distance;
        float normalY = dy / distance;
        particle.vx += normalX * SCATTER_SPEED - normalY * SCATTER_TANGENT_SPEED;
        particle.vy += normalY * SCATTER_SPEED + normalX * SCATTER_TANGENT_SPEED;
    }
}

void ParticleDialView::OnAnimatorTick()
{
    uint32_t now = HALTick::GetInstance().GetTime();
    float deltaSeconds = static_cast<float>(HALTick::GetInstance().GetElapseTime(lastTick_)) / 1000.0f;
    lastTick_ = now;
    if (deltaSeconds <= 0.0f) {
        deltaSeconds = 1.0f / 60.0f;
    } else if (deltaSeconds > MAX_DELTA_SECONDS) {
        deltaSeconds = MAX_DELTA_SECONDS;
    }
    timeSeconds_ += deltaSeconds;
    UpdateParticles(deltaSeconds);
    DrawParticles();
}

void ParticleDialView::UpdateParticles(float deltaSeconds)
{
    float targetVelocityX = (touchX_ - previousTouchX_) / deltaSeconds;
    float targetVelocityY = (touchY_ - previousTouchY_) / deltaSeconds;
    previousTouchX_ = touchX_;
    previousTouchY_ = touchY_;

    for (uint16_t i = 0; i < PARTICLE_COUNT; ++i) {
        Particle& particle = particles_[i];
        float targetX = particle.homeX + sinf(timeSeconds_ * AMBIENT_FREQUENCY + particle.phase) *
            AMBIENT_RADIUS;
        float targetY = particle.homeY + cosf(timeSeconds_ * AMBIENT_FREQUENCY * 0.83f + particle.phase) *
            AMBIENT_RADIUS;
        float spring = HOME_SPRING;
        float damping = RELEASE_DAMPING;

        if (dragging_) {
            float gatherRadius = 5.0f + static_cast<float>(i % 7U) * 2.4f;
            targetX = touchX_ + cosf(particle.phase + timeSeconds_ * 1.4f) * gatherRadius;
            targetY = touchY_ + sinf(particle.phase + timeSeconds_ * 1.4f) * gatherRadius;
            spring = GATHER_SPRING;
            damping = DRAG_DAMPING;
            particle.vx += (targetVelocityX - particle.vx) * 0.08f;
            particle.vy += (targetVelocityY - particle.vy) * 0.08f;
        }

        particle.vx += (targetX - particle.x) * spring * deltaSeconds;
        particle.vy += (targetY - particle.y) * spring * deltaSeconds;
        float dampingFactor = 1.0f - fminf(damping * deltaSeconds, 0.92f);
        particle.vx *= dampingFactor;
        particle.vy *= dampingFactor;

        float speedSquared = particle.vx * particle.vx + particle.vy * particle.vy;
        if (speedSquared > MAX_SPEED * MAX_SPEED) {
            float scale = MAX_SPEED / sqrtf(speedSquared);
            particle.vx *= scale;
            particle.vy *= scale;
        }

        particle.x += particle.vx * deltaSeconds;
        particle.y += particle.vy * deltaSeconds;
        particle.x = fminf(fmaxf(particle.x, 1.0f), static_cast<float>(HORIZONTAL_RESOLUTION - 2));
        particle.y = fminf(fmaxf(particle.y, 1.0f), static_cast<float>(VERTICAL_RESOLUTION - 2));
    }
}

void ParticleDialView::DrawParticles()
{
    Clear();
    PaintExt paint;
    paint.SetStyle(Paint::PaintStyle::FILL_STYLE);
    paint.SetAntialiased(true);

    for (uint16_t i = 0; i < PARTICLE_COUNT; ++i) {
        const Particle& particle = particles_[i];
        paint.SetFillColor(GetParticleColor(i));
        paint.SetOpacity(static_cast<uint8_t>(150U + (i % 4U) * 32U));
        Point center = { static_cast<int16_t>(particle.x), static_cast<int16_t>(particle.y) };
        DrawCircle(center, static_cast<uint16_t>(particle.radius), paint);
    }
}
} // namespace OHOS
#endif

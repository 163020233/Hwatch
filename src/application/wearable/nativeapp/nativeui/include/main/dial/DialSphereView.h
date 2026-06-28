/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: DialSphereView
 * Author:
 * Create: 2025-10-09
 */

#ifndef DIAL_SPHERE_VIEW_H
#define DIAL_SPHERE_VIEW_H
#include "animator/animator.h"
#include "components/ui_3d_sphere.h"

namespace OHOS {
class DialSphereView : public UI3DSphere {
public:
    enum class DialSphereType {
        EARTH,
        MOON,
    };

    DialSphereView(const DialSphereView&) = delete;
    DialSphereView &operator=(const DialSphereView&) = delete;
    DialSphereView() = default;
    ~DialSphereView() override;

    /**
     * @brief Starts this animator.
     *
     * The forward playback starts from the first image and the reverse playback starts from the last image.
     */
    void Start();

    /**
     * @brief Stops this animator.
     */
    void Stop();

    /**
     * @brief Pauses this animator.
     */
    void Pause();

    /**
     * @brief Resumes this animator.
     */
    void Resume();

    /**
    * @brief Sets the start longitude angle.
    *
    * Defines the initial longitude value, often used for positioning or rotation reference.
    *
    * @param angle The value of the start longitude, in radians or degrees (depending on system definition).
    */
    void SetStartLongitude(float angle);

    /**
    * @brief Sets the type of the dial sphere.
    *
    * Specifies the display or behavior type of the dial sphere.
    *
    * @param type The type of the dial sphere, defined by the DialSphereType enum.
    */
    void SetDialSphereType(DialSphereType type);

private:
    class DialSphereCallback;
    friend class DialSphereCallback;
    DialSphereCallback *callback_ = nullptr;
    Animator *animator_ = nullptr;
    DialSphereType type_ = DialSphereType::EARTH;
    float startLongitude_ = 0.0;
}; // class DialSphereView
} // namespace OHOS
#endif // DIAL_SPHERE_VIEW_H

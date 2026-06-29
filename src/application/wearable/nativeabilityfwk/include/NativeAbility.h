/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: NativeAbility
 * Create: 2025-11
 */

#ifndef NATIVEABILITYFWK_NATIVE_ABILITY_H
#define NATIVEABILITYFWK_NATIVE_ABILITY_H

#include "Ability.h"

#include "slite_ability.h"

namespace OHOS {
/*
 * Remove parameters, variables, or expressions compiling warning.
 */
#undef UNUSED // in case others define the same macro
#define UNUSED(a) (void)(a)

#ifdef JS_ENABLE
#define OVERRIDE override
#else
#define OVERRIDE
#endif


#ifdef JS_ENABLE
class NativeAbility : public BaseAbility, public SliteAbility {
#else
class NativeAbility : public BaseAbility {
#endif
public:
    static NativeAbility &GetInstance(void);
#ifdef JS_ENABLE
    void InstallAbility(void);
#endif
    // SliteAbility, AMS控制的应用生命周期
    void OnStart(const Want &want);
    void OnActive(const Want &want);
    void OnInactive();
    void OnStop() OVERRIDE;
    void OnCreate(const Want &want) OVERRIDE;
    void OnForeground(const Want &want) OVERRIDE;
    void OnBackground(void) OVERRIDE;
    int GetNativeAbilityState() const;
private:
    // targetId consists of slice id and page id
    void ResumeAbility(uint32_t targetId);
    void PauseAbility();
    void StopAbility();
    NativeAbility() {}
    void ResumeSliceWithJS();
    ~NativeAbility() override {}
    int nativeAbilityState_ = 0;
};
}
#endif // NATIVEABILITYFWK_NATIVE_ABILITY_H

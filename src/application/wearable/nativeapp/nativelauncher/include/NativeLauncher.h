/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: NativeLauncher
 * Create: 2025-11
 */

#ifndef NATIVE_LAUNCHER_H
#define NATIVE_LAUNCHER_H
#include "AppViewIDs.h"

namespace OHOS {
class NativeLauncher {
public:
    static NativeLauncher &GetInstance(void);
    void InitNativeLauncher(void);
private:
    NativeLauncher();
    NativeLauncher(const NativeLauncher &);
    NativeLauncher &operator=(const NativeLauncher &);
    ~NativeLauncher();
};
}
#endif // NATIVE_LAUNCHER_H

/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ModelListenerSample
 * Created: 2025-12-15
 */

#ifndef MODEL_LISTENER_SAMPLE_H
#define MODEL_LISTENER_SAMPLE_H

#include "common/task.h"

namespace OHOS {
class ModelListenerSample : public Task {
public:
    ModelListenerSample();
    virtual ~ModelListenerSample();
    virtual void Notify(void) = 0;
    virtual void OnTickEvent(void) {};
    void Callback() override;
};
}
#endif

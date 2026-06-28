/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Model
 * Create: 2025-11
 */

#ifndef APPFWK_MODEL_H
#define APPFWK_MODEL_H

#include "heap_base.h"

namespace OHOS {
template<class P>
class Model : public HeapBase {
public:
    Model() : presenter_(nullptr) {}
    virtual ~Model() {}

    /**
     * @brief This function will attach presenter to this model
     *
     * @param presenter presenter pointer defined by developer
     */
    void Attach(P* presenter)
    {
        presenter_ = presenter;
    }

protected:
    P* presenter_;

private:
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;
    Model(Model&&) = delete;
    Model& operator=(Model&&) = delete;
};
}
#endif

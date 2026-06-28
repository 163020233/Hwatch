/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Presenter
 * Create: 2025-11
 */

#ifndef APPFWK_PRESENTER_H
#define APPFWK_PRESENTER_H

#include "View.h"

namespace OHOS {
class PresenterBase {
public:
    PresenterBase() : viewBase_(nullptr) {}
    virtual ~PresenterBase() = default;

    /**
     * @deprecated 推荐使用方法OnStart(void* data)
     */
    virtual void OnStart() {}

    /**
     * @brief Scheduled by slice when it is started and before transition animator starts if exsit
     *
     *        You can create non-ui resource and update ui based on Model data here
     */
    virtual void OnStart(void* data) {}

    /**
     * @brief Scheduled by slice when it will be visiable and after transition animator ends if exsit
     *
     *        You can set custom input event listener, request focus, and start its own animator here.
     */
    virtual void OnResume() {}

    /**
     * @brief Scheduled by slice when another slice will be started and before slice transition animator starts if exsit
     *
     *        You can clear input event listener, clear focus, and stop animator here.
     */
    virtual void OnPause() {}

    /**
     * @brief Presenter lifecycle scheduled by slice when it will be invisible and another slice will be visible
     *
     *        You can destroy non-ui resources here
     */
    virtual void OnStop() {}

    /**
     * @brief This function will attach view to this presenter
     *
     * @param view view pointer defined by developer
     */
    virtual void Attach(ViewBase* view)
    {
        viewBase_ = view;
    }

    PresenterBase(const PresenterBase&) = delete;
    PresenterBase& operator=(const PresenterBase&) = delete;
    PresenterBase(PresenterBase&&) = delete;
    PresenterBase& operator=(PresenterBase&&) = delete;
protected:
    ViewBase* viewBase_;
};

template<class V, class M = void>
class Presenter : public PresenterBase {
public:
    Presenter() : view_(nullptr), model_(nullptr) {}

    /**
     * @brief This function will attach view to this presenter
     *
     * @param view view pointer defined by developer
     */
    void Attach(ViewBase* view) override
    {
        PresenterBase::Attach(view);
        view_ = dynamic_cast<V*>(viewBase_);
    }

    /**
     * @brief This function will attach model to this presenter
     *
     * @param model model pointer defined by developer
     */
    void Attach(M* model)
    {
        model_ = model;
    }

protected:
    V* view_;
    M* model_;
};
}
#endif

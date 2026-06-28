/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: IMapStateHandler - Interface for map page state handlers
 * Created: 2026-01-14
 */
#ifndef IMAP_STATE_HANDLER_H
#define IMAP_STATE_HANDLER_H

#include "components/ui_view.h"
#include "events/drag_event.h"

namespace OHOS {

class IMapStateHandler {
public:
    virtual ~IMapStateHandler() = default;

    virtual void Enter() = 0;
    virtual void Exit() = 0;
    virtual bool OnDragStart(const DragEvent &event)
    {
        return false;
    }
    virtual bool OnDrag(const DragEvent &event)
    {
        return false;
    }
    virtual bool OnDragEnd(const DragEvent &event)
    {
        return false;
    }
};

}  // namespace OHOS
#endif  // IMAP_STATE_HANDLER_H

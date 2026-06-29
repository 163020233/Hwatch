/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ui_service
 * Created: 2025-12-15
 */

#ifndef OHOS_UI_SERVICE_H
#define OHOS_UI_SERVICE_H

#include <stdint.h>
#include "service.h"
#include "iunknown.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UI_SERVICE "uiservice"

typedef struct UiServiceFeatureApi {
    INHERIT_IUNKNOWN;
    int32_t (*SendMsg)(int32_t msgId, uint32_t msgValue, void *data, int32_t dataLen);
} UiServiceFeatureApi;

typedef struct UiService UiService;

struct UiService {
    INHERIT_SERVICE;
    INHERIT_IUNKNOWNENTRY(UiServiceFeatureApi);
    Identity identity;
};

typedef enum {
    JS_INSTALL_SUCESS_TO_UI,
    JS_UNINSTALL_SUCESS_TO_UI,
    BMS_REFRESH_APPLIST_EVENT_TO_UI,
    BT_CONNECTED_TO_UI,
    BT_DISCONNECTED_TO_UI,
    UI_ADD_SLEEP_VETO,
    UI_RM_SLEEP_VETO,
    UI_UPDATE_LANGUAGE,
    CONNECT_NEW_PHONE,
    UI_OTA_UPDATE,
    BT_HFP_STATE_CHANGE,
    BT_CALL_STATE_CHANGE,
    BT_SCO_STATE_CHANGE,
    BT_GAP_ENABLE_SUCC,
    BT_A2DP_SRC_STATE_CHANGE,
    BT_AVRCP_TG_PLAY_CONTROL,
    BT_AVRCP_TG_VOLUME_CONTROL,
    BT_SCAN_COMPLETE,
} UiServiceMsgId;

#define BUNDLE_NAME_LEN 128
#define BUNDLE_PATH_LEN 255
typedef struct {
    char bundleName[BUNDLE_NAME_LEN];
    char label[BUNDLE_PATH_LEN];
    char smallIconPath[BUNDLE_PATH_LEN];
    char bigIconPath[BUNDLE_PATH_LEN];
} BundleData;

void SendMsgToUIService(int32_t msgId, uint32_t msgValue, void *data, int32_t dataLen);

void ScreenTurnOn(int32_t timeout);
void ScreenTurnOff(void);
void ScreenOffEnable(int32_t enable);

#ifdef __cplusplus
}
#endif

#endif // OHOS_UiService_SERVICE_H

/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024. All rights reserved.
 * Description: BT AUDIO module.
 */

/**
 * @defgroup bluetooth_bt_audio AUDIO API
 * @ingroup  bluetooth
 * @{
 */

#ifndef BT_AUDIO_H
#define BT_AUDIO_H
#ifdef __cplusplus
extern "C" {
#endif

#define BT_AVRCP_EVENT_STATUS_MAX_LEN 0x08

typedef struct {
    unsigned char event_id;       /*!< @if Eng  event id @else 事件id @endif */
    unsigned char pad[3];         /*!< @if Eng  reserved  @else 预留 @endif */
    unsigned char event_status[BT_AVRCP_EVENT_STATUS_MAX_LEN];  /*!< @if Eng  event status @else 事件的状态信息 @endif */
} bt_avrcp_tg_evt_status_param;

typedef void (*bt_avrcp_tg_get_media_status_cbks)(bt_avrcp_tg_evt_status_param *event_status_param);
typedef void (*bt_avrcp_tg_pass_through_response_cbks)(unsigned int key_operation, unsigned int key_value);

/**
 * @if Eng
 * @brief  Struct of Avrcp TG media callback function.
 * @else
 * @brief  Avrcp TG媒体回调函数接口定义。
 * @endif
 */
typedef struct {
    bt_avrcp_tg_pass_through_response_cbks notify_pass_through_status_cbk;
    bt_avrcp_tg_get_media_status_cbks   get_media_status_cbk;
} bt_avrcp_tg_bts_cbk;

void bt_avrcp_tg_register_audio_cbk(bt_avrcp_tg_bts_cbk *func);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* __BT_A2DP_SERVICE_H__ */

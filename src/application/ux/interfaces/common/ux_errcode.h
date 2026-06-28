/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: provides ux common error code define
 * Author: AuthorNameMagicTag
 * Create: 2023-04-07
 */

#ifndef UX_ERRCODE_H
#define UX_ERRCODE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* |----8bit----| -----8bit------ | --16bit---| */
/* | 固定值0x80 | 模块名,从0x10开始 | 具体错误码 | */
/* | 0x80100001 | */

/* Common */
#define UX_ERR_SUCCESS                                     (0)
#define UX_ERR_FAILURE                                     (-1)
#define UX_ERR_INVALID_PARAMETER                           (0x80100001)
#define UX_ERR_INVALID_LENGTH                              (0x80100002)
#define UX_ERR_MALLOC_FAILED                               (0x80100003)
#define UX_ERR_MEMCPY_FAILED                               (0x80100004)
#define UX_ERR_MEMSET_FAILED                               (0x80100005)
#define UX_ERR_STRCAT_FAILED                               (0x80100006)
#define UX_ERR_NULL_PTR                                    (0x80100007)
#define UX_ERR_REPEAT_OPERATE                              (0x80100008)
#define UX_ERR_SEND_MSG_FAIL                               (0x80100009)
#define UX_ERR_EXCEED_BUFFER_SIZE                          (0x8010000a)
#define UX_ERR_MUTEX_INIT_FAILED                           (0x8010000b)
#define UX_ERR_MUTEX_LOCK_FAILED                           (0x8010000c)
#define UX_ERR_WRITE_NV_FAILED                             (0x8010000d)
#define UX_ERR_READ_NV_FAILED                              (0x8010000e)
#define UX_ERR_ARRAY_OVERFLOW                              (0x8010000f)
#define UX_ERR_STRCPY_FAILED                               (0x80100010)
#define UX_ERR_REPEAT_MALLOC                               (0x80100011)
#define UX_ERR_NOTIFY_FAIL                                 (0x80100012)
#define UX_ERR_INVALID_STATUS                              (0x80100013)
#define UX_ERR_SPRINTF_FAILED                              (0x80100014)
#define UX_ERR_ADDR_TRANS_FAILED                           (0x80100015)
#define UX_ERR_NOT_SUPPORT                                 (0x80100016)
#define UX_ERR_FAIL_TO_REGISTER                            (0x80100017)
#define UX_ERR_FAIL_TO_UNREGISTER                          (0x80100018)
#define UX_ERR_FILE_INVALID                                (0x80100019)
#define UX_ERR_FAIL_TO_OPEN_FILE                           (0x8010001A)
#define UX_ERR_FAIL_TO_CLOSE_FILE                          (0x8010001B)
#define UX_ERR_FAIL_TO_REMOVE_FILE                         (0x8010001C)
#define UX_ERR_NOT_INIT                                    (0x8010001D)
#define UX_ERR_TIMER_START_FAILED                          (0x8010001E)
#define UX_ERR_TIMER_STOP_FAILED                           (0x8010001F)
#define UX_ERR_TASK_ALREADY_EXISTS                         (0x80100020)
#define UX_ERR_TASK_CREATE_FAILED                          (0x80100021)
#define UX_ERR_SEM_INIT_FAILED                             (0x80100022)
#define UX_ERR_SNPRINTF_S_FAILED                           (0x80100023)
#define UX_ERR_CREATE_MSG_QUEUE_FAILED                     (0x80100024)
#define UX_ERR_OSAL_READ_MSG_FAILED                        (0x80100025)

/* BG module */
#define UX_ERR_BG_INIT_FAILED                              (0x80110001)
   // BG CONFIG (0x8011_0001 - 0x8011_1000)
#define UX_ERR_BG_INVALID_PROTOCOL                         (0x80110002)
#define UX_ERR_BG_INVALID_ROLE                             (0x80110003)
#define UX_ERR_BG_REPEAT_WHITELIST_ADDR                    (0x80110004)
#define UX_ERR_BG_WHITELIST_FULL                           (0x80110005)
#define UX_ERR_BG_LOAD_ADDR_FAIL                           (0x80110006)
#define UX_ERR_BG_LOAD_NAME_FAIL                           (0x80110007)
#define UX_ERR_BG_CONFIG_ADDR_FAIL                         (0x80110008)
#define UX_ERR_BG_CONFIG_NAME_FAIL                         (0x80110009)
#define UX_ERR_BG_PAIR_FAIL                                (0x8011000a)
#define UX_ERR_BG_NON_THIS_DEVICE                          (0x8011000b)
#define UX_ERR_BG_SERVICE_OFF_WORKING                      (0x8011000c)
#define UX_ERR_BG_RECONNECT_FAILED                         (0x8011000d)
#define UX_ERR_BG_PAIR_NEW_DEVICE_FAILED                   (0x8011000e)
#define UX_ERR_BG_DEVICE_DOES_NOT_EXIST                    (0x8011000f)
#define UX_ERR_BG_ACL_DISCONNECT                           (0x80110010)
#define UX_ERR_BG_FAIL_TO_CONNECT                          (0x80110011)
#define UX_ERR_BG_FAIL_TO_REPLY                            (0x80110012)
#define UX_ERR_BG_FAIL_TO_REQUEST                          (0x80110013)
#define UX_ERR_BG_PROFILE_DISCONNECT                       (0x80110014)
#define UX_ERR_BG_PROFILE_HAD_CONNECTED                    (0x80110015)
#define UX_ERR_BG_REPEAT_OPERATE                           (0x80110016)
#define UX_ERR_BG_INVALID_CHANNEL                          (0x80110017)
   // SLE ACTIVITY (0x8011_1001 - 0x8011_2000)
   // SLE SOLUTION (0x8011_2001 - 0x8011_3000)
#define UX_ERR_SLE_SOLUTION_NOT_INIT                       (0x80112001)
#define UX_ERR_SLE_SOLUTION_EMPTY_PAIRINFO_IN_NV           (0x80112002)
#define UX_ERR_SLE_SOLUTION_SET_SLE_CONFIG_FAILED          (0x80112003)
#define UX_ERR_SLE_SOLUTION_DB_INVALID_SLOT                (0x80112004)
#define UX_ERR_SLE_SOLUTION_MAX_DEVICE_REACHED             (0x80112005)
   // BT ACTIVITY (0x8011_3001 - 0x8011_4000)
   // BT SOLUTION (0x8011_4001 - 0x8011_5000)
#define UX_ERR_BT_SOLUTION_INIT_FAILED                     (0x80114001)
#define UX_ERR_BT_NOT_NEED_TO_REPULL_FILE                  (0x80114002)

/* Communication module */
#define UX_ERR_COMMU_INIT_FAILED                           (0x80130001)
#define UX_ERR_COMMU_SET_FAILED                            (0x80130002)
#define UX_ERR_COMMU_GET_FAILED                            (0x80130003)
#define UX_ERR_COMMU_NOTIFY_FAILED                         (0x80130004)
#define UX_ERR_COMMU_INVALID_PROTOCOL                      (0x80130005)

/* Audio module */
#define UX_ERR_AUDIO_INIT_FAILED                           (0x80140001)
   // AUDIO ACTIVITY (0x8014_0002 - 0x8014_8000)
#define UX_ERR_AUDIO_ACTIVITY_UAC_INIT_FAILED              (0x80140002)
#define UX_ERR_AUDIO_ACTIVITY_UAC_CREATE_FAILED            (0x80140003)
#define UX_ERR_AUDIO_ACTIVITY_UAC_DESTROY_FAILED           (0x80140004)
#define UX_ERR_AUDIO_ACTIVITY_UAC_DEINIT_FAILED            (0x80140005)

#define UX_ERR_AUDIO_ACTIVITY_LIVE_MIC_INIT_FAILED         (0x80140006)
#define UX_ERR_AUDIO_ACTIVITY_LIVE_MIC_CREATE_FAILED       (0x80140007)
#define UX_ERR_AUDIO_ACTIVITY_LIVE_MIC_DESTROY_FAILED      (0x80140008)
#define UX_ERR_AUDIO_ACTIVITY_LIVE_MIC_DEINIT_FAILED       (0x80140009)

   // AUDIO SOLUTION (0x8014_8001 - 0x8015_0000)

#define UX_ERR_AUDIO_CREATE_STREAM_FAILED                  (0x80148006) // 创建音频流失败
#define UX_ERR_AUDIO_LIVE_MIC_DEV_OVER_MAX                 (0x80148010)
#define UX_ERR_AUDIO_LIVE_MIC_DEV_NO_EXIST                 (0x80148011)
#define UX_ERR_AUDIO_LIVE_MIC_START_FAILED                 (0x80148012)
#define UX_ERR_AUDIO_LIVE_MIC_STOP_FAILED                  (0x80148013)

#define UX_ERR_AUDIO_MEDIA_STREAM_INIT_FAILED              (0x80148020) // 初始化音频流失败
#define UX_ERR_AUDIO_MEDIA_STREAM_DEINIT_FAILED            (0x80148021) // 去初始化音频流失败

#define UX_ERR_AUDIO_MEDIA_STREAM_CREATE_FAILED            (0x80148022) // 创建流失败
#define UX_ERR_AUDIO_SET_DEFAULT_VOLUME_FAILED             (0x80148024) // 设置默认音量失败
#define UX_ERR_AUDIO_PARAM_ERR                             (0x80148025) // 音频参数错误
#define UX_ERR_AUDIO_APPLY_SESSION_FAIL                    (0x80148026) // 申请session id失败
#define UX_ERR_AUDIO_ACTIVE_INTERRUPT_FAIL                 (0x80148027) // 激活音频控制焦点失败
#define UX_ERR_AUDIO_START_STREAM_FAIL                     (0x80148028) // 开启音频流失败
#define UX_ERR_AUDIO_STOP_STREAM_FAIL                      (0x80148029) // 停止音频流失败
#define UX_ERR_AUDIO_SET_CODEC_FAIL                        (0x8014802A) // 设置编码格式失败
#define UX_ERR_AUDIO_SET_VOLUME_FAIL                       (0x8014802B) // 设置音量失败
#define UX_ERR_AUDIO_ANC_CLOSE_FAIL                        (0x8014802C) // 关闭ANC失败
#define UX_ERR_AUDIO_ANC_OPEN_FAIL                         (0x8014802D) // 开启ANC失败

/* Power module */
#define UX_ERR_POWER_INIT_FAILED                           (0x80150001)
#define UX_ERR_POWER_INVAILD_STATE                         (0x80150002)
#define UX_ERR_POWER_NOTIFY_BATTERY_SOC_FAILED             (0x80150003)
#define UX_ERR_POWER_DEVICE_NOT_POWER_ON                   (0x80150004)
#define UX_ERR_POWER_PM_STAGE_SWITCH_FAILED                (0x80150005)
#define UX_ERR_POWER_SERVICE_EXECUTE_FAILED                (0x80150006)
/* Display module */
#define UX_ERR_DISPLAY_INIT_FAILED                         (0x80160001)
#define UX_ERR_DISPLAY_INVALID_CALLBACK                    (0x80160002)

/* Factory module */
#define UX_ERR_FACTORY_INIT_FAILED                         (0x80170001)

/* Protocol module */
#define UX_ERR_PROTOCOL_INIT_FAILED                        (0x80180001)

/* USB module */
#define UX_ERR_USB_INIT_FAILED                             (0x80190001)
#define UX_ERR_USB_DETECT_EMMC_FAILED                      (0x80190002)

/* UPDATE module */
#define UX_ERR_UPDATE_INIT_FAILED                          (0x801a0001)
#define UX_ERR_UPDATE_STATE_IN_OTA_FAILED                  (0x801a0002)
#define UX_ERR_UPDATE_STATE_IN_RECOVER_FAILED              (0x801a0003)
#define UX_ERR_UPDATE_BATTERY_SOC_FAILED                   (0x801a0004)
#define UX_ERR_UPDATE_POWER_OFF_FAILED                     (0x801a0005)
#define UX_ERR_UPDATE_RECORD_STOP_FAILED                   (0x801a0006)
#define UX_ERR_UPDATE_GET_RESULT_FAILED                    (0x801a0007)
#define UX_ERR_UPDATE_RESULT_FAILED                        (0x801a0007)

/* Button module */
#define UX_ERR_BUTTON_INIT_FAILED                          (0x801b0001)

/* Box module */
#define UX_ERR_BOX_INIT_FAILED                             (0x801c0001)

/* LED module */
#define UX_ERR_LED_INIT_FAILED                             (0x801d0001)
#define UX_ERR_LED_TIMER_FAILED                            (0x801d0002)
#define UX_ERR_LED_INVALID_COLOR                           (0x801d0003)
#define UX_ERR_LED_INVALID_CONFIG                          (0x801d0004)
#define UX_ERR_LED_REPETITION_TYPE                         (0x801d0005)

/* Fwk module */
#define UX_ERR_FWK_INIT_FAILED                             (0x801e0001)

/* Manager module */
#define UX_ERR_MANAGER_INIT_FAILED                         (0x801f0001)
#define UX_ERR_MANAGER_FORMAT_FAILED                       (0x801f0002)

/* Jack module */
#define UX_ERR_JACK_INIT_FAILED                            (0x80200001)

/* Device Manager module */
#define UX_ERR_DEVICE_MGR_INIT_FAILED                      (0x80210001)
#define UX_ERR_DEVICE_MGR_NOT_INIT                         (0x80210002)
#define UX_ERR_DEVICE_MGR_TABLE_INIT_FAILED                (0x80210003)
#define UX_ERR_DEVICE_MGR_DELETE_NODE_FAILED               (0x80210004)
#define UX_ERR_DEVICE_MGR_NO_DEVICE_FOUND                  (0x80210005)
#define UX_ERR_DEVICE_MGR_TABLE_NOT_FOUND                  (0x80210006)
#define UX_ERR_DEVICE_MGR_NO_AVAILABLE_DEVICE_ID_ALLOCATE  (0x80210007)
#define UX_ERR_DEVICE_MGR_INVALID_DEVICE_ROLE              (0x80210008)
#define UX_ERR_DEVICE_MGR_INVALID_DEVICE_ID                (0x80210009)
#define UX_ERR_DEVICE_MGR_HEAD_NODE_NULL                   (0x8021000a)
#define UX_ERR_DEVICE_MGR_DELETE_REPEAT_NODE               (0x8021000b)
#define UX_ERR_DEVICE_MGR_PAIR_SLOT_FULL                   (0x8021000c)
#define UX_ERR_DEVICE_MGR_CONN_SLOT_FULL                   (0x80210010)
#define UX_ERR_INVALID_LOGIC_ID                            (0x80210011)
#define UX_ERR_INVALID_DEVICE_ID                           (0x80210012)

#ifdef __cplusplus
}
#endif
#endif

/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Breath Ease
 * Author: wangle
 * Create: 2025-4-23
 */
#ifndef WATCH_SETTING_PROXY_H
#define WATCH_SETTING_PROXY_H

#include <vector>
#include <string>
#include "msg_center_protocol.h"
#include "cpp/audio_manager.h"
#include "adc.h"
#include "tcxo.h"
#include "product_pin.h"
#include "pinctrl_porting.h"
#include "hal_adc.h"

namespace OHOS {

typedef struct storage_info {
    uint32_t total_size;  // 总大小
    uint32_t used_size;   // 已使用
    uint32_t free_size;   // 未使用
} storage_info_t;

typedef struct volume_info {
    uint8_t type;  // 类型
    uint8_t perc;  // 百分比
} volume_info_t;

typedef struct ota_info {
    uint32_t package_size;
} ota_info_t;

enum class OTA_ERR : uint32_t {
    OTA_SUCC = 0x00000000u,                            //"成功"
    OTA_ERR_INVALID_PARAM = 0x00000001u,               //"参数错误"
    OTA_ERR_INITIALIZE_ERR = 0x00000002u,              //"OTA初始化错误"
    OTA_ERR_STATUS_UPDATING = 0x00000003u,             //"OTA正在升级中"
    OTA_ERR_INVAILD_STORAGE = 0x00000004u,             //"获取升级区信息错误"
    OTA_ERR_INSUFFICIENT_STORAGE_SPACE = 0x00000005u,  //"升级区没有足够的空间"
    OTA_ERR_PACKAGE_SIZE_ERROR = 0x00000006u,          //"升级包大小不匹配"
};

static constexpr const uint8_t vol_type_all_idx = 0;
static constexpr const uint8_t vol_type_max_idx = 7;
static constexpr const uint8_t vol_type_alarm = 1;      // 报警类
static constexpr const uint8_t vol_type_ring = 2;       // 来电铃声
static constexpr const uint8_t vol_type_call = 3;       // 语音通话
static constexpr const uint8_t vol_type_assistant = 4;  // 语音助手
static constexpr const uint8_t vol_type_notification = 5; // 通用提示音
static constexpr const uint8_t vol_type_music = 6;      // 音乐
static constexpr const uint8_t vol_type_video = 7;      // 视频


class device_proxy {
public:
    static void get_storage_info(storage_info &info);
    static void get_language_id(uint8_t &lang_id);
    static void set_language_id(uint8_t lang_id);
    static void get_product_info(std::string &info_string);
    static void get_volume_info(uint8_t vol_type, std::vector<volume_info> &vol_info);
    static void set_volume_info(volume_info &vol_info);
    static OTA_ERR ota_init(ota_info &info);
    static OTA_ERR ota_update(ota_info &info);
    static void call_ui_ota_update();
public:
    static uint32_t calc_stat_size(uint64_t size);
    static uint8_t calc_vol_percentage(int32_t min, int32_t max, int32_t curr);
    static int32_t calc_vol_volume(int32_t min, int32_t max, uint8_t percentage);
    static void trans_type_to_audio_stream_type(uint8_t type, std::vector<AudioStreamType> &audio_types);
    static int delete_file_with_cleanup(const char *ppath, const char *path);
};
}  // namespace OHOS
#endif
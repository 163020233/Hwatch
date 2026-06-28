/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: dfx cloud report
 * This file should be changed only infrequently and with great care.
 */

#include "dfx_cloud_report.h"
#include "osal_event.h"
#include "dfx_adapt_layer.h"
#include "diag_data_store.h"
#include "diag_log_hook.h"
#include "diag_metrics_common.h"
#include "diag_metrics.h"
#include "diag_data_transport.h"
#include "diag_data_store_adapt.h"
#include "diag_batch.h"
#include "diag_device_info.h"
#include "product.h"
#include "bts_br_gap.h"
#ifdef CONFIG_NV_FEATURE_SUPPORT
#include "nv.h"
#endif

#define DFX_REPORT_READ_WAIT_TIMEOUT        10000
#define DFX_REPORT_SEND_WAIT_TIME           100
#define DFX_FIRST_REPORT_DELAY              500
#define DFX_REPORT_DELAY                    10
#define DFX_REPORT_CRITICAL_RETRY_TIMES     5 // 定义critical数据发送失败重传次数
#define DFX_REPORT_ITEM_THRESHOLD           10
#define DFX_BATCH_MAX_SIZE                  500
#define DFX_BATCH_THRESHOLD_SIZE            300

#define BT_MAC_LEN                          6
#define BT_ADDR_HALF_LEN                    3
#ifdef CONFIG_PRODUCT_EVB_DITING
#define HARDWARE_MODEL_STRING               "HiDiTing"
#else
#define HARDWARE_MODEL_STRING               "Hi3322_demo"
#endif
#define CHIP_MODEL_STRING                   "Hi3322"
#define DEVICE_VERSION_PREFIX               "FW_ID="
#define DEVICE_VERSION_DATE_STRING          "(" __DATE__ " " __TIME__")"

typedef struct {
    char *node_id;      // 设备id，用于设备注册云端
    bool init_done;     // 初始化标志
    bool enabled;       // 使能标志
} diag_cloud_report_data_t;

typedef enum {
    DIAG_CONNECT_STATE_DISCONNECTED = 0, // 未连接
    DIAG_CONNECT_STATE_CONNECTED,        // 已连接
} diag_connect_state_t;

static diag_cloud_report_data_t g_diag_cloud_report_data;
static osal_event g_report_ev = { 0 };
static uint32_t g_last_kick_ms = 0;
static uint8_t g_batch_buf[DFX_BATCH_MAX_SIZE];
static diag_batch_t g_diag_report_batch;
 
static errcode_t diag_report_flush_batch(diag_batch_t *batch)
{
    errcode_t ret;
 
    if ((batch == NULL) || (diag_batch_size(batch) == 0U)) {
        return ERRCODE_SUCC;
    }
 
    ret = diag_data_transport_data_send_buf(diag_batch_data(batch), diag_batch_size(batch));
    if (ret == ERRCODE_SUCC) {
        diag_batch_reset(batch);
    }
 
    return ret;
}

static errcode_t get_bt_addr(uint16_t key, uint8_t *addr, uint16_t addr_len)
{
    if (addr == NULL || addr_len == 0 || addr_len != BT_MAC_LEN) {
        return ERRCODE_INVALID_PARAM;
    }

    uint16_t read_len = addr_len;
    errcode_t err = uapi_nv_read(key, addr_len, &read_len, addr);
    if (err != ERRCODE_SUCC) {
        return err;
    }

    if (read_len != addr_len) {
        return ERRCODE_FAIL;
    }

    // 转换为小端
    for (int i = 0; i < BT_ADDR_HALF_LEN; i++) {
        uint8_t tmp = addr[i];
        addr[i] = addr[BT_MAC_LEN - 1 - i];
        addr[BT_MAC_LEN - 1 - i] = tmp;
    }
    return ERRCODE_SUCC;
}

static errcode_t set_device_info(void)
{
    diag_device_model_t device_model = {0};

    // 软件版本
    const char *version = DEVICE_VERSION_PREFIX APPLICATION_VERSION_STRING DEVICE_VERSION_DATE_STRING;
    errno_t err = strncpy_s(device_model.version, sizeof(device_model.version), version, sizeof(device_model.version) - 1);
    if (err != EOK) {
        return ERRCODE_FAIL;
    }

    // 硬件版本
    const char *hardware_model = HARDWARE_MODEL_STRING;
    err = strncpy_s(device_model.hardware_model, sizeof(device_model.hardware_model), hardware_model, sizeof(device_model.hardware_model) - 1);
    if (err != EOK) {
        return ERRCODE_FAIL;
    }

    // 芯片版本
    const char *chip_model = CHIP_MODEL_STRING;
    err = strncpy_s(device_model.chip_model, sizeof(device_model.chip_model), chip_model, sizeof(device_model.chip_model) - 1);
    if (err != EOK) {
        return ERRCODE_FAIL;
    }

    // MAC地址（设备标识）
    errcode_t ret = get_bt_addr(NV_ID_BT_FACTORY_ADDR, (uint8_t *)device_model.sn, BT_MAC_LEN);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    // 软件类型
    ret = diag_device_info_set(&device_model);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ERRCODE_SUCC;
}

// 读取并上报关键数据
static errcode_t report_critical(uint32_t read_wait, uint32_t delay_time)
{
    static diag_data_item_t *item_critical = NULL;       // 关键数据上报失败时，可用于保存关键数据，下次继续上报
    static uint8_t retry_times = 0;
    errcode_t ret = ERRCODE_FAIL;

    // 检查是否需要读取新数据
    if (item_critical == NULL) {
        ret = diag_data_store_critical_read(&item_critical, read_wait);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    }

    // 上报关键数据
    ret = diag_data_transport_data_send(item_critical);
    retry_times += 1;

    if (ret == ERRCODE_SUCC || retry_times > DFX_REPORT_CRITICAL_RETRY_TIMES) {
        // 上报成功或超过最大重试次数，释放关键数据
        diag_data_store_critical_release(item_critical);
        item_critical = NULL;
        retry_times = 0;

        // 注册消息上报后，sleep等待注册完成
        dfx_sleep(delay_time);
    } else {
        dfx_log_err("[DIAG_REPORT]diag_data_transport_data_send failed, retry_times = %d\n", retry_times - 1);
    }

    return ret;
}

// 读取并上报非关键数据
static errcode_t report_non_critical(uint32_t read_wait, uint32_t delay_time)
{
    diag_data_item_t *item_non_critical = NULL;
    diag_batch_append_ret_t ret;
    bool retried = false;
    uint32_t now_ms = (uint32_t)uapi_systick_get_ms();
 
    if (diag_data_store_non_critical_read(&item_non_critical, read_wait) != ERRCODE_SUCC) {
        // 没有读到消息，则尝试清空消息batch
        if (diag_batch_should_flush(&g_diag_report_batch, now_ms)) {
            (void)diag_report_flush_batch(&g_diag_report_batch);
        }
        return ERRCODE_FAIL;
    }
 
    for (;;) {
        // 非关键数据放入batch
        ret = diag_batch_append(&g_diag_report_batch, item_non_critical, now_ms);
        switch (ret) {
            // 当前batch放不下，先flush再重试
            case DIAG_BATCH_APPEND_ERR_NO_SPACE:
                if (retried || (diag_report_flush_batch(&g_diag_report_batch) != ERRCODE_SUCC)) {
                    diag_data_transport_data_send(item_non_critical);
                    goto nc_release;
                }
                retried = true;
                continue;
            // append成功，达到阈值，立即flush
            case DIAG_BATCH_APPEND_NEED_FLUSH:
                diag_report_flush_batch(&g_diag_report_batch);
            // append成功，暂不需要flush
            case DIAG_BATCH_APPEND_OK:
                goto nc_release;
            // 参数错误，直接上报
            case DIAG_BATCH_APPEND_ERR_PARAM:
            // 消息过大，直接上报
            case DIAG_BATCH_APPEND_ERR_ITEM_TOO_LARGE:
            default:
                diag_data_transport_data_send(item_non_critical);
                goto nc_release;
        }
    }
 
nc_release:
    // 上报完成，释放非关键数据
    diag_data_store_non_critical_release(item_non_critical);
    item_non_critical = NULL;
    dfx_sleep(delay_time);
    return ERRCODE_SUCC;
}

// 循环任务中读取diag存储区域数据并上报
static errcode_t dfx_report_data(void)
{
    static uint8_t state = DIAG_CONNECT_STATE_DISCONNECTED;
    errcode_t ret_non_crit = ERRCODE_FAIL;
    errcode_t ret_crit = ERRCODE_FAIL;
    
    if (!diag_data_transport_is_connected()) {
        if (state == DIAG_CONNECT_STATE_CONNECTED) {
            dfx_log_info("[DIAG_REPORT] Device now disconnect\n");
        }
        state = DIAG_CONNECT_STATE_DISCONNECTED;
        return ERRCODE_DIAG_NOT_CONNECT;
    }

    if (state == DIAG_CONNECT_STATE_DISCONNECTED) {
        dfx_log_info("[DIAG_REPORT] Device now connect\n");
    }

    if (!diag_get_report_ctrl()) {
        dfx_log_debug("[DIAG_REPORT] Device not registered\n");
        state = DIAG_CONNECT_STATE_CONNECTED;
        return ERRCODE_FAIL;
    }

    // 读取上报关键数据
    uint32_t critical_read_wait = (state == DIAG_CONNECT_STATE_DISCONNECTED) ? DFX_REPORT_READ_WAIT_TIMEOUT : 0;
    uint32_t critical_delay = (state == DIAG_CONNECT_STATE_DISCONNECTED) ? DFX_FIRST_REPORT_DELAY : 0;
    state = DIAG_CONNECT_STATE_CONNECTED;
    ret_crit = report_critical(critical_read_wait, critical_delay);

    // 读取上报非关键数据
    uint32_t non_critical_read_wait = 0;
    uint32_t non_critical_delay = 0;
    ret_non_crit = report_non_critical(non_critical_read_wait, non_critical_delay);

    return (ret_crit == ERRCODE_SUCC || ret_non_crit == ERRCODE_SUCC) ? ERRCODE_SUCC : ERRCODE_FAIL;
}

// 循环任务上报diag数据
void diag_report_consumer_task(void *arg)
{
    unused(arg);
    errcode_t ret;

    while (1) {
        if (!g_diag_cloud_report_data.enabled) {
            dfx_sleep(DFX_REPORT_SEND_WAIT_TIME);
            continue;
        }
        ret = dfx_report_data();
        if (ret == ERRCODE_SUCC) {
            // 读取数据成功，继续循环读取
            osal_event_clear(&g_report_ev, DFX_CLOUD_REPORT_MSG);
            continue;
        }

        if (diag_batch_size(&g_diag_report_batch) != 0) {
            dfx_sleep(DFX_REPORT_SEND_WAIT_TIME);
            continue;
        }

        // 等report事件 + 读出并清除
        osal_event_read(&g_report_ev, DFX_CLOUD_REPORT_MSG, OSAL_WAIT_FOREVER, OSAL_WAITMODE_OR | OSAL_WAITMODE_CLR);
    }
}

#if defined(CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK) && (CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK == DFX_YES)
// 回调：日志上报，写入diag存储
static errcode_t log_write_cb(uint16_t cmd_id, void *data, uint32_t len, void *cb_arg)
{
    unused(cb_arg);
    diag_data_store_ctx_t store_ctx = {
        .idx = 0,
        .enable_timestamp = false,
    };
    // 日志写入非关键数据存储
    return diag_data_store_non_critical_write(cmd_id, data, len, &store_ctx);
}

// 回调：事件上报，写入diag存储
static errcode_t event_write_cb(uint16_t cmd_id, void *data, uint32_t len, void *cb_arg)
{
    unused(cb_arg);
    diag_data_store_ctx_t store_ctx = {
        .idx = 1,
        .enable_timestamp = false,
    };
    // 事件写入非关键数据存储
    return diag_data_store_non_critical_write(cmd_id, data, len, &store_ctx);
}
#endif

static errcode_t diag_report_batch_init(void)
{
    diag_batch_cfg_t cfg = {
        .max_buf_size    = sizeof(g_batch_buf),
        .bytes_threshold = DFX_BATCH_THRESHOLD_SIZE,
        .timeout_ms      = DFX_REPORT_SEND_WAIT_TIME,
    };
 
    return diag_batch_init(&g_diag_report_batch,
                           g_batch_buf,
                           sizeof(g_batch_buf),
                           &cfg);
}

// 读取NV存储中的日志上报配置
static uint8_t nv_read_log_flag(void)
{
#ifdef CONFIG_NV_FEATURE_SUPPORT
    uint8_t flag = 0;
    uint16_t len;
    // 从NV中读取日志上报开关
    errcode_t ret = uapi_nv_read(NV_ID_HCD_LOG_ENABLE_FLAG, sizeof(flag), &len, &flag);
    if (ret != ERRCODE_SUCC) {
        return 0;
    }
    return flag;
#else
    return 0;
#endif
}

// 初始化metrics上报
static errcode_t metrics_init(void)
{
    errcode_t ret;
    ret = diag_metrics_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

#if (CONFIG_DFX_SUPPORT_OS_METRICS_REPORT == DFX_YES)
    // 初始化OS metrics
    diag_os_metrics_init();
#endif /* CONFIG_DFX_SUPPORT_OS_METRICS_REPORT */
#if (CONFIG_DFX_SUPPORT_CONNECTIVITY_METRICS_REPORT == DFX_YES)
    // 初始化connectivity metrics
    diag_connectivity_metrics_init();
#endif /* CONFIG_DFX_SUPPORT_CONNECTIVITY_METRICS_REPORT */
#if (CONFIG_DFX_SUPPORT_SENSOR_METRICS_REPORT == DFX_YES)
    // 初始化传感器 metrics
    diag_sensor_metrics_init();
#endif /* CONFIG_DFX_SUPPORT_SENSOR_METRICS_REPORT */

#ifdef CONFIG_NV_FEATURE_SUPPORT
    uint8_t flag = 0;
    uint16_t len;
    // 从NV中读取metrics上报开关
    ret = uapi_nv_read(NV_ID_HCD_METRICS_ENABLE_FLAG, sizeof(flag), &len, &flag);
    if (ret != ERRCODE_SUCC && flag != 0) {
        diag_metrics_enable();
    }
#endif
    return ERRCODE_SUCC;
}

// 初始化metrics上报
static errcode_t metrics_deinit(void)
{
    diag_os_metrics_deinit(); // 去初始化os metrics
    diag_connectivity_metrics_deinit(); // 去初始化conenctivity metrics
    diag_sensor_metrics_deinit(); // 去初始化sensor metrics
    return diag_metrics_deinit();
}

// 去使能diag cloud report
errcode_t dfx_report_disable(void)
{
    diag_data_store_deinit(); // 去初始化数据存储
#if defined(CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK) && (CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK == DFX_YES)
    diag_log_hook_deinit();  // 去初始化日志钩子
#endif
#if defined(CONFIG_DFX_SUPPORT_METRICS_REPORT) && (CONFIG_DFX_SUPPORT_METRICS_REPORT == DFX_YES)
    metrics_deinit(); // 去初始化metrics
#endif
    g_diag_cloud_report_data.enabled = false; // 去使能标志
    return ERRCODE_SUCC;
}

static void event_kick_now(uint32_t now)
{
    if (!diag_get_report_ctrl()) {
        dfx_log_debug("[DIAG_REPORT] event_kick_now, but device not registered\n");
        return;
    }
    osal_event_write(&g_report_ev, DFX_CLOUD_REPORT_MSG);
    g_last_kick_ms = now;
}

void diag_report_write_event(const diag_write_evt_t *evt)
{
    if (evt == NULL) {
        return;
    }

    uint32_t now = (uint32_t)uapi_systick_get_ms();

    // 强触发：immediate 事件立刻踢
    if (evt->trigger_flags & DIAG_TRIGGER_IMMEDIATE) {
        event_kick_now(now);
        return;
    }

    // 强触发：达到条数阈值立刻踢
    if (evt->item_num >= DFX_REPORT_ITEM_THRESHOLD) {
        event_kick_now(now);
        return;
    }

    // 强触发：首包保障,队列从空到非空
    if (evt->item_num == 1) {
        event_kick_now(now);
        return;
    }
    
    // 弱触发：限频兜底
    if ((uint32_t)(now - g_last_kick_ms) >= DFX_REPORT_SEND_WAIT_TIME) {
        event_kick_now(now);
    }
}

// 使能diag cloud report
errcode_t dfx_report_enable(dfx_report_config_t *config)
{
    errcode_t err = ERRCODE_SUCC;
    if (!config) {
        return ERRCODE_INVALID_PARAM;
    }

    // 1. 初始化传输拼包
    err = diag_report_batch_init();
    if (err != ERRCODE_SUCC) {
        dfx_log_err("[DIAG_REPORT]Failed to initialize report batch.");
        goto enable_err;
    }

    // 2. 获取数据存储驱动
    diag_data_store_driver_t *nc_queue_driver = diag_data_store_queue_get_non_critical_driver();
    diag_data_store_driver_t *c_queue_driver = diag_data_store_queue_get_critical_driver();

    // 3. 注册非关键数据驱动
    diag_data_store_register_non_critical_driver(nc_queue_driver);
    // 4. 注册关键数据驱动
    diag_data_store_register_critical_driver(c_queue_driver);

    // 5. 初始化整个数据存储系统
    if (diag_data_store_init() != ERRCODE_SUCC) {
        dfx_log_err("[DIAG_REPORT]Failed to initialize data store!\n");
        return ERRCODE_FAIL;
    }
    diag_log_config_t log_config = {
        .log_write_cb = log_write_cb,
        .event_write_cb = event_write_cb,
        .cb_arg = NULL,
    };
#if defined(CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK) && (CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK == DFX_YES)
    // 6. 初始化日志钩子
    err = diag_log_hook_init(&log_config);
    if (err != ERRCODE_SUCC) {
        dfx_log_err("[DIAG_REPORT]Failed to initialise Log hook.");
        goto enable_err;
    }
    diag_log_hook_enable(config->log_level);
#endif /* CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK */

#if defined(CONFIG_DFX_SUPPORT_METRICS_REPORT) && (CONFIG_DFX_SUPPORT_METRICS_REPORT == DFX_YES)
    // 7. 初始化metrics
    err = metrics_init();
    if (err != ERRCODE_SUCC) {
        dfx_log_err("[DIAG_REPORT]Failed to initialize metrics.");
        goto enable_err;
    }
#endif /* CONFIG_DFX_SUPPORT_METRICS_REPORT */

    g_diag_cloud_report_data.enabled = true;
    g_diag_cloud_report_data.init_done = true;
    return ERRCODE_SUCC;
enable_err:
    dfx_report_disable();
    return err;
}

// 去初始化diag cloud report
void dfx_report_system_deinit(void)
{
    if (!g_diag_cloud_report_data.init_done) {
        return;
    }
    osal_event_destroy(&g_report_ev); // 销毁report事件
    diag_device_info_clear(); // 去初始化设备信息
    diag_data_transport_disconnect(); // 断开数据传输
    dfx_report_disable(); // 去使能diag cloud report
    diag_data_transport_unregister(); // 去初始化数据传输
    g_diag_cloud_report_data.init_done = false;
}

// 初始化diag cloud report
errcode_t dfx_report_system_init(void)
{
    errcode_t err = ERRCODE_FAIL;
#if (CONFIG_DFX_DATA_TRANSPORT_BLE == DFX_YES)
    // 初始化数据传输
    err = diag_data_transport_register(&g_default_diag_data_transport_ble);
    if (err != ERRCODE_SUCC && err != ERRCODE_DIAG_NOT_INIT) {
        dfx_log_err("[DIAG_REPORT]Failed to register transport");
        goto init_err;
    }
#endif

    // 初始化report事件
    if (osal_event_init(&g_report_ev) != OSAL_SUCCESS) {
        dfx_log_err("[DIAG_REPORT]Failed to init report event.");
        goto init_err;
    }

    err = set_device_info();
    if (err != ERRCODE_SUCC) {
        dfx_log_err("[DIAG_REPORT]Failed to set device info.");
        goto init_err;
    }

    // 使能diag cloud report
    dfx_report_config_t config = {
        .log_level = nv_read_log_flag(),
    };
    err = dfx_report_enable(&config);
    if (err != ERRCODE_SUCC) {
        goto init_err;
    }

    // 连接数据传输
    err = diag_data_transport_connect();
    if (err != ERRCODE_SUCC) {
        dfx_log_err("[DIAG_REPORT]Failed to connect to transport.");
        goto init_err;
    }

    err = diag_metrics_cmd_init();
    if (err != ERRCODE_SUCC) {
        dfx_log_err("[DIAG_REPORT]Failed to initialize HCD cmd.");
        goto init_err;
    }

    return ERRCODE_SUCC;
init_err:
    dfx_report_system_deinit();
    return err;
}
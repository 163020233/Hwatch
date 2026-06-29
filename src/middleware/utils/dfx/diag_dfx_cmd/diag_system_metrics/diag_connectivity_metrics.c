/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag connectivity metrics
 * This file should be changed only infrequently and with great care.
 */

#include "dfx_channel.h"
#include "diag_metrics_common.h"
#include "diag_metrics.h"
#include "dfx_adapt_layer.h"
#include "soc_diag_cmd_id.h"

#define SPP_RATE_MAX_SEC 20            // spp速率统计窗口上限
#define SPP_RATE_DEFAULT_SEC 10        // 默认窗口10秒

typedef struct {
    int rssi; // 信号强度
    bool is_running; // 运行状态
    bool init; // 初始化标志
} connect_diag_priv_data_t;

typedef struct {
    uint32_t sec_stamp;
    uint32_t tx_bytes;
    uint32_t rx_bytes;
} sec_bucket_t;

static struct {
    uint16_t max_window_sec;   // 运行期可用的窗口上限
    sec_bucket_t *b;
} g_spp_rate_ctx_t;

static connect_diag_priv_data_t g_priv_data;
static sec_bucket_t g_buckets[SPP_RATE_MAX_SEC];  // 用于保存每秒蓝牙上下行数据量
static uint64_t g_spp_rx_total = 0;               // 记录蓝牙接收总字节数
static uint64_t g_spp_tx_total = 0;               // 记录蓝牙发送总字节数

// 取得“某秒”的桶并确保是当前秒可用的（旧秒自动清零）
static sec_bucket_t* touch_bucket(uint32_t sec)
{
    uint16_t idx = (uint16_t)(sec % g_spp_rate_ctx_t.max_window_sec);
    sec_bucket_t *bk = &g_spp_rate_ctx_t.b[idx];
    if (bk->sec_stamp != sec) { // 新秒 或 被环覆盖
        bk->sec_stamp = sec;
        bk->tx_bytes = 0;
        bk->rx_bytes = 0;
    }
    return bk;
}

// 初始化spp速率metric：传入最大支持的窗口秒数（比如 10秒）
static void spp_rate_init(uint16_t max_window_sec)
{
    if (max_window_sec == 0 || max_window_sec > SPP_RATE_MAX_SEC) {
        max_window_sec = SPP_RATE_MAX_SEC;
    }
    g_spp_rate_ctx_t.max_window_sec  = max_window_sec;
    g_spp_rate_ctx_t.b               = g_buckets;
    if (memset_s(g_spp_rate_ctx_t.b, sizeof(g_buckets), 0, sizeof(g_buckets)) != 0) {
        dfx_log_err("[DIAG_REPORT] spp rate memset fail");
    }
}

void diag_spp_rate_on_rx(uint32_t bytes)
{
    uint32_t sec = dfx_get_cur_second();
    sec_bucket_t *bk = touch_bucket(sec);
    bk->rx_bytes += bytes;
    g_spp_rx_total += bytes;
}

void diag_spp_rate_on_tx(uint32_t bytes)
{
    uint32_t sec = dfx_get_cur_second();
    sec_bucket_t *bk = touch_bucket(sec);
    bk->tx_bytes += bytes;
    g_spp_tx_total += bytes;
}

// 查询：计算“近 n 秒”的平均速率（bps）；n 必须 <= 初始化时的 max_window_sec
// 返回：true 表示有效；tx_bps / rx_bps 输出平均比特率
static bool spp_rate_avg_bps(uint16_t n_sec, uint32_t *tx_bps, uint32_t *rx_bps)
{
    if (n_sec == 0 || n_sec > g_spp_rate_ctx_t.max_window_sec || !tx_bps || !rx_bps) return false;

    uint32_t sec_now = dfx_get_cur_second();
    uint64_t sum_tx = 0, sum_rx = 0;

    // 汇总“[sec_now - n_sec + 1, sec_now]”这些秒的字节
    for (uint16_t i = 0; i < n_sec; ++i) {
        uint32_t sec = sec_now - i;
        uint16_t idx = (uint16_t)(sec % g_spp_rate_ctx_t.max_window_sec);
        sec_bucket_t *bk = &g_spp_rate_ctx_t.b[idx];
        if (bk->sec_stamp == sec) { // 只算时间戳匹配的桶
            sum_tx += bk->tx_bytes;
            sum_rx += bk->rx_bytes;
        }
    }

    // 平均 bps：字节×8 / 秒数
    *tx_bps = (uint32_t)((sum_tx * 8ULL) / n_sec);
    *rx_bps = (uint32_t)((sum_rx * 8ULL) / n_sec);
    return true;
}

// 回调：获取蓝牙速率，ctx 里携带窗口秒数 n（默认1秒）
static errcode_t diag_get_spp_rate(void *data_ptr, size_t *len_ptr, void *ctx)
{
    if (!data_ptr || !len_ptr || *len_ptr < sizeof(diag_spp_rate_info_t)) {
        return ERRCODE_FAIL;
    }

    uint16_t n_sec = (uint16_t)(uintptr_t)ctx;
    if (n_sec == 0) {
        n_sec = 1; // 默认 1 秒
    }

    diag_spp_rate_info_t out;
    if (memset_s(&out, sizeof(out), 0, sizeof(out)) != 0) {
        return ERRCODE_MEMSET;
    }
    out.window_sec = n_sec;
    out.tx_bytes_total = g_spp_tx_total;
    out.rx_bytes_total = g_spp_rx_total;
    if (!spp_rate_avg_bps(n_sec, &out.tx_bps, &out.rx_bps)) {
        return ERRCODE_FAIL;
    }
    if (memcpy_s(data_ptr, sizeof(out), &out, sizeof(out)) != 0) {
        return ERRCODE_MEMCPY;
    }
    *len_ptr = sizeof(out);
    return ERRCODE_SUCC;
}

// 初始化，注册connectivity metrics
errcode_t diag_connectivity_metrics_init(void)
{
    errcode_t ret;

    if (g_priv_data.init) {
        return ERRCODE_SUCC; // Or an error indicating it's already running
    }

    if (memset_s(&g_priv_data, sizeof(g_priv_data), 0, sizeof(g_priv_data)) != EOK) {
        return ERRCODE_MEMSET;
    }
    g_priv_data.is_running = true;

    // 注册spp速率metrics
    spp_rate_init(SPP_RATE_DEFAULT_SEC);  // 初始化窗口为10秒
    ret = uapi_diag_metrics_period_report(DIAG_CMD_DIAG_REPORT_SPP_RATE, diag_get_spp_rate, 0,
                                          sizeof(diag_spp_rate_info_t), (void*)(uintptr_t)SPP_RATE_DEFAULT_SEC);
    if (ret != ERRCODE_SUCC) {
        dfx_log_err("[DIAG_REPORT] SPP rate metrics register fail, ret: 0x%x", ret);
    }

    g_priv_data.init = true;
    return ERRCODE_SUCC;
}

// 去初始化 metrics
errcode_t diag_connectivity_metrics_deinit(void)
{
    if (!g_priv_data.init) {
        return ERRCODE_SUCC; // Or an error indicating it's already running
    }
    g_priv_data.is_running = false;
    uapi_diag_metrics_stop_report(DIAG_CMD_DIAG_REPORT_SPP_RATE);
    g_priv_data.init = false;
    return ERRCODE_SUCC;
}
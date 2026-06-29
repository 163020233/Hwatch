/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag data mock
 * This file should be changed only infrequently and with great care.
 */

#include <stdlib.h>
#include <time.h>
#include "diag.h"
#include "los_tick.h"
#include "dfx_adapt_layer.h"

#define CPU_HIGH_TH 600
#define CPU_CRIT_TH 800
#define JITTER_CNT 2
#define CPUP_TH 1000
#define CPUP_CRITICAL_AV 850
#define CPUP_HIGH_AV 650
#define CRIT_PROB 2
#define HIGH_PROB 10
#define RANDOM_SEED_MAGIC 1103515245
#define RANDOM_SEED_BIAS 12345
#define RANDOM_SEED_MASK 0x7FFF
#define RANDOM_SEED_OFFSET 16
#define PERCENT_RANGE 100

static int anomaly_remain = 0;
static int anomaly_target = 0;
static int anomaly_active = 0;  // 0=normal, 1=high, 2=critical

static uint32_t my_random(void)
{
    static unsigned int seed = 0;
    if (seed == 0) {
        seed = (uint32_t)osal_get_jiffies();  // 用系统tick初始化种子
    }
    seed = seed * RANDOM_SEED_MAGIC + RANDOM_SEED_BIAS;
    return (seed >> RANDOM_SEED_OFFSET) & RANDOM_SEED_MASK;  // 类似 rand()
}

// 模拟 CPU 使用率数据，带异常注入
uint32_t inject_cpu_anomaly(uint32_t measured)
{
    int out = (int)measured;

    if (anomaly_remain > 0) {
        // 在异常段中
        int jitter = (anomaly_active == 2) ? 50 : 80; // critical 抖动小一些
        out = anomaly_target + ((int)my_random() % (JITTER_CNT * jitter + 1) - jitter);
        if (out < 0) {
            out = 0;
        }
        if (out > CPUP_TH) {
            out = CPUP_TH;
        }
        anomaly_remain--;
        if (anomaly_remain == 0) {
            anomaly_active = 0; // 恢复正常
        }
    } else {
        // 正常状态，低概率触发一次异常
        int r = (int)my_random() % 1000;
        if (r < CRIT_PROB) { // ~0.2% 概率 critical
            anomaly_active = 2;
            anomaly_target = (int)(CPUP_CRITICAL_AV + my_random() % PERCENT_RANGE); // 85%~95%
            anomaly_remain = (int)(1 + my_random() % 5);     // 1~5 次，保证 <=6
            out = anomaly_target;
            dfx_log_debug("[DIAG] inject_cpu_anomaly: critical anomaly, origin: %d, anomaly: %d", measured, out);
        } else if (r < HIGH_PROB) { // ~0.8% 概率 high
            anomaly_active = 1;
            anomaly_target = (int)(CPUP_HIGH_AV + my_random() % PERCENT_RANGE); // 65%~75%
            anomaly_remain = (int)(1 + my_random() % 5);     // 1~5 次
            out = anomaly_target;
            dfx_log_debug("[DIAG] inject_cpu_anomaly: high anomaly, origin: %d, anomaly: %d", measured, out);
        } else {
            out = (int)measured; // 透传正常值
        }
    }
    return (uint32_t)out;
}

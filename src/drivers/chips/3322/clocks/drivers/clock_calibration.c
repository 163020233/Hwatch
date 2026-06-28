/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: CLOCK CALIBRATION DRIVER.
 *
 * Create: 2025-05-28
 */

#include "clock_calibration.h"
#include "common_def.h"
#include "soc_reg.h"
#include "chip_io.h"
#include "osal_interrupt.h"
#include "calendar_porting.h"
#include "systick_porting.h"
#include "debug_print.h"

#ifdef CONFIG_ENABLE_RC_CALIBRATION
#include "pmu_alarm.h"
#include "cmsis_os2.h"
#include "cmsis_os.h"
#endif

#define CALI_CLOCK_SHIFT             16
#define CALI_CLOCK_FREQ_32K_100      3276800
#define TCXO_CLK_FREQ                32000000
#define CALI_CLOCK_FREQ_32K_RECORD_COUNTS  5 // 校准滑窗平均次数

static uint32_t g_clock_32k_100 = CALI_CLOCK_FREQ_32K_100;
#if BT_RC_32K_SUPPORT
// 滑窗队列平均校准
typedef struct {
    uint32_t data[CALI_CLOCK_FREQ_32K_RECORD_COUNTS]; // 定义一个固定大小为10的数组
    uint32_t head;     // 头指针
    uint32_t tail;     // 尾指针
    uint32_t length;   // 当前队列长度
} rc32k_cali_queue;

rc32k_cali_queue g_32k_cali_queue = {0};

uint32_t g_clock_32k_cur_freq = 0;

void clocks_32k_cali_enqueue(rc32k_cali_queue *q, uint32_t cali_value)
{
    if (q->length >= CALI_CLOCK_FREQ_32K_RECORD_COUNTS) {
        // 队列已满，移除头部元素
        q->head = (q->head + 1) % CALI_CLOCK_FREQ_32K_RECORD_COUNTS;
        q->length--;
    }
    q->data[q->tail] = cali_value;
    q->tail = (q->tail + 1) % CALI_CLOCK_FREQ_32K_RECORD_COUNTS;
    q->length++;
}

uint32_t clocks_32k_cali_average(rc32k_cali_queue *q)
{
    if (q->length == 0) {
        return 0;  // 队列为空
    }
    uint32_t sum = 0;
    for (uint32_t i = 0; i < q->length; i++) {
        sum += q->data[(q->head + i) % CALI_CLOCK_FREQ_32K_RECORD_COUNTS];
    }
    return sum / q->length;
}

uint32_t clocks_calibration_get_cali_average_result(void)
{
    uint32_t result_l = readw(PMU2_CMU_CTL_RB_PMU2_GP_REG1_REG);
    uint32_t result_h = readw(PMU2_CMU_CTL_RB_PMU2_GP_REG0_REG) & 0x3FFF;
    uint32_t result = (uint32_t)(result_l | (result_h << CALI_CLOCK_SHIFT));

    uint32_t cali_count = readw(GLB_CTL_M_RB_32K_DET_VAL_REG);

    uint32_t clock_32k_tmp = (uint32_t)(((uint64_t)(cali_count) * TCXO_CLK_FREQ * CALI_CLOCK_MUL) / result);

    g_clock_32k_cur_freq =  clock_32k_tmp;

    clocks_32k_cali_enqueue(&g_32k_cali_queue, clock_32k_tmp);
    g_clock_32k_100 = clocks_32k_cali_average(&g_32k_cali_queue);
    return g_clock_32k_100;
}

uint32_t clocks_calibration_cur_freq_get(void)
{
    return g_clock_32k_cur_freq;
}

uint32_t clocks_calibration_get_detfunc(void)
{
    return (uint32_t)reg16_getbit(GLB_CTL_M_RB_32K_CALI_CFG_REG, GLB_CTL_M_RB_CLK_32K_DET_EN_OFFSET);
}
#endif

uint32_t clocks_calibration_get_freq(uint32_t cali_count)
{
    writew(GLB_CTL_M_RB_32K_DET_VAL_REG, cali_count);

    reg16_setbit(GLB_CTL_M_RB_32K_CALI_CFG_REG, GLB_CTL_M_RB_CLK_32K_DET_EN_OFFSET);

    while (reg16_getbit(GLB_CTL_M_RB_32K_CALI_CFG_REG, GLB_CTL_M_RB_CLK_32K_DET_DONE_STS_OFFSET) != 1) {
    }

    uint32_t result_l = readw(PMU2_CMU_CTL_RB_PMU2_GP_REG1_REG);
    uint32_t result_h = readw(PMU2_CMU_CTL_RB_PMU2_GP_REG0_REG) & 0x3FFF;
    uint32_t result = (uint32_t)(result_l | (result_h << CALI_CLOCK_SHIFT));
    if (result == 0) {
        return g_clock_32k_100;
    }

    uint32_t clock_32k_100 = (uint32_t)(((uint64_t)(cali_count) * TCXO_CLK_FREQ * CALI_CLOCK_MUL) / result);

    reg16_clrbit(GLB_CTL_M_RB_32K_CALI_CFG_REG, GLB_CTL_M_RB_CLK_32K_DET_EN_OFFSET);

    return clock_32k_100;
}

void clocks_calibration_set_freq_result(uint32_t freq)
{
    g_clock_32k_100 = freq;
}

uint32_t clocks_calibration_get_freq_result(void)
{
    return g_clock_32k_100;
}

void clocks_calibration_config_cali_cycle(uint8_t cali_count)
{
    writew(GLB_CTL_M_RB_32K_DET_VAL_REG, cali_count);
}

uint32_t clocks_calibration_get_cali_result(void)
{
    uint32_t result_l = readw(PMU2_CMU_CTL_RB_PMU2_GP_REG1_REG);
    uint32_t result_h = readw(PMU2_CMU_CTL_RB_PMU2_GP_REG0_REG) & 0x3FFF;
    uint32_t result = (uint32_t)(result_l | (result_h << CALI_CLOCK_SHIFT));
    if (result == 0) {
        return g_clock_32k_100;
    }

    uint32_t cali_count = readw(GLB_CTL_M_RB_32K_DET_VAL_REG);

    uint32_t clock_32k_100 = (uint32_t)(((uint64_t)(cali_count) * TCXO_CLK_FREQ * CALI_CLOCK_MUL) / result);

    return clock_32k_100;
}

void clocks_calibration_enable(uint8_t enable)
{
    if (enable) {
        reg16_setbit(GLB_CTL_M_RB_32K_CALI_CFG_REG, GLB_CTL_M_RB_CLK_32K_DET_EN_OFFSET);
    } else {
        reg16_clrbit(GLB_CTL_M_RB_32K_CALI_CFG_REG, GLB_CTL_M_RB_CLK_32K_DET_EN_OFFSET);
    }
}

bool clocks_calibration_get_done_status(void)
{
    return reg16_getbit(GLB_CTL_M_RB_32K_CALI_CFG_REG, GLB_CTL_M_RB_CLK_32K_DET_DONE_STS_OFFSET);
}

void calibration_set_xo_core_ctrim(uint8_t xo_ctrim_value)
{
    if (xo_ctrim_value > CALIBRATION_XO_CORE_CTRIM_MAX) {
        return;
    }

    reg32_setbits(ULP_AON_FUSE_CTL_RB_FUSE_CTL_AUTO_RD_36_REG, ULP_AON_FUSE_CTL_RB_CFG_XO_CORE_CTRIM_MAN_OFFSET,
        ULP_AON_FUSE_CTL_RB_CFG_XO_CORE_CTRIM_MAN_LEN, xo_ctrim_value);
}

void calibration_xo_core_ctrim_algorithm(bool increase, uint8_t step_num)
{
    uint8_t xo_ctrim_val = (uint8_t)reg32_getbits(ULP_AON_FUSE_CTL_RB_FUSE_CTL_AUTO_RD_36_REG,
        ULP_AON_FUSE_CTL_RB_CFG_XO_CORE_CTRIM_MAN_OFFSET, ULP_AON_FUSE_CTL_RB_CFG_XO_CORE_CTRIM_MAN_LEN);

    if (increase) {
        if ((xo_ctrim_val + step_num) > CALIBRATION_XO_CORE_CTRIM_MAX) {
            xo_ctrim_val = CALIBRATION_XO_CORE_CTRIM_MAX;
        } else {
            xo_ctrim_val = xo_ctrim_val + step_num;
        }
    } else {
        if (xo_ctrim_val < step_num) {
            xo_ctrim_val = CALIBRATION_XO_CORE_CTRIM_MIN;
        } else {
            xo_ctrim_val = xo_ctrim_val - step_num;
        }
    }

    calibration_set_xo_core_ctrim(xo_ctrim_val);
}

void calibration_get_xo_core_ctrim_reg(uint8_t *xo_ctrim_value)
{
    if (xo_ctrim_value == NULL) {
        return;
    }
    *xo_ctrim_value = reg32_getbits(ULP_AON_FUSE_CTL_RB_FUSE_CTL_AUTO_RD_36_REG,
        ULP_AON_FUSE_CTL_RB_CFG_XO_CORE_CTRIM_MAN_OFFSET, ULP_AON_FUSE_CTL_RB_CFG_XO_CORE_CTRIM_MAN_LEN);
}

void calibration_set_xo_core_trim_reg(uint8_t xo_trim)
{
    uint32_t irq_sts = osal_irq_lock();
    reg32_setbits(ULP_AON_FUSE_CTL_RB_FUSE_CTL_AUTO_RD_36_REG, ULP_AON_FUSE_CTL_RB_CFG_XO_CORE_CTRIM_MAN_OFFSET,
        ULP_AON_FUSE_CTL_RB_CFG_XO_CORE_CTRIM_MAN_LEN, xo_trim);
    osal_irq_restore(irq_sts);
}

#ifdef CONFIG_ENABLE_RC_CALIBRATION

uint32_t get_smooth_freq(uint32_t rc_clocks)
{
#define WINDOW_SIZE (5)
    static uint32_t smooth_freq = 0;
    if (smooth_freq == 0) {
        smooth_freq = rc_clocks;
    } else {
        smooth_freq = (smooth_freq * (WINDOW_SIZE - 1) + rc_clocks) / WINDOW_SIZE;
    }
    return smooth_freq;
}

typedef enum {
    CALI_MODE_ORIGIN = 0, // 单次校准取原始频率
    CALI_MODE_SMOOTH,     // 滑动窗口
} cali_mode_t;

volatile int32_t g_set_rc32k_cali_mode = CALI_MODE_ORIGIN;
void hal_set_cali_mode(uint32_t cail_mode)
{
    g_set_rc32k_cali_mode = cail_mode;
}

int32_t hal_get_cali_mode(void)
{
    return g_set_rc32k_cali_mode;
}

void rc32k_cali_handler(void *args)
{
    unused(args);
    notify_alarm(CLOCK_CALIBRATION);
}

static uint32_t g_cali_timer_internal_ms = 10 * 60 * MS_PER_S; // 默认校准间隔
static osTimerId_t g_rc32k_cali_timerId;
void cali_32k_init(void)
{
    uint32_t freq = clocks_calibration_get_freq(0x100);
    PRINT("rc32k_cali_init rc 32k: %.2f Hz.\n", (float)freq / CALI_CLOCK_MUL);
    clocks_calibration_set_freq_result(freq);

    g_rc32k_cali_timerId = osTimerExtNew(rc32k_cali_handler, osTimerPeriodic, NULL, NULL,
        osTimerRousesAllow, osTimerAlignAllow);
    osTimerStart(g_rc32k_cali_timerId, g_cali_timer_internal_ms);
}

void set_cali_timer_internal(uint32_t seconds)
{
    g_cali_timer_internal_ms = seconds * MS_PER_S;
    PRINT("g_cali_timer_internal_ms: %lu\n", g_cali_timer_internal_ms);
    // 重启定时器
    osTimerStop(g_rc32k_cali_timerId);
    osTimerStart(g_rc32k_cali_timerId, g_cali_timer_internal_ms);
}

void cali_32k(uint32_t cali_count, uint32_t update)
{
    if (reg16_getbit(LP_CTL_ULP_A_RB_RC_XO_CLK_MUX_REG, LP_CTL_ULP_A_RB_RC_XO_SEL_OFFSET) != 0) {
        PRINT("not cali\n");
        return;
    }

    uint32_t rc_clocks_100 = clocks_calibration_get_freq(cali_count);
    uint32_t filtered_freq = rc_clocks_100;
    int32_t mode = hal_get_cali_mode();
    if (mode == CALI_MODE_SMOOTH) {
        filtered_freq = get_smooth_freq(rc_clocks_100);
    }
    PRINT("rc 32k: %d Hz, filter_freq: %d Hz\n", rc_clocks_100 / CALI_CLOCK_MUL, ((int32_t)filtered_freq) / CALI_CLOCK_MUL);
    clocks_calibration_set_freq_result(filtered_freq);

    // 通知到绝对时间模块
    if (update) {
        systick_cail_update();
        calendar_cail_update();
    }
}

void rc32k_cali_process(void)
{
#define CALI_COUNT (0x80)
    cali_32k(CALI_COUNT, 1);
}

#endif  // CONFIG_ENABLE_RC_CALIBRATION

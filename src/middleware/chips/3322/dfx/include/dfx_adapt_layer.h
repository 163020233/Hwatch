/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description: dfx adapt layer
 * This file should be changed only infrequently and with great care.
 */
#ifndef DFX_ADAPT_LAYER_H
#define DFX_ADAPT_LAYER_H
#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "securec.h"
#include "dfx_feature_config.h"
#include "dfx_resource_id.h"
#include "diag_config.h"
#include "soc_osal.h"
#include "soc_log.h"
#include "cmsis_os2.h"
#include "core.h"
#include "soc_diag_util.h"
#include "diag_debug.h"
#include "dfx_system_init.h"
#include "diag_dfx.h"
#include "systick.h"
#include "pm_veto.h"
#include "watchdog.h"
#include "debug_print.h"
#include "diag_adapt_flash.h"
#if CORE == APPS
#include "sfc_porting.h"
#include "sfc_v2.h"
#endif

#define LOG_MIN_WATCHDOG_LEFT_TIME                  2000
#define ERASE_LENGTH_4K                             0x1000
#define DUMP_INFO_FLASH_ADDR                        (DFX_FLASH_REGION_START - FLASH_START)
#define MCPU_TRACE_MEM_REGION_LENGTH                0x1200
#define BCPU_TRACE_MEM_REGION_LENGTH                0x2100

typedef enum {
    FLASH_OP_TYPE_OTA,
    FLASH_OP_TYPE_LOG_FILE,
    FLASH_OP_TYPE_DOTTING,
    FLASH_OP_TYPE_FLASH_DATA,
    FLASH_OP_TYPE_USER,
    FLASH_OP_TYPE_DUMP_INFO,
    FLASH_OP_TYPE_MAX,
} dfx_flash_op_type_t;

static inline uint32_t dfx_int_lock(void)
{
    return osal_irq_lock();
}

static inline void dfx_int_restore(uint32_t state)
{
    osal_irq_restore(state);
}

static inline void dfx_sleep(uint32_t ms)
{
    osDelay(ms);
}

static inline void *dfx_malloc(uint32_t id, uint32_t size)
{
    unused(id);
#if (CONFIG_DFX_MEMORY_DEBUG == DFX_YES)
    uint32_t len = size + (uint32_t)sizeof(uint32_t);
    uint32_t *p = (uint32_t *)malloc(len);
    if (p == NULL) {
        return NULL;
    }
    (void)memset_s(p, len, 0, len);
    *p = size;
    diag_dfx_mem_alloc(size);
    return (void *)(p + 1);
#else
    return malloc(size);
#endif
}

static inline void *dfx_zalloc(uint32_t id, uint32_t size)
{
    unused(id);
#if (CONFIG_DFX_MEMORY_DEBUG == DFX_YES)
    uint32_t *p = (uint32_t *)malloc(size + (uint32_t)sizeof(uint32_t));
    if (p == NULL) { return NULL; }
    (void)memset_s(p, size + (uint32_t)sizeof(uint32_t), 0, size + (uint32_t)sizeof(uint32_t));
    *p = size;
    diag_dfx_mem_alloc(size);
    return (void *)(p + 1);
#else
    void *p = malloc(size);
    if (p != NULL) { (void)memset_s(p, size, 0, size); }
    return p;
#endif
}

static inline void *dfx_malloc_permanent(uint32_t id, uint32_t size)
{
    unused(id);
    return dfx_malloc(id, size);
}

static inline void dfx_free(uint32_t id, void *addr)
{
    unused(id);
#if (CONFIG_DFX_MEMORY_DEBUG == DFX_YES)
    uint32_t *p = ((uint32_t *)addr) - 1;
    diag_dfx_mem_free(*p);
    free(p);
#else
    free(addr);
#endif
}

static inline uint32_t dfx_get_cur_second(void)
{
    osal_timeval cur_time = { 0 };
    osal_gettimeofday(&cur_time);

    return (uint32_t)cur_time.tv_sec;
}

static inline uint32_t dfx_get_cur_us(void)
{
    return (uint32_t)uapi_systick_get_us();
}

/* Event 接口 */
static inline errcode_t dfx_event_write(dfx_event_id_t event_id)
{
    unused(event_id);
    return ERRCODE_SUCC;
}

/* Msg 接口 */
static inline errcode_t dfx_msg_write(uint32_t msg_id, uint8_t *msg, uint16_t msg_len, bool wait)
{
    unsigned int timeout = (wait) ? OSAL_MSGQ_WAIT_FOREVER : 0;
    uint8_t msg_data[DFX_MSG_MAX_SIZE + DFX_MSG_ID_LEN];

    *(uint32_t*)msg_data = msg_id;

    if ((msg != NULL) && (memcpy_s(&msg_data[DFX_MSG_ID_LEN], DFX_MSG_MAX_SIZE, msg, msg_len) != EOK)) {
        return ERRCODE_FAIL;
    }

    return (errcode_t)osal_msg_queue_write_copy(dfx_get_osal_queue_id(),
                                                msg_data, DFX_MSG_MAX_SIZE + DFX_MSG_ID_LEN, timeout);
}

/*
 * 文件传输消息发送接口
 * 如文件传输未单独创建线程，可使用dfx_msg_write适配
 * 如文件传输单独创建线程，使用g_dfx_transmit_queue_id发送消息
 */
static inline errcode_t transmit_msg_write(uint32_t msg_id, uint8_t *msg, uint16_t msg_len, bool wait)
{
#if (defined(MEMORY_MINI))
    return dfx_msg_write(msg_id, msg, msg_len, wait);
#else
    unsigned int timeout = (wait) ? OSAL_MSGQ_WAIT_FOREVER : 0;
    uint8_t msg_data[DFX_MSG_MAX_SIZE + DFX_MSG_ID_LEN];

    *(uint32_t*)msg_data = msg_id;

    if ((msg != NULL) && (memcpy_s(&msg_data[DFX_MSG_ID_LEN], DFX_MSG_MAX_SIZE, msg, msg_len) != EOK)) {
        return ERRCODE_FAIL;
    }

    return (errcode_t)osal_msg_queue_write_copy(dfx_get_transmit_queue_id(),
                                                msg_data, DFX_MSG_MAX_SIZE + DFX_MSG_ID_LEN, timeout);
#endif
}

static inline int32_t dfx_msg_queue_is_full(unsigned long queue_id)
{
    return osal_msg_queue_is_full(queue_id);
}

/* TIMER接口 */
typedef osal_timer dfx_timer;
typedef void (*dfx_timer_handler)(uintptr_t data);
typedef void (*temp_osal_handler)(unsigned long data);
static inline errcode_t dfx_timer_init(dfx_timer *timer, dfx_timer_handler handler, uintptr_t data, uint32_t ms)
{
    timer->handler = (temp_osal_handler)handler;
    timer->data = data;
    timer->interval = ms;
    return osal_timer_init(timer);
}

static inline errcode_t dfx_timer_start(dfx_timer *timer, uint32_t ms)
{
    unused(ms);
    return osal_timer_start(timer);
}

static inline errcode_t dfx_timer_stop(dfx_timer *timer)
{
    return osal_timer_stop(timer);
}

static inline errcode_t dfx_timer_destroy(dfx_timer *timer)
{
    return osal_timer_destroy(timer);
}

/* machine 接口 */
static inline uint32_t dfx_machine_get_id(void)
{
    return 0;
}

static inline char *dfx_machine_get_name(void)
{
#if CORE == BT
    return "bt";
#elif CORE == APPS
    return "apps";
#else
    return "unknown";
#endif
}

/* fault 处理 */
static inline void dfx_fault_event_data(uint32_t event_id, uint8_t *data, uint16_t len)
{
    unused(event_id);
    unused(data);
    unused(len);
}

#if CORE == APPS
/* flash 读写 */
static inline int32_t dfx_flash_read(uint8_t opt_type, uint32_t offset, uint8_t *buf, uint32_t size)
{
    return diag_adapt_flash_read(opt_type, offset, buf, size);
}

static inline int32_t dfx_flash_write(uint8_t opt_type, uint32_t offset, const uint8_t *buf, uint32_t size,
    bool do_erase)
{
    return diag_adapt_flash_write(opt_type, offset, buf, size, do_erase);
}

/*
 * flash擦除数据接口
 */
static inline errcode_t dfx_flash_erase(uint8_t opt_type, const uint32_t offset, const uint32_t size)
{
    return diag_adapt_flash_erase(opt_type, offset, size);
}

uint32_t dfx_flash_get_region_size(uint8_t opt_type);

#endif

static inline void dfx_watchdog_kick(void)
{
    uint32_t wdg_left_time;
    uapi_watchdog_get_left_time(&wdg_left_time);
    if (wdg_left_time < LOG_MIN_WATCHDOG_LEFT_TIME) {
        uapi_watchdog_kick();
    }
}

static inline errcode_t dfx_pm_add_sleep_veto(void)
{
    return uapi_pm_add_sleep_veto(PM_ID_DFX);
}

static inline errcode_t dfx_pm_remove_sleep_veto(void)
{
    return uapi_pm_remove_sleep_veto(PM_ID_DFX);
}

static inline errcode_t dfx_log_get_level(uint8_t core, uint8_t *level)
{
    unused(core);
    unused(level);
#ifdef HI3322_ADAPT
    get_log_level((cores_t)core, (log_level_e *)level);
#endif
    return ERRCODE_SUCC;
}

static inline errcode_t dfx_log_set_level(uint8_t core, uint8_t level)
{
    unused(core);
    unused(level);
#ifdef HI3322_ADAPT
    set_log_level((cores_t)core, (log_level_e)level);
#endif
    return ERRCODE_SUCC;
}

#define dfx_log_debug(fmt, ...) \
    do { \
        if (diag_get_debug_level() >= DIAG_LOG_LEVEL_DBG) { \
            PRINT(fmt, ##__VA_ARGS__); \
        } \
    } while (0)

#define dfx_log_info(fmt, ...) \
    do { \
        if (diag_get_debug_level() >= DIAG_LOG_LEVEL_INFO) { \
            PRINT(fmt, ##__VA_ARGS__); \
        } \
    } while (0)

#define dfx_log_err(fmt, ...) \
    do { \
        if (diag_get_debug_level() >= DIAG_LOG_LEVEL_ERROR) { \
            PRINT(fmt, ##__VA_ARGS__); \
        } \
    } while (0)

#define dfx_assert(x) ((void)0)

#endif /* DFX_ADAPT_LAYER_H */

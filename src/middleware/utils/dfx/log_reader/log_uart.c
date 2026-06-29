/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:   LOG UART MODULE
 *
 * Create:
 */
#if defined USE_CMSIS_OS
#include <string.h>
#ifdef USE_OSAL_INSTEAD_CMSIS
#include "soc_osal.h"
#include "los_sched_pri.h"
#else
#include "cmsis_os2.h"
#endif
#include "core.h"
#if CORE == MASTER_BY_ALL
#include "stdio.h"
#if (CORE_NUMS  > 1)
#include "watchdog.h"
#endif
#include "hal_uart.h"
#endif
#ifdef TEST_SUITE
#include "test_suite_uart.h"
#endif
#include "log_panic.h"
#include "log_buffer.h"
#include "log_buffer_reader.h"
#include "log_trigger.h"
#include "panic.h"
#include "non_os.h"
#ifdef SUPPORT_IPC
#include "ipc.h"
#endif

#include "uart.h"
#if defined(CONFIG_UART_SUPPORT_DMA)
#include "dma.h"
#endif  /* CONFIG_UART_SUPPORT_DMA */
#include "log_uart_porting.h"
#include "log_uart.h"
#ifdef HSO_SUPPORT
#include "securec.h"
#include "diag_adapt_sdt.h"
#endif
#ifdef IPC_NEW
#include "ipc.h"
#include "ipc_porting.h"
#endif
#include "diag_log.h"
#include "dfx_adapt_layer.h"
#ifdef LOG_TRIGGER_SET
#include "log.h"
#endif
#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE) && (CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE == DFX_YES))
#include "massdata_reader.h"
#include "massdata_trigger.h"
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE */
#include "log_uart.h"
#ifdef CFG_DRIVERS_USB_AS_SERIAL
#include "usb_porting.h"
#endif

#if CORE == MASTER_BY_ALL
/**
 * Security core logging watermark level.
 * When the level is under the watermark and the Security core goes to sleep the log will stop until retriggered. */
static uint16_t g_log_uart_security_log_watermark = LOG_UART_LOG_WATERMARK_VALUE_ON_INIT;
static uint8_t g_uart_log_rx_buffer_test[LOG_UART_RX_MAX_BUFFER_SIZE];
#endif

/** Time(ms) of the oldest message left in the logging buffer. */
#define OLDEST_MESSAGE_IN_BUFFER 1000
#define MASSDATA_FLUSH_PERIOD    5000

#ifdef SW_UART_DEBUG
#ifndef SW_UART_BAUDRATE
#define SW_UART_BAUDRATE 115200UL
#endif
#endif

/** UART Settings. Define these in the C file to avoid pulling in the UART header in the header file. */
#ifdef LOG_UART_CUSTOMER_DEFINE_BAUD_RATE
#define LOG_UART_BAUD_RATE LOG_UART_CUSTOMER_DEFINE_BAUD_RATE
#else
#define LOG_UART_BAUD_RATE 115200
#endif
#define LOG_UART_DATA_BITS UART_DATA_BIT_8
#define LOG_UART_STOP_BITS UART_STOP_BIT_1
#define LOG_UART_PARITY    UART_PARITY_NONE

/** Number of milliseconds to delay the trigger of a new UART write in case the UART is busy */
#define LOG_UART_DELAY_ON_UART_BUSY 1
#if CORE == MASTER_BY_ALL

#define LOG_UART_EVENT_MASK     0x3
#define LOG_UART_EVENT_LOG_BIT  0x1
#define LOG_UART_EVENT_MASS_BIT 0x2

/** Mutex protecting accesses to the circular buffer. */
#ifdef USE_OSAL_INSTEAD_CMSIS
static osal_semaphore g_semaphore_uart = { NULL };
#else
static osSemaphoreId_t g_semaphore_uart = NULL;
#endif

#if (defined(CONFIG_DFX_SUPPORT_LOG_BUFFER_BLOCK_READ) && CONFIG_DFX_SUPPORT_LOG_BUFFER_BLOCK_READ == DFX_YES)
static log_reader_callback g_log_reader = NULL;
#endif

#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE) && (CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE == DFX_YES))
static osal_event g_event_logs;
static massdata_reader_callback g_massdata_reader = NULL;
static osTimerId_t g_massdata_flush_timer = NULL;
STATIC void massdata_trigger_callback(void *pay_i, uint8_t core, uint8_t type);
STATIC void massdata_retrigger_from_timer(void *arg);
STATIC void massdata_trigger_init(void);
#else /* CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE */
#ifdef USE_OSAL_INSTEAD_CMSIS
static osal_semaphore g_semaphore_logs = { NULL };
#else
static osSemaphoreId_t g_semaphore_logs = NULL;
#endif /* USE_OSAL_INSTEAD_CMSIS */
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE */

/** UART Handle */
static uart_bus_t g_log_uart = LOG_UART_BUS;

#if defined(CONFIG_UART_SUPPORT_DMA)
#define DMA_UART_TRANSFER_WIDTH_WORD        4
#define DMA_UART_TRANSFER_WIDTH_HALF_WORD   2
#define DMA_UART_TRANSFER_WIDTH_8           0
#define DMA_UART_TRANSFER_WIDTH_16          1
#define DMA_UART_TRANSFER_WIDTH_32          2
#define DMA_UART_BURST_TRANSACTION_LENGTH_8 2
#endif  /* CONFIG_UART_SUPPORT_DMA */

/* Local function prototypes. */
STATIC void log_uart_tx_isr_callback(const void *buffer, uint32_t length, const void *params);

/**
 * This handler gets called when one of the cores wants to trigger the log reader
 * @param message   The IPC message
 * @param payload_p The payload, contains the reset reason
 * @param src       The core who has just started
 * @param id        The message ID (should always be 0 - the first message after starting)
 * @return
 */
#if (CORE_NUMS  > 1) && defined(IPC_NEW)
static void log_reader_info_action_handler(uint8_t *payload_addr, uint32_t payload_len)
{
    unused(payload_addr);
    unused(payload_len);
    log_uart_trigger();
}
#elif (CORE_NUMS > 1) && (defined CONFIG_DFX_SUPPORT_CUSTOM_LOG) && (CONFIG_DFX_SUPPORT_CUSTOM_LOG == DFX_YES)
void log_reader_info_action_handler(void)
{
    dfx_log_reader_irq_clr();      /* Clear Software interrupt. */
    log_uart_trigger();
}

#elif (CORE_NUMS  > 1)
static bool log_reader_info_action_handler(ipc_action_t message,
                                           const volatile ipc_payload *payload_p, cores_t src, uint32_t id)
{
    unused(message);
    unused(id);
    unused(payload_p);
    unused(src);
    log_uart_trigger();
    return true;
}
#endif


/* Initialise the UART and open it at the specified speed! */
void log_uart_init(void)
{
#if (CORE_NUMS > 1)
    unused(log_reader_info_action_handler);
#endif
    uart_attr_t uart_line_config = {
        .baud_rate = LOG_UART_BAUD_RATE,
        .data_bits = LOG_UART_DATA_BITS,
        .stop_bits = LOG_UART_STOP_BITS,
        .parity = LOG_UART_PARITY
    };
    uart_buffer_config_t uart_buffer_config;

    uart_buffer_config.rx_buffer_size = LOG_UART_RX_MAX_BUFFER_SIZE;
    uart_buffer_config.rx_buffer = g_uart_log_rx_buffer_test;

    /* Open and configure the UART */
    g_log_uart = LOG_UART_BUS;
#if defined(CONFIG_UART_SUPPORT_DMA)
    uart_extra_attr_t extra_attr = {
        .tx_dma_enable = 1,
        .tx_int_threshold = 0,
        .rx_dma_enable = 1,
        .rx_int_threshold = 0
    };

    uapi_dma_init();
    uapi_dma_open();
    (void)uapi_uart_init(LOG_UART_BUS, NULL, &uart_line_config, &extra_attr, &uart_buffer_config);
#else
    (void)uapi_uart_init(LOG_UART_BUS, NULL, &uart_line_config, NULL, &uart_buffer_config);
#endif  /* CONFIG_UART_SUPPORT_DMA */

#if SYS_DEBUG_MODE_ENABLE == YES
    uapi_uart_register_rx_callback(LOG_UART_BUS, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                   LOG_UART_RX_MAX_BUFFER_SIZE, log_uart_rx_callback);
#endif
}

void log_uart_send_buffer(const uint8_t *buffer, uint16_t length)
{
    uint32_t waite_forever = 0xFFFFFFFF;
#ifdef CFG_DRIVERS_USB_AS_SERIAL
    uapi_usb_serial_write(USB_SERIAL_1,  (const void *)buffer, length);
#else
#ifdef CONFIG_UART_LOG_WRITE_WITH_NOLOCK
    uapi_uart_write_nolock(g_log_uart, (const void *)buffer, length, waite_forever);
#else
    uapi_uart_write(g_log_uart, (const void *)buffer, length, waite_forever);
#endif
#endif
}

void log_uart_update_watermark_level(log_memory_region_section_t log_mem_region, uint16_t new_watermark_val)
{
#if CORE_NUMS > 1
    if (log_mem_region == LOG_MEMORY_REGION_SECTION_1) {
        g_log_uart_security_log_watermark = new_watermark_val;
    }
#else
    if (log_mem_region == LOG_MEMORY_REGION_SECTION_0) {
        g_log_uart_security_log_watermark = new_watermark_val;
    }
#endif
}

/* Initialises the UART when RTOS functions are available. This should be called before the RTOS scheduler starts,
 * but when we can create the mutex.
 */
// Run after OS, but before threads are started, so in a single thread.
void log_uart_init_after_rtos(void)
{
    /* Create the mutex used to control logs written to . */
#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE) && (CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE == DFX_YES))
    if (osal_event_init(&g_event_logs) != OSAL_SUCCESS) {
#else
#ifdef USE_OSAL_INSTEAD_CMSIS
    if (osal_sem_binary_sem_init(&g_semaphore_logs, 1) != OSAL_SUCCESS) {
#else
    g_semaphore_logs = osSemaphoreNew(1, 0, NULL);
    if (!g_semaphore_logs) {
#endif /* USE_OSAL_INSTEAD_CMSIS */
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE */
        panic(PANIC_LOG, LOG_PANIC_MUTEX_NOT_READY);
        return;
    }

    register_log_trigger(log_uart_trigger);

#ifdef USE_OSAL_INSTEAD_CMSIS
    if (osal_sem_binary_sem_init(&g_semaphore_uart, 1) != OSAL_SUCCESS) {
#else
    g_semaphore_uart = osSemaphoreNew(1, 0, NULL);
    if (!g_semaphore_uart) {
#endif
        panic(PANIC_LOG, LOG_PANIC_MUTEX_NOT_READY);
        return;
    }

#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE) && (CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE == DFX_YES))
    massdata_trigger_init();
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE */

    log_reader_ret_t lret = log_buffer_reader_init();
    if (lret != LOG_READER_RET_OK) {
        panic(PANIC_LOG, __LINE__);
        return;
    }

    // Register IPC handler to handle log trigger notifications from other cores
#if (CORE_NUMS > 1) && defined(IPC_NEW)
    if (non_os_is_driver_initialised(DRIVER_INIT_IPC) == false) {
        uapi_ipc_init();
    }
    ipc_rx_handler_info_t handler_info;
    handler_info.msg_id = IPC_MSG_LOG_INFO;
    handler_info.cb = log_reader_info_action_handler;
    (void)uapi_ipc_register_rx_handler(&handler_info);
#elif (CORE_NUMS > 1) && (defined CONFIG_DFX_SUPPORT_CUSTOM_LOG) && (CONFIG_DFX_SUPPORT_CUSTOM_LOG == DFX_YES)
    dfx_log_read_info_init();
#elif (CORE_NUMS > 1)
    if (non_os_is_driver_initialised(DRIVER_INIT_IPC) == false) {
        ipc_init();
    }

    ipc_register_handler(IPC_ACTION_LOG_INFO, log_reader_info_action_handler);
#endif
}

/*
 * UART has trasnmitted some data. Call in interrupt context,
 * simply schedule the read index update and additional data check.
 */
STATIC void log_uart_tx_isr_callback(const void *buffer, uint32_t length, const void *params)
{
    unused(buffer);
    unused(params);
    unused(length);

    /* Pass the amount of data actually transmitted, so we can update the read pointer. */
#ifdef USE_OSAL_INSTEAD_CMSIS
    osal_sem_up(&g_semaphore_uart);
#else
    (void)osSemaphoreRelease(g_semaphore_uart);
#endif
}

void log_uart_trigger(void)
{
    /* Give the semaphore to ensure the main logging task will be active */
#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE) && (CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE == DFX_YES))
    osal_event_write(&g_event_logs, LOG_UART_EVENT_LOG_BIT);
#else /* CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE */
#ifdef USE_OSAL_INSTEAD_CMSIS
    osal_sem_up(&g_semaphore_logs);
#else
    (void)osSemaphoreRelease(g_semaphore_logs);
#endif /* USE_OSAL_INSTEAD_CMSIS */
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE */
}

#if defined(CONFIG_UART_SUPPORT_DMA)
static inline bool uart_dma_align_check_word(uint32_t data_buf, uint32_t data_len, uint32_t align_data)
{
    bool ret1 = (data_buf % align_data) == 0 ? true : false;
    bool ret2 = (data_len % align_data) == 0 ? true : false;
    return ret1 && ret2;
}

static uint8_t uart_dma_get_mem_width(uint32_t buff, uint32_t bytes)
{
    if (uart_dma_align_check_word(buff, bytes, (uint32_t)DMA_UART_TRANSFER_WIDTH_WORD)) {
        return DMA_UART_TRANSFER_WIDTH_32;
    }
    if (uart_dma_align_check_word(buff, bytes, (uint32_t)DMA_UART_TRANSFER_WIDTH_HALF_WORD)) {
        return DMA_UART_TRANSFER_WIDTH_16;
    }
    return DMA_UART_TRANSFER_WIDTH_8;
}

static uart_write_dma_config_t dma_cfg = { 0 };
static void log_uart_write_blocking_dma(const void *buf, uint32_t len)
{
    dma_cfg.src_width = uart_dma_get_mem_width((uint32_t)(uintptr_t)buf, len);
    dma_cfg.dest_width = DMA_UART_TRANSFER_WIDTH_8;
    dma_cfg.burst_length = DMA_UART_BURST_TRANSACTION_LENGTH_8;
    if (uapi_uart_write_by_dma(g_log_uart, buf, (uint16_t)len, &dma_cfg) != (int32_t)len) {
        return;
    }
    while (!uapi_uart_tx_fifo_is_empty(g_log_uart)) {};
}
#endif  /* CONFIG_UART_SUPPORT_DMA */

static void log_uart_write_blocking_int(const void *buf, uint32_t len)
{
    // Write The buffer to the uart
    while (uapi_uart_write_int(g_log_uart, buf, (uint16_t)len, NULL, log_uart_tx_isr_callback) != ERRCODE_SUCC) {
        // If it was unsuccessful try again after some ticks
#ifdef USE_OSAL_INSTEAD_CMSIS
        if (OS_SCHEDULER_ALL_ACTIVE) {
            (void)osal_msleep(LOG_UART_DELAY_ON_UART_BUSY);
        }
#else
        if (osKernelGetState() == osKernelRunning) {
            (void)osDelay(LOG_UART_DELAY_ON_UART_BUSY);
        }
#endif
    }

    // Wait until the uart write has been completed
#ifdef USE_OSAL_INSTEAD_CMSIS
    if (OS_SCHEDULER_ALL_ACTIVE && osal_sem_down(&g_semaphore_uart) != OSAL_SUCCESS) {
        panic(PANIC_LOG, LOG_PANIC_MUTEX_NOT_READY);
        return;
    }
#else
    if (osKernelGetState() == osKernelRunning && osSemaphoreAcquire(g_semaphore_uart, osWaitForever) != osOK) {
        panic(PANIC_LOG, LOG_PANIC_MUTEX_NOT_READY);
        return;
    }
#endif
    else {
        while (!uapi_uart_tx_fifo_is_empty(g_log_uart)) {};
    }
}

void log_uart_write_blocking(const void *buf, uint32_t len)
{
#if defined(CONFIG_UART_SUPPORT_DMA)
    if (len <= CONFIG_UART_FIFO_DEPTH) {
        log_uart_write_blocking_int(buf, len);
    } else {
        log_uart_write_blocking_dma(buf, len);
    }
#else
    log_uart_write_blocking_int(buf, len);
#endif  /* CONFIG_UART_SUPPORT_DMA */
}

#define IBRD_NEED_BAUD_OFFSET_NUM      3
#define REMAINDER_NEED_BAUD_OFFSET_NUM 3
#define FBRD_NEED_REMAINDER_OFFSET_NUM 4

static void log_uart_set_baud_rate(uart_bus_t bus, uint32_t baud)
{
    uart_attr_t uart_line_config = {
        .baud_rate = baud,
        .data_bits = LOG_UART_DATA_BITS,
        .stop_bits = LOG_UART_STOP_BITS,
        .parity = LOG_UART_PARITY
    };
    uapi_uart_set_attr(bus, &uart_line_config);
}

void log_uart_reset_baud_rate(void)
{
    log_uart_set_baud_rate(LOG_UART_BUS, LOG_UART_BAUD_RATE);

#ifdef SW_UART_DEBUG
    log_uart_set_baud_rate(SW_DEBUG_UART_BUS, SW_UART_BAUDRATE);
#endif
#ifdef TEST_SUITE
    log_uart_set_baud_rate(TEST_SUITE_UART_BUS, TEST_SUITE_UART_BAUD_RATE);
#endif
}

#if (defined(CONFIG_DFX_SUPPORT_LOG_BUFFER_BLOCK_READ) && CONFIG_DFX_SUPPORT_LOG_BUFFER_BLOCK_READ == DFX_YES)
void log_buffer_reader_register_handle(log_reader_callback cb)
{
    if (cb != NULL) {
        g_log_reader = cb;
    }
}

STATIC void log_uart_check_log_buffer(void)
{
    log_reader_ret_t lr_ret;
    log_memory_region_section_t lregion;
    uint8_t *b1 = NULL;
    uint32_t l1 = 0;
    uint8_t *b2 = NULL;
    uint32_t l2 = 0;
    errcode_t len_ret = 0;

    while (log_buffer_reader_get_next_lock(&lregion) == LOG_READER_RET_OK) {
        lr_ret = log_buffer_reader_get_data(lregion, &b1, &l1, &b2, &l2);
        if (lr_ret != LOG_READER_RET_OK) {
            uapi_diag_error_log(0, "[log_uart]data error, r1=0x%x, len1=0x%x, r2=0x%x, len2=0x%x",
                (uintptr_t)b1, l1, (uintptr_t)b2, l2);
            log_buffer_reader_error_recovery(lregion);
            break;
        }

        if (g_log_reader != NULL) {
            len_ret = g_log_reader(b1, l1, b2, l2);
        }

        lr_ret = log_buffer_reader_discard_by_len(lregion, l1 + l2);
        if (lr_ret != LOG_READER_RET_OK) {
            uapi_diag_error_log(0, "[log_uart]discard error, lregion=0x%x, len_ret=0x%x", lregion, len_ret);
            log_buffer_reader_error_recovery(lregion);
        }

        dfx_watchdog_kick();
    }
}
#else /* CONFIG_DFX_SUPPORT_LOG_BUFFER_BLOCK_READ */
STATIC void log_uart_check_log_buffer(void)
{
    log_reader_ret_t lr_ret;
    log_memory_region_section_t lregion;
    log_buffer_header_t lb_header = { 0 };
    uint8_t *b1 = NULL;
    uint32_t l1 = 0;
    uint8_t *b2 = NULL;
    uint32_t l2 = 0;

    // Check if there are messages
    while (log_buffer_reader_lock_next(&lregion, &lb_header) == LOG_READER_RET_OK) {
        // Claim the message available
        lr_ret = log_buffer_reader_claim_next(lregion, &b1, &l1, &b2, &l2);
        // we are sure there is a new message
        if ((lr_ret != LOG_READER_RET_OK) || ((lb_header.length - sizeof(lb_header)) != (l1 + l2))) {
            uapi_diag_error_log(0, "[log_uart]data error, r1=0x%x, len1=0x%x, r2=0x%x, len2=0x%x",
                (uintptr_t)b1, l1, (uintptr_t)b2, l2);
            log_buffer_reader_error_recovery(lregion);
            break;
        }
#ifdef HSO_SUPPORT
        zdiag_adapt_sdt_msg_proc(b1, l1, b2, l2);
#else
        /* Copy SYNC, TIME, SEQUENCE and MSG to circular buffer. */
        if  (l1 > 0) {
#ifdef CFG_DRIVERS_USB_AS_SERIAL
            uapi_usb_serial_write(USB_SERIAL_1, (const void *)b1, l1);
#else
            if (g_log_uart != UART_BUS_NONE) {
                log_uart_write_blocking((const void *)b1, l1);
            }
#endif
        }
        if (l2 > 0) {
#ifdef CFG_DRIVERS_USB_AS_SERIAL
            uapi_usb_serial_write(USB_SERIAL_1, (const void *)b2, l2);
#else
            if (g_log_uart != UART_BUS_NONE) {
                log_uart_write_blocking((const void *)b2, l2);
            }
#endif
        }
#endif
        log_buffer_reader_discard(lregion);
        dfx_watchdog_kick();
#ifdef LOG_TRIGGER_SET
        if (log_get_trigger() == true) {
            log_set_trigger(false);
        }
#endif
    }
}
#endif /* CONFIG_DFX_SUPPORT_LOG_BUFFER_BLOCK_READ */

#if (defined(CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE) && (CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE == DFX_YES))

STATIC void massdata_trigger_callback(void *pay_i, uint8_t core, uint8_t type)
{
    unused(pay_i);
    unused(core);
    unused(type);
    osal_event_write(&g_event_logs, LOG_UART_EVENT_MASS_BIT);
}

STATIC void massdata_retrigger_from_timer(void *arg)
{
    unused(arg);
    osal_event_write(&g_event_logs, LOG_UART_EVENT_MASS_BIT);
}

STATIC void massdata_trigger_init(void)
{
    register_massdata_trigger(massdata_trigger_callback);
    g_massdata_flush_timer = osTimerNew(massdata_retrigger_from_timer, osTimerPeriodic,
                                        (void *)LOG_UART_EVENT_MASS_BIT, NULL);
    if (!g_massdata_flush_timer) {
        panic(PANIC_LOG, LOG_PANIC_TIMER_INIT);
    }
}

STATIC void log_uart_check_massdata_buffer(void)
{
    massdata_reader_ret_t lr_ret;
    massdata_memory_region_section_t mregion;
    uint8_t *b1 = NULL;
    uint32_t l1 = 0;
    uint8_t *b2 = NULL;
    uint32_t l2 = 0;

    // Check mass data
    lr_ret = massdata_reader_has_next(&mregion);
    while (lr_ret == MASSDATA_READER_RET_OK) {
        lr_ret = massdata_reader_get_next(mregion, &b1, &l1, &b2, &l2);
        if (lr_ret != MASSDATA_READER_RET_OK) {
            uapi_diag_error_log(0, "[log_uart]data error, r1=0x%x, len1=0x%x, r2=0x%x, len2=0x%x",
                (uintptr_t)b1, l1, (uintptr_t)b2, l2);
            break;
        }

        if (g_massdata_reader != NULL) {
            g_massdata_reader(b1, l1, b2, l2);
        }

        lr_ret = massdata_reader_discard(mregion, l1 + l2);

        lr_ret = massdata_reader_has_next(&mregion);
        if (lr_ret == MASSDATA_READER_RET_THERE_IS_NO_NEXT_MESSAGE) {
            break;
        }

        dfx_watchdog_kick();
    }
}

void log_main(const void *unused_p)
{
    unused(unused_p);

    int32_t event = LOG_UART_EVENT_MASK;

    for (;;) {
        if ((((uint32_t)event) & LOG_UART_EVENT_LOG_BIT) != 0) {
            log_uart_check_log_buffer();
        }

        if ((((uint32_t)event) & LOG_UART_EVENT_MASS_BIT) != 0) {
            log_uart_check_massdata_buffer();
        }

        // If there has been log indicator get run again
#ifdef USE_OSAL_INSTEAD_CMSIS
        if (OS_SCHEDULER_ALL_ACTIVE) {
            event = osal_event_read(&g_event_logs, LOG_UART_EVENT_MASK, OSAL_WAIT_FOREVER,
                                    OSAL_WAITMODE_OR | OSAL_WAITMODE_CLR);
            if (event == OSAL_FAILURE) {
                panic(PANIC_LOG, LOG_PANIC_MUTEX_NOT_READY);
            }
        }

#else
        if (osKernelGetState() == osKernelRunning) {
            event = osal_event_read(&g_event_logs, LOG_UART_EVENT_MASK, OSAL_WAIT_FOREVER,
                                    OSAL_WAITMODE_OR | OSAL_WAITMODE_CLR);
            if (event == OSAL_FAILURE) {
                panic(PANIC_LOG, LOG_PANIC_MUTEX_NOT_READY);
            }
        }
#endif /* USE_OSAL_INSTEAD_CMSIS */
#ifdef UT_TEST
        break;
#endif
    }
}

void massdata_register_reader_callback(massdata_reader_callback callback)
{
    if (g_massdata_reader == callback) {
        return;
    }
    if (g_massdata_reader == NULL) {
        (void)osTimerStop(g_massdata_flush_timer);
        osal_event_write(&g_event_logs, LOG_UART_EVENT_MASS_BIT); // Clear massdata buffer
    }
    g_massdata_reader = callback;
    if (g_massdata_reader != NULL) {
        (void)osTimerStart(g_massdata_flush_timer, MASSDATA_FLUSH_PERIOD);
    }
}

#else /* CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE */

void log_main(const void *unused_p)
{
    unused(unused_p);

    for (;;) {
        log_uart_check_log_buffer();
        // If there has been log indicator get run again
#ifdef USE_OSAL_INSTEAD_CMSIS
        if (OS_SCHEDULER_ALL_ACTIVE && osal_sem_down(&g_semaphore_logs) != OSAL_SUCCESS) {
            panic(PANIC_LOG, LOG_PANIC_MUTEX_NOT_READY);
        }
#else
        if (osKernelGetState() == osKernelRunning && osSemaphoreAcquire(g_semaphore_logs, osWaitForever) != osOK) {
            panic(PANIC_LOG, LOG_PANIC_MUTEX_NOT_READY);
        }
#endif /* USE_OSAL_INSTEAD_CMSIS */
#ifdef UT_TEST
        break;
#endif
    }
}

#endif /* CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE */

#endif /* CORE == MASTER_BY_ALL */

#endif  // defined USE_CMSIS_OS

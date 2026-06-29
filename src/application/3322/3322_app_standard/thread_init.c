/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description:   Application Core OS Initialize Interface for Standard
 * Author:
 * Create: 2021-03-09
 */
#include "thread_init.h"
#include "tcxo.h"
#include "log_common.h"
#include "log_def.h"
#include "log_uart.h"
#include "log_file.h"
#include "los_memory.h"
#include "los_tick.h"
#include "pmu_interrupt.h"
#include "log_reg_dump.h"
#include "watchdog.h"
#include "preserve.h"
#include "hal_reboot.h"
#include "pinctrl_porting.h"
#include "pm_sleep.h"
#include "pm_pmu.h"
#include "dfx_system_init.h"
#include "cpu_load.h"
#include "systick.h"
#include "systick_porting.h"
#include "sys/time.h"
#include "time.h"
#include "diag_log.h"
#include "cmsis_os.h"
#include "clocks_switch.h"
#include "pm_pmu.h"
#include "temperature.h"
#ifdef DEMO_WATCH
#include "ux_api_fwk.h"
#endif

#ifdef AT_COMMAND
#include "at_product.h"
#endif

#include "clocks_config.h"

#include "ipc.h"
#include "ipc_porting.h"

#ifdef SUPPORT_AUTO_OTA
#include "auto_ota_service.h"
#endif

#ifdef SUPPORT_VOLTE
#include "cat1_net_interface.h"
#include "imsa_interface.h"
#endif
#ifdef SUPPORT_LWIP
#include "lwip/tcpip.h"
#ifdef SUPPORT_VOLTE
#include "lwip_public.h"
#endif
#endif
#include "clock_calibration.h"
#include "calendar_porting.h"
#ifdef PLT_SUPPORT_CHR
#include "plt_chr.h"
#endif

#define TASK_PRIORITY_APP           (osPriority_t)(17)
#define TASK_PRIORITY_LOG           (osPriority_t)(18)
#define TASK_PRIORITY_CMD           (osPriority_t)(30)
#define TASK_PRIORITY_DFX_MSG       (osPriority_t)(17)
#define TASK_PRIORITY_TRANSMIT      (osPriority_t)(19)
#define TASK_PRIORITY_BTH_SDK       (osPriority_t)(19)
#define TASK_PRIORITY_SRV           (osPriority_t)(18)
#define TASK_PRIORITY_BT_IPC_RECV   (osPriority_t)(26)
#define TASK_PRIORITY_SLEH_SAF      (osPriority_t)(17)
#define TASK_PRIORITY_UX_TASK       (osPriority_t)(17)
#define TASK_PRIORITY_LWIP          (osPriority_t)(17)
#define TASK_PRIORITY_DFX_HCD       (osPriority_t)(19)

#ifdef SUPPORT_AUTO_OTA
#define TASK_PRIORITY_AUTO_OTA      (osPriority_t)(36)
#endif

#ifdef SUPPORT_VOLTE
#define TASK_PRIORITY_IMSA          (osPriority_t)(33)
#define TASK_PRIORITY_IMSA_PIP_UART (osPriority_t)(26)
#define TASK_PRIORITY_IMSA_PIP_SPI  (osPriority_t)(34)
#endif

#define BTH_SDK_STACK_SIZE      0xC00
#define DFX_MSG_STACK_SIZE      0x1e00
#define APP_MAIN_STACK_SIZE     0xF00
#define BTH_SERVICE_STACK_SIZE  0x1000
#define BTH_IPC_RECV_STACK_SIZE 0xC00
#define TASK_STAKDEPTH_SLEH_SAF 0xC00
#define LWIP_MAIN_STACK_SIZE    0xC00
#define DFX_HCD_STACK_SIZE      0xf00

#ifdef SUPPORT_AUTO_OTA
#define AUTO_OTA_STACK_SIZE         0xC00
#endif

#ifdef SUPPORT_VOLTE
#define IMSA_PROCESS_STACK_SIZE         0x2000
#define IMSA_PROCESS_IMS_PIP_UART_SIZE  0x1000
#define IMSA_PROCESS_IMS_PIP_SPI_SIZE   0x1000
#endif

#ifdef AT_IPC_V1_TEST
#define AT_STACK_SIZE           0x4d00
#else
#define AT_STACK_SIZE           0x2000
#endif

#define UX_TASK_STACK_SIZE      0x1000

#define TEMPERATURE_INCREASE_VALID_CYCLE 10
#define TEMPERATURE_ADJUST_VOL_THRESHOLD (-20)

static uint32_t g_temperature_inc_times = 0;
static osTimerId_t g_app_main_timerId;
static osSemaphoreId_t g_semaphore_app_main = NULL;

#if SUPPORT_LWIP
static void lwip_main(void);
#endif
static void app_main(const void *unused);
void bt_acore_task_main(const void *pvParams);
int msg_process_thread(void *data);
errcode_t thread_msg_event_init(void);
int transmit_process_thread(void *data);
errcode_t transmit_thread_msg_init(void);
void diag_report_consumer_task(void *arg); // DIAG_REPORT
void bt_acore_task_main(const void *pvParams);
void btsrv_task_body(const void *data);
void bgh_ipc_recv_task(const void *arg);
void saf_task_main(void const * params);
void sle_service_init(void);
void GnssMsgProcHandle(void *param);

static volatile uint64_t g_time_base;

app_task_definition_t g_app_tasks[] = {
    { { "app", 0, NULL, 0, NULL, APP_MAIN_STACK_SIZE, TASK_PRIORITY_APP, 0, 0 },
      (osThreadFunc_t)app_main, NULL, NULL, USE_DTCM_MEM },
#ifndef XTS_SUPPORT
#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
    { { "log", 0, NULL, 0, NULL, (0xD00), TASK_PRIORITY_LOG, 0, 0 },
      (osThreadFunc_t)log_main, NULL, NULL, USE_DTCM_MEM },
#endif
#endif
#ifdef AT_COMMAND
    { { "at", 0, NULL, 0, NULL, AT_STACK_SIZE, TASK_PRIORITY_CMD, 0, 0 },
      (osThreadFunc_t)uapi_at_msg_main, NULL, NULL, USE_DTCM_MEM },
#endif
#ifdef HSO_SUPPORT
    { { "dfx_msg", 0, NULL, 0, NULL, DFX_MSG_STACK_SIZE, TASK_PRIORITY_DFX_MSG, 0, 0 },
      (osThreadFunc_t)msg_process_thread, NULL, (CreateQueueFunction_t)thread_msg_event_init, USE_DTCM_MEM },
#endif
#ifndef SUPPORT_AUTO_OTA
#ifdef DEMO_WATCH
    { {"ux_task", 0, NULL, 0, NULL, UX_TASK_STACK_SIZE, TASK_PRIORITY_UX_TASK, 0, 0 },
      (osThreadFunc_t)ux_api_fwk_task, NULL, NULL, USE_DTCM_MEM},
#endif
#endif
#if (CONFIG_DFX_SUPPORT_TRANSMIT_FILE == DFX_YES && !defined(MEMORY_MINI))
    { { "transmit", 0, NULL, 0, NULL, DFX_MSG_STACK_SIZE, TASK_PRIORITY_TRANSMIT, 0, 0 },
      (osThreadFunc_t)transmit_process_thread, NULL, (CreateQueueFunction_t)transmit_thread_msg_init, USE_DTCM_MEM },
#endif
#ifdef SUPPORT_LWIP
    { {"lwip_thread", 0, NULL, 0, NULL, LWIP_MAIN_STACK_SIZE, TASK_PRIORITY_LWIP, 0, 0 },
      (osThreadFunc_t)lwip_main, NULL, NULL, USE_EXTEND_MEM},
#endif
#ifdef SUPPORT_AUTO_OTA
    { { "auto_ota", 0, NULL, 0, NULL, AUTO_OTA_STACK_SIZE, TASK_PRIORITY_AUTO_OTA, 0, 0 },
      (osThreadFunc_t)auto_ota_task_main, NULL, NULL, USE_DTCM_MEM },
#endif
#ifdef SUPPORT_VOLTE
#ifndef CONFIG_FT_DITING
    /* imsa处理线程 SUPPORT_VOLTE */
    { { "imsa", 0, NULL, 0, NULL, IMSA_PROCESS_STACK_SIZE, TASK_PRIORITY_IMSA, 0, 0 },
      (osThreadFunc_t)imsa_process_thread, NULL,
      (CreateQueueFunction_t)imsa_process_task_init, USE_EXTEND_MEM },
#endif
    /* imsa ims uart通道处理线程 */
    { { "imsa_pip_task", 0, NULL, 0, NULL, IMSA_PROCESS_IMS_PIP_UART_SIZE, TASK_PRIORITY_IMSA_PIP_UART, 0, 0 },
      (osThreadFunc_t)imsa_pip_task_message_proc, NULL,
      (CreateQueueFunction_t)imsa_pip_task_init, USE_EXTEND_MEM },

    /* imsa ims spi通道处理线程 */
    { { "imsa_pip_spi", 0, NULL, 0, NULL, IMSA_PROCESS_IMS_PIP_SPI_SIZE, TASK_PRIORITY_IMSA_PIP_SPI, 0, 0 },
      (osThreadFunc_t)imsa_pip_spi_message_proc, NULL,
      NULL, USE_EXTEND_MEM },
#endif
#if defined (CONFIG_DFX_CLOUD_REPORT) && (CONFIG_DFX_SUPPORT_DIAG_REPORT == DFX_YES)
    { { "diag_report", 0, NULL, 0, NULL, DFX_HCD_STACK_SIZE, TASK_PRIORITY_DFX_HCD, 0, 0 },
      (osThreadFunc_t)diag_report_consumer_task, NULL, NULL, USE_DTCM_MEM },
#endif
};

#ifdef SUPPORT_BG_HOST
app_task_definition_t g_app_bg_tasks[] = {
    { { "bt_ipc_recv", 0, NULL, 0, NULL, BTH_IPC_RECV_STACK_SIZE, TASK_PRIORITY_BT_IPC_RECV, 0, 0 },
      (osThreadFunc_t)bgh_ipc_recv_task, NULL, (CreateQueueFunction_t)NULL, USE_DTCM_MEM },
    { { "bt_service", 0, NULL, 0, NULL, BTH_SERVICE_STACK_SIZE, TASK_PRIORITY_SRV, 0, 0 },
      (osThreadFunc_t)btsrv_task_body, NULL, (CreateQueueFunction_t)NULL, USE_DTCM_MEM },
};
#endif

#ifdef SUPPORT_BG_HOST
void bg_task_init(void)
{
#ifdef DEMO_WATCH
    ux_api_fwk_system_init();
#endif
    /* Create queues before tasks in case they are used during initialisation. */
    for (uint8_t i = 0; i < (sizeof(g_app_bg_tasks) / sizeof(app_task_definition_t)); i++) {
        if (g_app_bg_tasks[i].create_queue_handle) {
            g_app_bg_tasks[i].create_queue_handle();
        }
    }

    /* Create tasks */
    for (uint8_t i = 0; i < (sizeof(g_app_bg_tasks) / sizeof(app_task_definition_t)); i++) {
        g_app_bg_tasks[i].task_handle = osThreadNew(g_app_bg_tasks[i].func, NULL, &(g_app_bg_tasks[i].attr));
        if (g_app_bg_tasks[i].task_handle == NULL) {
            panic(PANIC_TASK_CREATE_FAILED, i);
        }
    }
}
#endif

void thread_init(void)
{
    /* Create queues before tasks in case they are used during initialisation. */
    for (uint8_t i = 0; i < (sizeof(g_app_tasks) / sizeof(app_task_definition_t)); i++) {
        if (g_app_tasks[i].create_queue_handle) {
            g_app_tasks[i].create_queue_handle();
        }
    }

    /* Create tasks */
    for (uint8_t i = 0; i < (sizeof(g_app_tasks) / sizeof(app_task_definition_t)); i++) {
        if (g_app_tasks[i].use_mem == USE_EXTEND_MEM) {
            g_app_tasks[i].attr.stack_mem = memalign(ALIGNMENT_16, g_app_tasks[i].attr.stack_size);
        }
        g_app_tasks[i].task_handle = osThreadNew(g_app_tasks[i].func, NULL, &(g_app_tasks[i].attr));
        if (g_app_tasks[i].task_handle == NULL) {
            PRINT("%s task creat failed"NEWLINE, g_app_tasks[i].attr.name);
            panic(PANIC_TASK_CREATE_FAILED, i);
        }
    }

#ifdef SUPPORT_BG_HOST
    // Currently only compatible with BT startup
    bg_task_init();
#ifndef CONFIG_UNSUPPORT_SLE_SERVICE
    sle_service_init();
#endif
#endif
}

static void print_system_sleep_info(void)
{
    sleep_info_t *sleep_info = uapi_pm_get_sleep_info();
    if (sleep_info == NULL) {
        return;
    }

    uint32_t run_time = (uint32_t)(uapi_systick_get_ms() - g_time_base);
    uint32_t wfi_veto_time = (uint32_t)convert_count_2_ms(sleep_info->sleep_history[PM_WFI_WITHVETO].total_slp_time);
    uint32_t wfi_noveto_time = (uint32_t)convert_count_2_ms(sleep_info->sleep_history[PM_WFI_NOVETO].total_slp_time);
    uint32_t deepsleep_time = (uint32_t)convert_count_2_ms(sleep_info->sleep_history[PM_SLEEP_DS].total_slp_time);
    uint32_t lightsleep_time = (uint32_t)convert_count_2_ms(sleep_info->sleep_history[PM_SLEEP_LS].total_slp_time);

    struct timeval cur_time = { 0 };
    gettimeofday(&cur_time, NULL);
    struct tm lm = { 0 };
    localtime_r(&cur_time.tv_sec, &lm);
    lightsleep_time = lightsleep_time + wfi_veto_time + wfi_noveto_time;
    uint16_t sys_slp = reg16_getbit(LP_CTL_GLB_RB_LP_CTL_GLB_HISTORY_REG, LP_CTL_GLB_RB_SYS_SLP_HISTORY_SW_OFFSET);
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_HISTORY_REG, LP_CTL_GLB_RB_SYS_SLP_HISTORY_SW_CLR_OFFSET);
    uint16_t m_slp_his = reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_STS_3_REG, LP_CTL_MCU_RB_MCU_SYS_SLP_HISTORY_STS_OFFSET);
    writew(LP_CTL_MCU_RB_LP_CTL_MCU_HISTORY_REG, LP_CTL_MCU_RB_MCU_SYS_SLP_HISTORY_CLR_MASK);

    uapi_diag_info_log(0, "sys sleep flag: %d ", sys_slp);
    uapi_diag_info_log(0, "app run time:%d, app deepsleep time:%d, app lightsleep time:%d, mcu sleep flag: %d",
        run_time, deepsleep_time, lightsleep_time, m_slp_his);
    PRINT("run:%d ms, sleep:%d, %d ms, slp flag: %d %d, time: %d/%d/%d %d:%d:%d" NEWLINE,
        run_time, deepsleep_time, lightsleep_time, sys_slp, m_slp_his,
        (lm.tm_year + 1900), (lm.tm_mon + 1), lm.tm_mday, lm.tm_hour, lm.tm_min, lm.tm_sec); /* tm_year: since 1900 */
}

void app_main_timeout_call_back(void *args)
{
    unused(args);
    (void)osSemaphoreRelease(g_semaphore_app_main);
}
#ifdef PLT_SUPPORT_CHR
static uint32_t g_chr_record_time_sum = 0;
void record_system_chr_periodic(uint32_t time)
{
#define SYSTEM_CHR_RECORD_TIME 60 * 1000 * 30
    g_chr_record_time_sum += time;
    if (g_chr_record_time_sum >= SYSTEM_CHR_RECORD_TIME) {
        PRINT("record_system_chr_periodic in." NEWLINE);
        system_chr_statistics_process();
        g_chr_record_time_sum = 0;
    }
}
#endif

static void temperature_detection_process(void)
{
    int32_t temperature_data;
    if (uapi_get_temperature(&temperature_data) != ERRCODE_SUCC) {
        return;
    }
    PRINT("T:%d,V0P8:%d,inc times:%u." NEWLINE, temperature_data, uapi_pmu_ldo_get_voltage(PMU_BUCK_LDO_BUCK_0P8),
        g_temperature_inc_times);
    if ((temperature_data > TEMPERATURE_ADJUST_VOL_THRESHOLD) && (get_clock_0p8_vol_vmin() == PMU_0P9_VSET_0V8_25)) {
        g_temperature_inc_times++;
        if (g_temperature_inc_times > TEMPERATURE_INCREASE_VALID_CYCLE) {
            if (uapi_pmu_ldo_get_voltage(PMU_BUCK_LDO_BUCK_0P8) == PMU_0P9_VSET_0V8_25) {
                uapi_pmu_ldo_set_voltage(PMU_BUCK_LDO_BUCK_0P8, PMU_0P9_VSET_0V8);
            }
            set_clock_0p8_vol_vmin(PMU_0P9_VSET_0V8);
        }
        return;
    }
    g_temperature_inc_times = 0;
    if ((temperature_data <= TEMPERATURE_ADJUST_VOL_THRESHOLD) && (get_clock_0p8_vol_vmin() == PMU_0P9_VSET_0V8)) {
        if (uapi_pmu_ldo_get_voltage(PMU_BUCK_LDO_BUCK_0P8) == PMU_0P9_VSET_0V8) {
            uapi_pmu_ldo_set_voltage(PMU_BUCK_LDO_BUCK_0P8, PMU_0P9_VSET_0V8_25);
        }
        set_clock_0p8_vol_vmin(PMU_0P9_VSET_0V8_25);
        return;
    }
}

static void app_main(const void *unused)
{
#define APP_MAIN_TIMER_PERIOD_TIME    (20 * OS_SYS_MS_PER_SECOND)
    unused(unused);

    dfx_set_log_enable();
    uapi_clocks_config_mode(PM_ID_BSP, PM_MODE_NORMAL);

    g_time_base = uapi_systick_get_ms();
    g_semaphore_app_main = osSemaphoreNew(1, 0, NULL);

    g_app_main_timerId = osTimerExtNew(app_main_timeout_call_back, osTimerPeriodic, NULL, NULL,
        osTimerRousesAllow, osTimerAlignAllow);
    osTimerStart(g_app_main_timerId, APP_MAIN_TIMER_PERIOD_TIME);

#ifdef CONFIG_ENABLE_RC_CALIBRATION
    cali_32k_init();
#endif

    while (1) {
        if (osKernelGetState() == osKernelRunning && osSemaphoreAcquire(g_semaphore_app_main, osWaitForever) != osOK) {
            panic(PANIC_NONE, __LINE__);
            return;
        }
        uapi_watchdog_kick();
        uapi_logfile_fsync(STORE_DIAG);
#if defined(CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE) && (CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE == DFX_YES)
        uapi_logfile_fsync(STORE_DOTTING);
#endif /* CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE */
#if defined(CONFIG_DFX_SUPPORT_STRING_LOG_SAVE_FILE) && (CONFIG_DFX_SUPPORT_STRING_LOG_SAVE_FILE == DFX_YES)
        uapi_logfile_fsync(STORE_STR);
#endif /* CONFIG_DFX_SUPPORT_STRING_LOG_SAVE_FILE */
        update_os_time();
        temperature_detection_process();
        print_system_sleep_info();
#ifdef PLT_SUPPORT_CHR
        record_system_chr_periodic(APP_MAIN_TIMER_PERIOD_TIME);
#endif
#ifdef DFX_PRINT_MEM_USAGE
        dfx_show_heap_statistic();
        dfx_show_all_task_heap();
#endif
    }
}

#ifdef SUPPORT_LWIP
static void lwip_main(void)
{
    PRINT("lwip_main run start."NEWLINE);
    tcpip_init(NULL, NULL);
#if defined(SUPPORT_ALIPAY_HSC32I1)
    lwip_netif_switch_bt_pan();
#endif
#ifdef SUPPORT_VOLTE
    if (imsa_netif_create_and_init() != CAT1_NET_IF_INIT_SUCC) {
        PRINT("imsa_netif_create_and_init fail."NEWLINE);
    }
    lwip_cat1_open();
    lwip_cat1_set_ip();
#endif
}
#endif
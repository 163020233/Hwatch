/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:  Application Core Main function for Standard.
 * Author:
 * Create: 2025-05-13
 */
#include "main.h"
#include "arch/exception.h"
#include "los_task_pri.h"
#include "cmsis_os2.h"
#include "string.h"
#include "rtc.h"
#include "app_os_init.h"
#include "gpio.h"
#include "chip_io.h"
#include "panic.h"
#include "securec.h"
#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "tcxo.h"
#include "systick.h"
#include "cpu_trace_unified.h"
#include "application_version.h"
#include "watchdog.h"
#include "watchdog_porting.h"
#include "cpu_utils.h"
#include "exception.h"
#include "log_oml_exception.h"
#include "uart.h"
#include "uart_porting.h"
#include "pmp_config.h"
#include "partition.h"
#include "soc_osal.h"
#include "arch_encoding.h"
#include "arch_port.h"
#include "upg_config.h"
#include "los_printf_pri.h"
#include "cpu_trace_porting.h"
#include "timer.h"
#include "dma.h"
#include "security_init.h"
#include "log_memory_region.h"
#include "pm_pmu_porting.h"
#include "pmu_sub_control.h"
#include "psram_v2.h"
#include "sfc_v2.h"

ree_nmi_stack_t g_nmi_stack;

#define WDT_TIMEOUT_S       30
#define IMG_NUM_MAX         2
#define IMG_SEC             0
#define IMG_HIFI            1
#define SW_PRINT_DELAY      200ULL
#define CSR_WST_VAL         0x4 // 向寄存器目标位写入的值(0b100)
#define CSR_WST_POS         0xb // 寄存器偏移，结合CSR_WST_VAL改变11、12、13位的值
#define CSR_WST_MASK        0xffffc7ff

#define COM_UART_BAUDRATE   115200

extern void *g_intheap_size;
extern char __extend_heap_start;
extern void *g_extend_heap_size;

#if (defined SUPPORT_CXX)
typedef void (*SystemInitFunc)(void);
extern char __init_array_start;
extern char __init_array_end;
#endif

#ifdef LOSCFG_NET_LWIP_SACK
int lwip_close(int s)
{
    return 0;
}

int lwip_fcntl(int s, int cmd, int val)
{
    return 0;
}

ssize_t lwip_send(int s, const void *data, size_t size, int flags)
{
    return 0;
}

ssize_t lwip_recv(int s, void *mem, size_t len, int flags)
{
    return 0;
}
#endif

static void psram_init_handle(void)
{
    errcode_t retcode = uapi_psram_init(PSRAM_ID_0);
    if (retcode != 0) {
        PRINT("psram init RET:%u", retcode);
        return;
    }
}

static void service_init(void)
{
    cpu_utils_init();
    hal_register_exception_dump_callback(log_exception_dump);
}

static void rc_32k_ulp_gpio_process(void)
{
    uapi_pin_set_mode(S_UGPIO0, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_ds(S_UGPIO0, (pin_drive_strength_t)HAL_PIO_DRIVE_3);
    uapi_pin_set_pull(S_UGPIO0, (pin_pull_t)HAL_PIO_PULL_NONE);
    uapi_gpio_set_dir(S_UGPIO0, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(S_UGPIO0, GPIO_LEVEL_HIGH);
    uapi_pin_set_mode(S_UGPIO1, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_ds(S_UGPIO1, (pin_drive_strength_t)HAL_PIO_DRIVE_3);
    uapi_pin_set_pull(S_UGPIO1, (pin_pull_t)HAL_PIO_PULL_NONE);
    uapi_gpio_set_dir(S_UGPIO1, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(S_UGPIO1, GPIO_LEVEL_HIGH);
    return;
}

static void pio_func_init(void)
{
    size_t pin_num;
    hal_pio_config_t *pio_func = NULL;

    get_pio_func_config(&pin_num, &pio_func);

    for (pin_t i = 0; i < pin_num; i++) {
        if (pio_func[i].func == HAL_PIO_FUNC_DEFAULT_HIGH_Z) {
            uapi_pin_set_mode(i, (pin_mode_t)HAL_PIO_FUNC_GPIO);
            uapi_gpio_set_dir(i, GPIO_DIRECTION_INPUT);
        } else {
            uapi_pin_set_mode(i, (pin_mode_t)pio_func[i].func);
        }

        uapi_pin_set_ds(i, (pin_drive_strength_t)pio_func[i].drive);
        uapi_pin_set_pull(i, (pin_pull_t)pio_func[i].pull);
        uapi_pin_set_ie(i, (pin_input_enable_t)pio_func[i].ie);
    }
    if (reg16_getbit(LP_CTL_ULP_A_RB_RC_XO_CLK_MUX_REG, LP_CTL_ULP_A_RB_RC_XO_SEL_OFFSET) == 0) {
        rc_32k_ulp_gpio_process();
        PRINT("rc 32k, disable swd func.\r\n");
    }
}

static void board_init(void)
{
    /* flash init */
    sfc_flash_config_t sfc_cfg = {
        .read_type = FAST_READ_QUAD_IO,
        .write_type = QUAD_INPUT_PAGE_PROGRAM,
        .mapping_addr = FLASH_START,
        .mapping_size = FLASH_LENGTH,
    };
    /* Need first flash power on */
    errcode_t ret = uapi_sfc_init(SFC_ID_0, &sfc_cfg);
    if (ret != ERRCODE_SUCC) {
        PRINT("flash err = 0x%x \r\n", ret);
    }
    pio_func_init();
    uapi_partition_init();
    PRINT("partition init success"NEWLINE);
}

static void chip_init(void)
{
    uapi_cpu_trace_enable(cpu_trace_get_self_id());
    uapi_pin_init();
    uapi_gpio_init();
    uapi_tcxo_init();
    uapi_systick_init();
    uapi_rtc_init();
    uapi_rtc_adapter(RTC_0, RTC_0_IRQN, g_aucIntPri[RTC_0_IRQN]);
    uapi_timer_init();
    uapi_timer_adapter(TIMER_INDEX_1, TIMER_1_IRQN, irq_prio(TIMER_1_IRQN));
    uapi_dma_init();
    uapi_dma_open();
    uapi_drv_cipher_env_init();
    uapi_pmu_init();

#if CHIP_ASIC
    uapi_clocks_init();
#endif
#ifdef CFG_DRIVERS_MMC
    uapi_pmu_hs_sub_control(HS_SUB_EMMC_ID, PMU_POWER_ON_AND_RUN);
#elif defined(CFG_DRIVERS_NANDFLASH)
    uapi_pmu_hs_sub_control(HS_SUB_FMC_ID, PMU_POWER_ON_AND_RUN);
#endif
    psram_init_handle();
}

static void chip_watchdog_init(void)
{
    watchdog_turnon_clk();
    watchdog_func_adapt(CHIP_WDT_TIMEOUT_32S);
    uapi_watchdog_init(CHIP_WDT_TIMEOUT_32S);
    uapi_watchdog_enable(WDT_MODE_INTERRUPT);
}

static void kernel_init(void)
{
    chip_watchdog_init();

    reg16_setbit(COM_CTL_RB_COM_UART_REG, COM_CTL_RB_COM_UART_CLKEN_OFFSET);
    reg16_setbit(COM_CTL_RB_COM_UART_REG, COM_CTL_RB_COM_UART_SOFT_RST_N_OFFSET);

    /* uart init */
    sw_debug_uart_init(COM_UART_BAUDRATE);
    PRINT("Recovery uart init succ."NEWLINE);

    panic_init();

    /* enable icache and dcahce */
    pmp_init();
    OsCacheInit();

    /* os kernel adapt config */
    ArchSetExcHook((EXC_PROC_FUNC)do_fault_handler);
    ArchSetNMIHook((NMI_PROC_FUNC)do_hard_fault_handler);
    OsSetMainTask();
    OsCurrTaskSet((LosTaskCB *)OsGetMainTask());

    (void)osKernelInitialize();
}

void sec_main(void)
{
    kernel_init();

    chip_init();

    board_init();

    service_init();

    app_os_init(NULL, 0);

    (void)osKernelStart();
    for (;;) {
    }
}

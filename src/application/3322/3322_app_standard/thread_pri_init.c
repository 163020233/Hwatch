/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description:   Application Core OS Priority Initialize Interface for Standard
 * Author:
 * Create: 2022-12-03
 */

#include "thread_pri_init.h"
#include "debug_print.h"
#include "thread_init.h"
#include "common_def.h"
#include "pm.h"
#include "securec.h"
#include "watchdog.h"
#include "cpu_load.h"
#include "pinctrl_porting.h"
#include "hal_gpio.h"
#ifdef CFG_DRIVERS_MMC
#include "soc/mmc.h"
#endif
#include "panic.h"
#include "board_init.h"
#include "kernel_init.h"
#include "chip_io.h"
#include "pm_pmu.h"
#include "pmu_sub_control.h"
#include "pm_veto.h"
#include "clocks_switch.h"
#include "temperature.h"
#include "usb_porting.h"
#if DSP_EXIST == YES
#include "soc_uapi_audio_sys.h"
#include "soc_uapi_vendor.h"
#include "soc_uapi_sound.h"
#endif
#ifdef SUPPORT_DISPLAY_TECH
#include "soc_lcd_api.h"
#endif
#include "los_config.h"
/* after reboot system emmc need write exc info to the file system from sector */
#if defined(SAVE_EXC_INFO) && defined(CFG_DRIVERS_MMC)
#include "crash_data.h"
#endif
#ifdef CONFIG_DFX_CLOUD_REPORT
#include "hcd.h"
#include "event_code.h"
#include "dfx_cloud_report.h"
#endif
#include "pmu_alarm.h"
#if defined(SUPPORT_INPUT_WEAR) || defined(SUPPORT_INPUT_FEATURE)
#include "input_feature.h"
#endif
#if defined(SUPPORT_TOUCH_WEAR) || defined(SUPPORT_DISPLAY_TECH)
#include "touch_feature.h"
#endif
#include "nv.h"
#include "calendar.h"
#include "nv_ipc.h"
#include "nv_upg.h"
#if defined(SAVE_EXC_INFO)
#include "exception.h"
#endif
#include "gpio.h"
#include "pinctrl.h"
#include "cpu_trace_porting.h"
#include "cpu_trace_unified.h"
#ifdef SUPPORT_OHOSFWK
#include "ohosfwk_startup.h"
#endif
#ifdef CFG_DRIVERS_NANDFLASH
#include "nand_fmc.h"
#include "exception.h"
#endif
#ifdef CFG_DRIVERS_SPINOR
#include "littlefs_init.h"
#endif
#ifdef SUPPORT_MEDIA_AUDIO_MANAGER
#include "audio_manager_service.h"
#endif
#define TASK_PRIORITY_APP (osPriority_t)(17)
#define TICK_TEN_MS                 (10U)
#ifdef CFG_DRIVERS_USB_AS_SERIAL
#include "usb_porting.h"
#endif

static void thread_main(const void *unused);
uint32_t button_init(void);

app_task_definition_t g_thread_pri[] = {
    { { "thread_pri", 0, NULL, 0, NULL, (0x4000), TASK_PRIORITY_APP, 0, 0 },
      (osThreadFunc_t)thread_main, NULL, NULL, USE_DTCM_MEM },
};

#ifdef SUPPORT_CXX
typedef void (*system_init_func)(void);

static void system_init_array(uintptr_t start, uintptr_t end)
{
    system_init_func func;
    uintptr_t *it = (uintptr_t *)start;
    for (; it < (uintptr_t *)end; it++) {
        func = (system_init_func)(*it);
        func();
    }
}
#endif

#ifdef SUPPORT_DISPLAY_TECH
static void display_init(bool skip_lcd_init)
{
    if (!skip_lcd_init) {
        int32_t ret = uapi_lcd_init();
        PRINT("lcd init %d"NEWLINE, ret);
    }

#ifdef SUPPORT_GPU_GMMU
    uapi_gmmu_module_init();
#endif
    uapi_vau_module_init();
    uapi_dpu_module_init();
#ifdef SUPPORT_GPU_JPEG
    uapi_jpeg_module_init();
#endif
}
#endif

#ifdef CFG_DRIVERS_MMC
static void emmc_pio_func_init(void)
{
    size_t pin_num;
    hal_pio_config_t *pio_func = NULL;
    get_pio_func_config(&pin_num, &pio_func);
    for (pin_t i = S_HGPIO0; i <= S_HGPIO9; i++) {
        uapi_pin_set_mode(i, (pin_mode_t)pio_func[i].func);
        uapi_pin_set_ds(i, (pin_drive_strength_t)pio_func[i].drive);
        uapi_pin_set_pull(i, (pin_pull_t)pio_func[i].pull);
        uapi_pin_set_ie(i, (pin_input_enable_t)pio_func[i].ie);
    }
}
static void emmc_vfs_control(void)
{
    int ret = emmc_drv_context_init();
    PRINT("emmc init [%d]\r\n", ret);
}
#endif

#if DSP_EXIST == YES
#ifdef AUDIO_SMARTPA_AW88262
/**
 * The sample code of the vendor,
 * which will be implemented by the customer in the future
 */
/* Audio codec vendor select according to hardware connection:
 * define AUDIO_SMARTPA_AW88262 : macro to init smartpa aw88262, defined in config/target_config/3322/config.py;
 * define AUDIO_SMARTPA_AW88262_I2S_ID UAPI_SND_OUT_PORT_I2S0 : macro to specify i2s id connecting to smartpa aw88262;
 */
#define AUDIO_SMARTPA_AW88262_I2S_ID UAPI_SND_OUT_PORT_I2S0

td_s32 audio_aw88262_init(const uapi_vendor_codec_attr *attr);
td_s32 audio_aw88262_deinit(td_void);

static void audio_smartpa_init(void)
{
    static uapi_vendor_driver g_aw88262_driver = {
        .out_port = AUDIO_SMARTPA_AW88262_I2S_ID,
        .init = audio_aw88262_init,
        .deinit = audio_aw88262_deinit,
        .set_aef_profile = TD_NULL,
    };

    td_s32 ret = uapi_audio_vendor_register(&g_aw88262_driver);
    if (ret != 0) {
        PRINT("aw88262 register driver error, ret = %d"NEWLINE, ret);
    }
}
#endif /* AUDIO_SMARTPA_AW88262 */

#ifdef SUPPORT_AUDIO_ANAPA_POWER_CTRL

#define AUDIO_ANAPA_DAC_ID UAPI_SND_OUT_PORT_DAC0
#define AUDIO_ANAPA_POWER_CTL_GPIO_ID S_AGPIO30

static td_s32 audio_analog_pa_power_on(const uapi_vendor_codec_attr *attr)
{
    uapi_pin_set_mode(AUDIO_ANAPA_POWER_CTL_GPIO_ID, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(AUDIO_ANAPA_POWER_CTL_GPIO_ID, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(AUDIO_ANAPA_POWER_CTL_GPIO_ID, GPIO_LEVEL_HIGH);

    PRINT("analog pa power on\n");
    unused(attr);
    return 0;
}

static td_s32 audio_analog_pa_power_off(td_void)
{
    uapi_gpio_set_val(AUDIO_ANAPA_POWER_CTL_GPIO_ID, GPIO_LEVEL_LOW);

    PRINT("analog pa power off\n");
    return 0;
}

static void audio_analog_pa_register(void)
{
    static uapi_vendor_driver g_analog_pa_driver = {
        .out_port = AUDIO_ANAPA_DAC_ID,
        .init = audio_analog_pa_power_on,
        .deinit = audio_analog_pa_power_off,
        .set_aef_profile = TD_NULL,
    };

    td_s32 ret = uapi_audio_vendor_register(&g_analog_pa_driver);
    if (ret != 0) {
        PRINT("analog pa register driver error, ret = %d"NEWLINE, ret);
    }
}

#endif /* SUPPORT_AUDIO_ANAPA_POWER_CTRL */

#ifdef CFG_LOAD_FROM_FS
#define DSP_FILE_PATH "/boot/dsp/"
#endif

static void audio_init(void)
{
#ifdef AUDIO_SMARTPA_AW88262
    audio_smartpa_init();
#endif

#ifdef SUPPORT_AUDIO_ANAPA_POWER_CTRL
    audio_analog_pa_register();
#endif

    td_s32 ret = uapi_audio_init();
    if (ret != 0) {
        PRINT("audio init error:[%#x] \r\n", ret);
    }

#ifdef CFG_LOAD_FROM_FS
    ret = uapi_audio_set_dsp_file_path(DSP_FILE_PATH, sizeof(DSP_FILE_PATH));
    if (ret != 0) {
        PRINT("audio set dsp file path error:[%#x]", ret);
    }
#endif
}

#endif

#ifdef CFG_DRIVERS_SDIO
static void sdio_pio_func_init(void)
{
    size_t pin_num;
    hal_pio_config_t *pio_func = NULL;
    get_pio_func_config(&pin_num, &pio_func);
    for (pin_t i = S_HGPIO10; i <= S_HGPIO15; i++) {
        uapi_pin_set_mode(i, (pin_mode_t)pio_func[i].func);
        uapi_pin_set_ds(i, (pin_drive_strength_t)pio_func[i].drive);
        uapi_pin_set_pull(i, (pin_pull_t)pio_func[i].pull);
        uapi_pin_set_ie(i, (pin_input_enable_t)pio_func[i].ie);
    }
}
#endif

void app_alarm_callback(alarm_handle_t handle)
{
    PRINT("app recv alarm trigger. handle=%d\r\n", handle);
}

static void thread_main(const void *unused)
{
    unused(unused);
    uapi_pm_add_sleep_veto(PM_ID_BSP);
    los_vfs_init();
    uint32_t ret = dpal_driver_base_init();
    if (ret != 0) {
        PRINT("dpal driver base init error" NEWLINE);
        panic(PANIC_ASSERT, __LINE__);
    }
#ifdef CFG_DRIVERS_USB_AS_SERIAL
    uapi_usb_device_init(DEV_SERIAL);
#endif
#if defined(CFG_DRIVERS_MMC) || defined(CFG_DRIVERS_NANDFLASH) || defined(CFG_DRIVERS_SPINOR)
    PRINT("filesystem init start!\r\n");

#ifdef CFG_DRIVERS_SPINOR
    littlefs_init();
#endif

#ifdef CFG_DRIVERS_MMC
    emmc_pio_func_init();
    emmc_vfs_control();
#endif
#ifdef CFG_DRIVERS_NANDFLASH
    /* true: boot partition */
    int32_t fs_init_ret = nandflash_filesystem_init(true);
    if (fs_init_ret != 0) {
        PRINT("nandflash init boot partition error[%d]"NEWLINE, fs_init_ret);
    }
#endif
    PRINT("filesystem init end!\r\n");
#endif

#ifdef CFG_DRIVERS_SDIO
    sdio_pio_func_init();
    sdio_drv_context_init();
#endif
    startup_other_core();
#ifdef SUPPORT_INPUT_FEATURE
    input_app_init();
    button_init();
#endif
    registe_vbus_irq();

#ifdef SUPPORT_DISPLAY_TECH
    ret = touch_init();
    PRINT("touch_init ret = %u"NEWLINE, ret);
    uapi_clocks_config_mode(PM_ID_VIDEO, PM_MODE_PERFORMACE);
    bool skip_lcd_init = false;
#ifdef CONFIG_FT_DITING
    if (ret != EXT_ERR_SUCCESS) {
        skip_lcd_init = true;
    }
#endif
    display_init(skip_lcd_init);
#endif

#ifdef SUPPORT_OHOSFWK
    ohos_show_logo();
#endif

#if DSP_EXIST == YES
    audio_init();
#endif
#ifdef CFG_DRIVERS_NANDFLASH
    /* false: non-boot partition */
    fs_init_ret = nandflash_filesystem_init(false);
    if (fs_init_ret != 0) {
        PRINT("nandflash init non-boot partition error[%d]"NEWLINE, fs_init_ret);
    }
    set_file_system_mount_ret(fs_init_ret);
#endif

#if defined(SAVE_EXC_INFO)
    hard_watchdog_reset_save_info();
#endif
    uapi_cpu_trace_enable(cpu_trace_get_self_id());
    PRINT("trace clk reg status 0x%x\r\n", readl(MCU_DIAG_CTL_RB_CFG_MCU_DIAG_MONITOR_CLOCK_REG));
    uapi_nv_init();
    (void)nv_upg_upgrade_task_process();
    uapi_calendar_init();
    nv_ipc_reset_xotrim();
    thread_init();
#ifdef SUPPORT_OHOSFWK
    ohos_startup();
#endif

#ifdef SUPPORT_MEDIA_AUDIO_MANAGER
    bool success = AudioManagerInit();
    PRINT("audio manager init %s\r\n", success ? "ok" : "error");
#endif

    uapi_alarm_register_callback(ALARM_APP_SERVICE, app_alarm_callback);
    uapi_alarm_init();
#ifdef CONFIG_DFX_CLOUD_REPORT
    dfx_report_system_init();
    uapi_diag_report_event(0, "EVTCODE_BOOT_FINISH(0x%x)@System boot finished.", EVTCODE_BOOT_FINISH);
#endif
    uapi_temperature_adapt_init();
    uapi_pm_remove_sleep_veto(PM_ID_BSP);
}

void thread_pri_init(void)
{
#ifdef SUPPORT_CXX
    system_init_array((uintptr_t)&__init_array_start, (uintptr_t)&__init_array_end);
#endif
    /* Create tasks */
    for (uint8_t i = 0; i < (sizeof(g_thread_pri) / sizeof(app_task_definition_t)); i++) {
        if (g_thread_pri[i].use_mem == USE_EXTEND_MEM) {
            g_thread_pri[i].attr.stack_mem = memalign(ALIGNMENT_16, g_thread_pri[i].attr.stack_size);
        }
        g_thread_pri[i].task_handle = osThreadNew(g_thread_pri[i].func, NULL, &(g_thread_pri[i].attr));
        if (g_thread_pri[i].task_handle == NULL) {
            PRINT("create thread pri failed!");
        }
    }
}

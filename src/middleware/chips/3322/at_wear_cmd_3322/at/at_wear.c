/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: At wear function \n
 *
 */

#include "at_wear.h"
#include <string.h>
#include "gpio.h"
#include "pinctrl.h"
#include "pinctrl_porting.h"
#include "watchdog.h"
#include "at_cmd.h"
#include "at_utils.h"
#include "at_wear_cmd_register.h"
#include "at_wear_cmd_table.h"
#include "debug_print.h"
#include "hal_reboot.h"
#include "hal_psram.h"
#include "los_memory_pri.h"
#include "at_adapter.h"
#include "tcxo.h"
#include "watchdog_porting.h"
#include "pmu_ulp.h"
#include "soc/mmc.h"
#include "diag_channel.h"
#include "cmsis_os.h"
#ifdef CFG_DRIVERS_MMC
#include "block.h"
#endif
#include "idle_config.h"
#include "pm_veto.h"
#include "panic.h"
#ifdef CONFIG_DFX_SUPPORT_PRINT
#include "dfx_print.h"
#endif
#ifdef SUPPROT_EXECUTE_CORMARK
#include "core_portme.h"
#endif
#ifdef AT_IPC_V1_TEST
#include "ipc.h"
#include "ipc_porting.h"
#endif
#ifdef IPC_NEW
#include "ipc.h"
#include "ipc_new_shared.h"
#endif
#include "uart_porting.h"
#include "clocks_ccrg.h"
#include "hal_clocks_ccrg.h"
#include "pm_pmu.h"
#include "pmu_sub_control.h"
#include "efuse.h"
#include "dfx_adapt_layer.h"
#include "sys/time.h"
#include "time.h"
#include "os_dfx.h"
#include "los_swtmr_pri.h"
#include "los_task_pri.h"
#include "pm_clocks.h"
#include "arch_encoding.h"
#include "secure_verify_boot_porting.h"
#ifdef SUPPORT_POWER_MANAGER
#include "power_display_service.h"
#endif
#include "pm_sleep_porting.h"
#ifdef SUPPORT_SPORT_HEALTH_TEST
#include "gesture_main.h"
#include "ppg_main.h"
#endif
#include "clock_calibration.h"
#include "nv.h"
#include "nv_ipc.h"
#include "pmu_alarm.h"
#include "calendar.h"
#ifdef JS_ENABLE
#include "msg_center_protocol.h"
#endif
#include "adc_porting.h"
#ifdef LOSCFG_SHELL_ADAPT_AT
#include "los_at.h"
#endif

#ifdef SUPPORT_AIVOICE
#include "intelligent_voice_service.h"
#endif
#ifdef SUPPORT_POWER_MANAGER
#include "power_display_service.h"
#endif
#include "thread_init.h"

#ifdef SUPPORT_AUTO_OTA
#include "auto_ota_service.h"
#endif

#ifdef CONFIG_SUPPORT_NPU_SAMPLES
#include "fuzz_demo.h"
#endif

#include "app_at_uart.h"
#include "hal_psram_regs_op.h"
#include "psram_porting.h"
#include "build_version.h"
#ifdef SUPPORT_TOUCH_FEATURE
#include "touch_feature.h"
#endif
#include "pm_dfx_porting.h"
#include "diag_filter.h"
#include "dhry.h"
#include "clocks_switch.h"
#include "dfx_channel.h"
#include "pmu_mem.h"
#ifdef SUPPORT_OHOSFWK
#ifndef CFG_NORFLASH_ONLY
#include "alarmclocktimerconfig/AlarmClockConfig.h"
#endif
#endif
#if (defined(CONFIG_DFX_SUPPORT_LOG_LIMIT) && (CONFIG_DFX_SUPPORT_LOG_LIMIT == DFX_YES))
#include "log_limit.h"
#endif

#include "usb_porting.h"

#define AT_WEAR_FUNC_NUM (sizeof(at_wear_cmd_parse_table) / sizeof(at_wear_cmd_parse_table[0]))
#define VER_STR_MAX             16

#define GET_CALLER_RA               1
#define COMPARE_CALLER_RA           2
#define CLEAN_UP_CALLER_RA_STATE    0

#define YEAR_BASE          1900

#ifdef SUPPORT_AUTO_OTA
#define NUM_3 3
#define NUM_2 2
#ifndef ONE_ADDR_LEN
#define ONE_ADDR_LEN 2
#endif
#ifndef ADDR_MAC_LEN
#define ADDR_MAC_LEN 6
#endif
#endif

int set_emmc_ecsd_info(uint8_t fields, uint8_t value);

void los_at_wear_cmd_register(void)
{
    print_str("los_at_wear_cmd_register EXCUTE\r\n");
    uapi_at_wear_register_cmd(at_wear_cmd_parse_table, AT_WEAR_FUNC_NUM);
}

at_ret_t at_reboot(void)
{
    hal_reboot_chip();
    return AT_RET_OK;
}

#ifdef SUPPROT_EXECUTE_CORMARK
at_ret_t wear_coremark_test(void)
{
    uint32_t irq_sts = osal_irq_lock();
    watchdog_turnoff_clk();
    coremark_test(0, NULL);
    watchdog_turnon_clk();
    osal_irq_restore(irq_sts);
    return AT_RET_OK;
}
#endif

#if defined (CFG_DRIVERS_MMC) || defined (CFG_DRIVERS_NANDFLASH) || defined (CFG_DRIVERS_SPINOR)
at_ret_t mcu_test_fs_baudwidth(void)
{
    char* path = "/user/test_spped.bin";
    at_adapter_print("fs bw test start!please wait...\r\n");
    fs_test_rw_speed(path, BYTES_LEN_128);
    fs_test_rw_speed(path, BYTES_LEN_512);
    fs_test_rw_speed(path, BYTES_LEN_1K);
    fs_test_rw_speed(path, BYTES_LEN_4K);
    fs_test_rw_speed(path, BYTES_LEN_8K);
    return AT_RET_OK;
}
#endif

at_ret_t mcu_test_dtcm_baudwidth(void)
{
    uint64_t start_time, end_time;
    uint32_t total_time;
    uint32_t bw;

    char *test_buf = (char*)(APP_DTCM_ORIGIN + DTCM_RAM_OFF);
    uint32_t irq_sts = osal_irq_lock();
    start_time = uapi_tcxo_get_us();
    if (memcpy_s(test_buf, BUFF_LEN_48K, (void *)(APP_DTCM_ORIGIN + REE_DTCM_OFF), BUFF_LEN_48K) != 0) {
        at_adapter_print("[dctm mem test]dtcm read memcpy_s fail.\r\n");
    }
    end_time = uapi_tcxo_get_us();
    osal_irq_restore(irq_sts);
    total_time = (uint32_t)(end_time - start_time);
    bw = (ONE_SECOND / total_time) * BUFF_LEN_48K / (MB_SIZE);
    at_adapter_print("[dctm mem test]dtcm read time:%dus bw:%dMB/s\r\n", total_time, bw);

    irq_sts = osal_irq_lock();
    start_time = uapi_tcxo_get_us();
    if (memcpy_s(test_buf, BUFF_LEN_48K, (void *)(APP_DTCM_ORIGIN + REE_DTCM_OFF), BUFF_LEN_48K) != 0) {
        at_adapter_print("[dctm mem test]dtcm write memcpy_s fail.\r\n");
    }
    end_time = uapi_tcxo_get_us();
    osal_irq_restore(irq_sts);
    total_time = (uint32_t)(end_time - start_time);
    bw = (ONE_SECOND / total_time) * BUFF_LEN_48K / (MB_SIZE);
    at_adapter_print("[dctm mem test]dtcm write time:%dus bw:%dMB/s\r\n", total_time, bw);
    free(test_buf);
    return AT_RET_OK;
}

at_ret_t mcu_test_l2m_baudwidth(void)
{
    uint64_t start_time, end_time;
    uint32_t total_time;
    uint32_t bw;

    char* l2m_ptr = (char*)(L2RAM_GMMU_START);
    char *test_buf = (char*)(APP_DTCM_ORIGIN + DTCM_RAM_OFF);
    uint32_t irq_sts = osal_irq_lock();
    start_time = uapi_tcxo_get_us();
    if (memcpy_s(test_buf, BUFF_LEN_48K, l2m_ptr, BUFF_LEN_48K) != 0) {
        at_adapter_print("[l2ram mem test]l2ram read memcpy_s fail.\r\n");
    }
    end_time = uapi_tcxo_get_us();
    osal_irq_restore(irq_sts);
    total_time = end_time - start_time;
    bw = (ONE_SECOND / total_time) * BUFF_LEN_48K / (MB_SIZE);
    at_adapter_print("[l2m mem test]l2m read time:%dus bw:%dMB/s\r\n", total_time, bw);

    irq_sts = osal_irq_lock();
    start_time = uapi_tcxo_get_us();
    if (memcpy_s(l2m_ptr, BUFF_LEN_48K, test_buf, BUFF_LEN_48K) != 0) {
        at_adapter_print("[l2ram mem test]l2ram write memcpy_s fail.\r\n");
    }
    end_time = uapi_tcxo_get_us();
    osal_irq_restore(irq_sts);
    total_time = end_time - start_time;
    bw = (ONE_SECOND / total_time) * BUFF_LEN_48K / (MB_SIZE);
    at_adapter_print("[l2m mem test]l2m write time:%dus bw:%dMB/s\r\n", total_time, bw);

    return AT_RET_OK;
}

at_ret_t mcu_test_psram_baudwidth(void)
{
    uint64_t start_time, end_time;
    uint32_t total_time;
    uint32_t bw;

    char *test_buf = osal_pool_mem_alloc(OS_EXTEND_MEM_ADDR, BUFF_LEN_48K);
    if (test_buf == NULL) {
        at_adapter_print("[psram mem test]buf malloc fail!\r\n");
        return AT_RET_OK;
    }
    char *test_buf1 = (char*)(APP_DTCM_ORIGIN + DTCM_RAM_OFF);

    uint32_t irq_sts = osal_irq_lock();
    ArchDCacheFlush();  // flush以避免cache影响测试速度
    start_time = uapi_tcxo_get_us();
    if (memcpy_s(test_buf1, BUFF_LEN_48K, test_buf, BUFF_LEN_48K) != 0) {
        at_adapter_print("[psram mem test]psram read memcpy_s fail.\r\n");
    }
    end_time = uapi_tcxo_get_us();
    osal_irq_restore(irq_sts);
    total_time = (uint32_t)(end_time - start_time) - TCXO_ERROR;
    bw = ONE_SECOND / total_time * BUFF_LEN_48K / MB_SIZE;
    at_adapter_print("[psram mem test]psram read time:%dus bw:%dMB/s\r\n", total_time, bw);

    irq_sts = osal_irq_lock();
    ArchDCacheFlush();
    start_time = uapi_tcxo_get_us();
    if (memcpy_s(test_buf, BUFF_LEN_48K, test_buf1, BUFF_LEN_48K) != 0) {
        at_adapter_print("[psram mem test]psram write memcpy_s fail.\r\n");
    }
    end_time = uapi_tcxo_get_us();
    osal_irq_restore(irq_sts);
    total_time = (uint32_t)(end_time - start_time) - TCXO_ERROR;
    bw = (ONE_SECOND / total_time) * BUFF_LEN_48K / (MB_SIZE);
    at_adapter_print("[psram mem test]psram write time:%dus bw:%dMB/s\r\n", total_time, bw);
    osal_pool_mem_free(OS_EXTEND_MEM_ADDR, test_buf);

    return AT_RET_OK;
}

at_ret_t mcu_test_comram_baudwidth(void)
{
    uint64_t start_time, end_time;
    uint32_t total_time;
    uint32_t bw;

    char *test_buf = (char*)(RESERVED_SHARE_MEM_ORIGIN);
    if (test_buf == NULL) {
        at_adapter_print("[comram mem test]buf malloc fail!\r\n");
        return AT_RET_OK;
    }
    char *test_buf1 = (char*)(APP_DTCM_ORIGIN + DTCM_RAM_OFF);

    uint32_t irq_sts = osal_irq_lock();
    start_time = uapi_tcxo_get_us();
    if (memcpy_s(test_buf1, RESERVED_SHARE_MEM_LENGTH, test_buf, RESERVED_SHARE_MEM_LENGTH) != 0) {
        at_adapter_print("[comram mem test]comram read memcpy_s fail.\r\n");
    }
    end_time = uapi_tcxo_get_us();
    osal_irq_restore(irq_sts);
    total_time = (uint32_t)(end_time - start_time) - TCXO_ERROR;
    bw = ONE_SECOND / total_time * RESERVED_SHARE_MEM_LENGTH / MB_SIZE;
    at_adapter_print("[comram mem test]comram read time:%dus bw:%dMB/s\r\n", total_time, bw);

    irq_sts = osal_irq_lock();
    start_time = uapi_tcxo_get_us();
    if (memcpy_s(test_buf, RESERVED_SHARE_MEM_LENGTH, test_buf1, RESERVED_SHARE_MEM_LENGTH) != 0) {
        at_adapter_print("[comram mem test]comram write memcpy_s fail.\r\n");
    }
    end_time = uapi_tcxo_get_us();
    osal_irq_restore(irq_sts);
    total_time = (uint32_t)(end_time - start_time) - TCXO_ERROR;
    bw = (ONE_SECOND / total_time) * RESERVED_SHARE_MEM_LENGTH / (MB_SIZE);
    at_adapter_print("[comram mem test]comram write time:%dus bw:%dMB/s\r\n", total_time, bw);
    osal_pool_mem_free(OS_EXTEND_MEM_ADDR, test_buf);

    return AT_RET_OK;
}

int32_t fmc_nand_get_id(void **ids);
int get_emmc_cid(uint32_t *raw_cid);
#define AT_EMMC_ID_INDEX0 0
#define AT_EMMC_ID_INDEX1 1
#define AT_EMMC_ID_INDEX2 2
#define AT_EMMC_ID_INDEX3 3
#define AT_PSRAM_WINBOND_MASK 0xF
at_ret_t at_get_peripherals_id(const peripherals_type_t *args)
{
    uint32_t deviceid[4] = {0};
    errcode_t ret;
    switch (args->type) {
#ifdef CFG_DRIVERS_MMC
        case 0:     /* 0---emmc */
        {
            get_emmc_cid(deviceid);
            PRINT("EMMC (CID %08x%08x%08x%08x)\n", deviceid[AT_EMMC_ID_INDEX0], deviceid[AT_EMMC_ID_INDEX1],
                  deviceid[AT_EMMC_ID_INDEX2], deviceid[AT_EMMC_ID_INDEX3]);
            break;
        }
#endif
#ifdef CFG_DRIVERS_NANDFLASH
        case 1:     /* 1---nand */
        {
            uint8_t *nand_ids;
            fmc_nand_get_id((void **)&nand_ids);
            PRINT("nand ID 0x%x%x\n", nand_ids[0], nand_ids[1]);
            break;
        }
#endif
        case 2:     /* 2---nor */
        {
            sfc_info_t info = {0};
            ret = uapi_sfc_get_flash_info(SFC_ID_0, &info);
            if (ret != ERRCODE_SUCC) {
                PRINT("get sfc info error, ret = 0x%x", ret);
                break;
            }
            PRINT("flash_id is 0x%x, flash_size is 0x%xB\r\n", info.flash_id, info.flash_size);
            break;
        }
        case 3:     /* 3---psram */
        {
            ret = hal_dmac_read_mr_reg(1, &deviceid[0]);
            if (ret != ERRCODE_SUCC) {
                PRINT("PSRAM get mr1 error=%x\r\n", ret);
                break;
            }
            if (psram_port_winbond_flag_get()) {
                PRINT("PSRAM VENDOR ID=0x%02X\n", (deviceid[0] >> OFFSET_16) & AT_PSRAM_WINBOND_MASK);
            } else {
                PRINT("PSRAM MR1=0x%02X\n", (uint8_t)deviceid[0]);
            }
            break;
        }
        default:
            break;
    }
    return AT_RET_OK;
}

typedef enum {
    AT_PINMUX,
    AT_PAD_DS,
    AT_PAD_IE,
    AT_PAD_PULL,
    AT_PAD_ST,
    AT_PAD_INFO,
    AT_GPIO_DIR,
    AT_GPIO_VAL,
    AT_GPIO_INFO,
    AT_GPIO_EN_ISR,
    AT_GPIO_UN_ISR,
} at_io_config_t;
at_ret_t at_set_pad_config(uint32_t type, uint32_t pin, uint32_t val)
{
    switch (type) {
        case AT_PINMUX:     /* 0---set pinmux */
            uapi_pin_set_mode(pin, val);
            PRINT("set mode pin:%d mode:%d\n", pin, val);
            break;
        case AT_PAD_DS:     /* 1---set ds */
            uapi_pin_set_ds(pin, val);
            PRINT("set ds pin:%d ds:%d\n", pin, val);
            break;
        case AT_PAD_IE:     /* 2---set ie */
            uapi_pin_set_ie(pin, val);
            PRINT("set ie pin:%d ie:%d\n", pin, val);
            break;
        case AT_PAD_PULL:     /* 3---set pull */
            uapi_pin_set_pull(pin, val);
            PRINT("set pull pin:%d pull:%d\n", pin, val);
            break;
        case AT_PAD_ST:     /* 3---set st */
            uapi_pin_set_st(pin, val);
            PRINT("set st pin:%d st:%d\n", pin, val);
            break;
        default:
            break;
    }
    return AT_RET_OK;
}

static void gpio_callback_func(pin_t pin, uintptr_t param)
{
    unused(pin);
    unused(param);
    PRINT("PIN:%d interrupt success. \r\n", pin);
}

at_ret_t at_set_gpio_config(uint32_t type, uint32_t pin, uint32_t val)
{
    switch (type) {
        case AT_GPIO_DIR:
            uapi_gpio_set_dir(pin, val);
            PRINT("set gpio dir pin:%d dir:%d\n", pin, val);
            break;
        case AT_GPIO_VAL:
            uapi_gpio_set_val(pin, val);
            PRINT("set gpio val pin:%d val:%d\n", pin, val);
            break;
        case AT_GPIO_INFO:
            PRINT("get gpio info pin:%d dir:%d val:%d\n", pin, uapi_gpio_get_dir(pin), uapi_gpio_get_val(pin));
            break;
        case AT_GPIO_EN_ISR:
            if (uapi_gpio_register_isr_func(pin, val, gpio_callback_func) != ERRCODE_SUCC) {
                PRINT("PIN: %d int registet failed. \r\n", pin);
                uapi_gpio_unregister_isr_func(pin);
                PRINT("PIN: %d int unregistet success. \r\n", pin);
            } else {
                PRINT("PIN: %d int registet success. \r\n", pin);
            }
            break;
        case AT_GPIO_UN_ISR:
            uapi_gpio_unregister_isr_func(pin);
            PRINT("unregister gpio isr pin:%d \n", pin);
            break;
        default:
            break;
    }
    return AT_RET_OK;
}

at_ret_t at_io_config(const ioconfig_type_t *args)
{
    if (args->type < AT_PAD_INFO) {
        at_set_pad_config(args->type, args->pin, args->val);
    }

    if (args->type == AT_PAD_INFO) {
        PRINT("pin:%d mode:%d ds:%d st:%d ie:%d pull:%d\n", args->pin, uapi_pin_get_mode(args->pin),
              uapi_pin_get_ds(args->pin), uapi_pin_get_st(args->pin),
              uapi_pin_get_ie(args->pin), uapi_pin_get_pull(args->pin));
    }

    if (args->type > AT_PAD_INFO) {
        at_set_gpio_config(args->type, args->pin, args->val);
    }
    return AT_RET_OK;
}

at_ret_t mcu_test_norflash_baudwidth(void)
{
#define TEST_NORFLASH_ADDR (FOTA_FLASH_REGION_START - FLASH_START)
#define NORFLASH_TEST_MB_TO_KB 1024
    uint64_t start_time, end_time;
    uint32_t total_time;
    uint32_t bw;

    char *test_buf = (char *)(APP_DTCM_ORIGIN + DTCM_RAM_OFF);
    uint32_t irq_sts = osal_irq_lock();
    start_time = uapi_tcxo_get_us();
    if (memcpy_s(test_buf, BUFF_LEN_48K, (void *)FLASH_START, BUFF_LEN_48K) != 0) {
        at_adapter_print("[norflash mem test]norflash read memcpy_s fail.\r\n");
    }
    end_time = uapi_tcxo_get_us();
    osal_irq_restore(irq_sts);
    total_time = (uint32_t)(end_time - start_time);
    bw = (ONE_SECOND / total_time) * BUFF_LEN_48K / (MB_SIZE);
    at_adapter_print("[norflash mem test]nor flash read time:%dus bw:%dMB/s\r\n", total_time, bw);

    irq_sts = osal_irq_lock();
    start_time = uapi_tcxo_get_us();
    errcode_t ret = uapi_sfc_erase(0, TEST_NORFLASH_ADDR, BUFF_LEN_48K);
    if (ret != ERRCODE_SUCC) {
        at_adapter_print("erase fail, ret[0x%x]. \r\n", ret);
    }
    ret = uapi_sfc_write(0, TEST_NORFLASH_ADDR, (uint8_t *)test_buf, BUFF_LEN_48K);
    if (ret != ERRCODE_SUCC) {
        at_adapter_print("write fail, ret[0x%x]", ret);
    }
    end_time = uapi_tcxo_get_us();
    osal_irq_restore(irq_sts);
    total_time = (uint32_t)(end_time - start_time) - TCXO_ERROR;
    bw = (ONE_SECOND * NORFLASH_TEST_MB_TO_KB / total_time) * BUFF_LEN_48K / (MB_SIZE);
    at_adapter_print("[norflash mem test]nor write time:%dus bw:%dKB/s\r\n", total_time, bw);
    osal_pool_mem_free(OS_EXTEND_MEM_ADDR, test_buf);

    return AT_RET_OK;
}

at_ret_t cmd_set_pm(const pm_args_t *args)
{
    unused(args);
#if defined(ENABLE_LOW_POWER) && (ENABLE_LOW_POWER == YES)
    bool open_pm = (args->para1 == 0) ? false : true;
    idle_set_open_pm(open_pm);
#endif
    return AT_RET_OK;
}

at_ret_t at_dfx_pm_rm_vote(void)
{
#if defined(ENABLE_LOW_POWER) && (ENABLE_LOW_POWER == YES)
    uapi_pm_remove_sleep_veto(PM_ID_DEBUG);
#endif
    return AT_RET_OK;
}

at_ret_t at_dfx_pm_add_vote(void)
{
#if defined(ENABLE_LOW_POWER) && (ENABLE_LOW_POWER == YES)
    uapi_pm_add_sleep_veto(PM_ID_DEBUG);
#endif
    return AT_RET_OK;
}

#ifdef CONFIG_SUPPORT_NPU_SAMPLES
at_ret_t at_npu_sample_fcw(void)
{
    fuzzyCommandWordMain();
    return AT_RET_OK;
}
#endif

at_ret_t at_sub_power_info(void)
{
    pmu_sub_info_t *pmu_sub_info_tmp = get_pmu_sub_info();

    PRINT("power on count bt:%u, audio:%u, display:%u, vau:%u, h264:%u, hsperp:%u," NEWLINE,
        pmu_sub_info_tmp[PMU_BT_SUB].poweron_cnt, pmu_sub_info_tmp[PMU_AUDIO_SUB].poweron_cnt, pmu_sub_info_tmp[PMU_DISPLAY_SUB].poweron_cnt,
        pmu_sub_info_tmp[PMU_VAU_SUB].poweron_cnt, pmu_sub_info_tmp[PMU_H264_SUB].poweron_cnt, pmu_sub_info_tmp[PMU_MCU_HS_PERP_SUB].poweron_cnt);
    PRINT("power off count bt:%u, audio:%u, display:%u, vau:%u, h264:%u, hsperp:%u," NEWLINE,
        pmu_sub_info_tmp[PMU_BT_SUB].poweroff_cnt, pmu_sub_info_tmp[PMU_AUDIO_SUB].poweroff_cnt, pmu_sub_info_tmp[PMU_DISPLAY_SUB].poweroff_cnt,
        pmu_sub_info_tmp[PMU_VAU_SUB].poweroff_cnt, pmu_sub_info_tmp[PMU_H264_SUB].poweroff_cnt, pmu_sub_info_tmp[PMU_MCU_HS_PERP_SUB].poweroff_cnt);
    return AT_RET_OK;
}

at_ret_t cmd_set_pm_debug_level(const pm_debug_args_t *args)
{
    unused(args);
    pm_port_set_debug_level(args->para1);
    return AT_RET_OK;
}

at_ret_t at_set_sleep_mode(const sleepmode_args_t *args)
{
    unused(args);
#if defined(ENABLE_LOW_POWER) && (ENABLE_LOW_POWER == YES)
    uint32_t slp_mode = args->para1;
    if (slp_mode == 0) {                    /* 0代表关闭低功耗 */
        idle_set_open_pm(false);
    } else if (slp_mode == 1) {             /* 1代表进入浅睡模式 */
        idle_set_open_pm(true);
        uapi_pm_add_sleep_veto(PM_ID_DEBUG);
    } else if (slp_mode == 2) {             /* 2代表进入深睡模式 */
        idle_set_open_pm(true);
        uapi_pm_remove_sleep_veto(PM_ID_DEBUG);
    }
#endif
    return AT_RET_OK;
}

// 各项目中完成下面两个函数实现
#define WATCHDOG_TIMEOUT            0
#define HARDFUALT_PMP_ERROR         1
#define PANIC_ERROR                 2
#define REE_ACCESS_TEE_ERROR        3

static void pmp_access_error(void)
{
#if CORE == APPS
    writew(0x26000000, 0x1234); // write norflash
#elif CORE == BT
    reg32_clrbit(0x59000800, 0x8); // close coex_uart clock
    writew(0x59010004, 0x5a); // write data to the fifo of coex_uart
#endif
}

void do_exception_test(uint8_t type)
{
    uint32_t val;
    uint32_t irq_sts;

    switch (type) {
        case WATCHDOG_TIMEOUT:
            irq_sts = osal_irq_lock();
            while (1) { }
            osal_irq_restore(irq_sts);
            break;
        case HARDFUALT_PMP_ERROR:
            pmp_access_error();
            break;
        case PANIC_ERROR:
            panic(PANIC_EXCEPTION_TEST, __LINE__);
            break;
        case REE_ACCESS_TEE_ERROR:
            val = readl(0x0);
            writel(val, 0x100);
            break;
        default :
            break;
    }
}

#ifdef IPC_NEW
typedef struct {
    uint32_t src_type;
    ipc_exception_test_command_e dst_type;
} diag_crash_type_t;

static diag_crash_type_t g_bt_crash_type[] = {
    {WATCHDOG_TIMEOUT, EXCEPTION_TEST_COMMAND_BT_WDT_REBOOT},
    {HARDFUALT_PMP_ERROR, EXCEPTION_TEST_COMMAND_BT_HARDFAULT},
    {PANIC_ERROR, EXCEPTION_TEST_COMMAND_BT_PANIC},
    {REE_ACCESS_TEE_ERROR, EXCEPTION_TEST_COMMAND_BT_STD_CHIP_WDT_FRST},
};
static diag_crash_type_t g_dsp_crash_type[] = {
    {WATCHDOG_TIMEOUT, EXCEPTION_TEST_COMMAND_BT_WDT_REBOOT},
    {HARDFUALT_PMP_ERROR, EXCEPTION_TEST_COMMAND_BT_HARDFAULT},
    {PANIC_ERROR, EXCEPTION_TEST_COMMAND_BT_PANIC},
    {REE_ACCESS_TEE_ERROR, EXCEPTION_TEST_COMMAND_BT_STD_CHIP_WDT_FRST},
};

static const diag_crash_type_t* g_crash_type_ptr[] = {
    g_bt_crash_type,
    g_dsp_crash_type,
};
#endif

void wear_send_crash_test_info(uint8_t cores, uint32_t type)
{
    unused(cores);
    unused(type);
#ifdef IPC_NEW
    uint8_t i, cnt;
    ipc_payload_exception_test test_command;
    const diag_crash_type_t* mem_ptr = g_crash_type_ptr[(uint8_t)cores];
    if (cores == BT) {
        cnt = (uint8_t)(sizeof(g_bt_crash_type) / sizeof(diag_crash_type_t));
    } else if (cores == PROTOCOL) {
        cnt = (uint8_t)(sizeof(g_dsp_crash_type) / sizeof(diag_crash_type_t));
    } else {
        PRINT("%s %d cores:%d type:%d failed\r\n", __func__, __LINE__, cores, type);
        return;
    }
    for (i = 0; i < cnt; i++) {
        if (mem_ptr[i].src_type == type) {
            test_command.command = mem_ptr[i].dst_type;
            break;
        }
    }
    if (i == cnt) {
        PRINT("%s %d cores:%d type:%d failed\r\n", __func__, __LINE__, cores, type);
        return;
    }

    ipc_msg_info_t msg_info = {0};
    msg_info.dst_core = cores;
    msg_info.priority = 1;
    msg_info.channel = 0;
    msg_info.msg_id = IPC_MSG_EXCEPTION_TEST;
    msg_info.buf_addr = (uint8_t *)&test_command;
    msg_info.buf_len = sizeof(ipc_payload_exception_test);
    errcode_t ret = uapi_ipc_send_msg_async(&msg_info);
    PRINT("%s %d cores:%d type:%d command:%u ret:%d\r\n", __func__, __LINE__, cores, type, test_command.command, ret);
#endif
}

at_ret_t wear_crash_test(const wear_crash_args_t *argc)
{
    int32_t cores = (int32_t)argc->core;
    int32_t type = (int32_t)argc->type;

    if (cores == APPS) {
        do_exception_test(type);
    } else {
        wear_send_crash_test_info(cores, type);
    }

    return AT_RET_OK;
}

#ifdef AT_IPC_V1_TEST
at_ret_t a2x_ipc_v1_basic(const wear_ipc_test_args_t *argc)
{
    ipc_test_params param;
    param.src_core = argc->src_core;
    param.dst_core = argc->dst_core;
    param.cmd_type = argc->cmd_type;
    param.channel = argc->channel;
    param.times = argc->times;
    param.size = argc->size;
    at_adapter_print("src = %d, dst = %d, cmd_type = %d, channel = %d, times = %d, size = %d. \r\n",
        argc->src_core, param.dst_core, param.cmd_type, param.channel, param.times, param.size);

    if (param.src_core == param.dst_core) {
        at_adapter_print("not allow src==dst\r\n");
        return AT_RET_RUN_ERROR;
    }

    if (param.cmd_type == IPC_TEST_ASYNC || param.cmd_type == IPC_TEST_ASYNC_MULTI) {
        test_ipc_async(&param);
        return AT_RET_OK;
    }

    if (argc->src_core == CORES_APPS_CORE) {
        test_ipc_basic_send(&param);
    } else {
        ipc_msg_info_t msg_info = {0};
        msg_info.dst_core = argc->src_core;
        msg_info.msg_id = IPC_MSG_AT_A2X_TEST;
        msg_info.channel = 1;
        msg_info.priority = IPC_PRIORITY_LOWEST;
        msg_info.buf_addr = (uint8_t *)&param;
        msg_info.buf_len = sizeof(ipc_test_params);
        PRINT("[%s %d] \r\n", __func__, __LINE__);
        uapi_ipc_send_msg_async(&msg_info);
    }
    return AT_RET_OK;
}
#endif

#ifdef SUPPORT_LWIP
static uint32_t get_rndis_net_info(const char* src, ip4_addr_t* net_info)
{
    const char *tmp_bc = src;
    char *end = NULL;
    int a[4] = {0}; // len 4
    for (int i = 0; i < 4; i++) { // len 4
        a[i] = (int)strtol(tmp_bc, &end, 10); // base 10
        if (end == tmp_bc) {
            return AT_RET_CMD_PARA_ERROR;
        } // 没读到数字
        if (i < 3) { // index 3
            if (*end != '.') {
                return AT_RET_CMD_PARA_ERROR;
            }
            tmp_bc = end + 1;
        } else {
            if (*end != '\0') {
                return AT_RET_CMD_PARA_ERROR;
            }
        }
    }
    IP4_ADDR(net_info, a[0], a[1], a[2], a[3]); // index 1, 2, 3
    return AT_RET_OK;
}

at_ret_t at_usb_device_rndis_net(const at_usb_device_rndis_args_t* args)
{
    const char *net_ip = args->ip;
    const char *net_nm = args->netmask;
    const char *net_gw = args->gw;
    ip_info_t ip_info;
    if (get_rndis_net_info(net_ip, &ip_info.ipaddr) != AT_RET_OK ||
        get_rndis_net_info(net_nm, &ip_info.netmask) != AT_RET_OK ||
        get_rndis_net_info(net_gw, &ip_info.gw) != AT_RET_OK) {
        at_adapter_print("error params\r\n");
        return AT_RET_CMD_PARA_ERROR;
    }
    uapi_usb_set_rndis_netif_info(ip_info);
    uapi_usb_device_init(DEV_ETHERNET);
    return AT_RET_OK;
}
#endif

at_ret_t at_set_log_uart_baud(const log_uart_args_t *args)
{
    uint32_t baud = args->baud_rate;
    PRINT("log uart baud=%X\r\n", baud);
    log_uart_port_reinit(baud);
    return AT_RET_OK;
}
#ifdef CONFIG_PRODUCT_EVB_DITING
#include "burn_ctrl.h"
#include "data_convertor.h"
static uint32_t handler_burn_error(uint32_t error_no, int rets, bin_info_t *bins, burn_context_t* ctx)
{
    if (error_no == 0x2) {
        PRINT("2131 burn malloc failed !\r\n");
    } else if (error_no == 0x3) {
        PRINT("Parse fwpkg file failed !\r\n");
    } else if (error_no == 0x4) {
        PRINT("Open uart failed ret = %d !\r\n", rets);
    } else if (error_no == 0x5) {
        PRINT("Handshake failed ret = %d !\r\n", rets);
    } else if (error_no == 0x6) {
        PRINT("Burn failed ret = %d\r\n", rets);
    }
    dump_burn_uart_buffer();
    if (bins != NULL) {
        free(bins);
    }
    burn_close(ctx);
    return AT_RET_RUN_ERROR;
}

uint32_t parse2131_fwpkg_file(bin_info_t *bins, uint32_t* bin_count)
{
#define FWPKG_VERSION_LEN  64
    char version[FWPKG_VERSION_LEN] = {0};
    char fwpkg_path[] = "/user/2131_package/hi_2131_v100.fwpkg";
    if (dc_parse_fwpkg(fwpkg_path, bins, BURN_MAX_FILES, bin_count, version, sizeof(version)) != 0) {
        return AT_RET_RUN_ERROR;
    }
    PRINT("Parse fwpkg success !\n");
    PRINT("Fwpkg version = %s, bin count = %d \n", version, *bin_count);
    return AT_RET_OK;
}

at_ret_t at_burn_2131_fwpkg(const burn_2131_args_t *args)
{
    uint32_t burn_baud = args->baud_rate;
    uint32_t radio = args->burn_radio_config;
    burn_context_t ctx = {0};
    uint32_t bin_count = 0;
    bin_info_t *bins = malloc(sizeof(bin_info_t) * BURN_MAX_FILES);
    if (bins == NULL || parse2131_fwpkg_file(bins, &bin_count)) {
        uint32_t burn_err_no = (bins == NULL) ? 0x2: 0x3;
        return handler_burn_error(burn_err_no, 0, bins, &ctx);
    }
    burn_serial_cfg_t cfg = {0};
    cfg.baud = burn_baud;
    cfg.databits = 8; // bit 8
    cfg.stopbits = 1;
    cfg.parity = 0;
    cfg.flowctrl = 0;
    burn_io_t io = {0};
    burn_context_init(&ctx, &io);
    burn_host_info_t host_info;
    serial_register_uart_func(&host_info, &io);
    int rets = burn_open(&ctx, &cfg);
    if (rets != BURN_ERR_OK) {
        return handler_burn_error(0x4, rets, bins, &ctx);
    }
    PRINT("Open Uart success !\n");
    rets = burn_handshake(&ctx, &cfg);
    if (rets != BURN_ERR_OK) {
        return handler_burn_error(0x5, rets, bins, &ctx);
    }
    PRINT("Start burn !\n");
    rets = burn_execute(&ctx, bins, bin_count, radio);
    if (rets != BURN_ERR_OK) {
        return handler_burn_error(0x6, rets, bins, &ctx);
    }
    if (bins != NULL) {
        free(bins);
    }
    burn_close(&ctx);
    PRINT("Burn success\n");
    return AT_RET_OK;
}
#endif

at_ret_t at_mmc_tune_test(void)
{
#ifdef CFG_DRIVERS_MMC
    PRINT("start emmc tuning !!\r\n");
    do_emmc_tuning();
#endif
    return AT_RET_OK;
}

at_ret_t at_sdio_set_default_tune(void)
{
    PRINT("set sdio default tune .\r\n");
    // sdio host rx clk
    reg16_clrbit(HS_CTL_RB_SDIO_TUNING_CCLK_RX_CTL_REG, HS_CTL_RB_SDIO_HOST_RX_CLK_INV_SEL_OFFSET);
    reg16_setbit(HS_CTL_RB_SDIO_TUNING_CCLK_RX_CTL_REG, HS_CTL_RB_SDIO_SAMPLE_CCLK_RX_SEL_OFFSET); // 使用tunning 调相

    reg16_clrbit(HS_CTL_RB_SDIO_TUNING_CCLK_RX_CTL_REG, HS_CTL_RB_SDIO_TUNING_CCLK_RX_SEL_UPDATA_OFFSET);
    reg16_setbits(HS_CTL_RB_SDIO_TUNING_CCLK_RX_CTL_REG, 0x8, 0x3, 0x7); // bit 4-7 细调相位; bit 8-10 粗调相位;
    reg16_setbit(HS_CTL_RB_SDIO_TUNING_CCLK_RX_CTL_REG, HS_CTL_RB_SDIO_TUNING_CCLK_RX_SEL_UPDATA_OFFSET);

    // sdio host card clk
    reg16_clrbit(HS_CTL_RB_SDIO_TUNING_CARD_CLK_CTL_REG, HS_CTL_RB_SDIO_HOST_CARD_CLK_INV_SEL_OFFSET);
    reg16_setbit(HS_CTL_RB_SDIO_TUNING_CARD_CLK_CTL_REG, HS_CTL_RB_SDIO_SAMPLE_CARD_CLK_SEL_OFFSET); // 使用tunning 调相

    reg16_clrbit(HS_CTL_RB_SDIO_TUNING_CARD_CLK_CTL_REG, HS_CTL_RB_SDIO_TUNING_CARD_CLK_SEL_UPDATA_OFFSET);
    reg16_setbits(HS_CTL_RB_SDIO_TUNING_CARD_CLK_CTL_REG, HS_CTL_RB_SDIO_TUNING_CARD_CLK_SEL_OFFSET,
        HS_CTL_RB_SDIO_TUNING_CARD_CLK_SEL_LEN, 0x5); // bit 4-6 粗调相位;
    reg16_setbit(HS_CTL_RB_SDIO_TUNING_CARD_CLK_CTL_REG, HS_CTL_RB_SDIO_TUNING_CARD_CLK_SEL_UPDATA_OFFSET);

    return AT_RET_OK;
}

at_ret_t at_core_power_handle(const core_power_args_t *args)
{
#ifdef CONFIG_SUPPORT_COREPOWER_SWITCH
    PRINT("at_core_power_handle core[%u], sw[%u]\r\n", args->core_type, args->sw);
    if (uapi_pmu_control(args->core_type, args->sw) != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
#endif
    return AT_RET_OK;
}

at_ret_t at_set_voltage(const voltage_cfg_args_t *args)
{
    errcode_t ret = ERRCODE_FAIL;
    if (args->ldotype <= PMU_BUCK_LDO_END) {
        ret = uapi_pmu_ldo_set_voltage(args->ldotype, args->voltage);
    }

    if (ret == ERRCODE_SUCC) {
        return AT_RET_OK;
    }
    return AT_RET_CMD_PARA_ERROR;
}

at_ret_t at_print_vetoinfo(void)
{
    pm_veto_t *veto_info = uapi_pm_veto_get_info();

    at_adapter_print("\r\n");

    at_adapter_print("pm veto total counts: [%u]\r\n", veto_info->veto_counts.total_counts);
    for (uint8_t veto_id = 0; veto_id < PM_VETO_ID_MAX; veto_id++) {
        if (veto_info->veto_counts.sub_counts[veto_id] != 0x0) {
            at_adapter_print("pm sub counts modid: [%u]\r\n", veto_id);
        }
    }
    at_adapter_print("pm veto lastveto id: [%u], lastveto lr: [%x], timeout_timestamp: [%llu]\r\n",
        veto_info->last_veto_id, veto_info->last_veto_lr, veto_info->veto_timeout_timestamp);

    at_adapter_print("\r\n");

    return AT_RET_OK;
}

#if defined(CONFIG_PM_VETO_TRACK_ENABLE) && (CONFIG_PM_VETO_TRACK_ENABLE == 1)
at_ret_t at_print_veto_trackinfo(const veto_track_loc_args_t *args)
{
    if (args == NULL) {
        return AT_RET_CMD_PARA_ERROR;
    }
    pm_veto_track_t *veto_track_info = uapi_pm_veto_get_trackinfo();
    uint16_t output_locs = args->output_loc > veto_track_info->track_loc ?
        veto_track_info->track_loc : args->output_loc;

    at_adapter_print("\r\n");

    at_adapter_print("pm veto track config: track_mask [%u], track_len [%u], track_loc [%u]\r\n",
        veto_track_info->track_mask, veto_track_info->track_len, veto_track_info->track_loc);
    at_adapter_print("pm veto track records:\r\n");

    for (uint8_t loc = 0; loc < output_locs; loc++) {
        at_adapter_print("pm veto track record-[%u]: vetotime:[%llu], vetid:[%u], vetocounts[%u], veto_lr[%x],\
            veto_timeout[%u]\r\n", loc, veto_track_info->track_info[loc].veto_time,
            veto_track_info->track_info[loc].veto_id, veto_track_info->track_info[loc].veto_counts,
            veto_track_info->track_info[loc].veto_lr, veto_track_info->track_info[loc].veto_timeout);
    }

    at_adapter_print("\r\n");

    return AT_RET_OK;
}
#endif

at_ret_t at_enter_shipmode(void)
{
    uapi_sys_shipmode(0);
    return AT_RET_OK;
}

at_ret_t at_set_efuse_bit(const efuse_bit_args_t *args)
{
    at_adapter_print("set efuse byte: %d, bit: %d"NEWLINE, args->start_byte, args->bit_offset);
    if (uapi_efuse_write_bit(args->start_byte, args->bit_offset) != ERRCODE_SUCC) {
        return AT_RET_PROC_SET_FUNC_MISSING;
    }
    return AT_RET_OK;
}

at_ret_t at_get_efuse_bit(const efuse_bit_args_t *args)
{
    uint8_t value;
    at_adapter_print("get efuse byte: %d, bit: %d"NEWLINE, args->start_byte, args->bit_offset);
    if (uapi_efuse_read_bit(&value, args->start_byte, args->bit_offset) != ERRCODE_SUCC) {
        return AT_RET_PROC_READ_FUNC_MISSING;
    }
    at_adapter_print("value is 0x%x"NEWLINE, value);
    return AT_RET_OK;
}

at_ret_t at_get_version(void)
{
    at_adapter_print("REE SDK Version is %s"NEWLINE, get_version_string());
    at_adapter_print("TEE SDK Version is S%x"NEWLINE, get_tee_version());

    uint32_t key_ver = 0;
    uint32_t code_ver = 0;
    secure_verify_get_board_version(SSB_FLASH_REGION_START, &key_ver, &code_ver);
    at_adapter_print("flashboot image key ver is 0x%x, code ver is 0x%x"NEWLINE, key_ver, code_ver);

    secure_verify_get_board_version(SELITEOS_FLASH_REGION_START, &key_ver, &code_ver);
    at_adapter_print("seliteos image key ver is 0x%x, code ver is 0x%x"NEWLINE, key_ver, code_ver);

    secure_verify_get_board_version(RECOVERY_FLASH_REGION_START, &key_ver, &code_ver);
    at_adapter_print("Recovery image key ver is 0x%x, code ver is 0x%x"NEWLINE, key_ver, code_ver);

    secure_verify_get_board_version(APP_FLASH_REGION_START, &key_ver, &code_ver);
    at_adapter_print("Application image key ver is 0x%x, code ver is 0x%x"NEWLINE, key_ver, code_ver);
#ifndef LOAD_IMAGE_FROM_FS
    secure_verify_get_board_version(BT_FLASH_REGION_START, &key_ver, &code_ver);
    at_adapter_print("BT image key ver is 0x%x, code ver is 0x%x"NEWLINE, key_ver, code_ver);
#endif
    uint8_t chip_version[0x6];
    uapi_efuse_get_chip_version(chip_version, 0x6);
    at_adapter_print("Chip Version is %s"NEWLINE, chip_version);

    return ERRCODE_SUCC;
}

at_ret_t at_get_die_id(void)
{
#define DIE_ID_BYTE_LEN     20
    uint8_t die_id[DIE_ID_BYTE_LEN];
    uapi_efuse_get_die_id(die_id, DIE_ID_BYTE_LEN);
    at_adapter_print("Chip DIE ID is:");
    for (int i = 0; i < DIE_ID_BYTE_LEN; i++) {
        at_adapter_print("0x%x ", die_id[i]);
    }
    at_adapter_print(NEWLINE);
    return ERRCODE_SUCC;
}

at_ret_t at_tp_debug(void)
{
#ifdef SUPPORT_TOUCH_FEATURE
    (void)touch_init();
#endif
    return ERRCODE_SUCC;
}

static at_ret_t emmc_mem_test(void)
{
    // todo
    return AT_RET_OK;
}

static at_ret_t psram_mem_test(void)
{
    uint32_t test_value[0x4] = { 0x5A5A5A5A, 0xA5A5A5A5, 0xFFFFFFFF, 0x00000000 };
    for (uint32_t i = 0; i < 0x4; i++) {
        PRINT("psram memory test value 0x%x"NEWLINE, i, test_value[i]);
        uint32_t addr = PSRAM_NPU_APP_HEAP_START;
        while (addr < (PSRAM_NPU_APP_HEAP_START + PSRAM_NPU_APP_HEAP_LENGTH)) {
            writel(addr, test_value[i]);
            if (readl(addr) != test_value[i]) {
                PRINT("err addr 0x%x, value 0x%x"NEWLINE, addr, test_value[i]);
            }
            addr = addr + 0x4;
        }

#ifndef CONFIG_WDT_UNIFIED_CONTROL
        uapi_watchdog_kick();
#endif
    }
    return AT_RET_OK;
}

static at_ret_t norflash_mem_test(void)
{
    while (1) {
        at_adapter_print("FLASH_START addr: 0x%x, value: 0x%x.\r\n", FLASH_START, readl(FLASH_START));
        at_adapter_print("ssb addr: 0x%x, value: 0x%x.\r\n", SSB_FLASH_REGION_START, readl(SSB_FLASH_REGION_START));
        at_adapter_print("app addr: 0x%x, value: 0x%x.\r\n", APP_FLASH_REGION_START, readl(APP_FLASH_REGION_START));
        uapi_tcxo_delay_ms(500); // delay 500ms
        uapi_watchdog_kick();
    }
    return AT_RET_OK;
}

at_ret_t at_module_mem_test(const module_mem_test_args_t *args)
{
    PRINT("at_check_module: %s.\r\n", args->module);
    if (strcmp((char*)(uintptr_t)"PSRAM", (char*)(uintptr_t)args->module) == 0) {
        return psram_mem_test();
    }
    if (strcmp((char*)(uintptr_t)"NORFLASH", (char*)(uintptr_t)args->module) == 0) {
        return norflash_mem_test();
    }
    if (strcmp((char*)(uintptr_t)"EMMC", (char*)(uintptr_t)args->module) == 0) {
        return emmc_mem_test();
    }
    at_adapter_print("wrong module.\r\n");
    return AT_RET_CMD_PARA_ERROR;
}

at_ret_t at_time_set(const time_args_t *args)
{
#define TIME_ZONE_UTC_8             (8 * 60) // UTC+8
    struct tm tm = { 0 };
    tm.tm_year = args->year - YEAR_BASE;
    tm.tm_mon = args->month - 1;
    tm.tm_mday = args->day;
    tm.tm_hour = args->hour;
    tm.tm_min = args->min;
    tm.tm_sec = args->sec;
    time_t sec = mktime(&tm);
    if (sec == (time_t)-1) {
        return AT_RET_RUN_ERROR;
    }

    struct timeval tv = { 0 };
    struct timezone tz = { 0 };
    tv.tv_sec = sec;
    tv.tv_usec = 0;
    tz.tz_dsttime = 0;
    tz.tz_minuteswest = TIME_ZONE_UTC_8;

    at_adapter_print("tv_sec: %d" NEWLINE, tv.tv_sec);

    settimeofday(&tv, &tz);

    at_adapter_print("at_time_set: %d/%d/%d %d:%d:%d" NEWLINE,
        args->year, args->month, args->day, args->hour, args->min, args->sec);

#ifdef SUPPORT_OHOSFWK
#ifndef CFG_NORFLASH_ONLY
    AlarmClockReset();
#endif
#endif

    return AT_RET_OK;
}

#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
at_ret_t at_dfx_show_task_heap(const task_args_t *args)
{
    dfx_show_task_heap(args->task_id);
    return AT_RET_OK;
}

at_ret_t at_dfx_show_all_task_heap(void)
{
    dfx_show_all_task_heap();
    return AT_RET_OK;
}

at_ret_t at_dfx_mem_leak(const mem_leak_t *args)
{
    int32_t cmd_type = (int32_t)args->cmd_type;
    uint32_t task_id = (uint32_t)args->task_id;
    if (cmd_type == GET_CALLER_RA) {
        dfx_get_callerRa(task_id);
    } else if (cmd_type == COMPARE_CALLER_RA) {
        dfx_detect_callerRa(task_id);
    } else if (cmd_type == CLEAN_UP_CALLER_RA_STATE) {
        cleanup_all_callerRa_state();
    }
    return AT_RET_OK;
}

at_ret_t at_dfx_show_mem_info(void)
{
    dfx_show_mem_info();
    return AT_RET_OK;
}

at_ret_t at_dfx_show_stack_waterline(void)
{
    dfx_show_stack_waterline();
    return AT_RET_OK;
}
#endif

at_ret_t at_dfx_list_timer(void)
{
    OsDbgSwtmrInfoGet(OS_ALL_SWTMR_MASK);
    return AT_RET_OK;
}

at_ret_t at_dfx_os_stack_info(void)
{
    OsDbgTskInfoGet(OS_ALL_TASK_MASK);
    return AT_RET_OK;
}

at_ret_t at_dfx_os_heap_info(void)
{
    dfx_show_heap_statistic();
    return AT_RET_OK;
}

#ifdef LOSCFG_SHELL_ADAPT_AT
at_ret_t at_shell_exec(shell_args_t *args)
{
    if (args->cmd_name == NULL || args->cmd_name[0] == '\0') {
        at_adapter_print("no shell command input.\r\n");
        return AT_RET_CMD_PARA_ERROR;
    }
    int32_t ret = OSExecUserCmd(args->cmd_name);
    if (ret < 0) {
        at_adapter_print("shell command failed. err code: %d\r\n", ret);
        return AT_RET_CMD_PARA_ERROR;
    }
    at_adapter_print("shell command OK!\r\n");
    return AT_RET_OK;
}
#endif

#define CLK_STRLEN  12

STATIC const char g_shell_clock_moudle_name[][CLK_STRLEN] = {
    "mcpu",
    "perp ls",
    "perp uart",
    "perp spi",
    "com bus",
    "sec",
    "sdiom",
#ifdef CFG_DRIVERS_MMC
    "emmc",
#else
    "fmc",
#endif
    "can",
    "usb",
    "hifi",
    "codec",
    "sfc",
    "psram",
    "dpu",
    "qspi_lcd",
    "npu",
    "clkout0",
    "clkout1",
};

STATIC const char g_shell_clock_src_name[][CLK_STRLEN] = {
    "xo_rc",
    "dll",
    "pll",
    "pll_vco",
    "NG",
    "32k",
    "NG",
    "pad_clkin",
    "NG",
};

at_ret_t at_dfx_pm_dump(void)
{
    /* clock router */
    at_adapter_print("\nclk_name     src    div  freq"
                     "\n--------     -----  ---  --------\n");
    for (uint32_t i = 0; i < HAL_CLOCKS_MODULE_MAX; i++) {
        uint8_t clk_src = hal_clocks_ccrg_module_get_src((hal_clocks_ccrg_module_t)i);
        uint8_t clk_div = hal_clocks_ccrg_module_get_div((hal_clocks_ccrg_module_t)i);
        uint32_t freq = clocks_ccrg_get_frequency((clocks_ccrg_module_t)i);
        at_adapter_print("%-12s "
                         "%-6s "
                         "%-4u "
                         "%d\n", g_shell_clock_moudle_name[i], g_shell_clock_src_name[clk_src], clk_div, freq);
    }

    /* clokc user mode */
    uapi_clocks_dfx_get_mode_info();

    /* buck mode */
    at_adapter_print("\nBuck 0P8 vol: %d", uapi_pmu_ldo_get_voltage(PMU_BUCK_LDO_BUCK_0P8));
    at_adapter_print("\nBuck 1P9 vol: %d", uapi_pmu_ldo_get_voltage(PMU_BUCK_LDO_BUCK_1P9));
    at_adapter_print("\nBuck 1P0 vol: %d\n", uapi_pmu_ldo_get_voltage(PMU_BUCK_LDO_BUCK_1P0));

    pm_veto_t *veto_info = uapi_pm_veto_get_info();
    at_adapter_print("pm veto total counts: [%u]\r\n", veto_info->veto_counts.total_counts);
    for (uint8_t veto_id = 0; veto_id < PM_VETO_ID_MAX; veto_id++) {
        if (veto_info->veto_counts.sub_counts[veto_id] != 0x0) {
            at_adapter_print("pm sub counts modid: [%u] counts: [%u]\r\n", veto_id,
                veto_info->veto_counts.sub_counts[veto_id]);
        }
    }
    at_adapter_print("pm veto lastveto id: [%u], lastveto lr: [%x], timeout_timestamp: [%llu]\r\n",
        veto_info->last_veto_id, veto_info->last_veto_lr, veto_info->veto_timeout_timestamp);

    /* wakup src */
    pm_dfx_dump_wakeup_src_statistics();
    pm_dfx_clear_wakeup_src_statistics();
    return AT_RET_OK;
}

static errcode_t at_clock_set_allmodule_mode(pm_mode_t mode)
{
    errcode_t ret;
    for (uint32_t i = PM_ID_BSP; i < PM_ID_MAX; i++) {
        ret = uapi_clocks_config_mode(i, mode);
        if (ret != ERRCODE_SUCC) {
            at_adapter_print("set module:%u fail.\n");
            break;
        }
    }
    return ret;
}

at_ret_t at_dfx_pm_req_mode(const pm_module_args_t *args)
{
    at_adapter_print("\nconfig %s mode\n", args->module);
    if (strcmp((char*)(uintptr_t)"LP", (char*)(uintptr_t)args->module) == 0) {
        return at_clock_set_allmodule_mode(PM_MODE_LP);
    }
    if (strcmp((char*)(uintptr_t)"PERFORMACE", (char*)(uintptr_t)args->module) == 0) {
        return at_clock_set_allmodule_mode(PM_MODE_PERFORMACE);
    }
    if (strcmp((char*)(uintptr_t)"NORMAL", (char*)(uintptr_t)args->module) == 0) {
        return at_clock_set_allmodule_mode(PM_MODE_NORMAL);
    }
    if (strcmp((char*)(uintptr_t)"MIDDLE", (char*)(uintptr_t)args->module) == 0) {
        return at_clock_set_allmodule_mode(PM_MODE_MIDDLE);
    }
    at_adapter_print("Para error\n");
    return AT_RET_CMD_PARA_ERROR;
}

at_ret_t at_set_pin(const pin_set_args_t *args)
{
    uapi_pin_set_mode((pin_t)args->pin, (pin_mode_t)args->mode);
    uapi_pin_set_pull((pin_t)args->pin, (pin_pull_t)args->pull);
    uapi_pin_set_ie((pin_t)args->pin, (pin_input_enable_t)args->ie);
    uapi_pin_set_slp_ie((pin_t)args->pin, (uint8_t)args->sleep_ie);
    uapi_pin_set_ds((pin_t)args->pin, (pin_drive_strength_t)args->ds);
    return AT_RET_OK;
}

at_ret_t at_get_pin(const pin_get_args_t *args)
{
    at_adapter_print("pin %d, mode %d, pull %d, ie %d, slp_ie %d, ds %d\n",
                     args->pin,
                     uapi_pin_get_mode((pin_t)args->pin),
                     uapi_pin_get_pull((pin_t)args->pin),
                     uapi_pin_get_ie((pin_t)args->pin),
                     uapi_pin_get_slp_ie((pin_t)args->pin),
                     uapi_pin_get_ds((pin_t)args->pin));
    return AT_RET_OK;
}

at_ret_t at_get_all_pin(void)
{
#define PULL_STRLEN  5

    STATIC const char g_shell_pin_pull_name[][PULL_STRLEN] = {
        "none",
        "down",
        "up",
        "err"
    };

    at_adapter_print("\npin   mode   pull   ie   slp ie   ds"
                     "\n---   ----   ----   --   ------   --\n");
    for (uint32_t i = 0; i < PIN_NONE; i++) {
        at_adapter_print("%-3u   %-4u   %-4s   %-2u   %-6u   %d\n", i,
            uapi_pin_get_mode((pin_t)i),
            g_shell_pin_pull_name[(uint16_t)uapi_pin_get_pull((pin_t)i)],
            uapi_pin_get_ie((pin_t)i),
            uapi_pin_get_slp_ie((pin_t)i),
            uapi_pin_get_ds((pin_t)i));
    }
    return AT_RET_OK;
}

at_ret_t at_set_level_msg(const msg_filter_args_t *argc)
{
    uint16_t level = (int32_t)argc->module_id;
    uint8_t switch_code = (int32_t)argc->switch_code;

    zdiag_set_level_enable((uint8_t)(level), switch_code);

    return AT_RET_OK;
}

at_ret_t at_set_mod_id_msg(const msg_filter_args_t *argc)
{
    uint16_t module_id = (int32_t)argc->module_id;
    uint8_t switch_code = (int32_t)argc->switch_code;

    zdiag_set_id_enable(module_id, switch_code);

    return AT_RET_OK;
}

at_ret_t at_get_log_level(void)
{
    uint32_t level;

    level = diag_get_debug_level();
    at_adapter_print("log level is 0x%x"NEWLINE, level);

    return AT_RET_OK;
}

at_ret_t at_set_log_level(const loglevel_args_t *argc)
{
    int32_t level = (int32_t)argc->para1;

    diag_set_debug_level(level);

    return AT_RET_OK;
}

typedef struct {
    union test_seliteos {
        struct {
            unsigned int read : 1;
            unsigned int write : 1;
            unsigned int exec : 1;
            unsigned int mode : 2;
            unsigned int reserve : 2;
            unsigned int lock : 1;
        } bits;
        unsigned char u8;
    } pmpcfg;
    unsigned int pmpaddr;
    bool pmpte;
    unsigned int idx;
} pmp_item_t;

unsigned int g_pmpcfg_list[8]; // 8: pmp cfg list number
unsigned int g_pmpaddr_list[32]; // 32: pmp addr list number
unsigned int g_pmpte_list[2]; // 2 pmpte list number

static void inner_pmp_init(void)
{
    /* PMPCFG. */
    g_pmpcfg_list[0] = read_csr(0x3A0); // 0: number 0, 0x3A0: pmpcfg0
    g_pmpcfg_list[1] = read_csr(0x3A1); // 1: number 1, 0x3A1: pmpcfg1
    g_pmpcfg_list[2] = read_csr(0x3A2); // 2: number 2, 0x3A2: pmpcfg2
    g_pmpcfg_list[3] = read_csr(0x3A3); // 3: number 3, 0x3A3: pmpcfg3
    g_pmpcfg_list[4] = read_csr(0x3A4); // 4: number 4, 0x3A4: pmpcfg4
    g_pmpcfg_list[5] = read_csr(0x3A5); // 5: number 5, 0x3A5: pmpcfg5
    g_pmpcfg_list[6] = read_csr(0x3A6); // 6: number 6, 0x3A6: pmpcfg6
    g_pmpcfg_list[7] = read_csr(0x3A7); // 7: number 7, 0x3A7: pmpcfg7
    /* PMPADDR. */
    g_pmpaddr_list[0] = read_csr(0x3B0); // 0: number 0, 0x3B0: pmpaddr0
    g_pmpaddr_list[1] = read_csr(0x3B1); // 1: number 1, 0x3B1: pmpaddr1
    g_pmpaddr_list[2] = read_csr(0x3B2); // 2: number 2, 0x3B2: pmpaddr2
    g_pmpaddr_list[3] = read_csr(0x3B3); // 3: number 3, 0x3B3: pmpaddr3
    g_pmpaddr_list[4] = read_csr(0x3B4); // 4: number 4, 0x3B4: pmpaddr4
    g_pmpaddr_list[5] = read_csr(0x3B5); // 5: number 5, 0x3B5: pmpaddr5
    g_pmpaddr_list[6] = read_csr(0x3B6); // 6: number 6, 0x3B6: pmpaddr6
    g_pmpaddr_list[7] = read_csr(0x3B7); // 7: number 7, 0x3B7: pmpaddr7
    g_pmpaddr_list[8] = read_csr(0x3B8); // 8: number 8, 0x3B8: pmpaddr8
    g_pmpaddr_list[9] = read_csr(0x3B9); // 9: number 9, 0x3B9: pmpaddr9
    g_pmpaddr_list[0xa] = read_csr(0x3Ba); // 0xa: number 0xa, 0x3Ba: pmpaddr10
    g_pmpaddr_list[0xb] = read_csr(0x3Bb); // 0xb: number 0xb, 0x3Bb: pmpaddr11
    g_pmpaddr_list[0xc] = read_csr(0x3Bc); // 0xc: number 0xc, 0x3Bc: pmpaddr12
    g_pmpaddr_list[0xd] = read_csr(0x3Bd); // 0xd: number 0xd, 0x3Bd: pmpaddr13
    g_pmpaddr_list[0xe] = read_csr(0x3Be); // 0xe: number 0xe, 0x3Be: pmpaddr14
    g_pmpaddr_list[0xf] = read_csr(0x3Bf); // 0xf: number 0xf, 0x3Bf: pmpaddr15
    g_pmpaddr_list[0x10] = read_csr(0x3C0); // 0x10: number 0x10, 0x3C0: pmpaddr16
    g_pmpaddr_list[0x11] = read_csr(0x3C1); // 0x11: number 0x11, 0x3C1: pmpaddr17
    g_pmpaddr_list[0x12] = read_csr(0x3C2); // 0x12: number 0x12, 0x3C2: pmpaddr18
    g_pmpaddr_list[0x13] = read_csr(0x3C3); // 0x13: number 0x13, 0x3C3: pmpaddr19
    g_pmpaddr_list[0x14] = read_csr(0x3C4); // 0x14: number 0x14, 0x3C4: pmpaddr20
    g_pmpaddr_list[0x15] = read_csr(0x3C5); // 0x15: number 0x15, 0x3C5: pmpaddr21
    g_pmpaddr_list[0x16] = read_csr(0x3C6); // 0x16: number 0x16, 0x3C6: pmpaddr22
    g_pmpaddr_list[0x17] = read_csr(0x3C7); // 0x17: number 0x17, 0x3C7: pmpaddr23
    g_pmpaddr_list[0x18] = read_csr(0x3C8); // 0x18: number 0x18, 0x3C8: pmpaddr24
    g_pmpaddr_list[0x19] = read_csr(0x3C9); // 0x19: number 0x19, 0x3C9: pmpaddr25
    g_pmpaddr_list[0x1a] = read_csr(0x3CA); // 0x1a: number 0x1a, 0x3Ca: pmpaddr26
    g_pmpaddr_list[0x1b] = read_csr(0x3CB); // 0x1b: number 0x1b, 0x3Cb: pmpaddr27
    g_pmpaddr_list[0x1c] = read_csr(0x3CC); // 0x1c: number 0x1c, 0x3Cc: pmpaddr28
    g_pmpaddr_list[0x1d] = read_csr(0x3CD); // 0x1d: number 0x1d, 0x3Cd: pmpaddr29
    g_pmpaddr_list[0x1e] = read_csr(0x3CE); // 0x1e: number 0x1e, 0x3Ce: pmpaddr30
    g_pmpaddr_list[0x1f] = read_csr(0x3CF); // 0x1f: number 0x1f, 0x3Cf: pmpaddr31
    /* PMPTE. */
    g_pmpte_list[0] = read_csr(0x7F0); // 0: number 0, 0x7F0: pmpte0
    g_pmpte_list[1] = read_csr(0x7F1); //  1: number 1, 0x7F1: pmpte1
}

static void inner_read_pmp_item(pmp_item_t *pmp_item)
{
    unsigned int idx = pmp_item->idx;
    pmp_item->pmpaddr = g_pmpaddr_list[idx];
    /* 4: 4 bits, 8: 8 bits,0xFF: 8bits is all 1. */
    pmp_item->pmpcfg.u8 = (g_pmpcfg_list[idx / 4] >> ((idx % 4) * 8)) & 0xFF;
    pmp_item->pmpte = (g_pmpte_list[idx / 32] >> idx) & 1; // 32: all pmp number, 1: bit 0 is 1
}

static void inner_get_pmp_addr(unsigned int pmpaddr, unsigned int *start_addr,
                               unsigned int *end_addr)
{
    unsigned int i;
    if (pmpaddr == 0) {
        *start_addr = *end_addr = 0;
        return;
    }
    for (i = 0; i < 32; i++) { // 32: pmp number
        if (((pmpaddr >> i) & 1) == 0) {
            break;
        }
    }
    *start_addr = (pmpaddr & (0xFFFFFFFF << i)) * 4; // 4: left shift 2 bits
    *end_addr = *start_addr + (1 << (i + 3)); // 3: left shift 3 bits
}

static void inner_pmp_region_dump(unsigned int idx)
{
    pmp_item_t pmp_item = {0};
    unsigned int start_addr;
    unsigned int end_addr;
    pmp_item.idx = idx;
    inner_read_pmp_item(&pmp_item);
    PRINT("========== PMP Configuration for Region %u Begin ==========\n", idx);
    PRINT("pmpcfg is 0x%x\n", pmp_item.pmpcfg.u8);
    PRINT("R: %u, W: %u, X: %u, mode: 0x%x, lock: %u\n",
          pmp_item.pmpcfg.bits.read, pmp_item.pmpcfg.bits.write,
          pmp_item.pmpcfg.bits.exec, pmp_item.pmpcfg.bits.mode,
          pmp_item.pmpcfg.bits.lock);
    inner_get_pmp_addr(pmp_item.pmpaddr, &start_addr, &end_addr);
    PRINT("pmpaddr is 0x%x\n", pmp_item.pmpaddr);
    PRINT("pmpte is 0x%x\n", pmp_item.pmpte);
    PRINT("pmp region 0x%x ~ 0x%x\n", start_addr, end_addr);
    PRINT("========== PMP Configuration for Region %u End ==========\n", idx);
}

at_ret_t at_pmp_dump(void)
{
    inner_pmp_init();
    for (unsigned int i = 0; i < 32; i++) { // 32: pmp number
        inner_pmp_region_dump(i);
    }
    return AT_RET_OK;
}

at_ret_t at_lcd_on(void)
{
#ifdef SUPPORT_POWER_MANAGER
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api->get_screen_state() != SCREEN_ON) {
        display_api->turn_on_screen();
    }
#endif
    return AT_RET_OK;
}

at_ret_t at_lcd_off(void)
{
#ifdef SUPPORT_POWER_MANAGER
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    if (display_api->get_screen_state() != SCREEN_OFF) {
        display_api->turn_off_screen();
    }
#endif
    return AT_RET_OK;
}

at_ret_t at_usb_dfu_enable(void)
{
#define USB_DFU_EN          (710 / 8)
#define USB_FNPLL_EN        (714 / 8)

    uint8_t value = 0;
    uint32_t ret = uapi_efuse_read_bit(&value, USB_DFU_EN, 0x6);
    if (ret != ERRCODE_SUCC) {
        return AT_RET_PROGRESS_BLOCK;
    }

    if (value == 1) {
        at_adapter_print("dfu already enable!"NEWLINE);
        return AT_RET_OK;
    }

    ret = uapi_efuse_write_bit(USB_FNPLL_EN, 0x2);
    if (ret != ERRCODE_SUCC) {
        return AT_RET_PROGRESS_BLOCK;
    }

    ret = uapi_efuse_write_bit(USB_DFU_EN, 0x6);
    if (ret != ERRCODE_SUCC) {
        return AT_RET_PROGRESS_BLOCK;
    }

    at_adapter_print("usb dfu enable!"NEWLINE);
    return AT_RET_OK;
}

at_ret_t at_usb_dfu_soft_trigger(void)
{
#define USB_DFU_EN          (710 / 8)

    uint8_t value = 0;
    uint32_t ret = uapi_efuse_read_bit(&value, USB_DFU_EN, 0x6);
    if ((ret != ERRCODE_SUCC) || (value == 0)) {
        at_adapter_print("dfu no enable!"NEWLINE);
        return AT_RET_PROGRESS_BLOCK;
    }

    /* software trigger bootrom usb dfu process after reboot */
    writew(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_15_REG, 0x5A5A);

    hal_reboot_chip();
    return AT_RET_OK;
}

at_ret_t at_ssi_enable(void)
{
    // aon_bus pd force on
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_SLP_VAL_REG, LP_CTL_GLB_RB_AON_BUS_PWR_EN_SLP_VAL_OFFSET);
    reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_SLP_VAL_REG, LP_CTL_GLB_RB_AON_BUS_ISO_EN_SLP_VAL_OFFSET);

    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_SLP_VAL_5_REG, LP_CTL_GLB_RB_RST_SYS_AON_BUS_CRG_N_SLP_VAL_OFFSET);
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_SLP_VAL_5_REG, LP_CTL_GLB_RB_RST_SYS_AON_BUS_LGC_N_SLP_VAL_OFFSET);

    pm_port_force_sleep();

    at_adapter_print("frc sleep"NEWLINE);
    return AT_RET_OK;
}

#ifdef CFG_DRIVERS_SDIO
errcode_t hcc_init_etc(void);
uint32_t pm_firmware_init(void);
uint32_t firmware_download_etc(void);

at_ret_t at_sdio_cmd53_test(void)
{
    hcc_init_etc();
    pm_firmware_init();
    firmware_download_etc();
    return AT_RET_OK;
}
#endif

#ifdef CFG_DRIVERS_MMC
#define EMMC_SECTOR_SIZE                512
#define EMMC_ECSD_SIZE                  512
#define EMMC_EMMC_DS_SUPPORT_OFFSET     197
#define EMMC_EMMC_DS_VALUE_OFFSET       185
static char g_emmc_read_buf[EMMC_SECTOR_SIZE]__attribute__((__aligned__(64)))  = { 0 };
static char g_emmc_write_buf[EMMC_SECTOR_SIZE]__attribute__((__aligned__(64)))  = { 0 };
static volatile uint8_t g_extend_csd_reg[EMMC_ECSD_SIZE] = {0};
uint32_t EraseDiskByID(uint32_t diskId, uint32_t startSector, uint32_t sectors);

static int hex_char_to_int(char c)
{
    if ('0' <= c && c <= '9') {
        return c - '0';
    }
    if ('a' <= c && c <= 'f') {
        return c - 'a' + 10; // 10进制的数
    }
    if ('A' <= c && c <= 'F') {
        return c - 'A' + 10; // 10进制的数
    }
    return -1;  // 非法字符
}

static at_ret_t emmc_write_sector(uint32_t lba, uint32_t count, const char *data)
{
    uint32_t len = strlen(data);
    size_t byte_len = len / 0x2;
    if (len % 0x2 != 0 || byte_len > EMMC_SECTOR_SIZE) {
        at_adapter_print("invalid len len= %d byte_len = %d !!\r\n", len, byte_len);
        return AT_RET_SYNTAX_ERROR;
    }
    memset_s(g_emmc_write_buf, EMMC_SECTOR_SIZE, 0, EMMC_SECTOR_SIZE);
    for (uint32_t i = 0; i < byte_len; i++) {
        int high = hex_char_to_int(data[0x2 * i]);
        int low  = hex_char_to_int(data[0x2 * i + 1]);
        if (high == -1 || low == -1) {
            at_adapter_print("invalid char !!\r\n");
            return AT_RET_SYNTAX_ERROR;
        }
        g_emmc_write_buf[i] = (uint8_t)((high << 0x4) | low);
    }
    uint32_t write_count = emmc_raw_write(g_emmc_write_buf, lba, count);
    at_adapter_print("write_count = %d \r\n", write_count);
    return AT_RET_OK;
}

at_ret_t at_emmc_sector_test(const emmc_sector_op_args_t *argc)
{
    uint32_t type = argc->oper_type;
    uint32_t lba = argc->lba;
    uint32_t count = argc->count;
    if (type == 0) {
        at_adapter_print("read sector lba = %d count = %d \r\n", lba, count);
        uint32_t read_sector_count = emmc_raw_read(g_emmc_read_buf, lba, count);
        at_adapter_print("read_sector_count = %d \r\n", read_sector_count);
        for (uint32_t i = 0; i < read_sector_count; i++) {
            at_adapter_print("\n lba = %d content: \r\n", (lba + i));
            for (uint32_t j = 0; j < EMMC_SECTOR_SIZE; j++) {
                at_adapter_print("0x%02x ", g_emmc_read_buf[(i * EMMC_SECTOR_SIZE) + j] & 0xff);
                if ((i + 1) % 0x10 == 0) {
                    at_adapter_print("\n");
                }
            }
        }
    } else if (type == 0x1) {
        if (count != 0x1) {
            count = 0x1;
            at_adapter_print("only support write one sector ! \r\n", lba);
        }
        at_adapter_print("write sector lba = %d count = %d \r\n", lba, count);
        emmc_write_sector(lba, count, argc->write_data);
    } else if (type == 0x2) {
        at_adapter_print("erase sector lba = %d count = %d \r\n", lba, count);
        uint32_t rease_rets = EraseDiskByID(0, lba, count);
        at_adapter_print("erase done rets = %d \r\n", rease_rets);
    } else {
        at_adapter_print("invalid type = %d !!\r\n");
    }
    return AT_RET_OK;
}
at_ret_t at_switch_emmc_freq_level(const emmc_freq_level *args)
{
    if (args->type > 1) {
        at_adapter_print("Error params \r\n");
        return AT_RET_CMD_PARA_ERROR;
    }
    uint32_t clock = (args->type == 0) ? 32000000 : 100000000; // 32000000 100000000
    hi_set_emmc_clock(clock);
    at_adapter_print("switch to %d M.\r\n", (clock / 1000 / 1000)); // 1000 转化单位
    return AT_RET_OK;
}

at_ret_t at_set_emmc_driver_strength(const emmc_driver_strength *argc)
{
    memset_s((void *)g_extend_csd_reg, EMMC_ECSD_SIZE, 0, EMMC_ECSD_SIZE);
    get_emmc_exe_csd((uint8_t *)g_extend_csd_reg);
    uint8_t hs_timing_value = g_extend_csd_reg[EMMC_EMMC_DS_VALUE_OFFSET];
    at_adapter_print("Supported driver strength = 0x%x curr ds = 0x%x \r\n",
        g_extend_csd_reg[EMMC_EMMC_DS_SUPPORT_OFFSET], (hs_timing_value & 0xF0) >> 0x4); // 取高4位
    uint8_t ds = argc->ds;
    if (ds > 0xf) { // 仅查询 不配置
        return AT_RET_OK;
    }
    set_emmc_ecsd_info(EMMC_EMMC_DS_VALUE_OFFSET, (ds << 0x4) | (hs_timing_value & 0x0F));
    memset_s((void *)g_extend_csd_reg, EMMC_ECSD_SIZE, 0, EMMC_ECSD_SIZE);
    get_emmc_exe_csd((uint8_t *)g_extend_csd_reg);
    hs_timing_value = g_extend_csd_reg[EMMC_EMMC_DS_VALUE_OFFSET];
    at_adapter_print("set success curr ds = 0x%x \r\n", (hs_timing_value & 0xF0) >> 4); // 取高4位
    return AT_RET_OK;
}

#endif

at_ret_t at_start_dhrystone(const dhry_core_args_t *args)
{
    unused(args);
    uint32_t irq;

    at_adapter_print("start dhry test.\r\n");
    irq = osal_irq_lock();
    watchdog_turnoff_clk();
    while (1) {
        dhry_test();
    }
    osal_irq_restore(irq);
    at_adapter_print("dhry test finished.\r\n");
    return AT_RET_OK;
}

at_ret_t at_set_pwrkey_cfg(const pwrkey_args_t *args)
{
    uint32_t ret = uapi_pmu_ulp_set_pwrkey_reboot_time(args->time);
    if (ret != 0) {
        at_adapter_print("set fail,ret:0x%x \r\n", ret);
        return AT_RET_CMD_PARA_ERROR;
    }
    return AT_RET_OK;
}

#ifdef SUPPORT_SPORT_HEALTH_TEST
at_ret_t parse_hr(const sports_health_test_file_number *args)
{
    uint32_t dataset_number = args->number;
    uapi_watchdog_kick();
    OsMemInfoPrint(OS_SYS_MEM_ADDR);
    OsMemInfoPrint(OS_EXTEND_MEM_ADDR);
    ppg_algo_main(dataset_number);
    return AT_RET_OK;
}

at_ret_t parse_hr_all(void)
{
    uapi_watchdog_kick();
    OsMemInfoPrint(OS_SYS_MEM_ADDR);
    OsMemInfoPrint(OS_EXTEND_MEM_ADDR);
    ppg_algo_main_all();
    return AT_RET_OK;
}

at_ret_t parse_gesture_all(void)
{
    printf("parse_gesture_all\n");
    uapi_watchdog_kick();
    OsMemInfoPrint(OS_SYS_MEM_ADDR);
    OsMemInfoPrint(OS_EXTEND_MEM_ADDR);
    algorithm_valid_all();
    return AT_RET_OK;
}

at_ret_t parse_gesture(const sports_health_test_file_number *args)
{
    uint32_t dataset_number = args->number;
    uapi_watchdog_kick();
    OsMemInfoPrint(OS_SYS_MEM_ADDR);
    OsMemInfoPrint(OS_EXTEND_MEM_ADDR);
    algorithm_valid_one(dataset_number);
    return AT_RET_OK;
}

at_ret_t parse_gesture_single_type(const sports_health_test_file_number *args)
{
    uint32_t dataset_number = args->number;
    uapi_watchdog_kick();
    OsMemInfoPrint(OS_SYS_MEM_ADDR);
    OsMemInfoPrint(OS_EXTEND_MEM_ADDR);
    algorithm_valid_single_type(dataset_number);
    return AT_RET_OK;
}
#endif

at_ret_t plt_atcmd_xo_cali_ops(const plt_xo_cali_args_t *args)
{
#define XO_TRIM_MAX_VAL 0xFF
    plt_xo_cali_args_t *param = (plt_xo_cali_args_t *)args;
    uint16_t data = nv_ipc_get_xotrim();
    uint16_t step = param->para2;
    if (param->para1 == 1) {
        if (data + step >= XO_TRIM_MAX_VAL) {
            data = XO_TRIM_MAX_VAL;
        } else {
            data += step;
        }
    } else if (param->para1 == 0) {
        if ((int16_t)(data - step) <= 0) {
            data = 0;
        } else {
            data -= step;
        }
    }
    nv_ipc_set_xotrim(data);
    at_adapter_print("xo_ctrim_reg_val:0x%X\r\n", data);
#ifndef SUPPORT_XOTRIM_ON_EFUSE
    nv_key_attr_t attr = { false, false, true, 0 };
    errcode_t ret = uapi_nv_write_with_attr(NV_ID_BTSRV_XOTRIM0_CONFIG, (const uint8_t *)(&data), sizeof(uint16_t),
        &attr, NULL);
    if (ret != ERRCODE_SUCC) {
        at_adapter_print("SAVE NV FAILED 0X%X\r\n");
    }
#endif
    return AT_RET_OK;
}

at_ret_t at_time_get(void)
{
    struct timeval tv = {0};
    gettimeofday(&tv, NULL);

    struct tm lm = {0};
    localtime_r(&tv.tv_sec, &lm);
    at_adapter_print("cur date: %04d-%02d-%02d %02d:%02d:%02d\n", (lm.tm_year + YEAR_BASE), (lm.tm_mon + 1), lm.tm_mday,
                     lm.tm_hour, lm.tm_min, lm.tm_sec);
    at_adapter_print("cur timestamp: %d\n", tv.tv_sec);
    return AT_RET_OK;
}

at_ret_t at_pmu_alarm_set(const pmu_alarm_args_t *args)
{
    at_adapter_print("pmu alarm time is: %ds week_days:%d, hour:%d, min:%d" NEWLINE, args->time,
                     args->week_days, args->hour, args->min);

    errcode_t ret;
    alarm_handle_t handle;
    if (args->time > 0) {
        ret = uapi_alarm_create(&handle, args->time);
    } else {
        alarm_week_time_t alarm_week_time = {
            .hour = args->hour,
            .min = args->min,
        };
        alarm_week_time.week_days.raw = args->week_days;
        ret = uapi_alarm_period_create(&handle, alarm_week_time);
    }
    at_adapter_print("create alarm ret=%d, handle=%d\r\n", ret, handle);
    return ret ? AT_RET_RUN_ERROR : AT_RET_OK;
}

at_ret_t at_pmu_alarm_close(const pmu_alarm_handle_args_t *args)
{
    at_adapter_print("close alarm handle=%d\r\n", args->handle);
    errcode_t ret = uapi_alarm_close(args->handle);
    at_adapter_print("close ret=%d\r\n", ret);
    return ret ? AT_RET_RUN_ERROR : AT_RET_OK;
}

at_ret_t at_pmu_alarm_restart(const pmu_alarm_handle_args_t *args)
{
    at_adapter_print("restart alarm handle=%d\r\n", args->handle);
    errcode_t ret = uapi_alarm_restart(args->handle);
    at_adapter_print("restart ret=%d\r\n", ret);
    return ret ? AT_RET_RUN_ERROR : AT_RET_OK;
}

at_ret_t at_pmu_alarm_delete(const pmu_alarm_handle_args_t *args)
{
    at_adapter_print("delete alarm handle=%d\r\n", args->handle);
    errcode_t ret = uapi_alarm_delete(args->handle);
    at_adapter_print("delete ret=%d\r\n", ret);
    return ret ? AT_RET_RUN_ERROR : AT_RET_OK;
}

at_ret_t at_pmu_alarm_query(void)
{
    alarm_info_t alarm_infos[MAX_ALARM_HANDLE];
    errcode_t ret = uapi_alarm_query(alarm_infos, MAX_ALARM_HANDLE);
    at_adapter_print("query alarm ret=%d\r\n", ret);
    for (int i = 0; i < MAX_ALARM_HANDLE; ++i) {
        at_adapter_print(
            "query alarm handle=%d, status=%d, type=%d, alarm_timestamp=%llu, week_days=%d, hour=%d, min=%d\r\n", i,
            alarm_infos[i].status, alarm_infos[i].type, alarm_infos[i].alarm_timestamp,
            alarm_infos[i].alarm_week_time.week_days, alarm_infos[i].alarm_week_time.hour,
            alarm_infos[i].alarm_week_time.min);
    }
    return ret ? AT_RET_RUN_ERROR : AT_RET_OK;
}

at_ret_t at_pmu_alarm_deinit(void)
{
    uapi_alarm_deinit();
    return AT_RET_OK;
}

#define CLOCKS_MODULE_NPU_CORE HAL_CLOCKS_MODULE_SCU_CORE
#define MODULE_FRQ_16M 16
#define MODULE_FRQ_32M 32
#define MODULE_FRQ_64M 64
#define MODULE_FRQ_100M 100
#define MODULE_FRQ_133M 133
#define MODULE_FRQ_200M 200
#define MODULE_FRQ_266M 266
#define MODULE_FRQ_400M 400

typedef struct {
    uint8_t *module;
    clocks_ccrg_module_t module_convert;
} module_convert_t;

typedef struct {
    uint32_t frq;
    clocks_src_t clk_src;
    uint8_t clk_div;
} clk_convert_t;

module_convert_t module_convert_table[] = {
    {(uint8_t *)"MCU",      HAL_CLOCKS_MODULE_MCU_CORE},
    {(uint8_t *)"NPU",      HAL_CLOCKS_MODULE_NPU_CORE},
    {(uint8_t *)"SDIO",     HAL_CLOCKS_MODULE_SDIOM},
    {(uint8_t *)"NORFLASH", HAL_CLOCKS_MODULE_XIP_SFC},
#ifdef CONFIG_PSRAM_SUPPORT_LPM
    {(uint8_t *)"PSRAM",    HAL_CLOCKS_MODULE_XIP_OPI0},
#endif
    {(uint8_t *)"EMMC",     HAL_CLOCKS_MODULE_EMMC},
    {(uint8_t *)"HIFI",     HAL_CLOCKS_MODULE_HIFI},
    {(uint8_t *)"DPU",      HAL_CLOCKS_MODULE_DPU},
};

clk_convert_t clk_convert_table[] = {
    {MODULE_FRQ_16M, CLOCKS_SRC_XO, 0x2},
    {MODULE_FRQ_32M, CLOCKS_SRC_XO, 0x1},
    {MODULE_FRQ_64M, CLOCKS_SRC_DLL, 0x1},
    {MODULE_FRQ_100M, CLOCKS_SRC_PLL, 0x8},
    {MODULE_FRQ_133M, CLOCKS_SRC_PLL, 0x6},
    {MODULE_FRQ_200M, CLOCKS_SRC_PLL, 0x4},
    {MODULE_FRQ_266M, CLOCKS_SRC_PLL, 0x3},
    {MODULE_FRQ_400M, CLOCKS_SRC_PLL, 0x2},
};

static at_ret_t frq_module_convert(const uint8_t *module, clocks_ccrg_module_t *module_convert)
{
    for (uint32_t i = 0; i < sizeof(module_convert_table) / sizeof(module_convert_table[0]); i++) {
        if (strcmp((char *)(uintptr_t)module, (char *)(uintptr_t)module_convert_table[i].module) == 0) {
            *module_convert = module_convert_table[i].module_convert;
            return AT_RET_OK;
        }
    }
    UNUSED(module_convert);
    return AT_RET_CMD_PARA_ERROR;
}

static at_ret_t frq_clk_convert(const uint32_t frequency, clocks_src_t *clk_src, uint8_t *clk_div)
{
    for (uint32_t i = 0; i < sizeof(clk_convert_table) / sizeof(clk_convert_table[0]); i++) {
        if (frequency == clk_convert_table[i].frq) {
            *clk_src = clk_convert_table[i].clk_src;
            *clk_div = clk_convert_table[i].clk_div;
            return AT_RET_OK;
        }
    }
    return AT_RET_CMD_PARA_ERROR;
}

at_ret_t at_set_module_frq(const module_frq_args_t *args)
{
    PRINT("at_set_module_frq set module: %s, frequency:%d.\r\n", args->module, args->frequency);
    at_ret_t ret = AT_RET_OK;
    clocks_ccrg_module_t module_convert = 0;
    clocks_src_t clk = 0;
    uint8_t clk_div = 0;
    ret = frq_module_convert(args->module, &module_convert);
    if (ret != AT_RET_OK) {
        return ret;
    }
    ret = frq_clk_convert(args->frequency, &clk, &clk_div);
    if (ret != AT_RET_OK) {
        return ret;
    }
    PRINT("at_set_module_frq set module: %d, clk: %d, dive: %d.\r\n", module_convert, clk, clk_div);
    if (module_convert == HAL_CLOCKS_MODULE_XIP_OPI0) {
        clocks_set_psram_freq(clk, clk_div);
    } else {
        clocks_ccrg_set_frequency(module_convert, clk, clk_div);
    }
    if (module_convert == HAL_CLOCKS_MODULE_HIFI) {
        if (args->frequency == MODULE_FRQ_200M) {
            clocks_set_dsp_freq(HIFI_LPM_CLK_LEVEL2);
        } else if (args->frequency == MODULE_FRQ_64M) {
            clocks_set_dsp_freq(HIFI_LPM_CLK_LOW_LEVEL);
        }
    }
    return AT_RET_OK;
}

#ifdef JS_ENABLE
at_ret_t wear_get_trans_channel(void)
{
    msg_center_get_trans_channel();
    return AT_RET_OK;
}

at_ret_t wear_set_trans_channel(const trans_channel_args_t *args)
{
    msg_center_set_trans_channel(args->para1);
    return AT_RET_OK;
}
#endif

#define ADC_DISCARD_DELAY_MS (3)  // 3ms
static bool adc_get_sample_result(uint8_t channel, uint32_t sample_num, uint32_t sample_discard)
{
    PRINT("channel=%d, sample_num=%d, sample_discard=%d\r\n", channel, sample_num, sample_discard);
#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
    if (uapi_adc_auto_scan_is_enabled()) {
        return false;
    }
#endif /* CONFIG_ADC_SUPPORT_AUTO_SCAN */

    for (uint32_t i = 0; i < sample_discard; i++) {
        (void)uapi_adc_manual_sample(channel);
        uapi_tcxo_delay_ms(ADC_DISCARD_DELAY_MS);
    }

    uint16_t *sample_result = malloc(sizeof(uint16_t) * sample_num);
    if (sample_result == NULL) {
        PRINT("malloc failed\r\n");
        return false;
    }

    uint64_t time_start = uapi_tcxo_get_us();
    for (uint32_t i = 0; i < sample_num; i++) {
        sample_result[i] = (uint16_t)uapi_adc_manual_sample(channel);
    }
    uint64_t time_end = uapi_tcxo_get_us();

    uint32_t sample_zero_count = 0;
    uint32_t stick = 0;
    for (uint32_t i = 0; i < sample_num; i++) {
        PRINT("sample_result = %d.\r\n", sample_result[i]);
        if (sample_result[i] == 0) {
            sample_zero_count++;
        }
        stick += sample_result[i];
    }

    free(sample_result);
    sample_num -= sample_zero_count;
    if (sample_num == 0) {
        PRINT("sample_num = 0, sample_zero_count=%d\r\n", sample_zero_count);
        return false;
    }

    stick = (stick / sample_num);
    float voltage = adc_port_stick_transfer_voltage((uint16_t)stick);

    PRINT("tick: %d, sample time: %lldus. \r\n", stick, (uint64_t)((time_end - time_start) / sample_num));
    PRINT("voltage: %.4f.\r\n", voltage);
    return true;
}

at_ret_t at_get_adc_manual(const adc_manual_args_t *args)
{
    uapi_adc_init(ADC_CLOCK_500KHZ);

#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
    uapi_adc_auto_scan_disable();
#endif /* CONFIG_ADC_SUPPORT_AUTO_SCAN */

    uapi_adc_power_en(AFE_SCAN_MODE_MAX_NUM, true);
    errcode_t ret = uapi_adc_open_channel(args->channel);
    if (ret == ERRCODE_SUCC) {
        adc_get_sample_result(args->channel, args->sample_num, args->sample_discard);
    } else {
        PRINT("uapi_adc_open_channel failed=%d\r\n", ret);
    }

    uapi_adc_close_channel(args->channel);
    uapi_adc_power_en(AFE_SCAN_MODE_MAX_NUM, false);

    return AT_RET_OK;
}

#if defined(CONFIG_ADC_SUPPORT_AUTO_SCAN)
static bool g_adc_continue_sample = false;
static bool g_adc_print_buffer = false;
static void at_adc_autosample_callback(uint8_t channel, uint32_t *buffer, uint32_t length, bool *next)
{
    PRINT("[IRQ]channel: %d, buffer:0x%x, len: %d. \r\n", channel, buffer, length);
    uint64_t data = 0;
    for (uint32_t i = 0; i < length; i++) {
        if (g_adc_print_buffer) {
            PRINT("[%d]data: %d\r\n", i, buffer[i]);
        }
        data += buffer[i];
    }
    if (length != 0) {
        data = data / length;
    }
    PRINT("average data: %d voltage: %.4f\r\n", (uint32_t)data, adc_port_stick_transfer_voltage((uint16_t)data));
    *next = g_adc_continue_sample;
    if (!g_adc_continue_sample) {
        uapi_adc_auto_scan_ch_disable(channel);
    }
    return;
}

at_ret_t at_adc_start_auto_sample(const adc_start_auto_args_t *args)
{
    uint8_t channel = (uint8_t)args->channel;
    g_adc_print_buffer = args->print_buffer;
    g_adc_continue_sample = args->continue_sample;
    adc_scan_config_t config = {
        .freq = args->freq,
    };
    PRINT("start adc auto sample , channel=%u, g_continue_sample=%d, freq=%d\r\n", (uint32_t)channel,
          g_adc_continue_sample, args->freq);

    (void)uapi_adc_init(ADC_CLOCK_500KHZ);
    (void)uapi_adc_power_en(AFE_SCAN_MODE_MAX_NUM, true);
    if (uapi_adc_auto_scan_ch_enable(channel, config, at_adc_autosample_callback) != ERRCODE_SUCC) {
        return AT_RET_RUN_ERROR;
    }
    return AT_RET_OK;
}

at_ret_t at_adc_stop_auto_sample(const adc_stop_auto_args_t *args)
{
    uint8_t channel = (uint8_t)args->channel;
    g_adc_continue_sample = false;
    PRINT("stop adc auto sample , channel = %u\r\n", (uint32_t)channel);
    uapi_adc_auto_scan_ch_disable(channel);
    return AT_RET_OK;
}
#endif

#ifdef SUPPORT_XOTRIM_ON_EFUSE
static uint32_t efuse_write_bits(uint16_t start_bit, uint16_t size, const uint8_t *key_data)
{
#define THREE_BITS_OFFSET       3
    uint16_t i;
    uint16_t j;
    uint16_t a;
    uint16_t b;
    uint32_t ret;

    for (i = start_bit, j = 0; i < (start_bit + size); i++, j++) {
        a = j >> THREE_BITS_OFFSET; /* subscript of receive array. */
        b = j & 0x7;                /* bit offset in one byte. */
        if ((key_data[a] & (uint8_t)(1 << b)) != 0) {
            ret = uapi_efuse_write_bit((i >> THREE_BITS_OFFSET), (i & 0x7));
            if (ret != ERRCODE_SUCC && ret != ERRCODE_NOT_SUPPORT) {
                return ret;
            }
        }
    }
    return ERRCODE_SUCC;
}

at_ret_t at_set_xotrim0_val(const xotrim_args_t *args)
{
#define XOTRIM_EFUSE_START_BIT 159
#define XOTRIM_EFUSE_BIT_WIDTH 16
#define XOTRIM_BYTES_LEN        2
#define XOTRIM_DATA_OFFSET      8

    uint8_t data[XOTRIM_BYTES_LEN] = { 0 };
    uint16_t xo_trim0_val = 0;
    at_ret_t ret = AT_RET_RUN_ERROR;
    at_adapter_print("to set XO TRIM eFuse Value is: %#x \r\n", args->value);
    ret = efuse_read_item(EFUSE_XOTRIM0_VAL_ID, (uint8_t *)&data, (uint16_t)sizeof(data));
    if (ret != ERRCODE_SUCC) {
        PRINT("at_set_xotrim0_val efuse read Fail \r\n");
        return AT_RET_RUN_ERROR;
    }
    xo_trim0_val = ((uint16_t)data[0]) | (((uint16_t)data[1]) << XOTRIM_DATA_OFFSET);
    if (xo_trim0_val != 0) {
        at_adapter_print("xotrim0 value has alrady set! val:%#x\r\n", xo_trim0_val);
        return AT_RET_RUN_ERROR;
    }
    ret = efuse_write_bits(XOTRIM_EFUSE_START_BIT, XOTRIM_EFUSE_BIT_WIDTH, (uint8_t *)(&(args->value)));
    if (ret != AT_RET_OK) {
        at_adapter_print("efuse write Fail! \r\n");
        return ret;
    }
    nv_ipc_reset_xotrim();

    return AT_RET_OK;
}

at_ret_t at_get_xotrim0_val(void)
{
    uint16_t data = nv_ipc_get_xotrim();
    at_adapter_print("get XO TRIM eFuse Value:0x%x\r\n", data);

    return AT_RET_OK;
}
#endif

#ifdef SUPPORT_AIVOICE
at_ret_t at_start_aivoice(void)
{
    uint16_t data = nv_ipc_get_xotrim();

    IntelligentVoiceSeriviceStart();

    return AT_RET_OK;
}

at_ret_t at_stop_aivoice(void)
{
    uint16_t data = nv_ipc_get_xotrim();
    IntelligentVoiceSeriviceStop();
    return AT_RET_OK;
}
#endif

at_ret_t at_gatt_cb_init(const gatt_server_uuid_args_t *argc)
{
    int32_t service_uuid = (int32_t)argc->para1;
#ifndef JS_ENABLE
    diag_channel_register_bt_gatt_init(service_uuid);
#else
    diag_channel_register_bt_gatt_init();
#endif
    return AT_RET_OK;
}

at_ret_t at_spp_cb_init(void)
{
#ifndef JS_ENABLE
    diag_channel_register_bt_callback();
#endif
    return AT_RET_OK;
}
#ifdef SUPPORT_POWER_MANAGER
typedef enum {
    AT_PMS_FUNC,
    AT_PMS_STATE,
    AT_PMS_DEFAULT,
    AT_PMS_KEEPON,
    AT_PMS_GET,
} at_pms_config_t;

at_ret_t at_pms_set_info(const pms_set_args_t *args)
{
    at_adapter_print("at_pms_set_info,type:%u,value:%u.\r\n", args->type, args->value);
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    uint32_t ret = 0;
    const screen_context_t *screen_context = power_display_get_screen_context();
    switch (args->type) {
        case AT_PMS_FUNC: /* 0---func on/off */
            ret = display_api->set_auto_timeout_function(args->value);
            break;
        case AT_PMS_STATE: /* 1---lcd on/off */
            if (args->value) {
                ret = display_api->turn_on_screen();
            } else {
                ret = display_api->turn_off_screen();
            }
            break;
        case AT_PMS_DEFAULT: /* 2---screen timeout */
            ret = display_api->set_screen_auto_off_timeout(args->value);
            break;
        case AT_PMS_KEEPON: /* 3---keepon timeout */
            ret = display_api->set_screen_set_keepon_timeout(args->value);
            break;
        case AT_PMS_GET: /* 4---get pms info */
            at_adapter_print("pms state:%u,auto timeout:%u,keepon timeout:%u,aod func:%u,auto func:%u.\n",
                screen_context->current_state, screen_context->screen_off_timeout, screen_context->keep_on_timeout,
                screen_context->enable_aod, screen_context->time_out_enable);
            break;
        default:
            break;
    }
    at_adapter_print("excute:%u.\r\n", ret);
    return AT_RET_OK;
}
#endif

at_ret_t at_pm_mem_set_info(const pm_mem_set_args_t *args)
{
    at_adapter_print("at_pm_mem_set_info,type:%u,value:%u.\r\n", args->type, args->value);
    uapi_pmu_mem_control(args->type, args->value);
    return AT_RET_OK;
}

#ifdef SUPPORT_AUTO_OTA
at_ret_t at_mcu_auto_ota_block(void)
{
    uint8_t *auto_ota_upload_mode = get_auto_ota_upload_mode();
    *auto_ota_upload_mode = 1; // 阻塞等待升级包上传
    at_adapter_print("auto_ota block!\r\n");
    return ERRCODE_SUCC;
}

at_ret_t at_mcu_auto_ota_run(void)
{
    uint8_t *auto_ota_upload_mode = get_auto_ota_upload_mode();
    *auto_ota_upload_mode = 0; // 升级包上传完成后auto_ota继续执行
    at_adapter_print("auto_ota run!\r\n");
    return ERRCODE_SUCC;
}

at_ret_t at_mcu_auto_ota_del_mac(const auto_ota_args_t *argc)
{
    at_adapter_print("para = %s\r\n", argc->para);
    uint8_t addr[ADDR_MAC_LEN] = {0};
    char tmp[ONE_ADDR_LEN + 1] = {0};
    char *p = (char *)argc->para;

    for (uint8_t i = 0; i < ADDR_MAC_LEN; i++) {
        memcpy_s(tmp, ONE_ADDR_LEN + 1, p, ONE_ADDR_LEN);
        tmp[ONE_ADDR_LEN] = '\0';
        addr[i] = strtol(tmp, NULL, 16); // 16进制
        p = p + ONE_ADDR_LEN;
    }

    at_adapter_print("mcu_auto_ota_del_mac, mac = ****%02x%02x%02x%02x\r\n", addr[NUM_3], addr[NUM_2], addr[1],
        addr[0]);
    bool res = auto_ota_dev_del(addr);
    at_adapter_print("auto_ota_dev_del, res = %d\r\n", res);

    return ERRCODE_SUCC;
}

at_ret_t at_mcu_auto_ota_add_mac(const auto_ota_args_t *argc)
{
    at_adapter_print("enter mcu_auto_ota_add_mac\r\n");
    at_adapter_print("para = %s\r\n", argc->para);
    uint8_t addr[ADDR_MAC_LEN] = {0};
    char tmp[ONE_ADDR_LEN + 1] = {0};
    char *p = (char *)argc->para;

    for (uint8_t i = 0; i < ADDR_MAC_LEN; i++) {
        memcpy_s(tmp, ONE_ADDR_LEN + 1, p, ONE_ADDR_LEN);
        tmp[ONE_ADDR_LEN] = '\0';
        addr[i] = strtol(tmp, NULL, 16); // 16进制
        p = p + ONE_ADDR_LEN;
    }

    at_adapter_print("mcu_auto_ota_add_mac, mac = ****%02x%02x%02x%02x\r\n", addr[NUM_3], addr[NUM_2], addr[1],
        addr[0]);
    auto_ota_dev_add(addr);

    return ERRCODE_SUCC;
}

at_ret_t at_mcu_auto_ota_show_mac(void)
{
    at_adapter_print("enter mcu_auto_ota_show_mac\r\n");

    auto_ota_dev_show();

    return ERRCODE_SUCC;
}
#endif

typedef enum {
    TEST_RC32K_SET_PARAM = 0,
    TEST_RC32K_GET_CALI_FREQ = 1,
    TEST_RC32K_TIME = 20,
} test_rc32k_mode_t;

at_ret_t test_rc32k_set_param(test_rc32k_args_t *args)
{
#ifdef CONFIG_ENABLE_RC_CALIBRATION
    if (args->para1 == 0) {
        set_cali_timer_internal(args->para2);
    } else if (args->para1 == 1) {
        hal_set_cali_mode(args->para2);
        at_adapter_print("hal_set_cali_mode=%u\n", args->para2);
    }
#else
    unused(args);
#endif
    return AT_RET_OK;
}

// 时间测试，循环cycle次，每次将时间设置为TEST_START_TIME，然后开始等待wait_time_s秒，并打印本次的误差信息
#define TEST_START_TIME (1765123200)  // 2025-12-08 0:0:0
#define PPM (1000000)
void test_rc32k_wait_time(uint32_t wait_time_s)
{
    at_adapter_print("wait_time_s=%lu\n", wait_time_s);

    // 1.设置时间
    struct timeval set_tv = {0};
    set_tv.tv_sec = TEST_START_TIME;
    set_tv.tv_usec = 0;
    settimeofday(&set_tv, NULL);
    // 2.等待
    uint64_t end = uapi_tcxo_get_us() + wait_time_s * MS_PER_S * US_PER_MS;
    while (uapi_tcxo_get_us() < end) {
        continue;
    }
    // 获取时间
    struct timeval cur_tv = {0};
    gettimeofday(&cur_tv, NULL);

    struct tm lm = {0};
    localtime_r(&cur_tv.tv_sec, &lm);
    osal_printk("date=%04d-%02d-%02d %02d:%02d:%02d.%u\n", (lm.tm_year + YEAR_BASE), (lm.tm_mon + 1), lm.tm_mday,
                lm.tm_hour, lm.tm_min, lm.tm_sec, cur_tv.tv_usec / US_PER_MS);
    osal_printk("sec=%d,us=%d\n", cur_tv.tv_sec, cur_tv.tv_usec);
    int64_t wait_time_us = (int64_t)wait_time_s * US_PER_MS * MS_PER_S;
    int64_t diff_us = wait_time_us - (((int64_t)cur_tv.tv_sec - TEST_START_TIME) * US_PER_MS * MS_PER_S + cur_tv.tv_usec);
    int64_t real_ppm = diff_us * PPM / wait_time_us;
    osal_printk("wait=%llu,diff=%lld,ppm=%lld\n", wait_time_us, diff_us, real_ppm);
}

at_ret_t test_rc32k_time(test_rc32k_args_t *args)
{
    uint32_t wait_time_s = args->para1;
    uint32_t cycle = args->para2;
    at_adapter_print("cycle=%lu\n", wait_time_s, cycle);

    for (uint32_t i = 0; i < cycle; ++i) {
        test_rc32k_wait_time(wait_time_s);
    }
    at_adapter_print("end test\n");
    return AT_RET_OK;
}

at_ret_t at_test_rc32k(test_rc32k_args_t *args)
{
    switch (args->mode) {
        case TEST_RC32K_SET_PARAM:
            // 设置校准参数
            return test_rc32k_set_param(args);
        case TEST_RC32K_GET_CALI_FREQ: {
            uint32_t freq_100 = clocks_calibration_get_freq(0x80);
            at_adapter_print("freq_100= %lu\n", freq_100);
            return AT_RET_OK;
        }
        case TEST_RC32K_TIME:
            return test_rc32k_time(args);
        default:
            return AT_RET_RUN_ERROR;
    }
    return AT_RET_OK;
}

#ifdef LOSCFG_DRIVERS_USB_DEVICE_CLASS_DRIVERS
enum USB_HOST_MODE {
    USB_HOST_MODE_STOP = 0,
    USB_HOST_MODE_UMASS,
    USB_HOST_MODE_NET,
};
static volatile int current_usb_mode = USB_HOST_MODE_STOP;
extern uintptr_t g_usb_mem_addr_start;
extern unsigned long g_usb_mem_size;

static void test_usb_host_deinit(void)
{
    if (current_usb_mode == USB_HOST_MODE_STOP) {
        return;
    }
    uapi_usb_host_deinit();

    if (current_usb_mode == USB_HOST_MODE_UMASS) {
        if (g_usb_mem_addr_start != (uintptr_t)NULL) {
            osal_kfree((void *)g_usb_mem_addr_start);
            g_usb_mem_addr_start = (uintptr_t)NULL;
        }
    }

    if (current_usb_mode == USB_HOST_MODE_NET) {
        if (g_usb_mem_addr_start != (uintptr_t)NULL) {
            free((void *)g_usb_mem_addr_start);
            g_usb_mem_addr_start = (uintptr_t)NULL;
        }
    }

    current_usb_mode = USB_HOST_MODE_STOP;
}

static void test_usb_host_init_ums(void)
{
    if (current_usb_mode == USB_HOST_MODE_UMASS)
        return;

    if (current_usb_mode != USB_HOST_MODE_STOP)
        test_usb_host_deinit();

    g_usb_mem_size = 0x20000; // 128 KB
    g_usb_mem_addr_start = (uintptr_t)osal_kmalloc(g_usb_mem_size, OSAL_GFP_ATOMIC);
    if (g_usb_mem_addr_start == (uintptr_t)NULL) {
        PRINT("******** g_usb_mem_addr_start osal_kmalloc error\r\n");
        return;
    }
    PRINT("******** g_usb_mem_addr_start 0x%x\r\n ********", g_usb_mem_addr_start);
    if (uapi_usb_host_init() != OSAL_SUCCESS) {
        osal_kfree((void *)g_usb_mem_addr_start);
        g_usb_mem_addr_start = (uintptr_t)NULL;
        return;
    }

    current_usb_mode = USB_HOST_MODE_UMASS;
}

static void test_usb_host_init_net(void)
{
    if (current_usb_mode == USB_HOST_MODE_NET)
        return;

    if (current_usb_mode != USB_HOST_MODE_STOP)
        test_usb_host_deinit();

    g_usb_mem_size = 0x40000; // 256 KB
    g_usb_mem_addr_start = (uintptr_t)memalign(0x100, g_usb_mem_size);
    if (g_usb_mem_addr_start == (uintptr_t)NULL) {
        PRINT("******** g_usb_mem_addr_start malloc error\r\n");
        return;
    }
    PRINT("******** g_usb_mem_addr_start 0x%x\r\n ********", g_usb_mem_addr_start);
    if (uapi_usb_host_init() != OSAL_SUCCESS) {
        free((void *)g_usb_mem_addr_start);
        g_usb_mem_addr_start = (uintptr_t)NULL;
        return;
    }
    current_usb_mode = USB_HOST_MODE_NET;
}

at_ret_t at_usb_host_init(const usbhost_args_t *args)
{
    uint32_t init_mode = args->func;
    switch (init_mode) {
        case USB_HOST_MODE_STOP:
            test_usb_host_deinit();
            break;
        case USB_HOST_MODE_UMASS:
            test_usb_host_init_ums();
            break;
        case USB_HOST_MODE_NET:
            test_usb_host_init_net();
            break;
        default:
            break;
    }

    return AT_RET_OK;
}
#endif

#ifdef LOSCFG_DRIVERS_USB_MASS_STORAGE
#include "block.h"
#include "disk.h"
#include "fs/fs.h"
#include "fs/file.h"

#define SD_PATH                      "/usb_disk"
#define USB_DEV_PATH                 "/dev/sdap0"

static int test_usb_disk_read_write(void)
{
    int ret = -1;
    int fd;
    char wbuf[5] = {0x31, 0x32, 0x33, 0x34, 0x00}; // write buf len is 5
    char rbuf[6] = {0};  // read buf len is 6

    ret = mount(USB_DEV_PATH, SD_PATH, "vfat", MS_NOSYNC, NULL);
    if (ret != 0) {
        dprintf("mount fail. err: 0x%x\n", ret);
        return -1;
    }
    fd = open("/usb_disk/1.txt", O_NONBLOCK | O_CREAT | O_RDWR | O_EXCL, 0777); // 0777 permision rwxrwxrwx
    if (fd < 0) {
        dprintf("Open file fail.\n");
        goto out;
    }
    (void)write(fd, wbuf, 5); // write buf len is 5
    sync();
    close(fd);

    fd = open("/usb_disk/1.txt", O_RDONLY, 0777); // 0777 permision rwxrwxrwx
    if (fd < 0) {
        dprintf("Open file for reading fail\n");
        goto out;
    }
    read(fd, rbuf, 6); // read buf len is 6
    close(fd);
    dprintf("0x%x\t0x%x\t0x%x\t0x%x\t0x%x\r\n", rbuf[0], rbuf[1], rbuf[2], rbuf[3], rbuf[4]); // 0 1 2 3 4 are index
out:
    unlink(SD_PATH);
    ret = umount(SD_PATH);
    if (ret != 0) {
        dprintf("umount fail. err: 0x%x\n", ret);
        return -1;
    }
    dprintf("[%s] all done\r\n", __func__);
    return 0;
}

#define FILE_SIZE (1024 * 1024 * 100)
static int test_usb_disk_read_perf_with_fs(int copy_size_in_kilobyte)
{
    int ret = -1;
    int fd;
    char *dst = NULL;
    char *flash = NULL;
    UINT64 begin_time, end_time;
    static int iteration = 0;
    int copy_size = 1024 * copy_size_in_kilobyte;

    dst = osal_pool_mem_alloc_align((void *)g_usb_mem_addr_start, copy_size, 32);  /* 32 byte usb cache line */
    if (dst == NULL) {
        dprintf("malloc dst failed\n");
        return -1;
    }

    ret = mount(USB_DEV_PATH, SD_PATH, "vfat", MS_NOSYNC, NULL);
    if (ret != 0) {
        dprintf("mount fail. err: 0x%x\n", ret);
        goto out;
    }

    fd = open("/usb_disk/data.bin", O_RDONLY, 0777); // 0777 permision rwxrwxrwx
    if (fd < 0) {
        dprintf("Open file for reading fail.\n");
        goto out;
    }

    begin_time = osal_sched_clock();

    iteration = 0;
    for (iteration = 0; iteration < (FILE_SIZE / copy_size); iteration++) {
        ret = read(fd, dst, copy_size);
        if (ret < 0) {
            dprintf("Read error iteration: %d, err: 0x%x\n", iteration, ret);
            goto read_err;
        }
    }
read_err:
    end_time = osal_sched_clock();
    dprintf("read %d Bytes in %d times\r\n", (iteration * copy_size), iteration);
    close(fd);

out:
    if (dst != NULL)
        osal_pool_mem_free((void *)g_usb_mem_addr_start, dst);

    unlink(SD_PATH);
    ret = umount(SD_PATH);
    if (ret != 0) {
        dprintf("umount fail. err: 0x%x\n", ret);
        return -1;
    }
    dprintf("read %d Bytes in %d times\r\n", (iteration * copy_size), iteration);
    dprintf("being_nanosecs:%llu, end_nanosecs:%llu cost:%llu nanosecs\r\n",
            begin_time, end_time, (end_time - begin_time));
    dprintf("[%s] all done\r\n", __func__);

    return 0;
}

#define UMASS_BLOCK_SIZE 512
#define MS_PER_NS 1000000

static int test_usb_disk_read_perf_just_block(int block_count)
{
    char *dst = NULL;
    UINT64 begin_time, end_time;
    static uint32_t iteration = 0;
    static uint64_t sector = 0;
    uint32_t size = block_count * UMASS_BLOCK_SIZE;

    dst = osal_pool_mem_alloc_align((void *)g_usb_mem_addr_start, size, 32); /* 32 byte usb cache line */
    if (dst == NULL) {
        dprintf("[%s] malloc dst failed\r\n", __func__);
        return -1;
    }
    memset_s(dst, size, 0x5A, size);

    iteration = 0;
    sector = 0;
    dprintf("\r\nwrite begin\r\n");
    begin_time = osal_sched_clock();
    for (iteration = 0; iteration < (FILE_SIZE / size); iteration++) {
        if (los_disk_write(0, dst, sector, block_count) == VFS_ERROR) {
            dprintf("[%s] los_disk_write fialed\r\n", __func__);
            goto out;
        }
        sector += block_count;
    }
    end_time = osal_sched_clock();
    dprintf("write %d Bytes in %d times\r\n", FILE_SIZE, iteration);
    dprintf("being_nanosecs:%llu, end_nanosecs:%llu cost:%llu ms\r\n",
            begin_time, end_time, (end_time - begin_time) / MS_PER_NS);

    memset_s(dst, size, 0x00, size);

    dprintf("\r\nread begin\r\n");
    begin_time = osal_sched_clock();
    iteration = 0;
    sector = 0;
    for (iteration = 0; iteration < (FILE_SIZE / size); iteration++) {
        if (los_disk_read(0, dst, sector, block_count) == VFS_ERROR) {
            dprintf("[%s] los_disk_read fialed\r\n", __func__);
            goto out;
        }
        sector += block_count;
    }

    end_time = osal_sched_clock();
    dprintf("read %d Bytes in %d times\r\n", FILE_SIZE, iteration);
    dprintf("being_nanosecs:%llu, end_nanosecs:%llu cost:%llu ms\r\n",
            begin_time, end_time, (end_time - begin_time) / MS_PER_NS);

out:
    osal_pool_mem_free((void *)g_usb_mem_addr_start, dst);
    dprintf("[%s] all done\r\n", __func__);

    return 0;
}

#define FILE_SIZE_WITH_NAND (1024 * 1024 * 40)
#define FILE_NAME_SIZE 32
#define PRINT_DOT_ITERATION 50
static int test_usb_disk_copy_a2b(char *src_name, char *dst_name)
{
    int ret;
    int copy_size = 1024 * 80; // 1024 * 80 Bytes once
    int iteration = 0;
    int all_count = 0;
    int fd_src = -1;
    int fd_dst = -1;
    char *reading_cache = NULL;
    reading_cache = memalign(32, copy_size); /* 32 byte usb cache line */
    if (reading_cache == NULL) {
        dprintf("malloc dst failed\n");
        return -1;
    }

    fd_src = open(src_name, O_RDONLY, 0777); // 0777 permision rwxrwxrwx
    if (fd_src < 0) {
        dprintf("Open file \"%s\" fail.\n", src_name);
        ret = -1;
        goto out;
    }

    fd_dst = open(dst_name,  O_NONBLOCK | O_CREAT | O_RDWR | O_TRUNC, 0777); // 0777 permision rwxrwxrwx
    if (fd_dst < 0) {
        dprintf("Open file \"%s\" fail.\n", dst_name);
        ret = -1;
        goto close_fdsrc;
    }

    all_count = FILE_SIZE_WITH_NAND / copy_size;
    dprintf("testing copying from %s to %s", src_name, dst_name);
    for (iteration = 0; iteration < all_count; iteration++) {
        ret = read(fd_src, reading_cache, copy_size);
        if (ret < 0) {
            dprintf("\nRead %s error iteration: %d, err: 0x%x\n", src_name, iteration, ret);
            goto out;
        }
        ret = write(fd_dst, reading_cache, copy_size);
        if (ret < 0) {
            dprintf("\nWriting %s error iteration: %d, err: 0x%x\n", dst_name, iteration, ret);
            goto out;
        }
        if (iteration % PRINT_DOT_ITERATION == 0) {
            dprintf(".");
        }
    }
    ret = 0;
    sync();
    close(fd_dst);
close_fdsrc:
    close(fd_src);
out:
    free(reading_cache);
    return ret;
}

static int test_usb_disk_write_check(void)
{
    int ret = -1;
    struct timeval tv = {0};
    gettimeofday(&tv, NULL);
    char nand_name[FILE_NAME_SIZE];
    char usb_name[FILE_NAME_SIZE];
    (void)memset_s(nand_name, FILE_NAME_SIZE, 0, FILE_NAME_SIZE);
    ret = sprintf_s(nand_name, FILE_NAME_SIZE, "/user/%d.bin", tv.tv_sec);
    if (ret == -1) {
        dprintf("setting output file name failed\n");
        return -1;
    }
    (void)memset_s(usb_name, FILE_NAME_SIZE, 0, FILE_NAME_SIZE);
    ret = sprintf_s(usb_name, FILE_NAME_SIZE, "/usb_disk/%d.bin", tv.tv_sec);
    if (ret == -1) {
        dprintf("setting output file name failed\n");
        return -1;
    }

    ret = mount(USB_DEV_PATH, SD_PATH, "vfat", MS_NOSYNC, NULL);
    if (ret != 0) {
        dprintf("mount fail. err: 0x%x\n", ret);
        return -1;
    }

    if (test_usb_disk_copy_a2b("/usb_disk/data.bin", nand_name) != 0) {
        ret = -1;
        goto umount_usbdisk;
    }
    dprintf("copy /usb_disk/data.bin to %s done\n", nand_name);

    if (test_usb_disk_copy_a2b(nand_name, usb_name) != 0) {
        ret = -1;
        goto umount_usbdisk;
    }
    dprintf("copy %s to %s done\n", nand_name, usb_name);

umount_usbdisk:
    ret = umount(SD_PATH);
    if (ret != 0) {
        dprintf("umount fail. err: 0x%x\n", ret);
        return -1;
    }

    if (ret == 0) {
        dprintf("[%s] all done\r\n", __func__);
    }
    return ret;
}

at_ret_t at_usb_host_umass(const usbhost_umass_args_t *args)
{
    int ret = -1;
    dprintf("\r\nmethod = %d, block_count=%d\r\n", args->func, args->blk_cnt);

    switch (args->func) {
        case 1:  // testcase 1: read/write
            ret = test_usb_disk_read_write();
            break;
        case 2: // testcase 2: reading perf test with filesystem
            ret = test_usb_disk_read_perf_with_fs(args->blk_cnt);
            break;
        case 3: // testcase 3: reading perf test without filesystem
            ret = test_usb_disk_read_perf_just_block(args->blk_cnt);
            break;
        case 4: // testcase 4: read data.bin and write flash, then re-write to usb as another file
            ret = test_usb_disk_write_check();
            break;
        default:
            break;
    }

    if (ret != 0)
        return AT_RET_SYNTAX_ERROR;

    return AT_RET_OK;
}
#endif

#ifdef LOSCFG_DRIVERS_USB_HOST_NET_CLASS
#include <lwip/netif.h>
#include <lwip/dhcp.h>
#include <lwip/netifapi.h>
#include <lwip/inet.h>
#include <lwip/ip_addr.h>
#include <lwip/etharp.h>
#include <lwip/apps/ping.h>

static void test_usb_host_net_print_all_netifs(void)
{
    struct netif *netif_p = netif_list;
    int i;

    printf("begin print netifs:\n");

    while (netif_p != NULL) {
        printf("interface : %s\n", netif_p->name);

        printf("    MAC: ");
        for (i = 0; i < NETIF_MAX_HWADDR_LEN; i++) {
            printf("%02X", *(netif_p->hwaddr + i));
            if (i < (NETIF_MAX_HWADDR_LEN - 1)) {
                printf(":");
            }
        }
        printf("\n");
        dprintf("    IP: %s\n", ip4addr_ntoa(ip_2_ip4(&netif_p->ip_addr)));
        dprintf("    Netmask: %s\n", ip4addr_ntoa(ip_2_ip4(&netif_p->netmask)));
        dprintf("    Gateway: %s\n", ip4addr_ntoa(ip_2_ip4(&netif_p->gw)));
        dprintf("    flags: 0x%08X\n", netif_p->flags);

        if (netif_p->flags & NETIF_FLAG_LINK_UP) {
            printf("    status: UP\n");
        } else {
            printf("    status: DOWN\n");
        }

        netif_p = netif_p->next;
    }
    netif_p = NULL;
    printf("all done\n");
}

static void test_usb_host_net_set_ip(void)
{
    struct netif *tmp_netif = netif_list;

    while (tmp_netif != NULL) {
        if (strncmp(tmp_netif->name, "ue", strlen("ue")) == 0)
            break;

        tmp_netif = tmp_netif->next;
    }
    if (tmp_netif == NULL) {
        printf("no usb net card \n");
        return;
    }

    ip4_addr_t ipaddr, netmask, gw;
    IP4_ADDR(&ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&netmask, 0, 0, 0, 0);
    IP4_ADDR(&gw, 0, 0, 0, 0);

    netifapi_netif_set_addr(tmp_netif, &ipaddr, &netmask, &gw);
    return;
}

static void test_usb_host_net_ping(void)
{
    char *ip_addr = "0.0.0.0";
    os_start_ping(ip_addr, strlen(ip_addr));
    return;
}

at_ret_t at_usb_host_net(const usbhostnet_args_t *args)
{
    uint32_t test_mode = args->func;
    printf("%s mode: %d\r\n", __func__, test_mode);
    switch (test_mode) {
        case 1: // testcase 2: list netifs
            test_usb_host_net_print_all_netifs();
            break;
        case 2: // testcase 2: set ip
            test_usb_host_net_set_ip();
            break;
        case 3: // testcase 3: ping
            test_usb_host_net_ping();
            break;
        default:
            break;
    }

    return AT_RET_OK;
}
#endif

#if (defined(CONFIG_DFX_SUPPORT_LOG_LIMIT) && (CONFIG_DFX_SUPPORT_LOG_LIMIT == DFX_YES))
#define CORE_ID_OFFSET 13
const char *g_module_name_list[] = {
    "WIFI",     "BT",       "GNSS",     "DSP",      "PF",
    "MEDIA",    "NFC",      "APP",      "GPU",      "GUI",
    "SLP",      "BTH",      "OHOS",     "BGH",      "ALG",
    "EXT",      "BTSTATUS", "BTOTA"
};

const char *g_core_name_list[] = {
    "BT", "DSP", "APP"
};

static void get_limit_info_cb(uint8_t core, const log_limit_t *log_limits, uint32_t log_limit_num)
{
    if (core < array_size(g_core_name_list)) {
        at_uart_printf("core[%s]", g_core_name_list[core]);
    } else {
        at_uart_printf("unsupport core[%d]", core);
    }
    at_uart_printf("log limit info:\r\n"
                   "|module  |enable|group|max   |available|printed|lost  |all_printed|all_lost|\r\n");
    for (uint32_t i = 0; i < log_limit_num; i++) {
        uint16_t module_id = log_limits[i].module_id & ((1 << CORE_ID_OFFSET) - 1);
        if (module_id >= array_size(g_module_name_list)) {
            at_uart_printf("err module_id:\r\n|%8d", module_id);
        } else {
            at_uart_printf("|%8s", g_module_name_list[module_id]);
        }
        at_uart_printf("|%6d|%5d|%6d|%9d|%7d|%6d|%11d|%8d|\r\n",
            log_limits[i].enable, log_limits[i].group,
            log_limits[i].max_count, log_limits[i].available_count, log_limits[i].printed_count,
            log_limits[i].lost_count, log_limits[i].all_printed_count, log_limits[i].all_lost_count);
    }
}
#endif

at_ret_t plt_atcmd_set_log_limit(const void *args)
{
    unused(args);
#if (defined(CONFIG_DFX_SUPPORT_LOG_LIMIT) && (CONFIG_DFX_SUPPORT_LOG_LIMIT == DFX_YES))
    if (args == NULL) {
        uapi_at_report(AT_RESPONSE_ERROR);
        return AT_RET_CMD_PARA_ERROR;
    }
    plt_set_log_limit_args_t *param = (plt_set_log_limit_args_t *)args;
    const char *core = param->para0;
    const char *module = param->para1;
    uint16_t module_id = 0;
    bool enable = param->para2;
    uint8_t core_id = 0xFF;

    if (core == NULL || module == NULL) {
        at_uart_printf("core/module is NULL!\r\n");
        return AT_RET_CMD_PARA_ERROR;
    }

    for (uint32_t i = 0 ; i < array_size(g_core_name_list); i++) {
        if (strcmp(core, g_core_name_list[i]) == 0) {
            core_id = i;
        }
    }
    if (core_id == 0xFF) {
        at_uart_printf("unsupport core[%s]", core);
        return AT_RET_CMD_PARA_ERROR;
    }

    for (uint32_t i = 0 ; i < array_size(g_module_name_list); i++) {
        if (strcmp(module, g_module_name_list[i]) == 0) {
            module_id = (enum OML_LOG_MODULEID_ENUM)i | (core_id << CORE_ID_OFFSET);
            uapi_log_limit_set_mod_enable(core_id, module_id, enable);
            return AT_RET_OK;
        }
    }
    if (strcmp(module, "ALL") == 0) {
        uapi_log_limit_set_enable(core_id, enable);
        return AT_RET_OK;
    }

    at_uart_printf("unknown module[%s]", module);
#endif
    return AT_RET_CMD_PARA_ERROR;
}

at_ret_t plt_atcmd_get_log_limit_info(void)
{
#if (defined(CONFIG_DFX_SUPPORT_LOG_LIMIT) && (CONFIG_DFX_SUPPORT_LOG_LIMIT == DFX_YES))
    uapi_log_limit_get_info(APPS, get_limit_info_cb);
    uapi_log_limit_get_info(BT, get_limit_info_cb);
#endif
    return AT_RET_OK;
}

at_ret_t at_ldo_set_state(const ldo_set_state_args_t *args)
{
    at_adapter_print("ldotype:%d, state:%d\r\n", args->ldotype, args->state);
    return uapi_pmu_ldo_set_state(args->ldotype, args->state) == ERRCODE_SUCC ? AT_RET_OK : AT_RET_RUN_ERROR;
}

#define PSRAM_MS_PER_S 1000.0f
#define PSRAM_MB_PER_B (1024.0f * 1024.0f)
#define FREQ_MHZ 1000000.0f
#define PSRAM_BYTE_UNIT     8
#define PSRAM_FREQ_UNIT     4
#define PSRAM_FREQ_CONV_VAL 256
#define PSRAM_UTIL_RATE_PCT 100.0f
#define TWO_DIGIT_SCALE     100
#define FOUR_DIGIT_SCALE    10000
#define PSRAM_WRITE_DATA_REG                0x520e5160
#define PSRAM_READ_DATA_REG                 0x520e5164
#define PSRAM_TRAFFIC_MONITOR_CONFIG_REG    0x520e5144
#define PSRAM_TRAFFIC_MONITOR_ENABLE_REG    0x520e5140
static uint32_t g_print_timer_interval = 0;
static uint32_t g_psram_monitor_duration = 0;
static uint32_t g_psram_before_duration = 0;
static void psram_print_access_callback(void *args)
{
    unused(args);
    /* 读取寄存器中存储的统计值并换算为 Byte */
    uint32_t psram_write_data_size = readl(PSRAM_WRITE_DATA_REG) * PSRAM_BYTE_UNIT;
    uint32_t psram_read_data_size = readl(PSRAM_READ_DATA_REG) * PSRAM_BYTE_UNIT;

    /* 监测时间单位转换为 s */
    float second = (float)g_psram_monitor_duration / PSRAM_MS_PER_S;

    /* 获取当前 PSRAM 频率 */
    uint32_t freq = clocks_ccrg_get_frequency(HAL_CLOCKS_MODULE_XIP_OPI0);
    uint32_t div = hal_clocks_ccrg_module_get_div(HAL_CLOCKS_MODULE_XIP_OPI0);
    float freq_div = (float)freq / div / FREQ_MHZ;

    /* 计算统计寄存器配置值 */
    uint32_t config = (g_psram_monitor_duration * US_PER_MS) * freq_div / PSRAM_FREQ_CONV_VAL - 1;

    /* 计算带宽 */
    float write_bandwidth = psram_write_data_size/ second;
    float read_bandwidth = psram_read_data_size/ second;
    float total_bandwidth = write_bandwidth + read_bandwidth;
    float max_bandwidth = freq_div * PSRAM_FREQ_UNIT * FREQ_MHZ;

    /* 计算带宽占用率百分比形式 */
    float psram_utilization_rate = total_bandwidth / max_bandwidth * PSRAM_UTIL_RATE_PCT;

    /* 配置单次流量统计时间为 XXs */
    writel(PSRAM_TRAFFIC_MONITOR_CONFIG_REG, config);
    writel(PSRAM_TRAFFIC_MONITOR_ENABLE_REG, 0x1);

    if (g_psram_before_duration == g_psram_monitor_duration) {
        at_adapter_print("[DATA_SIZE] monitor_interval:%dms,\twrite:%d Byte,\t\tread:%d Byte.\r\n",
            g_psram_monitor_duration, psram_write_data_size, psram_read_data_size);
        at_adapter_print("[BANDWIDTH] write bw:%d.%dMB/s,\t\tread bw:%d.%dMB/s,\t\tbw_util:%d.%d pct\t\tmax bw:%dMB/s.\r\n",
            (uint32_t)(write_bandwidth / PSRAM_MB_PER_B),
            (uint32_t)((write_bandwidth / PSRAM_MB_PER_B - (uint32_t)(write_bandwidth / PSRAM_MB_PER_B)) * FOUR_DIGIT_SCALE),
            (uint32_t)(read_bandwidth / PSRAM_MB_PER_B),
            (uint32_t)((read_bandwidth / PSRAM_MB_PER_B - (uint32_t)(read_bandwidth / PSRAM_MB_PER_B)) * FOUR_DIGIT_SCALE),
            (uint32_t)psram_utilization_rate,
            (uint32_t)((psram_utilization_rate - (uint32_t)psram_utilization_rate) * TWO_DIGIT_SCALE),
            (uint32_t)(freq_div * PSRAM_FREQ_UNIT));
    }
    g_psram_before_duration = g_psram_monitor_duration;
}

static osTimerId_t g_psram_status_print_timerId = NULL;
at_ret_t at_psram_print_access_status(const psram_access_state_args_t *args)
{
    at_adapter_print("timer interval:%d, psram monitor duration:%d\r\n", args->interval, args->duration);
    if (args->duration > args->interval) {
        return AT_RET_CMD_PARA_ERROR;
    }

    if (args->interval == 0 || args->duration == 0) {
        if (g_psram_status_print_timerId != NULL) {
            at_adapter_print("psram access status print has closed\r\n");
            osTimerStop(g_psram_status_print_timerId);
            osTimerDelete(g_psram_status_print_timerId);
            g_psram_status_print_timerId = NULL;
            g_print_timer_interval = args->interval;
            return AT_RET_OK;
        } else {
            return AT_RET_CMD_PARA_ERROR;
        }
    }

    if (g_psram_status_print_timerId != NULL) {
        osTimerStop(g_psram_status_print_timerId);
        osTimerDelete(g_psram_status_print_timerId);
    }

    g_psram_status_print_timerId = osTimerExtNew(psram_print_access_callback, osTimerPeriodic, NULL, NULL,
        osTimerRousesAllow, osTimerAlignAllow);
    if (g_psram_status_print_timerId == NULL) {
        at_adapter_print("print timer created Fail\r\n");
        return AT_RET_RUN_ERROR;
    }

    osTimerStart(g_psram_status_print_timerId, args->interval);
    g_print_timer_interval = args->interval;
    g_psram_monitor_duration = args->duration;

    return AT_RET_OK;
}

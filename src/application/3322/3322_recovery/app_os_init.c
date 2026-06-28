/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:  Application Core OS Initialize Interface Header for Standard.
 */

#include "cmsis_os2.h"
#include "pmu_interrupt.h"
#include "log_reg_dump.h"
#include "watchdog.h"
#include "preserve.h"
#include "hal_reboot.h"
#include "pinctrl_porting.h"
#include "partition.h"
#include "partition_resource_id.h"
#include "upg.h"
#include "irmalloc.h"
#include "upg_definitions.h"
#include "upg_porting.h"
#include "non_os_reboot.h"
#include "debug_print.h"
#include "tcxo.h"
#include "cpu_utils.h"
#include "app_os_init.h"
#include "burn_ctrl.h"
#include "data_convertor.h"
#ifdef CFG_DRIVERS_SPINOR
#include "littlefs_init.h"
#endif


#define UPG_FLAG_DATA_LENGTH         0x1000
#define UPG_FLAG_OFFSET_OF_FOTA_DATA (FOTA_FLASH_REGION_LENGTH - UPG_FLAG_DATA_LENGTH)

typedef void (*CreateQueueFunction_t)(void);

#define TASK_PRIORITY_APP           (osPriority_t)(17)
#define TASK_PRIORITY_DFX_MSG       (osPriority_t)(18)

#define BOOT_CTRL_MAGIC_FLAG        0x5A
#define FPGA_BT_BOOTUP_CTRL_REG     0x52000010
#define FPGA_DSP_BOOTUP_CTRL_REG    0x52000014
#define FPGA_BT_WAIT_CRTL_REG       0x570007b8
#define UPG_FULL_CAT1_NAME          "/user/2131_package/Hi2131EV100.fwpkg"
#define UPG_CAT1_CONFIG_NAME        "/user/2131_package/cat1_upgrade_type.bin"

typedef struct {
    osThreadAttr_t attr;
    osThreadFunc_t func;
    uint32_t *task_handle;
    CreateQueueFunction_t create_queue_fn_p;
} app_task_definition_t;

static void app_main(const void *unused);

app_task_definition_t g_app_tasks[] = {
    { { "app", 0, NULL, 0, NULL, (0x2000), TASK_PRIORITY_APP, 0, 0 },
      (osThreadFunc_t)app_main, NULL, NULL },
};

#define M_NUM_TASKS (sizeof(g_app_tasks) / sizeof(app_task_definition_t))

void app_os_init(const void *unused1, unsigned int unused2)
{
    UNUSED(unused1);
    UNUSED(unused2);

    // Create queues before tasks in case they are used during initialisation.
    for (uint8_t i = 0; i < M_NUM_TASKS; i++) {
        if (g_app_tasks[i].create_queue_fn_p) {
            g_app_tasks[i].create_queue_fn_p();
        }
    }
    // Now create tasks
    for (uint8_t i = 0; i < M_NUM_TASKS; i++) {
        // You can't configure a task with a higher priority than the kernel
        // has been compiled to support. RTOS will clip the priority level,
        // which is probably not what you wanted.
        //
        // Similarly, if the watchdog kick is in the idle task and it's
        // intended to detect any other task being stuck, you don't want any
        // other task to have a priority <= the idle task.
        g_app_tasks[i].task_handle = osThreadNew(g_app_tasks[i].func, NULL, &(g_app_tasks[i].attr));
        if (g_app_tasks[i].task_handle == NULL) {
            panic(PANIC_TASK_CREATE_FAILED, i);
        }
    }
}

#ifdef CFG_DRIVERS_MMC
static void emmc_vfs_control(void)
{
    int ret = emmc_drv_context_init();
    PRINT("emmc init [%d]\r\n", ret);
}
#endif

static bool upg_need_upgrade(void)
{
    uint32_t fota_address = 0;
    partition_information_t info;
    errcode_t ret_val = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret_val != ERRCODE_SUCC || info.type != PARTITION_BY_ADDRESS) {
        return false;
    }
    fota_address = info.part_info.addr_info.addr;
    fota_upgrade_flag_area_t upg_flag_info;
    fota_address += UPG_FLAG_OFFSET_OF_FOTA_DATA;
    ret_val = uapi_sfc_read(0, fota_address, (uint8_t *)(&upg_flag_info), sizeof(fota_upgrade_flag_area_t));
    if (ret_val != ERRCODE_SUCC) {
        return false;
    }

    PRINT("[UPG] head_magic = [%#x],  head_end_magic = [%#x],  complete_flag = [%#x]  \r\n",
        upg_flag_info.head_magic, upg_flag_info.head_end_magic, upg_flag_info.complete_flag);

    if (!(upg_flag_info.head_magic == UPG_HEAD_MAGIC &&
          upg_flag_info.head_end_magic == UPG_END_MAGIC &&
          upg_flag_info.complete_flag != 0)) {
        /* 不需要升级直接返回 */
        PRINT("No need to upgrade...\r\n");
        return false;
    }

    return true;
}

static uint32_t handler_burn_error(uint32_t error_no, int rets, bin_info_t *bins, burn_context_t* ctx)
{
    if (error_no == 0x2) {
        PRINT("cat1 burn malloc failed !\r\n");
    } else if (error_no == 0x3) {
        PRINT("parse fwpkg file failed !\r\n");
    } else {
        PRINT("burn failed ret = %d, error no = %d \r\n", rets, error_no);
    }
    if (bins != NULL) {
        free(bins);
    }
    burn_close(ctx);
    return -1;
}
uint32_t parse_cat1_fwpkg_file(bin_info_t *bins, uint32_t* bin_count)
{
#define FWPKG_VERSION_LEN  64
    char version[FWPKG_VERSION_LEN] = {0};
    if (dc_parse_fwpkg(UPG_FULL_CAT1_NAME, bins, BURN_MAX_FILES, bin_count, version, sizeof(version)) != 0) {
        return -1;
    }
    PRINT("Fwpkg version = %s\n", version);
    return 0;
}

uint32_t burn_cat1_fwpkg(void)
{
    uint32_t radio = 0;
    burn_context_t ctx = {0};
    uint32_t bin_count = 0;
    bin_info_t *bins = malloc(sizeof(bin_info_t) * BURN_MAX_FILES);
    if (bins == NULL || parse_cat1_fwpkg_file(bins, &bin_count)) {
        uint32_t burn_err_no = (bins == NULL) ? 0x2 : 0x3;
        return handler_burn_error(burn_err_no, 0, bins, &ctx);
    }
    burn_serial_cfg_t cfg = {0};
    cfg.baud = 2000000; // 2000000 2M baud
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
    rets = burn_handshake(&ctx, &cfg);
    if (rets != BURN_ERR_OK) {
        return handler_burn_error(0x5, rets, bins, &ctx);
    }
    rets = burn_execute(&ctx, bins, bin_count, radio);
    if (rets != BURN_ERR_OK) {
        return handler_burn_error(0x6, rets, bins, &ctx);
    }
    if (bins != NULL) {
        free(bins);
    }
    burn_close(&ctx);
    PRINT("burn success\n");
    return 0;
}

typedef enum cat1_upgrade_type {
    NONE_CAT1_UPGRADE,
    FULL_CAT1_UPGRADE,
    DIFF_CAT1_UPGRADE,
    MAX_CAT1_UPGRADE
} cat1_upgrade_type_t;

static void diff_cat1_upgrade(void)
{
    PRINT("diff cat1 upgrade\r\n");
}

static void full_cat1_upgrade(void)
{
#define UPG_CAT1_START_MAGIC            0x5A5A5A5A
#define UPG_CAT1_FINISH_MAGIC           0xA5A5A5A5
    PRINT("full cat1 upgrade\r\n");
    uint32_t start_addr = 0;
    upg_get_slave_chip_upgrade_flag_flash_start_addr(&start_addr);
    uint32_t flag = UPG_CAT1_START_MAGIC;
    uapi_sfc_write(0, start_addr, &flag, sizeof(uint32_t));
    uint32_t retries_count = 3; // 3 times
    do {
        if (burn_cat1_fwpkg() == 0) {
            if (remove(UPG_FULL_CAT1_NAME) != 0) {
                PRINT("remove error path=%s, ret=%d", UPG_FULL_CAT1_NAME, get_errno());
            }
            if (remove(UPG_CAT1_CONFIG_NAME) != 0) {
                PRINT("remove error path=%s, ret=%d", UPG_CAT1_CONFIG_NAME, get_errno());
            }
            break;
        } else {
            PRINT("cat1 upgrade failed, retry ! left count = %d \r\n", retries_count - 1);
        }
    } while (--retries_count);
    flag = UPG_CAT1_FINISH_MAGIC; // 不关注成功或者失败，只关注流程是否结束
    uapi_sfc_write(0, start_addr, &flag, sizeof(uint32_t));
}

static cat1_upgrade_type_t read_cat1_upgrade_cfg(void)
{
    cat1_upgrade_type_t type = NONE_CAT1_UPGRADE;
    FILE *fp = fopen(UPG_CAT1_CONFIG_NAME, "r");
    if (fp == NULL) {
        PRINT("open fail!path:%s error = %x \r\n", UPG_CAT1_CONFIG_NAME, get_errno());
        return type;
    }
    if (fread((uint8_t*)&type, sizeof(uint32_t), 1, fp) != 1) {
        type = NONE_CAT1_UPGRADE;
    };
    if (fclose(fp) != 0) {
        PRINT("fclose error ret=%d", get_errno());
    }
    return type >= MAX_CAT1_UPGRADE ? NONE_CAT1_UPGRADE : type;
}

static void cat1_upgrade(void)
{
    cat1_upgrade_type_t cat1_upgrade_type = read_cat1_upgrade_cfg();
    // 询问是否升级cat1
    if (cat1_upgrade_type == FULL_CAT1_UPGRADE) {
        full_cat1_upgrade();
    } else if (cat1_upgrade_type == DIFF_CAT1_UPGRADE) {
        diff_cat1_upgrade();
    } else {
        PRINT("cat1 not need upgrade \r\n");
    }
}
static void app_main(const void *unused)
{
    PRINT("recovery main.\r\n");
    UNUSED(unused);
    los_vfs_init();
    uint32_t ret = dpal_driver_base_init();
    if (ret != 0) {
        PRINT("dpal driver base init error"NEWLINE);
        panic(PANIC_ASSERT, __LINE__);
    }

#ifdef CFG_DRIVERS_SPINOR
    littlefs_init();
#endif

#ifdef CFG_DRIVERS_MMC
    emmc_vfs_control();
#if defined(SAVE_EXC_INFO)
    if (emmc_exc_info_write() != ERRCODE_SUCC) {
        PRINT("crash data write emmc failed\r\n");
    }
#endif
#endif
#ifdef CFG_DRIVERS_NANDFLASH
    /* true: boot partition */
    nandflash_filesystem_init(true);
    /* false: non-boot partition */
    nandflash_filesystem_init(false);
#endif
    PRINT("fs init success.\r\n");
    if (upg_need_upgrade()) {
        PRINT("need upgrade\r\n");
        upg_func_t upg_func = {0};
        upg_func.malloc = malloc;
        upg_func.free = free;
        (void)uapi_upg_init(&upg_func);
        errcode_t ret = uapi_upg_start();
        if (ret != ERRCODE_SUCC) {
            PRINT("--------------------------\r\n");
            PRINT("upgrade failed, ret = 0x%x\r\n", ret);
        } else {
            PRINT("--------------------------\r\n");
            PRINT("master chip upgrade success\r\n");
        }
    } else {
        PRINT("--------------------------\r\n");
        PRINT("master chip not need upgrade \r\n");
    }
    cat1_upgrade();
    PRINT("upgrade finished, reset now\r\n");
    upg_reboot();
}

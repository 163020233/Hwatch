/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: main
 * Author: HiSilicon
 * Create: 2021-03-09
 */

#include "boot_cmd_loop.h"
#include "boot_serial.h"
#include "boot_flash.h"
#include "chip_io.h"
#include "pinctrl.h"
#include "boot_delay.h"
#include "watchdog.h"
#include "watchdog_porting.h"
#include "efuse.h"
#include "soc_reg.h"
#include "main.h"
#include "sfc_v2.h"
#include "hal_fapc.h"
#include "hal_gpio.h"
#include "usb_load.h"
#include "irmalloc.h"
#include "f_dfu.h"
#include "boot_watchdog.h"
#include "tcxo.h"
#ifdef CONFIG_LOADERBOOT_SUPPORT_FLASH_CHIP_ERASE
#include "boot_erase.h"
#endif
#ifdef SUPPORT_FMC_NAND
#include "nand/nand_fmc.h"
#endif
#ifdef SUPPORT_EMMC_LOADERBOOT
#include "mmc.h"
#endif
#include "clocks_ccrg.h"
#include "gpio.h"

#define FAPC_TEE_MID (1 << 0)
#define FAPC_REE_MID (1 << 1)
#define BOOT_USB_DFU_EN_VALUE 0x2
#define MAX_NO_DFU_CALLBACK_TIMEOUT (3 * 60000)

__attribute__((section("BOOT_UART"))) uart_param_stru g_uart_prag;

#define SHARE_ROMBOOT_INFO __attribute__((section("romboot_info")))

SHARE_ROMBOOT_INFO bootup_ctrl g_bootup_ctrl;

const sfc_flash_config_t g_sfc_cfg = {
    .read_type = FAST_READ_QUAD_OUTPUT,
    .write_type = PAGE_PROGRAM,
    .mapping_addr = FLASH_START,
    .mapping_size = FLASH_LENGTH,
};

STATIC uint32_t sfc_flash_init(void)
{
    return uapi_sfc_init(SFC_ID_0, (sfc_flash_config_t *)&g_sfc_cfg);
}

STATIC uint32_t sfc_flash_read(uint32_t addr, uint32_t read_size, uint8_t *read_buffer)
{
    return uapi_sfc_read(SFC_ID_0, addr, read_buffer, read_size);
}

STATIC uint32_t sfc_flash_erase(uint32_t addr, uint32_t size)
{
#ifdef CONFIG_LOADERBOOT_SUPPORT_FLASH_CHIP_ERASE
    boot_msg0("Flash erase begin!");

    if (size == FLASH_CHIP_ERASE_SIZE) {
        errcode_t ret;
        ret = uapi_sfc_erase_chip(SFC_ID_0);
        if (ret != ERRCODE_SUCC) {
            boot_msg1("Flash erase fail, ret=", ret);
            return ret;
        }
        boot_msg0("Flash erase ok!");
        return ret;
    }
#endif
    return uapi_sfc_erase(SFC_ID_0, addr - g_sfc_cfg.mapping_addr, size);
}

STATIC void sfc_flash_deinit(void)
{
    uapi_sfc_deinit(SFC_ID_0);
}

#ifdef SUPPORT_FMC_NAND
uint32_t fmc_flash_init(void)
{
    return nand_init();
}

STATIC uint32_t fmc_flash_read(uint32_t addr, uint32_t read_size, uint8_t *read_buffer)
{
    uint32_t ret = 0;
    ret = (uint32_t)fmc_nand_read((void *)read_buffer, (uint64_t)addr, read_size);
    return ret;
}

STATIC uint32_t fmc_flash_write(uint32_t addr, uint32_t size, const uint8_t *write_data)
{
    uint32_t ret = 0;
    ret = (uint32_t)fmc_nand_write((void *)write_data, (uint64_t)addr, size);
    return ret;
}

STATIC uint32_t fmc_flash_erase(uint32_t addr, uint32_t size)
{
    return (uint32_t)fmc_nand_erase((uint64_t)addr, size);
}
#endif

#ifdef SUPPORT_EMMC_LOADERBOOT
uint32_t emmc_flash_init(void)
{
    int ret;
    ret = emmc_drv_context_init();
    if (ret != ERRCODE_SUCC) {
        boot_msg1("emmc init error:[%d]\r\n", ret);
        return ERRCODE_FAIL;
    } else {
        boot_msg0("emmc init success\r\n");
        return ERRCODE_SUCC;
    }
}

STATIC uint32_t emmc_flash_read(uint32_t start_sector, uint32_t read_size, uint8_t *read_buffer)
{
    uint32_t ret = 0;
    uint32_t nsectors = read_size / MMC_SECTOR_SIZE;
    if (read_size % MMC_SECTOR_SIZE != 0) {
        nsectors++;
    }
    ret = emmc_raw_read((char *)read_buffer, start_sector, nsectors);
    if (ret == nsectors) {
        return ERRCODE_SUCC;
    } else {
        return ERRCODE_FAIL;
    }
}

#define MMC_SECTOR_COUNT        256
#define CACHE_BUFFER_LEN        (MMC_SECTOR_COUNT * MMC_SECTOR_SIZE) // 128k
static char g_write_buffer[CACHE_BUFFER_LEN] __attribute__((__aligned__(64))) = {0}; // align 64

static volatile uint32_t g_write_buffer_offset = 0;
static volatile uint64_t g_write_start_sectors = 0;
static volatile uint64_t g_write_sector_counts = 0;

static volatile uint64_t g_write_total_size = 0;
static volatile uint64_t g_aready_write_size = 0;

STATIC uint32_t emmc_flash_write(uint32_t start_sector, uint32_t size, const uint8_t *write_data)
{
    if (g_write_buffer_offset + size > CACHE_BUFFER_LEN) {
        boot_msg2("error package size starts , size, \r\n", start_sector, size);
        return ERRCODE_FAIL;
    }
    if (g_write_buffer_offset == 0) {
        g_write_start_sectors = start_sector;
    }
    memcpy_s((void *)(g_write_buffer + g_write_buffer_offset), size, (void *)write_data, size);
    g_write_buffer_offset += size;
    g_write_sector_counts += (size / MMC_SECTOR_SIZE);
    if (size % MMC_SECTOR_SIZE != 0) {
        g_write_sector_counts++;
    }
    if (g_write_buffer_offset == CACHE_BUFFER_LEN || g_aready_write_size + size >= g_write_total_size) {
        uint32_t written = emmc_raw_write((char *)g_write_buffer, g_write_start_sectors, g_write_sector_counts);
        if (written == g_write_sector_counts) {
            memset_s(g_write_buffer, CACHE_BUFFER_LEN, 0, CACHE_BUFFER_LEN);
            g_aready_write_size += g_write_sector_counts * MMC_SECTOR_SIZE;
            g_write_buffer_offset = 0;
            g_write_sector_counts = 0;
            return ERRCODE_SUCC;
        }
        boot_msg2("write mul sectors fail \r\n", g_write_start_sectors, written);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

STATIC uint32_t emmc_flash_erase(uint32_t start_sector, uint32_t size)
{
    uint32_t nsectors = size / MMC_SECTOR_SIZE;
    if (size % MMC_SECTOR_SIZE != 0) {
        nsectors++;
    }
    g_write_total_size = size;
    g_aready_write_size = 0;
    g_write_buffer_offset = 0;
    g_write_sector_counts = 0;
    if ((uint32_t)EraseDiskByID(0, start_sector, nsectors) == nsectors) {
        memset_s(g_write_buffer, CACHE_BUFFER_LEN, 0, CACHE_BUFFER_LEN);
        boot_msg1("EraseDiskByID success! erased sectors:", g_write_total_size);
        return ERRCODE_SUCC;
    }
    return ERRCODE_FAIL;
}
#endif

STATIC uint32_t flash_init(void)
{
    uint32_t ret = sfc_flash_init();
    boot_msg1("Flash Init ert = ", ret);
    return ret;
}

STATIC uint32_t flash_read(uint32_t addr, uint32_t read_size, uint8_t *read_buffer)
{
    uint32_t ret;
#ifdef SUPPORT_FMC_NAND
    if (NAND_BURN_START <= addr && addr < EMMC_BURN_START) {
        ret = fmc_flash_read(addr - NAND_BURN_START, read_size, read_buffer);
        return ret;
    }
#endif
#ifdef SUPPORT_EMMC_LOADERBOOT
    if (addr >= EMMC_BURN_START) {
        ret = emmc_flash_read(addr - EMMC_BURN_START, read_size, read_buffer);
        return ret;
    }
#endif
    ret = sfc_flash_read(addr, read_size, read_buffer);
    return ret;
}

STATIC uint32_t flash_write(uint32_t addr, uint32_t size, const uint8_t *write_data, bool do_erase)
{
    unused(do_erase);
#ifdef SUPPORT_FMC_NAND
    if ((NAND_BURN_START - FLASH_START) <= addr && addr < (EMMC_BURN_START - FLASH_START)) {
        return fmc_flash_write(addr - (NAND_BURN_START - FLASH_START), size, write_data);
    }
#endif
#ifdef SUPPORT_EMMC_LOADERBOOT
    if (addr >= (EMMC_BURN_START - FLASH_START)) {
        return emmc_flash_write(addr - (EMMC_BURN_START - FLASH_START), size, write_data);
    }
#endif
    if ((addr + size) > FLASH_LENGTH) {
        return 0xFFFFFFFF;
    }
    return memcpy_s((void *)(uintptr_t)(addr + FLASH_START), size, write_data, size);
}

STATIC uint32_t flash_erase(uint32_t addr, uint32_t size)
{
#ifdef SUPPORT_FMC_NAND
    if (NAND_BURN_START <= addr && addr < EMMC_BURN_START) {
        boot_msg0("Erase Nand Flash!");
        return fmc_flash_erase((uint32_t)(uintptr_t)addr - NAND_BURN_START, size);
    }
#endif
#ifdef SUPPORT_EMMC_LOADERBOOT
    if (addr >= EMMC_BURN_START) {
        boot_msg0("Erase Emmc!");
        return emmc_flash_erase((uint32_t)(uintptr_t)addr - EMMC_BURN_START, size);
    }
#endif
    boot_msg0("Erase Norflash!");
    return sfc_flash_erase(addr, size);
}

STATIC void flash_deinit(void)
{
    sfc_flash_deinit();
}

STATIC void boot_flash_init(void)
{
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO6_MODE_REG, HAL_PIO_FUNC_QSPI0);  /* qspi0_d0 */
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO7_MODE_REG, HAL_PIO_FUNC_QSPI0);  /* qspi0_d1 */
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO8_MODE_REG, HAL_PIO_FUNC_QSPI0);  /* qspi0_d2 QSPI mode */
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO9_MODE_REG, HAL_PIO_FUNC_QSPI0);  /* qspi0_d3 QSPI mode */
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO10_MODE_REG, HAL_PIO_FUNC_QSPI0); /* qspi0_clk */
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO11_MODE_REG, HAL_PIO_FUNC_QSPI0); /* qspi0_cs */

    uapi_pin_set_pull(S_EGPIO8, PIN_PULL_UP);
    uapi_pin_set_pull(S_EGPIO9, PIN_PULL_UP);

    flash_cmd_func flash_funcs = { 0 };
    flash_funcs.init = flash_init;
    flash_funcs.read = flash_read;
    flash_funcs.write = flash_write;
    flash_funcs.erase = flash_erase;
    flash_funcs.deinit = flash_deinit;
    boot_regist_flash_cmd(&flash_funcs);

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_FRC_2_REG, LP_CTL_MCU_RB_D_PMU_IOLDO6_EN_MCU_MAN_FRC_OFFSET);

    mdelay(0x5);
    flash_init();
}

STATIC void watchdog_init(void)
{
    watchdog_turnoff_clk();
    uapi_watchdog_init(CHIP_WDT_TIMEOUT_128S);
    uapi_watchdog_enable((wdt_mode_t)CHIP_WDT_MODE_INTERRUPT);
}

STATIC void uart_reset(void)
{
    reg16_clrbit(M_CTL_RB_MTOP3_L_REG, M_CTL_RB_UART_L0_RST_SOFT_N_OFFSET); /* reset uart l0 */
    reg16_setbit(M_CTL_RB_MTOP3_L_REG, M_CTL_RB_UART_L0_RST_SOFT_N_OFFSET); /* dereset uart l0 */
}

STATIC void cat1_power_on(void)
{
    /* cat1_vbat_en */
    uapi_pin_set_mode(S_AGPIO1, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(S_AGPIO1, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(S_AGPIO1, GPIO_LEVEL_HIGH);
}

STATIC void loaderboot_init(void)
{
    delay_init();
    /* uapi_efuse_init */
    uapi_efuse_init();
    uapi_pin_init();
    uapi_gpio_init();
    uart_reset();
    hiburn_uart_init(g_uart_prag, g_bootup_ctrl.uart_bus);
    boot_msg0("Loadboot Uart Init Succ!");
    irmalloc_init_default();
    boot_flash_init();
    watchdog_init();
    cat1_power_on();
}

typedef struct {
    uint32_t region;
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t read_per;
    uint32_t write_per;
} fapc_config_t;

STATIC fapc_config_t g_fapc_config[] = {
    {
        0,
        FLASH_START,
        FLASH_START + FLASH_LENGTH,
        FAPC_TEE_MID,
        FAPC_TEE_MID,
    },
};

STATIC void fapc_config_init(void)
{
    for (uint32_t i = 0; i < sizeof(g_fapc_config) / sizeof(fapc_config_t); i++) {
        hal_fapc_set_region_addr(i, g_fapc_config[i].start_addr, g_fapc_config[i].end_addr);
        hal_fapc_set_region_permission(i, g_fapc_config[i].read_per, g_fapc_config[i].write_per);
        hal_fapc_set_region_mode(i, CRYPTO_FAPC_REGION_MODE_NORMAL);
        hal_fapc_region_enable(i, 1);
    }

    flash_spi_info_t *flash_info = sfc_get_flash_info(g_bootup_ctrl.flash_id);

    hal_fapc_set_cmd_lut0(0, 0, flash_info->erase_cmds[0].cmd); // REE cannot do CHIP_ERASE operation
    hal_fapc_set_cmd_lut0_enable(0, true);

    uint32_t cmd_lut1_index = 0;
    for (uint32_t i = 1; i < flash_info->erase_cmd_num; i++) {
        hal_fapc_set_cmd_lut1(cmd_lut1_index, flash_info->erase_cmds[i].size, flash_info->erase_cmds[i].cmd, true);
        hal_fapc_set_cmd_lut1_enable(cmd_lut1_index, true);
        cmd_lut1_index++;
    }
}

void usb_process(void)
{
#ifdef CONFIG_BOOT_SUPPORT_UART_SWITCH_USB
    usb_phy_init();
    usbc_init();
#endif
    uapi_efuse_init();
    usb_dfu_init();

    boot_wdt_kick();
    set_dfu_trans_flag(1);

    uint32_t start_ms = uapi_tcxo_get_ms();
    uint32_t timeout_ms = start_ms + MAX_NO_DFU_CALLBACK_TIMEOUT;

    while (1) {
        if (boot_usb_controllor_get_status() == BOOT_USB_FINISHED) {
            boot_msg0("================ usb load finish ================\r\n");
            break;
        }
        if (boot_usb_controllor_get_status() == BOOT_USB_DELAY_FINISHED) {
            boot_msg0("================ usb load delay finish ===========\r\n");
            break;
        }
        if (uapi_tcxo_get_ms() > timeout_ms) {
            boot_msg0("usb load timeout.\r\n");
            break;
        }
        if (boot_usb_controllor_get_status() == BOOT_USB_RUNING) {
            boot_usb_controllor_set_status(BOOT_USB_CLR_TIMEOUT);
            timeout_ms = uapi_tcxo_get_ms() + MAX_NO_DFU_CALLBACK_TIMEOUT;
        }
        boot_wdt_kick();
    }
    set_dfu_trans_flag(0);
    boot_wdt_kick();
    usb_dfu_deinit();
    if (boot_usb_controllor_get_status() == BOOT_USB_DELAY_FINISHED) {
        uapi_tcxo_delay_ms(1000 * 60); // 延时 1000 ms * 60 后 板子重启
    }
    boot_reset();
}

STATIC bool usb_boot_io_en(void)
{
#define GPIO1_SW_OUT_ADDR (GPIO1_BASE_ADDR + 0x0) // mgpio channel1, S_MGPIO0 ~ S_MGPIO26.
#define GPIO1_SW_OEN_ADDR (GPIO1_BASE_ADDR + 0x4)
#define S_MGPIO18_IDX 18
#define WAIT_IO_CHECK_TIMEOUT_US (180 << 0x5)

    if (g_bootup_ctrl.efuse.usb_dfu_en != BOOT_USB_DFU_EN_VALUE) {
        return false;
    }

    // fpga
    if (g_bootup_ctrl.efuse.fnpll_en == 0) {
        return true;
    }

    // uart rx pull up
    reg16_setbit(CFG_S_MGPIO_PAD_CFG_S_MGPIO18_CTRL_REG, CFG_S_MGPIO_PAD_CFG_S_MGPIO18_CTRL_PU_OFFSET);
    reg16_clrbit(CFG_S_MGPIO_PAD_CFG_S_MGPIO18_CTRL_REG, CFG_S_MGPIO_PAD_CFG_S_MGPIO18_CTRL_PD_OFFSET);

    // set as gpio
    writew(CFG_S_MGPIO_MODE_CFG_S_MGPIO18_MODE_REG, 0x0);
    reg32_setbit(GPIO1_SW_OEN_ADDR, S_MGPIO18_IDX); /* input mode, soc default */

    uapi_tcxo_delay_us(100UL); // delay 100us

    bool usb_boot_io_en = false;
    uint64_t wait_timeout, curr_tick;
    wait_timeout = uapi_tcxo_get_count() + WAIT_IO_CHECK_TIMEOUT_US;
    do {
        curr_tick = uapi_tcxo_get_count();
        if (reg32_getbit(GPIO1_SW_OUT_ADDR, S_MGPIO18_IDX) == 1) {
            usb_boot_io_en = false;
            break;
        }
        usb_boot_io_en = true;
    } while (curr_tick < wait_timeout);

    // set as uart rx
    writew(CFG_S_MGPIO_MODE_CFG_S_MGPIO18_MODE_REG, 0x1);

    // set as pull none
    reg16_clrbit(CFG_S_MGPIO_PAD_CFG_S_MGPIO18_CTRL_REG, CFG_S_MGPIO_PAD_CFG_S_MGPIO18_CTRL_PU_OFFSET);
    return usb_boot_io_en;
}

STATIC void clock_init(void)
{
    clocks_ccrg_init();
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_MCU_CORE, CLOCKS_SRC_PLL, 0x3); // 266MHz
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_XIP_SFC, CLOCKS_SRC_PLL, 0x8);  // 50MHz for adapt all norflash
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_EMMC, CLOCKS_SRC_PLL, 0x6);     // FMC IO 67MHz
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_SCR_SEC, CLOCKS_SRC_PLL, 0x4);  // 200MHz
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_MCU_PERP_UART, CLOCKS_SRC_PLL, 10);  // 10分频, 80MHz
#ifdef CONFIG_BOOT_SUPPORT_UART_SWITCH_USB
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_USB_BUS, CLOCKS_SRC_PLL, 0x4); //  usbc 200M
#endif
    uint32_t freq = clocks_ccrg_get_frequency(HAL_CLOCKS_MODULE_MCU_PERP_UART);
    uart_port_set_clock_value(UART_BUS_2, freq);
}

/* the entry of C. */
void start_loaderboot(void)
{
    if (g_bootup_ctrl.chip_lifecycle != 0) {
#ifdef JTAG_DEBUG_MODE
        reg16_setbit(TEE_CTL_RB_CFG_DEBUG_INTF_EN_REG, TEE_CTL_RB_SWD_EN_REG_OFFSET);
        reg16_setbit(TEE_CTL_RB_CFG_DEBUG_INTF_EN_REG, TEE_CTL_RB_SWD_EN_REG_LOCK_OFFSET);
#endif
    }
    int_setup();
    clock_init();
    fapc_config_init();

    uart_ctx cmd_ctx = { 0 };
    loaderboot_init();

    if ((g_bootup_ctrl.efuse.usb_dfu_en == BOOT_USB_DFU_EN_VALUE) &&
        ((usb_boot_io_en() == true) || (readw(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_15_REG) == 0x5a5b))) {
#ifdef CONFIG_BOOT_SUPPORT_USB_DFU
        if (readw(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_15_REG) == 0x5a5b) {
            writew(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_15_REG, 0x0);
        }
        usb_process();
#else
        boot_msg0("mismatched version, not config usb dfu !\r\n");
        return;
#endif
    }
    loader_ack(ACK_SUCCESS);
    boot_msg0("================ Load Cmd Loop ================\r\n");
    cmd_loop(&cmd_ctx);
#ifdef CONFIG_BOOT_SUPPORT_UART_SWITCH_USB
    if (g_bootup_ctrl.efuse.usb_dfu_en == BOOT_USB_DFU_EN_VALUE) {
        usb_process();
    }
#endif
}

#ifdef CONFIG_LOADERBOOT_SUPPORT_SET_BUADRATE
uint32_t serial_port_set_baudrate(const uart_param_stru *uart)
{
    hiburn_uart_deinit();
    hiburn_uart_init(*uart, g_bootup_ctrl.uart_bus);
    return ERRCODE_SUCC;
}
#endif
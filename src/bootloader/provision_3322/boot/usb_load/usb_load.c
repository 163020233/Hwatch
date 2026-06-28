/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: usb_load
 * Author: HiSilicon
 * Create: 2021-03-09
 */
#include "usb_load.h"
#include "tcxo.h"
#include "boot_serial.h"
#include "securec.h"
#include "main.h"
#include "chip_io.h"
#include "usb_init.h"
#include "f_dfu.h"
#include "irmalloc.h"
#include "boot_init.h"
#include "efuse_porting.h"
#include "boot_watchdog.h"
#include "boot_flash.h"
#include "boot_delay.h"
#include "boot_reset.h"

#define CHECKSUM_SIZE 2
#define USB_BOOT_DIEID_LEN 8
#define USB_DFU_SERIAL_LEN (USB_BOOT_DIEID_LEN * 4)
#define DIE_ID_LEN 16 /* DIEID has 160 bits, but we just use 128 bits */

#define USB_PHY_BASE_ADDR 0x5B040000
#define USB_CONTROL_BASE_ADDR 0x5B000000

#define USB_CKDT_STS (USB_PHY_BASE_ADDR + 0x314)
#define USB_PHY_RB_D_PLL_LOCK_OFFSET 0
#define WAIT_USB_PLL_LOCK_MS 100
#define USB_PHY_ANA_CFG0 (USB_PHY_BASE_ADDR + 0x0)
#define USB_PHY_ANA_CFG1 (USB_PHY_BASE_ADDR + 0x4)
#define USB_PHY_ANA_CFG2 (USB_PHY_BASE_ADDR + 0x8)
#define USB_PHY_ANA_CFG3 (USB_PHY_BASE_ADDR + 0xC)
#define USB_PHY_ANA_CFG4 (USB_PHY_BASE_ADDR + 0x10)
#define USB_PHY_ANA_CFG5 (USB_PHY_BASE_ADDR + 0x14)
#define USB_PHY_RB_RG_PLL_LOCK_OFFSET 16
#define USB_PHY_ANA_CFG6 (USB_PHY_BASE_ADDR + 0x18)
#define DELAY_FOR_HIBURN_CHANGE_STATUS 250
#define FLASH_ERASE_WHOLE_FLAG 0xFFFFFFFF
#define EFUSE_DIE_ID 0

static struct dfu_file_header_flash_info g_dfu_flash_info;
static struct dfu_file_header_efuse_info g_dfu_efuse_info;
static struct dfu_file_header_kv_info g_dfu_kv_info;
static struct dfu_file_header_command_info g_dfu_command_info;
static struct flash_erase_info g_flash_erase_info;

uintptr_t g_usb_mem_addr_start  = (uintptr_t)NULL;
unsigned long g_usb_mem_size  = 0x2000;
uint8_t g_dfu_bootrom_trans_Flag = 0;
uintptr_t g_loadaddr = LOAD_BOOT_ADDR;
static bool g_dnload_new_file = true;
struct dfu_file_header_provision_info g_provision_info;
static uint8_t g_frame_type = 0;
static uint32_t g_download_len = 0;
#ifdef SUPPORT_EMMC_LOADERBOOT
static uint32_t g_emmc_sector_offset = 0;
#endif
static uint8_t g_usb_status = BOOT_USB_INIT;

char g_usb_serial[USB_DFU_SERIAL_LEN] = { /* LEN 32 */
    '2', 0, '0', 0, '2', 0, '5', 0, '0', 0, '3', 0, '2', 0, '2', 0,
    '2', 0, '0', 0, '2', 0, '5', 0, '0', 0, '3', 0, '2', 0, '2', 0
};

struct device_string g_fdfu_str_serial = {
    .str = g_usb_serial,
    .len = USB_DFU_SERIAL_LEN
};

char g_die_id[USB_DFU_SERIAL_LEN] = {0}; /* LEN 32 */

void boot_usb_controllor_set_status(enum boot_usb_event_e status)
{
    g_usb_status = status;
}

uint8_t boot_usb_controllor_get_status(void)
{
    return g_usb_status;
}

void boot_reset(void)
{
    boot_msg0("\nReset device...\n");
    mdelay(5); /* delay 5ms */
    reset();
    return;
}

static uint8_t usb_val_to_str(uint8_t val)
{
    if (val < 16) { // val should be less than 16
        /* If val is less than 10, the value is converted to the ASCII code of '0'-'9'.
           Otherwise, the value - 10 is converted to the ASCII code of 'A'-'F'. */
        return (val >= 10 ? ((val - 10) + 'A') : (val + '0'));
    } else {
        return '0'; // If the value of val is invalid, the ASCII code '0' is returned.
    }
}

static void usb_get_serial_id(struct device_string *dfu_str_serial)
{
    if (unlikely(dfu_str_serial == NULL)) {
        return;
    }
    uint8_t efuse_die_id[DIE_ID_LEN];
    /* sizeof(efuse_die_id) = 16 */
    efuse_read_item(EFUSE_DIE_ID, (uint8_t *)efuse_die_id, (uint16_t)sizeof(efuse_die_id));

    // just usb 8 byte die id
    for (int i = 0; i < USB_BOOT_DIEID_LEN; i++) {
        /* 4 bytes form a group. Offset 0. Fill in the ASCII code converted from the lower 4 bits of bytes[i]. */
        g_die_id[4 * i] = usb_val_to_str((uint8_t)(efuse_die_id[i] & 0xf));
        /* 4 bytes form a group. Offset 1. Fill with 0. */
        g_die_id[4 * i + 1] = 0;
        /* 4 bytes form a group. Offset 2. Fill in the ASCII code converted from the higher 4 bits of bytes[i]. */
        g_die_id[4 * i + 2] = usb_val_to_str((uint8_t)((efuse_die_id[i] & 0xf0) >> 4));
        /* 4 bytes form a group. Offset 3. Fill with 0. */
        g_die_id[4 * i + 3] = 0;
    }
    dfu_str_serial->str = g_die_id;
    dfu_str_serial->len = sizeof(g_die_id); /* LEN 32 */
}

uint8_t get_provision_usb_dfu_frame_type(void)
{
    return g_frame_type;
}

static uint32_t boot_dfu_check_file_header(uint8_t *buff, uint32_t length)
{
    unused(length);
    uint32_t check_val = 0;
    uint32_t check_len = 0;
    if (g_frame_type == FRAME_TYPE_FLASH) {
        check_len = (uint32_t)(sizeof(struct dfu_file_header_flash_info) - sizeof(g_dfu_flash_info.check_sum));
        check_val = crc16_ccitt(0, buff, check_len);
        if (check_val == g_dfu_flash_info.check_sum) {
            return EXT_ERR_SUCCESS;
        }
        boot_msg1("[USB_DFU]: check flash file header error ", check_val);
    } else if ((g_frame_type == FRAME_TYPE_CMD_RESET) || (g_frame_type == FRAME_TYPE_CMD_EXIT_BURNING)) {
        check_len = sizeof(struct dfu_file_header_command_info) - sizeof(g_dfu_command_info.check_sum);
        check_val = crc16_ccitt(0, buff, check_len);
        if (check_val == g_dfu_command_info.check_sum) {
            return EXT_ERR_SUCCESS;
        }
        boot_msg1("[USB_DFU]: check other file header error ", check_val);
    } else if (g_frame_type == FRAME_TYPE_KV) {
        check_len = sizeof(struct dfu_file_header_kv_info) - sizeof(g_dfu_kv_info.check_sum);
        check_val = crc16_ccitt(0, buff, check_len);
        if (check_val == g_dfu_kv_info.check_sum) {
            return EXT_ERR_SUCCESS;
        }
        boot_msg1("[USB_DFU]: check kv file header error ", check_val);
    } else if (g_frame_type == FRAME_TYPE_EFUSE) {
        check_len = (uint32_t)(sizeof(struct dfu_file_header_efuse_info) - sizeof(g_dfu_efuse_info.check_sum));
        check_val = crc16_ccitt(0, buff, check_len);
        if (check_val == g_dfu_efuse_info.check_sum) {
            return EXT_ERR_SUCCESS;
        }
        boot_msg1("[USB_DFU]: check burn efuse file header error ", check_val);
    }
    return EXT_ERR_FAILURE;
}

uint32_t provision_usb_dfu_flash_file_parse(const uint8_t *buffer, uint32_t length)
{
    uint32_t len = (uint32_t)sizeof(struct dfu_file_header_flash_info);
    if (len > length) {
        return EXT_ERR_FAILURE;
    }
    memset_s(&g_dfu_flash_info, len, 0, len);
    if (memcpy_s(&g_dfu_flash_info, len, (struct dfu_file_header_flash_info *)(buffer), len) != EOK) {
        boot_msg0("[USB_DFU]: dfu_parse_file_header: memcpy_s failed!");
        return EXT_ERR_FAILURE;
    }
    g_frame_type = g_dfu_flash_info.common_info.frame_type;
    g_flash_erase_info.addr = g_dfu_flash_info.file_addr;
    g_flash_erase_info.size = 0;
    boot_msg1("[USB_DFU]: fileAddr=", g_dfu_flash_info.file_addr);
    boot_msg1("[USB_DFU]: erase length=", g_dfu_flash_info.erase_size);
    boot_msg1("[USB_DFU]: file size=", g_dfu_flash_info.file_len);
    return boot_dfu_check_file_header((uint8_t *)buffer, length);
}

uint32_t provision_usb_dfu_other_file_parse(const uint8_t *buffer, uint32_t length)
{
    uint32_t ret = EXT_ERR_FAILURE;
    uint32_t len = (uint32_t)sizeof(struct dfu_file_header_command_info);
    unused(length);
    memset_s(&g_dfu_command_info, len, 0, len);
    if (memcpy_s(&g_dfu_command_info, len, (struct dfu_file_header_command_info *)(buffer), len) != EOK) {
        boot_msg0("[USB_DFU]: provision_usb_dfu_other_file_parse: memcpy_s failed!");
        return ret;
    }
    g_frame_type = g_dfu_command_info.common_info.frame_type;
    switch (g_frame_type) {
        case FRAME_TYPE_CMD_RESET:
        case FRAME_TYPE_CMD_EXIT_BURNING:
            ret = boot_dfu_check_file_header((uint8_t *)buffer, length);
            if (ret == EXT_ERR_SUCCESS) {
                // 多烧写场景下 0x1E命令 param = 1 代表需要延时重启,
                if (g_dfu_command_info.param == 1) {
                    // 延时重启
                    boot_usb_controllor_set_status(BOOT_USB_DELAY_FINISHED);
                } else {
                    boot_usb_controllor_set_status(BOOT_USB_FINISHED);
                }
            }
            boot_msg2("[USB_DFU]: other_file_parse: param,checksum=", g_dfu_command_info.param,
                g_dfu_command_info.check_sum);
            break;
        default:
            boot_msg1("[USB_DFU]: boot_dfu_parse_file_header: error, invalid frameType:", g_frame_type);
    }
    return ret;
}

uint32_t boot_dfu_parse_file_header(const uint8_t *buffer, uint16_t length)
{
    uint32_t ret = -1;
    struct dfu_file_header_common_info *file_info = (struct dfu_file_header_common_info *)(buffer);
    if (unlikely(length < sizeof(struct dfu_file_header_common_info))) {
        return -1;
    }
    boot_msg4("[USB_DFU]: frameType=", file_info->start_flag, file_info->packet_size, file_info->frame_type,
        file_info->frame_type_reverse);

    switch (file_info->frame_type) {
        case FRAME_TYPE_FLASH:
            ret = provision_usb_dfu_flash_file_parse(buffer, length);
            break;
        default:
            ret = provision_usb_dfu_other_file_parse(buffer, length);
            break;
    }
    return ret;
}

static uint32_t provision_usb_write_flash(uint32_t store_addr, const uint8_t *buff, uint32_t length)
{
    boot_wdt_kick();
    if (store_addr >= NAND_BURN_START) {
        flash_cmd_func *flash_ctr = boot_get_flash_funcs();
        // 减去 FLASH_START 和最后一个参数0 都是为了适配公共接口
        return flash_ctr->write(store_addr - FLASH_START, length, buff, 0);
    }
    if (memcpy_s((void *)(uintptr_t)store_addr, length, buff, length) != EOK) {
        boot_msg0("[USB_DFU]: flash write fail");
        return ERRCODE_FAIL;
    }
    boot_wdt_kick();
    return ERRCODE_SUCC;
}

static uint32_t provision_usb_receive_file(const uint8_t *buff, uint32_t length)
{
    uint32_t write_rets = ERRCODE_SUCC; // 最后一包不走g_frame_type == FRAME_TYPE_FLASH 默认为ERRCODE_SUCC
    if (g_frame_type == FRAME_TYPE_FLASH) {
#ifdef SUPPORT_EMMC_LOADERBOOT
        uint32_t offset = g_dfu_flash_info.file_addr >= EMMC_BURN_START ? g_emmc_sector_offset : g_download_len;
#else
        uint32_t offset = g_download_len;
#endif
        write_rets = provision_usb_write_flash(g_dfu_flash_info.file_addr + offset, buff, length);
        if (write_rets == ERRCODE_SUCC) {
            g_download_len += length;
#ifdef SUPPORT_EMMC_LOADERBOOT
            g_emmc_sector_offset += length / MMC_SECTOR_SIZE;
#endif
        }
    }
    if (g_download_len >= g_dfu_flash_info.file_len) {
        boot_msg1("[USB_DFU]: receive size:", g_download_len);
        g_dnload_new_file = true;
    }
    return write_rets;
}

static uint32_t provision_usb_erase_flash(uint32_t flash_offset, uint32_t erase_size)
{
    uint32_t size = erase_size;
    uint32_t ret = ERRCODE_FAIL;
    flash_cmd_func *flash_ctr = boot_get_flash_funcs();
    unused(flash_offset);
    unused(erase_size);
    boot_wdt_kick();
    if (flash_offset < NAND_BURN_START && size == FLASH_ERASE_WHOLE_FLAG) { /* 0xFFFFFFFF indicates erase whole */
        ret = flash_ctr->erase(0, FLASH_MEM_SIZE);
        if (ret != ERRCODE_SUCC) {
            boot_put_errno(ERRCODE_BOOT_FLASH_ERASE_ERR);
            return ERRCODE_FAIL;
        }
        boot_msg0("[USB_DFU]: Erase entire flash success!");
    } else if (size != 0) {
#if defined(SUPPORT_FMC_NAND) || defined(SUPPORT_EMMC_LOADERBOOT)
        SET_FLASH_SIZE(flash_offset);
        SET_ERASE_SIZE(flash_offset, size);
        size = GET_ERASE_SIZE;
#endif
        boot_msg2("[USB_DFU]: erase start !\r\n", flash_offset, size);
        ret = flash_ctr->erase(flash_offset, size);
        if (ret != ERRCODE_SUCC) {
            boot_put_errno(ERRCODE_BOOT_FLASH_ERASE_ERR);
            return ERRCODE_FAIL;
        }
        boot_msg2("[USB_DFU]: erase success !\r\n", flash_offset, size);
    } else { /* 0x0 indicates not erase */
        size = FLASH_ERASE_WHOLE_FLAG; /* change to 0xFFFFFFFF, in order to write flash in loader_serial_ymodem */
    }
    boot_wdt_kick();
    boot_msg0("[USB_DFU]: Ready for download");
    mdelay(DELAY_FOR_HIBURN_CHANGE_STATUS); /* hiburn need delay 250 ms to change status */
    return ret;
}

void set_dfu_trans_flag(uint8_t status)
{
    g_dfu_bootrom_trans_Flag = status;
}

uint32_t usb_dfu_download_callback(const uint8_t *buf, uint32_t len)
{
    if (g_dfu_bootrom_trans_Flag == 0) {
        return ERRCODE_FAIL;
    }
    if (buf == NULL || len == 0) {
        boot_msg0("[USB_DFU]: buf is NULL or len is zero. \r\n");
        return ERRCODE_FAIL;
    }
    boot_usb_controllor_set_status(BOOT_USB_RUNING);
    if (g_dnload_new_file == true) {
        boot_msg0("[USB_DFU]: dnload new file");
        if (boot_dfu_parse_file_header(buf, len) != 0) {
            boot_msg0("[USB_DFU]: dfu check fail");
            return ERRCODE_FAIL;
        }
        g_download_len = 0;
#ifdef SUPPORT_EMMC_LOADERBOOT
        g_emmc_sector_offset = 0;
#endif
        g_dnload_new_file = false;
        if (g_frame_type == FRAME_TYPE_FLASH) {
            return provision_usb_erase_flash(g_dfu_flash_info.file_addr, g_dfu_flash_info.erase_size);
        }
    }
    return provision_usb_receive_file(buf, len);
}

static void usb_phy_init_pre(void)
{
    // 0. usb gpio init
    writew(CFG_S_AGPIO_MODE_CFG_S_AGPIO18_MODE_REG, 0x1); // cfg gpio as usb_vbus
    // 1. 1p8 power on
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_72_REG, LP_CTL_GLB_RB_D_PMU_SW1P8_3_EN_OFFSET);
    uapi_tcxo_delay_us(240UL);
    // 2. 3v3 power on
    reg16_setbits(LP_CTL_MCU_RB_LP_CTL_MCU_DEF_14_REG, LP_CTL_MCU_RB_D_PMU_IOLDO3_EN_DELAY_TIME_OFFSET,
        LP_CTL_MCU_RB_D_PMU_IOLDO3_EN_DELAY_TIME_LEN, 0xF);
    uapi_tcxo_delay_us(100UL);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_FRC_2_REG, LP_CTL_MCU_RB_D_PMU_IOLDO3_EN_MCU_MAN_FRC_OFFSET);
    uapi_tcxo_delay_us(600UL);
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_70_REG, LP_CTL_GLB_RB_D_PMU_LPREF_IPOLY_EN_OFFSET);
    uapi_tcxo_delay_us(400UL);
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_72_REG, LP_CTL_GLB_RB_D_PMU_SW3P3_0_EN_OFFSET);
    uapi_tcxo_delay_us(240UL);
}

void usb_phy_init(void)
{
    usb_phy_init_pre();
    // 3. usbphy ref clock  dcxo_xoclkout2_en
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_DCXO_XOCLKOUT2_EN_MAN_OFFSET);
    uapi_tcxo_delay_us(300UL);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_DCXO_XOCLKOUT2_EN_MAN_SEL_OFFSET);
    uapi_tcxo_delay_us(10UL);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_DCXO_XOCLKOUT2_EN_MAN_OFFSET);
    uapi_tcxo_delay_us(300UL);
    // 4. phy clock en config
    reg16_setbit(HS_CTL_RB_USB_PHY_BUS_DIV_REG, HS_CTL_RB_CFG_HS_USBPHY_BUS_LOAD_DIV_EN_OFFSET);
    uapi_tcxo_delay_us(1);
    reg16_setbit(HS_CTL_RB_USB_PHY_BUS_DIV_REG, HS_CTL_RB_CFG_HS_USBPHY_BUS_DIV_EN_OFFSET);
    uapi_tcxo_delay_us(1);
    reg16_setbit(HS_CTL_RB_USB_PHY_PCLK_EN_REG, HS_CTL_RB_CFG_USB_APB_PCLK_EN_OFFSET);
    uapi_tcxo_delay_us(1);
    reg16_setbit(HS_CTL_RB_USB_PHY_CRG_CTL_REG, HS_CTL_RB_CFG_HS_USBPHY_BUS_CLKEN_OFFSET);       // open bus clk
    uapi_tcxo_delay_us(20); // 20us
    // 5. config 32M ref
    reg32_setbits(USB_PHY_BASE_ADDR + 0x100, 8, 3, 0x4); // bit 8-10 ref_clk->32M
    uapi_tcxo_delay_us(20); // 20us
    reg32_setbit(USB_PHY_BASE_ADDR + 0xc, 31); // bit 31
    uapi_tcxo_delay_us(20); // 20us
    reg32_setbit(USB_PHY_ANA_CFG5, 0); // 配置低功耗PLL不关闭
    uapi_tcxo_delay_us(20); // 0415 eda 波形检视 add 20us
    reg32_setbit(USB_PHY_ANA_CFG5, 1);
    uapi_tcxo_delay_us(20); // 0415 eda 波形检视 add 20us
    // 6. usb phy dereset
    reg16_setbit(HS_CTL_RB_USB_PHY_CRG_CTL_REG, HS_CTL_RB_CFG_HS_SOFT_RST_USBPHY_SYS_COM_N_OFFSET); // dereset sycom
    uapi_tcxo_delay_us(20); // 20us
    reg16_setbit(HS_CTL_RB_USB_PHY_CRG_CTL_REG, HS_CTL_RB_CFG_HS_SOFT_RST_USBPHY_POR_N_OFFSET);  // dereset por
    uapi_tcxo_delay_ms(1);
    // 8. wait usb 480Mhz clk ok
    uint32_t timeout_cnt = 0; // 查询锁定fnpll lock寄存器，两个都读
    while (1) {
        if ((reg32_getbit(USB_CKDT_STS, USB_PHY_RB_D_PLL_LOCK_OFFSET) == 1)) {
            break;
        }
        if (timeout_cnt > WAIT_USB_PLL_LOCK_MS) {
            boot_msg0("usb unlock.");
            break;
        }
        uapi_tcxo_delay_ms(1);
        timeout_cnt++;
    }
    // 9. utmi dereset
    reg16_clrbit(HS_CTL_RB_USB_PHY_CRG_CTL_REG, HS_CTL_RB_CFG_HS_SOFT_RST_USBPHY_UTMI_RST_REQ_OFFSET); // dereset port_rst
}

void usbc_init(void)
{
    // usbc clock and reset
    writew(HS_CTL_RB_USBC_CRG_CTL_REG, 0x1131);
    uapi_tcxo_delay_us(50); // 50us
    writew(HS_CTL_RB_USBC_CRG_CTL_REG, 0x1130);
    uapi_tcxo_delay_ms(1);

    reg32_setbits(USB_CONTROL_BASE_ADDR + 0xC110, 12, 2, 0x2); // set usbc is device mode offset 12 len 2
    uapi_tcxo_delay_us(100); // 100us
}

void usb_dfu_init(void)
{
    const char g_manufacturer[12] = {
        /* LEN 12 */
        'V', 0, 'e', 0, 'n', 0, 'd', 0, 'o', 0, 'r', 0,
    };
    struct device_string str_manufacturer = {
        .str = g_manufacturer,
        .len = 12
    };

    const char g_product[20] = { /* LEN 20 */
        'U', 0, 'S', 0, 'B', 0, 'B', 0, 'U', 0, 'R', 0, 'N', 0,
        'D', 0, 'F', 0, 'U', 0
    };
    struct device_string str_product = {
        .str = g_product,
        .len = 20
    };

    struct device_id dev_id = {
        .vendor_id = 0x3361, // 厂商ID, USB-IF 统一分配, 上海海思专用 0x3361; 终端专用 0x8983 其它部门(华技): 0x12D1
        .product_id = 0x3322, // 各个厂商内部定义, 产品ID
        .release_num = 0x0010
    };
    uint32_t retcode;
    g_usb_mem_addr_start = (uintptr_t)irmalloc(g_usb_mem_size);
    if (g_usb_mem_addr_start == (uintptr_t)NULL) {
        boot_msg0("[USB_DFU]: g_usb_mem_addr_start alloc fail.");
    }
    usb_get_serial_id(&g_fdfu_str_serial);

    usbd_set_device_info(DEV_DFU, &str_manufacturer, &str_product, &g_fdfu_str_serial, dev_id);

    retcode = usb_init(DEVICE, DEV_DFU);
    if (retcode != 0) {
        boot_msg1("[USB_DFU]: usb_init init fail.", retcode);
    }
}

void usb_dfu_deinit(void)
{
    usb_deinit();
}

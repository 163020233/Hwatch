/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: boot Cmd
 *
 * Create: 2023-01-09
 */
#include "boot_def.h"
#include "boot_watchdog.h"
#include "boot_transfer.h"
#include "boot_delay.h"
#include "boot_reset.h"
#include "boot_ymodem.h"
#include "boot_load.h"
#include "boot_crc.h"
#include "securec.h"
#include "boot_cmd_loop.h"
#ifdef CONFIG_LOADERBOOT_SUPPORT_READ_VERSION
#include "boot_init.h"
#endif
#ifdef CONFIG_LOADERBOOT_SUPPORT_EFUSE_BURN
#include "boot_efuse_opt.h"
#endif
#ifdef CONFIG_LOADERBOOT_SUPPORT_FLASH_CHIP_ERASE
#include "boot_flash.h"
#include "boot_erase.h"
#include "sfc_porting.h"
#endif
#ifdef CONFIG_LOADERBOOT_SUPPORT_DISABLE_CMD
#include "efuse_porting.h"
#endif
#ifdef CONFIG_BOOT_SUPPORT_OTA_RECOVERY_IMAGE
#include "upg_definitions.h"
#include "upg_common_porting.h"
#endif

#define CMD_RX_DELAY_MS     100 /* 100ms */
#define US_PER_MS           1000
#define CMD_FRAME_TIMEOUT   2000 /* 2 s */
#define CMD_ABNORMAL_MAX    100
#define CHECKSUM_SIZE       2
#define ACK_LEN             0x0C
#define BIT_U8   8
#define UPG_COMPLETE_FLAG_VALUE  0xFFFFFFFF

typedef enum {
    CMD_RX_STATUS_WAIT_START_0,
    CMD_RX_STATUS_WAIT_START_1,
    CMD_RX_STATUS_WAIT_START_2,
    CMD_RX_STATUS_WAIT_START_3,
    CMD_RX_STATUS_WAIT_SIZE_0,
    CMD_RX_STATUS_WAIT_SIZE_1,
    CMD_RX_STATUS_WAIT_TYPE,
    CMD_RX_STATUS_WAIT_PAD,
    CMD_RX_STATUS_WAIT_DATA,
    CMD_RX_STATUS_WAIT_CS_0,
    CMD_RX_STATUS_WAIT_CS_1,
} cmd_rx_status;

typedef uint32_t(*cmd_func) (const uart_ctx *cmd_ctx);

typedef struct {
    uint8_t cmd_type;
    cmd_func cmdfunc;
} loader_cmd;

const loader_cmd g_loader_cmdtable[] = {
    { CMD_DL_IMAGE,         loader_download_image },
    { CMD_RESET,            loader_reset },
    { CMD_FACTORY_IMAGE,    loader_download_image },
#ifdef CONFIG_LOADERBOOT_SUPPORT_EFUSE_BURN
    { CMD_BURN_EFUSE,       loader_burn_efuse },
    { CMD_READ_EFUSE,       loader_read_efuse },
#endif
#ifdef CONFIG_LOADERBOOT_SUPPORT_UPLOAD_DATA
    { CMD_UL_DATA,          loader_upload_data },
#endif
#ifdef CONFIG_LOADERBOOT_SUPPORT_READ_VERSION
    { CMD_VERSION,          loader_read_ver },
#endif
#ifdef CONFIG_LOADERBOOT_SUPPORT_SET_BUADRATE
    { CMD_SET_BUADRATE,     loader_set_baudrate },
#endif
    { CMD_QUIT_TO_DFU,      loader_quit_to_dfu },
};

uint32_t loader_quit_to_dfu(const uart_ctx *cmd_ctx)
{
    unused(cmd_ctx);
    return CMD_QUIT_TO_DFU;
}

#ifdef CONFIG_LOADERBOOT_SUPPORT_BUFFER_CLEAR
void __attribute__((weak)) loader_clear_controller_buf(void)
{
    return;
}
#endif

uint32_t loader_reset(const uart_ctx *cmd_ctx)
{
    unused(cmd_ctx);
#ifdef CONFIG_LOADERBOOT_SUPPORT_BUFFER_CLEAR
    loader_clear_controller_buf();
#endif
    boot_msg0("\nReset device...\n");
    loader_ack(ACK_SUCCESS);
    mdelay(5);  /* delay 5ms */
    reset();
    return ERRCODE_SUCC;
}

uint32_t loader_download_image(const uart_ctx *cmd_ctx)
{
    uint32_t download_addr = *(uint32_t *)(&cmd_ctx->packet.payload[0]);
    uint32_t file_len = *(uint32_t *)(&cmd_ctx->packet.payload[4]); /* offset 4 is file length */
    uint32_t erase_size = *(uint32_t *)(&cmd_ctx->packet.payload[8]); /* offset 8 is erase size */
    uint8_t burn_efuse = cmd_ctx->packet.payload[12]; /* offset 12 is burn efuse flag */
#if defined(SUPPORT_FMC_NAND) || defined(SUPPORT_EMMC_LOADERBOOT)
    SET_FLASH_SIZE(download_addr);
    SET_ERASE_SIZE(download_addr, erase_size);
    erase_size = GET_ERASE_SIZE;
#endif
    uint32_t flash_size = FLASH_MEM_SIZE;
#ifdef CONFIG_LOADERBOOT_SUPPORT_FLASH_CHIP_ERASE
    uint32_t ret;
    bool write_flash = false;

    ret = loader_erase_all_process(cmd_ctx, &write_flash);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("Chip Erase failure!", ret);
        return ret;
    }

    if (!write_flash) {
        // erase all only, no need to write flash, return.
        return ERRCODE_SUCC;
    }
#endif
    if (file_len == 0 || (erase_size != 0 && erase_size < file_len) || (file_len > flash_size)) {
        boot_msg0("Invalid params");
        serial_puts("download_addr ");
        serial_puthex(download_addr, 1);
        serial_puts(" file_len ");
        serial_puthex(file_len, 1);
        serial_puts(" erase_size ");
        serial_puthex(erase_size, 1);
        serial_puts("\r\n");
        return ERRCODE_FAIL;
    }
    if (cmd_ctx->packet.head.type == CMD_FACTORY_IMAGE) {
        return download_factory_image(download_addr, erase_size, flash_size, burn_efuse);
    }

    return download_image(download_addr, erase_size, flash_size, burn_efuse);
}

#ifdef CONFIG_LOADERBOOT_SUPPORT_UPLOAD_DATA
uint32_t loader_upload_data(const uart_ctx *cmd_ctx)
{
    uint32_t file_len = *(uint32_t *)(&cmd_ctx->packet.payload[0]);
    uint32_t upload_addr = *(uint32_t *)(&cmd_ctx->packet.payload[4]);  /* offset 4 is read addr */

    if (file_len == 0 || file_len > FLASH_MEM_SIZE) {
        boot_msg0("Upload length error");
        return ERRCODE_FAIL;
    }

    if ((upload_addr & 0x3) != 0) {
        boot_msg0("Upload addr error");
        return ERRCODE_FAIL;
    }

    if (upload_addr > (FLASH_ADDR_OFFSET + FLASH_MEM_SIZE) - file_len) {
        boot_msg0("Upload addr exceeds flash capacity");
        return ERRCODE_FAIL;
    }

    return upload_data(upload_addr, file_len);
}
#endif

#ifdef CONFIG_LOADERBOOT_SUPPORT_READ_VERSION
uint32_t loader_read_ver(const uart_ctx *cmd_ctx)
{
    unused(cmd_ctx);
    errno_t ret;
    uint8_t version[VERSION_STR_LEN] = { 0 };
    ret = memcpy_s(version, VERSION_STR_LEN, (const void *)VERSION_STR_ADDR, VERSION_STR_LEN);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    serial_put_buf((const char *)version, VERSION_STR_LEN);
    return ERRCODE_SUCC;
}
#endif

static uint32_t loader_frame_head_rx(uart_ctx *ctx)
{
    uint8_t ch;
    bool reset_flag = false;
    uint16_t rcv = 0;

    packet_data_head *head = &ctx->packet.head;
    uint8_t *payload = (uint8_t *)head;

    while (rcv <= CMD_ABNORMAL_MAX) {
        boot_wdt_kick();
        uint32_t ret = serial_getc_timeout(CMD_FRAME_TIMEOUT * US_PER_MS, &ch); /* read timeout < wdt timeout */
        if (ret != ERRCODE_SUCC) {
            continue;
        }

        rcv++;
        if (reset_flag == true) {
            reset_flag = false;
            head->start_flag = 0;
            ctx->status = CMD_RX_STATUS_WAIT_START_0;
        }
        if (ctx->status <= CMD_RX_STATUS_WAIT_START_3) {
            uint32_t start_flag = UART_PACKET_START_FLAG;
            uint8_t *start_byte = (uint8_t *)&start_flag;
            if (ch == start_byte[ctx->status]) {
                payload[ctx->status] = ch;
                ctx->status++;
                continue;
            } else if (ch == 0xEF) {
                payload[CMD_RX_STATUS_WAIT_START_0] = ch;
                ctx->status = CMD_RX_STATUS_WAIT_START_1;
                continue;
            }
            reset_flag = true;
            continue;
        } else {
            payload[ctx->status] = ch;
            if (ctx->status >= CMD_RX_STATUS_WAIT_START_1 && (head->packet_size > UART_PACKET_PAYLOAD_MAX)) {
                reset_flag = true;
                continue;
            }
            ctx->status++;
            if (ctx->status >= CMD_RX_STATUS_WAIT_DATA) {
                return ERRCODE_SUCC;
            }
        }
    }
    return ERRCODE_FAIL;
}

static uint32_t loader_frame_data_rx(uart_ctx *ctx)
{
    uint8_t ch;
    uint32_t ret;
    ctx->received = 0;

    packet_data_head *head = &ctx->packet.head;
    uint8_t *payload = ctx->packet.payload;

    while (ctx->received < (head->packet_size - sizeof(packet_data_head))) {
        ret = serial_getc_timeout(CMD_RX_DELAY_MS * US_PER_MS, &ch);
        if (ret == ERRCODE_SUCC) {
            payload[ctx->received++] = ch;
            continue;
        }
        return ERRCODE_FAIL;
    }
    ctx->packet.check_sum = (payload[head->packet_size - 9] << 8) | payload[head->packet_size - 10]; /* 8,9,10: sub */
    payload[head->packet_size - 9] = 0;  /* 9: sub 9 */
    payload[head->packet_size - 10] = 0; /* 10: sub 10 */

    if (ctx->received == (head->packet_size - sizeof(packet_data_head))) {
        return ERRCODE_SUCC;
    }

    return ERRCODE_FAIL;
}

#ifdef CONFIG_LOADERBOOT_SUPPORT_SET_BUADRATE
uint32_t loader_set_baudrate(const uart_ctx *cmd_ctx)
{
    uart_param_stru *uart;

    uart = uart_baudrate_rcv(cmd_ctx);
    if (uart == NULL) {
        return ERRCODE_FAIL;
    }
    return serial_port_set_baudrate(uart);
}
#endif

void loader_ack(uint8_t err_code)
{
    uart_ctx ctx = { 0 };
    packet_data_head *head = &ctx.packet.head;

    head->start_flag = UART_PACKET_START_FLAG;
    head->type = CMD_ACK;
    head->pad = (uint8_t)(~(CMD_ACK));
    head->packet_size = ACK_LEN;
    ctx.packet.payload[0] = err_code;
    ctx.packet.payload[1] = ~err_code;
    ctx.packet.check_sum = crc16_ccitt(0, (uint8_t *)&(ctx.packet), head->packet_size - CHECKSUM_SIZE);

    serial_put_buf((const char *)&(ctx.packet), (int)(head->packet_size - CHECKSUM_SIZE));
    serial_put_buf((const char *)&(ctx.packet.check_sum), CHECKSUM_SIZE);
}

static uint32_t loader_read_frame(uart_ctx *ctx)
{
    packet_data_info *packet = &ctx->packet;
    packet_data_head *head = &packet->head;
    uint32_t ret;

    /* Reset receiving status.CNcomment:复位接收状态 */
    ctx->status = CMD_RX_STATUS_WAIT_START_0;
    ctx->received = 0;
    if (memset_s(packet, sizeof(packet_data_info), 0, sizeof(packet_data_info)) != EOK) {
        return ERRCODE_FAIL;
    }

    ret = loader_frame_head_rx(ctx);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    ret = loader_frame_data_rx(ctx);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    uint16_t cs = crc16_ccitt(0, (uint8_t *)packet, head->packet_size - CHECKSUM_SIZE);
    if (cs == packet->check_sum) {
        return ERRCODE_SUCC;
    }

    return ERRCODE_FAIL;
}

static uint32_t loader_exe_cmd(uart_ctx *ctx)
{
    uint32_t i;

    packet_data_info *packet = &ctx->packet;
    packet_data_head *head = &packet->head;
    for (i = 0; i < sizeof(g_loader_cmdtable) / sizeof(g_loader_cmdtable[0]); i++) {
        if (head->type == g_loader_cmdtable[i].cmd_type) {
#ifdef CONFIG_LOADERBOOT_SUPPORT_DISABLE_CMD
            if (boot_cmd_enable(head->type) != ERRCODE_SUCC) {
                return ERRCODE_FAIL;
            }
#endif
            if (g_loader_cmdtable[i].cmdfunc != NULL) {
                return g_loader_cmdtable[i].cmdfunc(ctx);
            }
        }
    }

    boot_msg1("Unsupport CMD:", head->type);
    return ERRCODE_FAIL;
}

#ifdef CONFIG_BOOT_SUPPORT_OTA_RECOVERY_IMAGE
static errcode_t boot_get_flag_value(uint32_t flag_start_addr, uint8_t *package_len, uint8_t *firmware_num)
{
    flash_cmd_func *flash = boot_get_flash_funcs();
    if (flash == NULL) {
        boot_msg0("[BOOT UPG] get flash_funcs failed\n");
        return ERRCODE_FAIL;
    }

    uint32_t flash_offset = flag_start_addr + offsetof(fota_upgrade_flag_area_t, package_length);
    errcode_t ret = flash->read(flash_offset, sizeof(uint32_t), package_len);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("[BOOT UPG] get package_length failed, ret = \n", ret);
        return ret;
    }

    flash_offset = flag_start_addr + offsetof(fota_upgrade_flag_area_t, firmware_num);
    ret = flash->read(flash_offset, sizeof(uint32_t), firmware_num);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("[BOOT UPG] get firmware_num failed, ret = \n", ret);
    }
    return ret;
}

static errcode_t boot_set_flag_value(uint32_t flag_start_addr, uint8_t *package_len, uint8_t *firmware_num)
{
    flash_cmd_func *flash = boot_get_flash_funcs();
    if (flash == NULL) {
        boot_msg0("[BOOT UPG] get flash_funcs failed\n");
        return ERRCODE_FAIL;
    }

    /* 初始化 head_before_offset */
    uint32_t flash_offset = flag_start_addr + offsetof(fota_upgrade_flag_area_t, head_before_offset);
    uint32_t head_before_offset = 0;
    errcode_t ret = flash->write(flash_offset, sizeof(uint32_t), (uint8_t *)&head_before_offset, false);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("[BOOT UPG] set head_before_offset failed, ret = \n", ret);
        return ret;
    }

    /* 回写package_length */
    flash_offset = flag_start_addr + offsetof(fota_upgrade_flag_area_t, package_length);
    ret = flash->write(flash_offset, sizeof(uint32_t), package_len, false);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("[BOOT UPG] set package_len failed, ret = \n", ret);
        return ret;
    }

    /* 写入firmware_num */
    flash_offset = flag_start_addr + offsetof(fota_upgrade_flag_area_t, firmware_num);
    ret = flash->write(flash_offset, sizeof(uint32_t), firmware_num, false);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("[BOOT UPG] set firmware_num failed, ret = \n", ret);
    }

    return ret;
}

static bool boot_need_erase(uint32_t flag_start_addr)
{
    uint32_t start_magic = 0;
    uint32_t end_magic = 0;
    uint32_t complete_flag = 0;

    flash_cmd_func *flash = boot_get_flash_funcs();
    if (flash == NULL) {
        boot_msg0("[BOOT UPG] get flash_funcs failed\n");
        return false;
    }

    uint32_t flash_offset = flag_start_addr + offsetof(fota_upgrade_flag_area_t, head_magic);
    errcode_t ret = flash->read(flash_offset, sizeof(uint32_t), (uint8_t *)&start_magic);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("[BOOT UPG] get head_magic failed, ret = \n", ret);
        return false;
    }

    flash_offset = flag_start_addr + offsetof(fota_upgrade_flag_area_t, head_end_magic);
    ret = flash->read(flash_offset, sizeof(uint32_t), (uint8_t *)&end_magic);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("[BOOT UPG] get head_end_magic failed, ret = \n", ret);
        return false;
    }
    flash_offset = flag_start_addr + offsetof(fota_upgrade_flag_area_t, complete_flag);
    ret = flash->read(flash_offset, sizeof(uint32_t), (uint8_t *)&complete_flag);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("[BOOT UPG] get complete_flag failed, ret = \n", ret);
        return false;
    }

    if (start_magic == UPG_HEAD_MAGIC && end_magic == UPG_END_MAGIC && complete_flag == UPG_COMPLETE_FLAG_VALUE) {
        return true;
    }

    return false;
}

/* 烧写后清除升级标记 */
errcode_t boot_clear_upgrade_flag(void)
{
    flash_cmd_func *flash = boot_get_flash_funcs();
    if (flash == NULL) {
        boot_msg0("[BOOT UPG] get flash_funcs failed\n");
        return ERRCODE_FAIL;
    }

    /* FOTA升级标记区在FOTA分区的最后 */
    uint32_t flag_start_addr = 0;
    errcode_t ret = flash->get_flag_addr(&flag_start_addr);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("[BOOT UPG] get flag addr failed, ret = \n", ret);
        return ret;
    }

    /* 判断升级标记是否需要擦除 */
    if (!boot_need_erase(flag_start_addr)) {
        return ERRCODE_SUCC;
    }

    /* 擦除标记区前，读取package_len 和 firmware_num */
    uint32_t package_len = 0;
    uint32_t firmware_num = 0;
    ret = boot_get_flag_value(flag_start_addr, (uint8_t *)&package_len, (uint8_t *)&firmware_num);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* 擦除标记区 */
    ret = flash->erase(flag_start_addr, FOTA_DATA_FLAG_AREA_LEN);
    if (ret != ERRCODE_SUCC) {
        boot_msg1("[BOOT UPG] erase flag failed, ret = \n", ret);
        return ret;
    }

    return boot_set_flag_value(flag_start_addr, (uint8_t *)&package_len, (uint8_t *)&firmware_num);
}
#endif /* CONFIG_BOOT_SUPPORT_OTA_RECOVERY_IMAGE */

void cmd_loop(uart_ctx *ctx)
{
    uint32_t ret;
    for (;;) {
        ret = loader_read_frame(ctx);
        if (ret != ERRCODE_SUCC) {
            loader_ack(ACK_FAILURE);
            continue;
        }

        ret = loader_exe_cmd(ctx);
        if (ret == CMD_QUIT_TO_DFU) {
            boot_msg0("recv switch dfu");
            return;
        }
        if (ret != ERRCODE_SUCC) {
            loader_ack(ACK_FAILURE);
            continue;
        }
        boot_wdt_kick();
        loader_ack(ACK_SUCCESS);
#ifdef CONFIG_BOOT_SUPPORT_OTA_RECOVERY_IMAGE
        /* 烧写成功后，清除升级标记 */
        boot_clear_upgrade_flag();
#endif
        boot_msg0("Execution Successful");
        boot_msg0("=========================");
    }
}
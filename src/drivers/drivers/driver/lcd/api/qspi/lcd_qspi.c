/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Qspi display bus code encapsulation.
 *
 * Create: 2022-08-05
 */

#include "lcd_qspi.h"
#include "stdio.h"
#include "soc_errno.h"
#include "soc_lcd_api.h"
#include "lcd_bus.h"
#include "lcd_dfx.h"
#include "debug_print.h"
#include "tcxo.h"
#include "qspi_display.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "gpio.h"
#include "platform_core.h"
#include "dma_porting.h"
#include "lcd_common.h"
#include "osal_interrupt.h"
#include "pm_clocks.h"
#include "qspi_display_hal.h"
#include "qspi_display_dfx.h"
#include "cmsis_os2.h"

#define FORMAT_PIXEL_BYTE         4
#define STRIDE_POWER              2
#define QSPI_DR             (QSPI_LCD_BASE_ADDR + 0x30)

static uint32_t g_lcd_qspi_col_byte;
static uint32_t g_lcd_qspi_row;

typedef struct {
    bus_panel_info_t panel_info;
    ext_rect area;
    uint16_t x_start;   /* screen releated, be registered by LCD */
    uint16_t y_start;   /* screen releated, be registered by LCD */
    bool panel_is_init; /* only panel operation related */
    bool panel_is_reg;  /* only panel operation related */
} lcd_qspi_info_t;

static lcd_qspi_info_t g_lcd_qspi_info = {0};

ext_errno lcd_qspi_init(combo_dev_cfg_t *bus_cfg)
{
    uapi_qspi_display_init(bus_cfg->out_format);

    g_lcd_qspi_col_byte = bus_cfg->sync_info.hact_pixels * FORMAT_PIXEL_BYTE;
    g_lcd_qspi_row = bus_cfg->sync_info.vact_lines;
    g_lcd_qspi_info.panel_info.is_cmd_mode           = (bus_cfg->out_mode == OUT_MODE_DSI_CMD) ? true : false;
    g_lcd_qspi_info.panel_info.width                 = bus_cfg->sync_info.hact_pixels;
    g_lcd_qspi_info.panel_info.height                = bus_cfg->sync_info.vact_lines;
    g_lcd_qspi_info.panel_info.sync_info             = bus_cfg->sync_info;
    g_lcd_qspi_info.panel_info.pixel_clk             = bus_cfg->pixel_clk;
    g_lcd_qspi_info.area.x = 0;
    g_lcd_qspi_info.area.y = 0;
    g_lcd_qspi_info.area.width = g_lcd_qspi_info.panel_info.width;
    g_lcd_qspi_info.area.height = g_lcd_qspi_info.panel_info.height;
    g_lcd_qspi_info.panel_is_init = true;

    return EXT_ERR_SUCCESS;
}

ext_errno lcd_qspi_deinit(void)
{
    return uapi_qspi_display_deinit();
}

#ifdef GPU_DMA_QSPI
static uint32_t get_stride_num(uint32_t col_byte)
{
    uint32_t res = 0;
    if (col_byte == 1) {
        return 1;
    }
#ifdef SUPPORT_GPU_GMMU
    uint32_t count = 0;
    uint32_t temp = col_byte;
    const uint32_t shift = 1;
    while (temp > 0) {
        count++;
        temp /= 2; /* 2 for sqrt */
    }
    res = shift << count;
    if ((col_byte < res) && (2 * col_byte > res)) { /* 2 for sqrt */
        return res;
    } else {
        return col_byte;
    }
#else
    res = (col_byte + 64 - 1) & ~(64 - 1); /* Non-GMMU mode: 64-byte alignment */
    return res;
#endif
}

static ext_errno lcd_qspi_enable_by_addr(uint32_t addr, uint32_t ch)
{
    qspi_display_frame_info_t info = {0};

    info.frame_addr = addr;
    info.frame_col_byte = g_lcd_qspi_col_byte;
    info.frame_raw = g_lcd_qspi_row;
    info.col_byte_with_stride = get_stride_num(g_lcd_qspi_col_byte);
    info.channel = ch;

    return uapi_qspi_display_enable(&info);
}
#endif

ext_errno lcd_qspi_enable(void)
{
    ext_errno ret = EXT_ERR_SUCCESS;
#ifdef GPU_DMA_QSPI
#ifdef DISPLAY_RAM1
    ret = lcd_qspi_enable_by_addr(DISPLAY_RAM1, DMA_CHANNEL_0);
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }
#endif

#ifdef DISPLAY_RAM2
    ret = lcd_qspi_enable_by_addr(DISPLAY_RAM2, DMA_CHANNEL_1);
    if (ret != EXT_ERR_SUCCESS) {
        uapi_qspi_display_disable();
    }
#endif
#endif
    return ret;
}

ext_errno lcd_qspi_disable(void)
{
    return uapi_qspi_display_disable();
}

ext_errno lcd_qspi_write_cmd(lcd_cmd_sequ *cmd)
{
    ext_errno ret;
    qspi_cmd_info_t qspi_cmd = {0};

    if (cmd->data_len == 0) {
        return EXT_ERR_LCD_INVALID_DATA_LENGTH;
    }

    /* qspi reg addr is 24bit */
    if (lcd_get_bus_type() == BUS_DISPLAY_SPI) {
        qspi_cmd.reg_addr = (uint32_t)cmd->data[0];
    } else {
        qspi_cmd.reg_addr = (uint32_t)cmd->data[0] << 8; /* 8: reg_addr byte 1 */
    }

    qspi_cmd.data_len = cmd->data_len - 1;
    if (qspi_cmd.data_len > 0) {
        qspi_cmd.data = &(cmd->data[1]);
    }

    ret = uapi_qspi_display_write_cmd(&qspi_cmd);
    if (ret != EXT_ERR_SUCCESS) {
        PRINT("line[%d] qspi cmd send failed, ret=0x%col_byte\r\n", __LINE__, ret);
        return ret;
    }

    if (cmd->delay_ms != 0) {
        uapi_tcxo_delay_ms(cmd->delay_ms);
    }

    return EXT_ERR_SUCCESS;
}

ext_errno lcd_qspi_read_cmd(uint32_t cmd, uint8_t *buff, uint8_t len)
{
    uint32_t read_cmd = cmd << 8; /* 8: reg_addr byte 1 */
    if (lcd_get_bus_type() == BUS_DISPLAY_SPI) {
        read_cmd = cmd;
    }

    return uapi_qspi_display_read_cmd(read_cmd, buff, len);
}

ext_errno lcd_qspi_send_frame(uint32_t addr, qspi_send_frame_cb_t callback)
{
    return uapi_qspi_display_show_on(addr, callback);
}

ext_errno lcd_qspi_register_te_isr(gpio_callback_t callback)
{
#ifdef GPU_DMA_QSPI
    uapi_gpio_set_dir(DISPLAY_TE_GPIO, GPIO_DIRECTION_INPUT);

    if (uapi_gpio_register_isr_func(DISPLAY_TE_GPIO, GPIO_INTERRUPT_RISING_EDGE, callback) == ERRCODE_SUCC) {
        return EXT_ERR_SUCCESS;
    }
    return EXT_ERR_LCD_QSPI_TE_REGISTER_FAILED;
#else
    uapi_unused(callback);
    return EXT_ERR_SUCCESS;
#endif
}

ext_errno lcd_qspi_suspend(void)
{
    ext_errno ret;
    td_u32 wait_count = 0;
    for (; wait_count < 100; wait_count++) { // 100 is 100ms
        if (is_qspi_transfer_done()) {
            break;
        }
        osDelay(1);
    }
    ret = lcd_qspi_deinit();
    if (ret != EXT_ERR_SUCCESS) {
        PRINT("bus_deinit fail, ret = %X\r\n", ret);
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

ext_errno lcd_qspi_resume(void)
{
    ext_errno ret;
    lcd_drv_common *lcd_drv_comm_api = NULL;
    lcd_drv_cfg *lcd_cfg = NULL;
    hal_dma_transfer_base_config_t transfer_config = {0};
    hal_dma_transfer_peri_config_t peripheral_config = {0};

    lcd_drv_comm_api = lcd_drv_get_common_api();
    if (lcd_drv_comm_api == NULL) {
        PRINT("[resume]lcd_drv_comm_api is null.\r\n");
        return EXT_ERR_FAILURE;
    }

    lcd_cfg = lcd_drv_comm_api->get_lcd_config();
    if (lcd_cfg == NULL) {
        PRINT("[resume]lcd_cfg is null.\r\n");
        return EXT_ERR_FAILURE;
    }

    ret = lcd_qspi_init(lcd_cfg->bus_cfg);
    if (ret != EXT_ERR_SUCCESS) {
        PRINT("[resume]bus_init fail, ret = %X\r\n", ret);
        return ret;
    }
#ifdef GPU_DMA_QSPI
    transfer_config.dest = QSPI_DR;
    transfer_config.transfer_num = (uint16_t)(g_lcd_qspi_col_byte / (1 << HAL_DMA_TRANSFER_WIDTH_32));
    transfer_config.src_width = HAL_DMA_TRANSFER_WIDTH_32;
    transfer_config.dest_width = HAL_DMA_TRANSFER_WIDTH_32;
    transfer_config.src_burst_trans_length = HAL_DMA_BURST_TRANSACTION_LENGTH_4;
    transfer_config.dest_burst_trans_length = HAL_DMA_BURST_TRANSACTION_LENGTH_4;
    transfer_config.src_inc = HAL_DMA_ADDRESS_INC_INCREMENT;
    transfer_config.dest_inc = HAL_DMA_ADDRESS_INC_NO_CHANGE;

    peripheral_config.trans_type = HAL_DMA_TRANS_MEMORY_TO_PERIPHERAL_DMA;
    peripheral_config.hs_source = HAL_DMA_HANDSHAKING_QSPI2_1CS_TX;
    peripheral_config.hs_dest = HAL_DMA_HANDSHAKING_QSPI2_1CS_TX;
    peripheral_config.trans_dir = HAL_DMA_TRANSFER_DIR_MEM_TO_PERIPHERAL;

#ifdef DISPLAY_RAM1
    dmac_port_config_periph(DMA_CHANNEL_RESERVED_0, &transfer_config, &peripheral_config);
#endif
#ifdef DISPLAY_RAM2
    dmac_port_config_periph(DMA_CHANNEL_RESERVED_1, &transfer_config, &peripheral_config);
#endif

#else
    uapi_unused(transfer_config);
    uapi_unused(peripheral_config);
#endif
    return EXT_ERR_SUCCESS;
}

ext_errno lcd_qspi_get_panel_timing(const ext_rect *rect, td_bool cmp_output, bus_panel_info_t *info)
{
    uapi_unused(cmp_output);
    sync_info_t *sync_info = &g_lcd_qspi_info.panel_info.sync_info;
    if (!g_lcd_qspi_info.panel_is_init || !g_lcd_qspi_info.panel_is_reg) {
        PRINT("lcd qspi init error\r\n");
        return EXT_ERR_QSPI_NOT_ENABLE;
    }

    ext_rect full_screen_rect = {0, 0, g_lcd_qspi_info.panel_info.width, g_lcd_qspi_info.panel_info.height};
    g_lcd_qspi_info.area = (rect == NULL) ? full_screen_rect : *rect;
    ext_rect align_rect = g_lcd_qspi_info.area;
    sync_info->hact_pixels = (uint16_t)align_rect.width;
    sync_info->vact_lines = (uint16_t)align_rect.height;

    *info = g_lcd_qspi_info.panel_info;
    return EXT_ERR_SUCCESS;
}

void qspi_bus_register_panel_info(uint16_t x_start, uint16_t y_start, uint32_t fps)
{
    g_lcd_qspi_info.x_start = x_start;
    g_lcd_qspi_info.y_start = y_start;
    g_lcd_qspi_info.panel_info.refresh_rate = fps;
    g_lcd_qspi_info.panel_is_reg = true;
}

static ext_errno qspi_bus_update_screen(void)
{
    uint16_t x_start;
    uint16_t y_start;
    uint16_t x_end;
    uint16_t y_end;
    uint32_t addr_col = QSPI_SCREEN_COL_START_ADDR;
    uint32_t addr_row = QSPI_SCREEN_ROW_START_ADDR;

    x_start = (uint16_t)g_lcd_qspi_info.area.x + g_lcd_qspi_info.x_start;
    y_start = (uint16_t)g_lcd_qspi_info.area.y + g_lcd_qspi_info.y_start;
    x_end   = x_start + (uint16_t)g_lcd_qspi_info.area.width - 1;
    y_end   = y_start + (uint16_t)g_lcd_qspi_info.area.height - 1;
    if (lcd_get_bus_type() == BUS_DISPLAY_SPI) {
        addr_col = SPI_SCREEN_COL_START_ADDR;
        addr_row = SPI_SCREEN_ROW_START_ADDR;
    }
    ext_errno ret = uapi_qspi_display_set_partial_cmd(addr_col, x_start, x_end);
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }
    ret = uapi_qspi_display_set_partial_cmd(addr_row, y_start, y_end);
    return ret;
}

ext_errno lcd_qspi_bus_update_panel(panel_option_t opt, td_u32 pixel_clk)
{
    if (!g_lcd_qspi_info.panel_is_init || !g_lcd_qspi_info.panel_is_reg) {
        PRINT("lcd qspi init error\r\n");
        return EXT_ERR_QSPI_NOT_ENABLE;
    }

    g_lcd_qspi_info.panel_info.pixel_clk = pixel_clk;

    switch (opt) {
        case PANEL_UPDATE_SCREEN:
            return qspi_bus_update_screen();
        case PANEL_DISPLAY_REFRESH:
            return qspi_bus_update_screen();
        default:
            return EXT_ERR_LCD_NOT_SUPPORT_FUNC;
    }
}

ext_errno lcd_qspi_proc(lcd_bus_proc_type type, void *para)
{
    uapi_unused(para);
    uapi_unused(type);
    qspi_disp_dfx_info *dfx_info = qspi_disp_dfx_get_info();
    if (dfx_info == TD_NULL) {
        return EXT_ERR_FAILURE;
    }
    PRINT("qspi out fmt is %d, qspi clk div is %d\n", dfx_info->out_fmt, dfx_info->clk_div);
    return EXT_ERR_SUCCESS;
}

#ifdef SUPPORT_GPU_SPI
ext_errno lcd_qspi_frame_done_process(void)
{
    ext_errno ret = (ext_errno)uapi_pin_set_mode(QSPI_CS_GPIO, (pin_mode_t)HAL_PIO_FUNC_QSPI2);
    if (ret != ERRCODE_SUCC) {
        lcd_err("lcd set cs mode err, ret = %#x!\n", ret);
    }
    return ret;
}
#endif

void lcd_register_qspi_bus(void)
{
    lcd_bus_api qspi_bus_api = {0};

    qspi_bus_api.bus_init = lcd_qspi_init;
    qspi_bus_api.bus_deinit = lcd_qspi_deinit;
    qspi_bus_api.bus_enable = lcd_qspi_enable;
    qspi_bus_api.bus_disable = lcd_qspi_disable;
    qspi_bus_api.bus_write_cmd = lcd_qspi_write_cmd;
    qspi_bus_api.bus_read_cmd = lcd_qspi_read_cmd;
    qspi_bus_api.bus_send_frame = lcd_qspi_send_frame;
    qspi_bus_api.bus_suspend = lcd_qspi_suspend;
    qspi_bus_api.bus_resume = lcd_qspi_resume;
    qspi_bus_api.bus_register_te_isr = lcd_qspi_register_te_isr;
    qspi_bus_api.bus_get_panel_timing = lcd_qspi_get_panel_timing;
    qspi_bus_api.bus_register_panel_info = qspi_bus_register_panel_info;
    qspi_bus_api.bus_update_panel = lcd_qspi_bus_update_panel;
    qspi_bus_api.bus_proc = lcd_qspi_proc,
#ifdef SUPPORT_GPU_SPI
    qspi_bus_api.bus_frame_done = lcd_qspi_frame_done_process;
#endif
    lcd_bus_register_api(&qspi_bus_api, BUS_DISPLAY_QSPI);
}

void lcd_register_mipi_bus(void)
{
}

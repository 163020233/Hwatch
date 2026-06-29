/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Header file of display with qspi.
 *
 * Create: 2022-09-05
 */

#ifndef __QSPI_DISPLAY_H__
#define __QSPI_DISPLAY_H__

#include "soc_errno.h"
#include "core.h"
#include "errcode.h"
#include "pinctrl_porting.h"
#include "soc_lcd_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t reg_addr;
    uint32_t data_len;
    uint8_t  *data;
} qspi_cmd_info_t;

typedef struct {
    uint32_t frame_addr;            /* memory address saving frame data */
    uint32_t frame_col_byte;        /* column byte equal to column_num * color_mode_bytes, range[1, 4095] */
    uint32_t frame_raw;
    uint32_t col_byte_with_stride;  /* range[1, 4095] */
    uint32_t channel;
} qspi_display_frame_info_t;

#define QSPI_DISPLAY_MCU_INT_TO_TE_TIME 1200 // 1.2ms
#define QSPI_DISPLAY_US_PRE_SECOND  1000000
#define QSPI_SCREEN_COL_START_ADDR  0x2A00
#define QSPI_SCREEN_ROW_START_ADDR  0x2B00
#define SPI_SCREEN_COL_START_ADDR   0x2A
#define SPI_SCREEN_ROW_START_ADDR   0x2B

#ifndef GPU_1A22
#define QSPI_LCD_BASE_ADDR          0x24000000
#define QSPI_CS_GPIO                S_EGPIO5
#else
#define QSPI_LCD_BASE_ADDR          0x52029000
#endif
#define DPU_QSPI_DR_ADDR            (QSPI_LCD_BASE_ADDR + 0x30)
#define SPI_DCX_GPIO                S_AGPIO25

ext_errno uapi_qspi_display_init(out_format_t out_fmt);
ext_errno uapi_qspi_display_deinit(void);
ext_errno uapi_qspi_display_enable(qspi_display_frame_info_t *frame_info);
ext_errno uapi_qspi_display_disable(void);
ext_errno uapi_qspi_display_write_cmd(qspi_cmd_info_t *cmd_info);
ext_errno uapi_qspi_display_read_cmd(uint32_t cmd, uint8_t *buff, uint8_t len);
ext_errno uapi_qspi_display_show_on(uintptr_t frame_addr, qspi_send_frame_cb_t callback);
ext_errno uapi_qspi_display_show_on_auto_cs(uintptr_t frame_addr, qspi_send_frame_cb_t callback);
bool is_qspi_transfer_done(void);
errcode_t uapi_pin_set_mode(pin_t pin, pin_mode_t mode);
ext_errno uapi_qspi_display_set_partial_cmd(uint32_t addr, uint32_t start, uint32_t end);
void uapi_qspi_display_set_channel_cfg(void);
out_format_t uapi_qspi_get_color_fmt(void);
ext_errno uapi_qspi_set_out_fmt(uint32_t fmt);
#endif
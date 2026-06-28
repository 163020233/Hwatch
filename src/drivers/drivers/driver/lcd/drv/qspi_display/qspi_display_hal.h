/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: mipi tx hal header file.
 *
 * Create: 2026-04-14
 */

#ifndef __QSPI_DISPLAY_HAL_H__
#define __QSPI_DISPLAY_HAL_H__

#include "common_def.h"
#include "chip_io.h"
#include "td_type.h"
#ifndef GPU_1A22
#define QSPI_LCD_BASE_ADDR          0x24000000
#else
#define QSPI_LCD_BASE_ADDR          0x52029000
#endif
#define DPU_QSPI_DR_ADDR            (QSPI_LCD_BASE_ADDR + 0x30)

typedef enum {
    MOTOROLA_SPI = 0,    // 摩托罗拉协议
    TEXAS_SSP,           // TI协议
    NS_MICROWIRE,        // National Microwire 协议
    PROTOCOL_RESERVED,   // 保留
} frame_protocol;

typedef enum {
    STD_ENHFF = 0, // 标准SPI
    DUAL_ENHFF,    // 双线SPI
    QUAD_ENHFF,    // 四线SPI
    OCTAL_ENHFF,   // 八线SPI
} frame_format;

typedef enum {
    TRANSMIT_AND_RECEIVE = 0,  // 可接收可发送
    TRANSMIT_ONLY,             // 仅发送
    RECEIVE_ONLY,              // 仅接收
    EEPROM_READ,               // 先发后收
} transmit_format;

typedef enum {
    NO_INS = 0,
    INS_LEN_4BIT,
    INS_LEN_8BIT,
    INS_LEN_16BIT,
} cmd_bit_length;

typedef enum {
    STD_INS_STD_ADDR = 0,      // 以标准SPI模式发送指令及地址
    STD_INS_CUSTOM_ADDR,       // 指令以标志SPI模式发送，地址以CTRLRO.ENHFF定义的模式发送；
    CUSTOM_INS_CUSTOM_ADDR,    // 指令及地址均以ENHFF定义的模式发送；
} ins_addr_formate;

typedef enum {
    NO_REVERSE = 0,             // 00: 正常写入，数据不进行反序处理
    BYTE_REVERSE = 1,           // 01: 32bit写入数据按照Byte反序
    TWO_BYTE_REVERSE = 2,       // 10: 32bit写入数据按照2Byte反序
    FOUR_BYTE_REVERSE = 3,      // 11: 32bit写入数据按照4Byte反序
} data_write_order;

typedef enum {
    RECEIVE = 0,
    TRANSMIT,
} microwire_ctl;

typedef enum {
    NON_SEQUENTIAL = 0,     // 非连续Microwire传输
    SEQUENTIAL,             // 连续microwire传输
} microwire_trans_mode;

void qspi_disp_hal_reg_init(void);
void qspi_disp_hal_enable(td_bool enable);
void qspi_disp_hal_set_sst_enable(bool enable);
void qspi_disp_hal_set_frame_cfg(frame_protocol prs, frame_format fmt, transmit_format trsm,
    td_u32 frame_bit, td_u32 control_bit);
void qspi_disp_hal_set_receive_frm_len(td_u32 len);
void qspi_disp_hal_set_ins_addr_cfg(cmd_bit_length cmd_len, td_u32 addr_len, ins_addr_formate fmt);
void qspi_disp_hal_set_ddr_enable(td_bool ddr_en, td_bool ins_ddr_en, td_bool rxds_en);
void qspi_disp_hal_set_freq_div(td_u32 freq_div);
void qspi_disp_hal_set_microwire_cfg(microwire_ctl transmit, microwire_trans_mode mode,
    td_bool hand_shake_en);
void qspi_disp_hal_set_dma_enable(td_bool receive_en, td_bool transmit_en);
void qspi_disp_hal_set_dma_rx_watermark(td_u8 watermark_depth);
void qspi_disp_hal_set_dma_tx_watermark(td_u8 watermark_depth);
void qspi_disp_hal_write_data(uint32_t data);
uint32_t qspi_disp_hal_read_data(void);
void qspi_disp_hal_set_interrupt_mask(uint32_t mask);
void qspi_disp_hal_set_slave_enable(td_u8 enable);
void qspi_disp_hal_set_send_fifo_waterline(td_u8 len);
td_u8 qspi_disp_hal_get_send_fifo_datasize(void);
void qspi_disp_hal_set_receive_fifo_waterline(td_u8 len);
td_u8 qspi_disp_hal_get_receive_fifo_datasize(void);
td_bool qspi_disp_hal_tx_fifo_is_full(void);
void qspi_disp_hal_set_data_wait_en(bool enable);
void qspi_disp_hal_set_data_cfg(data_write_order type, td_u8 wait_cnt, bool data_only,
    bool pready_enable, td_u8 water_line);
bool qspi_disp_hal_get_inv_en(void);
void qspi_disp_hal_clear_data_cfg(void);
#endif /* end of #ifndef __QSPI_DISPLAY_HAL_H__ */

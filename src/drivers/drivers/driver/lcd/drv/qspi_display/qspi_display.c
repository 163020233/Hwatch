/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Display with qspi code encapsulation.
 *
 * Create: 2022-09-05
 */

#include "qspi_display.h"
#include "stdio.h"
#include "soc_osal.h"
#include "soc_errno.h"
#include "debug_print.h"
#include "tcxo.h"
#include "tcxo_porting.h"
#include "hal_dma.h"
#include "dma.h"
#include "chip_io.h"
#include "osal_semaphore.h"
#include "gpio.h"
#include "pm_clocks.h"
#include "lcd_bus.h"
#include "qspi_display_hal.h"
#include "qspi_display_dfx.h"

#ifdef GPU_1A22
#define QSPI_DMA_HANDSHAKE     HAL_DMA_HANDSHAKING_VO_QSPI_TX
#define QSPI_D0                QSPI2_D0
#define QSPI_D1                QSPI2_D1
#define QSPI_D2                QSPI2_D2
#define QSPI_D3                QSPI2_D3
#define QSPI_CLK               QSPI2_CLK
#define QSPI_CS                QSPI2_CS
#else
#define QSPI_DMA_HANDSHAKE     HAL_DMA_HANDSHAKING_QSPI2_1CS_TX
#define QSPI_D0                S_EGPIO0
#define QSPI_D1                S_EGPIO1
#define QSPI_D2                S_EGPIO2
#define QSPI_D3                S_EGPIO3
#define QSPI_CLK               S_EGPIO4
#define QSPI_CS                S_EGPIO5
#endif

#define DST_WORLD_WIDTH        HAL_DMA_TRANSFER_WIDTH_32
#define SRC_WORD_WIDTH         HAL_DMA_TRANSFER_WIDTH_32

#define QSPI_DRNM           (QSPI_LCD_BASE_ADDR + 0x2C)
#define QSPI_M1_HIGHEST_PRI        0x1248
#define QSPI_DISPLAY_MODE_QSPI2    1
#define QSPI_INT_MASK                 0x187
#define LCD_READ_CMD                  0x3
#define LCD_WRITE_STD_CMD             0x2
#define LCD_WRITE_QUAD_CMD            0x32
#define LCD_QSPI_WRITE_DATA_ADDR      0x2C00
#define LCD_SPI_WRITE_DATA_ADDR       0x2C
#define LCD_BRS_FRDV_TWO              0X2
#define LCD_BRS_FRDV_FOUR             0X4

#define BITS_IN_WORD                  32
#define BITS_IN_BYTE                  8
#define CMD_3BYTE_SHIFT               (3 * BITS_IN_BYTE)
#define ADDR_2BYTE_SHIFT              (2 * BITS_IN_BYTE)

#define QSPI_BAUD_CLK_DIV             32  /* baud = 32M / 32 */
#define QSPI_FIFO_THRESHOLD           0xD
#define QSPI_OPERATE_TIMEOUT          512

#define QSPI_DMA_LLI_NUM              2
#define BYTE_MASK                     0xff
#define QSPI_TX_DELAY                 5   /* us */
#define QSPI_DMA_TX_LEVEL             0xC

#define QSPI_CTRL                      (M_CTL_RB_BASE + 0xFF0)
#define QSPI_CTRL1                     (M_CTL_RB_BASE + 0xFF4)
#ifdef GPU_1A22
#define M_CTL_RB_BUS_PRIORITY_0_REG    (M_CTL_RB_BASE + 0xA00)
#define M_CTL_RB_M_SOFT_RST_N          (M_CTL_RB_BASE + 0x54)

#define QSPI_SOFTRST_STA_REG           (QSPI_LCD_BASE_ADDR + 0x04)
#define QSPI_SOFTRST_VAL               0
#endif

#define DEFAULT_CSF_LEN                1
#define CFS_8BIT                       8

#define DFS_8BIT                       8
#define DFS_24BIT                      24
#define DFS_32BIT                      32

#define ADDR_LEN_0BIT                  0
#define ADDR_LEN_8BIT                  8
#define ADDR_LEN_24BIT                 24

typedef struct {
    uint32_t src_addr;
    dma_channel_t ch;
} qspi_ch_addr_info_t;

typedef struct {
    out_format_t out_fmt;
    bool init;
} qspi_panel_info;

typedef enum {
    SPI_RD_CMD_LEN_8_BITS  = 0x1,
    SPI_RD_CMD_LEN_24_BITS = 0x3,
    SPI_RD_CMD_LEN_32_BITS = 0x4,

    SPI_RD_CMD_LEN_MAX
} spi_rd_cmd_len;

qspi_ch_addr_info_t g_qspi_ch_info[QSPI_DMA_LLI_NUM];

volatile bool g_qspi_display_is_init;
volatile bool g_qspi_dma_tranfer_done = true;
volatile bool g_qspi_dma_tranfer_success = false;
qspi_panel_info g_qspi_panel_info;
qspi_send_frame_cb_t g_qspi_display_callback = NULL;
volatile uint32_t g_qspi_clk_div = 4;

#ifdef SUPPORT_GPU_SPI
ext_errno spi_display_write_cmd(qspi_cmd_info_t *cmd_info);
ext_errno spi_display_read_cmd(uint32_t cmd, uint8_t *buff, uint8_t len);
void spi_set_cmd_addr(uint32_t brs_val);
#define DCX_DELAY_TIME 300
#endif

static uint32_t one_byte_reverse(uint32_t x)
{
    return ((x & 0x000000FF) << 24) | ((x & 0x0000FF00) << 8) | // 24 8 is offset
        ((x & 0x00FF0000) >> 8) | ((x & 0xFF000000) >> 24); // 24 8 is offset
}

static uint32_t two_byte_reverse(uint32_t x)
{
    return (((x & 0xFFFF0000) >> 16) | ((x & 0x0000FFFF) << 16));   // 16 is offset
}

static uint32_t four_byte_reverse(uint32_t x)
{
    uint32_t temp_x = x;
    temp_x = ((temp_x & 0x0F0F0F0F) << 4) | ((temp_x & 0xF0F0F0F0) >> 4);  // Reverse each 4-bit
    temp_x = ((temp_x & 0x00FF00FF) << 8) | ((temp_x & 0xFF00FF00) >> 8);  // Reverse each 8-bit
    temp_x = ((temp_x & 0x0000FFFF) << 16) | ((temp_x & 0xFFFF0000) >> 16); // Reverse the 16-bit
    return temp_x;
}

static void qspi_io_init(void)
{
    /* pin mux cfg */
    uapi_pin_set_mode(QSPI_D0, PIN_MODE_1);
    uapi_pin_set_mode(QSPI_D1, PIN_MODE_1);
    uapi_pin_set_mode(QSPI_D2, PIN_MODE_1);
    uapi_pin_set_mode(QSPI_D3, PIN_MODE_1);
    uapi_pin_set_mode(QSPI_CLK, PIN_MODE_1);
    uapi_pin_set_mode(QSPI_CS, PIN_MODE_1);
#ifdef SUPPORT_GPU_SPI
    uapi_pin_set_mode(SPI_DCX_GPIO, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(SPI_DCX_GPIO, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(SPI_DCX_GPIO, GPIO_LEVEL_LOW);
#endif
}

static void qspi_set_m1_highest_pri(void)
{
    writel(M_CTL_RB_BUS_PRIORITY_0_REG, QSPI_M1_HIGHEST_PRI);
    reg16_setbit(XIP_CACHE_CTL_RB_XIP_GP_REG0_REG, QSPI_DISPLAY_MODE_QSPI2);
}

out_format_t uapi_qspi_get_color_fmt(void)
{
    return g_qspi_panel_info.out_fmt;
}

ext_errno uapi_qspi_set_out_fmt(uint32_t fmt)
{
    if (fmt > OUT_FORMAT_BUTT) {
        printf("fmt [%u] error!!\n", fmt);
        return EXT_ERR_FAILURE;
    }
    g_qspi_panel_info.out_fmt = fmt;
    return EXT_ERR_SUCCESS;
}

bool is_qspi_transfer_done(void)
{
    return g_qspi_dma_tranfer_done;
}

#ifdef GPU_1A22
td_void qspi_wait_soft_reset_succ(void)
{
    td_u32 retry = 0;
    while (uapi_reg_read_val32(QSPI_SOFTRST_STA_REG) != QSPI_SOFTRST_VAL) {
        if (retry++ > 0xFF) {
            PRINT("qspi soft reset fail!");
            break;
        }
    }
}
#endif

ext_errno uapi_qspi_display_init(out_format_t out_fmt)
{
    qspi_disp_dfx_info *dfx_info = qspi_disp_dfx_get_info();
    g_qspi_panel_info.out_fmt = out_fmt;
    g_qspi_panel_info.init = TD_TRUE;
    dfx_info->out_fmt = out_fmt;
    uint32_t irq_sts = osal_irq_lock();
    qspi_disp_hal_reg_init();
#ifdef GPU_3322
    uapi_clocks_switch(CLOCKS_ID_M_QSPI_DISPLAY, TURN_ON);
#endif

#ifdef GPU_1A22
    uapi_reg_clrbit(M_CTL_RB_M_SOFT_RST_N, 0); // 1：解复位；0：复位；
    qspi_wait_soft_reset_succ();
    uapi_reg_setbit(M_CTL_RB_M_SOFT_RST_N, 0);
#endif

    qspi_set_m1_highest_pri();
    qspi_io_init();

    qspi_disp_hal_enable(TD_FALSE);
    qspi_disp_hal_set_sst_enable(TD_FALSE);
    qspi_disp_hal_set_frame_cfg(MOTOROLA_SPI, STD_ENHFF, TRANSMIT_ONLY, DFS_8BIT, DEFAULT_CSF_LEN);
    qspi_disp_hal_set_freq_div(QSPI_BAUD_CLK_DIV);
    qspi_disp_hal_set_interrupt_mask(QSPI_INT_MASK);
    qspi_disp_hal_set_send_fifo_waterline(QSPI_FIFO_THRESHOLD);
    qspi_disp_hal_set_receive_fifo_waterline(QSPI_FIFO_THRESHOLD);
    qspi_disp_hal_enable(TD_TRUE);

    g_qspi_display_is_init = true;
    osal_irq_restore(irq_sts);
    dfx_info->clk_div = QSPI_BAUD_CLK_DIV;
    return EXT_ERR_SUCCESS;
}

ext_errno uapi_qspi_display_deinit(void)
{
    if (!g_qspi_display_is_init) {
        return EXT_ERR_SUCCESS;
    }

    g_qspi_display_is_init = false;
    return EXT_ERR_SUCCESS;
}

static ext_errno qspi_create_lli(qspi_display_frame_info_t *frame_info, dma_channel_t ch)
{
    dma_ch_user_peripheral_config_t user_cfg = {0};

    user_cfg.dest = QSPI_DRNM;
    user_cfg.transfer_num = (uint16_t)(frame_info->frame_col_byte / (1 << SRC_WORD_WIDTH));
    user_cfg.dest_width = DST_WORLD_WIDTH;
    user_cfg.src_width = SRC_WORD_WIDTH;
    user_cfg.src_increment = HAL_DMA_ADDRESS_INC_INCREMENT;
    user_cfg.dest_increment = HAL_DMA_ADDRESS_INC_NO_CHANGE;
    user_cfg.burst_length = HAL_DMA_BURST_TRANSACTION_LENGTH_4;
    user_cfg.trans_type = HAL_DMA_TRANS_MEMORY_TO_PERIPHERAL_DMA;
    user_cfg.trans_dir = HAL_DMA_TRANSFER_DIR_MEM_TO_PERIPHERAL;

    user_cfg.dest_handshaking = QSPI_DMA_HANDSHAKE;
    user_cfg.src_handshaking = QSPI_DMA_HANDSHAKE;

    for (uint32_t i = 0; i < frame_info->frame_raw; i++) {
        user_cfg.src = frame_info->frame_addr + i * frame_info->col_byte_with_stride;
        if (uapi_dma_configure_peripheral_transfer_lli(ch, &user_cfg, NULL) != ERRCODE_SUCC) {
            uapi_dma_end_transfer(ch);
            return EXT_ERR_QSPI_DISPLAY_FAILED;
        }
    }

    return EXT_ERR_SUCCESS;
}

static uint8_t qspi_display_addr_exist(uint32_t addr)
{
    uint8_t id;

    for (id = 0; id < QSPI_DMA_LLI_NUM; id++) {
        if (g_qspi_ch_info[id].src_addr == addr) {
            break;
        }
    }

    return id;
}

static uint8_t qspi_display_get_info_id(uint32_t addr)
{
    uint8_t id;

    UNUSED(addr);
    for (id = 0; id < QSPI_DMA_LLI_NUM; id++) {
        if (g_qspi_ch_info[id].src_addr == 0) {
            break;
        }
    }

    return id;
}

ext_errno uapi_qspi_display_enable(qspi_display_frame_info_t *frame_info)
{
    uint8_t id;
    ext_errno ret;
    if (!g_qspi_display_is_init) {
        return EXT_ERR_QSPI_D_NOT_INIT;
    }

    if (qspi_display_addr_exist(frame_info->frame_addr) < QSPI_DMA_LLI_NUM) {
        return EXT_ERR_QSPI_REENABLE;
    }

    id = qspi_display_get_info_id(frame_info->frame_addr);
    if (id >= QSPI_DMA_LLI_NUM) {
        return EXT_ERR_QSPI_DMA_LLI_FULL;
    }

    uint32_t irq_sts = osal_irq_lock();
    ret = qspi_create_lli(frame_info, frame_info->channel);
    if (ret != EXT_ERR_SUCCESS) {
        osal_irq_restore(irq_sts);
        return ret;
    }

    g_qspi_ch_info[id].ch = frame_info->channel;
    g_qspi_ch_info[id].src_addr = frame_info->frame_addr;
    osal_irq_restore(irq_sts);

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_qspi_display_disable(void)
{
    uint32_t lli_id;

    for (lli_id = 0; lli_id < QSPI_DMA_LLI_NUM; lli_id++) {
        if (g_qspi_ch_info[lli_id].src_addr != 0) {
            uapi_dma_end_transfer(g_qspi_ch_info[lli_id].ch);
            g_qspi_ch_info[lli_id].src_addr = 0;
        }
    }
    return EXT_ERR_SUCCESS;
}

static void qspi_write_cmd_data(qspi_cmd_info_t *cmd_info)
{
    uint32_t cnt = 0;
    td_bool slave_en = TD_FALSE;
    while (cnt < cmd_info->data_len) {
        if (!qspi_disp_hal_tx_fifo_is_full()) {
            qspi_disp_hal_write_data(cmd_info->data[cnt]);
            cnt++;
        } else if (slave_en == TD_FALSE) { /* when fifo full enable tx send */
            slave_en = TD_TRUE;
            qspi_disp_hal_set_slave_enable(TD_TRUE);
        }
    }
}
static inline void qspi_display_restore_reg(void)
{
    bool inv_byte_en = qspi_disp_hal_get_inv_en();
    if (!inv_byte_en) {
        qspi_disp_hal_enable(TD_FALSE);
        qspi_disp_hal_clear_data_cfg();
        qspi_disp_hal_set_dma_enable(TD_FALSE, TD_FALSE);
        qspi_disp_hal_set_dma_rx_watermark(0x0);
        qspi_disp_hal_enable(TD_TRUE);
    }
}
static inline td_s32 wait_send_fifo_empty(void)
{
    for (int wait_cnt = 0; wait_cnt < 30000; wait_cnt++) { // 30000 is 30ms
        if (qspi_disp_hal_get_send_fifo_datasize() == 0) {
            return EXT_SUCCESS;
        }
        uapi_tcxo_delay_us(1); // 1 is 1us
    }
    printf("qspi wait send fifo empty time out\n");
    return EXT_FAILURE;
}

ext_errno uapi_qspi_display_write_cmd(qspi_cmd_info_t *cmd_info)
{
    if (!g_qspi_display_is_init) {
        return EXT_ERR_QSPI_D_NOT_INIT;
    }
#ifdef SUPPORT_GPU_SPI
    return spi_display_write_cmd(cmd_info);
#endif
    uint32_t irq_sts = osal_irq_lock();
    qspi_display_restore_reg();
    qspi_disp_dfx_set_clk_div(QSPI_BAUD_CLK_DIV);

    qspi_disp_hal_enable(TD_FALSE);
    qspi_disp_hal_set_slave_enable(TD_FALSE);
    qspi_disp_hal_set_freq_div(QSPI_BAUD_CLK_DIV); /* rm lcd screen min cycle is 20ns */
    if (lcd_get_bus_type() == BUS_DISPLAY_QSPI) {
        qspi_disp_hal_set_frame_cfg(MOTOROLA_SPI, STD_ENHFF, TRANSMIT_ONLY, DFS_8BIT, DEFAULT_CSF_LEN);
        qspi_disp_hal_set_ins_addr_cfg(INS_LEN_8BIT, ADDR_LEN_24BIT, STD_INS_STD_ADDR);
        qspi_disp_hal_enable(TD_TRUE);
        qspi_disp_hal_write_data(LCD_WRITE_STD_CMD);
        qspi_disp_hal_write_data((cmd_info->reg_addr >> 16) & BYTE_MASK); /* 16: reg_addr byte 2 */
        qspi_disp_hal_write_data((cmd_info->reg_addr >> 8) & BYTE_MASK);  /* 8: reg_addr byte 1 */
    } else {
        qspi_disp_hal_set_ins_addr_cfg(NO_INS, ADDR_LEN_0BIT, STD_INS_STD_ADDR);
        qspi_disp_hal_set_frame_cfg(NS_MICROWIRE, STD_ENHFF, TRANSMIT_ONLY, DFS_8BIT, CFS_8BIT);
        qspi_disp_hal_set_microwire_cfg(TRANSMIT, SEQUENTIAL, TD_FALSE);
        qspi_disp_hal_enable(TD_TRUE);
    }
    qspi_disp_hal_write_data(cmd_info->reg_addr & BYTE_MASK);
    qspi_write_cmd_data(cmd_info);

    qspi_disp_hal_set_slave_enable(TD_TRUE);
    wait_send_fifo_empty();
    uapi_tcxo_delay_us(QSPI_TX_DELAY);
    osal_irq_restore(irq_sts);
    return EXT_ERR_SUCCESS;
}

static uint8_t quad_data_convert_to_u8(uint32_t quad_data)
{
    uint32_t bit = 0;
    uint8_t offset = 0;
    uint8_t sum = 0;
    while (bit < BITS_IN_WORD) {
        uint8_t valid_bit = (uint8_t)((quad_data >> bit) & 0x1); /* only 1bit valid in 4bits */
        sum += valid_bit * (1 << offset);
        bit += 4; /* 4: obtain 4bits data every loop */
        offset++;
    }

    return sum;
}

static ext_errno spi_recv_data(uint8_t *buffer, uint32_t length)
{
    uint32_t ul_recv_timeout = 0;
    uint32_t ul_recv_num = 0;
    uint32_t value;

    while (ul_recv_num == 0) {
        ul_recv_num = qspi_disp_hal_get_receive_fifo_datasize();
        ul_recv_timeout++;
        if (ul_recv_timeout >= QSPI_OPERATE_TIMEOUT) {
            return EXT_ERR_QSPI_READ_TIMEOUT;
        }
    }
    value = qspi_disp_hal_read_data();
    if (length == SPI_RD_CMD_LEN_32_BITS) {
        buffer[0] = (value >> 24) & 0xFF; // 24bit
        buffer[1] = (value >> 16) & 0xFF; // 16bit
        buffer[2] = (value >> 8) & 0xFF;  // 8bit 2 num
        buffer[3] = value & 0xFF; // 3 num
    } else if (length == SPI_RD_CMD_LEN_24_BITS) {
        buffer[0] = (value >> 16) & 0xFF; // 16bit
        buffer[1] = (value >> 8) & 0xFF;  // 8bit
        buffer[2] = value & 0xFF; // 2 num
    }

    return EXT_ERR_SUCCESS;
}

static ext_errno recv_data_by_byte(uint8_t *buffer, uint32_t length)
{
    uint32_t ul_loop = 0;
    uint32_t ul_recv_timeout = 0;
    uint32_t ul_recv_num = 0;
    uint32_t tmp_data;
    while (ul_loop < length) {
        ul_recv_num = qspi_disp_hal_get_receive_fifo_datasize();
        while (ul_recv_num > 0) {
            tmp_data = qspi_disp_hal_read_data();
            buffer[ul_loop] = quad_data_convert_to_u8(tmp_data);
            ul_loop++;
            ul_recv_num--;
            ul_recv_timeout = 0;
        }

        ul_recv_timeout++;

        if (ul_recv_timeout >= QSPI_OPERATE_TIMEOUT) {
            return EXT_ERR_QSPI_READ_TIMEOUT;
        }
    }

    return EXT_ERR_SUCCESS;
}

static ext_errno spi_read_cmd(uint8_t len)
{
#ifdef GPU_1A22
    writel(QSPI_CTRL, 0x8140);
#endif
    /* 大于8bits 需要插dummy 切micro协议 */
    if (len == SPI_RD_CMD_LEN_8_BITS) {
        qspi_disp_hal_set_ins_addr_cfg(NO_INS, ADDR_LEN_8BIT, STD_INS_STD_ADDR);
        return EXT_ERR_SUCCESS;
    }
    qspi_disp_hal_set_frame_cfg(NS_MICROWIRE, STD_ENHFF, RECEIVE_ONLY, len * BITS_IN_BYTE, CFS_8BIT);
    qspi_disp_hal_set_ins_addr_cfg(NO_INS, ADDR_LEN_0BIT, STD_INS_STD_ADDR);
    qspi_disp_hal_set_microwire_cfg(RECEIVE, NON_SEQUENTIAL, TD_FALSE);
    return EXT_ERR_SUCCESS;
}

ext_errno uapi_qspi_display_read_cmd(uint32_t cmd, uint8_t *buff, uint8_t len)
{
    ext_errno ret;
    if (!g_qspi_display_is_init) {
        return EXT_ERR_QSPI_D_NOT_INIT;
    }
#ifdef SUPPORT_GPU_SPI
    return spi_display_read_cmd(cmd, buff, len);
#endif
    uint32_t irq_sts = osal_irq_lock();
    qspi_display_restore_reg();
    qspi_disp_hal_enable(TD_FALSE);
#ifdef GPU_1A22
    writel(QSPI_CTRL, 0x0); /* 还原 */
#endif
    qspi_disp_hal_set_frame_cfg(MOTOROLA_SPI, QUAD_ENHFF, EEPROM_READ, DFS_32BIT, DEFAULT_CSF_LEN);
    qspi_disp_hal_set_ins_addr_cfg(INS_LEN_8BIT, ADDR_LEN_24BIT, STD_INS_STD_ADDR);
    if (lcd_get_bus_type() == BUS_DISPLAY_SPI) {
        if (spi_read_cmd(len) != EXT_ERR_SUCCESS) {
            osal_irq_restore(irq_sts);
            return EXT_ERR_FAILURE;
        }
    }
    qspi_disp_hal_set_receive_frm_len(len);
    qspi_disp_hal_set_freq_div(QSPI_BAUD_CLK_DIV);
    qspi_disp_hal_enable(TD_TRUE);
    qspi_disp_dfx_set_clk_div(QSPI_BAUD_CLK_DIV);
    qspi_disp_hal_set_slave_enable(TD_FALSE);
    if (lcd_get_bus_type() != BUS_DISPLAY_SPI) {
        qspi_disp_hal_write_data(LCD_READ_CMD);
    }

    qspi_disp_hal_write_data(cmd);
    qspi_disp_hal_set_slave_enable(TD_TRUE);
    if (lcd_get_bus_type() == BUS_DISPLAY_SPI && len > SPI_RD_CMD_LEN_8_BITS) {
        ret = spi_recv_data(buff, len);
    } else {
        ret = recv_data_by_byte(buff, len);
    }
    osal_irq_restore(irq_sts);
    if (ret != EXT_ERR_SUCCESS) {
        PRINT("QSPI read data error, ret is %#x\n", ret);
    }
    return ret;
}

static void qspi_dma_tranfer_callback(hal_dma_interrupt_t int_type)
{
#ifndef GPU_1A22
    uapi_pin_set_mode(QSPI_CS_GPIO, (pin_mode_t)HAL_PIO_FUNC_QSPI2);
#endif
    switch (int_type) {
        case HAL_DMA_INTERRUPT_TFR:
            g_qspi_dma_tranfer_done = true;
            g_qspi_dma_tranfer_success = true;
            break;
        case HAL_DMA_INTERRUPT_BLOCK:
            g_qspi_dma_tranfer_done = true;
            g_qspi_dma_tranfer_success = false;
            break;
        case HAL_DMA_INTERRUPT_ERR:
            g_qspi_dma_tranfer_done = true;
            g_qspi_dma_tranfer_success = false;
            break;
        default:
            break;
    }

    if (g_qspi_display_callback != NULL && g_qspi_dma_tranfer_success) {
        g_qspi_display_callback();
    }
}

ext_errno uapi_qspi_display_show_on_auto_cs(uintptr_t frame_addr, qspi_send_frame_cb_t callback)
{
    uint8_t id;
    if (!g_qspi_display_is_init) {
        return EXT_ERR_QSPI_D_NOT_INIT;
    }

    id = qspi_display_addr_exist(frame_addr);
    if (id >= QSPI_DMA_LLI_NUM) {
        return EXT_ERR_QSPI_NOT_ENABLE;
    }

    g_qspi_display_callback = callback;
    g_qspi_dma_tranfer_done = false;
    g_qspi_dma_tranfer_success = false;

    qspi_disp_hal_enable(TD_FALSE);
    qspi_disp_hal_set_frame_cfg(MOTOROLA_SPI, QUAD_ENHFF, TRANSMIT_ONLY, DFS_24BIT, DEFAULT_CSF_LEN);
    qspi_disp_hal_set_ins_addr_cfg(INS_LEN_8BIT, ADDR_LEN_24BIT, STD_INS_STD_ADDR);
    qspi_disp_hal_set_slave_enable(TD_FALSE);
    qspi_disp_hal_enable(TD_TRUE);

    qspi_disp_hal_write_data(LCD_WRITE_QUAD_CMD);
    qspi_disp_hal_write_data(LCD_QSPI_WRITE_DATA_ADDR);
    qspi_disp_hal_set_dma_enable(TD_FALSE, TD_TRUE);
    uapi_dma_enable_lli((uint8_t)g_qspi_ch_info[id].ch, (dma_transfer_cb_t)qspi_dma_tranfer_callback,
        (uintptr_t)NULL);
    qspi_disp_hal_set_slave_enable(TD_TRUE);
    if (g_qspi_display_callback == NULL) {
        while (!g_qspi_dma_tranfer_done) {}

        if (!g_qspi_dma_tranfer_success) {
            PRINT("QSPI DISPLAY failed, dma tranfer wrong!\r\n");
            return EXT_ERR_QSPI_DISPLAY_FAILED;
        }
    }

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_qspi_display_show_on(uintptr_t frame_addr, qspi_send_frame_cb_t callback)
{
    uint8_t id;
    if (!g_qspi_display_is_init) {
        return EXT_ERR_QSPI_D_NOT_INIT;
    }

    id = qspi_display_addr_exist(frame_addr);
    if (id >= QSPI_DMA_LLI_NUM) {
        return EXT_ERR_QSPI_NOT_ENABLE;
    }

    g_qspi_display_callback = callback;
    g_qspi_dma_tranfer_done = false;
    g_qspi_dma_tranfer_success = false;

    qspi_disp_hal_enable(TD_FALSE);
    qspi_disp_hal_set_frame_cfg(MOTOROLA_SPI, STD_ENHFF, TRANSMIT_ONLY, DFS_32BIT, DEFAULT_CSF_LEN);
    qspi_disp_hal_set_ins_addr_cfg(INS_LEN_8BIT, ADDR_LEN_24BIT, STD_INS_STD_ADDR);
    qspi_disp_hal_set_freq_div(g_qspi_clk_div);
    qspi_disp_dfx_set_clk_div(QSPI_BAUD_CLK_DIV);
    qspi_disp_hal_set_dma_enable(TD_FALSE, TD_FALSE);
    qspi_disp_hal_set_dma_tx_watermark(QSPI_DMA_TX_LEVEL);
    qspi_disp_hal_set_slave_enable(TD_FALSE);
    qspi_disp_hal_enable(TD_TRUE);
    qspi_disp_hal_write_data((LCD_WRITE_QUAD_CMD << CMD_3BYTE_SHIFT) | LCD_QSPI_WRITE_DATA_ADDR);
#ifndef GPU_1A22
    uapi_pin_set_mode(QSPI_CS_GPIO, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(QSPI_CS_GPIO, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(QSPI_CS_GPIO, GPIO_LEVEL_LOW);
#endif
    qspi_disp_hal_set_slave_enable(TD_TRUE);
    uapi_tcxo_delay_us(QSPI_TX_DELAY);

    qspi_disp_hal_enable(TD_FALSE);
    qspi_disp_hal_set_frame_cfg(MOTOROLA_SPI, QUAD_ENHFF, TRANSMIT_ONLY, DFS_24BIT, DEFAULT_CSF_LEN);
    qspi_disp_hal_set_ins_addr_cfg(NO_INS, ADDR_LEN_0BIT, STD_INS_CUSTOM_ADDR);
    qspi_disp_hal_set_data_cfg(NO_REVERSE, 0, TD_TRUE, TD_TRUE, 16); // 0 is wait cnt 16 is water line
    qspi_disp_hal_enable(TD_TRUE);
    uapi_dma_enable_lli((uint8_t)g_qspi_ch_info[id].ch, (dma_transfer_cb_t)qspi_dma_tranfer_callback,
        (uintptr_t)NULL);
    qspi_disp_hal_set_dma_enable(TD_FALSE, TD_TRUE);

    if (g_qspi_display_callback == NULL) {
        while (!g_qspi_dma_tranfer_done) {}

        if (!g_qspi_dma_tranfer_success) {
            PRINT("QSPI DISPLAY failed, dma tranfer wrong!\r\n");
            return EXT_ERR_QSPI_DISPLAY_FAILED;
        }
    }

    return EXT_ERR_SUCCESS;
}

ext_errno uapi_qspi_display_set_partial_cmd(uint32_t addr, uint32_t start, uint32_t end)
{
    qspi_cmd_info_t cmd_info = {0};

    uint8_t *data = (uint8_t *)osal_kmalloc(4, OSAL_GFP_KERNEL);
    if (data == NULL) {
        printf("set partial cmd faile, malloc err\n");
        return EXT_ERR_MALLOC_FAILURE;
    }
    data[0] = (start >> 8) & 0xFF; // 0 is index, 8 is offset
    data[1] = start & 0xFF; // 1 is index,
    data[2] = (end >> 8) & 0xFF; // 2 is index,  8 is offset
    data[3] = end & 0xFF; // 3 is index,
    cmd_info.reg_addr = addr;

    cmd_info.data_len = 0x4;
    cmd_info.data = data;
    ext_errno ret = uapi_qspi_display_write_cmd(&cmd_info);
    osal_kfree(data);

    if (ret == EXT_ERR_SUCCESS) {
        return EXT_SUCCESS;
    } else {
        printf("set cmd failed, error code is 0x%x!!\r\n", ret);
        return ret;
    }
}

static void spi_reverse_data_cmd(data_write_order order)
{
    if (order == BYTE_REVERSE) {
        qspi_disp_hal_write_data(one_byte_reverse(LCD_SPI_WRITE_DATA_ADDR));
    } else if (order == FOUR_BYTE_REVERSE) {
        qspi_disp_hal_write_data(four_byte_reverse(LCD_SPI_WRITE_DATA_ADDR));
    } else {
        qspi_disp_hal_write_data(two_byte_reverse(LCD_SPI_WRITE_DATA_ADDR));
    }
}

static void qspi_reverse_data_cmd(data_write_order order)
{
    if (order == BYTE_REVERSE) {
        qspi_disp_hal_write_data(one_byte_reverse(LCD_WRITE_QUAD_CMD));
        qspi_disp_hal_write_data(one_byte_reverse(LCD_QSPI_WRITE_DATA_ADDR));
    } else if (order == FOUR_BYTE_REVERSE) {
        qspi_disp_hal_write_data(four_byte_reverse(LCD_WRITE_QUAD_CMD));
        qspi_disp_hal_write_data(four_byte_reverse(LCD_QSPI_WRITE_DATA_ADDR));
    } else {
        qspi_disp_hal_write_data(two_byte_reverse(LCD_WRITE_QUAD_CMD));
        qspi_disp_hal_write_data(two_byte_reverse(LCD_QSPI_WRITE_DATA_ADDR));
    }
}

static void qspi_set_cmd_addr(data_write_order order, uint32_t brs_val)
{
    qspi_disp_hal_enable(TD_FALSE);
    __asm__ __volatile__ ("fence" : : : "memory");
    if (lcd_get_bus_type() == BUS_DISPLAY_SPI) {
        qspi_disp_hal_set_frame_cfg(NS_MICROWIRE, STD_ENHFF, TRANSMIT_ONLY, DFS_32BIT, CFS_8BIT);
        qspi_disp_hal_set_ins_addr_cfg(NO_INS, ADDR_LEN_0BIT, STD_INS_STD_ADDR);
        qspi_disp_hal_set_microwire_cfg(TRANSMIT, SEQUENTIAL, TD_FALSE);
        qspi_disp_hal_set_data_cfg(order, 0X6, TD_FALSE, TD_TRUE, 16); // 6 is wait cnt 16 is water line
        qspi_disp_hal_set_data_wait_en(TD_TRUE);
    } else {
        qspi_disp_hal_set_frame_cfg(MOTOROLA_SPI, QUAD_ENHFF, TRANSMIT_ONLY, DFS_32BIT, DEFAULT_CSF_LEN);
        qspi_disp_hal_set_ins_addr_cfg(INS_LEN_8BIT, ADDR_LEN_24BIT, STD_INS_STD_ADDR);
        qspi_disp_hal_set_data_cfg(order, 1, TD_FALSE, TD_TRUE, 16); // 1 is wait cnt 16 is water line
    }

    qspi_disp_hal_set_receive_frm_len(64); // 64 is max receive frame len
    qspi_disp_hal_set_receive_fifo_waterline(0x2);
    qspi_disp_hal_set_send_fifo_waterline(0xd);
    qspi_disp_hal_set_dma_rx_watermark(0x1);
    qspi_disp_hal_set_dma_enable(TD_TRUE, TD_FALSE);
    qspi_disp_hal_set_freq_div(brs_val);
    qspi_disp_hal_set_slave_enable(TD_FALSE);
    __asm__ __volatile__ ("fence" : : : "memory");
    qspi_disp_hal_enable(TD_TRUE);

    if (lcd_get_bus_type() == BUS_DISPLAY_SPI) {
        spi_reverse_data_cmd(order);
    } else {
        qspi_reverse_data_cmd(order);
    }
    qspi_disp_hal_set_slave_enable(TD_TRUE);
    qspi_disp_dfx_set_clk_div(QSPI_BAUD_CLK_DIV);
}

void uapi_qspi_display_set_channel_cfg(void)
{
#ifdef SUPPORT_GPU_SPI
    spi_set_cmd_addr(LCD_BRS_FRDV_TWO);
    return;
#endif
    out_format_t out_fmt = OUT_FORMAT_RGB_24BIT;
    if (g_qspi_panel_info.init) {
        out_fmt = g_qspi_panel_info.out_fmt;
    }
    /*
     * RGB888 RGB666 RGB332 R8 onebyte
     * RGB565 twobyte  RGB444 4bit
     */
    if (out_fmt == OUT_FORMAT_RGB_16BIT) {
        qspi_set_cmd_addr(TWO_BYTE_REVERSE, LCD_BRS_FRDV_TWO);
    } else if (out_fmt == OUT_FORMAT_R_4BIT || out_fmt == OUT_FORMAT_G_4BIT || out_fmt == OUT_FORMAT_B_4BIT) {
        qspi_set_cmd_addr(BYTE_REVERSE, LCD_BRS_FRDV_FOUR);
    } else if (out_fmt == OUT_FORMAT_RGB_12BIT) {
        qspi_set_cmd_addr(FOUR_BYTE_REVERSE, LCD_BRS_FRDV_TWO);
    } else {
        qspi_set_cmd_addr(BYTE_REVERSE, LCD_BRS_FRDV_TWO);
    }
}

#ifdef SUPPORT_GPU_SPI
static inline void delay_10ns(uint32_t times)
{
    uint32_t i;
    for (i = 0; i < times; i++) {
        __asm__ __volatile__("nop");
    }
}
ext_errno spi_display_write_cmd(qspi_cmd_info_t *cmd_info)
{
    uint32_t cnt = 0;
    if (!g_qspi_display_is_init) {
        return EXT_ERR_QSPI_D_NOT_INIT;
    }
    if (cmd_info->reg_addr > 0xff) {
        cmd_info->reg_addr = cmd_info->reg_addr >> 8; // 8 is offset
    }
    uint32_t irq_sts = osal_irq_lock();
    uapi_pin_set_mode(QSPI_CS_GPIO, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(QSPI_CS_GPIO, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(QSPI_CS_GPIO, GPIO_LEVEL_LOW);

    qspi_disp_hal_enable(TD_FALSE);
    qspi_disp_hal_set_frame_cfg(MOTOROLA_SPI, STD_ENHFF, TRANSMIT_ONLY, DFS_8BIT, DEFAULT_CSF_LEN);
    qspi_disp_hal_set_ins_addr_cfg(NO_INS, ADDR_LEN_8BIT, STD_INS_STD_ADDR);
    uapi_gpio_set_val(SPI_DCX_GPIO, GPIO_LEVEL_LOW);
    qspi_disp_hal_set_slave_enable(TD_FALSE);
    qspi_disp_hal_enable(TD_TRUE);
    qspi_disp_hal_write_data(cmd_info->reg_addr & BYTE_MASK);
    qspi_disp_hal_set_slave_enable(TD_TRUE);
    delay_10ns(DCX_DELAY_TIME);
    uapi_gpio_set_val(SPI_DCX_GPIO, GPIO_LEVEL_HIGH);
    while (cnt < cmd_info->data_len) {
        if (!qspi_disp_hal_tx_fifo_is_full()) {
            qspi_disp_hal_write_data(cmd_info->data[cnt]);
            cnt++;
        }
    }
    wait_send_fifo_empty();
    uapi_tcxo_delay_us(QSPI_TX_DELAY);
    osal_irq_restore(irq_sts);
    uapi_pin_set_mode(QSPI_CS_GPIO, (pin_mode_t)HAL_PIO_FUNC_QSPI2);
    return EXT_ERR_SUCCESS;
}

ext_errno spi_display_read_cmd(uint32_t cmd, uint8_t *buff, uint8_t len)
{
    if (!g_qspi_display_is_init) {
        return EXT_ERR_QSPI_D_NOT_INIT;
    }
    if (cmd > 0xff) {
        cmd = cmd >> 8; // 8 is offset
    }
    uapi_gpio_set_val(SPI_DCX_GPIO, GPIO_LEVEL_LOW);
    qspi_disp_dfx_set_clk_div(50);  // 50 is frequency division ratio
    uint32_t irq_sts = osal_irq_lock();
    qspi_disp_hal_enable(TD_FALSE);
    qspi_disp_hal_set_frame_cfg(MOTOROLA_SPI, QUAD_ENHFF, EEPROM_READ, DFS_32BIT, DEFAULT_CSF_LEN);
    qspi_disp_hal_set_ins_addr_cfg(INS_LEN_8BIT, ADDR_LEN_0BIT, STD_INS_STD_ADDR);
    qspi_disp_hal_set_receive_frm_len(len);
    qspi_disp_hal_set_freq_div(50); // 50 is frequency division ratio
    qspi_disp_hal_enable(TD_TRUE);
    qspi_disp_hal_set_slave_enable(TD_FALSE);
    qspi_disp_hal_write_data(cmd);
    qspi_disp_hal_set_slave_enable(TD_TRUE);
    delay_10ns(DCX_DELAY_TIME);
    uapi_gpio_set_val(SPI_DCX_GPIO, GPIO_LEVEL_HIGH);
    ext_errno ret = recv_data_by_byte(buff, len);
    osal_irq_restore(irq_sts);
    if (ret != EXT_ERR_SUCCESS) {
        PRINT("QSPI read data error, ret is %#x\n", ret);
    }
    return ret;
}

void spi_set_cmd_addr(uint32_t brs_val)
{
#ifndef GPU_1A22
    uapi_pin_set_mode(QSPI_CS_GPIO, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(QSPI_CS_GPIO, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(QSPI_CS_GPIO, GPIO_LEVEL_LOW);
#endif
    qspi_disp_dfx_set_clk_div(brs_val);
    uapi_gpio_set_val(SPI_DCX_GPIO, GPIO_LEVEL_LOW);
    qspi_disp_hal_enable(TD_FALSE);
    qspi_disp_hal_set_frame_cfg(MOTOROLA_SPI, STD_ENHFF, TRANSMIT_ONLY, DFS_8BIT, DEFAULT_CSF_LEN);
    qspi_disp_hal_set_ins_addr_cfg(NO_INS, ADDR_LEN_8BIT, STD_INS_STD_ADDR);
    qspi_disp_hal_set_freq_div(brs_val);
    qspi_disp_hal_clear_data_cfg();
    qspi_disp_hal_set_slave_enable(TD_FALSE);
    qspi_disp_hal_enable(TD_TRUE);
    qspi_disp_hal_write_data(LCD_SPI_WRITE_DATA_ADDR);
    qspi_disp_hal_set_slave_enable(TD_TRUE); // start send addr

    uapi_tcxo_delay_us(QSPI_TX_DELAY);
    uapi_gpio_set_val(SPI_DCX_GPIO, GPIO_LEVEL_HIGH);

    qspi_disp_hal_enable(TD_FALSE);
    qspi_disp_hal_set_frame_cfg(MOTOROLA_SPI, STD_ENHFF, TRANSMIT_ONLY, DFS_32BIT, DEFAULT_CSF_LEN);
    qspi_disp_hal_set_ins_addr_cfg(NO_INS, ADDR_LEN_0BIT, STD_INS_CUSTOM_ADDR);
    qspi_disp_hal_set_data_cfg(NO_REVERSE, 0, TD_TRUE, TD_TRUE, 16); // 0 is wait cnt 16 is water line
    qspi_disp_hal_enable(TD_TRUE);
}
#endif

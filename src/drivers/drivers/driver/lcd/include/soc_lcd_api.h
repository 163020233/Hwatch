/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Header file of lcd driver.
 *
 * Create: 2022-03-05
 */

#ifndef __SOC_LCD_API_H__
#define __SOC_LCD_API_H__

#include "stdint.h"
#include "stdbool.h"
#include "soc_errno.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DATA_LENGTH 50

#ifdef GPU_FPGA_3321
#define DISPLAY_TE_GPIO  S_MGPIO11
#endif

#define LCD_CMP_OUT_WIDTH_ALIGN 12
#define LCD_CMP_OUT_HEIGHT_ALIGN 4
#define LCD_CMP_OUT_RATIO 3
#define LCD_NORMAL_BRIGHT_MAX 255

#define LCD_DEFAULT_FRAME_RATE  61
#define MCU_INT_TO_TE_TIME 1200 // 1.2ms

#ifdef GPU_1A21
#define WAKE_SCENE_MCU_INT_TO_TE_TIME 3000 // 3ms
#elif defined(GPU_3322)
#define WAKE_SCENE_MCU_INT_TO_TE_TIME 9000 // 9ms
#else
#define WAKE_SCENE_MCU_INT_TO_TE_TIME MCU_INT_TO_TE_TIME
#endif

typedef enum {
    LCD_DISPLAY_OFF,     /* LCD关闭显示 */
    LCD_NORMAL_DISPLAY,  /* LCD正常显示模式 */
    LCD_IDLE_MODE,       /* LCD空闲模式，显示效果变差，只显示8种颜色 */
    LCD_SLEEP_MODE,      /* LCD睡眠模式 */
} lcd_display_status;

typedef enum {
    LCD_TE_ON = 1,  /* LCD开启TE */
    LCD_TE_OFF,     /* LCD关闭TE */
} lcd_te_status;

typedef enum {
    LCD_BRIGHT_NORMAL = 0,  /* LCD正常亮度模式 */
    LCD_BRIGHT_HBM,         /* LCD高亮模式 */
} lcd_bright_status;

typedef struct {
    lcd_display_status disp_status;
    lcd_te_status te_status;
    lcd_bright_status bright_status;
} ext_lcd_status;

typedef struct lcd_cmd_sequ {
    uint8_t  dcs_flag; /* 当前数据是否通过DCS协议命令下发，为0表示不是DCS命令 */
    uint8_t  data_len; /* payload length, 小于等于64 */
    uint8_t  data[MAX_DATA_LENGTH]; /* data内包含cmd和参数，有效数据最短为1（例如0x11），最长为5 */
    uint16_t delay_ms; /* 发送命令后等待时间，单位ms */
} lcd_cmd_sequ;

typedef struct lcd_cmds_sequ {
    lcd_cmd_sequ *cmd; /* dsi命令数组 */
    uint16_t cmd_cnt; /* dsi命令个数 */
} lcd_cmds_sequ;

typedef struct {
    uint8_t reg_addr;  /* lcd寄存器地址 */
    uint16_t read_len; /* 读取寄存器数据长度 */
} ext_lcd_read_info;

typedef enum {
    BUS_DISPLAY_MIPI = 1,
    BUS_DISPLAY_QSPI,
    BUS_DISPLAY_SPI,

    BUS_DISPLAY_INVALID
} lcd_bus_type_t;

#define LANE_MAX_NUM            2

typedef enum {
    OUT_MODE_DSI_VIDEO      = 0x1,          /*!< @if Eng dsi video mode.
                                                 @else   dsi视频模式。 @endif */
    OUT_MODE_DSI_CMD        = 0x2,          /*!< @if Eng dsi command mode.
                                                 @else   dsi命令模式。。 @endif */
    OUT_MODE_BUTT
} out_mode_t;

typedef enum {
    MIPI_TX_WORK_MODE_HS     = 0x0,         /*!< @if Eng HS(High Speed) work mode.
                                                 @else   HS（高速）工作模式。@endif */
    MIPI_TX_WORK_MODE_LP     = 0x1,         /*!< @if Eng LP(Low Power) work mode.
                                                 @else   LP（低功耗）工作模式。@endif */
    MIPI_TX_WORK_MODE_BUTT
} mipi_tx_work_mode_t;

typedef enum {
    BURST_MODE                      = 0x0,
    NON_BURST_MODE_SYNC_PULSES      = 0x1,
    NON_BURST_MODE_SYNC_EVENTS      = 0x2,

    VIDEO_DATA_MODE_BUTT
} video_mode_t;

typedef enum {
    OUT_FORMAT_RGB_16BIT          = 0x0,
    OUT_FORMAT_RGB_18BIT          = 0x1,
    OUT_FORMAT_RGB_18BIT_LOOSELY  = 0x2,
    OUT_FORMAT_RGB_24BIT          = 0x3,
    OUT_FORMAT_YUV420_12BIT       = 0x4,
    OUT_FORMAT_YUV422_16BIT       = 0x5,
    OUT_FORMAT_RGB_30BIT          = 0x6,
    OUT_FORMAT_RGB_8BIT           = 0x7,
    OUT_FORMAT_RGB_12BIT          = 0x8,
    OUT_FORMAT_R_8BIT             = 0x9,
    OUT_FORMAT_G_8BIT             = 0x0A,
    OUT_FORMAT_B_8BIT             = 0x0B,
    OUT_FORMAT_R_4BIT             = 0x0C,
    OUT_FORMAT_G_4BIT             = 0x0D,
    OUT_FORMAT_B_4BIT             = 0x0E,
    OUT_FORMAT_BUTT
} out_format_t;

typedef struct {
    uint16_t  hsa_pixels;
    uint16_t  hbp_pixels;
    uint16_t  hact_pixels;
    uint16_t  hfp_pixels;

    uint16_t  vsa_lines;
    uint16_t  vbp_lines;
    uint16_t  vact_lines;
    uint16_t  vfp_lines;
} sync_info_t;

typedef struct {
    uint8_t   eotp_tx_en;
    uint8_t   eotp_rx_en;
} pck_en_t;

typedef enum {
    CLK_DISCONTINUE = 0,
    CLK_CONTINUE,
} clk_continue_t;

typedef struct {
    uint8_t         devno;                   /*!< @if Eng device number.
                                                 @else   设备编号。@endif */
    short           lane_id[LANE_MAX_NUM];   /*!< @if Eng lane_id: -1 - disable.
                                                 @else   车道ID:-1-禁用。@endif */
    out_mode_t      out_mode;                /*!< @if Eng output mode: DSI_VIDEO/DSI_CMD.
                                                 @else   输出模式：DSI_VIDEO/DSI_CMD。@endif */
    video_mode_t    video_mode;
    out_format_t    out_format;
    sync_info_t     sync_info;
    pck_en_t        pck_en;
    clk_continue_t  clk_continue;
    uint32_t        phy_data_rate;
    uint32_t        pixel_clk;
} combo_dev_cfg_t;

typedef struct {
    lcd_bus_type_t bus_type;
    combo_dev_cfg_t *bus_cfg;
    uint16_t x_start;
    uint16_t y_start;
    uint16_t fps;
    uint32_t lcd_id;
    uint16_t ddic_id;
    lcd_cmds_sequ *display_on_cmds;
    lcd_cmds_sequ *display_off_cmds;
    lcd_cmds_sequ *enter_idle_cmds;
    lcd_cmds_sequ *exit_idle_cmds;
    lcd_cmds_sequ *te_on_cmds;
    lcd_cmds_sequ *te_off_cmds;
    lcd_cmds_sequ *set_brightnes_cmds;
    lcd_cmds_sequ *hbm_brightness_cmds;
    lcd_cmds_sequ *sleep_in_cmds;
    lcd_cmds_sequ *sleep_out_cmds;
    ext_lcd_read_info lcd_id_info;
} lcd_drv_cfg;

typedef void (*qspi_send_frame_cb_t)(void);

typedef void (*mcu_int_callback)(void);

ext_errno uapi_lcd_info_register(lcd_drv_cfg *info);

ext_errno uapi_lcd_init(void);

ext_errno uapi_lcd_deinit(void);

ext_errno uapi_lcd_bus_init(combo_dev_cfg_t *bus_cfg);

ext_errno uapi_lcd_bus_deinit(void);

ext_errno uapi_lcd_bus_enable(void);

ext_errno uapi_lcd_bus_disable(void);

ext_lcd_status uapi_lcd_get_status(void);

bool uapi_is_lcd_connect(void);

ext_errno uapi_lcd_display_on(void);

ext_errno uapi_lcd_display_off(void);

ext_errno uapi_lcd_enter_sleep_mode(void);

ext_errno uapi_lcd_exit_sleep_mode(void);

ext_errno uapi_lcd_enter_idle_mode(void);

ext_errno uapi_lcd_exit_idle_mode(void);

ext_errno uapi_lcd_te_on(void);

ext_errno uapi_lcd_te_off(void);

ext_errno uapi_lcd_set_brightness(uint16_t value); // NORMAL模式：0-255  高亮模式：256-511

ext_errno uapi_lcd_get_brightness(uint16_t *value);

ext_errno uapi_lcd_write_reg(lcd_cmd_sequ *cmds, uint32_t cmd_count);

ext_errno uapi_lcd_read_reg(uint8_t cmd, uint8_t *buff, uint8_t buff_len);

ext_errno uapi_lcd_suspend(void);

ext_errno uapi_lcd_resume(void);

ext_errno uapi_lcd_enter_sdlp(void);

ext_errno uapi_lcd_exit_sdlp(void);

ext_errno uapi_lcd_exit_ulps(void);

ext_errno uapi_lcd_force_exit_ulps(void);

ext_errno uapi_lcd_clear_exit_ulps(void);

ext_errno uapi_lcd_set_auto_exit_ulps_en(bool enable);

ext_errno uapi_lcd_ulps_entry_suspend(bool enable);

ext_errno uapi_lcd_ulps_proc(void);

ext_errno uapi_lcd_detect_pld_errs(bool *is_err);

ext_errno uapi_lcd_dfx_proc_read(void);

bool lcd_get_status(void);

void lcd_set_status(bool lcd_on);

void uapi_lcd_register_bus(lcd_bus_type_t bus_type);

/* 设置mcu int中断上报周期(单位：TE周期)。 */
ext_errno uapi_lcd_set_mcu_int_cycle_num(uint32_t cycle_num);

ext_errno uapi_lcd_register_mcu_int_callback(mcu_int_callback callback);

void uapi_lcd_set_mcuint2te_time(uint32_t mcuint2te_time_us);

ext_errno uapi_lcd_exit_ulps_internal(void);

ext_errno uapi_lcd_force_exit_ulps_internal(void);

ext_errno uapi_lcd_clear_exit_ulps_internal(void);

ext_errno uapi_lcd_frame_done_process(void);

ext_errno uapi_lcd_ulps_entry_suspend_internal(bool enable);
bool uapi_lcd_get_ulps_entry_suspend_state(void);

bool uapi_lcd_get_refresh_flag(void);

void uapi_lcd_set_refresh_flag(bool refresh);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif
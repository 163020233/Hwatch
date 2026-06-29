/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Lcd driver configuration.
 *
 * Create: 2022-03-05
 */

#include "lcd_config.h"
#include <stdint.h>
#include "common_def.h"
#include "lcd_dfx.h"

#define LCD_EDO_466X466_SCREEN_ID1  0x65
#define LCD_EDO_466X466_SCREEN_ID2  0x66
#define LCD_EDO_454X454_SCREEN_ID  0xf00
#define LCD_BOE_454X454_SCREEN_ID  0x500
#define LCD_CO5300_1V8_SCREEN_ID  0X0178
#define LCD_408x480_SCREEN_ID  0X5A00
#define LCD_320x360_SCREEN_ID  0X43 /* 物料23021172 */
#define LCD_320x360_EDO_SCREEN_ID  0X53 /* 物料23021479 */
#define LCD_286X482_SCREEN_ID 0x7A
#define LCD_240X240_SCREEN_ID 0x85
#define LCD_480x854_SCREEN_ID  0xffffff
#define LCD_800x480_SCREEN_ID  0x490670
#define LCD_GH7006_800x480_SCREEN_ID 0x110670
#define LCD_ST7102_480X854_SCREEN_ID 0xfba080
#define LCD_GC9307_SPI_SCREEN_ID  0x834900
#define LCD_axs15231_320x480_SCREEN_ID 0x40
#define LCD_CO5300_QSPI_SCREEN_ID  0x1133
#define LCD_CO5300_MIPI_SCREEN_ID  0x1133
#define LCD_GH7002_1024X6000_SCREEN_ID 0xa0270


#ifdef PRE_FPGA
#define TYPICAL_PHY_DATA_RATE  416      /* Mbps */
#define TYPICAL_PIXEL_CLK 15000       /* KHz */
#else
#define TYPICAL_PHY_DATA_RATE 480     /* Mbps */
#define TYPICAL_PIXEL_CLK 20000        /* KHz */
#endif

static combo_dev_cfg_t g_panel_466x466_60_config = {
    .devno = 0,
    .lane_id = {0, -1},
    .out_mode = OUT_MODE_DSI_CMD,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .sync_info = {
        .hsa_pixels   = 10,
        .hbp_pixels   = 10,
        .hact_pixels  = 466,
        .hfp_pixels   = 14,
        .vsa_lines    = 4,
        .vbp_lines    = 10,
        .vact_lines   = 466,
        .vfp_lines    = 4,
    },
    .phy_data_rate = TYPICAL_PHY_DATA_RATE,
    .pixel_clk = TYPICAL_PIXEL_CLK,
};

static combo_dev_cfg_t g_panel_454x454_60_config = {
    .devno = 0,
    .lane_id = {0, -1},
    .out_mode = OUT_MODE_DSI_CMD,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .sync_info = {
        .hsa_pixels   = 10,
        .hbp_pixels   = 10,
        .hact_pixels  = 454,
        .hfp_pixels   = 10,
        .vsa_lines    = 4,
        .vbp_lines    = 12,
        .vact_lines   = 454,
        .vfp_lines    = 10,
    },
    .phy_data_rate = TYPICAL_PHY_DATA_RATE,     // Mbps
    .pixel_clk = TYPICAL_PIXEL_CLK,       // KHz
};

#ifdef GPU_SUPPORT_VIDEO_MODE
static combo_dev_cfg_t g_panel_466x466_60_video_config = {
    .devno = 0,
    .lane_id = {0, -1},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .sync_info = {
        .hsa_pixels   = 10,
        .hbp_pixels   = 14,
        .hact_pixels  = 466,
        .hfp_pixels   = 10,
        .vsa_lines    = 4,
        .vbp_lines    = 10,
        .vact_lines   = 466,
        .vfp_lines    = 20,
    },
    .phy_data_rate = 416, // Mbps  phy data rate must be aligned to 32
    .pixel_clk = 15000,       // KHz
};
#endif

static combo_dev_cfg_t g_panel_320x360_60_config = {
    .devno = 0,
    .lane_id = {0, -1},
    .out_mode = OUT_MODE_DSI_CMD,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .sync_info = {
        .hsa_pixels   = 10,
        .hbp_pixels   = 10,
        .hact_pixels  = 320,
        .hfp_pixels   = 14,
        .vsa_lines    = 4,
        .vbp_lines    = 10,
        .vact_lines   = 360,
        .vfp_lines    = 4,
    },
    .phy_data_rate = TYPICAL_PHY_DATA_RATE,     // Mbps
    .pixel_clk = TYPICAL_PIXEL_CLK, // KHz
};

static combo_dev_cfg_t g_panel_408x480_60_config = {
    .devno = 0,
    .lane_id = {0, -1},
    .out_mode = OUT_MODE_DSI_CMD,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .sync_info = {
        .hsa_pixels   = 10,
        .hbp_pixels   = 10,
        .hact_pixels  = 408,
        .hfp_pixels   = 14,
        .vsa_lines    = 4,
        .vbp_lines    = 10,
        .vact_lines   = 480,
        .vfp_lines    = 4,
    },
    .phy_data_rate = TYPICAL_PHY_DATA_RATE,     // Mbps
    .pixel_clk = TYPICAL_PIXEL_CLK,       // KHz
};

static combo_dev_cfg_t g_panel_st7701_480x854_config = {
    .devno = 0,
    .lane_id = {0, -1},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .sync_info = {
        .hsa_pixels   = 10,
        .hbp_pixels   = 30,
        .hact_pixels  = 480,
        .hfp_pixels   = 30,
        .vsa_lines    = 10,
        .vbp_lines    = 20,
        .vact_lines   = 854,
        .vfp_lines    = 20,
    },
    .phy_data_rate = 728,     // Mbps
    .pixel_clk = 30000,       // KHz
};

static combo_dev_cfg_t g_panel_axs15231_320x480_config = {
    .devno = 0,
    .lane_id = {0, -1},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .sync_info = {
        .hsa_pixels   = 38,
        .hbp_pixels   = 160,
        .hact_pixels  = 320,
        .hfp_pixels   = 168,
        .vsa_lines    = 50,
        .vbp_lines    = 20,
        .vact_lines   = 480,
        .vfp_lines    = 50,
    },
    .phy_data_rate = 608,     // Mbps
    .pixel_clk = 25000,       // KHz
};

#ifdef GPU_3322
static combo_dev_cfg_t g_panel_st7701_800x480_config = {
    .devno = 0,
    .lane_id = {0, -1},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_16BIT,
    .sync_info = {
        .hsa_pixels   = 20,
        .hbp_pixels   = 80,
        .hact_pixels  = 800,
        .hfp_pixels   = 90,
        .vsa_lines    = 8,
        .vbp_lines    = 20,
        .vact_lines   = 480,
        .vfp_lines    = 20,
    },
    .phy_data_rate = 512,     // Mbps
    .pixel_clk = 31000,       // KHz
};

static combo_dev_cfg_t g_panel_gh7006_800x480_config = {
    .devno = 0,
    .lane_id = {0, -1},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_16BIT,
    .sync_info = {
        .hsa_pixels   = 20,
        .hbp_pixels   = 30,
        .hact_pixels  = 800,
        .hfp_pixels   = 50,
        .vsa_lines    = 2,
        .vbp_lines    = 8,
        .vact_lines   = 480,
        .vfp_lines    = 8,
    },
    .phy_data_rate = 448,     // Mbps
    .pixel_clk = 27000,       // KHz
};

static combo_dev_cfg_t g_panel_st7102_480x854_config = {
    .devno = 0,
    .lane_id = {0, -1},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .sync_info = {
        .hsa_pixels   = 2,
        .hbp_pixels   = 40,
        .hact_pixels  = 480,
        .hfp_pixels   = 40,
        .vsa_lines    = 2,
        .vbp_lines    = 9,
        .vact_lines   = 854,
        .vfp_lines    = 193,
    },
    .phy_data_rate = 728,     // Mbps
    .pixel_clk = 30000,       // KHz
};

static combo_dev_cfg_t g_panel_GH7002_1024X600_video_config = {
    .devno = 0,
    .lane_id = {0, -1},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_16BIT,
    .clk_continue = CLK_CONTINUE,
    .sync_info =
        {
            .hsa_pixels = 80,
            .hbp_pixels = 80,
            .hact_pixels = 1024,
            .hfp_pixels = 90,
            .vsa_lines = 8,
            .vbp_lines = 8,
            .vact_lines = 600,
            .vfp_lines = 16,
        },
    .phy_data_rate = 784,  // Mbps
    .pixel_clk = 48600,    // KHz
};
#endif

static combo_dev_cfg_t g_panel_466x466_qspi_config = {
    .out_format = OUT_FORMAT_RGB_24BIT,
    .sync_info = {
        .hsa_pixels   = 1,
        .hbp_pixels   = 2,
        .hact_pixels  = 466,
        .hfp_pixels   = 1,
        .vsa_lines    = 4,
        .vbp_lines    = 16,
        .vact_lines   = 466,
        .vfp_lines    = 20,
    },
    .phy_data_rate = 50,     // Mbps
    .pixel_clk = 26000,       // KHz
};

static combo_dev_cfg_t g_panel_286x482_qspi_config = {
    .out_format = OUT_FORMAT_RGB_24BIT,
    .sync_info = {
        .hsa_pixels   = 1,
        .hbp_pixels   = 10,
        .hact_pixels  = 286,
        .hfp_pixels   = 12,
        .vsa_lines    = 4,
        .vbp_lines    = 10,
        .vact_lines   = 482,
        .vfp_lines    = 20,
    },
    .phy_data_rate = 50,     // Mbps
    .pixel_clk = 26000,       // KHz
};

static combo_dev_cfg_t g_panel_240x240_qspi_config = {
    .out_format = OUT_FORMAT_RGB_16BIT,
    .sync_info = {
        .hsa_pixels   = 1,
        .hbp_pixels   = 10,
        .hact_pixels  = 240,
        .hfp_pixels   = 12,
        .vsa_lines    = 4,
        .vbp_lines    = 10,
        .vact_lines   = 240,
        .vfp_lines    = 20,
    },
    .phy_data_rate = 50,     // Mbps
    .pixel_clk = 26000,       // KHz
};

static combo_dev_cfg_t g_panel_gc9307_240x284_spi_config = {
    .out_format = OUT_FORMAT_RGB_16BIT,
    .sync_info = {
        .hsa_pixels   = 1,
        .hbp_pixels   = 10,
        .hact_pixels  = 240,
        .hfp_pixels   = 12,
        .vsa_lines    = 4,
        .vbp_lines    = 10,
        .vact_lines   = 284,
        .vfp_lines    = 20,
    },
    .phy_data_rate = 50,     // Mbps
    .pixel_clk = 26000,       // KHz
};

static combo_dev_cfg_t g_panel_466x466_v1_config = {
    .devno = 0,
    .lane_id = {0, -1},
    .out_mode = OUT_MODE_DSI_CMD,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .sync_info = {
        .hsa_pixels   = 10,
        .hbp_pixels   = 40,
        .hact_pixels  = 466,
        .hfp_pixels   = 20,
        .vsa_lines    = 2,
        .vbp_lines    = 7,
        .vact_lines   = 466,
        .vfp_lines    = 9,
    },
    .phy_data_rate = 480,     // Mbps
    .pixel_clk = 20000,       // KHz
};


/* display off sequence for common lcd */
const static lcd_cmd_sequ g_common_cmd_display_off[] = {
    {1, 2, {0xfe, 0x00}, 0},   /* select default cmd page */
    {1, 1, {0x28}, 0},         /* display off */
    {1, 1, {0x10}, 120},       /* sleep in */
};

/* sleep in sequence for common lcd */
const static lcd_cmd_sequ g_common_cmd_sleep_in[] = {
    {1, 2, {0xfe, 0x00}, 0},   /* select default cmd page */
    {1, 1, {0x28}, 0},         /* display off */
    {1, 1, {0x10}, 120},       /* sleep in */
};

/* sleep in sequence for common lcd */
const static lcd_cmd_sequ g_common_cmd_sleep_out[] = {
    {1, 2, {0xfe, 0x00}, 0},   /* select default cmd page */
    {1, 1, {0x11}, 120},       /* sleep out */
    {1, 1, {0x29}, 0},         /* display on */
};

/* idle in sequence for EDO and BOE 454*454 lcd */
const static lcd_cmd_sequ g_common_cmd_idle_in[] = {
    {1, 2, {0xfe, 0x00}, 0},
    {1, 1, {0x39}, 0},
};

/* idle out sequence for EDO and BOE 454*454 lcd */
const static lcd_cmd_sequ g_common_cmd_idle_out[] = {
    {1, 2, {0xfe, 0x00}, 0},
    {1, 1, {0x38}, 0},
};

/* te on sequence for EDO and BOE 454*454 lcd */
const static lcd_cmd_sequ g_common_cmd_te_on[] = {
    {1, 2, {0xfe, 0x00}, 0},
    {1, 1, {0x35}, 0},
};

/* te off sequence for EDO and BOE 454*454 lcd */
const static lcd_cmd_sequ g_common_cmd_te_off[] = {
    {1, 2, {0xfe, 0x00}, 0},
    {1, 1, {0x34}, 0},
};

/* init sequence for EDO 466*466 lcd */
const static lcd_cmd_sequ g_edo_466x466_screen_init_on[] = {
    { 1, 2, { 0xFE, 0x82 }, 0 },
    { 1, 2, { 0x52, 0x01 }, 0 },
    { 1, 2, { 0xFE, 0x00 }, 0 },
    { 1, 2, { 0x35, 0x00 }, 0 },
    { 1, 2, { 0x53, 0x20 }, 0 },
    { 1, 2, { 0x51, 0xFF }, 0 },
    { 1, 5, { 0x44, 0x01, 0x4D, 0x00, 0x00 }, 0 },
    { 0, 5, { 0x2A, 0x00, 0x06, 0x01, 0xD7 }, 0 },
    { 0, 5, { 0x2B, 0x00, 0x00, 0x01, 0xD1 }, 0 },
    { 1, 1, { 0x11 }, 60 },
    { 1, 1, { 0x29 }, 0 },

};

/* set brightness sequence for EDO and BOE 454*454 lcd */
static lcd_cmd_sequ g_common_cmd_brightness[] = {
    {1, 2, {0xfe, 0x00}, 0},
    {1, 2, {0x66, 0x00}, 0},
    {1, 2, {0x51, 0xff}, 0},
};

static lcd_cmd_sequ g_common_cmd_hbm_brightness[] = {
    {1, 2, {0xfe, 0x00}, 0},
    {1, 2, {0x66, 0x02}, 0},
    {1, 2, {0x63, 0x00}, 0},
};

/* init sequence for EDO 454*454 lcd */
const static lcd_cmd_sequ g_edo_454x454_screen_init_on[] = {
    {1, 2, {0xfe, 0x01}, 0},
    {1, 2, {0x30, 0x43}, 0}, /* set LCD frequency to 15HZ */
    {1, 2, {0xfe, 0x00}, 0},
    {1, 2, {0x3a, 0x77}, 0},
    {1, 2, {0x53, 0x20}, 0},
    {1, 2, {0x51, 0xff}, 0},
    {0, 5, {0x2a, 0x00, 0x0e, 0x01, 0xd3}, 0},
    {0, 5, {0x2b, 0x00, 0x00, 0x01, 0xc5}, 0},
    {1, 3, {0x44, 0x01, 0xc5}, 0},
    {1, 2, {0x35, 0x00}, 0},
    {1, 1, {0x11}, 120},
    {1, 1, {0x29}, 0},
};

/* init sequence for BOE 454*454 lcd */
const static lcd_cmd_sequ g_boe_454x454_screen_init_on[] = {
    {1, 2, {0xfe, 0x0d}, 0},
    {1, 2, {0x01, 0x00}, 0},
    {1, 2, {0x03, 0x00}, 0},
    {1, 2, {0x1b, 0x00}, 0},
    {1, 2, {0x1d, 0x00}, 0},
    {1, 2, {0x37, 0x00}, 0},
    {1, 2, {0x39, 0x00}, 0},
    {1, 2, {0x52, 0x58}, 0},
    {1, 2, {0x53, 0x00}, 0},

    {1, 2, {0xfe, 0x01}, 0},
    {1, 2, {0x72, 0x05}, 0},
    {1, 2, {0x73, 0x16}, 0},
    {1, 2, {0x74, 0x0c}, 0},

    {1, 2, {0xfe, 0x01}, 0},
    {1, 2, {0x18, 0x55}, 0},
    {1, 2, {0xc5, 0x10}, 0},
    {1, 2, {0x72, 0x0e}, 0},
    {1, 2, {0x73, 0x1c}, 0},

    {1, 2, {0xfe, 0x07}, 0},
    {1, 2, {0x15, 0x04}, 0},
    {1, 2, {0xfe, 0x01}, 0},
    {1, 2, {0x0a, 0xf8}, 0},

    {1, 2, {0xfe, 0x00}, 0},
    {1, 2, {0x51, 0xff}, 0},
    {1, 2, {0x35, 0x00}, 0},
    {0, 5, {0x2a, 0x00, 0x0c, 0x01, 0xd1}, 0},
    {0, 5, {0x2b, 0x00, 0x00, 0x01, 0xc5}, 0},
    {1, 2, {0x53, 0x20}, 0},
    {1, 1, {0x11}, 120},
    {1, 1, {0x29}, 0},
};

const static lcd_cmd_sequ g_co5300_1v8_screen_init_on[] = {
    {0, 1, {0x11}, 100},
    {0, 5, {0x2a, 0x00, 0x00, 0x01, 0xd1}, 0},
    {0, 5, {0x2b, 0x00, 0x00, 0x01, 0xd1}, 0},
    {0, 2, {0x53, 0x20}, 0},
    {0, 2, {0x51, 0xff}, 0},
    {0, 2, {0x3a, 0x77}, 0},
    {0, 2, {0x35, 0x00}, 100},
    {0, 1, {0x29}, 100},
};

const static lcd_cmd_sequ g_co5300_3v3_screen_init_on[] = {
    {0, 2, {0xfe, 0x00}, 0},
    {0, 2, {0xc4, 0x80}, 0},
    {0, 2, {0x3a, 0x77}, 0},
    {0, 2, {0x36, 0x00}, 0},
    {0, 2, {0x35, 0x00}, 0},
    {0, 2, {0x53, 0x20}, 0},
    {0, 2, {0x51, 0x00}, 0},
    {0, 2, {0x63, 0xff}, 0},
    {0, 5, {0x2a, 0x00, 0x06, 0x01, 0xd7}, 0},
    {0, 5, {0x2b, 0x00, 0x00, 0x01, 0xd1}, 0},
    {0, 1, {0x11}, 80},
    {0, 1, {0x29}, 100},
};

const static lcd_cmd_sequ g_408x480_screen_init_on[] = {
    {1, 2, {0xfe, 0x00}, 0},
    {1, 2, {0x67, 0x02}, 0}, /* set LCD frequency to 15HZ */
    {1, 2, {0x68, 0x20}, 0},
    {1, 2, {0xfe, 0x00}, 0},
    {0, 5, {0x2a, 0x00, 0x00, 0x01, 0x97}, 0},
    {0, 5, {0x2b, 0x00, 0x00, 0x01, 0xdf}, 0},
    {1, 2, {0x35, 0x00}, 0},
    {1, 2, {0x53, 0x20}, 0},
    {1, 2, {0x51, 0xff}, 0},
    {1, 2, {0x63, 0xff}, 0},
    {1, 1, {0x11}, 60},
    {1, 1, {0x29}, 0},
};

const static lcd_cmd_sequ g_320x360_screen_init_on[] = {
    {1, 1, {0x11}, 5},
    {1, 5, {0x2a, 0x00, 0x00, 0x01, 0x3f}, 0},
    {1, 5, {0x2b, 0x00, 0x00, 0x01, 0x67}, 0},
    {1, 2, {0x35, 0x0}, 0}, /* te on */
    {1, 2, {0x53, 0x20}, 100},
    {1, 2, {0x51, 0xff}, 0},
    {1, 1, {0x29}, 100},
};

const static lcd_cmd_sequ g_320x360_edo_screen_init_on[] = {
    {1, 1, {0x11}, 100},
    {1, 3, {0xc0, 0x5a, 0x5a}, 0},
    {1, 3, {0xc1, 0x5a, 0x5a}, 0},
    {1, 2, {0xb1, 0x05}, 0},
    {1, 5, {0x2a, 0x00, 0x00, 0x01, 0x3f}, 0},
    {1, 5, {0x2b, 0x00, 0x00, 0x01, 0x67}, 0},
    {1, 3, {0x44, 0x01, 0x67}, 0}, /* te scan line */
    {1, 2, {0x35, 0x0}, 0}, /* te on */
    {1, 2, {0x53, 0x20}, 60},

    {1, 1, {0x29}, 100},
};

const static lcd_cmd_sequ g_286x482_screen_init_on[] = {
    {0, 2, {0xFE, 0x00}, 0 },
    {0, 2, {0x3A, 0x77}, 0 },
    {0, 2, {0x67, 0x02}, 0 },
    {0, 2, {0x68, 0x20}, 0 },
    {0, 2, {0x35, 0x00}, 0 },
    {0, 2, {0x53, 0x20}, 0 },
    {0, 2, {0x51, 0xFF}, 0 },
    {0, 2, {0x63, 0xFF}, 0 },
    {0, 5, {0x2A, 0x00, 0x00, 0x01, 0x1D}, 0 },
    {0, 5, {0x2B, 0x00, 0x00, 0x01, 0xE1}, 0 },
    {0, 3, {0x44, 0x01, 0xB6}, 0 },
    {0, 1, {0x11}, 60 },
    {0, 1, {0x29}, 0 },
};

const static lcd_cmd_sequ g_240x240_screen_init_on[] = {
    {0, 1, {0x11}, 120 },
    {0, 2, {0x36, 0x00}, 0 },
    {0, 2, {0x3A, 0x05}, 0 },
    {0, 3, {0xB0, 0x00, 0xD0}, 0 },
    {0, 6, {0xB2, 0x36, 0x36, 0x00, 0x33, 0x33}, 0 },
    {0, 2, {0xB7, 0x04}, 0 },
    {0, 2, {0xBB, 0x1D}, 0 },
    {0, 2, {0xC0, 0x2C}, 0 },
    {0, 2, {0xC2, 0x01}, 0 },
    {0, 2, {0xC3, 0x19}, 0 },
    {0, 2, {0xC4, 0x20}, 0 },
    {0, 2, {0xC6, 0xEF}, 0 },
    {0, 3, {0xD0, 0xA4, 0xA1}, 0 },
    {0, 2, {0xD6, 0xA5}, 0 },
    {0, 15, {0xE0, 0xF0, 0x1A, 0x1E, 0x09, 0x08, 0x26, 0x3B, 0x33, 0x4E, 0x36, 0x12, 0x12, 0x2C, 0x31}, 0 },
    {0, 15, {0xE1, 0xF0, 0x1C, 0x1F, 0x0B, 0x0A, 0x04, 0x3B, 0x32, 0x4D, 0x38, 0x14, 0x14, 0x2E, 0x32}, 0 },
    {0, 4, {0xE4, 0x1D, 0x00, 0x00}, 0 },
    {0, 2, {0x35, 0x00}, 0 },
    {0, 3, {0x44, 0x00, 0x60}, 0 },
    {0, 1, {0x21}, 0 },
    {0, 1, {0x29}, 0 },
    {0, 5, {0x2A, 0x00, 0x00, 0x00, 0xEF}, 0 },
    {0, 5, {0x2B, 0x00, 0x00, 0x00, 0xEF}, 0 },
    {0, 1, {0x2C}, 0 },
    {0, 1, {0x29}, 0 },
};

const static lcd_cmd_sequ g_st7701_480x854_init_on[] = {
    {0, 6, {0xff, 0x77, 0x1, 0x0, 0x0, 0x13}, 0},
    {0, 2, {0xef, 0x8}, 0},
    {0, 6, {0xff, 0x77, 0x1, 0x0, 0x0, 0x10}, 0},
    {0, 3, {0xc0, 0xe9, 0x3}, 0},
    {0, 3, {0xc1, 0xa, 0x2}, 0},
    {0, 3, {0xc2, 0x7, 0x6}, 0},
    {0, 2, {0xc7, 0x0}, 0},
    {0, 2, {0xcc, 0x18}, 0},
    {0, 17, {0xb0, 0x0, 0xb, 0x10, 0xd, 0x11, 0x6, 0x1, 0x8, 0x8, 0x1d, 0x4, 0x10, 0x10, 0x27, 0x30, 0x19}, 0},
    {0, 17, {0xb1, 0x0, 0xb, 0x14, 0xc, 0x11, 0x5, 0x3, 0x8, 0x8, 0x20, 0x4, 0x13, 0x10, 0x28, 0x30, 0x19}, 0},
    {0, 6, {0xff, 0x77, 0x1, 0x0, 0x0, 0x11}, 0},
    {0, 2, {0xb0, 0x35}, 0},
    {0, 2, {0xb1, 0x24}, 0},
    {0, 2, {0xb2, 0x87}, 0},
    {0, 2, {0xb3, 0x80}, 0},
    {0, 2, {0xb5, 0x4e}, 0},
    {0, 2, {0xb7, 0x85}, 0},
    {0, 2, {0xb8, 0x10}, 0},
    {0, 2, {0xb9, 0x10}, 0},
    {0, 2, {0xbc, 0x3}, 0},
    {0, 2, {0xc0, 0x89}, 0},
    {0, 2, {0xc1, 0x78}, 0},
    {0, 2, {0xc2, 0x78}, 0},
    {0, 2, {0xd0, 0x88}, 0},
    {0, 4, {0xe0, 0x0, 0x0, 0x2}, 0},
    {0, 12, {0xe1, 0x4, 0x0, 0x0, 0x0, 0x5, 0x0, 0x0, 0x0, 0x0, 0x20, 0x20}, 0},
    {0, 14, {0xe2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, 0},
    {0, 5, {0xe3, 0x0, 0x0, 0x33, 0x0}, 0},
    {0, 3, {0xe4, 0x22, 0x0}, 0},
    {0, 17, {0xe5, 0x4, 0x5c, 0xa0, 0xb8, 0x6, 0x5c, 0xa0, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, 0},
    {0, 5, {0xe6, 0x0, 0x0, 0x33, 0x0}, 0},
    {0, 3, {0xe7, 0x22, 0x0}, 0},
    {0, 17, {0xe8, 0x5, 0x5c, 0xa0, 0xb8, 0x7, 0x5c, 0xa0, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, 0},
    {0, 8, {0xeb, 0x2, 0x0, 0x40, 0x40, 0x0, 0x0, 0x0}, 0},
    {0, 3, {0xec, 0x0, 0x0}, 0},
    {0, 17, {0xed, 0xaa, 0x45, 0xb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb0, 0x54, 0xaa}, 0},
    {0, 7, {0xef, 0x8, 0x8, 0x8, 0x45, 0x3f, 0x54}, 0},
    {0, 6, {0xff, 0x77, 0x1, 0x0, 0x0, 0x13}, 0},
    {0, 3, {0xe8, 0x0, 0xe}, 0},
    {1, 1, {0x11}, 120},
    {0, 6, {0xff, 0x77, 0x1, 0x0, 0x0, 0x13}, 0},
    {0, 3, {0xe8, 0x0, 0xc}, 10},
    {0, 3, {0xe8, 0x0, 0x0}, 0},
    {0, 3, {0xe6, 0x16, 0x7c}, 0},
    {0, 2, {0x36, 0x0}, 0},
    {1, 1, {0x29}, 20},
};
const static lcd_cmd_sequ g_axs15231_320x480_init_on[] = {
    {0, 9, {0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0xa5}, 0},
    {0, 18,
     {0xA0, 0x00, 0x10, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     10},
    {0, 32,
     {0xA2, 0x20, 0x32, 0x36, 0x18, 0xc8, 0x88, 0xf8, 0xe0, 0x20, 0x10, 0x80, 0x80, 0x80, 0x20, 0xf8,
      0x10, 0x02, 0x00, 0x10, 0xf0, 0x90, 0x01, 0x32, 0xa0, 0x91, 0xc0, 0x20, 0x7f, 0xff, 0x00, 0x5a},
     10},
    {0, 28,
     {0xA3, 0xa0, 0x06, 0xaa, 0x00, 0x0a, 0xaa, 0x2a, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
      0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x55, 0x55, 0xaa, 0x00, 0x00, 0x00, 0x40, 0x01},
     10},
    {0, 31,
     {0xC1, 0x31, 0x04, 0x02, 0x02, 0x71, 0x05, 0x24, 0x55, 0x02, 0x00, 0x41, 0x00, 0x53, 0xff, 0xff,
      0xff, 0x4f, 0x52, 0x00, 0x4f, 0x52, 0x00, 0x4d, 0x3b, 0x0b, 0x02, 0x0d, 0x00, 0xff, 0x40},
     0},
    {0, 12,
     {0xC3, 0x00, 0x00, 0x00, 0x50, 0x03, 0x00, 0x00, 0x00, 0x01, 0x80, 0x03}, 0},
    {0, 21,
     {0xC4, 0x24, 0x37, 0x64, 0x32, 0x14, 0x64, 0xc8, 0xfa, 0x02, 0x90,
      0x01, 0x1e, 0xdc, 0xfa, 0x00, 0x00, 0x44, 0x50, 0x99, 0x00},
     0},
    {0, 24,
     {0xC5, 0x18, 0x00, 0x80, 0x03, 0xfe, 0x03, 0x18, 0x20, 0x00, 0x00, 0x88,
      0xde, 0x0d, 0x08, 0x0f, 0x0f, 0x01, 0x03, 0x18, 0x20, 0x00, 0x00, 0x00},
     0},
    {0, 21,
     {0xC6, 0x05, 0x0a, 0x05, 0x0a, 0x3f, 0xe0, 0x2e, 0x0b, 0x12, 0x22,
      0x12, 0x42, 0x0f, 0x03, 0x00, 0x3f, 0x6a, 0x18, 0xc8, 0x22},
     0},
    {0, 21,
     {0xC7, 0x50, 0x32, 0x28, 0x00, 0xa2, 0x80, 0x8f, 0x00, 0x80, 0xff,
      0x07, 0x11, 0x9f, 0x67, 0xff, 0x24, 0x0c, 0x0d, 0x0e, 0x0f},
     0},
    {0, 11, {0xC8, 0x80, 0x32, 0xea, 0x80, 0xfe, 0x10, 0x10, 0x00, 0x0a, 0x0a}, 0},
    {0, 5, {0xC9, 0x33, 0x44, 0x44, 0x01}, 0},
    {0, 30,
     {0xCF, 0x00, 0x00, 0x88, 0x58, 0x10, 0x18, 0x56, 0x18, 0x1e, 0x08, 0x71, 0x69, 0xda, 0x20,
      0x28, 0x0c, 0x77, 0x22, 0x44, 0x03, 0x03, 0x32, 0xa0, 0xc8, 0x98, 0x0c, 0xb6, 0x3f, 0x00},
     0},
    {0, 31,
     {0xD0, 0xe0, 0x40, 0x11, 0x24, 0x08, 0x05, 0x10, 0x0d, 0x22, 0x15, 0xc2, 0x42, 0x62, 0x22, 0xaa,
      0x03, 0x10, 0x12, 0x60, 0x14, 0x1e, 0x51, 0x15, 0x00, 0x44, 0x20, 0x00, 0x03, 0x0d, 0x12},
     0},
    {0, 31,
     {0xD5, 0x10, 0x60, 0x83, 0x80, 0x35, 0x03, 0x86, 0x56, 0x04, 0x87, 0x87, 0x00, 0x04, 0x28, 0xd3,
      0x42, 0x03, 0x03, 0x03, 0x03, 0x86, 0x00, 0x00, 0x00, 0xe0, 0x51, 0x8d, 0x00, 0x00, 0x00},
     0},
    {0, 31,
     {0xD6, 0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe, 0x95, 0x00, 0x01, 0x01, 0x87, 0x87, 0x00,
      0x75, 0x36, 0x20, 0x40, 0x40, 0x03, 0x03, 0x10, 0x10, 0x00, 0x83, 0x51, 0x22, 0x01, 0x00},
     0},
    {0, 27,
     {0xD7, 0x00, 0x18, 0x1f, 0x08, 0x0a, 0x0c, 0x0e, 0x06, 0x1f, 0x1e, 0x1f, 0x1f, 0x10,
      0x60, 0x04, 0x00, 0x12, 0x38, 0x1f, 0x01, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00},
     0},
    {0, 13, {0xD8, 0x01, 0x18, 0x1f, 0x09, 0x0b, 0x0d, 0x0f, 0x07, 0x1f, 0x1e, 0x1f, 0x1f}, 0},
    {0, 13, {0xD9, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f}, 0},
    {0, 13, {0xDD, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f}, 0},
    {0, 9, {0xDF, 0x44, 0x03, 0x4b, 0x00, 0x00, 0x8b, 0x42, 0x22}, 0},
    {0, 18,
     {0xE0, 0x25, 0x00, 0x05, 0x09, 0x0a, 0x07, 0x12, 0x19, 0x44, 0x13, 0x0d, 0x37, 0x15, 0x20, 0x24, 0x0f, 0x03},
     0},
    {0, 18,
     {0xE1, 0x2b, 0x00, 0x05, 0x09, 0x0a, 0x07, 0x12, 0x18, 0x44, 0x13, 0x0d, 0x37, 0x15, 0x20, 0x24, 0x0f, 0x0f},
     0},
    {0, 18,
     {0xE2, 0x3b, 0x07, 0x12, 0x18, 0x0e, 0x0d, 0x17, 0x35, 0x44, 0x32, 0x0c, 0x14, 0x14, 0x36, 0x3a, 0x2f, 0x0d},
     0},
    {0, 18,
     {0xE3, 0x37, 0x07, 0x12, 0x18, 0x0e, 0x0d, 0x17, 0x35, 0x44, 0x32, 0x0c, 0x14, 0x14, 0x36, 0x32, 0x2f, 0x0f},
     0},
    {0, 18,
     {0xE4, 0x3b, 0x07, 0x12, 0x18, 0x0e, 0x0d, 0x17, 0x39, 0x44, 0x2e, 0x0c, 0x14, 0x14, 0x36, 0x3a, 0x2f, 0x0d},
     0},
    {0, 18,
     {0xE5, 0x37, 0x07, 0x12, 0x18, 0x0e, 0x0d, 0x17, 0x39, 0x44, 0x2e, 0x0c, 0x14, 0x14, 0x36, 0x3a, 0x2f, 0x0f},
     0},
    {0, 9, {0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0},
    {1, 2, {0x3a, 0x77}, 5},
    {1, 1, {0x11}, 100},
    {1, 1, {0x29}, 20},
};

#ifdef GPU_3322
const static lcd_cmd_sequ g_st7102_480x854_init_on[] = {
    {0, 4, {0x99, 0x71, 0x2, 0xa2}, 0},
    {0, 4, {0x99, 0x71, 0x2, 0xa3}, 0},
    {0, 4, {0x99, 0x71, 0x2, 0xa4}, 0},
    {0, 8, {0xb0, 0x22, 0x6b, 0x1e, 0x75, 0x2f, 0x39, 0x43}, 0},
    {0, 3, {0xb7, 0x7d, 0x7d}, 0},
    {0, 3, {0xbf, 0x80, 0x80}, 0},
    {0, 9, {0xba, 0x72, 0x76, 0x52, 0xd6, 0xc0, 0x20, 0x0, 0x0}, 0},
    {0, 7, {0xd7, 0x0, 0xb, 0xc6, 0x19, 0xab, 0xab}, 0},
    {0,
     33,
     {0xa3, 0x51, 0x3, 0x88, 0x0, 0x44, 0x0, 0x0, 0x0, 0x0,  0x4,  0x5c, 0xcf, 0x0, 0x1a, 0x0, 0x45,
      0x5,  0x0,  0x0, 0x0,  0x0, 0x46, 0x0, 0x0, 0x2, 0x20, 0x52, 0x0,  0x0,  0x0, 0x0,  0xff},
     0},
    {0,
     49,
     {0xa7, 0x19, 0x19, 0x0,  0x64, 0x40, 0x5,  0x14, 0x40, 0x0,  0x4,  0x3,  0xcf, 0xcf, 0x0, 0x64, 0x40,
      0x23, 0x67, 0x1,  0x0,  0x2,  0x0,  0xcf, 0xcf, 0x0,  0x64, 0x40, 0x4b, 0x5a, 0x0,  0x0, 0x2,  0x1,
      0xcf, 0xcf, 0x0,  0x24, 0x40, 0x69, 0x78, 0x0,  0x0,  0x0,  0x0,  0xcf, 0xcf, 0x0,  0x44},
     0},
    {0,
     45,
     {0xa6, 0x30, 0x0, 0x24, 0x77, 0x36, 0x0,  0x37, 0x0, 0x5c, 0xcf, 0x2, 0xe4, 0x99, 0x0,
      0xb,  0x0,  0xb, 0xcf, 0x5c, 0x2,  0xa4, 0x11, 0x0, 0x0,  0x0,  0x0, 0x5c, 0xcf, 0x0,
      0xac, 0x11, 0x0, 0x0,  0x0,  0x0,  0x5c, 0xcf, 0x0, 0x0,  0x6,  0x0, 0x0,  0x0,  0x0},
     0},
    {0,
     49,
     {0xa7, 0x19, 0x19, 0x0,  0x64, 0x40, 0x5,  0x14, 0x40, 0x0,  0x4,  0x3,  0x5c, 0xcf, 0x0, 0x64, 0x40,
      0x23, 0x67, 0x1,  0x0,  0x2,  0x0,  0x5c, 0xcf, 0x0,  0x64, 0x40, 0x4b, 0x5a, 0x0,  0x0, 0x2,  0x1,
      0x5c, 0xcf, 0x0,  0x24, 0x40, 0x69, 0x78, 0x0,  0x0,  0x0,  0x0,  0x5c, 0xcf, 0x0,  0x44},
     0},
    {0,
     38,
     {0xac, 0x13, 0x10, 0x1, 0x0, 0x1c, 0x12, 0x2, 0x1b, 0x18, 0x9,  0x19, 0x1a, 0x1b, 0x1b, 0x18, 0x1b, 0xb,  0x11,
      0x0,  0x2,  0x1c, 0xa, 0x2, 0x18, 0x18, 0x8, 0x19, 0x1a, 0x1b, 0x1b, 0x18, 0x1b, 0xff, 0xff, 0xff, 0xff, 0x0},
     0},
    {0, 8, {0xad, 0xcc, 0x40, 0x46, 0x11, 0x4, 0x5c, 0xcf}, 0},
    {0, 15, {0xe8, 0x30, 0x7, 0x0, 0xeb, 0xeb, 0x9c, 0x0, 0xe2, 0x4, 0x0, 0x0, 0x0, 0x0, 0xef}, 0},
    {0, 10, {0xe9, 0x3c, 0x7f, 0x8, 0x12, 0x1a, 0x7a, 0x22, 0x1a, 0x33}, 0},
    {0, 3, {0x75, 0x3, 0x4}, 0},
    {0,
     34,
     {0xe7, 0x8b, 0x3c, 0x0,  0xc,  0xf0, 0x5d, 0x0,  0x5d, 0x5, 0x5d, 0x0, 0x5d, 0x0, 0xff, 0x0,  0x8,
      0x7b, 0x0,  0x0,  0xc8, 0x6a, 0x5a, 0x8,  0x1a, 0x3c, 0x0, 0x91, 0x1, 0xcc, 0x1, 0x7f, 0xf0, 0x22},
     0},
    {0,
     34,
     {0xe7, 0x8b, 0x3e, 0x0,  0xc,  0xf0, 0x5d, 0x0,  0x5d, 0x5, 0x5d, 0x0, 0x5d, 0x0, 0xff, 0x0,  0x8,
      0x7b, 0x0,  0x0,  0xc8, 0x6a, 0x5a, 0x8,  0x1a, 0x3c, 0x0, 0x91, 0x1, 0xcc, 0x1, 0x7f, 0xf0, 0x22},
     0},
    {0,
     38,
     {0xc8, 0x0, 0x0,  0x1a, 0x2a, 0x43, 0x0, 0x69, 0x3,  0x97, 0x5,  0x10, 0xd9, 0x7,  0x34, 0xf,  0x11, 0x71, 0x1,
      0xb0, 0x0, 0x21, 0xeb, 0x1,  0x3a, 0xf, 0x32, 0xb5, 0xa,  0x1d, 0xe,  0xf3, 0x76, 0x0,  0xb8, 0xe1, 0x3,  0xff},
     0},
    {0,
     38,
     {0xc9, 0x0, 0x0,  0x1a, 0x2a, 0x43, 0x0, 0x69, 0x3,  0x97, 0x5,  0x10, 0xd9, 0x7,  0x34, 0xf,  0x11, 0x71, 0x1,
      0xb0, 0x0, 0x21, 0xeb, 0x1,  0x3a, 0xf, 0x32, 0xb5, 0xa,  0x1d, 0xe,  0xf3, 0x76, 0x0,  0xb8, 0xe1, 0x3,  0xff},
     0},
    {0,
     38,
     {0xc8, 0x0, 0x0,  0x28, 0x43, 0x66, 0x0, 0x9b, 0x3, 0xdd, 0x6,  0x11, 0x37, 0x6,  0xa7, 0x2,  0x21, 0xf4, 0x2,
      0x33, 0x1, 0x22, 0x6d, 0xd,  0xaf, 0xa, 0x33, 0x8, 0xd,  0x51, 0xc,  0xf3, 0x8f, 0xf,  0xc0, 0xe7, 0x3,  0xff},
     0},
    {0,
     38,
     {0xc9, 0x0, 0x0,  0x28, 0x43, 0x66, 0x0, 0x9b, 0x3, 0xdd, 0x6,  0x11, 0x37, 0x6,  0xa7, 0x2,  0x21, 0xf4, 0x2,
      0x33, 0x1, 0x22, 0x6d, 0xd,  0xaf, 0xa, 0x33, 0x8, 0xd,  0x51, 0xc,  0xf3, 0x8f, 0xf,  0xc0, 0xe7, 0x3,  0xff},
     0},
    {0, 4, {0x99, 0x71, 0x2, 0x0}, 0},
    {0, 1, {0x11}, 120},
    {0, 1, {0x29}, 20},
    {0, 2, {0x35, 0x0}, 0},
    {0, 2, {0x36, 0x0}, 0},
};
const static lcd_cmd_sequ g_st7701_800x480_init_on[] = {
    {0, 2, {0xee, 0x01}, 0}, //  ENTER PAGE1
    {0, 2, {0xea, 0x07}, 0},
    {0, 2, {0xeb, 0x12}, 0},
    {0, 2, {0x0a, 0x40}, 0}, //  vcom
    {0, 2, {0x17, 0x35}, 0}, // VDDD
    {0, 2, {0x28, 0x28}, 0}, // 14vgh 16V 28
    {0, 2, {0x29, 0x25}, 0}, // 1bvgl -13.5V 25
    {0, 2, {0x2a, 0x62}, 0},
    {0, 2, {0x2f, 0xf3}, 0},
    {0, 2, {0x05, 0x10}, 0},
    {0, 2, {0xee, 0x02}, 0}, //  ENTER PAGE2
    {0, 2, {0x39, 0xa0}, 0}, // VSPNR 5.0
    // GAMMA 2.2
    {0, 2, {0x00, 0x00}, 0}, //  0
    {0, 2, {0x01, 0x04}, 0}, //  4
    {0, 2, {0x02, 0x08}, 0}, //  8
    {0, 2, {0x03, 0x06}, 0}, // 12
    {0, 2, {0x04, 0x0c}, 0},  // 28
    {0, 2, {0x05, 0x26}, 0}, //  52
    {0, 2, {0x06, 0x09}, 0}, //  76
    {0, 2, {0x07, 0x0b}, 0}, //  100
    {0, 2, {0x08, 0x0c}, 0}, //  131
    {0, 2, {0x09, 0x0b}, 0}, // 155
    {0, 2, {0x0A, 0x0f}, 0}, // 179
    {0, 2, {0x0b, 0x44}, 0}, // 203
    {0, 2, {0x0c, 0x13}, 0}, //  227
    {0, 2, {0x0d, 0x19}, 0}, //  243
    {0, 2, {0x0e, 0x28}, 0}, //  247
    {0, 2, {0x0f, 0x2d}, 0}, // 251
    {0, 2, {0x10, 0x3F}, 0}, //  255
    {0, 2, {0x20, 0x00}, 0},
    {0, 2, {0x21, 0x04}, 0},
    {0, 2, {0x22, 0x08}, 0},
    {0, 2, {0x23, 0x06}, 0},
    {0, 2, {0x24, 0x0c}, 0},
    {0, 2, {0x25, 0x26}, 0},
    {0, 2, {0x26, 0x09}, 0},
    {0, 2, {0x27, 0x0b}, 0},
    {0, 2, {0x28, 0x0c}, 0},
    {0, 2, {0x29, 0x0b}, 0},
    {0, 2, {0x2A, 0x0f}, 0},
    {0, 2, {0x2b, 0x44}, 0},
    {0, 2, {0x2c, 0x13}, 0},
    {0, 2, {0x2d, 0x19}, 0},
    {0, 2, {0x2e, 0x28}, 0},
    {0, 2, {0x2f, 0x2d}, 0},
    {0, 2, {0x30, 0x3F}, 0},
    {0, 2, {0xee, 0x03}, 0}, // page3
    {0, 2, {0x0B, 0x55}, 0},
    {0, 2, {0x0f, 0xb9}, 0}, //  上电擦黑
    {0, 2, {0xee, 0x04}, 0}, // page4
    {0, 2, {0x00, 0x00}, 0}, // 800  00
    {0, 2, {0x01, 0x00}, 0},
    {0, 2, {0x02, 0xf0}, 0}, //  480
    {0, 2, {0x03, 0x03}, 0}, //  03
    {0, 2, {0x04, 0x20}, 0}, // 20
    {0, 2, {0x06, 0x06}, 0},
    {0, 2, {0x07, 0x05}, 0},
    {0, 2, {0x08, 0x12}, 0}, // ss-tp
    {0, 2, {0x09, 0x20}, 0}, // pol 20
    {0, 2, {0x0a, 0x09}, 0}, // SMGIP
    {0, 2, {0x0b, 0x00}, 0},
    {0, 2, {0x20, 0x40}, 0},
    {0, 2, {0x2A, 0x00}, 0},
    {0, 2, {0x40, 0x80}, 0},
    {0, 2, {0x46, 0x10}, 0},
    {0, 2, {0x47, 0x10}, 0},
    {0, 2, {0x48, 0x0f}, 0},
    {0, 2, {0x49, 0x0f}, 0},
    {0, 2, {0xee, 0x05}, 0},
    {0, 2, {0x00, 0x00}, 0},
    {0, 2, {0x01, 0x04}, 0},   //  stva
    {0, 2, {0x02, 0x15}, 0},
    {0, 2, {0x03, 0x05}, 0},
    {0, 2, {0x10, 0x02}, 0},
    {0, 2, {0x11, 0x06}, 0},  //  ckva
    {0, 2, {0x12, 0x45}, 0},
    {0, 2, {0x13, 0x05}, 0},
    {0, 2, {0x19, 0xCd}, 0},
    {0, 2, {0x1a, 0x73}, 0},
    {0, 2, {0x23, 0x00}, 0},
    {0, 2, {0x40, 0xee}, 0},
    {0, 2, {0x43, 0x00}, 0},
    {0, 2, {0x44, 0x01}, 0},
    {0, 2, {0x45, 0x81}, 0},
    {0, 2, {0x46, 0x06}, 0},
    {0, 2, {0x47, 0x00}, 0},
    {0, 2, {0xee, 0x06}, 0}, // PAGE6
    {0, 2, {0x00, 0x23}, 0},
    {0, 2, {0x01, 0x01}, 0},
    {0, 2, {0x06, 0xCD}, 0},
    {0, 2, {0x08, 0x67}, 0},
    {0, 2, {0x09, 0x45}, 0},
    {0, 2, {0x0A, 0x23}, 0},
    {0, 2, {0x0B, 0x01}, 0},
    {0, 2, {0xee, 0x07}, 0}, // PAGE7
    // GIP LEFT 1-12
    {0, 2, {0x00, 0x16}, 0},
    {0, 2, {0x01, 0x14}, 0},
    {0, 2, {0x02, 0x3C}, 0},
    {0, 2, {0x03, 0x0c}, 0},
    {0, 2, {0x04, 0x0d}, 0},
    {0, 2, {0x05, 0x10}, 0},
    {0, 2, {0x06, 0x12}, 0},
    {0, 2, {0x07, 0x3F}, 0},
    {0, 2, {0x08, 0x00}, 0},
    {0, 2, {0x09, 0x02}, 0},
    {0, 2, {0x0A, 0x3C}, 0},
    {0, 2, {0x0b, 0x3C}, 0},
    // GIP RIGHT 1-12
    {0, 2, {0x20, 0x17}, 0},
    {0, 2, {0x21, 0x15}, 0},
    {0, 2, {0x22, 0x3C}, 0},
    {0, 2, {0x23, 0x0c}, 0},
    {0, 2, {0x24, 0x0d}, 0},
    {0, 2, {0x25, 0x11}, 0},
    {0, 2, {0x26, 0x13}, 0},
    {0, 2, {0x27, 0x3F}, 0},
    {0, 2, {0x28, 0x01}, 0},
    {0, 2, {0x29, 0x03}, 0},
    {0, 2, {0x2A, 0x3C}, 0},
    {0, 2, {0x2b, 0x3C}, 0},
    {0, 2, {0xee, 0x08}, 0},
    {0, 2, {0x11, 0x83}, 0},
    {0, 2, {0x12, 0xdd}, 0},
    {0, 2, {0x13, 0x2b}, 0},
    {0, 2, {0x18, 0x00}, 0},
    {0, 2, {0x20, 0x80}, 0},  //  auto
    {0, 2, {0xee, 0x0F}, 0},
    {0, 2, {0x00, 0x01}, 0},
    {0, 2, {0x01, 0x10}, 0},
    {0, 2, {0xea, 0x00}, 0}, //  ENTER PAGE 0
    {0, 2, {0xeb, 0x00}, 0},
    {0, 2, {0xee, 0x00}, 0},
    {0, 1, {0x11}, 120},
    {0, 1, {0x29}, 100},
};

const static lcd_cmd_sequ g_gh7006_800x480_init_on[] = {
    {0, 2, {0xee, 0x01}, 1}, // ENTER PAGE1
    {0, 2, {0xea, 0x07}, 0},
    {0, 2, {0xeb, 0x12}, 0},
    {0, 2, {0x0a, 0x40}, 0}, // vcom,
    {0, 2, {0x17, 0x35}, 0}, // VDDD
    {0, 2, {0x28, 0x28}, 0}, //  14vgh 16V 28
    {0, 2, {0x29, 0x25}, 0}, // 1bvgl -13.5V 25
    {0, 2, {0x2a, 0x62}, 0},
    {0, 2, {0x2f, 0xf3}, 0},
    {0, 2, {0xee, 0x02}, 1}, //  ENTER PAGE2
    {0, 2, {0x39, 0xa0}, 0}, // VSPNR 5.0
    //  GAMMA 2.2
    {0, 2, {0x00, 0x00}, 0}, // 0
    {0, 2, {0x01, 0x04}, 0}, // 4
    {0, 2, {0x02, 0x08}, 0}, // 8,
    {0, 2, {0x03, 0x06}, 0}, // 12,
    {0, 2, {0x04, 0x0c}, 0},  // 28
    {0, 2, {0x05, 0x26}, 0}, // 52
    {0, 2, {0x06, 0x09}, 0}, // 76
    {0, 2, {0x07, 0x0b}, 0}, // 100
    {0, 2, {0x08, 0x0c}, 0}, // 131
    {0, 2, {0x09, 0x0b}, 0}, // 155
    {0, 2, {0x0A, 0x0f}, 0}, // 179
    {0, 2, {0x0b, 0x44}, 0}, // 203
    {0, 2, {0x0c, 0x13}, 0}, // 227
    {0, 2, {0x0d, 0x19}, 0}, // 243
    {0, 2, {0x0e, 0x28}, 0}, // 247
    {0, 2, {0x0f, 0x2d}, 0}, // 251
    {0, 2, {0x10, 0x3F}, 0}, // 255
    {0, 2, {0x20, 0x00}, 0},
    {0, 2, {0x21, 0x04}, 0},
    {0, 2, {0x22, 0x08}, 0},
    {0, 2, {0x23, 0x06}, 0},
    {0, 2, {0x24, 0x0c}, 0},
    {0, 2, {0x25, 0x26}, 0},
    {0, 2, {0x26, 0x09}, 0},
    {0, 2, {0x27, 0x0b}, 0},
    {0, 2, {0x28, 0x0c}, 0},
    {0, 2, {0x29, 0x0b}, 0},
    {0, 2, {0x2A, 0x0f}, 0},
    {0, 2, {0x2b, 0x44}, 0},
    {0, 2, {0x2c, 0x13}, 0},
    {0, 2, {0x2d, 0x19}, 0},
    {0, 2, {0x2e, 0x28}, 0},
    {0, 2, {0x2f, 0x2d}, 0},
    {0, 2, {0x30, 0x3F}, 0},
    {0, 2, {0xee, 0x03}, 1}, //  page3,
    {0, 2, {0x0B, 0x55}, 0},
    {0, 2, {0x0f, 0xb9}, 0}, //  上电擦黑
    {0, 2, {0xee, 0x04}, 1}, //  page4
    {0, 2, {0x00, 0x00}, 0}, //  800  00
    {0, 2, {0x01, 0x00}, 0},
    {0, 2, {0x02, 0xf0}, 0}, //  480
    {0, 2, {0x03, 0x03}, 0}, //  03
    {0, 2, {0x04, 0x20}, 0}, //  20
    {0, 2, {0x06, 0x06}, 0},
    {0, 2, {0x07, 0x05}, 0},
    {0, 2, {0x08, 0x12}, 0}, //  ss-tp
    {0, 2, {0x09, 0x20}, 0}, //  pol 20
    {0, 2, {0x0a, 0x09}, 0}, //  SMGIP
    {0, 2, {0x0b, 0x00}, 0},
    {0, 2, {0x20, 0x40}, 0},
    {0, 2, {0x2A, 0x00}, 0},
    {0, 2, {0x40, 0x80}, 0},  //    bist
    {0, 2, {0x41, 0x55}, 0},  //    bist
    {0, 2, {0x46, 0x10}, 0},
    {0, 2, {0x47, 0x10}, 0},
    {0, 2, {0x48, 0x0f}, 0},
    {0, 2, {0x49, 0x0f}, 0},
    {0, 2, {0xee, 0x05}, 1},
    {0, 2, {0x00, 0x00}, 0},
    {0, 2, {0x01, 0x04}, 0},   //  stva,
    {0, 2, {0x02, 0x15}, 0},
    {0, 2, {0x03, 0x05}, 0},
    {0, 2, {0x10, 0x02}, 0},
    {0, 2, {0x11, 0x06}, 0},  //  ckva
    {0, 2, {0x12, 0x45}, 0},
    {0, 2, {0x13, 0x05}, 0},
    {0, 2, {0x19, 0xCd}, 0},
    {0, 2, {0x1a, 0x73}, 0},
    {0, 2, {0x23, 0x00}, 0},
    {0, 2, {0x40, 0xee}, 0},
    {0, 2, {0x43, 0x00}, 0},
    {0, 2, {0x44, 0x01}, 0},
    {0, 2, {0x45, 0x81}, 0},
    {0, 2, {0x46, 0x06}, 0},
    {0, 2, {0x47, 0x00}, 0},
    {0, 2, {0xee, 0x06}, 1}, //  PAGE6,
    {0, 2, {0x00, 0x23}, 0},
    {0, 2, {0x01, 0x01}, 0},
    {0, 2, {0x06, 0xCD}, 0},
    {0, 2, {0x08, 0x67}, 0},
    {0, 2, {0x09, 0x45}, 0},
    {0, 2, {0x0A, 0x23}, 0},
    {0, 2, {0x0B, 0x01}, 0},

    {0, 2, {0xee, 0x07}, 1}, //  PAGE7,
// GIP LEFT 1-12,
    {0, 2, {0x00, 0x16}, 0},
    {0, 2, {0x01, 0x14}, 0},
    {0, 2, {0x02, 0x3C}, 0},
    {0, 2, {0x03, 0x0c}, 0},
    {0, 2, {0x04, 0x0d}, 0},
    {0, 2, {0x05, 0x10}, 0},
    {0, 2, {0x06, 0x12}, 0},
    {0, 2, {0x07, 0x3F}, 0},
    {0, 2, {0x08, 0x00}, 0},
    {0, 2, {0x09, 0x02}, 0},
    {0, 2, {0x0A, 0x3C}, 0},
    {0, 2, {0x0b, 0x3C}, 0},
// GIP RIGHT 1-12,
    {0, 2, {0x20, 0x17}, 0},
    {0, 2, {0x21, 0x15}, 0},
    {0, 2, {0x22, 0x3C}, 0},
    {0, 2, {0x23, 0x0c}, 0},
    {0, 2, {0x24, 0x0d}, 0},
    {0, 2, {0x25, 0x11}, 0},
    {0, 2, {0x26, 0x13}, 0},
    {0, 2, {0x27, 0x3F}, 0},
    {0, 2, {0x28, 0x01}, 0},
    {0, 2, {0x29, 0x03}, 0},
    {0, 2, {0x2A, 0x3C}, 0},
    {0, 2, {0x2b, 0x3C}, 0},

    {0, 2, {0xee, 0x08}, 1},
    {0, 2, {0x10, 0x03}, 0},      //  R10=03  1LANE /R10=02  2LANE / R10=01 3LAN / R10= 00 4LAN
    {0, 2, {0x11, 0x83}, 0},
    {0, 2, {0x12, 0xda}, 0},
    {0, 2, {0x13, 0x1c}, 0},
    {0, 2, {0x18, 0x00}, 0},
    {0, 2, {0xee, 0x0F}, 1},
    {0, 2, {0x00, 0x01}, 0},
    {0, 2, {0x01, 0x10}, 0},
    {0, 2, {0xea, 0x00}, 0}, //  ENTER PAGE 0
    {0, 2, {0xeb, 0x00}, 0},
    {0, 2, {0xee, 0x00}, 1},
    {0, 2, {0x36, 0x00}, 0},
    {0, 2, {0xea, 0x00}, 0}, //  ENTER PAGE 0
    {0, 2, {0xeb, 0x00}, 0},
    {0, 2, {0xee, 0x00}, 1},
    {1, 2, {0x3a, 0x55}, 1},
    {1, 2, {0x11}, 120},       //  sleep out,
    {1, 2, {0x29}, 1},      //  display on,
};

const static lcd_cmd_sequ g_GH7002_1024X600_init_on[] = {
    /* Page 1 - Power and Voltage Configuration */
    {0, 2, {0xEE, 0x01}, 0},  /* Enter Page 1 */
    {0, 2, {0xEA, 0x07}, 0},
    {0, 2, {0xEB, 0x12}, 0},
    {0, 2, {0x0A, 0x50}, 0},  /* VCOM */
    {0, 2, {0x13, 0x14}, 0},
    {0, 2, {0x17, 0x35}, 0},  /* VDDD */
    {0, 2, {0x1D, 0x44}, 0},  /* VGH PUMP, 33 */
    {0, 2, {0x21, 0x01}, 0},
    {0, 2, {0x28, 0x2A}, 0},  /* VGH: 2A/30/25 */
    {0, 2, {0x29, 0x0D}, 0},  /* VGL: 0C/14/24 */
    {0, 2, {0x2A, 0x63}, 0},
    {0, 2, {0x2F, 0xF3}, 0},  /* Power Enable */
    {0, 2, {0x05, 0x55}, 0},  /* OSC */
    /* Page 2 - VSPNR and Gamma Correction */
    {0, 2, {0xEE, 0x02}, 0},  /* Enter Page 2 */
    {0, 2, {0x39, 0x90}, 0},  /* VSPNR */
    /* Gamma 2.2 Curve - Positive */
    {0, 2, {0x00, 0x00}, 0},
    {0, 2, {0x01, 0x0D}, 0},
    {0, 2, {0x02, 0x12}, 0},  /* 8 */
    {0, 2, {0x03, 0x08}, 0},  /* 12 */
    {0, 2, {0x04, 0x0E}, 0},  /* 28 */
    {0, 2, {0x05, 0x30}, 0},  /* 52 */
    {0, 2, {0x06, 0x0B}, 0},  /* 76 */
    {0, 2, {0x07, 0x0D}, 0},  /* 100 */
    {0, 2, {0x08, 0x0E}, 0},  /* 131 */
    {0, 2, {0x09, 0x0D}, 0},  /* 155 */
    {0, 2, {0x0A, 0x10}, 0},  /* 179 */
    {0, 2, {0x0B, 0x4D}, 0},  /* 203 */
    {0, 2, {0x0C, 0x13}, 0},  /* 227 */
    {0, 2, {0x0D, 0x19}, 0},  /* 243 */
    {0, 2, {0x0E, 0x2E}, 0},  /* 247 */
    {0, 2, {0x0F, 0x33}, 0},  /* 251 */
    {0, 2, {0x10, 0x3F}, 0},  /* 255 */
    /* Gamma 2.2 Curve - Negative */
    {0, 2, {0x20, 0x00}, 0},
    {0, 2, {0x21, 0x0D}, 0},
    {0, 2, {0x22, 0x12}, 0},
    {0, 2, {0x23, 0x08}, 0},
    {0, 2, {0x24, 0x0E}, 0},
    {0, 2, {0x25, 0x30}, 0},
    {0, 2, {0x26, 0x0B}, 0},
    {0, 2, {0x27, 0x0D}, 0},
    {0, 2, {0x28, 0x0E}, 0},
    {0, 2, {0x29, 0x0D}, 0},
    {0, 2, {0x2A, 0x10}, 0},
    {0, 2, {0x2B, 0x4D}, 0},
    {0, 2, {0x2C, 0x13}, 0},
    {0, 2, {0x2D, 0x19}, 0},
    {0, 2, {0x2E, 0x2E}, 0},
    {0, 2, {0x2F, 0x33}, 0},
    {0, 2, {0x30, 0x3F}, 0},
    /* Page 4 - Source and Gate Configuration */
    {0, 2, {0xEE, 0x04}, 0},  /* Enter Page 4 */
    {0, 2, {0x00, 0x05}, 0},  /* 05=512, SOURCE change */
    {0, 2, {0x01, 0x01}, 0},  /* GAT */
    {0, 2, {0x02, 0x2C}, 0},  /* GAT */
    {0, 2, {0x03, 0x04}, 0},  /* SOURCE H */
    {0, 2, {0x04, 0x00}, 0},  /* SOURCE H */
    {0, 2, {0x06, 0x16}, 0},
    {0, 2, {0x07, 0x0A}, 0},  /* SSTEP */
    {0, 2, {0x08, 0x1A}, 0},
    {0, 2, {0x09, 0x20}, 0},  /* POL */
    {0, 2, {0x0A, 0x0A}, 0},  /* +/-1 or +/-2 */
    {0, 2, {0x0B, 0x00}, 0},
    {0, 2, {0x0F, 0x0A}, 0},
    {0, 2, {0x20, 0x00}, 0},  /* Old=00 */
    {0, 2, {0x24, 0x08}, 0},
    {0, 2, {0x40, 0x80}, 0},  /* 80, BIST=00 */
    /* Page 5 - STVA/STAB and CLK Configuration */
    {0, 2, {0xEE, 0x05}, 0},  /* Enter Page 5 */
    {0, 2, {0x00, 0x01}, 0},  /* STVA */
    {0, 2, {0x01, 0x05}, 0},
    {0, 2, {0x02, 0x15}, 0},
    {0, 2, {0x03, 0x05}, 0},
    {0, 2, {0x07, 0xEB}, 0},  /* STVB */
    {0, 2, {0x08, 0xEF}, 0},
    {0, 2, {0x09, 0x44}, 0},
    {0, 2, {0x10, 0x03}, 0},  /* CLKA */
    {0, 2, {0x11, 0x07}, 0},
    {0, 2, {0x12, 0x55}, 0},
    {0, 2, {0x13, 0x05}, 0},
    {0, 2, {0x19, 0xBB}, 0},
    {0, 2, {0x1A, 0x74}, 0},
    {0, 2, {0x30, 0x01}, 0},
    {0, 2, {0x31, 0x01}, 0},
    {0, 2, {0x32, 0x00}, 0},
    {0, 2, {0x33, 0x14}, 0},
    {0, 2, {0x34, 0x14}, 0},
    {0, 2, {0x35, 0x78}, 0},
    {0, 2, {0x36, 0x01}, 0},
    {0, 2, {0x37, 0x01}, 0},
    {0, 2, {0x38, 0x00}, 0},
    {0, 2, {0x39, 0x14}, 0},
    {0, 2, {0x3A, 0x14}, 0},
    {0, 2, {0x40, 0x55}, 0},
    {0, 2, {0x41, 0x00}, 0},
    {0, 2, {0x43, 0x11}, 0},
    {0, 2, {0x44, 0x01}, 0},
    {0, 2, {0x45, 0x81}, 0},
    {0, 2, {0x46, 0x06}, 0},
    {0, 2, {0x47, 0x00}, 0},
    /* Page 6 - GIP Back */
    {0, 2, {0xEE, 0x06}, 0},  /* Page 6, GIP Back */
    {0, 2, {0x00, 0x01}, 0},
    {0, 2, {0x02, 0x45}, 0},
    {0, 2, {0x06, 0xCD}, 0},
    {0, 2, {0x08, 0x67}, 0},
    {0, 2, {0x09, 0x45}, 0},
    {0, 2, {0x0A, 0x23}, 0},
    {0, 2, {0x0B, 0x01}, 0},
    /* Page 7 - GIP Left and Right Configuration */
    {0, 2, {0xEE, 0x07}, 0},  /* Enter Page 7 */
    /* GIP LEFT 1-24 */
    {0, 2, {0x00, 0x01}, 0},
    {0, 2, {0x01, 0x05}, 0},
    {0, 2, {0x02, 0x0C}, 0},
    {0, 2, {0x03, 0x0D}, 0},
    {0, 2, {0x04, 0x3C}, 0},
    {0, 2, {0x05, 0x3C}, 0},
    {0, 2, {0x06, 0x21}, 0},
    {0, 2, {0x07, 0x20}, 0},
    {0, 2, {0x08, 0x12}, 0},
    {0, 2, {0x09, 0x12}, 0},
    {0, 2, {0x0A, 0x10}, 0},
    {0, 2, {0x0B, 0x10}, 0},
    {0, 2, {0x0C, 0x16}, 0},
    {0, 2, {0x0D, 0x16}, 0},
    {0, 2, {0x0E, 0x14}, 0},
    {0, 2, {0x0F, 0x14}, 0},
    {0, 2, {0x10, 0x3C}, 0},
    {0, 2, {0x11, 0x3C}, 0},
    {0, 2, {0x12, 0x3C}, 0},
    {0, 2, {0x13, 0x3C}, 0},
    {0, 2, {0x14, 0x3C}, 0},
    {0, 2, {0x15, 0x3C}, 0},
    /* GIP RIGHT 1-24 */
    {0, 2, {0x20, 0x00}, 0},
    {0, 2, {0x21, 0x04}, 0},
    {0, 2, {0x22, 0x0C}, 0},
    {0, 2, {0x23, 0x0D}, 0},
    {0, 2, {0x24, 0x3C}, 0},
    {0, 2, {0x25, 0x3C}, 0},
    {0, 2, {0x26, 0x21}, 0},
    {0, 2, {0x27, 0x20}, 0},
    {0, 2, {0x28, 0x13}, 0},
    {0, 2, {0x29, 0x13}, 0},
    {0, 2, {0x2A, 0x11}, 0},
    {0, 2, {0x2B, 0x11}, 0},
    {0, 2, {0x2C, 0x17}, 0},
    {0, 2, {0x2D, 0x17}, 0},
    {0, 2, {0x2E, 0x15}, 0},
    {0, 2, {0x2F, 0x15}, 0},
    {0, 2, {0x30, 0x3C}, 0},
    {0, 2, {0x31, 0x3C}, 0},
    {0, 2, {0x32, 0x3C}, 0},
    {0, 2, {0x33, 0x3C}, 0},
    {0, 2, {0x34, 0x3C}, 0},
    {0, 2, {0x35, 0x3C}, 0},
    /* Page 8 - VDDH and Lane Configuration */
    {0, 2, {0xEE, 0x08}, 0},  /* Enter Page 8 */
    {0, 2, {0x10, 0x03}, 0},
    {0, 2, {0x12, 0xDE}, 0},  /* VDDH */
    {0, 2, {0x13, 0x1C}, 0},
    {0, 2, {0x17, 0x23}, 0},
    {0, 2, {0x18, 0x30}, 0},
    {0, 2, {0x20, 0x00}, 0},  /* Auto Lane */
    /* Page F - Dual Gate Enable */
    {0, 2, {0xEE, 0x0F}, 0},  /* Enter Page F */
    {0, 2, {0x00, 0x01}, 0},  /* Dual Gate Enable */
    {0, 2, {0x01, 0x10}, 0},
    /* Page 0 - Final Configuration */
    {0, 2, {0xEE, 0x00}, 0},  /* Enter Page 0 */
    {0, 2, {0xEA, 0x00}, 0},
    {0, 2, {0xEB, 0x00}, 0},
    {0, 2, {0x36, 0x00}, 0},
    {0, 2, {0x3A, 0x55}, 0},
    {0, 2, {0x11, 0x00}, 120},  /* Sleep Out, 120ms delay */
    {0, 2, {0x29, 0x00}, 4},   /* Display On, 4ms delay */
};
#endif

const static lcd_cmd_sequ g_466x466_v1_screen_init_on[] = {
    {0, 2, {0xfe, 0x00}, 0},
    {0, 2, {0x35, 0x00}, 0},
    {0, 2, {0x53, 0x20}, 0},
    {0, 2, {0x51, 0xff}, 0},
    {0, 2, {0x63, 0xff}, 0},
    {0, 5, {0x2a, 0x00, 0x06, 0x01, 0xd7}, 0},
    {0, 5, {0x2b, 0x00, 0x00, 0x01, 0xd1}, 0},
    {0, 1, {0x11}, 60},
    {0, 1, {0x29}, 0},
};

const static lcd_cmd_sequ g_gc9307_240x284_init_on[] = {
    {0, 1, {0xfe}, 0},
    {0, 1, {0xef}, 0},
    {0, 2, {0x36, 0x48}, 0},
    {0, 2, {0x3a, 0x5}, 0},
    {0, 1, {0x20}, 0},
    {0, 2, {0x86, 0x98}, 0},
    {0, 2, {0x89, 0x13}, 0},
    {0, 2, {0x8b, 0x80}, 0},
    {0, 2, {0x8d, 0x33}, 0},
    {0, 2, {0x8e, 0xf}, 0},
    {0, 4, {0xec, 0x33, 0x7, 0xaf}, 0},
    {0, 2, {0xc9, 0x5}, 0},
    {0, 3, {0xe8, 0x12, 0x0}, 0},
    {0, 2, {0xc3, 0x10}, 0},
    {0, 2, {0xc4, 0x10}, 0},
    {0, 2, {0xff, 0x62}, 0},
    {0, 2, {0x99, 0x3e}, 0},
    {0, 2, {0x9d, 0x4b}, 0},
    {0, 2, {0x98, 0x3e}, 0},
    {0, 2, {0x9c, 0x4b}, 0},
    {0, 7, {0xf0, 0xc7, 0x8, 0x8, 0x8, 0x5, 0x2e}, 0},
    {0, 7, {0xf1, 0x45, 0x8f, 0x6f, 0x33, 0x36, 0x4f}, 0},
    {0, 7, {0xf2, 0xc7, 0x8, 0x8, 0x8, 0x5, 0x2e}, 0},
    {0, 7, {0xf3, 0x45, 0x8f, 0x6f, 0x33, 0x36, 0x4f}, 0},
    {0, 2, {0x35, 0x0}, 0},
    {0, 1, {0x11}, 120},
    {0, 1, {0x29}, 0},
};

static lcd_cmds_sequ g_axs15231_320x480_screen_init_on = {
    .cmd = (lcd_cmd_sequ *)g_axs15231_320x480_init_on,
    .cmd_cnt = array_size(g_axs15231_320x480_init_on),
};

static lcd_cmds_sequ g_gc9307_240x284_screen_init_on = {
    .cmd = (lcd_cmd_sequ *)g_gc9307_240x284_init_on,
    .cmd_cnt = array_size(g_gc9307_240x284_init_on),
};

#ifdef GPU_3322
static lcd_cmds_sequ g_gh7006_800x480_screen_init_on = {
    .cmd = (lcd_cmd_sequ *)g_gh7006_800x480_init_on,
    .cmd_cnt = array_size(g_gh7006_800x480_init_on),
};

static lcd_cmds_sequ g_st7701_800x480_screen_init_on = {
    .cmd = (lcd_cmd_sequ *)g_st7701_800x480_init_on,
    .cmd_cnt = array_size(g_st7701_800x480_init_on),
};

static lcd_cmds_sequ g_st7102_480x854_screen_init_on = {
    .cmd = (lcd_cmd_sequ *)g_st7102_480x854_init_on,
    .cmd_cnt = array_size(g_st7102_480x854_init_on),
};

static lcd_cmds_sequ g_GH7002_1024X600_screen_init_on = {
    .cmd = (lcd_cmd_sequ *)g_GH7002_1024X600_init_on,
    .cmd_cnt = array_size(g_GH7002_1024X600_init_on),
};
#endif

static lcd_cmds_sequ g_st7701_480x854_screen_init_on = {
    .cmd = (lcd_cmd_sequ *)g_st7701_480x854_init_on,
    .cmd_cnt = array_size(g_st7701_480x854_init_on),
};

static lcd_cmds_sequ g_display_off_cmds = {
    .cmd = (lcd_cmd_sequ *)g_common_cmd_display_off,
    .cmd_cnt = array_size(g_common_cmd_display_off),
};

static lcd_cmds_sequ g_idle_in_cmds = {
    .cmd = (lcd_cmd_sequ *)g_common_cmd_idle_in,
    .cmd_cnt = array_size(g_common_cmd_idle_in),
};

static lcd_cmds_sequ g_idle_out_cmds = {
    .cmd = (lcd_cmd_sequ *)g_common_cmd_idle_out,
    .cmd_cnt = array_size(g_common_cmd_idle_out),
};

static lcd_cmds_sequ g_te_on_cmds = {
    .cmd = (lcd_cmd_sequ *)g_common_cmd_te_on,
    .cmd_cnt = array_size(g_common_cmd_te_on),
};

static lcd_cmds_sequ g_te_off_cmds = {
    .cmd = (lcd_cmd_sequ *)g_common_cmd_te_off,
    .cmd_cnt = array_size(g_common_cmd_te_off),
};

static lcd_cmds_sequ g_brightness_cmds = {
    .cmd = g_common_cmd_brightness,
    .cmd_cnt = array_size(g_common_cmd_brightness),
};

static lcd_cmds_sequ g_hbm_brightness_cmds = {
    .cmd = g_common_cmd_hbm_brightness,
    .cmd_cnt = array_size(g_common_cmd_hbm_brightness),
};

static lcd_cmds_sequ g_edo_454x454_screen_init_on_cmds = {
    .cmd = (lcd_cmd_sequ *)g_edo_454x454_screen_init_on,
    .cmd_cnt = array_size(g_edo_454x454_screen_init_on),
};

static lcd_cmds_sequ g_edo_466x466_screen_init_on_cmds = {
    .cmd = (lcd_cmd_sequ *)g_edo_466x466_screen_init_on,
    .cmd_cnt = array_size(g_edo_466x466_screen_init_on),
};

static lcd_cmds_sequ g_boe_454x454_screen_init_on_cmds = {
    .cmd = (lcd_cmd_sequ *)g_boe_454x454_screen_init_on,
    .cmd_cnt = array_size(g_boe_454x454_screen_init_on),
};

static lcd_cmds_sequ g_286x482_screen_init_on_cmds = {
    .cmd = (lcd_cmd_sequ *)g_286x482_screen_init_on,
    .cmd_cnt = array_size(g_286x482_screen_init_on),
};

static lcd_cmds_sequ g_240x240_screen_init_on_cmds = {
    .cmd = (lcd_cmd_sequ *)g_240x240_screen_init_on,
    .cmd_cnt = array_size(g_240x240_screen_init_on),
};

static lcd_cmds_sequ g_co5300_1v8_screen_init_on_cmds = {
    .cmd = (lcd_cmd_sequ *)g_co5300_1v8_screen_init_on,
    .cmd_cnt = array_size(g_co5300_1v8_screen_init_on),
};

static lcd_cmds_sequ g_co5300_3v3_screen_init_on_cmds = {
    .cmd = (lcd_cmd_sequ *)g_co5300_3v3_screen_init_on,
    .cmd_cnt = array_size(g_co5300_3v3_screen_init_on),
};

static lcd_cmds_sequ g_sleep_in_cmds = {
    .cmd = (lcd_cmd_sequ *)g_common_cmd_sleep_in,
    .cmd_cnt = array_size(g_common_cmd_sleep_in),
};

static lcd_cmds_sequ g_sleep_out_cmds = {
    .cmd = (lcd_cmd_sequ *)g_common_cmd_sleep_out,
    .cmd_cnt = array_size(g_common_cmd_sleep_out),
};

static lcd_cmds_sequ g_320x360_screen_init_on_cmds = {
    .cmd = (lcd_cmd_sequ *)g_320x360_screen_init_on,
    .cmd_cnt = array_size(g_320x360_screen_init_on),
};

static lcd_cmds_sequ g_408x480_screen_init_on_cmds = {
    .cmd = (lcd_cmd_sequ *)g_408x480_screen_init_on,
    .cmd_cnt = array_size(g_408x480_screen_init_on),
};

static lcd_cmds_sequ g_466x466_v1_screen_init_on_cmds = {
    .cmd = (lcd_cmd_sequ *)g_466x466_v1_screen_init_on,
    .cmd_cnt = array_size(g_466x466_v1_screen_init_on),
};

static lcd_cmds_sequ g_320x360_edo_screen_init_on_cmds = {
    .cmd = (lcd_cmd_sequ *)g_320x360_edo_screen_init_on,
    .cmd_cnt = array_size(g_320x360_edo_screen_init_on),
};

PLT_SEC_ITCM_R static lcd_drv_cfg g_lcd_driver_ops[] = {
#ifdef GPU_3322
    {
        .bus_type             = BUS_DISPLAY_MIPI,
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_gh7006_800x480_config),
        .x_start              = 0,
        .y_start              = 0,
        .fps                  = 60,
        .lcd_id               = LCD_GH7006_800x480_SCREEN_ID,
        .display_on_cmds      = &g_gh7006_800x480_screen_init_on,
        .display_off_cmds     = &g_display_off_cmds,
        .enter_idle_cmds      = &g_idle_in_cmds,
        .exit_idle_cmds       = &g_idle_out_cmds,
        .te_on_cmds           = &g_te_on_cmds,
        .te_off_cmds          = &g_te_off_cmds,
        .set_brightnes_cmds   = &g_brightness_cmds,
        .sleep_in_cmds        = &g_sleep_in_cmds,
        .sleep_out_cmds       = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0x04,
        .lcd_id_info.read_len = 3,
    },
    {
        .bus_type             = BUS_DISPLAY_MIPI,
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_st7701_800x480_config),
        .x_start              = 0,
        .y_start              = 0,
        .fps                  = 60,
        .lcd_id               = LCD_800x480_SCREEN_ID,
        .display_on_cmds      = &g_st7701_800x480_screen_init_on,
        .display_off_cmds     = &g_display_off_cmds,
        .enter_idle_cmds      = &g_idle_in_cmds,
        .exit_idle_cmds       = &g_idle_out_cmds,
        .te_on_cmds           = &g_te_on_cmds,
        .te_off_cmds          = &g_te_off_cmds,
        .set_brightnes_cmds   = &g_brightness_cmds,
        .sleep_in_cmds        = &g_sleep_in_cmds,
        .sleep_out_cmds       = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0x04,
        .lcd_id_info.read_len = 3,
    },
    {
        .bus_type             = BUS_DISPLAY_MIPI,
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_st7102_480x854_config),
        .x_start              = 0,
        .y_start              = 0,
        .fps                  = 60,
        .lcd_id               = LCD_ST7102_480X854_SCREEN_ID,
        .display_on_cmds      = &g_st7102_480x854_screen_init_on,
        .display_off_cmds     = &g_display_off_cmds,
        .enter_idle_cmds      = &g_idle_in_cmds,
        .exit_idle_cmds       = &g_idle_out_cmds,
        .te_on_cmds           = &g_te_on_cmds,
        .te_off_cmds          = &g_te_off_cmds,
        .set_brightnes_cmds   = &g_brightness_cmds,
        .sleep_in_cmds        = &g_sleep_in_cmds,
        .sleep_out_cmds       = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0x04,
        .lcd_id_info.read_len = 3,
    },
    {
        .bus_type = BUS_DISPLAY_MIPI,
        .bus_cfg = (combo_dev_cfg_t *)(&g_panel_GH7002_1024X600_video_config),
        .x_start = 0,
        .y_start = 0,
        .fps = 60,
        .lcd_id = LCD_GH7002_1024X6000_SCREEN_ID,
        .display_on_cmds = &g_GH7002_1024X600_screen_init_on,
        .display_off_cmds = &g_display_off_cmds,
        .enter_idle_cmds = &g_idle_in_cmds,
        .exit_idle_cmds = &g_idle_out_cmds,
        .te_on_cmds = &g_te_on_cmds,
        .te_off_cmds = &g_te_off_cmds,
        .set_brightnes_cmds = &g_brightness_cmds,
        .sleep_in_cmds = &g_sleep_in_cmds,
        .sleep_out_cmds = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0x04,
        .lcd_id_info.read_len = 3,
    },
#endif
    {
        .bus_type             = BUS_DISPLAY_MIPI,
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_320x360_60_config),
        .x_start              = 0,
        .y_start              = 0,
        .fps                  = 60,
        .lcd_id               = LCD_320x360_EDO_SCREEN_ID,
        .display_on_cmds      = &g_320x360_edo_screen_init_on_cmds,
        .display_off_cmds     = &g_display_off_cmds,
        .enter_idle_cmds      = &g_idle_in_cmds,
        .exit_idle_cmds       = &g_idle_out_cmds,
        .te_on_cmds           = &g_te_on_cmds,
        .te_off_cmds          = &g_te_off_cmds,
        .set_brightnes_cmds   = &g_brightness_cmds,
        .hbm_brightness_cmds  = &g_hbm_brightness_cmds,
        .sleep_in_cmds        = &g_sleep_in_cmds,
        .sleep_out_cmds       = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0xdb,
        .lcd_id_info.read_len = 1,
    },
    {
        .bus_type             = BUS_DISPLAY_MIPI,
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_320x360_60_config),
        .x_start              = 0,
        .y_start              = 0,
        .fps                  = 60,
        .lcd_id               = LCD_320x360_SCREEN_ID,
        .display_on_cmds      = &g_320x360_screen_init_on_cmds,
        .display_off_cmds     = &g_display_off_cmds,
        .enter_idle_cmds      = &g_idle_in_cmds,
        .exit_idle_cmds       = &g_idle_out_cmds,
        .te_on_cmds           = &g_te_on_cmds,
        .te_off_cmds          = &g_te_off_cmds,
        .set_brightnes_cmds   = &g_brightness_cmds,
        .hbm_brightness_cmds  = &g_hbm_brightness_cmds,
        .sleep_in_cmds        = &g_sleep_in_cmds,
        .sleep_out_cmds       = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0xdb,
        .lcd_id_info.read_len = 1,
    },
    {
        .bus_type             = BUS_DISPLAY_MIPI,
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_st7701_480x854_config),
        .x_start              = 0,
        .y_start              = 0,
        .fps                  = 60,
        .lcd_id               = LCD_480x854_SCREEN_ID,
        .display_on_cmds      = &g_st7701_480x854_screen_init_on,
        .display_off_cmds     = &g_display_off_cmds,
        .enter_idle_cmds      = &g_idle_in_cmds,
        .exit_idle_cmds       = &g_idle_out_cmds,
        .te_on_cmds           = &g_te_on_cmds,
        .te_off_cmds          = &g_te_off_cmds,
        .set_brightnes_cmds   = &g_brightness_cmds,
        .sleep_in_cmds        = &g_sleep_in_cmds,
        .sleep_out_cmds       = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0x04,
        .lcd_id_info.read_len = 3,
    },
    {
        .bus_type             = BUS_DISPLAY_MIPI,
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_408x480_60_config),
        .x_start              = 0,
        .y_start              = 0,
        .fps                  = 60,
        .lcd_id               = LCD_408x480_SCREEN_ID,
        .display_on_cmds      = &g_408x480_screen_init_on_cmds,
        .display_off_cmds     = &g_display_off_cmds,
        .enter_idle_cmds      = &g_idle_in_cmds,
        .exit_idle_cmds       = &g_idle_out_cmds,
        .te_on_cmds           = &g_te_on_cmds,
        .te_off_cmds          = &g_te_off_cmds,
        .set_brightnes_cmds   = &g_brightness_cmds,
        .hbm_brightness_cmds  = &g_hbm_brightness_cmds,
        .sleep_in_cmds        = &g_sleep_in_cmds,
        .sleep_out_cmds       = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0x04,
        .lcd_id_info.read_len = 2,
    },
    {
        .bus_type             = BUS_DISPLAY_MIPI,
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_466x466_60_config),
        .x_start              = 0x6,
        .y_start              = 0,
        .fps                  = 60,
        .lcd_id               = LCD_EDO_466X466_SCREEN_ID1,
        .display_on_cmds      = &g_edo_466x466_screen_init_on_cmds,
        .display_off_cmds     = &g_display_off_cmds,
        .enter_idle_cmds      = &g_idle_in_cmds,
        .exit_idle_cmds       = &g_idle_out_cmds,
        .te_on_cmds           = &g_te_on_cmds,
        .te_off_cmds          = &g_te_off_cmds,
        .set_brightnes_cmds   = &g_brightness_cmds,
        .hbm_brightness_cmds  = &g_hbm_brightness_cmds,
        .sleep_in_cmds        = &g_sleep_in_cmds,
        .sleep_out_cmds       = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0xdb,
        .lcd_id_info.read_len = 3,
    },
    {
        .bus_type             = BUS_DISPLAY_MIPI,
    #ifdef GPU_SUPPORT_VIDEO_MODE
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_466x466_60_video_config),
    #else
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_466x466_60_config),
    #endif
        .x_start              = 0x6,
        .y_start              = 0,
        .fps                  = 60,
        .lcd_id               = LCD_EDO_466X466_SCREEN_ID2,
        .display_on_cmds      = &g_edo_466x466_screen_init_on_cmds,
        .display_off_cmds     = &g_display_off_cmds,
        .enter_idle_cmds      = &g_idle_in_cmds,
        .exit_idle_cmds       = &g_idle_out_cmds,
        .te_on_cmds           = &g_te_on_cmds,
        .te_off_cmds          = &g_te_off_cmds,
        .set_brightnes_cmds   = &g_brightness_cmds,
        .hbm_brightness_cmds  = &g_hbm_brightness_cmds,
        .sleep_in_cmds        = &g_sleep_in_cmds,
        .sleep_out_cmds       = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0xdb,
        .lcd_id_info.read_len = 3,
    },
    {
        .bus_type             = BUS_DISPLAY_MIPI,
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_454x454_60_config),
        .x_start              = 0xe,
        .y_start              = 0,
        .fps                  = 60,
        .lcd_id               = LCD_EDO_454X454_SCREEN_ID,
        .display_on_cmds      = &g_edo_454x454_screen_init_on_cmds,
        .display_off_cmds     = &g_display_off_cmds,
        .enter_idle_cmds      = &g_idle_in_cmds,
        .exit_idle_cmds       = &g_idle_out_cmds,
        .te_on_cmds           = &g_te_on_cmds,
        .te_off_cmds          = &g_te_off_cmds,
        .set_brightnes_cmds   = &g_brightness_cmds,
        .hbm_brightness_cmds  = &g_hbm_brightness_cmds,
        .sleep_in_cmds        = &g_sleep_in_cmds,
        .sleep_out_cmds       = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0x04,
        .lcd_id_info.read_len = 3,
    },
    {
        .bus_type             = BUS_DISPLAY_MIPI,
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_454x454_60_config),
        .x_start              = 0xc,
        .y_start              = 0,
        .fps                  = 60,
        .lcd_id               = LCD_BOE_454X454_SCREEN_ID,
        .display_on_cmds      = &g_boe_454x454_screen_init_on_cmds,
        .display_off_cmds     = &g_display_off_cmds,
        .enter_idle_cmds      = &g_idle_in_cmds,
        .exit_idle_cmds       = &g_idle_out_cmds,
        .te_on_cmds           = &g_te_on_cmds,
        .te_off_cmds          = &g_te_off_cmds,
        .set_brightnes_cmds   = &g_brightness_cmds,
        .hbm_brightness_cmds  = &g_hbm_brightness_cmds,
        .sleep_in_cmds        = &g_sleep_in_cmds,
        .sleep_out_cmds       = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0x04,
        .lcd_id_info.read_len = 3,
    },
    {
        .bus_type             = BUS_DISPLAY_MIPI,
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_axs15231_320x480_config),
        .x_start              = 0,
        .y_start              = 0,
        .fps                  = 60,
        .lcd_id               = LCD_axs15231_320x480_SCREEN_ID,
        .display_on_cmds      = &g_axs15231_320x480_screen_init_on,
        .display_off_cmds     = &g_display_off_cmds,
        .enter_idle_cmds      = &g_idle_in_cmds,
        .exit_idle_cmds       = &g_idle_out_cmds,
        .te_on_cmds           = &g_te_on_cmds,
        .te_off_cmds          = &g_te_off_cmds,
        .set_brightnes_cmds   = &g_brightness_cmds,
        .sleep_in_cmds        = &g_sleep_in_cmds,
        .sleep_out_cmds       = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0xda,
        .lcd_id_info.read_len = 1,
    },
    {
        .bus_type             = BUS_DISPLAY_QSPI,
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_466x466_qspi_config),
        .x_start              = 0,
        .y_start              = 0,
        .fps                  = 60,
        .lcd_id               = LCD_CO5300_1V8_SCREEN_ID,
        .display_on_cmds      = &g_co5300_1v8_screen_init_on_cmds,
        .display_off_cmds     = &g_display_off_cmds,
        .enter_idle_cmds      = &g_idle_in_cmds,
        .exit_idle_cmds       = &g_idle_out_cmds,
        .te_on_cmds           = &g_te_on_cmds,
        .te_off_cmds          = &g_te_off_cmds,
        .set_brightnes_cmds   = &g_brightness_cmds,
        .hbm_brightness_cmds  = &g_hbm_brightness_cmds,
        .sleep_in_cmds        = &g_sleep_in_cmds,
        .sleep_out_cmds       = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0x04,
        .lcd_id_info.read_len = 2,
    },
    {
        .bus_type             = BUS_DISPLAY_QSPI,
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_286x482_qspi_config),
        .x_start              = 0,
        .y_start              = 0,
        .fps                  = 60,
        .lcd_id               = LCD_286X482_SCREEN_ID,
        .display_on_cmds      = &g_286x482_screen_init_on_cmds,
        .display_off_cmds     = &g_display_off_cmds,
        .enter_idle_cmds      = &g_idle_in_cmds,
        .exit_idle_cmds       = &g_idle_out_cmds,
        .te_on_cmds           = &g_te_on_cmds,
        .te_off_cmds          = &g_te_off_cmds,
        .set_brightnes_cmds   = &g_brightness_cmds,
        .sleep_in_cmds        = &g_sleep_in_cmds,
        .sleep_out_cmds       = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0xdb,
        .lcd_id_info.read_len = 1,
    },
    {
        .bus_type             = BUS_DISPLAY_SPI,
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_gc9307_240x284_spi_config),
        .x_start              = 0,
        .y_start              = 0,
        .fps                  = 60,
        .lcd_id               = LCD_GC9307_SPI_SCREEN_ID,
        .display_on_cmds      = &g_gc9307_240x284_screen_init_on,
        .display_off_cmds     = &g_display_off_cmds,
        .enter_idle_cmds      = &g_idle_in_cmds,
        .exit_idle_cmds       = &g_idle_out_cmds,
        .te_on_cmds           = &g_te_on_cmds,
        .te_off_cmds          = &g_te_off_cmds,
        .set_brightnes_cmds   = &g_brightness_cmds,
        .hbm_brightness_cmds  = &g_hbm_brightness_cmds,
        .sleep_in_cmds        = &g_sleep_in_cmds,
        .sleep_out_cmds       = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0x04,
        .lcd_id_info.read_len = 3,
    },
    {
        .bus_type             = BUS_DISPLAY_SPI,
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_240x240_qspi_config),
        .x_start              = 0,
        .y_start              = 0,
        .fps                  = 60,
        .lcd_id               = LCD_240X240_SCREEN_ID,
        .display_on_cmds      = &g_240x240_screen_init_on_cmds,
        .display_off_cmds     = &g_display_off_cmds,
        .enter_idle_cmds      = &g_idle_in_cmds,
        .exit_idle_cmds       = &g_idle_out_cmds,
        .te_on_cmds           = &g_te_on_cmds,
        .te_off_cmds          = &g_te_off_cmds,
        .set_brightnes_cmds   = &g_brightness_cmds,
        .sleep_in_cmds        = &g_sleep_in_cmds,
        .sleep_out_cmds       = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0xdb,
        .lcd_id_info.read_len = 1,
    },
    {
        .bus_type             = BUS_DISPLAY_QSPI,
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_466x466_qspi_config),
        .x_start              = 0,
        .y_start              = 0,
        .fps                  = 60,
        .lcd_id               = LCD_CO5300_QSPI_SCREEN_ID,
        .display_on_cmds      = &g_co5300_3v3_screen_init_on_cmds,
        .display_off_cmds     = &g_display_off_cmds,
        .enter_idle_cmds      = &g_idle_in_cmds,
        .exit_idle_cmds       = &g_idle_out_cmds,
        .te_on_cmds           = &g_te_on_cmds,
        .te_off_cmds          = &g_te_off_cmds,
        .set_brightnes_cmds   = &g_brightness_cmds,
        .hbm_brightness_cmds  = &g_hbm_brightness_cmds,
        .sleep_in_cmds        = &g_sleep_in_cmds,
        .sleep_out_cmds       = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0x04,
        .lcd_id_info.read_len = 2,
    },
    {
        .bus_type             = BUS_DISPLAY_MIPI,
        .bus_cfg              = (combo_dev_cfg_t *)(&g_panel_466x466_v1_config),
        .x_start              = 0x6,
        .y_start              = 0,
        .fps                  = 60,
        .lcd_id               = LCD_CO5300_MIPI_SCREEN_ID,
        .display_on_cmds      = &g_466x466_v1_screen_init_on_cmds,
        .display_off_cmds     = &g_display_off_cmds,
        .enter_idle_cmds      = &g_idle_in_cmds,
        .exit_idle_cmds       = &g_idle_out_cmds,
        .te_on_cmds           = &g_te_on_cmds,
        .te_off_cmds          = &g_te_off_cmds,
        .set_brightnes_cmds   = &g_brightness_cmds,
        .hbm_brightness_cmds  = &g_hbm_brightness_cmds,
        .sleep_in_cmds        = &g_sleep_in_cmds,
        .sleep_out_cmds       = &g_sleep_out_cmds,
        .lcd_id_info.reg_addr = 0x4,
        .lcd_id_info.read_len = 2,
    },
};

lcd_drv_cfg *lcd_get_driver_ops(void)
{
    return g_lcd_driver_ops;
}

uint32_t lcd_get_ops_num(void)
{
    return array_size(g_lcd_driver_ops);
}

combo_dev_cfg_t *lcd_get_bus_escape_config(void)
{
    return g_lcd_driver_ops[0].bus_cfg;
}
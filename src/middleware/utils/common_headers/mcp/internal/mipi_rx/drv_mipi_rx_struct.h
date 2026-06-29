/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: mipi rx struct
 * Author: AuthorNameMagicTag
 * Create: 2021-09-23
 */

#ifndef __DRV_MIPI_RX_STRUCT_H__
#define __DRV_MIPI_RX_STRUCT_H__

#include "drv_mcp_type.h"

#define EXT_MIPI_RX_MIPI_LANE_NUM                4
#define EXT_MIPI_RX_LVDS_LANE_NUM                4

#define EXT_MIPI_RX_WDR_VC_NUM                   4
#define EXT_MIPI_RX_SYNC_CODE_NUM                4
#define EXT_MIPI_RX_MAX_DEV_NUM                  2

typedef enum {
    MIPI_RX_CIS_INCK_2MHZ      = 0x0,  // External cmos sensor inck is 2MHz
    MIPI_RX_CIS_INCK_3MHZ      = 0x1,  // External cmos sensor inck is 3MHz
    MIPI_RX_CIS_INCK_6MHZ      = 0x2,  // External cmos sensor inck is 6MHz
    MIPI_RX_CIS_INCK_12MHZ     = 0x3,  // External cmos sensor inck is 12MHz
    MIPI_RX_CIS_INCK_13_5MHZ   = 0x4,  // External cmos sensor inck is 13.5MHz
    MIPI_RX_CIS_INCK_18MHZ     = 0x5,  // External cmos sensor inck is 18MHz
    MIPI_RX_CIS_INCK_19_2MHZ   = 0x6,  // External cmos sensor inck is 19.2MHz
    MIPI_RX_CIS_INCK_24MHZ     = 0x7,  // External cmos sensor inck is 24MHz
    MIPI_RX_CIS_INCK_27MHZ     = 0x8,  // External cmos sensor inck is 27MHz
    MIPI_RX_CIS_INCK_37_125MHZ = 0x9,  // External cmos sensor inck is 37.125MHz
    MIPI_RX_CIS_INCK_MAX,
} ext_mipi_rx_cis_inck;

typedef enum {
    MIPI_RX_LANE_DIVIDE_MODE_0 = 0,
    MIPI_RX_LANE_DIVIDE_MODE_1 = 1,
    MIPI_RX_LANE_DIVIDE_MODE_MAX
} ext_mipi_rx_lane_mode;

typedef enum {
    MIPI_RX_INPUT_MODE_MIPI = 0x0, /* mipi */
    MIPI_RX_INPUT_MODE_SUBLVDS = 0x1, /* SUB_LVDS */
    MIPI_RX_INPUT_MODE_HISPI = 0x2, /* HISPI */

    MIPI_RX_INPUT_MODE_MAX
} ext_mipi_rx_input_mode;

typedef enum {
    MIPI_RX_DATA_RATE_X1 = 0, /* output 1 pixel per clock */

    MIPI_RX_DATA_RATE_MAX
} ext_mipi_rx_data_rate;

typedef struct {
    td_s32 x;
    td_s32 y;
    td_u32 width;
    td_u32 height;
} ext_mipi_rx_img_rect;

typedef struct {
    td_u32 width;
    td_u32 height;
} ext_mipi_rx_img_size;

typedef enum {
    MIPI_RX_DATA_TYPE_RAW_8BIT = 0,
    MIPI_RX_DATA_TYPE_RAW_10BIT,
    MIPI_RX_DATA_TYPE_RAW_12BIT,
    MIPI_RX_DATA_TYPE_RAW_14BIT,
    MIPI_RX_DATA_TYPE_RAW_16BIT,
    MIPI_RX_DATA_TYPE_YUV420_8BIT_NORMAL,
    MIPI_RX_DATA_TYPE_YUV420_8BIT_LEGACY,
    MIPI_RX_DATA_TYPE_YUV422_8BIT,
    MIPI_RX_DATA_TYPE_YUV422_PACKED, /* yuv422 8bit transform user define 16bit raw */
    MIPI_RX_DATA_TYPE_MAX
} ext_mipi_rx_data_type;

/* MIPI D_PHY WDR MODE defines */
typedef enum {
    MIPI_RX_MIPI_WDR_MODE_NONE = 0x0,
    MIPI_RX_MIPI_WDR_MODE_VC = 0x1, /* Virtual Channel */
    MIPI_RX_MIPI_WDR_MODE_DT = 0x2, /* Data Type */
    MIPI_RX_MIPI_WDR_MODE_DOL = 0x3, /* DOL Mode */
    MIPI_RX_MIPI_WDR_MODE_MAX
} ext_mipi_rx_wdr_mode;

typedef struct {
    ext_mipi_rx_data_type input_data_type; /* data type: 8/10/12/14/16 bit */
    ext_mipi_rx_wdr_mode wdr_mode; /* MIPI WDR mode */
    td_s16 lane_id[EXT_MIPI_RX_MIPI_LANE_NUM]; /* lane_id: -1 - disable */

    union {
        td_s16 data_type[EXT_MIPI_RX_WDR_VC_NUM]; /* used by the MIPI_RX_MIPI_WDR_MODE_DT */
    };
} ext_mipi_dev_attr;

typedef enum {
    MIPI_RX_LVDS_WDR_MODE_NONE = 0x0,
    MIPI_RX_LVDS_WDR_MODE_2F = 0x1,
    MIPI_RX_LVDS_WDR_MODE_3F = 0x2,
    MIPI_RX_LVDS_WDR_MODE_4F = 0x3,
    MIPI_RX_LVDS_WDR_MODE_DOL_2F = 0x4,
    MIPI_RX_LVDS_WDR_MODE_DOL_3F = 0x5,
    MIPI_RX_LVDS_WDR_MODE_DOL_4F = 0x6,
    MIPI_RX_LVDS_WDR_MODE_MAX
} ext_lvds_wdr_mode;

typedef enum {
    MIPI_RX_LVDS_SYNC_MODE_SOF = 0, /* sensor SOL, EOL, SOF, EOF */
    MIPI_RX_LVDS_SYNC_MODE_SAV, /* SAV, EAV */
    MIPI_RX_LVDS_SYNC_MODE_MAX
} ext_lvds_sync_mode;

typedef enum {
    MIPI_RX_LVDS_VSYNC_NORMAL = 0x00,
    MIPI_RX_LVDS_VSYNC_SHARE = 0x01,
    MIPI_RX_LVDS_VSYNC_HCONNECT = 0x02,
    MIPI_RX_LVDS_VSYNC_MAX
} ext_lvds_vsync_type;

typedef struct {
    td_u32 dev;
    ext_mipi_rx_lane_mode mode;
} ext_mipi_rx_dev_lane_mode;

typedef struct {
    ext_lvds_vsync_type sync_type;

    /* hconnect vsync blanking len, valid when the sync_type is MIPI_RX_LVDS_VSYNC_HCONNECT */
    td_u16 hblank1;
    td_u16 hblank2;
} ext_lvds_vsync_attr;

typedef enum {
    MIPI_RX_LVDS_FID_NONE = 0x00,
    MIPI_RX_LVDS_FID_IN_SAV = 0x01, /* frame identification id in SAV 4th */
    MIPI_RX_LVDS_FID_IN_DATA = 0x02, /* frame identification id in first data */
    MIPI_RX_LVDS_FID_MAX
} ext_lvds_fid_type;

typedef struct {
    ext_lvds_fid_type fid_type;

    /*
     * Sony DOL has the Frame Information Line, in DOL H-Connection mode,
     * should configure this flag as false to disable output the Frame Information Line
     */
    td_u8 output_fil;
} ext_lvds_fid_attr;

typedef enum {
    MIPI_RX_LVDS_ENDIAN_LITTLE = 0x0,
    MIPI_RX_LVDS_ENDIAN_BIG = 0x1,
    MIPI_RX_LVDS_ENDIAN_MAX
} ext_lvds_bit_endian;

typedef struct {
    ext_mipi_rx_data_type input_data_type; /* data type: 8/10/12/14 bit */
    ext_lvds_wdr_mode wdr_mode; /* WDR mode */

    ext_lvds_sync_mode sync_mode; /* sync mode: SOF, SAV */
    ext_lvds_vsync_attr vsync_attr; /* normal, share, hconnect */
    ext_lvds_fid_attr fid_attr; /* frame identification code */

    ext_lvds_bit_endian data_endian; /* data endian: little/big */
    ext_lvds_bit_endian sync_code_endian; /* sync code endian: little/big */
    td_s16 lane_id[EXT_MIPI_RX_LVDS_LANE_NUM]; /* lane_id: -1 - disable */

    /*
     * each vc has 4 params, sync_code[i]:
     * sync_mode is SYNC_MODE_SOF: SOF, EOF, SOL, EOL
     * sync_mode is SYNC_MODE_SAV: invalid sav, invalid eav, valid sav, valid eav
     */
    td_u16 sync_code[EXT_MIPI_RX_LVDS_LANE_NUM][EXT_MIPI_RX_WDR_VC_NUM][EXT_MIPI_RX_SYNC_CODE_NUM];
} ext_lvds_dev_attr;

typedef struct {
    td_u32 dev_id; /* device number */
    ext_mipi_rx_input_mode input_mode; /* input mode: MIPI/SUBLVDS/HISPI */
    ext_mipi_rx_data_rate data_rate;
    /* MIPI Rx device crop area (corresponding to the oringnal sensor input image size) */
    ext_mipi_rx_img_rect img_rect;
    ext_mipi_rx_lane_mode lane_mode;
    union {
        ext_mipi_dev_attr mipi_attr;
        ext_lvds_dev_attr lvds_attr; /* SUBLVDS/HISPI */
    } u;
} ext_mipi_rx_dev_attr;

typedef struct {
    td_bool enable;
    ext_mipi_rx_cis_inck inck;
} ext_mipi_rx_cis_mclk_attr;

#endif /* __DRV_MIPI_RX_STRUCT_H__ */

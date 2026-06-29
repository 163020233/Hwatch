/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: afe_reg.h for DallasV100
 * Author: audio
 */

#ifndef __REG_AFE_H__
#define __REG_AFE_H__

#include "reg_addr.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifndef array_size
#define array_size(array_var_name) (sizeof((array_var_name)) / sizeof((array_var_name)[0]))
#endif

typedef enum {
    AUDIO_DN_CHN_0,
    AUDIO_DN_CHN_1,
    AUDIO_DN_CHN_2,
    AUDIO_DN_CHN_3,
    AUDIO_DN_CHN_4,

    AUDIO_DN_CHN_0_1, /* fifo0双声道，选择chn0和chn1 */

    AUDIO_DN_CHN_MAX,
} audio_dn_chn_id;

typedef enum {
    AUDIO_DN_FS_SEL_16k = 0,
    AUDIO_DN_FS_SEL_32k = 1,
    AUDIO_DN_FS_SEL_48k = 2,
    AUDIO_DN_FS_SEL_96k = 3,
    AUDIO_DN_FS_SEL_192k = 7,

    AUDIO_DN_FS_SEL_MAX,
} audio_dn_fs_sel;

typedef enum {
    AUDIO_DN_SRCUP_MODE_3 = 0,
    AUDIO_DN_SRCUP_MODE_6 = 1,
    AUDIO_DN_SRCUP_MODE_12 = 2,
    AUDIO_DN_SRCUP_MODE_2 = 3,

    AUDIO_DN_SRCUP_MODE_MAX = 3,
} audio_dn_srcup_mode;

typedef enum {
    MIC_CH0,
    MIC_CH1,
    MIC_CH2,
    MIC_CH3,
    MIC_CH4,

    MIC_CH_MAX,
} mic_ch_id;

enum {
    DEC_CHN0,
    DEC_CHN1,
    DEC_CHN2,
    DEC_CHN3,

    AUDIO_DN_EC0,
    AUDIO_DN_EC1,
};

typedef enum {
    MIC_UP_FS_SEL_16K = 0,
    MIC_UP_FS_SEL_32K = 1,
    MIC_UP_FS_SEL_48K = 2,
    MIC_UP_FS_SEL_96K = 3,

    MIC_UP_FS_SEL_MAX,
} mic_up_fs_sel;

/* mic srcdn mode */
typedef enum {
    MIC_SRCDN_MODE_03 = 0,
    MIC_SRCDN_MODE_06,
    MIC_SRCDN_MODE_12,
    MIC_SRCDN_MODE_02,
} mic_srcdn_mode;

#define CDC_REG_BASE                            CODEC_REG_BASE_ADDR
#define CDC_REG_I2S_TX_DATA_SEL_OFFSET          0x00000364

#define CDC_REG_NONUNI_CTRL_OFFSET              0x00000004
#define CDC_REG_UPLINK_CH_RST_REQ_OFFSET        0x00000010
#define CDC_REG_MIC_UP_AFIFO_CTRL_OFFSET        0x0000025c
#define CDC_REG_MIC_UP_AFIFO_DBG_OFFSET         0x00000274
#define CDC_REG_DOUBLE_EAR_REG2_OFFSET          0x00003238
#define CDC_REG_DOUBLE_EAR_SYNC_CFG1_OFFSET     0x00003254
#define CDC_REG_DOUBLE_EAR_CFG1_OFFSET          0x0000326C

#define CDC_REG_RST_CODEC_SW_N_OFFSET           0x00000008
    /*
        RST_CODEC_SW_N  CODEC复位寄存器              0x0008  reserved        31:7    reserved
                                        codec_ana_clken 6       "codec_ana时钟控制。
        0：关闭时钟；
        1：打开时钟。"
                                        func_wakeup_clken       5       唤醒功能总使能信号，高有效;
                                        func_anc_clken  4       ANC总使能信号，高有效；
                                        func_i2s_clken  3       I2S总使能信号，高有效；
                                        func_dn_clken   2       下行通路总使能信号，高有效；
                                        func_up_clken   1       上行通路总使能信号，高有效；
                                        rst_codec_sw_n  0       CODEC软复位寄存器，低有效；

     */
#define CDC_REG_UPLINK_AFIFO_CLKEN_OFFSET       0x00000030 /* 1bit:MIC_AFIFO_NUM */
#define CDC_REG_MIC_DEC_CLKEN_OFFSET            0x000001E0 /* 1bit:MIC_CH_NUM */ /* pga clken\dec clken */
#define CDC_REG_MIC_UP_FS_CTRL_OFFSET           0x00000388 /* MIC_CH */
#define CDC_REG_MIC_DEC_FILTER_BYPASS_OFFSET    0x000003C4 /* MIC_CH */
#define CDC_REG_MIC_PGA_CTRL0_OFFSET            0x00000484 /* MIC_CH */
#define CDC_REG_MIC_PGA_GAIN_FLUSH_OFFSET       0x0000321C /* 1bit:MIC_CH_NUM */ /* pga gain flush mask\gain flush */
    /*
        MIC_PGA_CTRL0       MIC_CH  0x4*MIC_CH_NUM+0x484    reserved    31:16
                        mic_pga_fade_wait_th    15:8        "fade开始前的等待时间控制信号：单位：sample个数
        配置范围：0-255；
        等待时间：配置值/fs；"
                        reserved    7:5
                        mic_pga_fade_start  4
                        mic_pga_linear_auto_en  3       PGA模块step模式基于过零率检测的自动模式使能，高有效
                        mic_pga_linear_zero_en  2       PGA模块step模式过零切模式使能，高有效
                        reserved    1
                        mic_pga_linear_byp  0       "PGA模块step模式bypass信号
        1'b0:不bypass
        1'b1:bypass"
     */
#define CDC_REG_MIC_PGA_CTRL1_OFFSET            0x000004B4 /* MIC_CH */
    /*
        MIC_PGA_CTRL1       MIC_CH  0x4*MIC_CH_NUM+0x4B4    mic_pga_linear_thre_id  31:30
        "PGA阈值选择信号，用于过零检测。其中每个pga通过使用其中2bit的id选择四种场景的阈值
        0：选择pga_thre_ctrl[23:0]
        1：选择pga_thre_ctrl[47:24]
        2：选择pga_thre_ctrl[71:48]
        3：选择pga_thre_ctrl[95:72]"
                        reserved    29:26
                        mic_pga_linear_gain 25:0        PGA模块step模式增益配置，线性增益，S6.20

     */
#define CDC_REG_MIC_PGA_CTRL_OFFSET             0x00000514 /* MIC_CH */
    /*
        MIC_PGA_CTRL    MIC的上行通路PGA控制寄存器    MIC_CH  0x4*MIC_CH_NUM+0x514    mic_pga_gain    31:24
        "PGA增益配置，单位db，配置范围: -120~60dB（bit7为符号位）
        例如：60dB配置成0x3C；
                   0dB配置为0x00；
                   -120dB配置为0x88；"
                        mic_pga_cfg 23:21       "PGA三种功能使能开关，淡入淡出，抗削波，小信号
        bit位：
        [2]：淡入淡出功能使能，取值：
        1：淡入淡出功能使能
        0：淡入淡出功能不使能
        [1]:  抗削波功能使能，取值：
        1：抗削波功能使能
        0：抗削波功能不使能
        [0]: 小信号功能使能，取值：
        1：小信号功能使能
        0：小信号功能不使能"
                        mic_pga_fade_in 20:16       PGA模块step模式淡入时间等级设置，time=2^N/fs
                        mic_pga_fade_out    15:11       PGA模块step模式淡出时间等级设置，time=2^N/fs
                        mic_pga_bypass  10      "PGA bypass使能开关
        1'b0: 不bypass
        1'b1: bypass"
                        mic_pga_noise_en    9
                        "PGA噪音使能，高电平代表将噪音使能，即将抵于阈值的小信号放过去，低表示低于阈值的信号钳制到0
        1：不对噪音信号做任何处理
        0：将低于阈值的噪音信号过滤成0"
                        mic_pga_thre_id 8:7     "PGA阈值选择信号，用于过零检测。其中每个pga通过使用其中2bit的id选择四种场景的阈值
        0：选择pga_thre_ctrl[23:0]
        1：选择pga_thre_ctrl[47:24]
        2：选择pga_thre_ctrl[71:48]
        3：选择pga_thre_ctrl[95:72]"
                        mic_pga_zero_num    6:2     PGA过零检测数目，默认值为5'd20
                        mic_pga_linear_sel  1       "PGA线性选择控制
        1'b0: 选择对数淡入淡出
        1'b1: 选择线性淡入淡出"
                        reserved    0       reserved
     */
#define CDC_REG_MIC_SRCDN_CLKEN_OFFSET          0x000001E4
#define CDC_REG_SRCDN_CTRL_OFFSET               0x000005AC
#define CDC_REG_MIC_UPFIFO_INTERLV_EN_OFFSET    0x000005B8 /* 32bit:MIC_AFIFO_NUM */
#define CDC_REG_UPFIFO_INTERLV_FLUSH_OFFSET     0x000005DC
#define CDC_REG_UPFIFO_INTERLV_EN1_OFFSET       0x000005E0 /* 1bit:MIC_AFIFO_NUM */

#define CDC_REG_PGA_THRE_CTRL0                  0x00000314

/*************************************************************EC******************************************************/
#define CDC_REG_MIC_EC_FS_CTRL_OFFSET           0x000005F0 /* MIC_CH */
#define CDC_REG_EC_SRCDN_CTRL_OFFSET            0x00000614 /* SRC */
#define CDC_REG_MIC_EC_SRCDN_CLKEN_OFFSET       0x000001F0 /* SRC CLK */
#define CDC_REG_EC_UPFIFO_INTERLV_EN_OFFSET     0x000005D8 /* 32bit:EC_AFIFO_NUM */
/*************************************************************MAD*****************************************************/
#define CDC_REG_MAD_CTRL_OFFSET                 0x000034B8
    /*
        MAD_CTRL    MAD控制寄存器    MAD_CORE    0x34B8  0x00000042  S   reserved    31:9    -   0x000000        reserved
                                mad_r_wind_sel  8:6 RW  0x1     "bitmatch_MAD移位选择（右移10-mad_wind_sel bit）
                                                                0：右移10bit；
                                                                1：右移9bit；
                                                                ……
                                                                6：右移4bit；
                                                                7：右移3bit；"
                                mad_vad_ao  5   RW  0x0     "vad always-on使能
                                                                1'b0: 不使能
                                                                1'b1: 使能"
                                mad_int_en  4   RW  0x0     "mad中断使能寄存器
                                                                1'b0: 不使能
                                                                1'b1: 使能"
                                mad_l_wind_sel  3:1 RW  0x1 MAD_L_EXIST "bitmatch_MAD移位选择（右移10-mad_wind_sel bit）
                                                                0：右移10bit；
                                                                1：右移9bit；
                                                                ……
                                                                6：右移4bit；
                                                                7：右移3bit；"
                                reserved    0   -   0x0     reserved
     */
#define CDC_REG_MAD_CTRL_MAD_INT_EN_BIT         AUDIO_BIT4

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* __REG_AFE_H__ */

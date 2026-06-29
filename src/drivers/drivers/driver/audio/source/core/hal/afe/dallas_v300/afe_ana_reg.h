/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: header of afe ana reg define
 * Create: 2024-11-11
 */

#ifndef __AFE_ANA_REG_H__
#define __AFE_ANA_REG_H__

/* Define the union u_classsd_dig0_cfg0 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: classd_dig_en
        /*
         * 模块使能寄存器信号
         */
        unsigned int    classd_dig_en         : 1   ; /* [0] */
        // Comment of field: ref_byp
        /*
         * 数字前馈bypass信号
         */
        unsigned int    ref_byp               : 1   ; /* [1] */
        // Comment of field: digff_byp
        /*
         * 数字前馈bypass信号
         */
        unsigned int    digff_byp             : 1   ; /* [2] */
        // Comment of field: pp_byp
        /*
         * Pingpong模块bypass信号
         */
        unsigned int    pp_byp                : 1   ; /* [3] */
        // Comment of field: hyst_byp
        /*
         * 动态迟滞bypass信号
         */
        unsigned int    hyst_byp              : 1   ; /* [4] */
        // Comment of field: hyst_shift_n
        /*
         * Gain移位操作的移位bit，U2.0
         */
        unsigned int    hyst_shift_n          : 2   ; /* [6..5] */
        // Comment of field: pulse_tune_dly
        /*
         * pulse tune 输出与vref对齐
         * 0：保持原状，和vref少三拍
         * 1：打两拍后对齐
         * 2：打三拍后对齐
         * 3：打四拍对齐
         */
        unsigned int    pulse_tune_dly        : 2   ; /* [8..7] */
        // Comment of field: classd_soft_rst_n
        /*
         * classd模块软复位
         * 0：复位
         * 1：解复位
         */
        unsigned int    classd_soft_rst_n     : 1   ; /* [9] */
        // Comment of field: vref_dly
        /*
         * 输出vref信号是否使用codec_puse2时钟（晚1/4周期）打拍
         * 1：打拍
         * 0：不打拍
         */
        unsigned int    vref_dly              : 1   ; /* [10] */
        // Comment of field: data_dly_byp
        /*
         * 输入的data_in_8s信号dly 的bypass
         * 1：不delay，bypass掉
         * 0：delay
         */
        unsigned int    data_dly_byp          : 1   ; /* [11] */
        // Comment of field: data_dly_sel
        /*
         * 输入的data_in_8s信号dly 多少排配置，拍数为data_dly_sel + 1 配置 不能超过21
         */
        unsigned int    data_dly_sel          : 5   ; /* [16..12] */
        // Comment of field: reserved_0
        /*
         * reserved
         */
        unsigned int    reserved_0            : 15  ; /* [31..17] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_classsd_dig0_cfg0;

/* Define the union u_classsd_dig0_cfg1 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: ref_th0
        /*
         * 参考调节信号阈值B，U0.7
         */
        unsigned int    ref_th0               : 7   ; /* [6..0] */
        // Comment of field: reserved_0
        /*
         * reserved
         */
        unsigned int    reserved_0            : 1   ; /* [7] */
        // Comment of field: ref_th1
        /*
         * 参考调节信号阈值A(Max.)，U0.7
         */
        unsigned int    ref_th1               : 7   ; /* [14..8] */
        // Comment of field: reserved_1
        /*
         * reserved
         */
        unsigned int    reserved_1            : 1   ; /* [15] */
        // Comment of field: pp_vth
        /*
         * Pingpong模块输入大小判断阈值，U0.7
         */
        unsigned int    pp_vth                : 7   ; /* [22..16] */
        // Comment of field: reserved_2
        /*
         * reserved
         */
        unsigned int    reserved_2            : 1   ; /* [23] */
        // Comment of field: hyst_th4
        /*
         * 动态迟滞信号阈值Z(Max.)，U0.7
         */
        unsigned int    hyst_th4              : 7   ; /* [30..24] */
        // Comment of field: reserved_3
        /*
         * reserved
         */
        unsigned int    reserved_3            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_classsd_dig0_cfg1;

/* Define the union u_classsd_dig0_cfg2 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: hyst_th0
        /*
         * 动态迟滞开启阈值D(Min.)，U0.7
         */
        unsigned int    hyst_th0              : 7   ; /* [6..0] */
        // Comment of field: reserved_0
        /*
         * reserved
         */
        unsigned int    reserved_0            : 1   ; /* [7] */
        // Comment of field: hyst_th1
        /*
         * 动态迟滞信号阈值C，U0.7
         */
        unsigned int    hyst_th1              : 7   ; /* [14..8] */
        // Comment of field: reserved_1
        /*
         * reserved
         */
        unsigned int    reserved_1            : 1   ; /* [15] */
        // Comment of field: hyst_th2
        /*
         * 动态迟滞信号阈值B，U0.7
         */
        unsigned int    hyst_th2              : 7   ; /* [22..16] */
        // Comment of field: reserved_2
        /*
         * reserved
         */
        unsigned int    reserved_2            : 1   ; /* [23] */
        // Comment of field: hyst_th3
        /*
         * 动态迟滞信号阈值A，U0.7
         */
        unsigned int    hyst_th3              : 7   ; /* [30..24] */
        // Comment of field: reserved_3
        /*
         * reserved
         */
        unsigned int    reserved_3            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_classsd_dig0_cfg2;

/* Define the union u_classsd_dig0_cfg3 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: hyst_v0add
        /*
         * 迟滞电压D(Min.)，S1.7
         */
        unsigned int    hyst_v0add            : 8   ; /* [7..0] */
        // Comment of field: hyst_v1add
        /*
         * 迟滞电压C，S1.7
         */
        unsigned int    hyst_v1add            : 8   ; /* [15..8] */
        // Comment of field: hyst_v2add
        /*
         * 迟滞电压B，S1.7
         */
        unsigned int    hyst_v2add            : 8   ; /* [23..16] */
        // Comment of field: hyst_v3add
        /*
         * 迟滞电压A(Max.)，S1.7
         */
        unsigned int    hyst_v3add            : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_classsd_dig0_cfg3;

/* Define the union u_classsd_dig0_cfg4 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: ref_dyn_n
        /*
         * 动态参考调节n端参考电压code，S1.7
         */
        unsigned int    ref_dyn_n             : 8   ; /* [7..0] */
        // Comment of field: ref_dyn_p
        /*
         * 动态参考调节p端参考电压code，S1.7
         */
        unsigned int    ref_dyn_p             : 8   ; /* [15..8] */
        // Comment of field: reserved_0
        /*
         * reserved
         */
        unsigned int    reserved_0            : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_classsd_dig0_cfg4;

/* Define the union u_classsd_dig0_cfg5 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: ref_coef3
        /*
         * 0.25脉宽动态电压调节幅度，U0.7
         * 方案1：0.25对应配置32(默认)
         * 方案2：0.2对应配置26
         * 位宽待定
         */
        unsigned int    ref_coef3             : 7   ; /* [6..0] */
        unsigned int    reserved_0            : 9   ; /* [15..7] */
        // Comment of field: ref_coef2
        /*
         * 0.5脉宽动态电压调节幅度，U0.7
         * 方案1：0.5对应配置64(默认)
         * 方案2：0.6对应配置77
         * 位宽待定
         */
        unsigned int    ref_coef2             : 7   ; /* [22..16] */
        // Comment of field: reserved_1
        /*
         * reserved
         */
        unsigned int    reserved_1            : 9   ; /* [31..23] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_classsd_dig0_cfg5;

/* Define the union u_classsd_dig0_cfg6 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: cfg_det_en
        /*
         * D2检测使能。静态参数。
         * 0：不使能，D2检测不工作；
         * 1：使能，D2检测工作。
         */
        unsigned int    cfg_det_en            : 1   ; /* [0] */
        // Comment of field: cfg_det_clamp_en
        /*
         * 数据钳位功能使能信号
         * 1：使能；
         * 0：去使能；
         */
        unsigned int    cfg_det_clamp_en      : 1   ; /* [1] */
        // Comment of field: cfg_det_trigger_mask
        /*
         * D2检测触发模式屏蔽位。静态参数。
         * [0]：0：峰触发D2检测有效；1：屏蔽峰触发D2检测
         * [1]：0：谷触发D2检测有效；1：屏蔽谷触发D2检测
         */
        unsigned int    cfg_det_trigger_mask  : 2   ; /* [3..2] */
        // Comment of field: cfg_det_recovery_last_len
        /*
         * D2检测RECOVERY信号持续时间的周期数(6.144MHz时钟周期)。静态参数。
         */
        unsigned int    cfg_det_recovery_last_len : 8   ; /* [11..4] */
        // Comment of field: cfg_flat_cnt_th
        /*
         * D2检测统计时间的长度，由第一个峰谷异常超过阈值条件触发统计。单位为clk周期。
         * det_time_th=0~255,其他值时时间长度为det_time_th；静态参数。
         */
        unsigned int    cfg_flat_cnt_th       : 4   ; /* [15..12] */
        // Comment of field: cfg_flat_num_th
        /*
         * D2检测峰谷告警触发AMP_ALM信号的个数门限。峰谷个数大于此门限产生峰谷告警；静态参数。
         * 配置范围：0~15。
         */
        unsigned int    cfg_flat_num_th       : 4   ; /* [19..16] */
        // Comment of field: cfg_det_time_th
        /*
         * D2检测峰谷告警触发的门限。静态参数。
         */
        unsigned int    cfg_det_time_th       : 8   ; /* [27..20] */
        // Comment of field: cfg_det_recovery_mode
        /*
         * recovery来源选择寄存器。静态参数。
         * 0：自动模式：选择D2检测结果；
         * 1：手动模式：选择cfg_recovery；
         */
        unsigned int    cfg_det_recovery_mode : 1   ; /* [28] */
        // Comment of field: cfg_recovery_gen_manu
        /*
         * recovery寄存器，上升沿产生一次recovery
         */
        unsigned int    cfg_recovery_gen_manu : 1   ; /* [29] */
        // Comment of field: reserved_0
        /*
         * reserved
         */
        unsigned int    reserved_0            : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_classsd_dig0_cfg6;

/* Define the union u_classsd_dig0_cfg7 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: amp_alm_sync_flag_clr
        /*
         * D2检测异常指示信号clr清除寄存器
         */
        unsigned int    amp_alm_sync_flag_clr : 1   ; /* [0] */
        // Comment of field: det_recovery_cnt_ind_flag_clr
        /*
         * Recovery信号产生指示信号clr清除寄存器
         */
        unsigned int    det_recovery_cnt_ind_flag_clr : 1   ; /* [1] */
        // Comment of field: det_trough_flat_cnt_ind_flag_clr
        /*
         * 谷值异常指示信号clr清除寄存器
         */
        unsigned int    det_trough_flat_cnt_ind_flag_clr : 1   ; /* [2] */
        // Comment of field: det_peak_flat_cnt_ind_flag_clr
        /*
         * 峰值异常指示信号clr清除寄存器
         */
        unsigned int    det_peak_flat_cnt_ind_flag_clr : 1   ; /* [3] */
        // Comment of field: reserved_0
        /*
         * reserved
         */
        unsigned int    reserved_0            : 28  ; /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_classsd_dig0_cfg7;

/* Define the union u_classsd_dig0_st */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: amp_alm_sync_flag
        /*
         * D2检测异常指示信号，拉高后只可由clr清除
         */
        unsigned int    amp_alm_sync_flag     : 1   ; /* [0] */
        // Comment of field: det_recovery_cnt_ind_flag
        /*
         * Recovery信号产生指示信号，拉高后只可由clr清除
         */
        unsigned int    det_recovery_cnt_ind_flag : 1   ; /* [1] */
        // Comment of field: det_trough_flat_cnt_ind_flag
        /*
         * 谷值异常指示信号，拉高后只可由clr清除
         */
        unsigned int    det_trough_flat_cnt_ind_flag : 1   ; /* [2] */
        // Comment of field: det_peak_flat_cnt_ind_flag
        /*
         * 峰值异常指示信号，拉高后只可由clr清除
         */
        unsigned int    det_peak_flat_cnt_ind_flag : 1   ; /* [3] */
        // Comment of field: reserved_0
        /*
         * reserved
         */
        unsigned int    reserved_0            : 28  ; /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_classsd_dig0_st;

/* Define the union u_temp_code */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: temp_code
        /*
         * 输入进来的温度码字
         */
        unsigned int    temp_code             : 10  ; /* [9..0] */
        // Comment of field: reserved_0
        /*
         * reserved
         */
        unsigned int    reserved_0            : 22  ; /* [31..10] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_temp_code;

/* Define the union u_rxdre_dig0_cfg0 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: rxdre_cnt_th
        /*
         * 表示信号维持多长时间开始等待过零点，单位为0.16ms默认300为48ms
         */
        unsigned int    rxdre_cnt_th          : 16  ; /* [15..0] */
        // Comment of field: rxdre_zcd_th
        /*
         * 表示切换增益时，等待多少时间没有过零点后补充过零点，单位为0.16ms，配置为300 表示48ms
         */
        unsigned int    rxdre_zcd_th          : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_rxdre_dig0_cfg0;

/* Define the union u_rxdre_dig0_cfg1 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: rxdre_byp
        /*
         * 表示dre模块bypass掉
         * 1：byppass
         * 0：正常工作
         */
        unsigned int    rxdre_byp             : 1   ; /* [0] */
        // Comment of field: rxdre_mode
        /*
         * 表示增益调整模式
         * 0:3档增益调整
         * 1:2档增益调整
         */
        unsigned int    rxdre_mode            : 1   ; /* [1] */
        // Comment of field: rxdre_gain_sel
        /*
         * 表示增益输出使用计算结果还是寄存器手动配置
         * 1：寄存器手动配置
         * 0：计算结果
         */
        unsigned int    rxdre_gain_sel        : 1   ; /* [2] */
        // Comment of field: rxdre_gain_reg
        /*
         * 当rxdre_gain_sel为1时，输出该增益配置
         */
        unsigned int    rxdre_gain_reg        : 2   ; /* [4..3] */
        // Comment of field: rxdre_zcd_byp
        /*
         * 表示长时间无过零点补充过零点
         * 00：不补充过零点
         * 01：上切切补充过零点
         * 10：下切补充过零点
         * 11：上下切都补充过零点
         */
        unsigned int    rxdre_zcd_byp         : 2   ; /* [6..5] */
        // Comment of field: rxdre_soft_rst_n
        /*
         * 表示dre模块软复位
         * 1：解复位
         * 0：复位
         */
        unsigned int    rxdre_soft_rst_n      : 1   ; /* [7] */
        // Comment of field: reserved_0
        /*
         * reserved
         */
        unsigned int    reserved_0            : 24  ; /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_rxdre_dig0_cfg1;

/* Define the union u_reg_analog_wr00 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: d_pmu_codec_test_en
        /*
         * PMU_CODEC_SUB 测试使能信号；
         * 1：打开测试模块；
         * 0：关闭测试模块。
         */
        unsigned int    d_pmu_codec_test_en   : 1   ; /* [0] */
        // Comment of field: d_pmu_codec_ref_bg_en
        /*
         * PMU_CODEC_SUB内置 bg模块使能信号。
         * 1：开启；
         * 0：关闭。
         */
        unsigned int    d_pmu_codec_ref_bg_en : 1   ; /* [1] */
        // Comment of field: d_pmu_codec_ref_buf_en
        /*
         * PMU_CODEC_SUB内置 bg模块buffer使能信号。
         * 1：开启；
         * 0：关闭。
         */
        unsigned int    d_pmu_codec_ref_buf_en : 1   ; /* [2] */
        // Comment of field: d_pmu_codec_ref_eco_en
        /*
         * PMU_CODEC_SUB内置 bg模块低功耗模式使能信号。
         * 1：开启；
         * 0：关闭。
         */
        unsigned int    d_pmu_codec_ref_eco_en : 1   ; /* [3] */
        // Comment of field: d_pmu_codec_ibg_en
        /*
         * PMU_CODEC_SUB内置 ibg模块使能信号；
         * 1：开启；
         * 0：关闭。
         */
        unsigned int    d_pmu_codec_ibg_en    : 1   ; /* [4] */
        // Comment of field: d_pmu_codec_ipoly_en
        /*
         * PMU_CODEC_SUB内置 ipoly模块使能信号；
         * 1：开启；
         * 0：关闭。
         */
        unsigned int    d_pmu_codec_ipoly_en  : 1   ; /* [5] */
        // Comment of field: d_pmu_codec_iptat_tune_en
        /*
         * PMU_CODEC_SUB内置 iptat_tune模块使能信号。
         * 1：开启；
         * 0：关闭。
         */
        unsigned int    d_pmu_codec_iptat_tune_en : 1   ; /* [6] */
        // Comment of field: d_pmu_codec_ibias_test_en
        /*
         * PMU_CODEC_SUB内置 ibias模块电流测试控制开关信号。
         * 1：ibias测试开启；
         * 0：ibias测试关闭。
         */
        unsigned int    d_pmu_codec_ibias_test_en : 1   ; /* [7] */
        // Comment of field: d_pmu_codec_ldo1_en
        /*
         * PMU_CODEC_SUB内置 LDO1 使能信号。
         * 1：开启；
         * 0：关闭。
         */
        unsigned int    d_pmu_codec_ldo1_en   : 1   ; /* [8] */
        // Comment of field: d_pmu_codec_ldo1_en_delay
        /*
         * PMU_CODEC_SUB内置 LDO1 使能信号延迟控制；
         * 1：使能信号延时120us；
         * 0：使能信号无延时。
         */
        unsigned int    d_pmu_codec_ldo1_en_delay : 1   ; /* [9] */
        // Comment of field: d_pmu_codec_ldo1_en_eco
        /*
         * PMU_CODEC_SUB内置 LDO1  ECO模式使能信号。
         * 1：低功耗模式；
         * 0：正常模式。
         */
        unsigned int    d_pmu_codec_ldo1_en_eco : 1   ; /* [10] */
        // Comment of field: d_pmu_codec_ldo1_en_hiz
        /*
         * PMU_CODEC_SUB内置 LDO1 高阻模式配置信号。
         * 1：:输出端为高阻；
         * 0：输出正常。
         */
        unsigned int    d_pmu_codec_ldo1_en_hiz : 1   ; /* [11] */
        // Comment of field: d_pmu_codec_ldo1_en_ocp
        /*
         * PMU_CODEC_SUB内置 LDO1  OCP检测功能使能信号；
         * 1：开启；
         * 0：关闭。
         */
        unsigned int    d_pmu_codec_ldo1_en_ocp : 1   ; /* [12] */
        // Comment of field: d_pmu_codec_ldo1_en_sw
        /*
         * PMU_CODEC_SUB内置 LDO1  直通模式配置信号。
         * 1：开启；
         * 0：关闭。
         */
        unsigned int    d_pmu_codec_ldo1_en_sw : 1   ; /* [13] */
        // Comment of field: d_pmu_codec_ldo2_en
        /*
         * PMU_CODEC_SUB内置 LDO2 使能信号。
         * 1：开启；
         * 0：关闭。
         */
        unsigned int    d_pmu_codec_ldo2_en   : 1   ; /* [14] */
        // Comment of field: d_pmu_codec_ldo2_en_delay
        /*
         * PMU_CODEC_SUB内置 LDO2 使能信号延迟控制；
         * 1：使能信号延时120us；
         * 0：使能信号无延时。
         */
        unsigned int    d_pmu_codec_ldo2_en_delay : 1   ; /* [15] */
        // Comment of field: d_pmu_codec_ldo2_en_eco
        /*
         * PMU_CODEC_SUB内置 LDO2  ECO模式使能信号。
         * 1：低功耗模式；
         * 0：正常模式。
         */
        unsigned int    d_pmu_codec_ldo2_en_eco : 1   ; /* [16] */
        // Comment of field: d_pmu_codec_ldo2_en_hiz
        /*
         * PMU_CODEC_SUB内置 LDO2 高阻模式配置信号。
         * 1：:输出端为高阻；
         * 0：输出正常。
         */
        unsigned int    d_pmu_codec_ldo2_en_hiz : 1   ; /* [17] */
        // Comment of field: d_pmu_codec_ldo2_en_ocp
        /*
         * PMU_CODEC_SUB内置 LDO2  OCP检测功能使能信号；
         * 1：开启；
         * 0：关闭。
         */
        unsigned int    d_pmu_codec_ldo2_en_ocp : 1   ; /* [18] */
        // Comment of field: d_pmu_codec_ldo2_en_sw
        /*
         * PMU_CODEC_SUB内置 LDO2  直通模式配置信号。
         * 1：开启；
         * 0：关闭。
         */
        unsigned int    d_pmu_codec_ldo2_en_sw : 1   ; /* [19] */
        // Comment of field: d_pmu_codec_ldo3_en
        /*
         * PMU_CODEC_SUB内置 LDO3 使能信号。
         * 1：开启；
         * 0：关闭。
         */
        unsigned int    d_pmu_codec_ldo3_en   : 1   ; /* [20] */
        // Comment of field: d_pmu_codec_ldo3_en_delay
        /*
         * PMU_CODEC_SUB内置 LDO3 使能信号延迟控制；
         * 1：使能信号延时120us；
         * 0：使能信号无延时。
         */
        unsigned int    d_pmu_codec_ldo3_en_delay : 1   ; /* [21] */
        // Comment of field: d_pmu_codec_ldo3_en_eco
        /*
         * PMU_CODEC_SUB内置 LDO3  ECO模式使能信号。
         * 1：低功耗模式；
         * 0：正常模式。
         */
        unsigned int    d_pmu_codec_ldo3_en_eco : 1   ; /* [22] */
        // Comment of field: d_pmu_codec_ldo3_en_hiz
        /*
         * PMU_CODEC_SUB内置 LDO3 高阻模式配置信号。
         * 1：:输出端为高阻；
         * 0：输出正常。
         */
        unsigned int    d_pmu_codec_ldo3_en_hiz : 1   ; /* [23] */
        // Comment of field: d_pmu_codec_ldo3_en_ocp
        /*
         * PMU_CODEC_SUB内置 LDO3  OCP检测功能使能信号；
         * 1：开启；
         * 0：关闭。
         */
        unsigned int    d_pmu_codec_ldo3_en_ocp : 1   ; /* [24] */
        // Comment of field: d_pmu_codec_ldo3_en_sw
        /*
         * PMU_CODEC_SUB内置 LDO3  直通模式配置信号。
         * 1：开启；
         * 0：关闭。
         */
        unsigned int    d_pmu_codec_ldo3_en_sw : 1   ; /* [25] */
        // Comment of field: d_pmu_codec_ldo4_en
        /*
         * PMU_CODEC_SUB内置 LDO4 使能信号。
         * 1：开启；
         * 0：关闭。
         */
        unsigned int    d_pmu_codec_ldo4_en   : 1   ; /* [26] */
        // Comment of field: d_pmu_codec_ldo4_en_delay
        /*
         * PMU_CODEC_SUB内置 LDO4 使能信号延迟控制；
         * 1：使能信号延时120us；
         * 0：使能信号无延时。
         */
        unsigned int    d_pmu_codec_ldo4_en_delay : 1   ; /* [27] */
        // Comment of field: d_pmu_codec_ldo4_en_eco
        /*
         * PMU_CODEC_SUB内置 LDO4  ECO模式使能信号。
         * 1：低功耗模式；
         * 0：正常模式。
         */
        unsigned int    d_pmu_codec_ldo4_en_eco : 1   ; /* [28] */
        // Comment of field: d_pmu_codec_ldo4_en_hiz
        /*
         * PMU_CODEC_SUB内置 LDO4 高阻模式配置信号。
         * 1：:输出端为高阻；
         * 0：输出正常。
         */
        unsigned int    d_pmu_codec_ldo4_en_hiz : 1   ; /* [29] */
        // Comment of field: d_pmu_codec_ldo4_en_ocp
        /*
         * PMU_CODEC_SUB内置 LDO4  OCP检测功能使能信号；
         * 1：开启；
         * 0：关闭。
         */
        unsigned int    d_pmu_codec_ldo4_en_ocp : 1   ; /* [30] */
        // Comment of field: d_pmu_codec_ldo4_en_sw
        /*
         * PMU_CODEC_SUB内置 LDO4  直通模式配置信号。
         * 1：开启；
         * 0：关闭。
         */
        unsigned int    d_pmu_codec_ldo4_en_sw : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr00;

/* Define the union u_reg_analog_wr01 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: tx_idac_en_icell_d06_ch1
        /*
         * TX通道1 IDAC icell使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_idac_en_icell_d06_ch1 : 1   ; /* [0] */
        // Comment of field: tx_idac_en_vcm_d06_ch1
        /*
         * TX通道1 IDAC vcmbuf使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_idac_en_vcm_d06_ch1 : 1   ; /* [1] */
        // Comment of field: tx_cld_d2_func_en_d06_ch1
        /*
         * TX通道1 LPFT复位选择
         * 0：由TOP复位信号控制
         * 1：由小数字D2检测控制复位
         */
        unsigned int    tx_cld_d2_func_en_d06_ch1 : 1   ; /* [2] */
        // Comment of field: tx_cmfb_en_d06_ch1
        /*
         * TX通道1 CMFB使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_cmfb_en_d06_ch1    : 1   ; /* [3] */
        // Comment of field: tx_dre_en_d06_ch1
        /*
         * TX通道1DRE功能模拟使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_dre_en_d06_ch1     : 1   ; /* [4] */
        // Comment of field: tx_en_comp_d06_ch1
        /*
         * TX通道1 比较器使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_en_comp_d06_ch1    : 1   ; /* [5] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 1   ; /* [6] */
        // Comment of field: tx_en_op1_d06_ch1
        /*
         * TX通道1 LPFT OP1使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_en_op1_d06_ch1     : 1   ; /* [7] */
        // Comment of field: tx_en_op2_d06_ch1
        /*
         * TX通道1 LPFT OP2使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_en_op2_d06_ch1     : 1   ; /* [8] */
        // Comment of field: tx_en_op3_d06_ch1
        /*
         * TX通道1 LPFT OP3使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_en_op3_d06_ch1     : 1   ; /* [9] */
        // Comment of field: tx_en_op4_d06_ch1
        /*
         * TX通道1 LPFT OP4使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_en_op4_d06_ch1     : 1   ; /* [10] */
        // Comment of field: tx_en_subdac_d06_ch1
        /*
         * TX通道1 CLD subdac使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_en_subdac_d06_ch1  : 1   ; /* [11] */
        // Comment of field: tx_rstn_comp_d06_ch1
        /*
         * TX通道1CLD比较器输出复位信号
         * 0：复位
         * 1：不复位
         */
        unsigned int    tx_rstn_comp_d06_ch1  : 1   ; /* [12] */
        // Comment of field: tx_rstn_lpf_d06_ch1
        /*
         * TX通道1CLD积分器复位信号
         * 0：复位
         * 1：不复位
         */
        unsigned int    tx_rstn_lpf_d06_ch1   : 1   ; /* [13] */
        // Comment of field: tx_en_depop_d06_ch1
        /*
         * TX通道1下电depop功能使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_en_depop_d06_ch1   : 1   ; /* [14] */
        // Comment of field: tx_hsn_depop_en_d06_ch1
        /*
         * TX通道1depop 功率管OUTP使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_hsn_depop_en_d06_ch1 : 1   ; /* [15] */
        // Comment of field: tx_hsn_en_d06_ch1
        /*
         * TX通道1功率管OUTP使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_hsn_en_d06_ch1     : 1   ; /* [16] */
        // Comment of field: tx_hsp_depop_en_d06_ch1
        /*
         * TX通道1depop 功率管OUTP使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_hsp_depop_en_d06_ch1 : 1   ; /* [17] */
        // Comment of field: tx_hsp_en_d06_ch1
        /*
         * 功率管OUTP使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_hsp_en_d06_ch1     : 1   ; /* [18] */
        // Comment of field: tx_lsn_depop_en_d06_ch1
        /*
         * TX通道1depop 功率管OUTP使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_lsn_depop_en_d06_ch1 : 1   ; /* [19] */
        // Comment of field: tx_lsn_en_d06_ch1
        /*
         * 功率管OUTP使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_lsn_en_d06_ch1     : 1   ; /* [20] */
        // Comment of field: tx_lsp_depop_en_d06_ch1
        /*
         * TX通道1depop 功率管OUTP使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_lsp_depop_en_d06_ch1 : 1   ; /* [21] */
        // Comment of field: tx_lsp_en_d06_ch1
        /*
         * TX通道1功率管OUTP使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_lsp_en_d06_ch1     : 1   ; /* [22] */
        // Comment of field: tx_rstn_pop_d06_ch1
        /*
         * TX通道1depop下电过程1复位
         */
        unsigned int    tx_rstn_pop_d06_ch1   : 1   ; /* [23] */
        // Comment of field: tx_rstn_pop2_d06_ch1
        /*
         * TX通道1depop下电过程2复位
         */
        unsigned int    tx_rstn_pop2_d06_ch1  : 1   ; /* [24] */
        // Comment of field: tx_vcm_ctrl_en_d06_ch1
        /*
         * TX通道1共模时序开关
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_vcm_ctrl_en_d06_ch1 : 1   ; /* [25] */
        // Comment of field: reserved_1
        /*
         * Reserved
         */
        unsigned int    reserved_1            : 6   ; /* [31..26] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr01;

/* Define the union u_reg_analog_wr02 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: tx_idac_en_icell_d06_ch2
        /*
         * TX通道2 IDAC icell使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_idac_en_icell_d06_ch2 : 1   ; /* [0] */
        // Comment of field: tx_idac_en_vcm_d06_ch2
        /*
         * TX通道2 IDAC vcmbuf使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_idac_en_vcm_d06_ch2 : 1   ; /* [1] */
        // Comment of field: tx_cld_d2_func_en_d06_ch2
        /*
         * TX通道2 LPFT复位选择
         * 0：由TOP复位信号控制
         * 1：由小数字D2检测控制复位
         */
        unsigned int    tx_cld_d2_func_en_d06_ch2 : 1   ; /* [2] */
        // Comment of field: tx_cmfb_en_d06_ch2
        /*
         * TX通道2 CMFB使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_cmfb_en_d06_ch2    : 1   ; /* [3] */
        // Comment of field: tx_dre_en_d06_ch2
        /*
         * TX通道2DRE功能模拟使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_dre_en_d06_ch2     : 1   ; /* [4] */
        // Comment of field: tx_en_comp_d06_ch2
        /*
         * TX通道2 比较器使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_en_comp_d06_ch2    : 1   ; /* [5] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 1   ; /* [6] */
        // Comment of field: tx_en_op1_d06_ch2
        /*
         * TX通道2 LPFT OP1使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_en_op1_d06_ch2     : 1   ; /* [7] */
        // Comment of field: tx_en_op2_d06_ch2
        /*
         * TX通道2 LPFT OP2使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_en_op2_d06_ch2     : 1   ; /* [8] */
        // Comment of field: tx_en_op3_d06_ch2
        /*
         * TX通道2 LPFT OP3使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_en_op3_d06_ch2     : 1   ; /* [9] */
        // Comment of field: tx_en_op4_d06_ch2
        /*
         * TX通道2 LPFT OP4使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_en_op4_d06_ch2     : 1   ; /* [10] */
        // Comment of field: tx_en_subdac_d06_ch2
        /*
         * TX通道2 CLD subdac使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_en_subdac_d06_ch2  : 1   ; /* [11] */
        // Comment of field: tx_rstn_comp_d06_ch2
        /*
         * TX通道2CLD比较器输出复位信号
         * 0：复位
         * 1：不复位
         */
        unsigned int    tx_rstn_comp_d06_ch2  : 1   ; /* [12] */
        // Comment of field: tx_rstn_lpf_d06_ch2
        /*
         * TX通道2CLD积分器复位信号
         * 0：复位
         * 1：不复位
         */
        unsigned int    tx_rstn_lpf_d06_ch2   : 1   ; /* [13] */
        // Comment of field: tx_en_depop_d06_ch2
        /*
         * TX通道2下电depop功能使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_en_depop_d06_ch2   : 1   ; /* [14] */
        // Comment of field: tx_hsn_depop_en_d06_ch2
        /*
         * TX通道2depop 功率管OUTP使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_hsn_depop_en_d06_ch2 : 1   ; /* [15] */
        // Comment of field: tx_hsn_en_d06_ch2
        /*
         * TX通道2功率管OUTP使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_hsn_en_d06_ch2     : 1   ; /* [16] */
        // Comment of field: tx_hsp_depop_en_d06_ch2
        /*
         * TX通道2depop 功率管OUTP使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_hsp_depop_en_d06_ch2 : 1   ; /* [17] */
        // Comment of field: tx_hsp_en_d06_ch2
        /*
         * 功率管OUTP使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_hsp_en_d06_ch2     : 1   ; /* [18] */
        // Comment of field: tx_lsn_depop_en_d06_ch2
        /*
         * TX通道2depop 功率管OUTP使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_lsn_depop_en_d06_ch2 : 1   ; /* [19] */
        // Comment of field: tx_lsn_en_d06_ch2
        /*
         * 功率管OUTP使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_lsn_en_d06_ch2     : 1   ; /* [20] */
        // Comment of field: tx_lsp_depop_en_d06_ch2
        /*
         * TX通道2depop 功率管OUTP使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_lsp_depop_en_d06_ch2 : 1   ; /* [21] */
        // Comment of field: tx_lsp_en_d06_ch2
        /*
         * TX通道2功率管OUTP使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_lsp_en_d06_ch2     : 1   ; /* [22] */
        // Comment of field: tx_rstn_pop_d06_ch2
        /*
         * TX通道2depop下电过程1复位
         */
        unsigned int    tx_rstn_pop_d06_ch2   : 1   ; /* [23] */
        // Comment of field: tx_rstn_pop2_d06_ch2
        /*
         * TX通道2depop下电过程2复位
         */
        unsigned int    tx_rstn_pop2_d06_ch2  : 1   ; /* [24] */
        // Comment of field: tx_vcm_ctrl_en_d06_ch2
        /*
         * TX通道2共模时序开关
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_vcm_ctrl_en_d06_ch2 : 1   ; /* [25] */
        // Comment of field: reserved_1
        /*
         * Reserved
         */
        unsigned int    reserved_1            : 3   ; /* [28..26] */
        // Comment of field: tx_idac_en_vbgen_d06
        /*
         * Tx Vbgen 使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_idac_en_vbgen_d06  : 1   ; /* [29] */
        // Comment of field: tx_idac_en_fstup_ctrl_d06
        /*
         * Tx Vbgen 快启电流选择
         * 1：20uA
         * 0：10uA
         */
        unsigned int    tx_idac_en_fstup_ctrl_d06 : 1   ; /* [30] */
        // Comment of field: tx_idac_en_fstup_d06
        /*
         * Tx Vbgen 快启使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_idac_en_fstup_d06  : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr02;

/* Define the union u_reg_analog_wr03 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: rx_en_sdm_int1_d06_ch1
        /*
         * normal_adc通道1 RX 积分器op1使能信号：
         * 0：op1关闭
         * 1：op1使能
         */
        unsigned int    rx_en_sdm_int1_d06_ch1 : 1   ; /* [0] */
        // Comment of field: rx_en_sdm_int2_d06_ch1
        /*
         * normal_adc通道1 RX 积分器op2使能信号：
         * 0：op2关闭
         * 1：op2使能
         */
        unsigned int    rx_en_sdm_int2_d06_ch1 : 1   ; /* [1] */
        // Comment of field: rx_en_sdm_fls_d06_ch1
        /*
         * normal_adc通道1 Rxflash模块使能信号：
         * 0：flash关闭
         * 1：flahs使能
         */
        unsigned int    rx_en_sdm_fls_d06_ch1 : 1   ; /* [2] */
        // Comment of field: rx_en_clk_gen_d06_ch1
        /*
         * normal_adc通道1 RX CLK_GEN 模块使能信号：
         * 0：CLK_GEN 关闭
         * 1：CLK_GEN使能
         */
        unsigned int    rx_en_clk_gen_d06_ch1 : 1   ; /* [3] */
        // Comment of field: rx_en_dre_d06_ch1
        /*
         * normal_adc通道1 DRE使能信号
         * 0：DRE关闭
         * 1：DRE开启
         */
        unsigned int    rx_en_dre_d06_ch1     : 1   ; /* [4] */
        // Comment of field: rx_dwa_rst_d06_ch1
        /*
         * normal_adc通道1 DWA模块复位信号：
         * 0：正常工作
         * 1：复位
         */
        unsigned int    rx_dwa_rst_d06_ch1    : 1   ; /* [5] */
        // Comment of field: rx_rstn_d06_ch1
        /*
         * normal_adc通道1 RX复位信号：
         * 0：复位
         * 1：正常工作
         */
        unsigned int    rx_rstn_d06_ch1       : 1   ; /* [6] */
        // Comment of field: rx_rstn_dre_d06_ch1
        /*
         * normal_adc通道1 DRE复位信号
         * 0：复位
         * 1：正常工作
         */
        unsigned int    rx_rstn_dre_d06_ch1   : 1   ; /* [7] */
        // Comment of field: rx_en_clk_int1_d06_ch1
        /*
         * normal_adc通道1积分器op1 chopper使能信号：
         * 0：chopper关闭
         * 1：chopper开启
         */
        unsigned int    rx_en_clk_int1_d06_ch1 : 1   ; /* [8] */
        // Comment of field: rx_en_sdm_dac_a18_ch1
        /*
         * normal_adc通道1中ICELL模块使能
         * 0:使能关闭
         * 1：使能开启（默认）
         */
        unsigned int    rx_en_sdm_dac_a18_ch1 : 1   ; /* [9] */
        // Comment of field: rx_en_sdm_vbn_nf_a18_ch1
        /*
         * normal_adc通道1中VBN的Noisefilter结构的伪电阻使能，与reg_analog_wr11<10>一起设置
         * 0:关闭noisefilter伪电阻（默认）
         * 1：开启noisefilter伪电阻
         */
        unsigned int    rx_en_sdm_vbn_nf_a18_ch1 : 1   ; /* [10] */
        // Comment of field: rx_en_sdm_vbp_nf_a18_ch1
        /*
         * normal_adc通道1中VBP的Noisefilter结构的伪电阻使能，与reg_analog_wr11<10>一起设置
         * 0:关闭noisefilter伪电阻（默认）
         * 1：开启noisefilter伪电阻
         */
        unsigned int    rx_en_sdm_vbp_nf_a18_ch1 : 1   ; /* [11] */
        // Comment of field: rx_en_pga_d06_ch1
        /*
         * normal_adc通道1PGA使能信号：
         * 0:PGA关闭
         * 1：PGA开启
         */
        unsigned int    rx_en_pga_d06_ch1     : 1   ; /* [12] */
        // Comment of field: rx_en_dre_zcd_d06_ch1
        /*
         * normal_adc通道1 DRE过零检测比较器使能信号
         * 0：ZC比较器关闭
         * 1：ZC比较器开启
         */
        unsigned int    rx_en_dre_zcd_d06_ch1 : 1   ; /* [13] */
        // Comment of field: rx_en_zcd_a09_ch1
        /*
         * normal_adc通道1的过零检检模块的EN信号：0，不使能，1：使能
         */
        unsigned int    rx_en_zcd_a09_ch1     : 1   ; /* [14] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 1   ; /* [15] */
        // Comment of field: rx_en_sdm_int1_d06_ch2
        /*
         * normal_adc通道2 RX 积分器op1使能信号：
         * 0：op1关闭
         * 1：op1使能
         */
        unsigned int    rx_en_sdm_int1_d06_ch2 : 1   ; /* [16] */
        // Comment of field: rx_en_sdm_int2_d06_ch2
        /*
         * normal_adc通道2 RX 积分器op2使能信号：
         * 0：op2关闭
         * 1：op2使能
         */
        unsigned int    rx_en_sdm_int2_d06_ch2 : 1   ; /* [17] */
        // Comment of field: rx_en_sdm_fls_d06_ch2
        /*
         * normal_adc通道2 Rxflash模块使能信号：
         * 0：flash关闭
         * 1：flahs使能
         */
        unsigned int    rx_en_sdm_fls_d06_ch2 : 1   ; /* [18] */
        // Comment of field: rx_en_clk_gen_d06_ch2
        /*
         * normal_adc通道2RX CLK_GEN 模块使能信号：
         * 0：CLK_GEN 关闭
         * 1：CLK_GEN使能
         */
        unsigned int    rx_en_clk_gen_d06_ch2 : 1   ; /* [19] */
        // Comment of field: rx_en_dre_d06_ch2
        /*
         * normal_adc通道2 DRE使能信号
         * 0：DRE关闭
         * 1：DRE开启
         */
        unsigned int    rx_en_dre_d06_ch2     : 1   ; /* [20] */
        // Comment of field: rx_dwa_rst_d06_ch2
        /*
         * normal_adc通道2 DWA模块复位信号：
         * 0：正常工作
         * 1：复位
         */
        unsigned int    rx_dwa_rst_d06_ch2    : 1   ; /* [21] */
        // Comment of field: rx_rstn_d06_ch2
        /*
         * normal_adc通道2RX复位信号：
         * 0：复位
         * 1：正常工作
         */
        unsigned int    rx_rstn_d06_ch2       : 1   ; /* [22] */
        // Comment of field: rx_rstn_dre_d06_ch2
        /*
         * normal_adc通道2 DRE复位信号
         * 0：复位
         * 1：正常工作
         */
        unsigned int    rx_rstn_dre_d06_ch2   : 1   ; /* [23] */
        // Comment of field: rx_en_clk_int1_d06_ch2
        /*
         * normal_adc通道2积分器op1 chopper使能信号：
         * 0：chopper关闭
         * 1：chopper开启
         */
        unsigned int    rx_en_clk_int1_d06_ch2 : 1   ; /* [24] */
        // Comment of field: rx_en_sdm_dac_a18_ch2
        /*
         * normal_adc通道2中ICELL模块使能
         * 0:使能关闭
         * 1：使能开启（默认）
         */
        unsigned int    rx_en_sdm_dac_a18_ch2 : 1   ; /* [25] */
        // Comment of field: rx_en_sdm_vbn_nf_a18_ch2
        /*
         * normal_adc通道2中VBN的Noisefilter结构的伪电阻使能，与reg_analog_wr11<10>一起设置
         * 0:关闭noisefilter伪电阻（默认）
         * 1：开启noisefilter伪电阻
         */
        unsigned int    rx_en_sdm_vbn_nf_a18_ch2 : 1   ; /* [26] */
        // Comment of field: rx_en_sdm_vbp_nf_a18_ch2
        /*
         * normal_adc通道2中VBP的Noisefilter结构的伪电阻使能，与reg_analog_wr11<10>一起设置
         * 0:关闭noisefilter伪电阻（默认）
         * 1：开启noisefilter伪电阻
         */
        unsigned int    rx_en_sdm_vbp_nf_a18_ch2 : 1   ; /* [27] */
        // Comment of field: rx_en_pga_d06_ch2
        /*
         * normal_adc通道2 PGA使能信号：
         * 0:PGA关闭
         * 1：PGA开启
         */
        unsigned int    rx_en_pga_d06_ch2     : 1   ; /* [28] */
        // Comment of field: rx_en_dre_zcd_d06_ch2
        /*
         * normal_adc通道2 DRE过零检测比较器使能信号
         * 0：ZC比较器关闭
         * 1：ZC比较器开启
         */
        unsigned int    rx_en_dre_zcd_d06_ch2 : 1   ; /* [29] */
        // Comment of field: rx_en_zcd_a09_ch2
        /*
         * normal_adc通道2的过零检检模块的EN信号;
         * 0：不使能；
         * 1：使能。
         */
        unsigned int    rx_en_zcd_a09_ch2     : 1   ; /* [30] */
        // Comment of field: reserved_1
        /*
         * Reserved
         */
        unsigned int    reserved_1            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr03;

/* Define the union u_reg_analog_wr04 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: mic0_sel_d06
        /*
         * lpadc通路pga输入MUX选择开关。
         * 0：VPU_N/P通路不连通；
         * 1：VPU_N/P通路信号连通至pga。
         */
        unsigned int    mic0_sel_d06          : 1   ; /* [0] */
        // Comment of field: mic1_sel_d06
        /*
         * lpadc通路pga输入MUX选择开关。
         * 0：MIC0_N/P通路不连通；
         * 1：MIC0_N/P通路信号连通至pga。
         */
        unsigned int    mic1_sel_d06          : 1   ; /* [1] */
        // Comment of field: mic2_sel_d06
        /*
         * lpadc通路pga输入MUX选择开关。
         * 0：MIC1_N/P通路不连通；
         * 1：MIC1_N/P通路信号连通至pga。
         */
        unsigned int    mic2_sel_d06          : 1   ; /* [2] */
        // Comment of field: mic3_sel_d06
        /*
         * Reserved
         */
        unsigned int    mic3_sel_d06          : 1   ; /* [3] */
        // Comment of field: mad_en_pga_d06
        /*
         * lpadc通路pga使能开关。
         * 0：pga关闭；
         * 1：pga使能。
         */
        unsigned int    mad_en_pga_d06        : 1   ; /* [4] */
        // Comment of field: mad_en_zcd_d06
        /*
         * lpadc通路zcd模块使能开关。
         * 0：zcd关闭；
         * 1：zcd使能。
         */
        unsigned int    mad_en_zcd_d06        : 1   ; /* [5] */
        // Comment of field: mad_rstn_zc_d06
        /*
         * lpadc通路zcd模块复位开关。
         * 0：zcd复位；
         * 1：zcd解复位。
         */
        unsigned int    mad_rstn_zc_d06       : 1   ; /* [6] */
        // Comment of field: mad_en_sdm_d06
        /*
         * lpadc通路sdm通路使能开关。
         * 0：sdm关闭；
         * 1：sdm使能。
         */
        unsigned int    mad_en_sdm_d06        : 1   ; /* [7] */
        // Comment of field: mad_en_clk_gen_d06
        /*
         * lpadc通路clk_gen模块使能开关。
         * 0：clk_gen关闭；
         * 1：clk_gen使能。
         */
        unsigned int    mad_en_clk_gen_d06    : 1   ; /* [8] */
        // Comment of field: mad_endout_d06
        /*
         * lpadc通路数字码字输出控制开关。
         * 0：data固定输出0100；
         * 1：data正常输出。
         */
        unsigned int    mad_endout_d06        : 1   ; /* [9] */
        // Comment of field: mad_rstn_sdm_d06
        /*
         * lpadc通路clk_gen模块DFF复位开关。
         * 0：clk_gen关闭；
         * 1：clk_gen使能。
         */
        unsigned int    mad_rstn_sdm_d06      : 1   ; /* [10] */
        // Comment of field: ibias_rctune_en_d06
        /*
         * rctune模块ibias使能开关。
         * 0：rctune模块ibias关闭；
         * 1：rctune模块ibias使能。
         */
        unsigned int    ibias_rctune_en_d06   : 1   ; /* [11] */
        // Comment of field: ibias_rx_idac_en_d06
        /*
         * rx/lpadc通路ibias使能开关。
         * 0：rx/lpadc通路ibias关闭；
         * 1：rx/lpadc通路ibias使能。
         */
        unsigned int    ibias_rx_idac_en_d06  : 1   ; /* [12] */
        // Comment of field: ibias_tx_en_d06
        /*
         * tx通路ibias使能开关。
         * 0：tx通路ibias关闭；
         * 1：tx通路ibias使能。
         */
        unsigned int    ibias_tx_en_d06       : 1   ; /* [13] */
        // Comment of field: ibias_vcm09_en_d06
        /*
         * vcm_09模块ibias使能开关。
         * 0：vcm_09模块ibias关闭；
         * 1：vcm_09模块ibias使能。
         */
        unsigned int    ibias_vcm09_en_d06    : 1   ; /* [14] */
        // Comment of field: ibias_vcm045_en_d06
        /*
         * vcm_0p45模块ibias使能开关。
         * 0：vcm_0p45模块ibias关闭；
         * 1：vcm_0p45模块ibias使能。
         */
        unsigned int    ibias_vcm045_en_d06   : 1   ; /* [15] */
        // Comment of field: ibias_top_en_d06
        /*
         * ibias_top使能开关。
         * 0：ibias_top关闭；
         * 1：ibias_top使能。
         */
        unsigned int    ibias_top_en_d06      : 1   ; /* [16] */
        // Comment of field: rctune_add_en_d06
        /*
         * rctune模块输出所有码字add code使能开关：
         * 0：输出码字不加code；
         * 1：输出码字加code，具体数值通过寄存器配置。
         */
        unsigned int    rctune_add_en_d06     : 1   ; /* [17] */
        // Comment of field: rctune_en_d06
        /*
         * rctune模块使能开关：
         * 0：rctune模块关闭；
         * 1：rctune模块使能。
         */
        unsigned int    rctune_en_d06         : 1   ; /* [18] */
        // Comment of field: rctune_minus_en_d06
        /*
         * rctune模块输出所有码字minus code使能开关：
         * 0：输出码字不减code；
         * 1：输出码字减code，具体数值通过寄存器配置。
         */
        unsigned int    rctune_minus_en_d06   : 1   ; /* [19] */
        // Comment of field: rctune_rstn
        /*
         * rctune模块码字输出DFF复位开关。
         * 0：rctune送至rx/lpadc/tx码字复位至0；
         * 1：rctune送至rx/lpadc/tx码字正常输出。
         */
        unsigned int    rctune_rstn           : 1   ; /* [20] */
        // Comment of field: vcm0p9_en_d06
        /*
         * vcm0p9模块使能开关：
         * 0：vcm0p9模块关闭；
         * 1：vcm0p9模块使能。
         */
        unsigned int    vcm0p9_en_d06         : 1   ; /* [21] */
        // Comment of field: vcm0p45_en_d06
        /*
         * vcm0p45模块使能开关：
         * 0：vcm0p45模块关闭；
         * 1：vcm0p45模块使能。
         */
        unsigned int    vcm0p45_en_d06        : 1   ; /* [22] */
        // Comment of field: clkgen_en_d06
        /*
         * clkgen模块使能开关：
         * 0：clkgen模块关闭；
         * 1：clkgen模块使能。
         */
        unsigned int    clkgen_en_d06         : 1   ; /* [23] */
        // Comment of field: clkgen_mad_en_d06
        /*
         * clkgen模块lpadc时钟使能开关：
         * 0：lpadc相关时钟关闭；
         * 1：lpadc相关时钟使能。
         */
        unsigned int    clkgen_mad_en_d06     : 1   ; /* [24] */
        // Comment of field: clkgen_rx0_en_d06
        /*
         * clkgen模块rx0时钟使能开关：
         * 0：rx0相关时钟关闭；
         * 1：rx0相关时钟使能。
         */
        unsigned int    clkgen_rx0_en_d06     : 1   ; /* [25] */
        // Comment of field: clkgen_rx1_en_d06
        /*
         * clkgen模块rx1时钟使能开关：
         * 0：rx1相关时钟关闭；
         * 1：rx1相关时钟使能。
         */
        unsigned int    clkgen_rx1_en_d06     : 1   ; /* [26] */
        // Comment of field: clkgen_rctune_en_d06
        /*
         * clkgen模块rctune时钟使能开关：
         * 0：rctune相关时钟关闭；
         * 1：rctune相关时钟使能。
         */
        unsigned int    clkgen_rctune_en_d06  : 1   ; /* [27] */
        // Comment of field: clkgen_tx0_en_d06
        /*
         * clkgen模块tx0时钟使能开关：
         * 0：tx0相关时钟关闭；
         * 1：tx0相关时钟使能。
         */
        unsigned int    clkgen_tx0_en_d06     : 1   ; /* [28] */
        // Comment of field: clkgen_tx1_en_d06
        /*
         * clkgen模块tx1时钟使能开关：
         * 0：tx1相关时钟关闭；
         * 1：tx1相关时钟使能。
         */
        unsigned int    clkgen_tx1_en_d06     : 1   ; /* [29] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr04;

/* Define the union u_reg_analog_wr05 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: tx_dacvcm_iqenh_d06_ch1
        /*
         * TX通道1 IDAC vcmbuf功耗选择
         * 0000:7.5uA
         * 0001:15uA
         * 0010:22.5uA
         * 0011:30uA
         * 0100:37.5uA
         * 0101:45uA
         * 0110:52.5uA
         * 0111:60uA
         * 1000:67.5uA
         * 1001:75uA
         * 1010:82.5uA
         * 1011:90uA
         * 1100:97.5uA
         * 1101:105uA
         * 1110:112.5uA
         * 1111:120uA
         */
        unsigned int    tx_dacvcm_iqenh_d06_ch1 : 4   ; /* [3..0] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 5   ; /* [8..4] */
        // Comment of field: tx_cmfb_bias_ctrl_d06_ch1
        /*
         * TX通道1 CMFB op功耗选取
         * 0：输出级功耗15u
         * 1：输出级功耗30u
         */
        unsigned int    tx_cmfb_bias_ctrl_d06_ch1 : 1   ; /* [9] */
        // Comment of field: tx_deglitch_sel_d06_ch1
        /*
         * TX通道1 数据符号位相对调节延时，默认0
         */
        unsigned int    tx_deglitch_sel_d06_ch1 : 3   ; /* [12..10] */
        // Comment of field: reserved_1
        /*
         * Reserved
         */
        unsigned int    reserved_1            : 1   ; /* [13] */
        // Comment of field: tx_pulse_adj_byp_d06_ch1
        /*
         * Reserved
         */
        unsigned int    tx_pulse_adj_byp_d06_ch1 : 1   ; /* [14] */
        // Comment of field: tx_en_op1_chop_d06_ch1
        /*
         * TX通道1 LPFT OP1 CHOP使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_en_op1_chop_d06_ch1 : 1   ; /* [15] */
        // Comment of field: tx_sign_inv_sel_d06_ch1
        /*
         * TX通道1 CLD里VREF_PP_SIGN标志位方向
         * 0：不反向
         * 1：反向
         */
        unsigned int    tx_sign_inv_sel_d06_ch1 : 1   ; /* [16] */
        // Comment of field: tx_tdead_hsn_sel_d06_ch1
        /*
         * TX通道1 与cld_tdead_lsp_sel共同控制电流VOUTN->VOUTP时死区时间
         * 0000:不延时
         * 0001:delaycell*1
         * ...
         * 1111:delaycell*15
         */
        unsigned int    tx_tdead_hsn_sel_d06_ch1 : 4   ; /* [20..17] */
        // Comment of field: tx_tdead_hsp_sel_d06_ch1
        /*
         * TX通道1 与cld_tdead_lsn_sel共同控制电流VOUTP->VOUTN时死区时间
         * 0000:不延时
         * 0001:delaycell*1
         * ...
         * 1111:delaycell*15
         */
        unsigned int    tx_tdead_hsp_sel_d06_ch1 : 4   ; /* [24..21] */
        // Comment of field: tx_tdead_lsn_sel_d06_ch1
        /*
         * TX通道1 与cld_tdead_hsp_sel共同控制电流VOUTP->VOUTN时死区时间
         * 0000:不延时
         * 0001:delaycell*1
         * ...
         * 1111:delaycell*15
         */
        unsigned int    tx_tdead_lsn_sel_d06_ch1 : 4   ; /* [28..25] */
        // Comment of field: tx_dre_delay_sel_ch1
        /*
         * TX通道1 DRE电阻电容切换与接入vcm的延时
         * 000:0T
         * …
         * 111:7T
         */
        unsigned int    tx_dre_delay_sel_ch1  : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr05;

/* Define the union u_reg_analog_wr06 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: tx_tdead_lsp_sel_d06_ch1
        /*
         * TX通道1 与cld_tdead_hsn_sel共同控制电流VOUTN->VOUTP时死区时间
         * 0000:不延时
         * 0001:delaycell*1
         * ...
         * 1111:delaycell*15
         */
        unsigned int    tx_tdead_lsp_sel_d06_ch1 : 4   ; /* [3..0] */
        // Comment of field: tx_vcmctrl_dly_sel_d06_ch1
        /*
         * TX通道1 选择拉共模的小电阻传输门延时
         * 0000:不延时
         * ...
         * 1111:delaycell*15
         */
        unsigned int    tx_vcmctrl_dly_sel_d06_ch1 : 4   ; /* [7..4] */
        // Comment of field: tx_vcmctrl_sw_dly_sel_d06_ch1
        /*
         * TX通道1 选择拉共模的大电阻传输门延时
         * 0000:不延时
         * ...
         * 1111:delaycell*15
         */
        unsigned int    tx_vcmctrl_sw_dly_sel_d06_ch1 : 4   ; /* [11..8] */
        // Comment of field: tx_vcmctrl_sw_sel_d06_ch1
        /*
         * TX通道1 选择拉共模的传输门阻抗，00默认电阻，01电阻减半，10减为1/4,11浮空
         */
        unsigned int    tx_vcmctrl_sw_sel_d06_ch1 : 2   ; /* [13..12] */
        // Comment of field: tx_classd_drv_n_sel_d06_ch1
        /*
         * TX通道1 CLD drv n管驱动能力选择
         * 00：x1
         * 01：x1.5
         * 10：x2
         * 11：x2.5
         */
        unsigned int    tx_classd_drv_n_sel_d06_ch1 : 2   ; /* [15..14] */
        // Comment of field: tx_classd_drv_p_sel_d06_ch1
        /*
         * TX通道1 CLD drv p管驱动能力选择
         * 00：x1
         * 01：x1.5
         * 10：x2
         * 11：x2.5
         */
        unsigned int    tx_classd_drv_p_sel_d06_ch1 : 2   ; /* [17..16] */
        // Comment of field: tx_vcm_sel_d06_ch1
        /*
         * TX通道1 SUM TOP使用的VCM选择
         * 0：IDAC vcmbuf产生
         * 1：TOP vcmbuf产生
         */
        unsigned int    tx_vcm_sel_d06_ch1    : 1   ; /* [18] */
        // Comment of field: tx_loop_ctrl_d06_ch1
        /*
         * TX通道1 环路控制切换，0为正常工作环路，1为depop环路
         */
        unsigned int    tx_loop_ctrl_d06_ch1  : 1   ; /* [19] */
        // Comment of field: tx_pop_ctrl_d06_ch1
        /*
         * TX通道1 正常工作模式与depop下电过程1切换，0为正常工作模式
         */
        unsigned int    tx_pop_ctrl_d06_ch1   : 1   ; /* [20] */
        // Comment of field: tx_pop_ctrl2_d06_ch1
        /*
         * TX通道1 depop下电过程1与下电过程2切换,0为下电模式1
         */
        unsigned int    tx_pop_ctrl2_d06_ch1  : 1   ; /* [21] */
        // Comment of field: tx_pop_drive_sel_d06_ch1
        /*
         * TX通道1 depop ldo输入参考电压选择
         * 0000000:浮空
         * 1000000：1.8V
         * 0100000: 1.52V
         * 0010000: 1.25V
         * 0001000: 0.97V
         * 0000100: 0.7V
         * 0000010: 0.42V
         * 0000001: 0.14V
         */
        unsigned int    tx_pop_drive_sel_d06_ch1 : 7   ; /* [28..22] */
        // Comment of field: tx_vcm_ctrl_pop_d06_ch1
        /*
         * TX通道1 DEPOP 输出分流
         * 0：不使能
         * 1：使能
         */
        unsigned int    tx_vcm_ctrl_pop_d06_ch1 : 1   ; /* [29] */
        // Comment of field: tx_vcm_pop_sel_ctrl_d06_ch1
        /*
         * TX通道1 DEPOP vcm传输门使能
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_vcm_pop_sel_ctrl_d06_ch1 : 1   ; /* [30] */
        // Comment of field: tx_vref_ctrl_d06_ch1
        /*
         * TX通道1 DEPOP vcm信号选择
         * 1：subdac电阻分压产生
         * 0：IDC vcmbuf产生
         */
        unsigned int    tx_vref_ctrl_d06_ch1  : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr06;

/* Define the union u_reg_analog_wr07 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: tx_dacvcm_iqenh_d06_ch2
        /*
         * TX通道2 IDAC vcmbuf功耗选择
         * 0000:7.5uA
         * 0001:15uA
         * 0010:22.5uA
         * 0011:30uA
         * 0100:37.5uA
         * 0101:45uA
         * 0110:52.5uA
         * 0111:60uA
         * 1000:67.5uA
         * 1001:75uA
         * 1010:82.5uA
         * 1011:90uA
         * 1100:97.5uA
         * 1101:105uA
         * 1110:112.5uA
         * 1111:120uA
         */
        unsigned int    tx_dacvcm_iqenh_d06_ch2 : 4   ; /* [3..0] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 5   ; /* [8..4] */
        // Comment of field: tx_cmfb_bias_ctrl_d06_ch2
        /*
         * TX通道2 CMFB op功耗选取
         * 0：输出级功耗15u
         * 1：输出级功耗30u
         */
        unsigned int    tx_cmfb_bias_ctrl_d06_ch2 : 1   ; /* [9] */
        // Comment of field: tx_deglitch_sel_d06_ch2
        /*
         * TX通道2 数据符号位相对调节延时，默认0
         */
        unsigned int    tx_deglitch_sel_d06_ch2 : 3   ; /* [12..10] */
        // Comment of field: reserved_1
        /*
         * Reserved
         */
        unsigned int    reserved_1            : 2   ; /* [14..13] */
        // Comment of field: tx_en_op1_chop_d06_ch2
        /*
         * TX通道2 LPFT OP1 CHOP使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_en_op1_chop_d06_ch2 : 1   ; /* [15] */
        // Comment of field: tx_sign_inv_sel_d06_ch2
        /*
         * TX通道2 CLD里VREF_PP_SIGN标志位方向
         * 0：不反向
         * 1：反向
         */
        unsigned int    tx_sign_inv_sel_d06_ch2 : 1   ; /* [16] */
        // Comment of field: tx_tdead_hsn_sel_d06_ch2
        /*
         * TX通道2 与cld_tdead_lsp_sel共同控制电流VOUTN->VOUTP时死区时间
         * 0000:不延时
         * 0001:delaycell*1
         * ...
         * 1111:delaycell*15
         */
        unsigned int    tx_tdead_hsn_sel_d06_ch2 : 4   ; /* [20..17] */
        // Comment of field: tx_tdead_hsp_sel_d06_ch2
        /*
         * TX通道2 与cld_tdead_lsn_sel共同控制电流VOUTP->VOUTN时死区时间
         * 0000:不延时
         * 0001:delaycell*1
         * ...
         * 1111:delaycell*15
         */
        unsigned int    tx_tdead_hsp_sel_d06_ch2 : 4   ; /* [24..21] */
        // Comment of field: tx_tdead_lsn_sel_d06_ch2
        /*
         * TX通道2 与cld_tdead_hsp_sel共同控制电流VOUTP->VOUTN时死区时间
         * 0000:不延时
         * 0001:delaycell*1
         * ...
         * 1111:delaycell*15
         */
        unsigned int    tx_tdead_lsn_sel_d06_ch2 : 4   ; /* [28..25] */
        // Comment of field: tx_dre_delay_sel_ch2
        /*
         * TX通道2 DRE电阻电容切换与接入vcm的延时
         * 000:0T
         * …
         * 111:7T
         */
        unsigned int    tx_dre_delay_sel_ch2  : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr07;

/* Define the union u_reg_analog_wr08 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: tx_tdead_lsp_sel_d06_ch2
        /*
         * TX通道2 与cld_tdead_hsn_sel共同控制电流VOUTN->VOUTP时死区时间
         * 0000:不延时
         * 0001:delaycell*1
         * ...
         * 1111:delaycell*15
         */
        unsigned int    tx_tdead_lsp_sel_d06_ch2 : 4   ; /* [3..0] */
        // Comment of field: tx_vcmctrl_dly_sel_d06_ch2
        /*
         * TX通道2 选择拉共模的小电阻传输门延时
         * 0000:不延时
         * ...
         * 1111:delaycell*15
         */
        unsigned int    tx_vcmctrl_dly_sel_d06_ch2 : 4   ; /* [7..4] */
        // Comment of field: tx_vcmctrl_sw_dly_sel_d06_ch2
        /*
         * TX通道2 选择拉共模的大电阻传输门延时
         * 0000:不延时
         * ...
         * 1111:delaycell*15
         */
        unsigned int    tx_vcmctrl_sw_dly_sel_d06_ch2 : 4   ; /* [11..8] */
        // Comment of field: tx_vcmctrl_sw_sel_d06_ch2
        /*
         * TX通道2 选择拉共模的传输门阻抗，00默认电阻，01电阻减半，10减为1/4,11浮空
         */
        unsigned int    tx_vcmctrl_sw_sel_d06_ch2 : 2   ; /* [13..12] */
        // Comment of field: tx_classd_drv_n_sel_d06_ch2
        /*
         * TX通道2 CLD drv n管驱动能力选择
         * 00：x1
         * 01：x1.5
         * 10：x2
         * 11：x2.5
         */
        unsigned int    tx_classd_drv_n_sel_d06_ch2 : 2   ; /* [15..14] */
        // Comment of field: tx_classd_drv_p_sel_d06_ch2
        /*
         * TX通道2 CLD drv p管驱动能力选择
         * 00：x1
         * 01：x1.5
         * 10：x2
         * 11：x2.5
         */
        unsigned int    tx_classd_drv_p_sel_d06_ch2 : 2   ; /* [17..16] */
        // Comment of field: tx_vcm_sel_d06_ch2
        /*
         * TX通道2 SUM TOP使用的VCM选择
         * 0：IDAC vcmbuf产生
         * 1：TOP vcmbuf产生
         */
        unsigned int    tx_vcm_sel_d06_ch2    : 1   ; /* [18] */
        // Comment of field: tx_loop_ctrl_d06_ch2
        /*
         * TX通道2 环路控制切换，0为正常工作环路，1为depop环路
         */
        unsigned int    tx_loop_ctrl_d06_ch2  : 1   ; /* [19] */
        // Comment of field: tx_pop_ctrl_d06_ch2
        /*
         * TX通道2 正常工作模式与depop下电过程1切换，0为正常工作模式
         */
        unsigned int    tx_pop_ctrl_d06_ch2   : 1   ; /* [20] */
        // Comment of field: tx_pop_ctrl2_d06_ch2
        /*
         * TX通道2 depop下电过程1与下电过程2切换,0为下电模式1
         */
        unsigned int    tx_pop_ctrl2_d06_ch2  : 1   ; /* [21] */
        // Comment of field: tx_pop_drive_sel_d06_ch2
        /*
         * TX通道2 depop ldo输入参考电压选择
         * 0000000:浮空
         * 1000000：1.8V
         * 0100000: 1.52V
         * 0010000: 1.25V
         * 0001000: 0.97V
         * 0000100: 0.7V
         * 0000010: 0.42V
         * 0000001: 0.14V
         */
        unsigned int    tx_pop_drive_sel_d06_ch2 : 7   ; /* [28..22] */
        // Comment of field: tx_vcm_ctrl_pop_d06_ch2
        /*
         * TX通道2 DEPOP 输出分流
         * 0：不使能
         * 1：使能
         */
        unsigned int    tx_vcm_ctrl_pop_d06_ch2 : 1   ; /* [29] */
        // Comment of field: tx_vcm_pop_sel_ctrl_d06_ch2
        /*
         * TX通道2 DEPOP vcm传输门使能
         * 1：使能
         * 0：不使能
         */
        unsigned int    tx_vcm_pop_sel_ctrl_d06_ch2 : 1   ; /* [30] */
        // Comment of field: tx_vref_ctrl_d06_ch2
        /*
         * TX通道2 DEPOP vcm信号选择
         * 1：subdac电阻分压产生
         * 0：IDC vcmbuf产生
         */
        unsigned int    tx_vref_ctrl_d06_ch2  : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr08;

/* Define the union u_reg_analog_wr09 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: rx_dre_gain_flag_d06_ch1
        /*
         * 通道1 寄存器控制DRE比较阈值信号高比特，与wr10<11>共同控制
         * 00：信号为大信号
         * 01：信号为中信号
         * 11：信号为小信号
         */
        unsigned int    rx_dre_gain_flag_d06_ch1 : 1   ; /* [0] */
        // Comment of field: rx_sdm_mute_fls_d06_ch1
        /*
         * 通道1 SDM MUTE控制：
         * 0：sdm正常工作
         * 1：sdm mute
         */
        unsigned int    rx_sdm_mute_fls_d06_ch1 : 1   ; /* [1] */
        // Comment of field: rx_dre_vthhl_sel_d06_ch1
        /*
         * 通道1 DRE 大增益档位下比较器阈值电压低阈值档位选择信号
         * 000：-31.5dB
         * 001：-25.5dB
         * 010：-22dB
         * 011：-19.5dB
         * 100：-17.5dB
         * 101：-15.9dB
         * 110：-14dB
         * 111：-12dB
         */
        unsigned int    rx_dre_vthhl_sel_d06_ch1 : 3   ; /* [4..2] */
        // Comment of field: rx_dre_gain_sel_d06_ch1
        /*
         * 通道1 DRE比较器阈值控制选择信号：
         * 0：由数字反馈回来的信号（DRE_GAIN)控制
         * 1：由寄存器信号（GAIN_FLAG）控制
         */
        unsigned int    rx_dre_gain_sel_d06_ch1 : 1   ; /* [5] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 4   ; /* [9..6] */
        // Comment of field: rx_dre_gain_sel_d06_ch2
        /*
         * 通道2 DRE比较器阈值控制选择信号：
         * 0：由数字反馈回来的信号（DRE_GAIN）控制
         * 1：由寄存器信号（GAIN_FLAG)控制
         */
        unsigned int    rx_dre_gain_sel_d06_ch2 : 1   ; /* [10] */
        // Comment of field: rx_dre_vthhl_sel_d06_ch2
        /*
         * 通道1 DRE 大增益档位下比较器阈值电压低阈值档位选择信号
         * 000：-31.5dB
         * 001：-25.5dB
         * 010：-22dB
         * 011：-19.5dB
         * 100：-17.5dB
         * 101：-15.9dB
         * 110：-14dB
         * 111：-12dB
         */
        unsigned int    rx_dre_vthhl_sel_d06_ch2 : 3   ; /* [13..11] */
        // Comment of field: rx_sdn_mute_fls_a09_ch2
        /*
         * adc的mute信号：0：adc正常输出码值，1：adc输出的码值高4bit接死为TIEL，低4bit接死为TIEH
         */
        unsigned int    rx_sdn_mute_fls_a09_ch2 : 1   ; /* [14] */
        // Comment of field: rx_dre_gain_flag_d06_ch2
        /*
         * 通道2 寄存器控制DRE比较阈值信号高比特，与wr13<11>共同控制
         * 00：信号为大信号
         * 01：信号为中信号
         * 11：信号为小信号
         */
        unsigned int    rx_dre_gain_flag_d06_ch2 : 1   ; /* [15] */
        // Comment of field: rx_dac_res_sel_a18_ch1
        /*
         * 通道一中保障vbgen模块稳定性的电阻选择,搭配reg_analog_wr09<18><19>使用
         * 00：输入电流1uA，且IBIAS_SEL为0选择（默认档位）
         * 01：输入电流1uA，且IBIAS_SEL为1选择
         * 10：输入电流1.5uA，且IBIAS_SEL为1选择
         */
        unsigned int    rx_dac_res_sel_a18_ch1 : 2   ; /* [17..16] */
        // Comment of field: rx_dac_op_ibias_sel_a18_ch1
        /*
         * 通道一种vbgen模块的偏置电流是否加倍，例如输入1u，不加倍电路内部仍使用1u，加倍电路内部实际使用为2u
         * 0：不加倍
         * 1：加倍
         */
        unsigned int    rx_dac_op_ibias_sel_a18_ch1 : 1   ; /* [18] */
        // Comment of field: rx_dac_mill_r_sel_a18_ch1
        /*
         * 通道一中保障vbgen模块稳定性的密勒电容选择，搭配reg_analog_wr09<18><17:16>使用
         * 0：输入电流1uA，且IBIAS_SEL为0选择（默认档位）
         * 1：输入电流1uA，且IBIAS_SEL为1选择
         */
        unsigned int    rx_dac_mill_r_sel_a18_ch1 : 1   ; /* [19] */
        // Comment of field: tx_pulse_width_sel_d06_ch1
        /*
         * TX depop脉宽选择（step约2.1n）
         * 0000：3.2n
         * 0001:5.3n
         * 0010:7.4n
         * …
         * 1111:34.7n
         */
        unsigned int    tx_pulse_width_sel_d06_ch1 : 4   ; /* [23..20] */
        // Comment of field: tx_idac_vbgen_hp_en
        /*
         * 未使用
         */
        unsigned int    tx_idac_vbgen_hp_en   : 1   ; /* [24] */
        // Comment of field: tx_idac_pw_mode_d06
        /*
         * vbgen输出电流档位选择
         * 000:0.638uA
         * 001:0.6uA
         * 010:0.566uA
         * 011:0.545uA
         * 100:0.526uA
         * 101:0.5uA
         */
        unsigned int    tx_idac_pw_mode_d06   : 3   ; /* [27..25] */
        // Comment of field: reserved_1
        /*
         * Reserved
         */
        unsigned int    reserved_1            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr09;

/* Define the union u_reg_analog_wr10 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: rx_clk_int_sel_d06_ch1
        /*
         * 通道1 积分器时钟频率选择：
         * 00x:时钟频率为6.144M
         * 01x:时钟频率为384k
         * 1x0:时钟频率为1.536M
         * 1x1:时钟频率为3.072M
         */
        unsigned int    rx_clk_int_sel_d06_ch1 : 3   ; /* [2..0] */
        // Comment of field: rx_clk_pga_sel_d06_ch1
        /*
         * 通道1 RX PGA时钟频率选择：
         * 00x:时钟频率为96k
         * 01x:时钟频率为384k
         * 1x0:时钟频率为768k
         * 1x1:时钟频率为3.072M
         */
        unsigned int    rx_clk_pga_sel_d06_ch1 : 3   ; /* [5..3] */
        // Comment of field: rx_dwa_byp_d06_ch1
        /*
         * 通道1 DWAbypass信号：
         * 0：DWA正常工作
         * 1：DWAbypass
         */
        unsigned int    rx_dwa_byp_d06_ch1    : 1   ; /* [6] */
        // Comment of field: rx_syn_clk_sel_d06_ch1
        /*
         * 通道1RX 输出DATA按照时钟沿输出选择：
         * 0：时钟上升沿输出
         * 1：时钟下降沿输出
         */
        unsigned int    rx_syn_clk_sel_d06_ch1 : 1   ; /* [7] */
        // Comment of field: rx_dre_mode_sel_d06_ch1
        /*
         * 通道1 DRE档位模式选择信号
         * 0：三档
         * 1：两档
         */
        unsigned int    rx_dre_mode_sel_d06_ch1 : 1   ; /* [8] */
        // Comment of field: rx_byp_dre_zc_d06_ch1
        /*
         * 电路中不需要ZC byp功能，该寄存器不再使用
         */
        unsigned int    rx_byp_dre_zc_d06_ch1 : 1   ; /* [9] */
        // Comment of field: rx_byp_flag_limit_d06_ch1
        /*
         * 通道1 DRE limit信号bypass
         * 0：不bypass
         * 1：bypass
         */
        unsigned int    rx_byp_flag_limit_d06_ch1 : 1   ; /* [10] */
        // Comment of field: rx_dre_gain_flag_d06_ch1
        /*
         * 通道1 寄存器控制DRE比较阈值信号低比特，与wr9<0>共同控制
         * 00：信号为大信号
         * 01：信号为中信号
         * 11：信号为小信号
         */
        unsigned int    rx_dre_gain_flag_d06_ch1 : 1   ; /* [11] */
        // Comment of field: rx_sdm_flstn_d06_ch1
        /*
         * 通道1RX flash模块LSB电压控制：
         * 00：LSB为85.69mV
         * 01: LSB为78.4mV
         * 10: LSB为71.8mV
         * 11: LSB为66.6mV
         */
        unsigned int    rx_sdm_flstn_d06_ch1  : 2   ; /* [13..12] */
        // Comment of field: rx_data_sel_d06_ch1
        /*
         * 通道1 RX data是否输出：
         * 0：正常输出
         * 1: 输出定值
         */
        unsigned int    rx_data_sel_d06_ch1   : 1   ; /* [14] */
        // Comment of field: rx_clk_dre_sel_d06_ch1
        /*
         * Reserved
         */
        unsigned int    rx_clk_dre_sel_d06_ch1 : 3   ; /* [17..15] */
        // Comment of field: rx_ch_sel_d06_ch1
        /*
         * 通道1 RX PGA输入通道使能信号：
         * 000：通道1、2、3均关闭
         * 001：通道1开启
         * 010：通道2开启
         * 100：通道3开启
         */
        unsigned int    rx_ch_sel_d06_ch1     : 3   ; /* [20..18] */
        // Comment of field: rx_en_pga_chp_d06_ch1
        /*
         * 通道1 RX PGA chopper使能信号：
         * 0：PGAchopper关闭
         * 1：PGA chopper使能
         */
        unsigned int    rx_en_pga_chp_d06_ch1 : 1   ; /* [21] */
        // Comment of field: rx_mute_pga_d06_ch1
        /*
         * 通道1 RX PGA MUTE信号：
         * 0：PGA正常工作
         * 1：PGA mute
         */
        unsigned int    rx_mute_pga_d06_ch1   : 1   ; /* [22] */
        // Comment of field: rx_en_dre_iadd_d06_ch1
        /*
         * Reserved
         */
        unsigned int    rx_en_dre_iadd_d06_ch1 : 1   ; /* [23] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 1   ; /* [24] */
        // Comment of field: rx_en_test_d06_ch1
        /*
         * 通道1RX维测通道使能信号：
         * 0：维测通道关闭
         * 1：维测通道使能
         */
        unsigned int    rx_en_test_d06_ch1    : 1   ; /* [25] */
        // Comment of field: rx_dre_vthh_sel_d06_ch1
        /*
         * 通道1 DRE 大增益档位下比较器阈值电压高阈值档位选择信号
         * 000：-10dB
         * 001：-6.9dB
         * 010：-6dB
         * 011：-5.1dB
         * 100：-4.1dB
         * 101：-3dB
         * 110：-2dB
         * 111：-1.4dB
         */
        unsigned int    rx_dre_vthh_sel_d06_ch1 : 3   ; /* [28..26] */
        // Comment of field: rx_dre_vthlh_sel_d06_ch1
        /*
         * 通道1 DRE 小增益档位下比较器阈值电压高阈值档位选择信号
         * 000：-31.5dB
         * 001：-25.5dB
         * 010：-22dB
         * 011：-19.5dB
         * 100：-17.5dB
         * 101：-15.9dB
         * 110：-14dB
         * 111：-12dB
         */
        unsigned int    rx_dre_vthlh_sel_d06_ch1 : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr10;

/* Define the union u_reg_analog_wr11 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: rx_dre_vthl_sel_d06_ch1
        /*
         * 通道1 DRE 小增益档位下比较器阈值电压低阈值档位选择信号
         * 000：-37.5dB
         * 001：-31.5dB
         * 010：-28dB
         * 011：-25.5dB
         * 100：-23.5dB
         * 101：-22dB
         * 110：-20.5dB
         * 111：-19.5dB
         */
        unsigned int    rx_dre_vthl_sel_d06_ch1 : 3   ; /* [2..0] */
        // Comment of field: rx_dre_vthp_sel_d06_ch1
        /*
         * 通道1 DRE 限幅比较器阈值电压档位选择信号
         * 000：-4.1dB
         * 001：-2dB
         * 010：-3dB
         * 011：-1.4dB
         */
        unsigned int    rx_dre_vthp_sel_d06_ch1 : 2   ; /* [4..3] */
        // Comment of field: rx_byp_sdm_vbn_res_a18_ch1
        /*
         * 通道一中VBN的Noisefilter结构的bypass使能
         * 0:关闭noisefilter（默认）
         * 1：开启noisefilter
         */
        unsigned int    rx_byp_sdm_vbn_res_a18_ch1 : 1   ; /* [5] */
        // Comment of field: rx_byp_sdm_vbp_res_a18_ch1
        /*
         * 通道一中VBP的Noisefilter结构的bypass使能，与reg_analog_wr11<10>一起设置
         * 0:关闭noisefilter（默认）
         * 1：开启noisefilter
         */
        unsigned int    rx_byp_sdm_vbp_res_a18_ch1 : 1   ; /* [6] */
        // Comment of field: rx_sdm_adatn_a18_ch1
        /*
         * 通路一中VBGEN模块下方电阻的选择
         * 00：电阻值为90%的R
         * 01：电阻值为95%的R
         * 10：电阻值为100%的R（默认配置）
         * 11：电阻值为105%的R
         */
        unsigned int    rx_sdm_adatn_a18_ch1  : 2   ; /* [8..7] */
        // Comment of field: rx_sdm_dac_ictln_a18_ch1_bit0
        /*
         * 通道一中调节DAC中icell的匹配电流
         * 0：提供icell偏置电流16.7u（默认配置）
         * 1：提供icell偏置电流16.7u*2
         */
        unsigned int    rx_sdm_dac_ictln_a18_ch1_bit0 : 1   ; /* [9] */
        // Comment of field: rx_sdm_dac_ictln_a18_ch1_bit1
        /*
         * 通道一中保障vbgen模块稳定性的调零电阻选择，与reg_analog_wr30<31>一起设置
         * 0:关闭noisefilter时选择（默认）
         * 1：开启noisefilter时选择
         */
        unsigned int    rx_sdm_dac_ictln_a18_ch1_bit1 : 1   ; /* [10] */
        // Comment of field: rx_sdm_dac_ictp_a18_ch1_bit0
        /*
         * 通道1中调节DAC中VBGEN模块的电阻接入
         * 0：原始电阻数值（默认配置）
         * 1：接入电阻值减半
         */
        unsigned int    rx_sdm_dac_ictp_a18_ch1_bit0 : 1   ; /* [11] */
        // Comment of field: rx_sdm_dac_ictp_a18_ch1_bit1_2
        /*
         * Reserved
         */
        unsigned int    rx_sdm_dac_ictp_a18_ch1_bit1_2 : 2   ; /* [13..12] */
        // Comment of field: rx_sdm_dac_idletone_a18_ch1
        /*
         * 通道1中ICELL种的IDLETONE配置：
         * 默认001100110000
         * 单个IDELETONE配置：
         * 001100：偏置电流0u
         * 100000：偏置电流8.35u
         * 100001：偏置电流16.7u
         */
        unsigned int    rx_sdm_dac_idletone_a18_ch1 : 12  ; /* [25..14] */
        // Comment of field: rx_sdm_rf_res_d06_ch1
        /*
         * 通道1 RX loopfilter输入电阻阻值控制：
         * 001:10k
         * 010:20k
         * 100:40k
         */
        unsigned int    rx_sdm_rf_res_d06_ch1 : 3   ; /* [28..26] */
        // Comment of field: rx_pga_rin_sel_d06_ch1
        /*
         * 通道1RX PGA RIN电阻阻值控制：
         * 001：5k
         * 010:2.5k
         */
        unsigned int    rx_pga_rin_sel_d06_ch1 : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr11;

/* Define the union u_reg_analog_wr12 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: rx_pga_ref_sel_a09_ch1
        /*
         * 通道1 PGA 共模反馈VREF控制：
         * 0：电平是0.9，输入是差分信号
         * 1：电平是1.1，输入是单端信号
         */
        unsigned int    rx_pga_ref_sel_a09_ch1 : 1   ; /* [0] */
        // Comment of field: rx_gain_code_a09_ch1
        /*
         * normaladc的pga增益档位控制：
         * 0000:0db；0001:2db；0010:4db；0011:6db；0100:8db；0101:10db；0110:12db；0111:14db；
         * 1000:16db；1001:18db；1010:20db；1011:22db；1100:24db；其余reserved
         */
        unsigned int    rx_gain_code_a09_ch1  : 4   ; /* [4..1] */
        // Comment of field: rx_clk_vb_sel_a09_ch1
        /*
         * Reserved
         */
        unsigned int    rx_clk_vb_sel_a09_ch1 : 3   ; /* [7..5] */
        // Comment of field: rx_zcd_iadd_a09_ch1
        /*
         * zc的比较器第一级电流档位：0：3uA,1:9uA
         */
        unsigned int    rx_zcd_iadd_a09_ch1   : 1   ; /* [8] */
        // Comment of field: rx_clk_zcd_sel_a09_ch1
        /*
         * 通道1 zc的输入时钟频率档位控制：
         * 00x:时钟频率为48k
         * 01x:时钟频率为384k
         * 1x0:时钟频率为96k
         * 1x1:时钟频率为192k
         */
        unsigned int    rx_clk_zcd_sel_a09_ch1 : 3   ; /* [11..9] */
        // Comment of field: rx_rb_zc_a09_ch1
        /*
         * zc的dff是否解复位，0：复位，1：正常工作
         */
        unsigned int    rx_rb_zc_a09_ch1      : 1   ; /* [12] */
        // Comment of field: rx_zc_mute_byp_a09_ch1
        /*
         * adc的mute信号的控制方式：0：由ZC模块过零检测控制；1：由输入的寄存器直接控制
         */
        unsigned int    rx_zc_mute_byp_a09_ch1 : 1   ; /* [13] */
        // Comment of field: rx_zcto_a09_ch1
        /*
         * zc模块将48K/96K时钟进行分频，00:256分频；01:512分频；10:1024分频；11:2048分频
         */
        unsigned int    rx_zcto_a09_ch1       : 2   ; /* [15..14] */
        // Comment of field: rx_zcto_a09_ch2
        /*
         * zc模块将48K/96K时钟进行分频，00:256分频；01:512分频；10:1024分频；11:2048分频
         */
        unsigned int    rx_zcto_a09_ch2       : 2   ; /* [17..16] */
        // Comment of field: rx_zc_mute_byp_a09_ch2
        /*
         * adc的mute信号的控制方式：0：由ZC模块过零检测控制；1：由输入的寄存器直接控制
         */
        unsigned int    rx_zc_mute_byp_a09_ch2 : 1   ; /* [18] */
        // Comment of field: rx_rb_zc_a09_ch2
        /*
         * zc的dff是否解复位，0：复位，1：正常工作
         */
        unsigned int    rx_rb_zc_a09_ch2      : 1   ; /* [19] */
        // Comment of field: rx_clk_zcd_sel_a09_ch2
        /*
         * 通道2 zc的输入时钟频率档位控制：
         * 00x:时钟频率为48k
         * 01x:时钟频率为384k
         * 1x0:时钟频率为96k
         * 1x1:时钟频率为192k
         */
        unsigned int    rx_clk_zcd_sel_a09_ch2 : 3   ; /* [22..20] */
        // Comment of field: rx_zcd_iadd_a09_ch2
        /*
         * zc的比较器第一级电流档位。
         * 0：3uA；
         * 1：9uA。
         */
        unsigned int    rx_zcd_iadd_a09_ch2   : 1   ; /* [23] */
        // Comment of field: rx_clk_vb_sel_a09_ch2
        /*
         * Reserved
         */
        unsigned int    rx_clk_vb_sel_a09_ch2 : 3   ; /* [26..24] */
        // Comment of field: rx_gain_code_a09_ch2
        /*
         * 通道2 normaladc的pga增益档位控制（2dB/Step）；
         * 0000：0dB；
         * 0001：2dB；
         * ……
         * 1100：24dB。其余reserved
         */
        unsigned int    rx_gain_code_a09_ch2  : 4   ; /* [30..27] */
        // Comment of field: rx_pga_ref_sel_a09_ch2
        /*
         * 通道2 PGA 共模反馈VREF控制：
         * 0：电平是0.9，输入是差分信号
         * 1：电平是1.1，输入是单端信号
         */
        unsigned int    rx_pga_ref_sel_a09_ch2 : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr12;

/* Define the union u_reg_analog_wr13 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: rx_clk_int_sel_d06_ch2
        /*
         * 通道2 积分器时钟频率选择：
         * 00x:时钟频率为6.144M
         * 01x:时钟频率为384k
         * 1x0:时钟频率为1.536M
         * 1x1:时钟频率为3.072M
         */
        unsigned int    rx_clk_int_sel_d06_ch2 : 3   ; /* [2..0] */
        // Comment of field: rx_clk_pga_sel_d06_ch2
        /*
         * 通道2 RX PGA时钟频率选择：
         * 00x:时钟频率为96k
         * 01x:时钟频率为384k
         * 1x0:时钟频率为768k
         * 1x1:时钟频率为3.072M
         */
        unsigned int    rx_clk_pga_sel_d06_ch2 : 3   ; /* [5..3] */
        // Comment of field: rx_dwa_byp_d06_ch2
        /*
         * 通道2 DWAbypass信号：
         * 0：DWA正常工作
         * 1：DWAbypass
         */
        unsigned int    rx_dwa_byp_d06_ch2    : 1   ; /* [6] */
        // Comment of field: rx_syn_clk_sel_d06_ch2
        /*
         * 通道2RX 输出DATA按照时钟沿输出选择：
         * 0：时钟上升沿输出
         * 1：时钟下降沿输出
         */
        unsigned int    rx_syn_clk_sel_d06_ch2 : 1   ; /* [7] */
        // Comment of field: rx_dre_mode_sel_d06_ch2
        /*
         * 通道2 DRE档位模式选择信号
         * 0：三档
         * 1：两档
         */
        unsigned int    rx_dre_mode_sel_d06_ch2 : 1   ; /* [8] */
        // Comment of field: rx_byp_dre_zc_d06_ch2
        /*
         * Reserved
         */
        unsigned int    rx_byp_dre_zc_d06_ch2 : 1   ; /* [9] */
        // Comment of field: rx_byp_flag_limit_d06_ch2
        /*
         * 通道2 DRE limit信号bypass
         * 0：不bypass
         * 1：bypass
         */
        unsigned int    rx_byp_flag_limit_d06_ch2 : 1   ; /* [10] */
        // Comment of field: rx_dre_gain_flag_d06_ch2
        /*
         * 通道2 寄存器控制DRE比较阈值信号低比特，与wr9<15>共同控制
         * 00：信号为大信号
         * 01：信号为中信号
         * 11：信号为小信号
         */
        unsigned int    rx_dre_gain_flag_d06_ch2 : 1   ; /* [11] */
        // Comment of field: rx_sdm_flstn_d06_ch2
        /*
         * 通道2RX flash模块LSB电压控制：
         * 00：LSB为85.69mV
         * 01: LSB为78.4mV
         * 10: LSB为71.8mV
         * 11: LSB为66.6mV
         */
        unsigned int    rx_sdm_flstn_d06_ch2  : 2   ; /* [13..12] */
        // Comment of field: rx_data_sel_d06_ch2
        /*
         * 通道2 RX data是否输出：
         * 0：正常输出
         * 1: 输出定值
         */
        unsigned int    rx_data_sel_d06_ch2   : 1   ; /* [14] */
        // Comment of field: rx_clk_dre_sel_d06_ch2
        /*
         * Reserved
         */
        unsigned int    rx_clk_dre_sel_d06_ch2 : 3   ; /* [17..15] */
        // Comment of field: rx_ch_sel_d06_ch2
        /*
         * 通道2 RX PGA输入通道使能信号：
         * 000：通道1、2、3均关闭
         * 001：通道1开启
         * 010：通道2开启
         * 100：通道3开启
         */
        unsigned int    rx_ch_sel_d06_ch2     : 3   ; /* [20..18] */
        // Comment of field: rx_en_pga_chp_d06_ch2
        /*
         * 通道2 RX PGA chopper使能信号：
         * 0：PGAchopper关闭
         * 1：PGA chopper使能
         */
        unsigned int    rx_en_pga_chp_d06_ch2 : 1   ; /* [21] */
        // Comment of field: rx_mute_pga_d06_ch2
        /*
         * 通道2 RX PGA MUTE信号：
         * 0：PGA正常工作
         * 1：PGA mute
         */
        unsigned int    rx_mute_pga_d06_ch2   : 1   ; /* [22] */
        // Comment of field: rx_en_dre_iadd_d06_ch2
        /*
         * Reserved
         */
        unsigned int    rx_en_dre_iadd_d06_ch2 : 1   ; /* [23] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 1   ; /* [24] */
        // Comment of field: rx_en_test_d06_ch2
        /*
         * 通道2RX维测通道使能信号：
         * 0：维测通道关闭
         * 1：维测通道使能
         */
        unsigned int    rx_en_test_d06_ch2    : 1   ; /* [25] */
        // Comment of field: rx_dre_vthh_sel_d06_ch2
        /*
         * 通道1 DRE 大增益档位下比较器阈值电压高阈值档位选择信号
         * 000：-10dB
         * 001：-6.9dB
         * 010：-6dB
         * 011：-5.1dB
         * 100：-4.1dB
         * 101：-3dB
         * 110：-2dB
         * 111：-1.4dB
         */
        unsigned int    rx_dre_vthh_sel_d06_ch2 : 3   ; /* [28..26] */
        // Comment of field: rx_dre_vthlh_sel_d06_ch2
        /*
         * 通道1 DRE 小增益档位下比较器阈值电压高阈值档位选择信号
         * 000：-31.5dB
         * 001：-25.5dB
         * 010：-22dB
         * 011：-19.5dB
         * 100：-17.5dB
         * 101：-15.9dB
         * 110：-14dB
         * 111：-12dB
         */
        unsigned int    rx_dre_vthlh_sel_d06_ch2 : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr13;

/* Define the union u_reg_analog_wr14 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: rx_dre_vthl_sel_d06_ch2
        /*
         * 通道2 DRE 小增益档位下比较器阈值电压低阈值档位选择信号
         * 000：-37.5dB
         * 001：-31.5dB
         * 010：-28dB
         * 011：-25.5dB
         * 100：-23.5dB
         * 101：-22dB
         * 110：-20.5dB
         * 111：-19.5dB
         */
        unsigned int    rx_dre_vthl_sel_d06_ch2 : 3   ; /* [2..0] */
        // Comment of field: rx_dre_vthp_sel_d06_ch2
        /*
         * 通道1 DRE 限幅比较器阈值电压档位选择信号
         * 000：-4.1dB
         * 001：-2dB
         * 010：-3dB
         * 011：-1.4dB
         */
        unsigned int    rx_dre_vthp_sel_d06_ch2 : 2   ; /* [4..3] */
        // Comment of field: rx_byp_sdm_vbn_res_a18_ch2
        /*
         * 通道二中VBN的Noisefilter结构的bypass使能
         * 0:关闭noisefilter（默认）
         * 1：开启noisefilter
         */
        unsigned int    rx_byp_sdm_vbn_res_a18_ch2 : 1   ; /* [5] */
        // Comment of field: rx_byp_sdm_vbp_res_a18_ch2
        /*
         * 通道二中VBP的Noisefilter结构的bypass使能
         * 0:关闭noisefilter（默认）
         * 1：开启noisefilter
         */
        unsigned int    rx_byp_sdm_vbp_res_a18_ch2 : 1   ; /* [6] */
        // Comment of field: rx_sdm_adatn_a18_ch2
        /*
         * 通路二中VBGEN模块下方电阻的选择
         * 00：电阻值为90%的R
         * 01：电阻值为95%的R
         * 10：电阻值为100%的R（默认配置）
         * 11：电阻值为105%的R
         */
        unsigned int    rx_sdm_adatn_a18_ch2  : 2   ; /* [8..7] */
        // Comment of field: rx_sdm_dac_ictln_a18_ch2_bit0
        /*
         * 通道二中调节DAC中icell的匹配电流
         * 0：提供icell偏置电流16.7u（默认配置）
         * 1：提供icell偏置电流16.7u*2
         */
        unsigned int    rx_sdm_dac_ictln_a18_ch2_bit0 : 1   ; /* [9] */
        // Comment of field: rx_sdm_dac_ictln_a18_ch2_bit1
        /*
         * 通道二中保障vbgen模块稳定性的调零电阻选择，与reg_analog_wr30<31>一起设置
         * 0:关闭noisefilter时选择（默认）
         * 1：开启noisefilter时选择
         */
        unsigned int    rx_sdm_dac_ictln_a18_ch2_bit1 : 1   ; /* [10] */
        // Comment of field: rx_sdm_dac_ictp_a18_ch2_bit0
        /*
         * 通道二中调节DAC中VBGEN模块的电阻接入
         * 0：原始电阻数值（默认配置）
         * 1：接入电阻值减半
         */
        unsigned int    rx_sdm_dac_ictp_a18_ch2_bit0 : 1   ; /* [11] */
        // Comment of field: rx_sdm_dac_ictp_a18_ch2_bit1_2
        /*
         * Reserved
         */
        unsigned int    rx_sdm_dac_ictp_a18_ch2_bit1_2 : 2   ; /* [13..12] */
        // Comment of field: rx_sdm_dac_idletone_a18_ch2
        /*
         * 通道二中ICELL种的IDLETONE配置：
         * 默认001100110000
         * 单个IDELETONE配置：
         * 001100：偏置电流0u
         * 100000：偏置电流8.35u
         * 100001：偏置电流16.7u
         */
        unsigned int    rx_sdm_dac_idletone_a18_ch2 : 12  ; /* [25..14] */
        // Comment of field: rx_sdm_rf_res_d06_ch2
        /*
         * 通道2 RX loopfilter输入电阻阻值控制：
         * 001:10k
         * 010:20k
         * 100:40k
         */
        unsigned int    rx_sdm_rf_res_d06_ch2 : 3   ; /* [28..26] */
        // Comment of field: rx_pga_rin_sel_d06_ch2
        /*
         * 通道2 RX PGA RIN电阻阻值控制：
         * 001：5k
         * 010:2.5k
         */
        unsigned int    rx_pga_rin_sel_d06_ch2 : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr14;

/* Define the union u_reg_analog_wr15 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: mad_pga_mute1_d06
        /*
         * lpadc通路pga mute控制开关1：
         * 0：pga正常工作；
         * 1：pga mute，输入输出短接。
         */
        unsigned int    mad_pga_mute1_d06     : 1   ; /* [0] */
        // Comment of field: mad_pga_mute2_d06
        /*
         * lpadc通路pga mute控制开关2：
         * 0：pga正常工作；
         * 1：pga mute，输入输出短接。
         */
        unsigned int    mad_pga_mute2_d06     : 1   ; /* [1] */
        // Comment of field: mad_byp_pga_chp_d06
        /*
         * lpadc通路pga chopper功能控制开关：
         * 0：pga chopper打开；
         * 1：pga chopper关闭。
         */
        unsigned int    mad_byp_pga_chp_d06   : 1   ; /* [2] */
        // Comment of field: mad_byp_unlock_pga_d06
        /*
         * lpadc通路pga防共模死锁功能控制开关：
         * 0：pga防共模死锁打开；
         * 1：pga防共模死锁关闭。
         */
        unsigned int    mad_byp_unlock_pga_d06 : 1   ; /* [3] */
        // Comment of field: mad_pga_gain_d06
        /*
         * lpadc通路pga增益档位选择：
         * 0000：0dB；
         * 0001：2dB；
         * ……
         * 1100：24dB；
         * 1101：24dB；
         * 1110；24dB；
         * 1111：24dB。
         */
        unsigned int    mad_pga_gain_d06      : 4   ; /* [7..4] */
        // Comment of field: mad_zcd_ctr_d06
        /*
         * lpadc通路zcd模块时钟分频选择：
         * 00：时钟256分频；
         * 01：时钟512分频;
         * 10：时钟1024分频;
         * 11：时钟2048分频。
         */
        unsigned int    mad_zcd_ctr_d06       : 2   ; /* [9..8] */
        // Comment of field: mad_zcd_mute_byp_d06
        /*
         * lpadc通路zcd模块mute功能控制开关：
         * 0：zcd模块正常mute；
         * 1：zcd模块mute功能bypass。
         */
        unsigned int    mad_zcd_mute_byp_d06  : 1   ; /* [10] */
        // Comment of field: mad_sdm_boost_d06
        /*
         * lpadc通路sdm模块输入boost开关：
         * 0：boost模式关；
         * 1：boost模式开，sdm增益为26dB。
         */
        unsigned int    mad_sdm_boost_d06     : 1   ; /* [11] */
        // Comment of field: mad_sdm_vpu_sel_d06
        /*
         * lpadc通路sdm模块输入测试开关：
         * 0：sdm输入为pga输出；
         * 1：sdm输入为mic1外灌。
         */
        unsigned int    mad_sdm_vpu_sel_d06   : 1   ; /* [12] */
        // Comment of field: mad_sdm_res_sel_d06
        /*
         * lpadc通路sdm模块idel tone调节模块：
         * 0000：拉偏7.5%；
         * 0001：拉偏3.5%
         * 0011：拉偏1.5%；
         * 0111：拉偏0.5%；
         * 1111：拉偏0%。
         */
        unsigned int    mad_sdm_res_sel_d06   : 4   ; /* [16..13] */
        // Comment of field: mad_sdm_adatn_d06
        /*
         * lpadc通路sdm dac模块v2i电阻档位调节（默认配置10）：
         * 00：80K；
         * 01：85K；
         * 10：90K，默认档位；
         * 11：100K。
         */
        unsigned int    mad_sdm_adatn_d06     : 2   ; /* [18..17] */
        // Comment of field: mad_sdm_byp_chp_s1_d06
        /*
         * lpadc通路sdm 积分器op1 chopper功能控制开关：
         * 0：积分器op1 chopper打开；
         * 1：积分器op1 chopper关闭。
         */
        unsigned int    mad_sdm_byp_chp_s1_d06 : 1   ; /* [19] */
        // Comment of field: mad_sdm_byp_chp_s2_d06
        /*
         * lpadc通路sdm 积分器op2 chopper功能控制开关：
         * 0：积分器op2 chopper打开；
         * 1：积分器op2 chopper关闭。
         */
        unsigned int    mad_sdm_byp_chp_s2_d06 : 1   ; /* [20] */
        // Comment of field: mad_sdm_flstn_d06
        /*
         * lpadc通路sdm flash模块vref电压档位调节（默认配置10）：
         * 00：43.8mV；
         * 01：40mV；
         * 10：36.7mV，默认档位；
         * 11：32.7mV。
         */
        unsigned int    mad_sdm_flstn_d06     : 2   ; /* [22..21] */
        // Comment of field: mad_sdm_mute_fls_d06
        /*
         * lpadc通路sdm输出mute控制：
         * 0：sdm正常输出码字；
         * 1：sdm输出为默认0100；
         */
        unsigned int    mad_sdm_mute_fls_d06  : 1   ; /* [23] */
        // Comment of field: mad_dwa_byp_d06
        /*
         * lpadc通路dwa模块byp控制：
         * 0：dwa模块正常工作；
         * 1：byp dwa模块；
         */
        unsigned int    mad_dwa_byp_d06       : 1   ; /* [24] */
        // Comment of field: mad_dwa_rst_d06
        /*
         * lpadc通路dwa模块dff复位控制：
         * 0：dwa模块dff正常工作；
         * 1：dwa模块dff复位；
         */
        unsigned int    mad_dwa_rst_d06       : 1   ; /* [25] */
        // Comment of field: mad_clk_dig_sel_d06
        /*
         * lpadc通路sdm输出码字打拍时钟选择：
         * 0：拍数时钟使用clk_fls反向时钟；
         * 1：拍数时钟使用clk_dig；
         */
        unsigned int    mad_clk_dig_sel_d06   : 1   ; /* [26] */
        // Comment of field: mad_clk_edge_sel_d06
        /*
         * lpadc通路sdm输出码字打拍时钟clk_dig选择：
         * 0：clk_dig与clk_codec_ana同向；
         * 1：clk_dig与clk_codec_ana反向；
         */
        unsigned int    mad_clk_edge_sel_d06  : 1   ; /* [27] */
        // Comment of field: mad_clk_int_sel_d06
        /*
         * lpadc通路sdm积分器chopper时钟频率选择：
         * 00：chopper频率为192K；
         * 01：chopper频率为384K；
         * 10：chopper频率为96K；
         * 11：chopper频率为3.072M。
         */
        unsigned int    mad_clk_int_sel_d06   : 2   ; /* [29..28] */
        // Comment of field: mad_clk_pga_sel_d06
        /*
         * lpadc通路pga chopper时钟频率选择：
         * 00：chopper频率为96K；
         * 01：chopper频率为192K；
         * 10：chopper频率为1.536M；
         * 11：chopper频率为3.072M。
         */
        unsigned int    mad_clk_pga_sel_d06   : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr15;

/* Define the union u_reg_analog_wr16 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: ibct_mad_pga_1p25u_d06
        /*
         * lpadc通路pga op偏置电流选择：
         * 00：1uA；
         * 01：1.25uA（默认）；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_mad_pga_1p25u_d06 : 2   ; /* [1..0] */
        // Comment of field: ibct_mad_zcd_1p25u_d06
        /*
         * lpadc通路zcd模块比较器偏置电流选择：
         * 00：1uA；
         * 01：1.25uA（默认）；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_mad_zcd_1p25u_d06 : 2   ; /* [3..2] */
        // Comment of field: ibct_mad_int1_1p25u_d06
        /*
         * lpadc通路sdm积分器op1偏置电流选择：
         * 00：1uA；
         * 01：1.25uA（默认）；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_mad_int1_1p25u_d06 : 2   ; /* [5..4] */
        // Comment of field: ibct_mad_int2_1p25u_d06
        /*
         * lpadc通路sdm积分器op2偏置电流选择：
         * 00：1uA；
         * 01：1.25uA（默认）；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_mad_int2_1p25u_d06 : 2   ; /* [7..6] */
        // Comment of field: ibct_mad_dac_1p25u_d06
        /*
         * lpadc通路dac_vb op偏置电流选择：
         * 00：1uA；
         * 01：1.25uA（默认）；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_mad_dac_1p25u_d06 : 2   ; /* [9..8] */
        // Comment of field: ibct_mad_fls_1u_d06
        /*
         * lpadc通路flash偏置电流选择：
         * 00：1uA（默认）；
         * 01：1.25uA；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_mad_fls_1u_d06   : 2   ; /* [11..10] */
        // Comment of field: ibct_rx_dac_1u_d06_ch1
        /*
         * rx ch1通路dac偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_rx_dac_1u_d06_ch1 : 2   ; /* [13..12] */
        // Comment of field: ibct_rx_fls_1u_d06_ch1
        /*
         * rx ch1通路flash偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_rx_fls_1u_d06_ch1 : 2   ; /* [15..14] */
        // Comment of field: ibct_rx_int1_1u_d06_ch1
        /*
         * rx ch1通路sdm积分器op1偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_rx_int1_1u_d06_ch1 : 2   ; /* [17..16] */
        // Comment of field: ibct_rx_int2_1u_d06_ch1
        /*
         * rx ch1通路sdm积分器op2偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_rx_int2_1u_d06_ch1 : 2   ; /* [19..18] */
        // Comment of field: ibct_rx_pga_1u_d06_ch1
        /*
         * rx ch1通路pga模块op偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_rx_pga_1u_d06_ch1 : 2   ; /* [21..20] */
        // Comment of field: ibct_rx_dre_comp_1u_d06_ch1
        /*
         * rx ch1通路dre模块比较器偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_rx_dre_comp_1u_d06_ch1 : 2   ; /* [23..22] */
        // Comment of field: ibct_rx_dre_rega_1u_d06_ch1
        /*
         * rx ch1通路dre模块基准产生电路OPA偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_rx_dre_rega_1u_d06_ch1 : 2   ; /* [25..24] */
        // Comment of field: ibct_rx_dre_regb_1u_d06_ch1
        /*
         * rx ch1通路dre模块基准产生电路OPB偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_rx_dre_regb_1u_d06_ch1 : 2   ; /* [27..26] */
        // Comment of field: ibct_rx_dre_zcd_1u_d06_ch1
        /*
         * rx ch1通路dre中zcd模块比较器偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_rx_dre_zcd_1u_d06_ch1 : 2   ; /* [29..28] */
        // Comment of field: ibias_rx_zcd_sel_d06_ch1
        /*
         * rx ch1通路zcd模块比较器偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibias_rx_zcd_sel_d06_ch1 : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr16;

/* Define the union u_reg_analog_wr17 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: ibct_rx_dac_1u_d06_ch2
        /*
         * rx ch2通路dac偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_rx_dac_1u_d06_ch2 : 2   ; /* [1..0] */
        // Comment of field: ibct_rx_fls_1u_d06_ch2
        /*
         * rx ch2通路flash偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_rx_fls_1u_d06_ch2 : 2   ; /* [3..2] */
        // Comment of field: ibct_rx_int1_1u_d06_ch2
        /*
         * rx ch2通路sdm积分器op1偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_rx_int1_1u_d06_ch2 : 2   ; /* [5..4] */
        // Comment of field: ibct_rx_int2_1u_d06_ch2
        /*
         * rx ch2通路sdm积分器op2偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_rx_int2_1u_d06_ch2 : 2   ; /* [7..6] */
        // Comment of field: ibct_rx_pga_1u_d06_ch2
        /*
         * rx ch2通路pga模块op偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_rx_pga_1u_d06_ch2 : 2   ; /* [9..8] */
        // Comment of field: ibct_rx_dre_comp_1u_d06_ch2
        /*
         * rx ch2通路dre模块比较器偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_rx_dre_comp_1u_d06_ch2 : 2   ; /* [11..10] */
        // Comment of field: ibct_rx_dre_rega_1u_d06_ch2
        /*
         * rx ch2通路dre模块基准产生电路OPA偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_rx_dre_rega_1u_d06_ch2 : 2   ; /* [13..12] */
        // Comment of field: ibct_rx_dre_regb_1u_d06_ch2
        /*
         * rx ch2通路dre模块基准产生电路OPB偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_rx_dre_regb_1u_d06_ch2 : 2   ; /* [15..14] */
        // Comment of field: ibct_rx_dre_zcd_1u_d06_ch2
        /*
         * rx ch2通路dre中zcd模块比较器偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_rx_dre_zcd_1u_d06_ch2 : 2   ; /* [17..16] */
        // Comment of field: ibias_rx_zcd_sel_d06_ch2
        /*
         * rx ch2通路zcd模块比较器偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibias_rx_zcd_sel_d06_ch2 : 2   ; /* [19..18] */
        // Comment of field: ibct_rx_nf_vbn0_0p5u_d06_ch1
        /*
         * rx ch1通路noise filter模块VBN OP1偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_rx_nf_vbn0_0p5u_d06_ch1 : 2   ; /* [21..20] */
        // Comment of field: ibct_rx_nf_vbn1_0p5u_d06_ch1
        /*
         * rx ch1通路noise filter模块VBN OP2偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_rx_nf_vbn1_0p5u_d06_ch1 : 2   ; /* [23..22] */
        // Comment of field: ibct_rx_nf_vbn2_0p5u_d06_ch1
        /*
         * rx ch1通路noise filter模块VBN伪电阻RES1偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_rx_nf_vbn2_0p5u_d06_ch1 : 2   ; /* [25..24] */
        // Comment of field: ibct_rx_nf_vbn3_0p5u_d06_ch1
        /*
         * rx ch1通路noise filter模块VBN伪电阻RES2偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_rx_nf_vbn3_0p5u_d06_ch1 : 2   ; /* [27..26] */
        // Comment of field: ibct_rx_nf_vbp0_0p5u_d06_ch1
        /*
         * rx ch1通路noise filter模块VBP OP1偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_rx_nf_vbp0_0p5u_d06_ch1 : 2   ; /* [29..28] */
        // Comment of field: ibct_rx_nf_vbp1_0p5u_d06_ch1
        /*
         * rx ch1通路noise filter模块VBP OP2偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_rx_nf_vbp1_0p5u_d06_ch1 : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr17;

/* Define the union u_reg_analog_wr18 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: ibct_rx_nf_vbp2_0p5u_d06_ch1
        /*
         * rx ch1通路noise filter模块VBP伪电阻RES1偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_rx_nf_vbp2_0p5u_d06_ch1 : 2   ; /* [1..0] */
        // Comment of field: ibct_rx_nf_vbp3_0p5u_d06_ch1
        /*
         * rx ch1通路noise filter模块VBP伪电阻RES2偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_rx_nf_vbp3_0p5u_d06_ch1 : 2   ; /* [3..2] */
        // Comment of field: ibct_rx_nf_vbn0_0p5u_d06_ch2
        /*
         * rx ch2通路noise filter模块VBN OP1偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_rx_nf_vbn0_0p5u_d06_ch2 : 2   ; /* [5..4] */
        // Comment of field: ibct_rx_nf_vbn1_0p5u_d06_ch2
        /*
         * rx ch2通路noise filter模块VBN OP2偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_rx_nf_vbn1_0p5u_d06_ch2 : 2   ; /* [7..6] */
        // Comment of field: ibct_rx_nf_vbn2_0p5u_d06_ch2
        /*
         * rx ch2通路noise filter模块VBN伪电阻RES1偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_rx_nf_vbn2_0p5u_d06_ch2 : 2   ; /* [9..8] */
        // Comment of field: ibct_rx_nf_vbn3_0p5u_d06_ch2
        /*
         * rx ch2通路noise filter模块VBN伪电阻RES2偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_rx_nf_vbn3_0p5u_d06_ch2 : 2   ; /* [11..10] */
        // Comment of field: ibct_rx_nf_vbp0_0p5u_d06_ch2
        /*
         * rx ch2通路noise filter模块VBP OP1偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_rx_nf_vbp0_0p5u_d06_ch2 : 2   ; /* [13..12] */
        // Comment of field: ibct_rx_nf_vbp1_0p5u_d06_ch2
        /*
         * rx ch2通路noise filter模块VBP OP2偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_rx_nf_vbp1_0p5u_d06_ch2 : 2   ; /* [15..14] */
        // Comment of field: ibct_rx_nf_vbp2_0p5u_d06_ch2
        /*
         * rx ch2通路noise filter模块VBP伪电阻RES1偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_rx_nf_vbp2_0p5u_d06_ch2 : 2   ; /* [17..16] */
        // Comment of field: ibct_rx_nf_vbp3_0p5u_d06_ch2
        /*
         * rx ch2通路noise filter模块VBP伪电阻RES2偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_rx_nf_vbp3_0p5u_d06_ch2 : 2   ; /* [19..18] */
        // Comment of field: ibct_tx_dac_2p5u_d06
        /*
         * tx ch1通路idac_vb op偏置电流选择：
         * 00：1uA；
         * 01：1.25uA（默认）；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_tx_dac_2p5u_d06  : 2   ; /* [21..20] */
        // Comment of field: ibct_tx_dac_vcm_2p5u_d06_ch1
        /*
         * tx ch1通路vcm buffer op偏置电流选择：
         * 00：1uA；
         * 01：1.25uA（默认）；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_tx_dac_vcm_2p5u_d06_ch1 : 2   ; /* [23..22] */
        // Comment of field: ibct_tx_dac_vcm_2p5u_d06_ch2
        /*
         * tx ch2通路vcm buffer op偏置电流选择：
         * 00：1uA；
         * 01：1.25uA（默认）；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_tx_dac_vcm_2p5u_d06_ch2 : 2   ; /* [25..24] */
        // Comment of field: ibct_tx_nf_vbn_op1_0p5u_d06
        /*
         * tx通路noise filter模块VBN OP1偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_tx_nf_vbn_op1_0p5u_d06 : 2   ; /* [27..26] */
        // Comment of field: ibct_tx_nf_vbn_op2_0p5u_d06
        /*
         * tx通路noise filter模块VBN OP2偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_tx_nf_vbn_op2_0p5u_d06 : 2   ; /* [29..28] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr18;

/* Define the union u_reg_analog_wr19 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: ibct_tx_nf_vbn_res1_0p5u_d06
        /*
         * tx通路noise filter模块VBN伪电阻RES1偏置电流选择：
         * 000：0.125uA；
         * 001：0.25uA；
         * 010：0.375uA；
         * 011：0.5uA（默认）；
         * 100：0.625uA；
         * 101：0.75uA；
         * 110：0.875uA；
         * 111：1uA。
         */
        unsigned int    ibct_tx_nf_vbn_res1_0p5u_d06 : 3   ; /* [2..0] */
        // Comment of field: ibct_tx_nf_vbn_res2_0p5u_d06
        /*
         * tx通路noise filter模块VBN伪电阻RES2偏置电流选择：
         * 000：0.125uA；
         * 001：0.25uA；
         * 010：0.375uA；
         * 011：0.5uA（默认）；
         * 100：0.625uA；
         * 101：0.75uA；
         * 110：0.875uA；
         * 111：1uA。
         */
        unsigned int    ibct_tx_nf_vbn_res2_0p5u_d06 : 3   ; /* [5..3] */
        // Comment of field: ibct_tx_nf_vbp_op1_0p5u_d06
        /*
         * tx通路noise filter模块VBP OP1偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_tx_nf_vbp_op1_0p5u_d06 : 2   ; /* [7..6] */
        // Comment of field: ibct_tx_nf_vbp_op2_0p5u_d06
        /*
         * tx通路noise filter模块VBP OP2偏置电流选择：
         * 00：0.375uA；
         * 01：0.5uA（默认）；
         * 10：0.625uA；
         * 11：0.75uA。
         */
        unsigned int    ibct_tx_nf_vbp_op2_0p5u_d06 : 2   ; /* [9..8] */
        // Comment of field: ibct_tx_nf_vbp_res1_0p5u_d06
        /*
         * tx通路noise filter模块VBP伪电阻RES1偏置电流选择：
         * 000：0.125uA；
         * 001：0.25uA；
         * 010：0.375uA；
         * 011：0.5uA（默认）；
         * 100：0.625uA；
         * 101：0.75uA；
         * 110：0.875uA；
         * 111：1uA。
         */
        unsigned int    ibct_tx_nf_vbp_res1_0p5u_d06 : 3   ; /* [12..10] */
        // Comment of field: ibct_tx_nf_vbp_res2_0p5u_d06
        /*
         * tx通路noise filter模块VBP伪电阻RES2偏置电流选择：
         * 000：0.125uA；
         * 001：0.25uA；
         * 010：0.375uA；
         * 011：0.5uA（默认）；
         * 100：0.625uA；
         * 101：0.75uA；
         * 110：0.875uA；
         * 111：1uA。
         */
        unsigned int    ibct_tx_nf_vbp_res2_0p5u_d06 : 3   ; /* [15..13] */
        // Comment of field: ibct_tx_int1_1p25u_d06_ch1
        /*
         * tx ch1通路积分器op1偏置电流选择：
         * 0000：0.5uA；
         * 0001：0.625uA；
         * 0010：0.75uA；
         * 0011：0.875uA；
         * 0100：1uA；
         * 0101：1.125uA；
         * 0110：1.25uA（默认）；
         * 0111：1.375；
         * 1000：1.5uA；
         * 1001：1.625uA；
         * 1010：1.75uA；
         * 1011：1.875uA；
         * 1100：2uA；
         * 1101：2.125uA；
         * 1110：2.2uA；
         * 1111：2.375uA。
         */
        unsigned int    ibct_tx_int1_1p25u_d06_ch1 : 4   ; /* [19..16] */
        // Comment of field: ibct_tx_int2_1p25u_d06_ch1
        /*
         * tx ch1通路积分器op2偏置电流选择：
         * 00：1uA；
         * 01：1.25uA（默认）；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_tx_int2_1p25u_d06_ch1 : 2   ; /* [21..20] */
        // Comment of field: ibct_tx_int3_1p25u_d06_ch1
        /*
         * tx ch1通路积分器op3偏置电流选择：
         * 00：1uA；
         * 01：1.25uA（默认）；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_tx_int3_1p25u_d06_ch1 : 2   ; /* [23..22] */
        // Comment of field: ibct_tx_int4_1p25u_d06_ch1
        /*
         * tx ch1通路积分器op4偏置电流选择：
         * 00：1uA；
         * 01：1.25uA（默认）；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_tx_int4_1p25u_d06_ch1 : 2   ; /* [25..24] */
        // Comment of field: ibct_tx_cmfb_1u_d06_ch1
        /*
         * tx ch1通路cmfb op偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_tx_cmfb_1u_d06_ch1 : 2   ; /* [27..26] */
        // Comment of field: ibct_tx_cmp_1u_d06_ch1
        /*
         * tx ch1通路比较器偏置电流选择：
         * 0000：0.5uA；
         * 0001：0.625uA；
         * 0010：0.75uA；
         * 0011：0.875uA；
         * 0100：1uA（默认）；
         * 0101：1.125uA；
         * 0110：1.25uA；
         * 0111：1.375；
         * 1000：1.5uA；
         * 1001：1.625uA；
         * 1010：1.75uA；
         * 1011：1.875uA；
         * 1100：2uA；
         * 1101：2.125uA；
         * 1110：2.2uA；
         * 1111：2.375uA。
         */
        unsigned int    ibct_tx_cmp_1u_d06_ch1 : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr19;

/* Define the union u_reg_analog_wr20 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: ibct_tx_int1_1p25u_d06_ch2
        /*
         * tx ch2通路积分器op1偏置电流选择：
         * 0000：0.5uA；
         * 0001：0.625uA；
         * 0010：0.75uA；
         * 0011：0.875uA；
         * 0100：1uA；
         * 0101：1.125uA；
         * 0110：1.25uA（默认）；
         * 0111：1.375；
         * 1000：1.5uA；
         * 1001：1.625uA；
         * 1010：1.75uA；
         * 1011：1.875uA；
         * 1100：2uA；
         * 1101：2.125uA；
         * 1110：2.2uA；
         * 1111：2.375uA。
         */
        unsigned int    ibct_tx_int1_1p25u_d06_ch2 : 4   ; /* [3..0] */
        // Comment of field: ibct_tx_int2_1p25u_d06_ch2
        /*
         * tx ch2通路积分器op2偏置电流选择：
         * 00：1uA；
         * 01：1.25uA（默认）；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_tx_int2_1p25u_d06_ch2 : 2   ; /* [5..4] */
        // Comment of field: ibct_tx_int3_1p25u_d06_ch2
        /*
         * tx ch2通路积分器op3偏置电流选择：
         * 00：1uA；
         * 01：1.25uA（默认）；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_tx_int3_1p25u_d06_ch2 : 2   ; /* [7..6] */
        // Comment of field: ibct_tx_int4_1p25u_d06_ch2
        /*
         * tx ch2通路积分器op4偏置电流选择：
         * 00：1uA；
         * 01：1.25uA（默认）；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_tx_int4_1p25u_d06_ch2 : 2   ; /* [9..8] */
        // Comment of field: ibct_tx_cmfb_1u_d06_ch2
        /*
         * tx ch2通路cmfb op偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_tx_cmfb_1u_d06_ch2 : 2   ; /* [11..10] */
        // Comment of field: ibct_tx_cmp_1u_d06_ch2
        /*
         * tx ch2通路比较器偏置电流选择：
         * 0000：0.5uA；
         * 0001：0.625uA；
         * 0010：0.75uA；
         * 0011：0.875uA；
         * 0100：1uA（默认）；
         * 0101：1.125uA；
         * 0110：1.25uA；
         * 0111：1.375；
         * 1000：1.5uA；
         * 1001：1.625uA；
         * 1010：1.75uA；
         * 1011：1.875uA；
         * 1100：2uA；
         * 1101：2.125uA；
         * 1110：2.2uA；
         * 1111：2.375uA。
         */
        unsigned int    ibct_tx_cmp_1u_d06_ch2 : 4   ; /* [15..12] */
        // Comment of field: ibct_tx_popldo_1p25u_d06_ch1
        /*
         * tx ch1通路电源pop模块op偏置电流选择：
         * 00：1uA；
         * 01：1.25uA（默认）；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_tx_popldo_1p25u_d06_ch1 : 2   ; /* [17..16] */
        // Comment of field: ibct_tx_popvcm_2p5u_d06_ch1
        /*
         * tx ch1通路vcm pop模块op偏置电流选择：
         * 00：1uA；
         * 01：1.25uA（默认）；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_tx_popvcm_2p5u_d06_ch1 : 2   ; /* [19..18] */
        // Comment of field: ibct_tx_popldo_1p25u_d06_ch2
        /*
         * tx ch2通路电源pop模块op偏置电流选择：
         * 00：1uA；
         * 01：1.25uA（默认）；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_tx_popldo_1p25u_d06_ch2 : 2   ; /* [21..20] */
        // Comment of field: ibct_tx_popvcm_2p5u_d06_ch2
        /*
         * tx ch2通路vcm pop模块op偏置电流选择：
         * 00：1uA；
         * 01：1.25uA（默认）；
         * 10：1.5uA；
         * 11：1.75uA。
         */
        unsigned int    ibct_tx_popvcm_2p5u_d06_ch2 : 2   ; /* [23..22] */
        // Comment of field: ibct_vcm09_1u_d06
        /*
         * vcm_09 buffer中op偏置电流选择：
         * 00：0.75uA；
         * 01：1uA（默认）；
         * 10：1.25uA；
         * 11：1.5uA。
         */
        unsigned int    ibct_vcm09_1u_d06     : 2   ; /* [25..24] */
        // Comment of field: ibct_vcm045_2p5u_d06
        /*
         * vcm_045 buffer中op偏置电流选择：
         * 00：2uA；
         * 01：2.5uA（默认）；
         * 10：3uA；
         * 11：3.5uA。
         */
        unsigned int    ibct_vcm045_2p5u_d06  : 2   ; /* [27..26] */
        // Comment of field: ibct_rctune_2p5u_d06
        /*
         * rctune中ibias op偏置电流选择：
         * 00：2uA；
         * 01：2.5uA（默认）；
         * 10：3uA；
         * 11：3.5uA。
         */
        unsigned int    ibct_rctune_2p5u_d06  : 2   ; /* [29..28] */
        // Comment of field: ibct_rctune_5u_d06
        /*
         * rctune中比较器偏置电流选择：
         * 00：4.5uA；
         * 01：5uA（默认）；
         * 10：5.5uA；
         * 11：6uA。
         */
        unsigned int    ibct_rctune_5u_d06    : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr20;

/* Define the union u_reg_analog_wr21 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: rctune_add_d06
        /*
         * tx rctune码字固定增加code。
         */
        unsigned int    rctune_add_d06        : 5   ; /* [4..0] */
        // Comment of field: rctune_add_int2_d06
        /*
         * lpadc rctune码字固定增加code。
         */
        unsigned int    rctune_add_int2_d06   : 5   ; /* [9..5] */
        // Comment of field: rctune_add_pga_d06
        /*
         * rx rctune码字固定增加code。
         */
        unsigned int    rctune_add_pga_d06    : 5   ; /* [14..10] */
        // Comment of field: rctune_st_d06
        /*
         * rctune模块工作使能信号，需先置0再拉高至1。
         */
        unsigned int    rctune_st_d06         : 1   ; /* [15] */
        // Comment of field: adc_int_code_d06
        /*
         * 外部输入tx rctune code。
         */
        unsigned int    adc_int_code_d06      : 5   ; /* [20..16] */
        // Comment of field: adc_int_code_int2_d06
        /*
         * 外部输入lpadc rctune code。
         */
        unsigned int    adc_int_code_int2_d06 : 5   ; /* [25..21] */
        // Comment of field: adc_int_code_pga_d06
        /*
         * 外部输入rx rctune code。
         */
        unsigned int    adc_int_code_pga_d06  : 5   ; /* [30..26] */
        // Comment of field: adc_tune_sel_d06
        /*
         * tx rctune code来源选择：
         * 0：选择外部输入rctune code；
         * 1：选择内部rctune产生code。
         */
        unsigned int    adc_tune_sel_d06      : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr21;

/* Define the union u_reg_analog_wr22 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: adc_tune_sel_int2_d06
        /*
         * lpadc rctune code来源选择：
         * 0：选择外部输入rctune code；
         * 1：选择内部rctune产生code。
         */
        unsigned int    adc_tune_sel_int2_d06 : 1   ; /* [0] */
        // Comment of field: adc_tune_sel_pga_d06
        /*
         * rx rctune code来源选择：
         * 0：选择外部输入rctune code；
         * 1：选择内部rctune产生code。
         */
        unsigned int    adc_tune_sel_pga_d06  : 1   ; /* [1] */
        // Comment of field: vcm0p9_ib_trim_d06
        /*
         * vcm09 buffer输出级电流选择：
         * 0000：默认档位；
         */
        unsigned int    vcm0p9_ib_trim_d06    : 4   ; /* [5..2] */
        // Comment of field: clkgen_tx_idac_sel_d06
        /*
         * tx idac模块采数时钟沿选择：
         * 0：分频后时钟不反向；
         * 1：分频后时钟反向。
         */
        unsigned int    clkgen_tx_idac_sel_d06 : 1   ; /* [6] */
        // Comment of field: clkgen_tx_lpf_sel_d06
        /*
         * tx lpf模块chopper时钟沿选择：
         * 0：分频后时钟不反向；
         * 1：分频后时钟反向。
         */
        unsigned int    clkgen_tx_lpf_sel_d06 : 1   ; /* [7] */
        // Comment of field: iso_a18_d06_reg
        /*
         * Reserved
         */
        unsigned int    iso_a18_d06_reg       : 1   ; /* [8] */
        // Comment of field: iso_a09_d06_reg
        /*
         * Reserved
         */
        unsigned int    iso_a09_d06_reg       : 1   ; /* [9] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 22  ; /* [31..10] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr22;

/* Define the union u_reg_analog_wr23 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: d_pmu_codec_ldo1_rsv
        /*
         * PMU_CODEC_SUB内置 LDO1 预留寄存器
         */
        unsigned int    d_pmu_codec_ldo1_rsv  : 16  ; /* [15..0] */
        // Comment of field: d_pmu_codec_ldo1_vset
        /*
         * PMU_CODEC_SUB内置 LDO1 电压档位配置信号
         * 默认档位： 101100 -- 1.8V
         * CODE=(VOUT-0.7)/0.025;
         * 6bit 档位可调，调压范围：000000-111111 0.7V-2.275V 25mV/step
         */
        unsigned int    d_pmu_codec_ldo1_vset : 8   ; /* [23..16] */
        // Comment of field: d_pmu_codec_ldo1_ictr
        /*
         * PMU_CODEC_SUB内置 LDO1 偏置电流控制信号
         */
        unsigned int    d_pmu_codec_ldo1_ictr : 4   ; /* [27..24] */
        // Comment of field: d_pmu_codec_ldo1_ocp_set
        /*
         * PMU_CODEC_SUB内置 LDO1  OCP检测功能阈值配置信号
         */
        unsigned int    d_pmu_codec_ldo1_ocp_set : 2   ; /* [29..28] */
        // Comment of field: d_pmu_codec_ldo1_ocp_bypass
        /*
         * PMU_CODEC_SUB内置 LDO1  OCP检测功能屏蔽信号：
         * 1：屏蔽OCP 功能；
         * 0：不屏蔽。
         */
        unsigned int    d_pmu_codec_ldo1_ocp_bypass : 1   ; /* [30] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr23;

/* Define the union u_reg_analog_wr24 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: d_pmu_codec_ldo2_rsv
        /*
         * PMU_CODEC_SUB内置 LDO2 预留寄存器
         */
        unsigned int    d_pmu_codec_ldo2_rsv  : 16  ; /* [15..0] */
        // Comment of field: d_pmu_codec_ldo2_vset
        /*
         * PMU_CODEC_SUB内置 LDO2 电压档位配置信号
         * 默认档位： 101100 -- 1.8V
         * CODE=(VOUT-0.7)/0.025;
         * 6bit 档位可调，调压范围：000000-111111 0.7V-2.275V 25mV/step
         */
        unsigned int    d_pmu_codec_ldo2_vset : 8   ; /* [23..16] */
        // Comment of field: d_pmu_codec_ldo2_ictr
        /*
         * PMU_CODEC_SUB内置 LDO2 偏置电流控制信号
         */
        unsigned int    d_pmu_codec_ldo2_ictr : 4   ; /* [27..24] */
        // Comment of field: d_pmu_codec_ldo2_ocp_set
        /*
         * PMU_CODEC_SUB内置 LDO2  OCP检测功能阈值配置信号
         */
        unsigned int    d_pmu_codec_ldo2_ocp_set : 2   ; /* [29..28] */
        // Comment of field: d_pmu_codec_ldo2_ocp_bypass
        /*
         * PMU_CODEC_SUB内置 LDO2  OCP检测功能屏蔽信号。
         * 1：屏蔽OCP 功能；
         * 0：不屏蔽。
         */
        unsigned int    d_pmu_codec_ldo2_ocp_bypass : 1   ; /* [30] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr24;

/* Define the union u_reg_analog_wr25 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: d_pmu_codec_ldo3_rsv
        /*
         * PMU_CODEC_SUB内置 LDO3 预留寄存器
         */
        unsigned int    d_pmu_codec_ldo3_rsv  : 16  ; /* [15..0] */
        // Comment of field: d_pmu_codec_ldo3_vset
        /*
         * PMU_CODEC_SUB内置 LDO3 电压档位配置信号
         * 默认档位： 110111 -- 1.8V
         * CODE=(VOUT-0.425)/0.025;
         * 6bit 档位可调，调压范围：000000-111111 0.425V-2V 25mV/step
         */
        unsigned int    d_pmu_codec_ldo3_vset : 8   ; /* [23..16] */
        // Comment of field: d_pmu_codec_ldo3_ictr
        /*
         * PMU_CODEC_SUB内置 LDO3 偏置电流控制信号
         */
        unsigned int    d_pmu_codec_ldo3_ictr : 4   ; /* [27..24] */
        // Comment of field: d_pmu_codec_ldo3_ocp_set
        /*
         * PMU_CODEC_SUB内置 LDO3  OCP检测功能阈值配置信号
         */
        unsigned int    d_pmu_codec_ldo3_ocp_set : 2   ; /* [29..28] */
        // Comment of field: d_pmu_codec_ldo3_ocp_bypass
        /*
         * PMU_CODEC_SUB内置 LDO3  OCP检测功能屏蔽信号；
         * 1：屏蔽OCP 功能；
         * 0：不屏蔽。
         */
        unsigned int    d_pmu_codec_ldo3_ocp_bypass : 1   ; /* [30] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr25;

/* Define the union u_reg_analog_wr26 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: d_pmu_codec_ldo4_rsv
        /*
         * PMU_CODEC_SUB内置 LDO4 预留寄存器
         */
        unsigned int    d_pmu_codec_ldo4_rsv  : 16  ; /* [15..0] */
        // Comment of field: d_pmu_codec_ldo4_vset
        /*
         * PMU_CODEC_SUB内置 LDO4 电压档位配置信号
         * 默认档位： 010011 -- 0.9V
         * CODE=(VOUT-0.425)/0.025;
         * 6bit 档位可调，调压范围：000000-111111 0.425V-2V 25mV/step
         */
        unsigned int    d_pmu_codec_ldo4_vset : 8   ; /* [23..16] */
        // Comment of field: d_pmu_codec_ldo4_ictr
        /*
         * PMU_CODEC_SUB内置 LDO4 偏置电流控制信号
         */
        unsigned int    d_pmu_codec_ldo4_ictr : 4   ; /* [27..24] */
        // Comment of field: d_pmu_codec_ldo4_ocp_set
        /*
         * PMU_CODEC_SUB内置 LDO4  OCP检测功能阈值配置信号
         */
        unsigned int    d_pmu_codec_ldo4_ocp_set : 2   ; /* [29..28] */
        // Comment of field: d_pmu_codec_ldo4_ocp_bypass
        /*
         * PMU_CODEC_SUB内置 LDO4  OCP检测功能屏蔽信号。
         * 1：屏蔽OCP 功能；
         * 0：不屏蔽。
         */
        unsigned int    d_pmu_codec_ldo4_ocp_bypass : 1   ; /* [30] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr26;

/* Define the union u_reg_analog_wr27 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 16  ; /* [15..0] */
        // Comment of field: d_pmu_codec_test_sel
        /*
         * PMU_CODEC_SUB 测试信号MUX选通信号 for PMU_TEST（未描述档位为Reserved）。
         * 0000：TEST_CODEC输出IBG_10uA测试；
         * 0001：TEST_CODEC输出VREF1P0测试；
         * 0010：TEST_CODEC输出VREF0P7测试；
         * 0011：TEST_CODEC输出LDO4_0P9测试；
         * 0110：TEST_CODEC输出LDO3_1P8测试；
         * 0111：TEST_CODEC输出INTLDO_1P8测试。
         */
        unsigned int    d_pmu_codec_test_sel  : 8   ; /* [23..16] */
        // Comment of field: reserved_1
        /*
         * Reserved
         */
        unsigned int    reserved_1            : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr27;

/* Define the union u_reg_analog_wr28 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: d_pmu_codec_ldo1_trim
        /*
         * PMU_CODEC_SUB内置 LDO1 输出电压精度调节信号
         */
        unsigned int    d_pmu_codec_ldo1_trim : 8   ; /* [7..0] */
        // Comment of field: d_pmu_codec_ldo2_trim
        /*
         * PMU_CODEC_SUB内置 LDO2 输出电压精度调节信号
         */
        unsigned int    d_pmu_codec_ldo2_trim : 8   ; /* [15..8] */
        // Comment of field: d_pmu_codec_ldo3_trim
        /*
         * PMU_CODEC_SUB内置 LDO3 输出电压精度调节信号
         */
        unsigned int    d_pmu_codec_ldo3_trim : 8   ; /* [23..16] */
        // Comment of field: d_pmu_codec_ldo4_trim
        /*
         * PMU_CODEC_SUB内置 LDO4 输出电压精度调节信号
         */
        unsigned int    d_pmu_codec_ldo4_trim : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr28;

/* Define the union u_reg_analog_wr29 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: mad_clk_zcd_sel_d06
        /*
         * lpadc通路zcd模块时钟频率选择：
         * 00：chopper频率为96K；
         * 01：chopper频率为192K；
         * 10：chopper频率为1.536M；
         * 11：chopper频率为3.072M。
         */
        unsigned int    mad_clk_zcd_sel_d06   : 2   ; /* [1..0] */
        // Comment of field: mad_zcd_iadd_d06
        /*
         * lpadc通路zcd模块比较器第一级电流选择：
         * 0：默认电流3uA；
         * 1：电流为6uA。
         */
        unsigned int    mad_zcd_iadd_d06      : 1   ; /* [2] */
        // Comment of field: mad_pga_inr_sel_d06
        /*
         * lpadc通路pga输入电阻选择：
         * 0：输入电阻为100K；
         * 1：输入电阻为200K。
         */
        unsigned int    mad_pga_inr_sel_d06   : 1   ; /* [3] */
        // Comment of field: mad_pga_inr_byp_d06
        /*
         * lpadc通路pga输入电阻byp选择：
         * 0：输入电阻正常选择；
         * 1：输入直通至pga虚地点。
         */
        unsigned int    mad_pga_inr_byp_d06   : 1   ; /* [4] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 27  ; /* [31..5] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr29;

/* Define the union u_reg_analog_wr30 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: rx_iadc_byp_res_setup_a18_ch2
        /*
         * Reserved
         */
        unsigned int    rx_iadc_byp_res_setup_a18_ch2 : 1   ; /* [0] */
        // Comment of field: rx_iadc_sel_res_a18_ch2
        /*
         * 通道1中noisefilter伪电阻电流上的选择
         * 00：电流0.4u
         * 01：电流0.2u（默认）
         * 10：电流0.1u
         * 11：电流0.05u
         */
        unsigned int    rx_iadc_sel_res_a18_ch2 : 2   ; /* [2..1] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 13  ; /* [15..3] */
        // Comment of field: rx_dac_mill_r_sel_a18_ch2
        /*
         * 通道2中保障vbgen模块稳定性的密勒电容选择，搭配reg_analog_wr30<17><19:18>使用
         * 0：输入电流1uA，且IBIAS_SEL为0选择（默认档位）
         * 1：输入电流1uA，且IBIAS_SEL为1选择
         */
        unsigned int    rx_dac_mill_r_sel_a18_ch2 : 1   ; /* [16] */
        // Comment of field: rx_dac_op_ibias_sel_a18_ch2
        /*
         * 通道1中vbgen模块的偏置电流是否加倍，例如输入1u，不加倍电路内部仍使用1u，加倍电路内部实际使用为2u
         * 0：不加倍
         * 1：加倍
         */
        unsigned int    rx_dac_op_ibias_sel_a18_ch2 : 1   ; /* [17] */
        // Comment of field: rx_dac_res_sel_a18_ch2
        /*
         * 通道2中保障vbgen模块稳定性的电阻选择，搭配reg_analog_wr30<17><16>使用
         * 00：输入电流1uA，且IBIAS_SEL为0选择（默认档位）
         * 01：输入电流1uA，且IBIAS_SEL为1选择
         * 10：输入电流1.5uA，且IBIAS_SEL为1选择
         */
        unsigned int    rx_dac_res_sel_a18_ch2 : 2   ; /* [19..18] */
        // Comment of field: reserved_1
        /*
         * Reserved
         */
        unsigned int    reserved_1            : 9   ; /* [28..20] */
        // Comment of field: rx_iadc_sel_res_a18_ch1
        /*
         * 通道1中noisefilter伪电阻电流上的选择
         * 00：电流0.4u
         * 01：电流0.2u（默认）
         * 10：电流0.1u
         * 11：电流0.05u
         */
        unsigned int    rx_iadc_sel_res_a18_ch1 : 2   ; /* [30..29] */
        // Comment of field: rx_iadc_byp_res_setup_a18_ch1
        /*
         * Reserved
         */
        unsigned int    rx_iadc_byp_res_setup_a18_ch1 : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr30;

/* Define the union u_reg_analog_wr33 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: clk_test_en_d06
        /*
         * 时钟测试使能信号：
         * 0：输出测试时钟复位至0；
         * 1：正常输出测试时钟；
         */
        unsigned int    clk_test_en_d06       : 1   ; /* [0] */
        // Comment of field: clk_test_sel_d06
        /*
         * 时钟测试通路输出时钟选择：
         * 00：输出时钟为clk_codec_pulse2；
         * 01：输出时钟为clk_codec_pulse1；
         * 10：输出时钟为clk_codec_pulse0；
         * 11：输出时钟为clk_codec_ana。
         */
        unsigned int    clk_test_sel_d06      : 2   ; /* [2..1] */
        // Comment of field: test_en_ana_d06
        /*
         * 模拟测试通路使能信号：
         * 0：模拟测试通路关闭；
         * 1：模拟测试通路打开；
         */
        unsigned int    test_en_ana_d06       : 1   ; /* [3] */
        // Comment of field: test_en_dig_d06
        /*
         * 时钟测试通路使能信号：
         * 0：时钟测试通路关闭；
         * 1：时钟测试通路打开；
         */
        unsigned int    test_en_dig_d06       : 1   ; /* [4] */
        // Comment of field: test_dig_sel_d06
        /*
         * 时钟测试通路开关阻抗选择：
         * 0：时钟测试通路选择默认开关阻抗；
         * 1：时钟测试通路选择开关阻抗减半；
         */
        unsigned int    test_dig_sel_d06      : 1   ; /* [5] */
        // Comment of field: test_sel_vcm09_d06
        /*
         * 模拟测试通路vcm09测试使能信号：
         * 0：vcm09测试关闭；
         * 1：vcm09测试打开；
         */
        unsigned int    test_sel_vcm09_d06    : 1   ; /* [6] */
        // Comment of field: test_sel_vcm045_d06
        /*
         * 模拟测试通路vcm045测试使能信号：
         * 0：vcm045测试关闭；
         * 1：vcm045测试打开；
         */
        unsigned int    test_sel_vcm045_d06   : 1   ; /* [7] */
        // Comment of field: tx_op1_c_ctrl_d06_ch2
        /*
         * bit<0>：rx ch1通路idac模块noisefilter vbp_op使能信号：
         * 0：noisefilter vbp_op关闭（默认）；
         * 1：noisefilter vbp_op使能；
         * bit<1>：rx ch1通路idac模块noisefilter vbn_op使能信号：
         * 0：noisefilter vbn_op关闭（默认）；
         * 1：noisefilter vbn_op使能；
         */
        unsigned int    tx_op1_c_ctrl_d06_ch2 : 2   ; /* [9..8] */
        // Comment of field: tx_cap_to_vcm_en_d06_ch2
        /*
         * bit<0>：rx ch2通路idac模块noisefilter vbp_op使能信号：
         * 0：noisefilter vbp_op关闭（默认）；
         * 1：noisefilter vbp_op使能；
         * bit<1>：rx ch2通路idac模块noisefilter vbn_op使能信号：
         * 0：noisefilter vbn_op关闭（默认）；
         * 1：noisefilter vbn_op使能；
         */
        unsigned int    tx_cap_to_vcm_en_d06_ch2 : 1   ; /* [11..10] */
        // Comment of field: tx_byp2_nfilt_d06
        /*
         * rx ch1通路idac模块vb复用tx使能信号：
         * 0：rx ch1复用tx电流接收模块关闭（默认）；
         * 1：rx ch1复用tx电流接收模块使能；
         */
        unsigned int    tx_byp2_nfilt_d06     : 1   ; /* [12] */
        // Comment of field: tx_byp_nfilt_d06
        /*
         * rx ch1通路idac模块vb来源选择信号：
         * 0：idac模块vb来源复用tx；
         * 1：idac模块vb来源为rx vb产生（默认）；
         */
        unsigned int    tx_byp_nfilt_d06      : 1   ; /* [13] */
        // Comment of field: tx_en_nfiltn_pseudores_d06
        /*
         * tx_vb中复用至rx ch1 ibias源头使能信号：
         * 0：tx_vb复用至rx ch1 ibias源头关闭；
         * 1：tx_vb复用至rx ch1 ibias源头使能；
         */
        unsigned int    tx_en_nfiltn_pseudores_d06 : 1   ; /* [14] */
        // Comment of field: tx_en_nfiltn_op1_d06
        /*
         * rx ch2通路idac模块vb来源选择信号：
         * 0：idac模块vb来源复用tx；
         * 1：idac模块vb来源为rx vb产生（默认）；
         */
        unsigned int    tx_en_nfiltn_op1_d06  : 1   ; /* [15] */
        // Comment of field: tx_en_nfiltn_op2_d06
        /*
         * Reserved
         */
        unsigned int    tx_en_nfiltn_op2_d06  : 1   ; /* [16] */
        // Comment of field: tx_en_nfiltp_pseudores_d06
        /*
         * tx_vb中复用至rx ch2 ibias源头使能信号：
         * 0：tx_vb复用至rx ch2 ibias源头关闭；
         * 1：tx_vb复用至rx ch2 ibias源头使能；
         */
        unsigned int    tx_en_nfiltp_pseudores_d06 : 1   ; /* [17] */
        // Comment of field: tx_en_nfiltp_op1_d06
        /*
         * Reserved
         */
        unsigned int    tx_en_nfiltp_op1_d06  : 1   ; /* [18] */
        // Comment of field: tx_en_nfiltp_op2_d06
        /*
         * rx ch2通路idac模块vb复用tx使能信号：
         * 0：rx ch2复用tx电流接收模块关闭（默认）；
         * 1：rx ch2复用tx电流接收模块使能；
         */
        unsigned int    tx_en_nfiltp_op2_d06  : 1   ; /* [19] */
        // Comment of field: tx_pulse_adj_byp_d06_ch2
        /*
         * rx ch1通路idac模块vb使能信号：
         * 0：rx ch1通路idac模块关闭；
         * 1：rx ch1通路idac模块使能；
         */
        unsigned int    tx_pulse_adj_byp_d06_ch2 : 1   ; /* [20] */
        // Comment of field: rx_en_dac_vbgen_d06_ch2
        /*
         * rx ch2通路idac模块vb使能信号：
         * 0：rx ch2通路idac模块关闭；
         * 1：rx ch2通路idac模块使能；
         */
        unsigned int    rx_en_dac_vbgen_d06_ch2 : 1   ; /* [21] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 10  ; /* [31..22] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr33;

/* Define the union u_reg_analog_wr34 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: temp_mode_sel
        /*
         * TX 积分器1相位随温度调节模式选择
         * 1：软件直接配置
         * 0：小数字配置
         */
        unsigned int    temp_mode_sel         : 1   ; /* [0] */
        // Comment of field: c_ctrl_reg
        /*
         * TX 积分器1相位调节软件配置模式下控制输入
         * 1：补偿相位
         * 0：不补偿
         */
        unsigned int    c_ctrl_reg            : 1   ; /* [1] */
        // Comment of field: tsensor_temp_sel
        /*
         * TX 积分器1相位调节温度
         * 1：70度以下补偿相位
         * 0:0度以下补偿相位
         */
        unsigned int    tsensor_temp_sel      : 1   ; /* [2] */
        // Comment of field: rstn_ch1
        /*
         * TX 通道1顶层数据打拍解复位信号
         * 1：解复位
         * 0：复位
         */
        unsigned int    rstn_ch1              : 1   ; /* [3] */
        // Comment of field: rstn_ch2
        /*
         * TX 通道2顶层数据打拍解复位信号
         * 1：解复位
         * 0：复位
         */
        unsigned int    rstn_ch2              : 1   ; /* [4] */
        // Comment of field: depop_en_ch1_d06
        /*
         * TX 通道1 DEPOP 功能使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    depop_en_ch1_d06      : 1   ; /* [5] */
        // Comment of field: depop_en_ch2_d06
        /*
         * TX 通道2 DEPOP 功能使能信号
         * 1：使能
         * 0：不使能
         */
        unsigned int    depop_en_ch2_d06      : 1   ; /* [6] */
        // Comment of field: depop_pulse_adj_byp_d06
        /*
         * TX depop功能数据脉宽调节使能
         * 0：使能
         * 1：不使能
         */
        unsigned int    depop_pulse_adj_byp_d06 : 1   ; /* [7] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 8   ; /* [15..8] */
        // Comment of field: d_pmu_codec_rsv
        /*
         * PMU_CODEC_SUB 预留寄存器 unsigned int    d_pmu_codec_rsv       : 16  ; [31..16]
         * d_pmu_codec_rsv<0> for codecldo2 基准电压切换，1'b0-0.9V，1'b1-0.4V;
         * d_pmu_codec_rsv<3>for intldo en_sw;
         * d_pmu_codec_rsv<6:4> for codec_ref 滤波电阻大小选择，
         *                      默认3'b000 为最小阻值-0Ω，不接；3'b111为最大阻值-784kΩ，线性调节，step：112kΩ
         * d_pmu_codec_rsv<7> for codec_ref 输出方式切换，1'b0-buffer 输出，1'b1-直通输出；
         * d_pmu_codec_rsv<15:8> for intldo vset；默认档位： 00000 -- 1.8V
         */
        unsigned int    ldo2_base_vol_sel     : 1   ; /* [16] */
        unsigned int    reserved_1            : 2   ; /* [18..17] */
        unsigned int    int_ldo_en_sw         : 1   ; /* [19] */
        unsigned int    codec_ref_res_sel     : 3   ; /* [22..20] */
        unsigned int    codec_ref_out_mode    : 1   ; /* [23] */
        unsigned int    int_ldo_vset          : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_wr34;

/* Define the union u_reg_analog_rd00 */
typedef union {
    /* Define the struct bits */
    struct {
        // Comment of field: a2d_rctune_done_d06
        /*
         * rctune模块校正完成标志信号：
         * 0：rctune模块校正未完成/异常；
         * 1：rctune模块正常完成校正。
         */
        unsigned int    a2d_rctune_done_d06   : 1   ; /* [0] */
        // Comment of field: a2d_rctune_tx_out_d06
        /*
         * rctune模块tx部分校正输出码字。
         */
        unsigned int    a2d_rctune_tx_out_d06 : 5   ; /* [5..1] */
        // Comment of field: a2d_rctune_rx_out_d06
        /*
         * rctune模块rx部分校正输出码字。
         */
        unsigned int    a2d_rctune_rx_out_d06 : 5   ; /* [10..6] */
        // Comment of field: a2d_rctune_mad_out_d06
        /*
         * rctune模块lpadc部分校正输出码字。
         */
        unsigned int    a2d_rctune_mad_out_d06 : 5   ; /* [15..11] */
        // Comment of field: mad_zcd_done_d06
        /*
         * lpadc通路过零检测完成标志信号：
         * 0：lpadc通路过零检测未完成/异常；
         * 1：lpadc通路过零检测正常完成校正。
         */
        unsigned int    mad_zcd_done_d06      : 1   ; /* [16] */
        // Comment of field: rx_zcd_done_d06_ch1
        /*
         * rx ch1通路过零检测完成标志信号：
         * 0：lpadc通路过零检测未完成/异常；
         * 1：lpadc通路过零检测正常完成校正。
         */
        unsigned int    rx_zcd_done_d06_ch1   : 1   ; /* [17] */
        // Comment of field: rx_zcd_done_d06_ch2
        /*
         * rx ch2通路过零检测完成标志信号：
         * 0：lpadc通路过零检测未完成/异常；
         * 1：lpadc通路过零检测正常完成校正。
         */
        unsigned int    rx_zcd_done_d06_ch2   : 1   ; /* [18] */
        // Comment of field: reserved_0
        /*
         * Reserved
         */
        unsigned int    reserved_0            : 13  ; /* [31..19] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_reg_analog_rd00;

//==============================================================================
/* Define the classd struct */
typedef struct {
    volatile u_classsd_dig0_cfg0    classsd_dig0_cfg0;
    volatile u_classsd_dig0_cfg1    classsd_dig0_cfg1;
    volatile u_classsd_dig0_cfg2    classsd_dig0_cfg2;
    volatile u_classsd_dig0_cfg3    classsd_dig0_cfg3;
    volatile u_classsd_dig0_cfg4    classsd_dig0_cfg4;
    volatile u_classsd_dig0_cfg5    classsd_dig0_cfg5;
    volatile u_classsd_dig0_cfg6    classsd_dig0_cfg6;
    volatile u_classsd_dig0_cfg7    classsd_dig0_cfg7;
    volatile u_classsd_dig0_st      classsd_dig0_st;
} s_classd_dig_reg_stru;

/* Define the rx dre struct */
typedef struct {
    volatile u_rxdre_dig0_cfg0      rxdre_dig0_cfg0;
    volatile u_rxdre_dig0_cfg1      rxdre_dig0_cfg1;
} s_rx_dre_dig_reg_stru;

/* Define the global struct */
typedef struct {
    volatile unsigned int           classsd_dig_id                   ; /* 0x0 */
    volatile s_classd_dig_reg_stru  classd_dig_list[2]               ; /* 0x4~0x48 */
    volatile u_temp_code            temp_code                        ; /* 0x4c */
    volatile s_rx_dre_dig_reg_stru  rx_dre_dig_list[2]               ; /* 0x50~0x5c */
    volatile u_reg_analog_wr00      reg_analog_wr00                  ; /* 0x60 */
    volatile u_reg_analog_wr01      reg_analog_wr01                  ; /* 0x64 */
    volatile u_reg_analog_wr02      reg_analog_wr02                  ; /* 0x68 */
    volatile u_reg_analog_wr03      reg_analog_wr03                  ; /* 0x6c */
    volatile u_reg_analog_wr04      reg_analog_wr04                  ; /* 0x70 */
    volatile u_reg_analog_wr05      reg_analog_wr05                  ; /* 0x74 */
    volatile u_reg_analog_wr06      reg_analog_wr06                  ; /* 0x78 */
    volatile u_reg_analog_wr07      reg_analog_wr07                  ; /* 0x7c */
    volatile u_reg_analog_wr08      reg_analog_wr08                  ; /* 0x80 */
    volatile u_reg_analog_wr09      reg_analog_wr09                  ; /* 0x84 */
    volatile u_reg_analog_wr10      reg_analog_wr10                  ; /* 0x88 */
    volatile u_reg_analog_wr11      reg_analog_wr11                  ; /* 0x8c */
    volatile u_reg_analog_wr12      reg_analog_wr12                  ; /* 0x90 */
    volatile u_reg_analog_wr13      reg_analog_wr13                  ; /* 0x94 */
    volatile u_reg_analog_wr14      reg_analog_wr14                  ; /* 0x98 */
    volatile u_reg_analog_wr15      reg_analog_wr15                  ; /* 0x9c */
    volatile u_reg_analog_wr16      reg_analog_wr16                  ; /* 0xa0 */
    volatile u_reg_analog_wr17      reg_analog_wr17                  ; /* 0xa4 */
    volatile u_reg_analog_wr18      reg_analog_wr18                  ; /* 0xa8 */
    volatile u_reg_analog_wr19      reg_analog_wr19                  ; /* 0xac */
    volatile u_reg_analog_wr20      reg_analog_wr20                  ; /* 0xb0 */
    volatile u_reg_analog_wr21      reg_analog_wr21                  ; /* 0xb4 */
    volatile u_reg_analog_wr22      reg_analog_wr22                  ; /* 0xb8 */
    volatile u_reg_analog_wr23      reg_analog_wr23                  ; /* 0xbc */
    volatile u_reg_analog_wr24      reg_analog_wr24                  ; /* 0xc0 */
    volatile u_reg_analog_wr25      reg_analog_wr25                  ; /* 0xc4 */
    volatile u_reg_analog_wr26      reg_analog_wr26                  ; /* 0xc8 */
    volatile u_reg_analog_wr27      reg_analog_wr27                  ; /* 0xcc */
    volatile u_reg_analog_wr28      reg_analog_wr28                  ; /* 0xd0 */
    volatile u_reg_analog_wr29      reg_analog_wr29                  ; /* 0xd4 */
    volatile u_reg_analog_wr30      reg_analog_wr30                  ; /* 0xd8 */
    volatile unsigned int           reg_analog_wr31                  ; /* 0xdc */
    volatile unsigned int           reg_analog_wr32                  ; /* 0xe0 */
    volatile u_reg_analog_wr33      reg_analog_wr33                  ; /* 0xe4 */
    volatile u_reg_analog_wr34      reg_analog_wr34                  ; /* 0xe8 */
    volatile unsigned int           reserved_1[13]                   ; /* 0xec~0x11c */
    volatile u_reg_analog_rd00      reg_analog_rd00                  ; /* 0x120 */
    volatile unsigned int           reg_analog_rd01                  ; /* 0x124 */
    volatile unsigned int           reg_analog_rd02                  ; /* 0x128 */
} s_afe_ana_reg_stru;

#endif /* __AFE_ANA_REG_H__ */

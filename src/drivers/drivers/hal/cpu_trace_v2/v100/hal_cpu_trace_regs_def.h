/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Provides cpu trace register
 */

#ifndef HAL_CPU_TRACE_REGS_DEF_H
#define HAL_CPU_TRACE_REGS_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_cpu_trace_regs_def CPU TRACE Regs Definition
 * @ingroup  drivers_hal_cpu_trace
 * @{
 */

/**
 * @brief  This union represents the bit fields in the cfg_cpu_trace_save_sel register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_cpu_trace_save_sel {
    uint32_t d32;                           /*!< Raw register data. */
    struct {
        uint32_t locate_sel : 1;              /*!< TRACE存储位置选择
                                                 1'b0：存储在soc指定的memroy空间
                                                 1'b1：存储数据送到AXI总线上,可存储到L2RAM中 */
    } b;                                    /*!< Register bits. */
} cfg_cpu_trace_save_sel_t;

/**
 * @brief  This union represents the bit fields in the cpu_trace_enable register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 */
typedef union cpu_trace_enable_data {
    uint32_t d32;                           /*!< Raw register data. */
    struct {
        uint32_t diag_enable : 1;           /*!< cpu trace diag使能寄存器
                                                 1'b0：cpu trace关闭
                                                 1'b1：cpu trace打开 */
    } b;                                    /*!< Register bits. */
} cpu_trace_enable_data_t;

/**
 * @brief  This union represents the bit fields in the cfg_diag_sample_mode_data register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_diag_sample_mode_data {
    uint32_t d32;                           /*!< Raw register data. */
    struct {
        uint32_t diag_sample_mode : 2;      /*!< 当cfg_monitor_sel=1’b0时,用于配置CPU TRACE
                                                存储方式：
                                                2’b00：存满停止;
                                                2’b01：循环存储;
                                                当cfg_monitor_sel=1’b1时,用于配置数据采集的模式：
                                                2’b00：片内固定长度采集
                                                2’b01：片内条件触发采集 */
        uint32_t reserved : 6;                       /*!< reserved */
        uint32_t diag_sample_en : 1;        /*!< 数据采集使能信号,电平拉高后开始采集数据 */
        uint32_t diag_sample_sync : 1;      /*!< 数采配置同步信号,在配置其他cfg信号前置0,配置完成后置1; */
    } b;                                    /*!< Register bits. */
} cfg_diag_sample_mode_data_t;

/**
 * @brief  This union represents the bit fields in the cfg_diag_sample_mode_data register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_cpu_trace_locate_data {
    uint32_t d32;                           /*!< Raw register data. */
    struct {
        uint32_t diag_locate : 16;            /*!< 地址的低16位有效. */
    } b;                                    /*!< Register bits. */
} cfg_cpu_trace_locate_data_t;

/**
 * @brief  Registers associated with cpu trace.
 */
typedef struct cpu_trace_contrl_regs {
    volatile uint32_t cfg_cpu_trace_save_sel;  /*!< TRACE存储位置配置.<i>Offset: 00h</i>. */
    volatile uint32_t cpu_trace_enable_data;   /*!< CPU TRACE功能配置.<i>Offset: 04h</i>. */
    volatile uint32_t diag_monitor_clock;      /*!< 数采时钟配置.<i>Offset: 08h</i>. */
} cpu_trace_control_regs_t;

typedef struct cpu_trace_sample_regs {
    volatile uint32_t cfg_diag_sample_mode_data;    /*!< 数据采集模式配置.<i>Offset: 0h</i>. */
    volatile uint32_t diag_sample_len_l;            /*!< 数据采集长度配置低16位.<i>Offset: 04h</i>. */
    volatile uint32_t diag_sample_len_h;            /*!< 数据采集长度配置高16位.<i>Offset: 08h</i>. */
    volatile uint32_t diag_sample_start_l;          /*!< 数据采集起始地址配置低16位.<i>Offset: 0Ch</i>. */
    volatile uint32_t diag_sample_start_h;          /*!< 数据采集起始地址配置高16位.<i>Offset: 10h</i>. */
    volatile uint32_t diag_sample_end_l;            /*!< 数据采集结束地址配置低16位.<i>Offset: 14h</i>. */
    volatile uint32_t diag_sample_end_h;            /*!< 数据采集结束地址配置高16位.<i>Offset: 18h</i>. */
    volatile uint32_t diag_sample_done_l;           /*!< 数采完成上报地址低16位.<i>Offset: 1Ch</i>. */
    volatile uint32_t diag_sample_done_h;           /*!< 数采完成上报地址高16位.<i>Offset: 20h</i>. */
    volatile uint32_t diag_sample_done_flag;        /*!< 数据完成指示.<i>Offset: 24h</i>. */
} cpu_trace_sample_regs_t;

/**
 * @brief  This union represents the bit fields in the pc_lr_load_data register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 */
typedef union pc_lr_load_data {
    uint32_t d32;                           /*!< Raw register data. */
    struct {
        uint32_t load : 1;                  /*!< 手动触发一次锁存pc/lr/sp
                                                  1'b0:不锁存
                                                  1'b1:锁存
                                                  此处需要先写1,读到ok状态为非0,再写0 */
        uint32_t load_clr : 1;              /*!< 手动触发清除锁存状态
                                                  1'b0:不清楚
                                                  1'b1:清除
                                                  此处需要先写1,读到ok状态为0,再写0 */
        uint32_t pclr_mode_sel : 1;         /*!< 锁定模式选择
                                                 0:只锁定第一次触发的pc/lr,希望软件感知到中断后,清除pc/lr,
                                                   再次锁定下一次;
                                                 1:锁定最后一次触发的pc/lr,但是如果MSUB被复位,锁定的pc/lr会被清除;
                                                   如果两次触发的间隔时间很近,会采样到错误的pc/lr;如果使用,建议
                                                   bcpu_pclr_en一次检测只使能1个bit有效(3'b001/5'b010等),
                                                   避免两次触发间隔时间很近时的问题; */
        uint32_t reserved : 4;
        uint32_t load_ok : 3;               /*!< 锁存成功指示信号
                                                 对应bit为1：触发成功
                                                 对应bit为0：未触发
                                                 bit 2: PC/LR/SP直通成功
                                                 bit 1: 手动触发成功
                                                 bit 0: cwdt_int触发成功 */
    } b;                                    /*!< Register bits. */
} pc_lr_load_data_t;

/**
 * @brief  This union represents the bit fields in the pc_lr_enable_data register.
 *         Read the register into the <i>d32</i> member then set/clear the bits using the <i>b</i> elements.
 */
typedef union pc_lr_enable_data {
    uint32_t d32;                           /*!< Raw register data. */
    struct {
        uint32_t pclr_en : 5;               /*!< 锁存使能信号
                                                 对应bit为1：使能
                                                 对应bit为0：未使能
                                                 bit 2: PC/LR/SP直通使能
                                                 bit 1: 手动触发使能
                                                 bit 0: cwdt_int触发使能 */
    } b;                                    /*!< Register bits. */
} pc_lr_enable_t;

/**
 * @brief  This union represents the bit fields in the pc_l_data register.
 *         Read the register into the <i>d32</i> member then get the bits using the <i>b</i> elements.
 */
typedef union pc_l_data {
    uint32_t d32;                           /*!< Raw register data. */
    struct {
        uint32_t addr_l : 16;               /*!< 锁存pc的低16位地址 */
    } b;                                    /*!< Register bits. */
} pc_l_data_t;

/**
 * @brief  This union represents the bit fields in the pc_h_data register.
 *         Read the register into the <i>d32</i> member then get the bits using the <i>b</i> elements.
 */
typedef union pc_h_data {
    uint32_t d32;                           /*!< Raw register data. */
    struct {
        uint32_t addr_h : 16;               /*!< 锁存pc的高16位地址 */
    } b;                                    /*!< Register bits. */
} pc_h_data_t;

/**
 * @brief  This union represents the bit fields in the lr_l_data register.
 *         Read the register into the <i>d32</i> member then get the bits using the <i>b</i> elements.
 */
typedef union lr_l_data {
    uint32_t d32;                           /*!< Raw register data. */
    struct {
        uint32_t addr_l : 16;               /*!< 锁存lr的低16位地址 */
    } b;                                    /*!< Register bits. */
} lr_l_data_t;

/**
 * @brief  This union represents the bit fields in the lr_h_data register.
 *         Read the register into the <i>d32</i> member then get the bits using the <i>b</i> elements.
 */
typedef union lr_h_data {
    uint32_t d32;                           /*!< Raw register data. */
    struct {
        uint32_t addr_h : 16;               /*!< 锁存lr的高16位地址 */
    } b;                                    /*!< Register bits. */
} lr_h_data_t;

/**
 * @brief  This union represents the bit fields in the sp_l_data register.
 *         Read the register into the <i>d32</i> member then get the bits using the <i>b</i> elements.
 */
typedef union sp_l_data {
    uint32_t d32;                           /*!< Raw register data. */
    struct {
        uint32_t addr_l : 16;               /*!< 锁存sp的低16位地址 */
    } b;                                    /*!< Register bits. */
} sp_l_data_t;

/**
 * @brief  This union represents the bit fields in the sp_h_data register.
 *         Read the register into the <i>d32</i> member then get the bits using the <i>b</i> elements.
 */
typedef union sp_h_data {
    uint32_t d32;                           /*!< Raw register data. */
    struct {
        uint32_t addr_h : 16;               /*!< 锁存sp的高16位地址 */
    } b;                                    /*!< Register bits. */
} sp_h_data_t;

/**
 * @brief  Registers associated with pclr.
 */
#ifdef CPU_LOCK_32_WIDTH_SUPPORT
typedef struct pclr_regs {
    volatile uint32_t pclr_load;    /*!< 加载控制配置. */
    volatile uint32_t pclr_enable;  /*!< 使能配置. */
    volatile uint32_t pc;           /*!< 获取pc地址. */
    volatile uint32_t reserved_1;
    volatile uint32_t lr;           /*!< 获取lr地址. */
    volatile uint32_t reserved_2;
    volatile uint32_t sp;           /*!< 获取sp地址. */
    volatile uint32_t reserved_3;
} pclr_regs_t;

#else
typedef struct pclr_regs {
    volatile uint32_t pclr_load;    /*!< 加载控制配置. */
    volatile uint32_t pclr_enable;  /*!< 使能配置. */
    volatile uint32_t pc_l;         /*!< 获取pc低16位地址. */
    volatile uint32_t pc_h;         /*!< 获取pc高16位地址. */
    volatile uint32_t lr_l;         /*!< 获取lr低16位地址. */
    volatile uint32_t lr_h;         /*!< 获取lr高16位地址. */
    volatile uint32_t sp_l;         /*!< 获取sp低16位地址. */
    volatile uint32_t sp_h;         /*!< 获取sp高16位地址. */
} pclr_regs_t;
#endif  /* #ifdef CPU_LOCK_32_WIDTH_SUPPORT */
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

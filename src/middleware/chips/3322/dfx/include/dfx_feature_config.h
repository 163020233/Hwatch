/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description: dfx feature config file
 * This file should be changed only infrequently and with great care.
 */

#ifndef DFX_FEATURE_CONFIG_H
#define DFX_FEATURE_CONFIG_H

#include "chip_core_definition.h"

#define DFX_YES     1
#define DFX_NO      0

/* 特性:是否支持文件传输 */
#ifndef CONFIG_DFX_SUPPORT_TRANSMIT_FILE
#if (CORE == APPS)
#define CONFIG_DFX_SUPPORT_TRANSMIT_FILE            DFX_YES
#else
#define CONFIG_DFX_SUPPORT_TRANSMIT_FILE            DFX_NO
#endif
#endif

/* 特性:是否支持文件传输回调 */
#ifndef CONFIG_DFX_SUPPORT_TRANSMIT_FILE_HOOK
#define CONFIG_DFX_SUPPORT_TRANSMIT_FILE_HOOK       DFX_YES
#endif

/* 特性:是否支持断点续传 */
#ifndef CONFIG_DFX_SUPPORT_CONTINUOUSLY_TRANSMIT
#define CONFIG_DFX_SUPPORT_CONTINUOUSLY_TRANSMIT    DFX_NO
#endif

/* 特性:是否支持DIAG */
#ifndef CONFIG_DFX_SUPPORT_DIAG
#define CONFIG_DFX_SUPPORT_DIAG                     DFX_YES
#endif

/* 特性:是否支持DIAG接收数据包 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_RX_PKT
#define CONFIG_DFX_SUPPORT_DIAG_RX_PKT              DFX_YES
#endif

/* 特性:是否支持DIAG分帧和组包 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_MULTI_FRAME
#define CONFIG_DFX_SUPPORT_DIAG_MULTI_FRAME         DFX_YES
#endif

/* 特性:是否支持串口连接UART */
#ifndef CONFIG_DFX_SUPPORT_DIAG_UART_CHANNEL
#define CONFIG_DFX_SUPPORT_DIAG_UART_CHANNEL        DFX_YES
#endif

/* 特性:DIAG作为上位机处理IND */
#ifndef CONFIG_DFX_SUPPORT_DIAG_UP_MACHINE
#define CONFIG_DFX_SUPPORT_DIAG_UP_MACHINE          DFX_NO
#endif

/* 特性:DIAG连接支持心跳机制 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_BEAT_HEART
#define CONFIG_DFX_SUPPORT_DIAG_BEAT_HEART          DFX_YES
#endif

/* 特性: 支持密码连接 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_CONNECT_PASSWORD
#define CONFIG_DFX_SUPPORT_DIAG_CONNECT_PASSWORD    DFX_NO
#endif

/* 特性: DIAG支持的日志类型 */
/* 精简型日志 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_MINIMAL_MSG
#define CONFIG_DFX_SUPPORT_DIAG_MINIMAL_MSG         DFX_NO
#endif
/* 标准型日志 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_NORMAL_MSG
#define CONFIG_DFX_SUPPORT_DIAG_NORMAL_MSG          DFX_NO
#endif
/* 扩展型日志(毫秒精度时间戳) */
#ifndef CONFIG_DFX_SUPPORT_DIAG_EXTEND_MSG
#define CONFIG_DFX_SUPPORT_DIAG_EXTEND_MSG          DFX_YES
#endif
/* 完整型日志 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_FULL_MSG
#define CONFIG_DFX_SUPPORT_DIAG_FULL_MSG            DFX_NO
#endif

#if ((CONFIG_DFX_SUPPORT_DIAG_MINIMAL_MSG + CONFIG_DFX_SUPPORT_DIAG_NORMAL_MSG + \
    CONFIG_DFX_SUPPORT_DIAG_EXTEND_MSG + CONFIG_DFX_SUPPORT_DIAG_FULL_MSG) != 1)
#error "SELLECT DIAG LOG TYPE ERROR"
#endif

/* 特性:支持虚拟AT */
#ifndef CONFIG_DFX_SUPPORT_DIAG_VIRTUAL_AT
#define CONFIG_DFX_SUPPORT_DIAG_VIRTUAL_AT          DFX_NO
#endif

/* 特性:支持虚拟SHELL */
#ifndef CONFIG_DFX_SUPPORT_DIAG_VRTTUAL_SHELL
#define CONFIG_DFX_SUPPORT_DIAG_VRTTUAL_SHELL       DFX_YES
#endif

/* 特性: DIAG适配SOC LOG */
#ifndef CONFIG_DFX_SUPPORT_DIAG_ADAPT_SOC_LOG
#define CONFIG_DFX_SUPPORT_DIAG_ADAPT_SOC_LOG       DFX_YES
#endif

/* 特性: 支持SOC LOG */
#ifndef CONFIG_DFX_SUPPORT_SOC_LOG
#define CONFIG_DFX_SUPPORT_SOC_LOG                  DFX_NO
#endif

/* 特性: 系统状态监控 */
#ifndef CONFIG_DFX_SUPPORT_SYS_MONITOR
#define CONFIG_DFX_SUPPORT_SYS_MONITOR              DFX_NO
#endif

/* 特性: 延时重启 */
#ifndef CONFIG_DFX_SUPPORT_DELAY_REBOOT
#define CONFIG_DFX_SUPPORT_DELAY_REBOOT             DFX_NO
#endif

/* 特性: DIAG支持USB */
#ifndef CONFIG_DFX_SUPPORT_DIAG_CONNECT_USB
#define CONFIG_DFX_SUPPORT_DIAG_CONNECT_USB         DFX_NO
#endif

/* 特性: DIAG支持蓝牙连接 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_CONNECT_BLE
#define CONFIG_DFX_SUPPORT_DIAG_CONNECT_BLE         DFX_YES
#endif

/* 特性: DIAG支持SLE连接 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_CONNECT_SLE
#define CONFIG_DFX_SUPPORT_DIAG_CONNECT_SLE         DFX_NO
#endif

/* 特性: 支持文件系统（全局配置，决定是否支持open/read/write/fopen/fread/fwrite等函数） */
#ifndef CONFIG_DFX_SUPPORT_FILE_SYSTEM
#if (CORE == APPS)
#define CONFIG_DFX_SUPPORT_FILE_SYSTEM              DFX_YES
#else
#define CONFIG_DFX_SUPPORT_FILE_SYSTEM              DFX_NO
#endif
#endif

/*
 * 特性: 配置log_file是否存储至文件系统
 *（DFX_YES -- 文件系统; DFX_NO -- 裸Flash）
 */
#ifndef CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM
#define CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM      DFX_YES
#endif

/*
 * 特性: 配置异常信息是否存储至文件系统
 *（DFX_YES -- 文件系统; DFX_NO -- 裸Flash）
 */
#ifndef CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM
#define CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM     DFX_YES
#endif

/*
 * 特性: 支持日志离线存储到本地
 * 存储至文件系统还是裸Flash，由CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM配置
 */
#ifndef CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE
#if (CORE == APPS)
#define CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE         DFX_YES
#else
#define CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE         DFX_NO
#endif
#endif

/* 特性: 支持大数据打点 */
#ifndef CONFIG_DFX_SUPPORT_MASSDATA_DOTTING
#define CONFIG_DFX_SUPPORT_MASSDATA_DOTTING         DFX_YES
#endif

/*
 * 特性: 支持log_file使用多个文件：DFX_YES -- 使用多文件；DFX_NO -- 使用单个文件
 * 说明: CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM为DFX_NO时，该特性不生效
 */
#ifndef CONFIG_DFX_SUPPORT_MULTI_LOG_FILE
#define CONFIG_DFX_SUPPORT_MULTI_LOG_FILE           DFX_YES
#endif

/*
 * 特性: 支持精简模式的离线日志格式（记录头较短）
 */
#ifndef CONFIG_DFX_SUPPORT_SIMPLIFIED_LOG_FILE
#define CONFIG_DFX_SUPPORT_SIMPLIFIED_LOG_FILE      DFX_YES
#endif

/*
 * 特性: 支持大数据打点保存到本地
 * 存储至文件系统还是裸Flash，由CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM配置
 */
#ifndef CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE
#define CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE       DFX_YES
#endif

/*
 * 特性: 支持字符串日志保存到本地
 * 存储至文件系统还是裸Flash，由CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM配置
 */
#ifndef CONFIG_DFX_SUPPORT_STRING_LOG_SAVE_FILE
#define CONFIG_DFX_SUPPORT_STRING_LOG_SAVE_FILE     DFX_YES
#endif

/* 异常死机支持按照index保存多份日志 */
#ifndef CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS
#define CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS  DFX_YES
#endif

#if ((CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS == DFX_YES) && CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM == DFX_NO)
#error "CAN NOT SUPPORT MULTIPLE CRASH RECOREDS ON NON FILE SYSTEM"
#endif

#if (CONFIG_DFX_SUPPORT_MASSDATA_DOTTING == DFX_NO && CONFIG_DFX_SUPPORT_MASSDATA_SAVE_FILE == DFX_YES)
#error "Do not enable saving when massdata is disabled!"
#endif
/*
 * 特性: 支持日志限流
 */
#ifndef CONFIG_DFX_SUPPORT_LOG_LIMIT
#define CONFIG_DFX_SUPPORT_LOG_LIMIT                DFX_NO
#endif

/* DFX模块内存统计 */
#ifndef CONFIG_DFX_MEMORY_DEBUG
#define CONFIG_DFX_MEMORY_DEBUG                     DFX_YES
#endif

/* DFX模块HSO内存操作 */
#ifndef CONFIG_DFX_MEMORY_OPERATE
#define CONFIG_DFX_MEMORY_OPERATE                   DFX_NO
#endif

/* DFX模块SDT内存操作 */
#ifndef CONFIG_DFX_SDT_MEMORY_OPERATE
#define CONFIG_DFX_SDT_MEMORY_OPERATE               DFX_YES
#endif

/* DFX模块SDT dump异常弹窗 */
#ifndef CONFIG_DFX_SDT_DUMP_EXCEPTION
#define CONFIG_DFX_SDT_DUMP_EXCEPTION               DFX_YES
#endif

/* DFX模块BT采样操作 */
#ifndef CONFIG_DFX_BT_SAMPLE_DATA
#define CONFIG_DFX_BT_SAMPLE_DATA                   DFX_YES
#endif

/* DFX模块统计信息 */
#ifndef CONFIG_DFX_STATS
#define CONFIG_DFX_STATS                            DFX_YES
#endif

/* 特性: DIAG维测数据主动上报云端 */
/* 主动上报开关 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_REPORT
#define CONFIG_DFX_SUPPORT_DIAG_REPORT              DFX_YES
#endif

/* 日志主动上报 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK
#define CONFIG_DFX_SUPPORT_DIAG_LOG_HOOK            DFX_YES
#endif

/* Metrics主动上报 */
#ifndef CONFIG_DFX_SUPPORT_METRICS_REPORT
#define CONFIG_DFX_SUPPORT_METRICS_REPORT           DFX_YES
#endif

/* 传输 metrics主动上报（蓝牙、wifi等） */
#ifndef CONFIG_DFX_SUPPORT_CONNECTIVITY_METRICS_REPORT
#define CONFIG_DFX_SUPPORT_CONNECTIVITY_METRICS_REPORT      DFX_YES
#endif

/* OS metrics主动上报 */
#ifndef CONFIG_DFX_SUPPORT_OS_METRICS_REPORT
#define CONFIG_DFX_SUPPORT_OS_METRICS_REPORT        DFX_YES
#endif

/* 传感器 metrics主动上报 */
#ifndef CONFIG_DFX_SUPPORT_SENSOR_METRICS_REPORT
#define CONFIG_DFX_SUPPORT_SENSOR_METRICS_REPORT    DFX_YES
#endif

/* EVENT 主动上报 */
#ifndef CONFIG_DFX_SUPPORT_EVENT_REPORT
#define CONFIG_DFX_SUPPORT_EVENT_REPORT             DFX_YES
#endif

/* 上报通道支持SPP */
#ifndef CONFIG_DFX_DATA_TRANSPORT_BT
#define CONFIG_DFX_DATA_TRANSPORT_BT                DFX_YES
#endif

/* 上报通道支持BLE */
#ifndef CONFIG_DFX_DATA_TRANSPORT_BLE
#define CONFIG_DFX_DATA_TRANSPORT_BLE               DFX_NO
#endif

#endif /* DFX_FEATURE_CONFIG_H */

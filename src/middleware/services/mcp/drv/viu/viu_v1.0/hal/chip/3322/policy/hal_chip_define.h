/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: head file of crg
 * Author: AuthorNameMagicTag
 * Create: 2025-04-23
 */

#ifndef HAL_CHIP_DEFINE_H
#define HAL_CHIP_DEFINE_H

#include "drv_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* #ifdef __cplusplus */

/* VALUE_BETWEEN(x,min.max)   True if x is between [min,max] inclusively. */
#define VALUE_BETWEEN(x, min, max) (((x) >= (min)) && ((x) <= (max)))

/* CHN中断状态 */
typedef enum {
    VI_CHN_INT_FSTART      = 0x1 << 0,  /* 场/帧起始中断状态 */
    VI_CHN_INT_CC          = 0x1 << 1,  /* 获取完毕中断状态 */
    VI_CHN_INT_BUFF_OVF    = 0x1 << 2,  /* 内部FIFO溢出错误中断状态 */
    VI_CHN_INT_FRM_THROW   = 0x1 << 3,  /* 场/帧丢失中断状态 */
    VI_CHN_INT_UPDATE_CFG  = 0x1 << 4,  /* 寄存器更新中断状态 */
    VI_CHN_INT_DLY_0       = 0x1 << 15, /* delay0后的场/帧起始中断状态 */
    VI_CHN_INT_EM_FINISH   = 0x1 << 18, /* EM信息完成中断状态 */
    VI_CHN_INT_EM_BUF_OVF  = 0x1 << 20, /* EM写出内部FIFO溢出错误中断状态 */
    VI_CHN_INT_TUNL_LINE   = 0x1 << 21, /* 低延时水线中断状态 */
} vi_chn_int;

#define VI_CHN_INT_MASK (VI_CHN_INT_FSTART | VI_CHN_INT_TUNL_LINE)
#define VI_CHN_INT_EARLY_MASK (VI_CHN_INT_FSTART | VI_CHN_INT_DLY_0 | VI_CHN_INT_TUNL_LINE)
#define VI_CHN_INT_EARLY_END_MASK (VI_CHN_INT_FSTART | VI_CHN_INT_DLY_0 | VI_CHN_INT_CC | VI_CHN_INT_TUNL_LINE)
#define VI_CHN_CLEAR_INT_STATUS (0xFFFFFFFF)

/* 中断使能掩码或中断状态 */
#define WCH_FRAMEPULSE_INT      (0x1U << 0)      /* [0]场起始中断 */
#define WCH_CC_INT              (0x1U << 1)      /* [1]原始码流获取完毕中断状态 */
#define WCH_BUFOVF_INT          (0x1U << 2)      /* [2]原始码流内部FIFO溢出错误中断状态 */
#define WCH_DATATHROW_INT       (0x1U << 3)      /* [3]场数据丢失中断 */
#define WCH_REGUPDATE_INT       (0x1U << 4)      /* [4]工作寄存器更新中断 */
#define WCH_FSTART_DLY_INT      (0x1U << 15)     /* [15]帧起始延时中断 */

#define VICAP_FRAME_START_INT WCH_FRAMEPULSE_INT
#define VICAP_FRAME_EARLY_INT WCH_FSTART_DLY_INT
#define VICAP_FRAME_END_INT   WCH_CC_INT
#define VICAP_FRAME_ERR_INT   (WCH_BUFOVF_INT | WCH_DATATHROW_INT)
#define VICAP_FRAME_ALL_INT   0xFFFF

#define VIU_GLOBAL_INT_PT0           (0x1U << 0)
#define VIU_GLOBAL_INT_PT1           (0x1U << 1)
#define VIU_GLOBAL_INT_WCH0          (0x1U << 8)
#define VIU_GLOBAL_INT_WCH1          (0x1U << 9)
#define VIU_GLOBAL_INT_WCH2          (0x1U << 10)
#define VIU_GLOBAL_INT_WCH3          (0x1U << 11)
#define VIU_GLOBAL_INT_ISP_FE0       (0x1U << 16)
#define VIU_GLOBAL_INT_ISP_FE1       (0x1U << 17)
#define VIU_GLOBAL_INT_ISP_FE2       (0x1U << 18)
#define VIU_GLOBAL_INT_ISP_FE3       (0x1U << 19)
#define VIU_GLOBAL_INT_DBG           (0x1U << 26) /* 系统总线异常中断使能 */

#define VIU_GLOBAL_INT_MASK          (VIU_GLOBAL_INT_WCH0 | VIU_GLOBAL_INT_WCH1)

#define VIU_CAP_ERR_INT               (WCH_BUFOVF_INT | WCH_DATATHROW_INT)

/* 读写outstanding */
#define VICAP_RD_OUTSTANDING 16
#define VICAP_WR_OUTSTANDING 8

typedef enum {
    VI_SYNC_MODE_INTERNAL = 0,
    VI_SYNC_MODE_EXTERNAL
} hal_sync_mode;

#define viu_hal_err(fmt...)
#define viu_hal_trace(fmt, ...)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

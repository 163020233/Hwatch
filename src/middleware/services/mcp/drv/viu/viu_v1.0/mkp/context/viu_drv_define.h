/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 * Notes:
 * History:
 */

#ifndef __VIU_DRV_DEFINE_H__
#define __VIU_DRV_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VIU_MAX_STITCH_GRP_NUM          0
#define VIPROC_IRQ_NUM                  1

#define VIU_FPN_MAX_WIDTH               4096
#define VIU_FPN_MAX_HEIGHT              VIU_DEV_MAX_HEIGHT

#define VIU_PIPE_WDR_FIRST_MAX_WIDTH    4096
#define VIU_PIPE_WDR_SECOND_MAX_WIDTH   2048
#define VIU_VIR_PIPE_MAX_WIDTH          4608
#define VIU_VIR_PIPE_MAX_HEIGHT         4608

#define VIU_PHYCHN_OFFLINE_MIN_WIDTH    120
#define VIU_PHYCHN_OFFLINE_MIN_HEIGHT   120
#define VIU_PHYCHN_OFFLINE_MAX_WIDTH    4608
#define VIU_PHYCHN_OFFLINE_MAX_HEIGHT   4608

#define VIU_PHYCHN_ONLINE_MIN_WIDTH     120
#define VIU_PHYCHN_ONLINE_MIN_HEIGHT    120
#define VIU_PHYCHN_ONLINE_MAX_WIDTH     4096
#define VIU_PHYCHN_ONLINE_MAX_HEIGHT    4608

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

#define VIU_LUMA_PHY_ALIGN              16
#define VIU_REGION_LUMA_BUFFER_SIZE     (ISP_LUMA_STAT_NUM * VIU_LUMA_PHY_ALIGN)

#define VIU_BUF_ALIGN_BYTE                  256 /* 以上各buffer首地址都要求256byte对齐 */

#define VIU_MAX_FPS                         240
#define VIU_HW_HEIGHT_ALIGN                 4U
#define VIU_HW_WIDTH_ALIGN                  4U
#define VIU_HW_CROP_ALIGN                   2U
#define VIU_LINE_BUFFER                     4096U
#define VIU_WDR_BUFFER                      4096U
#define VIU_PIPE_POST_CROP_MIN_WIDTH        32U
#define VIU_PIPE_POST_CROP_MIN_HEIGHT       32U

#define VIU_INDICATOR_MASK                  (VIU_GLOBAL_INT_WCH0 | VIU_GLOBAL_INT_WCH1 | \
                                                VIU_GLOBAL_INT_WCH2 | VIU_GLOBAL_INT_WCH3)

#define VIU_FE_CHN_INT_MASK_OF              0x0
#define VIU_FE_CHN_INT_MASK_ON              0x1

#define ISP_DRC_DDR_SIZE                   (66912) /* (49 + 2) * 41 * 8 * 4 */

#define VIU_PROC_FRAME_PLUSE_INT            VIU_PROC_FRAME_START

#define VIU_FPN_MODE_CORRECTION       0x0
#define VIU_FPN_MODE_CALIBRATE        0x1

#define VIU_DBG_INT_AXI_BUS_ERROR    (0x1U << 0)
#define VIU_DBG_INT_APB_BUS_ERROR    (0x1U << 1)

#define VIU_AXI_WR_ERR_INT_CH0_Y     (0x1U << 0)
#define VIU_AXI_WR_ERR_INT_CH0_C     (0x1U << 1)
#define VIU_AXI_WR_ERR_INT_CH1_Y     (0x1U << 2)
#define VIU_AXI_WR_ERR_INT_CH1_C     (0x1U << 3)
#define VIU_AXI_WR_ERR_INT_CH2_Y     (0x1U << 4)
#define VIU_AXI_WR_ERR_INT_CH2_C     (0x1U << 5)
#define VIU_AXI_WR_ERR_INT_CH3_Y     (0x1U << 6)
#define VIU_AXI_WR_ERR_INT_CH3_C     (0x1U << 7)
#define VIU_AXI_WR_ERR_INT_CH2_EM    (0x1U << 16)
#define VIU_AXI_WR_ERR_INT_ISP0_STT  (0x1U << 22)
#define VIU_AXI_WR_ERR_INT_ISP1_STT  (0x1U << 23)
#define VIU_AXI_WR_ERR_INT_ISP2_STT  (0x1U << 24)
#define VIU_AXI_WR_ERR_INT_ISP3_STT  (0x1U << 25)

#define VIU_PROC_EOF_INT          (0x1U << 0)   /* 帧完成中断 */
#define VIU_PROC_EOF_END_INT      (0x1U << 1)   /* 链表完成中断 */
#define VIU_PROC_TIME_OUT_INT     (0x1U << 2)   /* 离线超时中断 */
#define VIU_PROC_WBUS_ERR_INT     (0x1U << 3)   /* 读总线错误中断 */
#define VIU_PROC_RBUS_ERR_INT     (0x1U << 4)   /* 写总线错误中断 */
#define VIU_PROC_WRAP_OVERFLOW    (0x1U << 5)   /* wrap_overflow绕线溢出中断 */
#define VIU_PROC_APB_ERR_INT      (0x1U << 6)   /* APB总线错误中断 */
#define VIU_PROC_DCMP_ERR_INT     (0x1U << 7)   /* 解压错误中断 */
#define VIU_PROC_FRAME_FLOW_INT   (0x1U << 8)   /* 在线时帧溢出中断 */
#define VIU_PROC_FRAME_ERR_INT    (0x1U << 9)   /* 在线时帧错误中断 */
#define VIU_PROC_FRAME_START      (0x1U << 10)  /* 在线时帧起始中断 */
#define VIU_PROC_TUNL_FINISH_INT  (0x1U << 11)  /* tunl结束完成中断 */
#define VIU_PROC_CFG_LOSS         (0x1U << 12)  /* 在线时寄存器配置丢失中断 */
#define VIU_PROC_LOAD_LUT         (0x1U << 13)  /* LUT载入完成中断mask */
#define VIU_PROC_BE_FRAME_START   (0x1U << 16)  /* 在线时BE起始中断 */
#define VIU_PROC_BE_UPDATE_CFG    (0x1U << 17)  /* 在线时BE寄存器更新中断 */
#define VIU_PROC_BE_CFG_LOSS      (0x1U << 18)  /* 在线时BE寄存器配置丢失中断 */
#define VIU_PROC_AE_EOF           (0x1U << 20)  /* 在线时AE统计信息完成中断 */
#define VIU_PROC_AWB_EOF          (0x1U << 21)  /* 在线时AWB统计信息完成中断 */
#define VIU_PROC_AF_EOF           (0x1U << 22)  /* 在线时AF统计信息完成中断 */
#define VIU_PROC_DIS_EOF          (0x1U << 23)  /* 在线时DIS统计信息完成中断 */
#define VIU_PROC_LDCI_EOF         (0x1U << 25)  /* 在线时LDCI统计信息完成中断 */
#define VIU_PROC_DEHAZE_EOF       (0x1U << 26)  /* 在线时DEHAZE统计信息完成中断 */
#define VIU_PROC_FLICK_EOF        (0x1U << 27)  /* 在线时FLICK统计信息完成中断 */
#define VIU_PROC_MG_EOF           (0x1U << 28)  /* 在线时MG统计信息完成中断 */
#define VIU_PROC_DRC_INT          (0x1U << 29)  /* 在线时DRC统计信息完成中断 */
#define VIU_PROC_FPN_INT          (0x1U << 30)  /* 在线时FPN统计信息完成中断 */
#define VIU_PROC_DCG_INT          (0x1U << 31)  /* 在线时DCG统计信息完成中断 */

#define VIU_PROC_ONLINE_MASK      VIU_PROC_FRAME_START

#define VIU_PROC_ONLINE_ERR_INT  (VIU_PROC_FRAME_ERR_INT | VIU_PROC_FRAME_FLOW_INT \
        | VIU_PROC_DCMP_ERR_INT | VIU_PROC_WBUS_ERR_INT | VIU_PROC_RBUS_ERR_INT)

#define VIU_PROC_OFFLINE_ERR_INT (VIU_PROC_TIME_OUT_INT | VIU_PROC_FRAME_FLOW_INT \
        | VIU_PROC_DCMP_ERR_INT | VIU_PROC_WBUS_ERR_INT | VIU_PROC_RBUS_ERR_INT)

#define VIU_PROC_LOST_CFG_INT         (VIU_PROC_CFG_LOSS)

#define VIU_CAP_ERR_INT               (WCH_BUFOVF_INT | WCH_DATATHROW_INT)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* End of __VIU_DRV_DEFINE_H__ */


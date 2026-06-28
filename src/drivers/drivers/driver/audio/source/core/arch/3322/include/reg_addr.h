/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: reg addr
 * Author: audio
 */

#ifndef __REG_ADDR_H__
#define __REG_ADDR_H__

#define AUDIO_SYS_BASE_ADDR     0xE8000000
#define DRV_CFG_SUB_BASE_ADDR   0xE8007000 /* AUD_DSP_CFG */
#define CODEC_REG_BASE_ADDR     0xE8380000
#define DRV_DMAC_BASE_ADDR      0xE8000000
#define DRV_WATCHDOG_BASE_ADDR  0xE800B000
#define SOC_REG_BASE_ADDR       0x57000000
#define DRV_UART_BASE_ADDR      0x55004000 /* COM UART */

#define REG_WDOG_BASE_ADDR      (AUDIO_SYS_BASE_ADDR + 0x003000)
#define DSP_UART_BASE_ADDR      (AUDIO_SYS_BASE_ADDR + 0x005000)
#define SIO_CRG_REG_ADDR        (AUDIO_SYS_BASE_ADDR + 0x007000)
#define DSP_UART_DIV_0_ADDR     (AUDIO_SYS_BASE_ADDR + 0x00700C)
#define DSP_WORK_ON_ADDR        (AUDIO_SYS_BASE_ADDR + 0x0070B0)
#define CODEC_DIG_CFG_REG_ADDR  (AUDIO_SYS_BASE_ADDR + 0x009000)
#define AUDIO_CODEC_BASE_ADDR   CODEC_REG_BASE_ADDR

#define DRV_CFG_REG_RST_EN1     (DRV_CFG_SUB_BASE_ADDR + 0x8)
#define DRV_CFG_HIFI_INT        (DRV_CFG_SUB_BASE_ADDR + 0x0030)
#define DRV_CFG_DSP_INT_MASK    (DRV_CFG_SUB_BASE_ADDR + 0x0070)
#define DRV_CFG_DSP2OUT_INT     (DRV_CFG_SUB_BASE_ADDR + 0x0098)
#define DRV_CFG_CLK_DIV_LOAD_EN (DRV_CFG_SUB_BASE_ADDR + 0x0594)
#define DRV_CFG_DSP_INT_MASK2   (DRV_CFG_SUB_BASE_ADDR + 0x0620)

#define DEFAULT_MST_HIFI3Z_CTL          (DRV_CFG_SUB_BASE_ADDR + 0x0750)
#define DEFAULT_MST_HIFI3Z_DES_ADDR     (DRV_CFG_SUB_BASE_ADDR + 0x0754)
#define DEFAULT_MST_HIFI3Z_RO_STS       (DRV_CFG_SUB_BASE_ADDR + 0x0768)
#define DEFAULT_MST_HIFI3Z_ARADDR_RO    (DRV_CFG_SUB_BASE_ADDR + 0x076C)
#define DEFAULT_MST_HIFI3Z_AWADDR_RO    (DRV_CFG_SUB_BASE_ADDR + 0x0770)
#define DEFAULT_MST_AXIDMA_CTL          (DRV_CFG_SUB_BASE_ADDR + 0x0774)
#define DEFAULT_MST_AXIDMA_DES_ADDR     (DRV_CFG_SUB_BASE_ADDR + 0x0778)
#define DEFAULT_MST_AXIDMA_RO_STS       (DRV_CFG_SUB_BASE_ADDR + 0x0790)
#define DEFAULT_MST_AXIDMA_ARADDR_RO    (DRV_CFG_SUB_BASE_ADDR + 0x0794)
#define DEFAULT_MST_AXIDMA_AWADDR_RO    (DRV_CFG_SUB_BASE_ADDR + 0x0798)

#define LP_CTL_AUDIO_BASE_ADDR          (SOC_REG_BASE_ADDR + 0x5000)
#define LP_CTL_AUDIO_INTR_EN            (LP_CTL_AUDIO_BASE_ADDR + 0x100)
#define LP_CTL_AUDIO_WKUP_EN            (LP_CTL_AUDIO_BASE_ADDR + 0x150)
#define LP_CTL_AUDIO_WKUP_CLR           (LP_CTL_AUDIO_BASE_ADDR + 0x250)
#define LP_CTL_AUDIO_WKUP_INT_EN        (LP_CTL_AUDIO_BASE_ADDR + 0x300)
#define LP_CTL_AUDIO_WKUP_INT_CLR       (LP_CTL_AUDIO_BASE_ADDR + 0x400)
#define LP_CTL_AUDIO_ALLOW_TO_SLEEP     (LP_CTL_AUDIO_BASE_ADDR + 0x450)
#define LP_CTL_AUDIO_SLP_VAL            (LP_CTL_AUDIO_BASE_ADDR + 0x550)
#define LP_CTL_AUDIO_MAN_FRC_ON_SEQ     (LP_CTL_AUDIO_BASE_ADDR + 0x860)
#define LP_CTL_AUDIO_SLEEPING           (LP_CTL_AUDIO_BASE_ADDR + 0xC5C)

#define RTC_BASE_ADDR                   (SOC_REG_BASE_ADDR + 0x17000)
#define RTC_TIMER_CURRENT_VAL           (RTC_BASE_ADDR + 0x300)
#define RTC_TIMER_CONTROL_REG           (RTC_BASE_ADDR + 0x310)
#define RTC_TIMER_INT_CLR               (RTC_BASE_ADDR + 0x314)

/* EDMA start */
#define DSP_REG_CLK_RST_EN_ADDR (DRV_CFG_SUB_BASE_ADDR + 0x0008) /* REG_CLK_RST_EN1 */
#define DSP_DMAC_CLK_EN_BIT       0 /* dmac int mask is the bit0 of *REG_CLK_RST_EN1* reg; bit0:clk_en; bit1:rst_en */
#define DSP0_INT_MASK_OFFSET      0x0070 /* AUD_DSP_CFG */
#define DSP1_INT_MASK_OFFSET      0x0070 /* no use:don't have dsp1 */
#define DSP_DMAC_INT_MASK_BIT     7 /* dmac int mask is the bit7 of *INT_MASK* reg; bit7:s; bit8:ns */
#define EDMAC0_BREQ_CLR_EN_OFFSET 0x0084 /* AUD_DSP_CFG */
#define EDMAC1_BREQ_CLR_EN_OFFSET 0x0084 /* no use:don't have dsp1 */
/* EDMA end */

/* BT toggle reg start */
#define REG_BT_TOG_INT_CLR (DRV_CFG_SUB_BASE_ADDR + 0x0024)
#define REG_BT_TOG_INT_MASK (DRV_CFG_SUB_BASE_ADDR + 0x0028)
#define REG_BT_TOG_INT_STATE (DRV_CFG_SUB_BASE_ADDR + 0x002C)
/* BT toggle reg end */

#define DSP_WDG_REG_BASE        REG_WDOG_BASE_ADDR
#define WDT_LOAD                0x0000
#define WDT_VAL                 0x0004
#define WDT_CTRL                0x0008
#define WDT_INTCLR              0x000C
#define WDT_RIS                 0x0010
#define WDT_MIS                 0x0014
#define WDT_LOCK                0x0C00

#define SOC_M_CTL_RB            0x52000000

#define SOC_COM_CTL_RB          0x55000000
#define COM_HIFI_CR_CH1_OFFSET  0x0640
#define COM_CH_SEL_OFFSET       0x0AF0

#define SOC_GLB_CTL_M_RB        0x57000000
#define SOC_PMU1_CTL_RB         (SOC_GLB_CTL_M_RB + 0x4000)
#define SOC_AGPIO_MODE_PAD      (SOC_GLB_CTL_M_RB + 0x9000)
#define SOC_AGPIO_MODE_RB       (SOC_GLB_CTL_M_RB + 0xA400)

/* systick in AON */
#define SOC_AON_SYSTICK_CLK_HZ    (32768)
#define SOC_AON_SYSTICK_CFG_M     (SOC_GLB_CTL_M_RB + 0x03E4)
#define SOC_AON_SYSTICK_VALUE_M_0 (SOC_GLB_CTL_M_RB + 0x03E8)
#define SOC_AON_SYSTICK_VALUE_M_1 (SOC_GLB_CTL_M_RB + 0x03EC)
#define SOC_AON_SYSTICK_VALUE_M_2 (SOC_GLB_CTL_M_RB + 0x03F0)
#define SOC_AON_SYSTICK_VALUE_M_3 (SOC_GLB_CTL_M_RB + 0x03F4)

/* systick for dsp */
#define DSP_SYSTICK_CLK_HZ        SOC_AON_SYSTICK_CLK_HZ
#define DSP_SYSTICK_CFG_M         SOC_AON_SYSTICK_CFG_M
#define DSP_SYSTICK_VALUE_M_0     SOC_AON_SYSTICK_VALUE_M_0
#define DSP_SYSTICK_VALUE_M_1     SOC_AON_SYSTICK_VALUE_M_1
#define DSP_SYSTICK_VALUE_M_2     SOC_AON_SYSTICK_VALUE_M_2
#define DSP_SYSTICK_VALUE_M_3     SOC_AON_SYSTICK_VALUE_M_3

#define SOC_GLB_CTL_D_RB        0x57000800
#define DRV_CFG_TASK_LOOP_CNT_BASE_ADDR          (SOC_GLB_CTL_D_RB + 0x0010) /* DSP0:0x0010;DSP1:0x0014 */
#define DRV_CFG_KEY_POINT_MARK_BASE_ADDR         (SOC_GLB_CTL_D_RB + 0x0018) /* DSP0:0x0018;DSP1:0x001C */

#define SOC_AUD_DSP_CFG         0x58007000

#define I2S_REG_BASE_ADDR       (CODEC_REG_BASE_ADDR + 0x8000)

#define DSPIF0_FIFO_ADDR        0xE8010000
#define DSPIF1_FIFO_ADDR        (DSPIF0_FIFO_ADDR + 0x1000)
#define DSPIF2_FIFO_ADDR        (DSPIF0_FIFO_ADDR + 0x2000) /* not used */

#define CODEC_FIFO_ADDR         0xE838E000
#define ANC_HT_DNLINK_ADDR      0
#define ANC_TP1_DNLINK_ADDR     0
#define ANC_TP0_DNLINK_ADDR     0
#define ANC_TOSPK_FAREND_ADDR   0
#define ANC_REF_ERR_ADDR        0
#define ANC_FF_FB_ADDR          0
#define AUDIO_DNLINK_ADDR       (CODEC_FIFO_ADDR + 0x000)
#define AUDIO_DNLINK1_ADDR      (CODEC_FIFO_ADDR + 0x004)
#define AUDIO_DNLINK2_ADDR      (CODEC_FIFO_ADDR + 0x008) /* not used */
#define AUDIO_DNLINK3_ADDR      (CODEC_FIFO_ADDR + 0x00C) /* not used */
#define AUDIO_DNLINK4_ADDR      (CODEC_FIFO_ADDR + 0x010)
#define AUDIO_DNLINK5_ADDR      (CODEC_FIFO_ADDR + 0x014) /* not used */
#define MIC01_UPLINK_ADDR       (CODEC_FIFO_ADDR + 0x018)
#define MIC23_UPLINK_ADDR       (CODEC_FIFO_ADDR + 0x01C) /* not used */
#define MIC45_UPLINK_ADDR       (CODEC_FIFO_ADDR + 0x020) /* not used */
#define MIC67_UPLINK_ADDR       0
#define EC_UPLINK_ADDR          (CODEC_FIFO_ADDR + 0x034)
#define VAD_UPLINK_ADDR         (CODEC_FIFO_ADDR + 0x03C)
#define MAD_UPLINK_ADDR         VAD_UPLINK_ADDR
#define ANC_TP0_UPLINK_ADDR     (CODEC_FIFO_ADDR + 0x044) /* not used */
#define ANC_TP1_UPLINK_ADDR     (CODEC_FIFO_ADDR + 0x048) /* not used */
#define ANC_TP2_UPLINK_ADDR     (CODEC_FIFO_ADDR + 0x04C) /* not used */
#define ANC_TP3_UPLINK_ADDR     (CODEC_FIFO_ADDR + 0x050) /* not used */
#define ANC_TP4_UPLINK_ADDR     (CODEC_FIFO_ADDR + 0x054) /* not used */

#define AUDIO_DN0_DMA_CFG                   0x83322087
#define AUDIO_DN1_DMA_CFG                   0x83322097
#define AUDIO_DN2_DMA_CFG                   0x833220c7
#define AUDIO_DN3_DMA_CFG                   0x833221e7
#define AUDIO_DN4_DMA_CFG                   0x833220D7 // AUDIO_LD_DNLINK
#define AUDIO_DN5_DMA_CFG                   0x833220D7

#define TP0_DMA_CFG                         0x4ff220e7
#define TP1_DMA_CFG                         0x4ff220f7
#define TP2_DMA_CFG                         0x43322107
#define TP3_DMA_CFG                         0x43322117
#define TP4_DMA_CFG                         0x43322127
#define MIC01_DMA_CFG                       0x433220C7
#define MIC23_DMA_CFG                       0x43322187
#define MIC45_DMA_CFG                       0x43322197
#define VAD_DMA_CFG                         0x433220B7
#define MAD_DMA_CFG                         VAD_DMA_CFG
#define EC_DMA_CFG                          0x433220A7

#define I2S0_UP_DMA_CFG                     0x40033007
#define I2S0_DN_DMA_CFG                     0x80033017
#define I2S0_UP_PCM_MODE_DMA_CFG            0x47722007
#define I2S0_DN_PCM_MODE_DMA_CFG            0x87722017
#define I2S1_UP_DMA_CFG                     0x40033027
#define I2S1_DN_DMA_CFG                     0x80033037
#define I2S1_UP_PCM_MODE_DMA_CFG            0x47722027
#define I2S1_DN_PCM_MODE_DMA_CFG            0x87722037

#define SIO_MIC01_UPLINK_FIFO MIC01_UPLINK_ADDR

#define ROM_ENTRY_ADDR                      0xc8100000
#define ROM_SIZE                            0x280000

#endif /* __REG_ADDR_H__ */

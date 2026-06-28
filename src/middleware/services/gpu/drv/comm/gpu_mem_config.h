/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description : gpu sram config file
 */

#ifndef COMM_GPU_MEM_CONFIG_H
#define COMM_GPU_MEM_CONFIG_H

#ifdef GPU_3322
#include "psram.h"
#endif

#ifdef GPU_1A22
#include "memory_config_common.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#if defined(GPU_1A21)
#define GPU_SRAM_START      0x20340000
#define GPU_SRAM_END        0x204DFFFF
#define GPU_SRAM_VIR_FB0    0x21340000
#define GPU_SRAM_VIR_FB1    0x21B40000
#define GPU_REG_LUT_ADDR    0x204E0000
#define GPU_REG_LUT_SIZE    0x95
#elif defined(GPU_3322)
#define GPU_SRAM_START      0x20340000
#define GPU_SRAM_END        0x203FFFFF
#define GPU_SRAM_VIR_FB0    0x21340000
#define GPU_SRAM_VIR_FB1    0x21340000
#define GPU_REG_LUT_ADDR    0x204E0000
#define GPU_REG_LUT_SIZE    0x97F
#define NPU_SRAM_START      0x20040000
#elif defined(GPU_1A22)
#if defined(LPDDR_SUPPORT) && (LPDDR_SUPPORT == YES) && !defined(U7_ENVIRONMENT)
#define GPU_SRAM_START      L2RAM_FRAMEBUFFER_START_ADDR
#define GPU_SRAM_END        (GPU_SRAM_START + L2RAM_FRAMEBUFFER_LENGTH - 1)
#else
#define GPU_SRAM_START      0xB0350000
#define GPU_SRAM_END        0xB0390000
#endif
#else
#define GPU_SRAM_START      0x60240000
#define GPU_SRAM_END        0x6039FFFF
#define GPU_SRAM_VIR_FB0    0x61240000
#define GPU_SRAM_VIR_FB1    0x61A40000
#define GPU_REG_LUT_ADDR    0x603E0000
#define GPU_REG_LUT_SIZE    0x3FFF
#endif

#define GPU_SRAM_SIZE       (GPU_SRAM_END + 1 - GPU_SRAM_START)

#if defined(GPU_1A21)

#define GPU_PSRAM_START     0x25F00000
#define GPU_DSP_NODE_START  0x20300000
#define GPU_DSP_NODE_END    0x20340000

#define GPU_HDR_COEF_SIZE   (1 * 1024) /* 1K for HDR COEF */
#define GPU_HDR_COEF_ADDR   (GPU_DSP_NODE_END - GPU_HDR_COEF_SIZE)    /* 0x2033FC00 ~ 0x20340000 */

#define GPU_SRAM_DRAW_SIZE  (31 * 1024) /* 31K for path cmd/data */
#define GPU_SRAM_DRAW_ADDR  (GPU_HDR_COEF_ADDR - GPU_SRAM_DRAW_SIZE)  /* 0x20338000 ~ 0x2033FC00 */

#define GPU_SRAM_NODE_SIZE  (224 * 1024) /* 224K for node/handle */
#define GPU_SRAM_NODE_ADDR  (GPU_SRAM_DRAW_ADDR - GPU_SRAM_NODE_SIZE) /* 0x20300000 ~ 0x20338000 */

#define GPU_SRAM_CMD_SIZE   (0) /* cmd size use dsp sram for gpu */

#elif defined(GPU_1A22)

#define GPU_PSRAM_START     0xC9500000

#define GPU_HDR_COEF_SIZE   (1 * 1024) /* 1K for HDR COEF */
#define GPU_HDR_COEF_ADDR   (GPU_SRAM_END + 1 - GPU_HDR_COEF_SIZE)

#define GPU_ZME_COEF_SIZE   (1 * 1024) /* 1K for ZME COEF */
#define GPU_ZME_COEF_ADDR   (GPU_HDR_COEF_ADDR - GPU_ZME_COEF_SIZE)

#define GPU_SRAM_DRAW_SIZE  (30 * 1024) /* 30K for path cmd/data */
#define GPU_SRAM_DRAW_ADDR  (GPU_ZME_COEF_ADDR - GPU_SRAM_DRAW_SIZE)

#define GPU_SRAM_NODE_SIZE  (224 * 1024) /* 224K for node/handle */
#define GPU_SRAM_NODE_ADDR  (GPU_SRAM_DRAW_ADDR - GPU_SRAM_NODE_SIZE)

#define GPU_SRAM_CMD_SIZE   (0) /* cmd size use dsp sram for gpu */

#elif defined(GPU_3322)

#define GPU_PSRAM_START     PSRAM_DISP_VAU_NODE_ADDR

#define GPU_HDR_COEF_SIZE   (1 * 1024) /* 1K for HDR COEF */
#define GPU_HDR_COEF_ADDR   (GPU_SRAM_END + 1 - GPU_HDR_COEF_SIZE)    /* 0x203FFC00 ~ 0x20400000 */

#define GPU_ZME_COEF_SIZE   (1 * 1024) /* 1K for ZME COEF */
#define GPU_ZME_COEF_ADDR   (GPU_HDR_COEF_ADDR - GPU_ZME_COEF_SIZE)   /* 0x203FF800 ~ 0x203FFC00 */

#define GPU_SRAM_DRAW_SIZE  (28 * 1024) /* 28K for cmd/data */
#define GPU_SRAM_DRAW_ADDR  (GPU_ZME_COEF_ADDR - GPU_SRAM_DRAW_SIZE)  /* 0x203F8800 ~ 0x203FF800 */

#define GPU_SRAM_NODE_SIZE  (96 * 1024) /* 96K for node/handle */
#ifdef GPU_HIGH_RESOLUTION
#define GPU_SRAM_NODE_ADDR  NPU_SRAM_START /* 0x20040000 ~ 0x20058000 */
#else
#define GPU_SRAM_NODE_ADDR  (GPU_SRAM_DRAW_ADDR - GPU_SRAM_NODE_SIZE) /* 0x203E0800 ~ 0x203F8800 */
#endif

#define GPU_SRAM_CMD_SIZE   (GPU_HDR_COEF_SIZE + GPU_ZME_COEF_SIZE + GPU_SRAM_DRAW_SIZE + GPU_SRAM_NODE_SIZE)

#else

#define GPU_SRAM_PRE_SIZE   (1 * 1024) /* 1K for vau ip tmp dcmp */
#define GPU_SRAM_PRE_ADDR   (GPU_SRAM_END + 1 - GPU_SRAM_PRE_SIZE) /* 0x6039FC00 ~ 0x6039FFFF */

#define GPU_SRAM_DRAW_SIZE  (63 * 1024) /* 63K for cmd/data */
#define GPU_SRAM_DRAW_ADDR  (GPU_SRAM_PRE_ADDR - GPU_SRAM_DRAW_SIZE) /* 0x60390000 ~ 0x6039FC00 */

#define GPU_SRAM_NODE_SIZE  (128 * 1024) /* 128K for node/handle */
#define GPU_SRAM_NODE_ADDR  (GPU_SRAM_DRAW_ADDR - GPU_SRAM_NODE_SIZE) /* 0x60370000 ~ 0x60390000 */

#define GPU_SRAM_CMD_SIZE   (GPU_SRAM_PRE_SIZE + GPU_SRAM_DRAW_SIZE + GPU_SRAM_NODE_SIZE)
#endif

#if defined(GPU_PSRAM_SUPPORT) || defined(GPU_DDR_SUPPORT)
#define GPU_DUMP_DEBUG_ADDR (GPU_PSRAM_START)
#ifdef GPU_DUMP_SUPPORT
#define GPU_DUMP_DEBUG_SIZE (5 * 1024) /* 5K for DUMP DEBUG */
#else
#define GPU_DUMP_DEBUG_SIZE 0
#endif

#ifdef GPU_1A21
#define GPU_PSRAM_ADDR         (GPU_DUMP_DEBUG_ADDR + GPU_DUMP_DEBUG_SIZE)
#define GPU_PSRAM_SIZE         (0x100000 - GPU_DUMP_DEBUG_SIZE) // 1MB - 5KB
#elif defined(GPU_1A22)
#define GPU_SDF_ALG_ADDR       (GPU_DUMP_DEBUG_ADDR + GPU_DUMP_DEBUG_SIZE)
#define GPU_SDF_ALG_SIZE       (28 * 1024) /* 7*4=28K for SDF COEF */
#define GPU_PSRAM_ADDR         (GPU_SDF_ALG_ADDR + GPU_SDF_ALG_SIZE)
#define GPU_PSRAM_SIZE         (0x100000 - GPU_SDF_ALG_SIZE - GPU_DUMP_DEBUG_SIZE)
#else
#define GPU_PSRAM_ADDR         (GPU_DUMP_DEBUG_ADDR + GPU_DUMP_DEBUG_SIZE)
#define GPU_PSRAM_SIZE         (0x100000 - GPU_DUMP_DEBUG_SIZE) // 1MB - 5KB
#endif

// blur internal buf & node reuse
#define GPU_PSRAM_BLUR_MASK_A8 (GPU_PSRAM_ADDR)
#define GPU_PSRAM_BLUR_MASK_A8_SIZE 0x40000
// for scale 1X RGB, 384K max
#define GPU_PSRAM_BLUR_INT_BUF (GPU_PSRAM_BLUR_MASK_A8 + GPU_PSRAM_BLUR_MASK_A8_SIZE)
#define GPU_PSRAM_BLUR_INT_BUF_SIZE 0x60000

#define GPU_PSRAM_NODE_ADDR     (GPU_PSRAM_ADDR)
#define GPU_PSRAM_NODE_SIZE     0x64000

#define GPU_PSRAM_POOL_ADDR    (GPU_PSRAM_BLUR_INT_BUF + GPU_PSRAM_BLUR_INT_BUF_SIZE)
#define GPU_PSRAM_POOL_SIZE    (GPU_PSRAM_SIZE - GPU_PSRAM_BLUR_MASK_A8_SIZE - GPU_PSRAM_BLUR_INT_BUF_SIZE)

#else
#define GPU_PSRAM_ADDR         (0)
#define GPU_PSRAM_SIZE         (0)

#define GPU_PSRAM_BLUR_MASK_A8 (0)
#define GPU_PSRAM_BLUR_INT_BUF (0)

#define GPU_PSRAM_NODE_ADDR    (0)
#define GPU_PSRAM_NODE_SIZE    (0)

#define GPU_PSRAM_POOL_ADDR    (0)
#define GPU_PSRAM_POOL_SIZE    (0)

#define GPU_DUMP_DEBUG_SIZE    (0)
#define GPU_DUMP_DEBUG_ADDR    (0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* COMM_GPU_MEM_CONFIG_H */

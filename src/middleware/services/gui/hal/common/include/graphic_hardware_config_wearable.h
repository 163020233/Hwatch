/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd..
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GRAPHIC_HARDWARE_CONFIG_WEARABLE_H
#define GRAPHIC_HARDWARE_CONFIG_WEARABLE_H

#include "memory_config_common.h"
#ifdef CONFIG_PSRAM_SUPPORT
#include "psram.h"
#endif

/**
 * @brief Hardware acceleration for drawing path. Prerequirement is ENABLE_VGU_ENGINE is 1.
 */
#define CAPABILITY_HWDRAW_PATH                   1
/**
 * @brief Hardware acceleration for drawing letter vector path. Prerequirement is that both ENABLE_VGU_ENGINE and
 *        CAPABILITY_HWDRAW_PATH is 1.
 */
#define CAPABILITY_HWDRAW_LETTER_VECTOR          1
/**
 * @brief Hardware acceleration for drawing alpha bitmap. Prerequirement is ENABLE_VGU_ENGINE is 1.
 */
#define CAPABILITY_HWDRAW_ALPHA_BITMAP           1
/**
 * @brief 1 means outRect is supported.
 *        0 means outRect is not supported, and must be same as the buffer rect.
 */
#define CAPABILITY_HW_OUT_RECT                   1
/**
 * @brief Hardware acceleration for rotate and scale in one operation.
 */
#define CAPABILITY_HW_ROTATE_AND_SCALE           1
/**
 * @brief Enable JPEG preview which includes decoding jpeg file and cacheing decoded data.
 */
#ifdef SUPPORT_GPU_JPEG
#define CAPABILITY_HW_JPEG_DECODE                1
#else
#define CAPABILITY_HW_JPEG_DECODE                0
#endif

#define CAPABILITY_SW_JPEG_DECODE                1
/**
 * @brief Hardware acceleration for gaussian blur.
 */
#define CAPABILITY_HW_GAUSSIAN_BLUR              1
/**
 * @brief Hardware acceleration for gaussian blur by mask.
 */
#define CAPABILITY_HW_GAUSSIAN_BLUR_BY_MASK              1
/**
 * @brief Hardware acceleration for light.
 */
#define CAPABILITY_HW_LIGHT              1
/**
 * @brief Hardware acceleration for colorize.
 */
#define CAPABILITY_HW_COLORIZE              1
/**
 * @brief Hardware acceleration for HSV.
 */
#define CAPABILITY_HW_HSV             1
/**
 * @brief Hardware acceleration for rain.
 */
#define CAPABILITY_HW_RAIN              1

/**
 * @brief ENABLE GMMU MEM.
 */
#ifdef SUPPORT_GPU_GMMU
#undef SUPPORT_GPU_GMMU
#define ENABLE_GMMU                              0
#else
#define ENABLE_GMMU                              0
#endif
/**
 * @brief Enable GPU COMPRESS. Support 2X or 3X compress.
 */
#ifdef SUPPORT_FB_COMPRESS
#define ENABLE_FB_COMPRESS                1
#else
#define ENABLE_FB_COMPRESS                0
#endif
/**
 * @brief Allow video to be sent through VAU.
 *        1 means video to be sent through VAU.
 *        0 means video to be sent through DPU.
 */
#define ENABLE_VIDEO_RGB                          1
/**
 * @brief specify buffer mode when use single buffer.
 *        1 means compressed buffer.
 *        0 means uncompressed buffer.
 */
#define BUFFER_COMPRESS                          1
/**
 * @brief Some of hardware acceleration operations requires an aligned rect.
 *        If this prerequisite is not meet, it could cause extra data been handled.
 */
#define HW_RECT_ALIGN_RESTRICTION                0
/**
 * @brief Hardware acceleration for drawing ellipse. Prerequirement is ENABLE_VGU_ENGINE is 1.
 */
#define CAPABILITY_HWDRAW_ELLIPSE         0
/**
 * @brief Hardware acceleration for drawing line. Prerequirement is ENABLE_VGU_ENGINE is 1.
 */
#define CAPABILITY_HWDRAW_LINE            1
/**
 * @brief Hardware acceleration for drawing arc. Prerequirement is ENABLE_VGU_ENGINE is 1.
 */
#define CAPABILITY_HWDRAW_ARC             1
/**
 * @brief Hardware acceleration for drawing cubic bezier curve. Prerequirement is ENABLE_VGU_ENGINE is 1.
 */
#define CAPABILITY_HWDRAW_CURVE           1
/**
 * @brief Hardware acceleration for drawing rect. Prerequirement is ENABLE_VGU_ENGINE is 1.
 */
#define CAPABILITY_HWDRAW_RECT           1
/**
 * @brief Hardware acceleration for filling small size. Prerequirement is ENABLE_VGU_ENGINE is 1.
 */
#define CAPABILITY_HWDRAW_SMALL_SIZE    1
/**
 * @brief Whether open offline dial.
 */
#define USE_OFFLINE_DIAL           0

#define BLIT_COUNT_MAX 20
#define STROKE_MAX_CNT 48

#define HARDWARE_ACC_SIZE_LIMIT (50 * 50)
/**
 * @brief Use double buffer for rendering process.
 */

#ifndef DOUBLE_BUFFER
#define DOUBLE_BUFFER                     1
#endif

/**
 * @brief Display resolution and the clockwise rotation angle of the screen resolution.
 *        Needs to be configured at 90-degree multiples.
 */
#ifndef HW_ROTATE_ANGLE
#define HW_ROTATE_ANGLE                     0
#endif

/**
 * @brief ROTATE_METHOD rotation mode definition
 * 0: VAU rotation using a temp buffer after rendering
 * 1: VAU rotation dynamically during each draw operation
 * 2: DPU rotation after rendering (default mode)
 * 3: No rotation
 */
#ifndef ROTATE_METHOD
#define ROTATE_METHOD                       2
#endif

/**
 * @brief When HIGH_RESOLUTION is equal to 1, one of frame buffers will be allocated in PSRAM.
 */
#ifndef HIGH_RESOLUTION
#define HIGH_RESOLUTION                     0
#endif

#if !defined(RESOLUTION_WIDTH) || !defined(RESOLUTION_HEIGHT)
#define RESOLUTION_WIDTH                  466
#define RESOLUTION_HEIGHT                 466
#endif

#define ENABLE_ULPS                       0

#if defined(QSPI_DISPLAY)
#define LAYER_PF_CODE                     1
#define LAYER_BPP                         32
#elif defined(MEMORY_MINI)
#define LAYER_PF_CODE                     0
#define LAYER_BPP                         16
#elif HIGH_RESOLUTION
#define LAYER_PF_CODE                     3
#define LAYER_BPP                         16
#else
#define LAYER_PF_CODE                     2
#define LAYER_BPP                         24
#endif

/**
 * @brief Memory pool define.
 */
#define HAL_ALIGN_UP(x, a) ((((x) + ((a) - 1)) / (a)) * (a))

#ifndef CONFIG_PSRAM_SUPPORT // equal to MEMORY_MINI
    #define MEM_POOL_UI_POOL_START  L2M_GPU_RAM_GPU_SHARE_ORIGIN
    #define MEM_POOL_UI_POOL_SIZE   0x13c000 // 1408 - 144 = 1264k

    #define MEM_POOL_UI_FONT    MEM_POOL_UI_POOL_START // 0x60240000
    #define MEM_POOL_UI_FONT_SIZE   0x12000

    #define FB_ADDR_1   (MEM_POOL_UI_FONT + MEM_POOL_UI_FONT_SIZE)
    #define FB_SIZE     (HAL_ALIGN_UP(RESOLUTION_WIDTH, 16) * RESOLUTION_HEIGHT * LAYER_BPP / 8)
    #define FB_ADDR_2   (FB_ADDR_1 + FB_SIZE)
    #define FB_COUNT_IN_MEM_POOL    2
    #define DRAW_MAX_CNT 48
#else
    #define MEM_POOL_UI_POOL_START  PSRAM_GUI_MEM_POOL_ADDR
    #define MEM_POOL_UI_POOL_SIZE   PSRAM_GUI_MEM_POOL_SIZE

    #define MEM_POOL_UI_FONT        MEM_POOL_UI_POOL_START
    #define MEM_POOL_UI_FONT_SIZE   0x132000

    #ifdef SUPPORT_GPU_GMMU // FB(sram) is virtual addr, FB0:0x61240000 FB1:0x61a40000
        #define FB_COUNT_IN_MEM_POOL    0
    #else
        #ifdef QSPI_DISPLAY // FB(sram) is physical addr, FB0:0x60280000 FB1:0x602fe000
            #define FB_ADDR_1   L2M_GPU_RAM_GPU_SHARE_ORIGIN // 0x60280000
            // L2M_GPU_RAM_GPU_SHARE_LENGTH 0x120000 - GPU node 144*1024K
            #define FB_SIZE     (0xfc000 / 2)
            #define FB_ADDR_2   (FB_ADDR_1 + FB_SIZE)
            #define FB_COUNT_IN_MEM_POOL  0
        #else // FB(psram) is physical addr, GUI dynamic allocation
            #define FB_ADDR_1   0x20340000
            #if ENABLE_FB_COMPRESS
                #define FB_SIZE (((HAL_ALIGN_UP(RESOLUTION_WIDTH, 4) / 4) * \
                                (HAL_ALIGN_UP(RESOLUTION_HEIGHT, 4) / 4) * LAYER_BPP))
            #else
                #define FB_SIZE_1 ((HAL_ALIGN_UP(RESOLUTION_WIDTH, 16) * RESOLUTION_HEIGHT * LAYER_BPP / 8))
                #define FB_SIZE_2 ((HAL_ALIGN_UP(RESOLUTION_HEIGHT, 16) * RESOLUTION_WIDTH * LAYER_BPP / 8))
                #define FB_SIZE ((FB_SIZE_1 > FB_SIZE_2) ? FB_SIZE_1 : FB_SIZE_2)
            #endif
            #if HIGH_RESOLUTION
                #define FB_ADDR_2   (MEM_POOL_UI_FONT + MEM_POOL_UI_FONT_SIZE)
                #define FB_COUNT_IN_MEM_POOL    1
            #else
                #define FB_ADDR_2   (FB_ADDR_1 + FB_SIZE)
                #define FB_COUNT_IN_MEM_POOL    0
            #endif
        #endif
    #endif
    #define DRAW_MAX_CNT 640
#endif
#define MEM_POOL_UI_OTHER   (MEM_POOL_UI_POOL_START + MEM_POOL_UI_FONT_SIZE + FB_COUNT_IN_MEM_POOL * FB_SIZE)
#define MEM_POOL_UI_OTHER_SIZE  (MEM_POOL_UI_POOL_SIZE - MEM_POOL_UI_FONT_SIZE - FB_COUNT_IN_MEM_POOL * FB_SIZE)

#define CAPABILITY_HW_SUPPORT_EMOJI 1
#define CAPABILITY_SW_DFX_FRAME_TRACE 1
#define ENABLE_DYNAMIC_FRAME_RATE 1
#define MATRIX_TRANSFORM_SPACE_WORLD 1

#ifdef XTS_SUPPORT
#define RTC_EVENT_THREAD_STACK_SIZE             0x1000u
#else
#define RTC_EVENT_THREAD_STACK_SIZE             0x800u
#endif
#define GUI_MAIN_THREAD_STACK_SIZE              0x6000u
#endif // GRAPHIC_HARDWARE_CONFIG_WEARABLE_H

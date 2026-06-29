/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd..
 * Description: simulator file
 */

#ifndef GRAPHIC_HARDWARE_CONFIG_SIMULATOR_H
#define GRAPHIC_HARDWARE_CONFIG_SIMULATOR_H

/**
 * @brief Hardware acceleration for drawing path. Prerequirement is ENABLE_VGU_ENGINE is 1.
 */

#define ENABLE_VECTOR_FONT_STB                   1

#define CAPABILITY_HWDRAW_PATH                   1
/**
 * @brief Hardware acceleration for drawing letter vector path. Prerequirement is that BOARD_CS defined
 *  and both ENABLE_VGU_ENGINE and CAPABILITY_HWDRAW_PATH is 1.
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
#define CAPABILITY_HW_JPEG_DECODE                1
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
 * @brief Hardware acceleration for HSV.
 */
#define CAPABILITY_HW_HSV             1

/**
 * @brief Hardware acceleration for rain.
 */
#define CAPABILITY_HW_RAIN              1

/**
 * @brief ENABLE GMMU MEM. Prerequimrent is BOARD_CS defined.
 */
#define ENABLE_GMMU                              0
/**
 * @brief specify buffer mode when use single buffer.
 *        1 means compressed buffer.
 *        0 means uncompressed buffer.
 */
#define BUFFER_COMPRESS                          0
/**
 * @brief Some of hardware acceleration operations requires an aligned rect.
 *        If this prerequisite is not meet, it could cause extra data been handled.
 */
#define HW_RECT_ALIGN_RESTRICTION                0
/**
 * @brief Default path for compressed img bin file.
 *        VER1 means that img bin file is for CS version.
 *        VER0 means that img bin file is for ES version.
 */
#define CMP_RES_PATH                             "/bin/vs/sd0p0/pic/VER1_"
/**
 * @brief Default path for recent app bin file.
 */
#ifdef RELEASE_VERSION
#define RENCENT_APP_PATH           "res/recent/"
#else
#define RENCENT_APP_PATH           "../../../wearable/res/recent/"
#endif
/**
 * @brief Support ultra low power
 */
#define ENABLE_ULPS                              0
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

// if ENABLE_OPENVG is set to be 1, dislpay_vau.c should be compiled.
#define ENABLE_OPENVG 0
#if ENABLE_OPENVG
#define BLIT_COUNT_MAX 1
#else
#define BLIT_COUNT_MAX 20
#endif

#define ALIGN_UP(x, a) ((((x) + ((a) - 1)) / (a)) * (a))
#define HARDWARE_ACC_SIZE_LIMIT (50 * 50)
/**
 * @brief Use double buffer for rendering process.
 */
#define DOUBLE_BUFFER                     0
#define MEM_POOL_UI_FONT (uintptr_t)nullptr
#if PSRAM_DENSITY_MB == 16
#define MEM_POOL_UI_FONT_SIZE  0x100000
#else
#define MEM_POOL_UI_FONT_SIZE  0x380000
#endif
#define LAYER_BPP                         24
#define RESOLUTION_WIDTH                  454
#define RESOLUTION_HEIGHT                 454
#define DRAW_MAX_CNT                      48
#define STROKE_MAX_CNT                    48
#ifdef XTS_SUPPORT
#define RTC_EVENT_THREAD_STACK_SIZE             0x1000u
#else
#define RTC_EVENT_THREAD_STACK_SIZE             0x800u
#endif
#define GUI_MAIN_THREAD_STACK_SIZE              0x6000u
#endif // GRAPHIC_HARDWARE_CONFIG_H

/**
 * @brief Based on the HW_ROTATE_ANGLE capability,
 *        the rotation during refresh is replaced with rotation during drawing.
 */
#ifndef ENABLE_PREROTATE_CAPABILITY
#define ENABLE_PREROTATE_CAPABILITY         0
#endif

/**
 * @brief Display resolution and the clockwise rotation angle of the screen resolution.
 *        Needs to be configured at 90-degree multiples.
 */
#ifndef HW_ROTATE_ANGLE
#define HW_ROTATE_ANGLE                     0
#endif

#ifndef GRAPHIC_ROTATE_ANGLE_MULTIPLE
#define GRAPHIC_BUFFER_ROTATE_0 0
#define GRAPHIC_BUFFER_ROTATE_90 1
#define GRAPHIC_BUFFER_ROTATE_180 2
#define GRAPHIC_BUFFER_ROTATE_270 3
#define GRAPHIC_BUFFER_ROTATE_MAGIC 0xafa
#endif
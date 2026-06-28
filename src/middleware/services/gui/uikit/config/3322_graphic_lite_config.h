/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: wearable graphic config
 * Author: Hisi Graphic Team
 * Created: 2025-07
 */
/**
 * @addtogroup Graphic
 * @{
 *
 * @brief Defines a lightweight graphics system that provides basic UI and container views,
 *        including buttons, images, labels, lists, animators, scroll views, swipe views, and layouts.
 *        This system also provides the Design for X (DFX) capability to implement features such as
 *        view rendering, animation, and input event distribution.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file graphic_config.h
 *
 * @brief Provides configuration items required for graphics.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_3322_LITE_GRAPHIC_CONFIG_H
#define GRAPHIC_3322_LITE_GRAPHIC_CONFIG_H
#include "gfx_utils/graphic_types.h"
#include "graphic_hardware_config.h"

namespace OHOS {
#define VERSION_IOT

/* Defines some configurations and their default values on LiteOS and other system platforms. */
#ifndef ENABLE_FIXED_POINT
#define ENABLE_FIXED_POINT                    1
#endif

/**
 * @brief Multi-Font type, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_MULTI_FONT
#define ENABLE_MULTI_FONT                     1
#endif

/**
 * @brief checkBox iamge, which default value = 1 means use the default image,
 * if haven't use checkBox image could set value = 0 to reduce the build resources.
 */
#ifndef ENABLE_DEFAULT_CHECKBOX_IMAGE
#define ENABLE_DEFAULT_CHECKBOX_IMAGE         1
#endif

/**
 * @brief For vector type font, enable different font size or font family for label.
 */
#ifndef ENABLE_SPANNABLE_STRING
#define ENABLE_SPANNABLE_STRING               0
#endif

/* Defines some configurations and their default values on LiteOS and other system platforms. */
/**
 * @brief Dfx cmds, which is enabled by default.
 */
#ifndef ENABLE_DFX_CMD
#define ENABLE_DFX_CMD                    1
#endif
#if ENABLE_DFX_CMD && defined(__LITEOS_M__)
/**
 * @brief Dfx traceui cmds, used for visualizing performance analysis
 */
#define ENABLE_DFX_TRACEUI                1
#endif

/**
 * @brief Default animation for scroll bar/button etc.
 */
#ifndef DEFAULT_ANIMATION
#define DEFAULT_ANIMATION                 0
#endif
 /**
  * @brief draw view on fb firstly, and then get its bitmap copied from fb
 */
#ifndef ENABLE_GET_BITMAP_BASED_ON_FB
#define ENABLE_GET_BITMAP_BASED_ON_FB        1
#endif
 /**
  * @brief use map buffer to support view transformation. if disable it to save memory,
  *        transformation will not be supported except UIImageView, UITextureMapper and UICanvasExt.
  */
#ifndef ENABLE_MAP_BUFFER
#define ENABLE_MAP_BUFFER                    0
#endif
/**
 * @brief Font shaping, which is disabled by default on LiteOS.
 *        Cannot take effect without ENABLE_ICU.
 */
#ifndef ENABLE_SHAPING
#define ENABLE_SHAPING                    1
#endif
/**
 * @brief Advanced algorithm for line breaks, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_ICU
#define ENABLE_ICU                        1
#endif
/**
 * @brief Multi-window, which does not take effect on LiteOS.
 */
#ifndef ENABLE_WINDOW
#define ENABLE_WINDOW                     0
#endif
/**
 * @brief Display buffer for rendering data refresh, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_FRAME_BUFFER
#define ENABLE_FRAME_BUFFER               1
#endif
/**
 * @brief Vector type font, which is enabled by default on LiteOS.
 */
#ifndef ENABLE_VECTOR_FONT
#define ENABLE_VECTOR_FONT                1
#endif
/**
 * @brief Vector type font by stb_truetype, which is enabled by default on LiteOS.
 */
#ifndef ENABLE_VECTOR_FONT_STB
#define ENABLE_VECTOR_FONT_STB            1
#endif
/**
 * @brief Bitmap type font, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_BITMAP_FONT
#define ENABLE_BITMAP_FONT                0
#endif
/**
 * @brief Static Bitmap type font, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_STATIC_FONT
#define ENABLE_STATIC_FONT                0
#endif

/**
 * @brief Use bitmap font for TextId multi language convert, use vector font for glyphs.
 *        Normally you do not need to enable this macro
 */
#ifndef ENABLE_MIX_FONT
#define ENABLE_MIX_FONT                0
#endif

/**
 * @brief JPEG and PNG, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_JPEG
#define ENABLE_JPEG                       0
#endif
#ifndef ENABLE_PNG
#define ENABLE_PNG                        0
#endif
#ifndef ENABLE_JPEG_AND_PNG
#if defined(FT_CONFIG_OPTION_USE_PNG) && defined(FT_CONFIG_OPTION_SYSTEM_ZLIB)
#define ENABLE_JPEG_AND_PNG               1
#else
#define ENABLE_JPEG_AND_PNG               0
#endif
#endif
/**
 * @brief Cache the decoded content for Jpeg and Png image
 */
#ifndef CACHE_JPEG_AND_PNG
#if defined(FT_CONFIG_OPTION_USE_PNG) && defined(FT_CONFIG_OPTION_SYSTEM_ZLIB)
#define CACHE_JPEG_AND_PNG                1
#else
#define CACHE_JPEG_AND_PNG               0
#endif
#endif
/**
 * @brief Customize the memory area when loading image resources
 */
#ifndef IMG_CACHE_MEMORY_CUSTOM
#define IMG_CACHE_MEMORY_CUSTOM           1
#endif
/**
 * @brief GIF, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_GIF
#define ENABLE_GIF                        0
#endif
/**
 * @brief Fully rectangle render everytime, which is disabled by default on LiteOS.
 */
#ifndef FULLY_RENDER
#define FULLY_RENDER                     0
#endif
/**
 * @brief Graphics rendering hardware acceleration, which is enabled by default on LiteOS.
 */
#ifndef ENABLE_HARDWARE_ACCELERATION
#define ENABLE_HARDWARE_ACCELERATION      0
#endif
/**
 * @brief Graphics rendering hardware acceleration for text, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_HARDWARE_ACCELERATION_FOR_TEXT
#define ENABLE_HARDWARE_ACCELERATION_FOR_TEXT      0
#endif
/**
 * @brief Graphics rendering accelerated by gfx_engines, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_GFX_ENGINES
#define ENABLE_GFX_ENGINES                0
#endif
/**
 * @brief Graphics rendering accelerated by VGUEngine, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_VGU_ENGINE
#define ENABLE_VGU_ENGINE                 1
#endif
/**
 * @brief ARM NEON ability, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_ARM_NEON
#define ENABLE_ARM_NEON                   0
#endif
/**
 * @brief vector font global ability, which is disabled by default on LiteOS.
 */
#define ENABLE_FONT_VECTOR_GLOBAL                   1
/**
 * @brief image bin ability, which is disabled by default on LiteOS.
 */
#define ENABLE_IMAGE_PACKER                   1

#ifndef ENABLE_ROTATE_INPUT
#define ENABLE_ROTATE_INPUT               1
#endif

#ifndef ENABLE_VIBRATOR
#define ENABLE_VIBRATOR                   1
#endif

#ifndef ENABLE_FOCUS_MANAGER
#define ENABLE_FOCUS_MANAGER              1
#endif

/**
 * @brief Graphics local rendering, which is disabled by default.
 */
#ifndef LOCAL_RENDER
#define LOCAL_RENDER                      0
#endif
/**
 * @brief Enable SoftKeyboard, which is enabled by default.
 */
#ifndef ENABLE_SOFT_KEYBOARD
#define ENABLE_SOFT_KEYBOARD              1
#endif
/**
 * @brief Actually use ARM NEON optimization.
 *        __ARM_NEON__ and __ARM_NEON are set by the compiler according to the compilation option -mfpu=neon
 */
#if (defined(__ARM_NEON__) || defined(__ARM_NEON)) && ENABLE_ARM_NEON == 1
#define ARM_NEON_OPT
#endif
/**
 * @brief Enable UISweepClock widget, which is denabled by default.
 */
#ifndef ENABLE_SWEEP_CLOCK
#define ENABLE_SWEEP_CLOCK                1
#endif
/**
 * @brief Do not support CacheManager, which is disabled by default.
 */
#ifndef NO_CACHE_MANAGER
#define NO_CACHE_MANAGER                  0
#endif
/**
 * @brief Graphics bottom-layer RGBA, which is enabled by default.
 */
#ifndef ENABLE_BUFFER_RGBA
#define ENABLE_BUFFER_RGBA                1
#endif
/**
 * @brief Debug mode, which is disabled by default.
 */
#ifndef ENABLE_DEBUG
#define ENABLE_DEBUG                      1
#endif
/**
 * @brief Memory check, which is enabled by default. Statistics system dynamic memory after it is enabled.
 */
#ifndef ENABLE_MEMORY_CHECK
#define ENABLE_MEMORY_CHECK               1
#endif
/**
 * @brief Memory hook, which is enabled by default. The system memory allocation is taken over after it is enabled.
 */
#ifndef ENABLE_MEMORY_HOOKS
#define ENABLE_MEMORY_HOOKS               1
#endif
/**
 * @brief Function for monitoring the image refresh frame rate, which is disabled by default.
 */
#ifndef ENABLE_FPS_SUPPORT
#define ENABLE_FPS_SUPPORT                0
#endif
/**
 * @brief Anti-aliasing, which is enabled by default.
 */
#ifndef ENABLE_ANTIALIAS
#define ENABLE_ANTIALIAS                  1
#endif
/**
 * @brief Rectangle anti-aliasing, which is disabled by default.
 */
#ifndef ENABLE_RECT_ANTIALIAS
#define ENABLE_RECT_ANTIALIAS             0
#endif
/**
 * @brief Font color mode, which is disabled by default.
 * After it is enabled, the font color mode is set to <b>4</b> to accelerate font rendering.
 */
#ifndef ENABLE_SPEC_FONT
#define ENABLE_SPEC_FONT                  0
#endif
/**
 * @brief Log function of a graphics subsystem, which is enabled by default
 */
#ifndef ENABLE_GRAPHIC_LOG
#define ENABLE_GRAPHIC_LOG                1
#endif
/**
 * @brief Performance tracking for debugging, which is disabled by default.
 */
#ifndef ENABLE_DEBUG_PERFORMANCE_TRACE
#define ENABLE_DEBUG_PERFORMANCE_TRACE    0
#endif
/**
 * @brief Function for receiving input events in screen-off mode, which is disabled by default.
 */
#ifndef ENABLE_AOD
#define ENABLE_AOD                        0
#endif

/**
 * @brief Defines the log level. A smaller value indicates a higher priority.
 * Logs whose priorities are higher than a specified level can be recorded.
 * Log levels:
 * NONE: disabling logs
 * FATAL: fatal level
 * ERROR: error level
 * WARN: warning level
 * INFO: info level
 * DEBUG: debugging level
 */
#ifndef GRAPHIC_LOG_LEVEL
#define GRAPHIC_LOG_LEVEL                 2
#endif
/**
 * @brief Defines the color depth of graphics rendering. The default value is <b>32</b> bits.
 * The value can be <b>16</b> or <b>32</b>.
 */
#ifndef COLOR_DEPTH
#define COLOR_DEPTH 32
#endif

#ifndef GRAPHIC_ENABLE_LINECAP_FLAG
#define GRAPHIC_ENABLE_LINECAP_FLAG    1
#endif

#ifndef GRAPHIC_ENABLE_LINEJOIN_FLAG
#define GRAPHIC_ENABLE_LINEJOIN_FLAG    1
#endif

#ifndef GRAPHIC_ENABLE_DASH_GENERATE_FLAG
#define GRAPHIC_ENABLE_DASH_GENERATE_FLAG    1
#endif

#ifndef GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
#define GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG    1
#endif

#ifndef GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
#define GRAPHIC_ENABLE_GRADIENT_FILL_FLAG    1
#endif

#ifndef GRAPHIC_ENABLE_PATTERN_FILL_FLAG
#define GRAPHIC_ENABLE_PATTERN_FILL_FLAG    1
#endif

#ifndef GRAPHIC_ENABLE_DRAW_TEXT_FLAG
#define GRAPHIC_ENABLE_DRAW_TEXT_FLAG    1
#endif

#ifndef GRAPHIC_ENABLE_DRAW_IMAGE_FLAG
#define GRAPHIC_ENABLE_DRAW_IMAGE_FLAG    1
#endif

/**
 * @brief Represents the code number of the layer pixel format.
 * 0:     LAYER_PF_ARGB1555
 * 1:     LAYER_PF_ARGB8888
 * 2:     LAYER_PF_RGB888
 * Other: LAYER_PF_ARGB8888
 */
#ifndef LAYER_PF_CODE
#define LAYER_PF_CODE                     2
#endif

/**
 * @brief Defines whether the C library math function is supported.
 * 0:    Unsupport
 * 1:    Support
  */
#ifndef ENABLE_CMATH
#define ENABLE_CMATH              1
#endif

/**
 * @brief Defines the file name of default vector font.
 */
#define DEFAULT_VECTOR_FONT_FILENAME      "HarmonyOS_Sans_SC_Regular.ttf"
#define BOLD_VECTOR_FONT_FILENAME         "HarmonyOS_Sans_SC_Bold.ttf"
#define NOTO_SANS_THAI_REGULAR     "NotoSansThai-Regular.ttf"
#define HARMONYOS_SANS_NASKH_ARABIC_LIGHT    "HarmonyOS_Sans_Naskh_Arabic_Light.ttf"
#define NOTO_SANS_BENGALI_REGULAR     "NotoSansBengali-Regular.ttf"
#define NOTO_SANS_KR_REGULAR     "NotoSansKR-Regular.ttf"
#define NOTO_SANS_REGULAR     "NotoSans-Regular.ttf"
/* resources path. */
#define RES_PATH "/user/res/"
#define CMP_RES_PATH_CS "/user/res/CS_"
#define CMP_RES_PATH_ES "/user/res/ES_"
/* Defines the file name of default line break rule. */
#ifndef DEFAULT_LINE_BREAK_RULE_FILENAME
#define DEFAULT_LINE_BREAK_RULE_FILENAME   "line_cj.brk"
#endif

#ifndef DEFAULT_MAX_CACHE_ENTRY_NUM
#define DEFAULT_MAX_CACHE_ENTRY_NUM                     64
#endif // DEFAULT_MAX_CACHE_ENTRY_NUM

/* Defines some configurations and their default values on LiteOS and other system platforms. */
/* Resolution width of a graphics display screen. The default value is <b>454</b>. */
#define GRAPHIC_BUFFER_ROTATE_0 0
#define GRAPHIC_BUFFER_ROTATE_90 1
#define GRAPHIC_BUFFER_ROTATE_180 2
#define GRAPHIC_BUFFER_ROTATE_270 3
#define GRAPHIC_BUFFER_ROTATE_MAGIC 0xafa

#ifndef HORIZONTAL_RESOLUTION
#if (HW_ROTATE_ANGLE == 0) || (HW_ROTATE_ANGLE == 2)
#define HORIZONTAL_RESOLUTION        RESOLUTION_WIDTH
#else
#define HORIZONTAL_RESOLUTION        RESOLUTION_HEIGHT
#endif
#endif // HORIZONTAL_RESOLUTION

/* Resolution height of a graphics display screen. The default value is <b>454</b>. */
#ifndef VERTICAL_RESOLUTION
#if (HW_ROTATE_ANGLE == 0) || (HW_ROTATE_ANGLE == 2)
#define VERTICAL_RESOLUTION         RESOLUTION_HEIGHT
#else
#define VERTICAL_RESOLUTION         RESOLUTION_WIDTH
#endif
#endif // VERTICAL_RESOLUTION

/**
 * @brief When using the rotation drawing capability,
 *        the layer's resolution needs to match the screen resolution.
 */
#define LAYER_RESOLUTION_WIDTH      RESOLUTION_WIDTH
#define LAYER_RESOLUTION_HEIGHT     RESOLUTION_HEIGHT

/* Defines some configurations and their default values on LiteOS and other system platforms. */
/* Default file path for font */
#ifndef VECTOR_FONT_DIR
#define VECTOR_FONT_DIR                                 "/user/res/"
#endif

/* Maximum number of cached images. The default value is <b>64</b>. */
#ifndef IMG_CACHE_SIZE
#define IMG_CACHE_SIZE                                  64
#endif
} // namespace OHOS
#endif // GRAPHIC_LITE_GRAPHIC_CONFIG_H

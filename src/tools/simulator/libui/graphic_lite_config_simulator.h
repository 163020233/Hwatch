/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd..
 * Description: simulator file
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

#ifndef GRAPHIC_LITE_GRAPHIC_CONFIG_SIMULATOR_H
#define GRAPHIC_LITE_GRAPHIC_CONFIG_SIMULATOR_H

#include "gfx_utils/graphic_types.h"
#include "graphic_hardware_config.h"

namespace OHOS {
/**
 * Defines three graphics library versions: lightweight, standard, and extended versions.
 * The three versions have different requirements on the memory and hardware.
 * The standard version is enabled by default.
 *
 * The macros of the versions are defined as follows:
 * Name                | Version Description
 * ------------------- | ----------
 * VERSION_LITE        | Lightweight version
 * VERSION_STANDARD    | Standard version
 * VERSION_EXTENDED    | Extended version
 */
#define ENABLE_FIXED_POINT                    1
/* Defines some configurations and their default values on LiteOS and other system platforms. */
#ifdef VERSION_IOT
/**
 * @brief Dfx cmds, which is enabled by default.
 */
#define ENABLE_DFX_CMD                    0
/**
 * @brief Font shaping, which is disabled by default.
 *        Cannot take effect without ENABLE_ICU.
 */
#define ENABLE_SHAPING                    1
/**
 * @brief Advanced algorithm for line breaks, which is disabled by default.
 */
#define ENABLE_ICU                        1
/**
 * @brief Multi-Font type, which is disabled by default.
 */
#define ENABLE_MULTI_FONT                 1
/**
 * @brief Multi-window, which does not take effect.
 */
#define ENABLE_WINDOW                     0
/**
 * @brief Display buffer for rendering data refresh, which is disabled by default.
 */
#define ENABLE_FRAME_BUFFER               1
/**
 * @brief Vector type font, which is enabled by default.
 */
#define ENABLE_VECTOR_FONT               1
/**
 * @brief Bitmap type font, which is enabled by default.
 */
#define ENABLE_BITMAP_FONT                0
/**
 * @brief Static Bitmap type font, which is disabled by default.
 */
#define ENABLE_STATIC_FONT                0

#define ENABLE_JPEG_AND_PNG               1
/**
 * @brief Cache the decoded content for Jpeg and Png image
 */
#define CACHE_JPEG_AND_PNG                1
/**
 * @brief Customize the memory area when loading image resources
 */
#define IMG_CACHE_MEMORY_CUSTOM           1
/**
 * @brief Graphics rendering hardware acceleration, which is disabled by default.
 */
#define ENABLE_HARDWARE_ACCELERATION      0
/**
 * @brief Graphics rendering hardware acceleration for text, which is disabled by default.
 */
#define ENABLE_HARDWARE_ACCELERATION_FOR_TEXT      0
/**
 * @brief Graphics rendering accelerated by gfx_engines, which is disabled by default.
 */
#define ENABLE_GFX_ENGINES                0
/**
 * @brief Graphics rendering accelerated by VGUEngine, which is enabled by default.
 */
#define ENABLE_VGU_ENGINE                 1

/**
 * @brief ARM NEON ability, which is disabled by default.
 */
#define ENABLE_ARM_NEON                   0
/**
 * @brief vector font global ability, which is disabled by default on LiteOS.
 */
#define ENABLE_FONT_VECTOR_GLOBAL              1
 /**
  * @brief image bin ability, which is disabled by default on LiteOS.
 */
#define ENABLE_IMAGE_PACKER                   1
 /**
  * @brief draw view on fb firstly, and then get its bitmap copied from fb
 */
#define ENABLE_GET_BITMAP_BASED_ON_FB         0
#endif

 /**
  * @brief use map buffer to support view transformation. if disable it to save memory,
  *        transformation will not be supported except UIImageView, UITextureMapper and UICanvasExt.
  */
#define ENABLE_MAP_BUFFER                    0

#define ENABLE_ROTATE_INPUT               1

#define ENABLE_VIBRATOR                   1

#define ENABLE_FOCUS_MANAGER              1
/**
 * @brief Graphics local rendering, which is enabled by default.
 */
#define LOCAL_RENDER                      0
/**
 * @brief Fully rectangle render everytime, which is disabled by default.
 */
#define FULLY_RENDER                      0

/**
 * @brief Enable SoftKeyboard, which is enabled by default.
 */
#define ENABLE_SOFT_KEYBOARD              1

/**
 * @brief Enable UISweepClock widget, which is denabled by default.
 */
#define ENABLE_SWEEP_CLOCK                1

/**
 * @brief Do not support CacheManager, which is disabled by default.
 */
#define NO_CACHE_MANAGER                  0
/**
 * @brief Graphics bottom-layer RGBA, which is enabled by default.
 */
#define ENABLE_BUFFER_RGBA                1
/**
 * @brief Debug mode, which is disabled by default.
 */
#define ENABLE_DEBUG                      1
/**
 * @brief Memory check, which is enabled by default. Statistics system dynamic memory after it is enabled.
 */
#define ENABLE_MEMORY_CHECK               1
/**
 * @brief Memory hook, which is enabled by default. The system memory allocation is taken over after it is enabled.
 */
#define ENABLE_MEMORY_HOOKS               1
/**
 * @brief Function for monitoring the image refresh frame rate, which is disabled by default.
 */
#define ENABLE_FPS_SUPPORT                0
/**
 * @brief Anti-aliasing, which is enabled by default.
 */
#define ENABLE_ANTIALIAS                  1
/**
 * @brief Rectangle anti-aliasing, which is disabled by default.
 */
#define ENABLE_RECT_ANTIALIAS             0
/**
 * @brief Font color mode, which is disabled by default.
 * After it is enabled, the font color mode is set to <b>4</b> to accelerate font rendering.
 */
#define ENABLE_SPEC_FONT                  0
/**
 * @brief Log function of a graphics subsystem, which is enabled by default
 */
#define ENABLE_GRAPHIC_LOG                1
/**
 * @brief Performance tracking for debugging, which is disabled by default.
 */
#define ENABLE_DEBUG_PERFORMANCE_TRACE    0
/**
 * @brief Function for receiving input events in screen-off mode, which is disabled by default.
 */
#define ENABLE_AOD                        0
/**
 * @brief Defines the log level. A smaller value indicates a higher priority.
 * Logs whose priorities are higher than a specified level can be recorded.
 * Log levels:
 * FATAL: fatal level 0
 * ERROR: error level 1
 * WARN: warning level 2
 * INFO: info level 3
 * DEBUG: debugging level 4
 */
#define GRAPHIC_LOG_LEVEL                 1
/**
 * @brief Defines the color depth of graphics rendering. The default value is <b>32</b> bits.
 * The value can be <b>16</b> or <b>32</b>.
 */
#define COLOR_DEPTH 32

/**
 * @brief Represents the code number of the layer pixel format.
 * 0:     LAYER_PF_ARGB1555
 * 1:     LAYER_PF_ARGB8888
 * Other: LAYER_PF_ARGB8888
 */
#define LAYER_PF_CODE                     2

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

#if LAYER_PF_CODE == 0
#define LAYER_PF_ARGB1555
#elif LAYER_PF_CODE == 1
#define LAYER_PF_ARGB8888
#elif LAYER_PF_CODE == 2
#define LAYER_PF_RGB888
#else
#define LAYER_PF_ARGB8888
#endif

/**
 * @brief Defines whether the C library math function is supported.
 * 0:    Unsupport
 * 1:    Support
  */
#define ENABLE_CMATH              1

/**
 * @brief Defines the file name of default vector font.
 */

#define DEFAULT_VECTOR_FONT_FILENAME      "HarmonyOS_Sans_SC_Regular.ttf"
#define BOLD_VECTOR_FONT_FILENAME         "HarmonyOS_Sans_SC_Bold.ttf"


/* resources path. */
#ifdef RELEASE_VERSION
#define RES_PATH "res/res/"
#define CMP_RES_PATH_CS "res/res/CS_"
#define CMP_RES_PATH_ES "res/res/ES_"
static constexpr const char* BACKGROUND_CIRCLE_RES_PATH = "res/res/backGround.bin";
static constexpr const char* BACKGROUND_RECT_RES_PATH = "res/res/backGroundRect.bin";
#else
#if !defined(_WIN32)
#define RES_PATH "../../wearable/res/res/"
#define CMP_RES_PATH_CS "../../wearable/res/res/CS_"
#define CMP_RES_PATH_ES "../../wearable/res/res/ES_"
static constexpr const char* BACKGROUND_CIRCLE_RES_PATH = "../../wearable/res/res/backGround.bin";
static constexpr const char* BACKGROUND_RECT_RES_PATH = "../../wearable/res/res/backGroundRect.bin";
#else
#define RES_PATH "../../../wearable/res/res/"
#define CMP_RES_PATH_CS "../../../wearable/res/res/CS_"
#define CMP_RES_PATH_ES "../../../wearable/res/res/ES_"
static constexpr const char* BACKGROUND_CIRCLE_RES_PATH = "../../../wearable/res/res/backGround.bin";
static constexpr const char* BACKGROUND_RECT_RES_PATH = "../../../wearable/res/res/backGroundRect.bin";
#endif
#endif

/* Defines the file name of default line break rule. */
#ifndef DEFAULT_LINE_BREAK_RULE_FILENAME
#define DEFAULT_LINE_BREAK_RULE_FILENAME   "line_cj.brk"
#endif

#ifndef DEFAULT_MAX_CACHE_ENTRY_NUM
#define DEFAULT_MAX_CACHE_ENTRY_NUM                     64
#endif // DEFAULT_MAX_CACHE_ENTRY_NUM

/* Defines some configurations and their default values on LiteOS and other system platforms. */
/* Resolution width of a graphics display screen. The default value is <b>454</b>. */
#ifndef HORIZONTAL_RESOLUTION
#if HW_ROTATE_ANGLE == 0 || HW_ROTATE_ANGLE == 2
#define HORIZONTAL_RESOLUTION        RESOLUTION_WIDTH
#else
#define HORIZONTAL_RESOLUTION        RESOLUTION_HEIGHT
#endif
#endif // HORIZONTAL_RESOLUTION

/* Resolution height of a graphics display screen. The default value is <b>454</b>. */
#ifndef VERTICAL_RESOLUTION
#if HW_ROTATE_ANGLE == 0 || HW_ROTATE_ANGLE == 2
#define VERTICAL_RESOLUTION         RESOLUTION_HEIGHT
#else
#define VERTICAL_RESOLUTION         RESOLUTION_WIDTH
#endif
#endif // VERTICAL_RESOLUTION
/* Default device shape. The default value is 1:SHAPE_CIRCLE. */
static constexpr int16_t DEVICE_SHAPE = 1;

/* Default file path for font */
#ifndef VECTOR_FONT_DIR
#ifdef RELEASE_VERSION
#define VECTOR_FONT_DIR             "res/res/"
#else
#define VECTOR_FONT_DIR             "../../../wearable/res/res/"
#endif
#endif
} // namespace OHOS
#endif // GRAPHIC_LITE_GRAPHIC_CONFIG_H

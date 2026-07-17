/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 */
/* *
 * @file sio_porting.h
 * @brief 3322 porting stub for the Arduino I2SClass + 3322 i2s.h.
 *
 * 3322 has no SIO driver: sio_porting.h / hal_sio.h don't exist, and 3322's
 * i2s.h references sio_bus_t / CONFIG_DATA_LEN_MAX / SIO_BUS_0 without defining
 * them (the SIO layer that normally provides these is absent). This stub defines
 * them so i2s.h parses and I2SClass.cpp compiles.
 *
 * Force-included via -include in arduino_pre.cmake so it lands BEFORE i2s.h.
 * CONFIG_I2S_SUPPORT stays undefined on 3322 → I2SClass.cpp compiles to its stub
 * path. Non-functional I2S until a real SIO/I2S porting is added.
 */

#ifndef SIO_PORTING_H
#define SIO_PORTING_H

#include <stdint.h>

/* sio_bus_t: ws63 defines this as an enum in platform_core.h; 3322 has none.
 * uint32_t is sufficient for the stub (SIO_BUS_0 = 0). */
typedef uint32_t sio_bus_t;

/* CONFIG_DATA_LEN_MAX: ws63 defines 128 (menuconfig / sio_porting.h); 3322's
 * i2s.h uses it for buffer sizing. Provide the ws63 value. */
#ifndef CONFIG_DATA_LEN_MAX
#define CONFIG_DATA_LEN_MAX 128
#endif

/* SIO_BUS_0: only referenced in CONFIG_I2S_SUPPORT-gated code (excluded on
 * 3322). Defined for completeness. */
#define SIO_BUS_0 0

/* i2s_sample_rate_t / I2S_SAMPLE_RATE_*: ws63's i2s.h uses these but defines
 * them in its (absent on 3322) SIO driver headers. 3322's i2s.h references
 * i2s_sample_rate_t without defining it. Provide the enum so i2s.h parses and
 * I2SClass.cpp's ungated map_sample_rate() compiles. Values are arbitrary
 * (map_sample_rate's returns are unused — CONFIG_I2S_SUPPORT is undefined). */
typedef enum {
    I2S_SAMPLE_RATE_8K = 0,
    I2S_SAMPLE_RATE_11K,
    I2S_SAMPLE_RATE_12K,
    I2S_SAMPLE_RATE_16K,
    I2S_SAMPLE_RATE_22K,
    I2S_SAMPLE_RATE_24K,
    I2S_SAMPLE_RATE_32K,
    I2S_SAMPLE_RATE_44K,
    I2S_SAMPLE_RATE_48K,
    I2S_SAMPLE_RATE_88K,
    I2S_SAMPLE_RATE_96K,
    I2S_SAMPLE_RATE_176K,
    I2S_SAMPLE_RATE_192K
} i2s_sample_rate_t;

#endif /* SIO_PORTING_H */

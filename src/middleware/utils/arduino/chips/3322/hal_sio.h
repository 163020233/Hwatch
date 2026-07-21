/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 */
/* *
 * @file hal_sio.h
 * @brief 3322 porting stub for the Arduino I2SClass.
 *
 * 3322 has no SIO driver (no hal_sio.h / sio_porting.h). I2SClass.cpp includes
 * hal_sio.h and its ungated map_data_width()/map_channels() helpers return
 * hal_sio_data_width_t / hal_sio_channel_number_t. This stub provides those two
 * enum types so I2SClass.cpp compiles. The real I2S code paths are excluded
 * (CONFIG_I2S_SUPPORT undefined on 3322), so this is a non-functional stub —
 * I2S is not supported on 3322 until a real SIO/I2S porting is added.
 */

#ifndef HAL_SIO_H
#define HAL_SIO_H

#include <stdint.h>

typedef enum {
    SIXTEEN_BIT,
    EIGHTEEN_BIT,
    TWENTY_BIT,
    TWENTY_FOUR_BIT,
    THIRTY_TWO_BIT
} hal_sio_data_width_t;

typedef enum {
    TWO_CH,
    FOUR_CH,
    EIGHT_CH,
    SIXTEEN_CH
} hal_sio_channel_number_t;

#endif /* HAL_SIO_H */

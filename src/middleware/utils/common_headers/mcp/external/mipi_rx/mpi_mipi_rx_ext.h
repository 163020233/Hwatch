/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: define for external use
 * Author: AuthorNameMagicTag
 * Create: 2021-09-23
 */

#ifndef __MPI_MIPI_RX_EXT_H__
#define __MPI_MIPI_RX_EXT_H__

#include "drv_ioctl_mipi_rx.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    ext_mipi_rx_input_mode input_mode; /* input mode: MIPI/SUBLVDS/HISPI */
    ext_mipi_rx_data_rate data_rate;
    ext_mipi_rx_img_rect img_rect; /* MIPI Rx device crop area  */
    ext_mipi_rx_lane_mode lane_mode;
    union {
        ext_mipi_dev_attr mipi_attr;
        ext_lvds_dev_attr lvds_attr; /* SUBLVDS/HISPI */
    } u;
} ext_mipi_rx_attr;

td_s32 ext_mpi_mipi_rx_init(td_void);
td_s32 ext_mpi_mipi_rx_deinit(td_void);
td_s32 ext_mpi_mipi_rx_set_dev_attr(const td_u32 dev, const ext_mipi_rx_attr *attr);
td_s32 ext_mpi_mipi_rx_reset_sensor(const td_u32 sns_id, td_bool enable);
td_s32 ext_mpi_mipi_rx_reset_mipi(const td_u32 dev, td_bool enable);
td_s32 ext_mpi_mipi_rx_config_mipi_clock(const td_u32 dev, td_bool enable);
td_s32 ext_mpi_mipi_rx_config_sensor_clock(const td_u32 sns_id, ext_mipi_rx_cis_mclk_attr *mclk);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_MIPI_RX_EXT_H__ */


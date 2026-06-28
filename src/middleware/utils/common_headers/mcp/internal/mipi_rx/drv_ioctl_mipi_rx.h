/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: mipi rx ioctl header
 * Author: AuthorNameMagicTag
 * Create: 2021-09-23
 */

#ifndef __DRV_IOCTL_MIPI_RX_H__
#define __DRV_IOCTL_MIPI_RX_H__

#include "drv_mipi_rx_struct.h"
#include "drv_mcp_dependency.h"

typedef struct {
    td_u32 sns_id;
    ext_mipi_rx_cis_mclk_attr mclk;
} mipirx_ioc_cis_arg;

typedef enum {
    IOC_MIPI_RX_SET_DEV_ATTR,
    IOC_MIPI_RX_RESET_SENSOR,
    IOC_MIPI_RX_UNRESET_SENSOR,
    IOC_MIPI_RX_RESET_MIPI,
    IOC_MIPI_RX_UNRESET_MIPI,
    IOC_MIPI_RX_SET_LANE_MODE,

    IOC_MIPI_RX_ENABLE_MIPI_CLOCK,
    IOC_MIPI_RX_DISABLE_MIPI_CLOCK,
    IOC_MIPI_RX_ENABLE_SENSOR_CLOCK,
    IOC_MIPI_RX_DISABLE_SENSOR_CLOCK,

    IOC_MIPI_RX_MAX
} ioc_mipi_rx;

#define EXT_MIPI_RX_SET_DEV_ATTR         _IOW(SOC_ID_MIPI_RX, IOC_MIPI_RX_SET_DEV_ATTR, ext_mipi_rx_dev_attr)

#define EXT_MIPI_RX_RESET_SENSOR         _IOW(SOC_ID_MIPI_RX, IOC_MIPI_RX_RESET_SENSOR, td_u32)

#define EXT_MIPI_RX_UNRESET_SENSOR       _IOW(SOC_ID_MIPI_RX, IOC_MIPI_RX_UNRESET_SENSOR, td_u32)

#define EXT_MIPI_RX_RESET_MIPI           _IOW(SOC_ID_MIPI_RX, IOC_MIPI_RX_RESET_MIPI, td_u32)

#define EXT_MIPI_RX_UNRESET_MIPI         _IOW(SOC_ID_MIPI_RX, IOC_MIPI_RX_UNRESET_MIPI, td_u32)
#define EXT_MIPI_RX_ENABLE_MIPI_CLOCK    _IOW(SOC_ID_MIPI_RX, IOC_MIPI_RX_ENABLE_MIPI_CLOCK, td_u32)

#define EXT_MIPI_RX_DISABLE_MIPI_CLOCK   _IOW(SOC_ID_MIPI_RX, IOC_MIPI_RX_DISABLE_MIPI_CLOCK, td_u32)

#define EXT_MIPI_RX_ENABLE_SENSOR_CLOCK  _IOW(SOC_ID_MIPI_RX, IOC_MIPI_RX_ENABLE_SENSOR_CLOCK, mipirx_ioc_cis_arg)

#define EXT_MIPI_RX_DISABLE_SENSOR_CLOCK _IOW(SOC_ID_MIPI_RX, IOC_MIPI_RX_DISABLE_SENSOR_CLOCK, mipirx_ioc_cis_arg)

#endif /* __DRV_IOCTL_MIPI_RX_H__ */

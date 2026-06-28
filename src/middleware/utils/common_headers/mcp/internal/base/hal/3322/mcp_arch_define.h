/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2021. All rights reserved.
 * Description: Function of mcp_arch_define.h
 * Author: AuthorNameMagicTag
 * Create: 2012-06-28
 */

#ifndef __MCP_ARCH_DEF_H__
#define __MCP_ARCH_DEF_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* isp sys part */
#define ISP_NORMAL_BLOCK_NUM                      1
#define ISP_VIPROC_IP_NUM                         1
#define ISP_VICAP_IP_NUM                          1

#define ISP_MAX_PHY_PIPE_NUM                      2
#define ISP_MAX_VIR_PIPE_NUM                      0
#define ISP_MAX_PIPE_NUM                          (ISP_MAX_PHY_PIPE_NUM + ISP_MAX_VIR_PIPE_NUM)

#define ISP_WDR_CHN_MAX                           2      /* current chip support max wdr frame mode */

#define SENSOR_RES_WIDTH_MAX                      4608
#define SENSOR_RES_HEIGHT_MAX                     4608
#define ISP_RES_WIDTH_MAX                         4608 /* 3322 no use */
#define ISP_RES_HEIGHT_MAX                        4608 /* 3322 no use */

#define MCP_VIDEV_INSTANCE_MAX     (1)
#define MCP_VICAP_INSTANCE_MAX     (ISP_MAX_PIPE_NUM)
#define MCP_VIPROC_INSTANCE_MAX    (0)
#define EXT_ISP_NRX_AUTO_PARAM_MAX 8

#define VIU_MAX_PHY_CHN_NUM             1
#define VIU_MAX_EXT_CHN_NUM             0
#define VIU_EXT_CHN_START               VIU_MAX_PHY_CHN_NUM
#define VIU_MAX_CHN_NUM                 (VIU_MAX_PHY_CHN_NUM + VIU_MAX_EXT_CHN_NUM)
#define VIU_MAX_DEV_NUM                 1
#define VIU_MAX_SPLIT_NODE_NUM          1U

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif


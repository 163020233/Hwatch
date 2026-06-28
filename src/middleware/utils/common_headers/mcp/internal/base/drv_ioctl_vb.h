/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vb ioctl
 * Author: AuthorNameMagicTag
 * Create: 2021-07-20
 */

#ifndef __DRV_IOCTL_VB_H__
#define __DRV_IOCTL_VB_H__

#include "drv_vb_struct.h"
#include "drv_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VB_DEV_NAME "soc_vb"
#ifdef CONFIG_SOCT_SHAOLINSPEAR_SUPPORT
#define VB_DEV_PATH "/dev/soc_vb"
#else
#define VB_DEV_PATH "/dev/mcp_vb"
#endif

typedef struct {
    td_u32 pool_id;
    td_u64 blk_size;
    td_u32 blk_cnt;
    td_u32 blk_handle;
    td_bool is_comm_pool;
    ext_vb_remap_mode remap_mode;
} vb_ioc_arg;

typedef struct {
    ext_vb_uid    vb_uid;
    ext_vb_config vb_conf;
} vb_ioc_cfg_arg;

typedef struct {
    td_u64 phy_addr;
    ext_vb_mem_info info;
} vb_ioc_mem_arg;

typedef enum {
    IOC_VB_CREATE = 1,
    IOC_VB_DESTROY,
    IOC_VB_GETBLK,
    IOC_VB_RELBLK,
    IOC_VB_PHYS2H,
    IOC_VB_H2PHYS,
    IOC_VB_H2POOL,
    IOC_VB_INITCPOOL,
    IOC_VB_EXITCPOOL,
    IOC_VB_GETCPOOL,
    IOC_VB_SETCPOOL,
    IOC_VB_GETINFO,
    IOC_VB_QRYUSER,
    IOC_VB_INITMODCPOOL,
    IOC_VB_EXITMODCPOOL,
    IOC_VB_GETMODCPOOL,
    IOC_VB_SETMODCPOOL,
    IOC_VB_SETSUPPLEMENT,
    IOC_VB_GETSUPPLEMENT,
    IOC_VB_GETSUPPLEMENTADDR,
    IOC_VB_H2USERCNT,
    IOC_VB_GETMEMINFO,
} ioc_vb;

#define VB_CREATE_CTRL              _IOWR(SOC_ID_VB, IOC_VB_CREATE, vb_ioc_arg)
#define VB_DESTROY_CTRL             _IOWR(SOC_ID_VB, IOC_VB_DESTROY, td_u32)
#define VB_GETBLK_CTRL              _IOWR(SOC_ID_VB, IOC_VB_GETBLK, vb_ioc_arg)
#define VB_RELBLK_CTRL              _IOWR(SOC_ID_VB, IOC_VB_RELBLK, td_u32)

#define VB_PHYS2H_CTRL              _IOWR(SOC_ID_VB, IOC_VB_PHYS2H, td_u64)
#define VB_H2PHYS_CTRL              _IOWR(SOC_ID_VB, IOC_VB_H2PHYS, td_u64)
#define VB_H2POOL_CTRL              _IOWR(SOC_ID_VB, IOC_VB_H2POOL, td_u32)
#define VB_H2USERCNT_CTRL           _IOWR(SOC_ID_VB, IOC_VB_H2USERCNT, td_u32)

#define VB_INIT_POOL_CTRL           _IO(SOC_ID_VB, IOC_VB_INITCPOOL)
#define VB_EXIT_POOL_CTRL           _IO(SOC_ID_VB, IOC_VB_EXITCPOOL)
#define VB_GET_CONF_CTRL            _IOR(SOC_ID_VB, IOC_VB_GETCPOOL, ext_vb_config)
#define VB_SET_CONF_CTRL            _IOW(SOC_ID_VB, IOC_VB_SETCPOOL, ext_vb_config)

#define VB_GET_INFO_CTRL            _IOWR(SOC_ID_VB, IOC_VB_GETINFO, ext_vb_pool_info)
#define VB_QRY_USER_CTRL            _IOWR(SOC_ID_VB, IOC_VB_QRYUSER, td_u32)

#define VB_INIT_MOD_POOL_CTRL       _IOW(SOC_ID_VB, IOC_VB_INITMODCPOOL, ext_vb_uid)
#define VB_EXIT_MOD_POOL_CTRL       _IOW(SOC_ID_VB, IOC_VB_EXITMODCPOOL, ext_vb_uid)
#define VB_GET_MOD_CONF_CTRL        _IOWR(SOC_ID_VB, IOC_VB_GETMODCPOOL, vb_ioc_cfg_arg)
#define VB_SET_MOD_CONF_CTRL        _IOW(SOC_ID_VB, IOC_VB_SETMODCPOOL, vb_ioc_cfg_arg)

#define VB_GET_SUPPLEMENTADDR_CTRL  _IOWR(SOC_ID_VB, IOC_VB_GETSUPPLEMENTADDR, ext_vb_video_supplement_info)
#define VB_SET_SUPPLEMENT_CONF_CTRL _IOW(SOC_ID_VB, IOC_VB_SETSUPPLEMENT, ext_vb_supplement_config)
#define VB_GET_SUPPLEMENT_CONF_CTRL _IOR(SOC_ID_VB, IOC_VB_GETSUPPLEMENT, ext_vb_supplement_config)

#define VB_GET_MEM_INFO_CTRL        _IOWR(SOC_ID_VB, IOC_VB_GETMEMINFO, vb_ioc_mem_arg)

/* user 32bit, kernel 64bit */
#define VB_COMPAT_GET_SUPPLEMENTADDR_CTRL  _IOWR(SOC_ID_VB, IOC_VB_GETSUPPLEMENTADDR, \
                                                 ext_vb_compat_video_supplement_info)

#define VB_COMPAT_GET_INFO_CTRL            _IOWR(SOC_ID_VB, IOC_VB_GETINFO, ext_vb_compat_pool_info)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DRV_IOCTL_VB_H__ */


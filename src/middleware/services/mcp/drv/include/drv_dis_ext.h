/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: dis ext define
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef __DRV_DIS_EXT_H__
#define __DRV_DIS_EXT_H__

#include "drv_dis_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef td_s32 fn_dis_dev_init(td_void);
typedef td_s32 fn_dis_dev_deinit(td_void);
typedef td_s32 fn_dis_create_chn(td_s32 dis_chn);
typedef td_s32 fn_dis_init(td_s32 dis_chn, ext_dis_config *dis_config, ext_dis_sys_config *sys_config);
typedef td_s32 fn_dis_get_dsf_cfg(td_s32 dis_chn, td_void *dis_dsf_cfg);
typedef td_s32 fn_dis_process(td_s32 dis_chn, td_void *ext_dis_task_data, ext_dis_config *dis_config,
    ext_dis_attr *dis_attr, ext_dis_param *dis_param);
typedef td_s32 fn_dis_de_init(td_s32 dis_chn);
typedef td_s32 fn_dis_destroy_chn(td_s32 dis_chn);
typedef td_s32 fn_dis_get_default_dis_param(ext_dis_param *dis_param);

typedef struct {
    fn_dis_dev_init *pfn_dis_dev_init;
    fn_dis_dev_deinit *pfn_dis_dev_deinit;
    fn_dis_process *pfn_dis_process;
    fn_dis_create_chn *pfn_dis_create_chn;
    fn_dis_destroy_chn *pfn_dis_destroy_chn;
    fn_dis_init *pfn_dis_process_init;
    fn_dis_de_init *pfn_dis_process_de_init;
    fn_dis_get_dsf_cfg *pfn_dis_get_dsf_cfg;
    fn_dis_get_default_dis_param *pfn_dis_get_default_dis_param;
} dis_export_func;

td_s32 drv_dis_mod_init(td_void);
td_void drv_dis_mod_exit(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_DIS_EXT_H__ */

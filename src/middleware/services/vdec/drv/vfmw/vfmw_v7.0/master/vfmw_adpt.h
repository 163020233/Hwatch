/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __VFMW_ADPT_H__
#define __VFMW_ADPT_H__

#include "vfmw.h"
#include "vfmw_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    VFMW_STATE_NULL = 0,
    VFMW_STATE_OPEN,
    VFMW_STATE_BUTT
} vfmw_state;

typedef enum {
    VFMW_CHAN_STATE_NULL = 0,
    VFMW_CHAN_STATE_WAIT,
    VFMW_CHAN_STATE_CREATE,
    VFMW_CHAN_STATE_BUTT
} vfmw_chan_state;

typedef enum {
    VFMW_TYPE_LOCAL,
    VFMW_TYPE_TEE,
    VFMW_TYPE_MCU,
    VFMW_TYPE_USER,
    VFMW_TYPE_BUFF
} vfmw_type;

typedef struct {
    td_u32 begin;
    td_u32 end;
    td_u32 cost;
} vfmw_intf_cost;

typedef struct {
    vfmw_intf_cost create;
    vfmw_intf_cost config;
    vfmw_intf_cost start;
    vfmw_intf_cost questm;
    vfmw_intf_cost getfrm;
    vfmw_intf_cost stop;
    vfmw_intf_cost reset;
    vfmw_intf_cost destroy;
} vfmw_kpi;

typedef struct {
    td_u32 save_begin;
    td_u32 save_end;
} vfmw_dflt;

typedef struct {
    td_s32 id;
    td_u32 frm_cnt;
    td_u32 que_cnt;
    td_u32 start_cnt;
    td_u32 stop_cnt;
    td_u32 flush_cnt;
    td_u32 reset_cnt;
    os_sema sema;
    vfmw_chan_state state;
    vfmw_frm_alloc_type alloc_type;
    vfmw_vpp_work_mode work_mode;
    vfmw_adpt_type adpt_type;
    td_handle mod_handle;
    td_handle ssm_handle;
    vfmw_type type;
    td_bool is_sec;
    vfmw_kpi kpi;
    os_file *yuv;
    os_file *mtdt;
} vfmw_chan;

typedef td_void (*fun_vfmw_crc_init)(td_void);
typedef td_void (*fun_vfmw_crc_exit)(td_void);
typedef td_s32 (*fun_vfmw_crc_create_instance)(td_s32 instance_id);
typedef td_s32 (*fun_vfmw_crc_destroy_instance)(td_s32 instance_id);
typedef td_s32 (*fun_vfmw_crc_set_config)(td_s32 instance_id, vfmw_chan_cfg *config);
typedef td_s32 (*fun_vfmw_crc_put_image)(td_s32 instance_id, vfmw_image *image);
typedef td_s32 (*fun_vfmw_crc_init_calc_env)(td_void);
typedef td_s32 (*fun_vfmw_crc_exit_calc_env)(td_void);
typedef struct {
    td_bool is_register;
    fun_vfmw_crc_init crc_init;
    fun_vfmw_crc_exit crc_exit;
    fun_vfmw_crc_create_instance crc_create_instance;
    fun_vfmw_crc_destroy_instance crc_destroy_instance;
    fun_vfmw_crc_set_config crc_set_config;
    fun_vfmw_crc_put_image crc_put_image;
    fun_vfmw_crc_init_calc_env crc_init_calc_env;
    fun_vfmw_crc_exit_calc_env crc_exit_calc_env;
} vfmw_crc_intf;

typedef struct {
    td_u32 reserve1;

    os_sema sema;
    os_sema dbg_sema;
    td_u32 ref_cnt;
    td_u32 chan_cnt;
    td_u32 suspend;
    vfmw_state state;
    vfmw_chan chan[VFMW_CHAN_NUM];
    vfmw_dflt dflt;

    vfmw_intf_cost init_kpi;
    vfmw_intf_cost exit_kpi;
    vfmw_intf_cost susp_kpi;
    vfmw_intf_cost resu_kpi;

    fun_vfmw_event_report event_report_vdec;
    fun_vfmw_event_report event_report_vavdec;

    vfmw_crc_intf crc_intf;

    td_u32 suspend_cnt;
    td_u32 resume_cnt;

    td_u32 reserve2;
} vfmw_entry;

td_s32 vadpt_open(td_void);
td_s32 vadpt_close(td_void);
td_s32 vadpt_init(td_void *args);
td_s32 vadpt_exit(const td_void *args);
td_s32 vadpt_suspend(const td_void *args);
td_s32 vadpt_resume(const td_void *args);
td_s32 vadpt_get_frame(td_s32 chan_id, td_void *frame, td_void *ext_frm_info);
td_s32 vadpt_release_frame(td_s32 chan_id, const td_void *frame);
td_s32 vadpt_control(td_s32 chan_id, td_s32 cmd, td_void *args, td_u32 len);
td_s32 vadpt_register_crc_intf(vfmw_crc_intf *crc_intf);
td_void vadpt_unregister_crc_intf(td_void);

vfmw_entry *vadpt_entry(td_void);
vfmw_chan *vadpt_chan(td_s32 chan_id);
vfmw_dflt *vadpt_get_dflt(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __VFMW_ADPT_H__ */

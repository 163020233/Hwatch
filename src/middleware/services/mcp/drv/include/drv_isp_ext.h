/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2021. All rights reserved.
 * Description: Function of drv_isp_ext.h
 * Author: AuthorNameMagicTag
 * Create: 2012-06-28
 */

#ifndef __DRV_ISP_EXT_H__
#define __DRV_ISP_EXT_H__

#include "drv_mcp_dependency.h"

#include "drv_isp_comm_struct.h"
#include "drv_isp_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define ISP_WRITE_I2C_MAX_REG_NUM        64

#define ISP_SYNC_TASK_ID_MAX_LENGTH   64

typedef enum {
    ISP_REG_BE = 0,
    ISP_REG_STT,
    ISP_REG_MAX,
} isp_reg_type;

typedef enum {
    ISP_SNS_BUS_TYPE_I2C = 0,
    ISP_SNS_BUS_TYPE_SSP,
    ISP_SNS_BUS_TYPE_BUTT,
} isp_sns_bus_type;

typedef struct {
    td_u8  vc_num;
} isp_pro_ctrl;

typedef struct {
#ifndef ISP_WRITE_I2C_THROUGH_MUL_REG
    td_s32 (*pfn_isp_write_i2c_data)(td_u8 i2c_dev, td_u8 dev_addr, td_u32 reg_addr,
        td_u32 reg_addr_byte_num, td_u32 data, td_u32 data_byte_num);
#else
    td_s32 (*pfn_isp_write_i2c_data)(td_u8 i2c_dev, td_u8 dev_addr,
        const td_s8 *data_buf, td_u8 data_len, td_u8 max_len, td_u32 reg_addr_byte_num, td_u32 data_byte_num);
#endif
    td_s32 (*pfn_isp_write_ssp_data)(td_u32 spi_dev, td_u32 spi_csn,
        td_u32 dev_addr, td_u32 dev_addr_byte_num, td_u32 reg_addr, td_u32 reg_addr_byte_num,
        td_u32 data, td_u32 data_byte_num);
} isp_sns_bus_callback;

typedef struct {
    td_s32 (*pfn_piris_gpio_update)(td_s32 pipe_id, const td_s32 *pos);
} isp_piris_callback;

/* AF */
#define DRV_AF_ZONE_ROW          15
#define DRV_AF_ZONE_COLUMN       17
#define DRV_WDR_CHN_MAX          4

typedef struct {
    td_u16  v1;
    td_u16  h1;
    td_u16  v2;
    td_u16  h2;
    td_u16  y;
    td_u16  hl_cnt;
} isp_focus_zone;

typedef struct {
    /* RO, The zoned measure of contrast */
    isp_focus_zone zone_metrics[DRV_WDR_CHN_MAX][DRV_AF_ZONE_ROW][DRV_AF_ZONE_COLUMN];
} isp_fe_focus_stat;

typedef struct {
    isp_focus_zone zone_metrics[DRV_AF_ZONE_ROW][DRV_AF_ZONE_COLUMN]; /* RO, The zoned measure of contrast */
} isp_be_focus_stat;

typedef struct {
    isp_fe_focus_stat  fe_af_stat;
    isp_be_focus_stat  be_af_stat;
} isp_focus_stat;

/* isp sync task */
typedef enum {
    ISP_SYNC_TSK_METHOD_HW_IRQ = 0,
    ISP_SYNC_TSK_METHOD_WORKQUE,
    ISP_SYNC_TSK_METHOD_BUTT
} isp_sync_tsk_method;

typedef struct {
    isp_sync_tsk_method method;
    td_s32 (*fn_sync_tsk_callback) (td_u64 data);
    td_u64 data;
    const char *name_id;
    struct osal_list_head list;
    isp_focus_stat *focus_stat;
} isp_sync_task_node;

typedef td_void fn_isp_task_call_back(td_s32 chn_id, td_s32 ret_val, const td_void *result, td_u32 result_len);

typedef struct {
    td_s32 pipe_id;
    isp_alg_mode_alone alg_mode;
    isp_alg_cmd_id cmd_id;
    td_bool is_block;
    td_u32 wait_time_in_ms;
    fn_isp_task_call_back *callback_func;
} isp_submit_task_info;

typedef struct {
    td_u64 yh_coef_phy_addr;
    td_u64 yv_coef_phy_addr;
    td_u64 ch_coef_phy_addr;
    td_u64 cv_coef_phy_addr;
    td_void *yh_coef_vir_addr;
    td_void *yv_coef_vir_addr;
    td_void *ch_coef_vir_addr;
    td_void *cv_coef_vir_addr;

    td_u32 yhstep;
    td_u32 yvstep;
    td_u32 chstep;
    td_u32 cvstep;
    td_u32 yhrat;
    td_u32 yvrat;
    td_u32 chrat;
    td_u32 cvrat;
} ext_isp_scale_cfg;

typedef struct {
    td_u32 yhrat;
    td_u32 yvrat;
    td_u32 chrat;
    td_u32 cvrat;
} ext_isp_scale_rate;

typedef enum {
    SCLCOEF_NORMAL = 0,
    SCLCOEF_IVE_TAP2 = 1,
    SCLCOEF_IVE_TAP4 = 2,
    SCLCOEF_IVE_TAP6 = 3,
    SCLCOEF_IVE_TAP8 = 4,
    SCLCOEF_DIS_LINEAR = 5,
    SCLCOEF_MAX
} ext_isp_scale_coef_mode;

typedef struct {
    td_bool online;
    td_bool deflicker;
    td_bool osd_mode;
    ext_isp_scale_coef_mode mode;
    ext_isp_scale_rate rate;
} ext_isp_scale_coef_param;

typedef struct {
    td_s32 (*pfn_isp_register_sync_task)(td_s32 pipe_id, isp_sync_task_node *new_node);
    td_s32 (*pfn_isp_unregister_sync_task)(td_s32 pipe_id, isp_sync_task_node *del_node);
    td_s32 (*pfn_isp_isr_cap)(td_s32 irq, void *id);
    td_s32 (*pfn_isp_isr_cap_bottom_half)(td_s32 irq, void *id);
    td_s32 (*pfn_isp_isr_proc)(td_s32 pipe_id);
    td_s32 (*pfn_isp_get_busy_be_buf)(td_s32 pipe_id, isp_cfg_rec *be_cfg_buf);
    td_s32 (*pfn_isp_put_free_be_buf)(td_s32 pipe_id, isp_cfg_rec *be_cfg_buf);
    td_s32 (*pfn_isp_hold_busy_be_buf)(td_s32 pipe_id, isp_cfg_rec *be_cfg_buf);
    td_s32 (*pfn_isp_get_be_sync_para)(td_s32 pipe_id, isp_be_sync_para *be_sync_para);
    td_s32 (*pfn_isp_get_frame_info)(td_s32 pipe_id, ext_isp_frame_alg_info *isp_frame);
    td_s32 (*pfn_isp_get_attach_info)(td_s32 pipe_id, ext_isp_attach_info *isp_attach_info);
    td_s32 (*pfn_isp_get_color_gamut_info)(td_s32 pipe_id, ext_isp_colorgammut_info *isp_color_gamut_info);
    td_s32 (*pfn_isp_get_dcf_info)(td_s32 pipe_id, ext_isp_dcf_info *isp_dcf);
    td_s32 (*pfn_isp_set_snap_attr)(td_s32 pipe_id, isp_snap_attr *snap_attr);
    td_s32 (*pfn_isp_get_snap_config)(td_s32 pipe_id, ext_isp_config_info *snap_info);
    td_s32 (*pfn_isp_trigger_pro)(td_s32 pipe_id);
    td_bool (*pfn_isp_inquire_pro_start)(td_s32 pipe_id, isp_pro_ctrl *pro_ctrl);
    td_s32 (*pfn_isp_get_scale_coef)(const ext_isp_scale_coef_param *param, ext_isp_scale_cfg *hw_cfg);
    td_u64 (*pfn_isp_get_reg_size)(isp_reg_type type);
    td_void (*pfn_isp_update_be_sync_param)(td_s32 pipe_id, td_void *be_node, isp_be_sync_para *be_sync_para);
    td_s32 (*pfn_isp_submit_alg_task)(const isp_submit_task_info *task_info,
        const td_void *input, td_u32 len_in, td_void *output, td_u32 len_out);
    td_s32 (*pfn_isp_set_face_stat)(td_s32 pipe_id, ext_isp_face_stat *face_stat);
    td_s32 (*pfn_isp_get_fv)(td_s32 pipe_id, td_u64 *fv);
    td_s32 (*pfn_isp_set_refresh_alg_flag)(td_s32 pipe_id);
} isp_export_func;

td_s32 isp_do_mod_init(td_void);
td_void isp_do_mod_exit(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_vicap.h
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef VIU_VICAP_H
#define VIU_VICAP_H

#include "viu_public.h"
#include "viu_node_list.h"
#include "drv_viu_struct.h"
#include "viu_drv_define.h"
#include "viu_base_define.h"

#if defined(CONFIG_CHIP_3322)
#include "drv_viu_vicap.h"
#else
#include "drv_viu.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VIU_STATUS_STARTED                0U
#define VIU_STATUS_STOPPING               1U
#define VIU_STATUS_STOPPED                2U

#define VIU_MAX_PIC_QUEUE_LENGTH          8U
#define VIU_MAX_SEND_RAW_PIC_QUEUE_LENGTH 8U
#define MAX_PIC_QUEUE_LENGTH_FOR_USER    3U

#define MAX_WDR_QUEUE_LENGTH             3U

#define INVALID_CMUX_SEL                 0xF

#define MAX_BIND_NUM                      8
#define VIU_MAX_MODEL_NUM                 ISP_MAX_PIPE_NUM

#define VIU_NODE_NUM                      (ISP_MAX_PIPE_NUM + 1)

#define VIU_IRQ_NAME_LEN                  20

#define MAX_PIC_QUEUE_LENGTH_FOR_USER    3U

#define VIU_PIPE_FRAME_CHN              (VIU_MAX_CHN_NUM)
#define VIU_PIPE_BNR_CHN                (VIU_MAX_CHN_NUM + 1)

/* 默认的通道源帧率, 用户图片使用 */
#define VIU_MAX_SRC_FRMRATE               30
#define VIU_INT_DETECT_INTERVAL           2
#define VIU_MAX_USERPIC_COUNT             ISP_MAX_PIPE_NUM

#define VIU_MAX_INT_TIME                  10000U       /* 中断处理时间超过10ms，提未中断处理时间过长 */

#define viu_down_sem_return(sema)                                \
    do {                                                        \
        if (mcp_osal_sem_down_interruptible(&(sema)) != TD_SUCCESS) {   \
            return (-ERESTARTSYS);                              \
        }                                                       \
    } while (0)

#define viu_up_sem(sema) mcp_osal_sem_up(&(sema))

#define viu_pipe_spin_lock(pipe_id, flags) \
    mcp_osal_spin_lock_irqsave(&g_ast_pipe_synch_ctx[pipe_id].pipe_lock, &(flags))
#define viu_pipe_spin_unlock(pipe_id, flags) \
    mcp_osal_spin_unlock_irqrestore(&g_ast_pipe_synch_ctx[pipe_id].pipe_lock, &(flags))

#define viu_dev_spin_lock(dev, flags) mcp_osal_spin_lock_irqsave(&g_ast_dev_synch_ctx[dev].dev_lock, &(flags))
#define viu_dev_spin_unlock(dev, flags) mcp_osal_spin_unlock_irqrestore(&g_ast_dev_synch_ctx[dev].dev_lock, &(flags))

#define viu_comm_spin_lock(flags) mcp_osal_spin_lock_irqsave(&g_viu_comm_lock, &(flags))
#define viu_comm_spin_unlock(flags) mcp_osal_spin_unlock_irqrestore(&g_viu_comm_lock, &(flags))

#define is_pipe_exist(pipe_id) (g_past_pipe_mkp_ctx[pipe_id] != TD_NULL)
#define is_pipe_unexist(pipe_id) (g_past_pipe_mkp_ctx[pipe_id] == TD_NULL)

#define is_pipe_destroying(pipe_id) (g_ast_pipe_synch_ctx[pipe_id].destroying == TD_TRUE)
#define is_pipe_resetting(pipe_id) (g_ast_pipe_synch_ctx[pipe_id].resetting == TD_TRUE)

#define is_pipe_working(pipe_id) (g_ast_pipe_synch_ctx[pipe_id].working == TD_TRUE)
#define pipe_working_start(pipe_id) (g_ast_pipe_synch_ctx[pipe_id].working = TD_TRUE)
#define pipe_working_finish(pipe_id) (g_ast_pipe_synch_ctx[pipe_id].working = TD_FALSE)

#define is_pipe_cap_working(pipe_id) (g_ast_pipe_synch_ctx[pipe_id].cap_working == TD_TRUE)
#define pipe_cap_working_start(pipe_id) (g_ast_pipe_synch_ctx[pipe_id].cap_working = TD_TRUE)
#define pipe_cap_working_finish(pipe_id) (g_ast_pipe_synch_ctx[pipe_id].cap_working = TD_FALSE)

#define is_usr_pic_timer_working(pipe_id) (g_ast_pipe_synch_ctx[pipe_id].no_int_timer_working == TD_TRUE)
#define usr_pic_timer_working_start(pipe_id) (g_ast_pipe_synch_ctx[pipe_id].no_int_timer_working = TD_TRUE)
#define usr_pic_timer_working_finish(pipe_id) (g_ast_pipe_synch_ctx[pipe_id].no_int_timer_working = TD_FALSE)

#define is_other_pipe_visiting(pipe_id) (g_ast_pipe_synch_ctx[pipe_id].other_pipe_visiting == TD_TRUE)
#define other_pipe_visiting_start(pipe_id) (g_ast_pipe_synch_ctx[pipe_id].other_pipe_visiting = TD_TRUE)
#define other_pipe_visiting_finish(pipe_id) (g_ast_pipe_synch_ctx[pipe_id].other_pipe_visiting = TD_FALSE)

#define other_mod_job_cnt_get(pipe_id) mcp_osal_atomic_read(&g_ast_pipe_synch_ctx[pipe_id].other_mod_job_cnt)
#define other_mod_job_cnt_add(pipe_id) \
    ((td_void)mcp_osal_atomic_inc_return(&g_ast_pipe_synch_ctx[pipe_id].other_mod_job_cnt))
#define other_mod_job_cnt_sub(pipe_id) \
    ((td_void)mcp_osal_atomic_dec_return(&g_ast_pipe_synch_ctx[pipe_id].other_mod_job_cnt))

#define dis_mod_job_cnt_get(pipe_id) mcp_osal_atomic_read(&g_ast_pipe_synch_ctx[pipe_id].dis_job_cnt)
#define dis_mod_job_cnt_add(pipe_id) ((td_void)mcp_osal_atomic_inc_return(&g_ast_pipe_synch_ctx[pipe_id].dis_job_cnt))
#define dis_mod_job_cnt_sub(pipe_id) ((td_void)mcp_osal_atomic_dec_return(&g_ast_pipe_synch_ctx[pipe_id].dis_job_cnt))

#define get_pipe_unique_id(pipe_id) (g_ast_pipe_synch_ctx[pipe_id].unique_id)
#define add_pipe_unique_id(pipe_id) (g_ast_pipe_synch_ctx[pipe_id].unique_id++)

#define viu_pipe_down_interruptible_sem(pipe_id) viu_down_sem_return(g_ast_pipe_synch_ctx[pipe_id].pipe_sem)
#define viu_pipe_up_sem(pipe_id) viu_up_sem(g_ast_pipe_synch_ctx[pipe_id].pipe_sem)

#define viu_chn_luma_down_interruptible_sem(pipe_id) viu_down_sem_return(g_ast_pipe_synch_ctx[pipe_id].get_chn_luma_sem)
#define viu_chn_luma_up_sem(pipe_id) viu_up_sem(g_ast_pipe_synch_ctx[pipe_id].get_chn_luma_sem)

#define viu_chn_frame_down_interruptible_sem(pipe_id, chn_id) \
    viu_down_sem_return(g_ast_pipe_synch_ctx[pipe_id].get_chn_frame_sem[chn_id])
#define viu_chn_frame_up_sem(pipe_id, chn_id) viu_up_sem(g_ast_pipe_synch_ctx[pipe_id].get_chn_frame_sem[chn_id])

#define is_chn_luma_finish(pipe_id, chn_id) (g_ast_pipe_synch_ctx[pipe_id].vgs_chn_luma_callback[chn_id] == TD_TRUE)
#define chn_luma_start(pipe_id, chn_id) (g_ast_pipe_synch_ctx[pipe_id].vgs_chn_luma_callback[chn_id] = TD_FALSE)
#define chn_luma_finish(pipe_id, chn_id) (g_ast_pipe_synch_ctx[pipe_id].vgs_chn_luma_callback[chn_id] = TD_TRUE)

#define is_chn_luma_success(pipe_id, chn_id) (g_ast_pipe_synch_ctx[pipe_id].chn_vgs_job_success[chn_id] == TD_TRUE)
#define chn_luma_fail(pipe_id, chn_id) (g_ast_pipe_synch_ctx[pipe_id].chn_vgs_job_success[chn_id] = TD_FALSE)
#define chn_luma_success(pipe_id, chn_id) (g_ast_pipe_synch_ctx[pipe_id].chn_vgs_job_success[chn_id] = TD_TRUE)

#define viu_set_pipe_mkp_ctx(pipe_id, ptr) (g_past_pipe_mkp_ctx[pipe_id] = (ptr))
#define viu_get_pipe_mkp_ctx(pipe_id) (g_past_pipe_mkp_ctx[pipe_id])
#define viu_get_pipe_mkp_setting_attr_ctx(pipe_id) (&g_past_pipe_mkp_ctx[pipe_id]->pipe_mkp_attr_setting)
#define viu_get_pipe_mkp_working_attr_ctx(pipe_id) (&g_past_pipe_mkp_ctx[pipe_id]->pipe_mkp_attr_working)

#define viu_dev_down_interruptible_sem(viu_dev_id) viu_down_sem_return(g_ast_dev_synch_ctx[viu_dev_id].dev_sem)
#define viu_dev_up_sem(viu_dev_id) viu_up_sem(g_ast_dev_synch_ctx[viu_dev_id].dev_sem)

#define is_bypass_be_mode(pipe_id) (g_past_pipe_mkp_ctx[pipe_id]->pipe_bypass_mode == EXT_VIU_PIPE_BYPASS_BE)

#define viu_get_mod_param_ctx(pipe_id) (&g_mod_param)

#define viu_time_diff(time_end, time_begin) (td_u32)((time_end) - (time_begin))

#define viu_module_spin_lock(flags) mcp_osal_spin_lock_irqsave(&g_viu_module_lock, &(flags))
#define viu_module_spin_unlock(flags) mcp_osal_spin_unlock_irqrestore(&g_viu_module_lock, &(flags))

td_s32 viu_check_chn_id_for_ioctl(td_s32 chn_id);
td_s32 viu_pipe_down_sem(td_s32 pipe_id);
td_u64 viu_get_sys_time_by_usec(td_void);
td_bool viu_linear_offline(td_s32 pipe_id, ext_isp_wdr_mode wdr_mode);
td_void viu_clear_input_queue(td_s32 pipe_id);

typedef struct {
    td_u32 chn_time_ref[VIU_MAX_CHN_NUM]; /* 通道向后端发送当前帧的timeref */
    td_u32 chn_last_time_ref[VIU_MAX_CHN_NUM]; /* 通道向后端发送上一帧的timeref */
    td_u32 raw_time_ref;
} viu_time_ref_info;

typedef struct {
    td_u32 bind_num;
    td_s32 bind_table[MAX_BIND_NUM];
} viu_extchn_bind;

typedef struct {
    ext_isp_rect region[ISP_LUMA_STAT_NUM]; /* region attribute */
    td_u32 region_num; /* count of the region */
    td_bool need_do;
    td_s32 chn;
    viu_mem_info region_luma_mem;
} viu_region_luma_attr;

typedef struct {
    td_bool coverex_en; /* 是否需要打COVEREX */
    td_bool region_luma_en; /* 是否需要获取亮度和 */
    td_bool osd_en; /* 是否需要打OSD */
    td_bool ldc_en; /* 是否需要做LDC */
    td_bool ldc_v3_en;
    td_bool ldc_v2_en; /* 是否需要做LDCV2 */
    td_bool rotation_en; /* 是否需要做旋转 */
    td_bool rotation_ex_en; /* 是否需要做任意角度旋转 */
    td_bool dis_en; /* 是否需要做DIS */
} viu_other_mod_task;

typedef enum {
    VB_CAPTURE_READY = 0, /* VB准备采集阶段 */
    VB_CAPTURE_START = 1, /* VB开始采集阶段 */
    VB_CAPTURE_DONE  = 2, /* VB采集结束阶段 */
    VB_CAPTURE_BUTT
} vb_capture_stage;

/////////////////////////////////////////////////////////// move from drv_viu_ext.h  fix me!
typedef struct {
    ext_isp_video_frame_info video_frame;
    ext_isp_config_info isp_config_info;
    ext_isp_dcf_update_info dcf_update_info;
    td_u64 start_time;
    td_u64 end_time;
    td_u64 vicap_start_time;
    td_u64 left_end_time;
} viu_cap_pic_info;

typedef struct {
    viu_cap_pic_info pic_info;
    td_bool buf_ok;
    td_bool cap_ok;
    td_bool send_ok;
    td_bool lost_repeat_frame;
} viu_buf_info;

typedef enum {
    FRAME_NORMAL = 0x0,
    FRAME_LOWDELAY,
    FRAME_LOWDELAY_FIN,
} viu_frame_info;

typedef enum {
    VIU_FRAME_SOURCE_NORMAL = 0x0,
    VIU_FRAME_SOURCE_USERPIC,
    VIU_FRAME_SOURCE_BUTT,
} viu_frame_source_type;

typedef struct {
    ext_isp_video_frame_info video_frame;
    ext_isp_video_frame_info single_y_frame;
    td_u64 start_time;
    td_u64 end_time;
    viu_frame_info frame_info;
    viu_frame_source_type frame_source;
} viu_pic_info;
// move end

// move from viu_pic_queue.h  fix me! 避免 viu.h 和 viu_pic_queue.h 互相包含
typedef struct {
    viu_buf_info cap_pic_info;
    isp_cfg_rec be_cfg_buf;
    isp_be_sync_para be_sync_param;
    struct osal_list_head list;
} viu_cap_pic_node;

typedef struct {
    ext_isp_video_frame_info video_frame;
    struct osal_list_head list;
} viu_pic_node;

typedef struct {
    viu_buf_info pic_info[ISP_WDR_CHN_MAX];
    isp_cfg_rec be_cfg_buf;
    isp_be_sync_para be_sync_param;
    td_bool be_cfged;
    struct osal_list_head list;
} viu_wdr_pic_node;

typedef struct {
    td_void *node_buf;
    struct osal_list_head free_list;
    struct osal_list_head busy_list;
    mcp_osal_spinlock spin_lock;
    td_u32 free_num;
    td_u32 busy_num;
    td_u32 max_num;
} viu_pic_queue;
// move end

typedef struct {
    viu_pic_info pic_info;
    td_bool buf_ok; /* 内存区域申请是否成功 */
    td_bool cap_ok; /* 采集是否正确 */
    td_bool send_ok; /* 是否已经发送 */
    td_bool dis_buf_ok; /* 内存区域申请是否成功 */
    vb_capture_stage cap_stage; /* 当前buf采集处于何种阶段，引入辅助early_end方案 */
} viu_chn_buf_info;

typedef struct {
    /* 离线PROC硬件工作统计 */
    td_u64 submit_cnt; /* 离线PROC启动的次数 */
    td_u64 offline_int_cnt; /* 离线PROC上报任意中断的次数 */
    td_u64 list_int_cnt; /* 离线PROC上报链表中断的次数 */
    td_u64 tm_out_int_cnt; /* 离线PROC上报超时中断的次数 */
    td_u64 offline_bus_err_cnt; /* 离线PROC bus error的次数 */
    td_u64 offline_dcmp_err_cnt; /* 离线PROC 解压错误的次数 */
    td_u64 start_err_cnt; /* 离线PROC 启动错误的次数 */
    td_u64 node_id_err_cnt; /* 离线PROC 节点号错误的次数 */

    /* 在线PROC硬件工作统计 */
    td_u64 online_int_cnt; /* 在线PROC上报任意中断的次数 */
    td_u64 frm_start_cnt; /* 在线PROC帧起始中断的次数 */
    td_u64 frm_err_cnt; /* 在线PROC帧错误的次数 */
    td_u64 frm_flow_cnt; /* 在线PROC帧溢出的次数 */
    td_u64 online_bus_err_cnt; /* 在线PROC bus error的次数 */
    td_u64 online_dcmp_err_cnt; /* 在线PROC 解压错误的次数 */
    td_u64 cfg_loss_err_cnt; /* 在线PROC 丢配置错误的次数 */
    td_u64 au64_chn_low_delay_cnt[VIU_MAX_PHY_CHN_NUM]; /* 在线PROC低延时中断的次数 */
    td_u64 cur_frm_cnt; /* 在线PROC当前正确采集帧数 */
    td_u64 last_sec_frm_cnt; /* 在线PROC一秒前正确采集帧数 */
    td_u32 per_sec_frm_cnt; /* 在线PROC每秒正确采集帧数 */

    /* PROC 离线硬件处理时间统计 */
    td_u64 hw_start_tm; /* PROC启动的时间点 */
    td_u64 hw_cost_tm; /* PROC硬件处理的时间 */
    td_u64 max_hw_cost_tm; /* PROC最大的硬件处理的时间 */
    td_u64 hw_cycle_cnt; /* PROC硬件cycle计数 */
    td_u64 max_cycle_cnt; /* PROC最大的硬件cycle计数 */

    /* PROC 中断计数耗时统计 */
    td_u64 cur_int_cnt; /* PROC当前中断总次数 */
    td_u64 last_sec_int_cnt; /* PROC一秒前中断总次数 */
    td_u32 int_cnt_per_sec; /* PROC最近一次的1秒内中断次数 */
    td_u32 max_int_cnt_per_sec; /* PROC历史上1秒内最大的中断次数 */

    /* PROC 中断处理时间统计 */
    td_u64 int_tm_total; /* 当前int执行时间总耗时 */
    td_u64 int_tm_last; /* 1秒前int执行时间总耗时 */

    td_u32 cur_int_cost_tm; /* 最近1次执行int的执行耗时 */
    td_u32 max_int_cost_tm; /* 执行1次int的最大耗时 */

    td_u32 int_cost_per_sec; /* 最近1秒执行int的执行耗时 */
    td_u32 max_int_cost_per_sec; /* 历史上1秒执行int的最大执行耗时 */

    td_u64 int_begin_tm; /* int统计开始时间 */
} viu_per_proc_statistic;

typedef struct {
    viu_per_proc_statistic proc_statistic[ISP_VIPROC_IP_NUM];
} viu_proc_statistic;

typedef struct {
    td_u32 send_pic_cnt; /* 通道成功发送的帧数 */
    td_u32 chn_frmlost_cnt; /* 通道丢帧数 */
    td_u32 rate_ctllost_cnt; /* 通道丢帧数 */
    td_u32 chn_vb_fail_cnt; /* 通道获取VB失败 */
    ext_isp_size size;

    td_u64 cur_chn_frm_cnt; /* 通道当前完成的帧数 */
    td_u64 last_sec_chn_frm_cnt; /* 通道一秒前完成的帧数 */
    td_u32 chn_frm_rate; /* 通道帧率 */
    td_u64 chn_begin_tm; /* 通道帧率统计开始的时间 */
} viu_chn_statistic;

typedef struct {
    td_bool detect_err_int; /* 是否使能错误检测 */
    td_bool cur_err_int; /* 当前中断是否错误 */
    td_bool need_lost_frame; /* 是否需要丢帧 */
    td_u32 err_int_cnt_to_drop_frame; /* 需要多少个错误中断才丢帧 */
    td_u32 drop_err_frm_cnt; /* 连续丢多少帧 */
    td_u32 cur_err_int_cnt_to_drop_frame; /* 当前已经有多少个错误中断 */
    td_u32 cur_drop_err_frm_cnt; /* 当前已经丢多少帧 */
} viu_err_int_detect;

typedef enum {
    VIU_LOST_CFG_STAGE_NONE = 0, /* 无丢配置中断 */

    VIU_LOST_CFG_STAGE_THIS_INT, /* 丢配置中断当前处理 */

    VIU_LOST_CFG_STAGE_BUTT
} viu_lost_cfg_stage;
typedef struct {
    td_u64 captured_time;
    td_u64 left_process_time;
    td_u64 up_line;
    td_u64 low_line;
} pipe_one_buf_time_info;

typedef struct {
    td_u64 recv_pic; /* PIPE接收到的帧数 */
    td_u32 pipe_frmlost_cnt; /* PIPE丢帧数 */
    td_u32 buf_cnt; /* PIPE当前占用VB数 */
    td_u32 int_err_cnt; /* 错误中断总次数 */
    td_bool cur_int_err; /* 当前是否有错误中断 */
    td_bool proc_int_err[ISP_VIPROC_IP_NUM]; /* PROC是否有错误中断,SBS使用 */
    td_bool proc_int[ISP_VIPROC_IP_NUM]; /* PROC是否有中断,SBS使用 */
    td_bool proc_lost_cfg_int_err[ISP_VIPROC_IP_NUM]; /* PROC是否有错误中断,SBS使用 */
    viu_lost_cfg_stage lost_cfg_stage; /* PROC是否有丢配置中断 */
    td_s32 start_fail_cnt; /* 所有通道未使能、query失败或未绑定造成启动失败的次数 */
    td_u64 soft_done_time;
    td_u64 cur_task_soft_cost_tm; /* 历史上耗时最长软件调度任务的耗时(从进pipe到提交硬件前) */
    td_u64 max_task_soft_cost_tm; /* 当前软件调度任务的耗时(从进pipe到提交硬件前) */
    td_u64 cur_task_cost_tm; /* 当前完成任务的耗时(从进pipe到硬件完成) */
    td_u64 max_task_cost_tm; /* 历史上耗时最长任务的执行时间(从进pipe到硬件完成) */
    td_u32 pipe_low_band_width; /* PIPE上报低带宽的次数 */

    td_u64 cur_task_start_time;

    viu_err_int_detect err_int_detect;

    viu_chn_statistic chn_statistic[VIU_MAX_CHN_NUM];

    pipe_one_buf_time_info one_buf_time_info;  /* onebuf方案时，统计buf相关的时间信息，proc显示使用 */
} viu_pipe_statistic;

typedef struct {
    td_bool destroying;
    td_bool resetting;
    td_bool working;
    td_bool cap_working;
    td_bool no_int_timer_working;
    td_bool other_pipe_visiting; /* 是否有其他PIPE正在访问本PIPE的上下文 */
    td_s32 unique_id; /* for re-create pipe */
    mcp_osal_atomic other_mod_job_cnt; /* VGS/fisheye等其他模块正在处理的job计数 */

    mcp_osal_wait wait_user_get_chn_frame[VIU_MAX_CHN_NUM]; /* wait_queue for user getting frame */
    mcp_osal_wait wait_user_send_frame; /* user send frame */
    mcp_osal_wait wait_user_get_region_luma; /* user get chn_rgn_luma */
    mcp_osal_wait wait_use_get_raw_frame; /* user get raw  */
    mcp_osal_wait wait_stop_working; /* wait pipe stop working */
    mcp_osal_wait wait_get_bnr_raw_frame; /* user get bnr raw  */

    mcp_osal_semaphore get_chn_luma_sem; /* 信号量 */
    mcp_osal_semaphore pipe_sem; /* 信号量 */
    mcp_osal_semaphore get_chn_frame_sem[VIU_MAX_CHN_NUM];

    mcp_osal_spinlock pipe_lock; /* 保护MKP层上下文 */

    td_bool vgs_chn_luma_callback[VIU_MAX_PHY_CHN_NUM];
    td_bool chn_vgs_job_success[VIU_MAX_PHY_CHN_NUM];

    mcp_osal_atomic dis_job_cnt;
    mcp_osal_wait wait_dis_fin;
} viu_pipe_synchronization_ctx;

typedef struct {
    mcp_osal_semaphore dev_sem; /* 信号量 */
    mcp_osal_spinlock dev_lock; /* 保护MKP层上下文 */
} viu_dev_synchronization_ctx;

/* MKP层DEV上下文 */
typedef struct {
    td_bool dev_cfged; /* dev configured flag */
    td_bool dev_en; /* dev enable flag */
    ext_viu_dev_bind_pipe dev_bind_pipe; /* 绑定的PIPE */
    td_u32 unique; /* Mark the process of opening device node */
} viu_dev_mkp_ctx;

typedef struct {
    td_u32 int_cnt; /* 通道中断数，调试用 use */
    td_u32 get_vb_fail; /* 获取视频帧缓存失败次数 */
    td_u32 throw_int_cnt; /* 硬件上报帧/场丢失中断的次数 use */
    td_u32 cc_int_err_cnt; /* 中断异常导致未采集完成的次数，调试用 与 detect_err_frame相关 */
    td_u32 cc_int_err_total_cnt; /* 中断异常导致未采集完成的次数，调试用 */
    td_u32 timing_err; /* 用于统计BUF溢出等异常中断次数 use */
    td_u64 last_int_time; /* 上次中断(帧/场中断)时间，调试用 use */
    td_u32 int_time; /* 中断占用时间，调试用 */
    td_u32 int_time_max; /* 最大中断占用时间，调试用 */
    td_u32 int_gap_time; /* 相邻两中断之间的时间间隔 */
    td_u32 int_gap_time_max; /* 相邻两中断之间的最大时间间隔 */
    td_bool not_first_int; /* 非第一个中断，与last_int_time配合使用，用于统计相邻两中断时间间隔 */
    td_u32 too_long_int_cnt; /* 中断处理时间超过10ms的次数 */
    td_u32 lost_int; /* 中断丢弃数，调试用 */

    td_u64 cur_pipe_frm_cnt; /* 当前完成的帧数 */
    td_u64 last_sec_pipe_frm_cnt; /* 一秒前完成的帧数 */
    td_u32 pipe_frm_rate; /* 帧率 */
    td_u64 pipe_begin_tm; /* 帧率统计开始的时间 */

    td_u32 buf_cnt; /* 占用buf数，调试用 */

    viu_err_int_detect err_int_detect;
} viu_wch_dbg_info;
typedef enum {
    VIU_WAIT_STOP_PREPARE = 0, /* chn prepare stop */
    VIU_WAIT_STOP_END = 1, /* chn stopped */
    VIU_WAIT_STOP_START = 2, /* chn start stop */
    VIU_WAIT_STOP_ING = 3, /* chn stopping */
    VIU_WAIT_VENC_RETURN_FRAME = 4,
    VIU_WAIT_STOP_BUTT
} viu_wait_stop;

typedef enum {
    VIU_SNAP_STATUS_DISABLE = 0,
    VIU_SNAP_STATUS_ENABLE,
    VIU_SNAP_STATUS_TRIGGER,
    VIU_SNAP_STATUS_TRIGGER_FINISHED,
    VIU_SNAP_STATUS_BUTT
} viu_snap_status_type;

#define MAX_TMP_ISPINFO_NUM              4

typedef struct {
    td_bool first_snap_frame;
    td_bool multi_trigger;
    td_bool double_pipe;
    td_s32 video_pipe_id;
    td_s32 snap_pipe_id;
    td_u32 snap_vc_num;
    td_u64 last_tmp_isp_info_pts;
    td_u32 pro_delay_int;  /* 从ISP取到pro_enable之后延迟几个中断是长曝光的帧 */
    td_u32 frame_flag;  /* 在线用 */
    td_u64 trigger_pts;
    td_u32 interval_cnt;
} viu_snap_pipe_ctx;

/* 用户图片库信息 */
typedef struct {
    td_s32 use_cnt;
    td_bool buf_ok;
    ext_isp_video_frame_info video_frame;
    td_bool need_update;
} viu_workingpic_info;

typedef struct {
    td_bool coverex; /* 是否需要打COVEREX */
    td_bool osd; /* 是否需要打OSD */
    td_bool rotation; /* 是否需要做旋转 */

    td_bool bypass;
} viu_userpic_other_mod_task;

typedef struct {
    td_bool set_usr_pic;
    ext_viu_user_pic_mode usr_pic_mode;
    ext_viu_user_pic_bgc bg_color;
    ext_isp_video_frame_info user_video_frame;

    viu_workingpic_info working_pic[VIU_MAX_USERPIC_COUNT];

    td_bool pipe_usr_pic_en[ISP_MAX_PIPE_NUM];
    td_u32 time_ref[ISP_MAX_PIPE_NUM];
    td_u64 time_pts[ISP_MAX_PIPE_NUM];
    td_s32 pipe_usr_pic_id[ISP_MAX_PIPE_NUM][VIU_MAX_PHY_CHN_NUM];
    viu_userpic_other_mod_task usr_pic_other_mod_task[ISP_MAX_PIPE_NUM][VIU_MAX_PHY_CHN_NUM];
} viu_userpic_comm;

typedef struct {
    td_u32 frm_rate; /* VI目标帧率 */
    td_u32 src_frm_rate; /* 输入源的帧率(PAL:25,NTSC:30,DC: ) */
    td_u32 lost_frm_key; /* 丢帧策略关键值 */
} viu_frm_rate_info;

typedef struct {
    td_bool has_int; /* 记录上次检测是否存在中断的标志 */
    td_s32 det_interval; /* 检测是否存在中断的间隔 */
    td_u32 u32last_int_cnt; /* 记录上次检测时的中断计数 */
    td_u32 no_int_cnt; /* 检测到的无中断次数 */
    td_u32 interval; /* 检测间隔(ms为单位) */
    mcp_osal_timer timer; /* 定时器结构体 */
    td_bool timer_existed; /* timer是否存在 */
    viu_frm_rate_info timer_fr_rt; /* 定时器的帧率控制 */
} viu_int_detect;

typedef struct {
    td_bool start; /* 是否可以开始接受图像 */
    td_bool chn_en[VIU_MAX_CHN_NUM];
    ext_snap_attr snap_attr;

    td_bool pipe_start_update;
    td_bool chn_enable_update[VIU_MAX_CHN_NUM];
} viu_pipe_mkp_attr;

typedef struct {
    td_u64  send_pts; /* 发送上一帧时，和BE处理BUF的send pts比较确定哪一帧 */
    td_u64  end_pts; /* 已发送的帧完成中断时间 */
    td_bool cap_ok; /* 记录上一帧的状态，early方案提前发送，如果没有帧完成，已发送帧不能应用于编码 */
} vicap_sent_frame_status;

/* MKP层PIPE上下文 */
typedef struct {
    ext_viu_pipe_bypass_mode pipe_bypass_mode;

    viu_pic_queue line_input_pic_queue;
    viu_pic_queue wdr_input_pic_queue;
    viu_pic_queue dump_pipe_raw_pic_queue;
    viu_pic_queue chn_pic_queue[VIU_MAX_CHN_NUM];
    td_s32 chn_lost_frm_key[VIU_MAX_CHN_NUM];

    // viproc 输入 buf
    viu_cap_pic_info *cur_pic_info;
    viu_cap_pic_node *cur_line_pic; /* 当前处理图像 */
    viu_wdr_pic_node *cur_wdr_pic;
    viu_wdr_pic_node *send_this_wdr_pic; /* 当前要发送的wdr节点 */

    td_bool first_frame; /* 是否第一幅图像 */
    td_bool update_first_frame_pipe_attr;
    td_bool update_first_frame_chn_attr;
    td_u64  first_int_pts;

    viu_extchn_bind ext_chn_bind[VIU_MAX_PHY_CHN_NUM]; /* 内部绑定关系 */

    td_u32 ori_width;
    td_u32 ori_height;

    td_bool cap_raw;

    td_u32 vc_number;
    td_s32 dst_mod;
    td_s32 dst_id;

    viu_buf_info this_frm;
    viu_buf_info next_frm; /* FE采集当前帧与下一帧 */
    td_bool    allocated; /* onebuf方案，是否已经分配了buf */
    viu_wait_stop raw_wait_stop_status;

    viu_buf_info src_this_frm;
    viu_buf_info src_next_frm; /* SRC读取 */

    // viproc 输入 buf
    viu_chn_buf_info chn_this_frm[VIU_MAX_CHN_NUM];
    viu_chn_buf_info chn_next_frm[VIU_MAX_CHN_NUM]; /* BE采集当前帧与下一帧 */
    viu_chn_buf_info lost_cfg_frm[VIU_MAX_CHN_NUM]; /* 丢配置的时候需要保存通道帧 */

    viu_wait_stop aen_chn_wait_stop_status[VIU_MAX_PHY_CHN_NUM];

    /* Region luma buffer */
    viu_region_luma_attr region_luma_attr;
    viu_other_mod_task other_mod_task[VIU_MAX_CHN_NUM];

    viu_wch_dbg_info wch_dbg;

    viu_pipe_statistic pipe_statistic; /* PIPE统计信息 */

    /* SNAP */
    td_bool snap_cfged;
    viu_snap_status_type viu_snap_status;
    viu_pic_queue snap_pipe_raw_pic_queue;
    viu_pic_queue snap_zsl_raw_pic_queue;
    viu_snap_pipe_ctx viu_snap_pipe_ctx;

    viu_pipe_mkp_attr pipe_mkp_attr_setting;
    viu_pipe_mkp_attr pipe_mkp_attr_working;

    viu_int_detect int_detect; /* 插入用户图片时，定时器检测相关信息 */

    /* USER VB POOL */
    vb_pool pipe_vb_pool;
    vb_pool a_chn_vb_pool[VIU_MAX_CHN_NUM];
    td_u64 vicap_frame_start_time;
    td_u64 wdr_frame_interval_pts;
    vicap_sent_frame_status sent_frame_status;

    /* Mark the process of opening device node */
    td_void *unique;
} viu_pipe_mkp_ctx;

typedef struct {
    viu_node_queue node_queue; /* VI HAL层节点队列 */
    viu_node *past_tail_node[ISP_VIPROC_IP_NUM]; /* 链表的第最后一个节点 */
    td_u32 sel_busy_queue[ISP_VIPROC_IP_NUM]; /* 用于工作的busy队列，与backup队列对应 */

    td_u32 viu_offline_vpss_offline_wait_queue_weight_sum; /* wait队列的权重和 */
    td_u32 viu_offline_vpss_offline_weight_for_single_viu_proc;

    td_u32 viu_offline_vpss_online_wait_queue_weight_sum; /* wait队列的权重和 */
    td_u32 viu_offline_vpss_online_weight_for_single_viu_proc;

    viu_drv_model model[VIU_MAX_MODEL_NUM]; /* 各PIPE维护一个MODEL，包含两个乒乓NODE */
    td_u32 frame_num[ISP_MAX_PIPE_NUM];

    td_u32 int_node_id; /* 中断时的节点id */
    td_u32 viu_offline_vpss_offline_proc_num;
    td_u32 viu_offline_vpss_online_proc_num;
    td_u32 min_viu_offline_vpss_offline_proc_id;
    td_u32 min_viu_offline_vpss_online_proc_id;
    viu_proc_statistic proc_statistic; /* PROC统计信息 */
} viu_comm_ctx;

typedef enum {
    VIU_SENDFRAME_WAKEUP = 0, /* send_frame wake up   */
    VIU_GETCHNLUMA_WAKEUP, /* get_chn_luma wake up  */
    VIU_GETFRAME_WAKEUP, /* get_frame wake up */
    VIU_SEND_WDR_FRAME_WAKEUP, /* send_frame wake up   */
    VIU_GETRAW_WAKEUP, /* get_frame wake up   */
    VIU_WIAT_BUTT
} viu_waitqueue_wakeup;

typedef struct {
    td_s32 viu_pipe_id;
    td_s32 viu_chn_id;
    td_s32 unique_id;
    td_bool wait_flag;
    viu_waitqueue_wakeup wake_condtion;
} viu_wait_cond_info;

typedef struct {
    isp_cfg_rec be_cfg_buf;
    isp_be_sync_para be_sync_param;
} isp_be_cfg;

#define viu_mkp_get_dev_ctx(viu_dev_id) (&g_ast_dev_mkp_ctx[viu_dev_id])

extern viu_dev_mkp_ctx g_ast_dev_mkp_ctx[VIU_MAX_DEV_NUM];
extern viu_dev_synchronization_ctx g_ast_dev_synch_ctx[VIU_MAX_DEV_NUM];

extern viu_pipe_mkp_ctx *g_past_pipe_mkp_ctx[ISP_MAX_PIPE_NUM];
extern viu_pipe_synchronization_ctx g_ast_pipe_synch_ctx[ISP_MAX_PIPE_NUM];
extern viu_pic_info g_ast_pic_info[ISP_MAX_PIPE_NUM][VIU_MAX_CHN_NUM];

extern mcp_osal_spinlock g_viu_comm_lock;

extern ext_viu_mod_param g_mod_param;

viu_pipe_synchronization_ctx* viu_get_pipe_synch_ctx(td_s32 pipe_id);

/********************************* MKP interface start**********************************/
td_s32 viu_set_dev_timing_attr(td_s32 viu_dev_id, ext_viu_dev_timing_attr *dev_timing_attr);
td_s32 viu_get_dev_timing_attr(td_s32 viu_dev_id, ext_viu_dev_timing_attr *dev_timing_attr);

td_s32 viu_mod_release_frame(ext_isp_video_frame_info *frame_info);

viu_vb_source viu_get_vb_source(td_void);

td_s32 viu_set_pipe_dump_attr(td_s32 pipe_id, const ext_viu_dump_attr *dump_attr);
td_s32 viu_get_pipe_dump_attr(td_s32 pipe_id, ext_viu_dump_attr *dump_attr);

td_s32 viu_send_pipe_rgb(td_s32 pipe_id, ext_isp_video_frame_info *video_frame, td_s32 milli_sec);
td_s32 viu_send_pipe_yuv(td_s32 pipe_id, ext_isp_video_frame_info *video_frame, td_s32 milli_sec);

td_s32 viu_send_mult_pipe_raw(td_s32 pipe_id, ioc_viu_usr_send_raw_timeout *send_raw_timeout);

td_s32 viu_get_pipe_raw(td_s32 pipe_id, ext_isp_video_frame_info *video_frame, td_s32 milli_sec);

td_s32 viu_get_chn_frame(td_s32 pipe_id, td_s32 chn_id, ioc_viu_usr_get_yuv_timeout *get_yuv_timeout);

td_s32 viu_set_chn_ldc_attr(td_s32 pipe_id, td_s32 chn_id, viu_gdc_ldc_attr *ldc_attr);
td_s32 viu_get_chn_ldc_attr(td_s32 pipe_id, td_s32 chn_id, ext_viu_ldc_attr *ldc_attr);
td_s32 viu_get_chn_ldc_drv_attr(td_s32 pipe_id, td_s32 chn_id, viu_gdc_ldc_attr *ldc_drv_attr);

/********************************* MKP interface end************************************/
td_s32 viu_do_poll(td_s32 pipe_id, td_s32 chn_id, mcp_osal_poll *poll_info);

td_bool viu_is_wait_cap_fram_stop(viu_pipe_mkp_ctx *pipe_mkp_ctx, viu_wait_stop *stop_status);

td_void viu_wait_capture_frame_stop(td_s32 pipe_id, viu_wait_stop *stop_status);

td_void viu_release_pipe_capture_vb(td_s32 pipe_id);

td_s32 viu_start_one_task(td_s32 pipe_id, ext_viu_operating_mode_type pipe_mode);

td_void viu_proc_send_pic(td_s32 pipe_id, td_s32 chn_id, viu_pic_info *pic_info);

td_s32 viu_cap_init(td_void);
td_void viu_cap_deinit(td_void *unique);
td_s32 viu_cap_comm_dev_init(td_void);
td_void viu_cap_comm_dev_exit(td_void);

td_s32 viu_proc_init(td_void);
td_s32 viu_proc_deinit(td_void *unique);
td_s32 viu_proc_comm_dev_init(td_void);
td_void viu_proc_comm_dev_exit(td_void);

td_void viu_set_default_mod_param(td_void);

td_void viu_init_time_ref(td_void);
td_u32 viu_calc_chn_time_ref(td_s32 pipe_id, td_s32 chn_id);
td_u32 viu_calc_raw_time_ref(td_s32 pipe_id);

td_s32 viu_check_mod_param(const ext_viu_mod_param *mod_param);

td_s32 viu_synch_ctx_init(td_void);
td_void viu_synch_ctx_exit(td_void);

td_void viu_comm_get_offline_proc_cnt(td_void);

td_s32 viu_model_create_one_model(td_s32 pipe_id);
td_void viu_model_destroy_one_model(td_s32 pipe_id);

td_void *viu_comm_create_one_node(td_s32 pipe_id, ext_viu_operating_mode_type pipe_mode);

td_s32 viu_comm_submit_one_node(td_void *node, ext_viu_operating_mode_type pipe_mode);

td_void viu_comm_cap_online_frame_irq_route(td_s32 pipe_id,
    ext_viu_operating_mode_type pipe_mode, td_u64 irq_begin_time);

td_s32 viu_proc_offline_irq_route(td_s32 pipe_id, ext_viu_operating_mode_type pipe_mode, td_bool err_irq);

td_void viu_update_working_pipe_mkp_attr(td_s32 pipe_id);
td_void viu_update_working_chn_mkp_attr(td_s32 pipe_id, td_s32 chn_id);
td_void viu_update_working_pipe_chn_mkp_attr(td_s32 pipe_id);

td_s32 viu_proc_chn_frame(td_s32 pipe_id, td_s32 chn_id, viu_chn_buf_info *this_frm);

td_void viu_clear_linear_pic_queue(td_s32 pipe_id, viu_pic_queue *node_queue);

/* 导出函数 ，给ISP使用 */
td_s32 viu_update_viu_vpss_mode(ext_viu_operating_mode *fn_viu_update_viu_ppu_mode);

td_s32 viu_get_be_stt_info(td_s32 pipe_id, td_u32 block, td_void *be_stt);

td_s32 viu_down_semaphore(td_s32 pipe_id);
td_s32 viu_up_semaphore(td_s32 pipe_id);
td_bool viu_is_pipe_existed(td_s32 pipe_id);
td_s32 viu_get_pipe_bind_dev_size(td_s32 pipe_id, ext_isp_size *dev_size);
td_s32 viu_get_pipe_nrx_status(td_s32 pipe_id, td_bool *nrx_en);
td_s32 viu_snap_get_isp_config(td_s32 pipe_id, ext_isp_snap_info *snap_info);

viu_proc_statistic* viu_proc_get_statistic(td_void);
td_u64 viu_comm_get_first_int_pts(td_u32 proc_id);

td_void viu_comm_cap_prepare_frame(td_s32 pipe_id);

td_void viu_snap_cap_send_frame(td_s32 pipe_id);
td_void viu_snap_offline_is_stop_irq_proc(td_s32 pipe_id);
td_void viu_snap_online_irq_proc(td_s32 pipe_id);
td_void viu_snap_find_pro_frame(td_s32 pipe_id);
td_void viu_snap_proc_chn_frame(td_s32 pipe_id, td_s32 chn_id, td_bool cap_ok);
td_bool viu_snap_is_valid_dump_frame(td_s32 pipe_id);
td_s32 viu_snap_get_isp_config_info(td_s32 pipe_id, ext_isp_config_info *isp_config_info);

td_void viu_pipe_frame_rate_statistic(td_s32 pipe_id, td_bool frame_ok);
td_void viu_chn_frame_rate_statistic(td_s32 pipe_id, td_s32 chn_id, td_bool send);

td_void viu_pipe_sync_ctx_init(td_s32 pipe_id);
td_void viu_destory_pipe_pic_queue(td_s32 pipe_id);
td_s32 viu_create_pipe_pic_queue(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr, ext_isp_wdr_mode wdr_mode);
td_void viu_pipe_mkp_ctx_exit(td_s32 pipe_id);
td_s32 viu_pipe_mkp_ctx_init(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr, td_void *unique);
td_s32 viu_pipe_ctx_init(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr, td_s32 bind_viu_dev,
    ext_isp_wdr_mode wdr_mode, td_void *unique);
td_void viu_pipe_ctx_exit(td_s32 pipe_id);
td_s32 viu_pipe_malloc_mem(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr, ext_isp_wdr_mode wdr_mode);
td_void viu_pipe_free_mem(td_s32 pipe_id);
td_void viu_init_err_int_detect(viu_err_int_detect *err_int_detect);
td_void viu_mkp_pipe_wdr_de_init(td_s32 pipe_id);
td_s32 viu_mkp_pipe_wdr_init(td_s32 pipe_id);
td_s32 viu_mkp_create_input_queue(td_s32 pipe_id, ext_isp_wdr_mode wdr_mode);

td_bool viu_is_phy_chn_attach_fisheye(td_s32 pipe_id, td_s32 chn_id);

td_s32 viu_check_comm_crop(ext_viu_crop_info *crop_info);

td_u32 viu_get_chn_depth(td_s32 pipe_id, td_s32 chn_id);

td_s32 viu_clear_pic_queue(viu_pic_queue *node_queue);

td_void viu_release_chn_capture_vb(td_s32 pipe_id, td_s32 chn_id);

td_s32 viu_abnormal_verify_after_wait(td_s32 pipe_id, td_s32 unique_id);

td_void viu_release_chn_buffer(td_s32 pipe_id);

td_void viu_release_cur_pic(td_s32 pipe_id);

td_s32 viu_wait_condition_call_back(const td_void *param);

td_s32 viu_wdr_mode_send_pipe_raw_from_be(td_s32 pipe_id, ioc_viu_usr_send_raw_timeout *send_raw_timeout,
    ext_viu_operating_mode_type pipe_mode);

td_s32 viu_line_mode_send_pipe_frame(td_s32 pipe_id, ext_isp_video_frame_info *video_frame, td_s32 milli_sec,
    ext_viu_operating_mode_type pipe_mode);

td_void viu_comm_cap_statistics_int_time(td_s32 pipe_id, td_u64 start_time);

td_bool viu_comm_is_pipe_start(td_s32 pipe_id);

td_bool viu_comm_need_do_irq_route(td_s32 pipe_id, td_bool master_pipe_do_irq);

td_void viu_comm_proc_online_update_attr(td_s32 pipe_id);

td_void viu_comm_proc_online_send_last_frame(td_s32 pipe_id, td_u64 irq_begin_time);

td_void viu_comm_prepare_next_frame_pipe_chn_attr(td_s32 pipe_id, td_s32 chn_id);

td_void viu_comm_lost_cfg_int_repair(td_s32 pipe_id, td_bool lost_int_err);

td_bool viu_statistic_err_int_detect(viu_err_int_detect *err_int_detect, td_bool int_err);

td_void viu_comm_update_first_frame_pipe_attr(td_s32 pipe_id);

td_void viu_comm_update_first_frame_chn_attr(td_s32 pipe_id, td_s32 chn_id);

td_void viu_comm_proc_prepare_next_chn_frame(td_s32 pipe_id, td_s32 chn_id);

td_s32 viu_isp_clear_input_queue(td_s32 pipe_id);

td_s32 viu_update_bayer_format(td_s32 pipe_id, ext_isp_bayer_format bayer_format);

td_u32 viu_get_chn_last_time_ref(td_s32 pipe_id, td_s32 chn_id);

td_void viu_update_chn_last_time_ref(td_s32 pipe_id, td_s32 chn_id, td_u32 time_ref);

td_bool viu_lost_repeat_frame(td_s32 pipe_id);

td_bool viu_wdr_offline(td_s32 pipe_id, ext_isp_wdr_mode wdr_mode);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* VIU_H */

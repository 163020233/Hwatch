/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
* Description:vb queue ext header file
* Author: AuthorNameMagicTag
* Create: 2025-06-30
*/
#ifndef __EXT_VB_STRUCT_H__
#define __EXT_VB_STRUCT_H__

#include "drv_mcp_dependency.h"
#include "drv_vb_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

/*
 * to_vb_drv_buffer() - cast 'ext_vb_buffer' to 'ext_vb_drv_buffer'
 * 将'ext_vb_buffer'类型指针转换为'ext_vb_drv_buffer'类型指针
 * 驱动模块可以通过该接口, 转换得到'ext_vb_drv_buffer'结构体
 *
 */
#define to_ext_vb_drv_buffer(vb) \
    osal_container_of(vb, ext_vb_drv_buffer, vb_buf)

typedef struct _vb_queue  ext_vb_queue;
typedef struct _vb_buffer ext_vb_buffer;

/**
 * vb_fence_ops - vb fence同步操作结构体描述;
 * @create_timeline:        创建sync_timeline;
 * @destroy_timeline:       销毁sync_timeline;
 * @create_fence_by_fd:     通过文件fd方式创建fence;
 * @destroy_fence:          销毁fence;
 * @add_callback:           添加回调函数到fence;
 * @del_callback:           从fence移除回调函数;
 * @signal:                 执行fence signal操作;
 * @has_error:              查询fence是否产生错误;
 * @sync_fd:                获取fence fd描述符;
 * @put:                    fence引用计数-1操作;
 *
 */
typedef td_void (*vb_fence_cb_func)(td_void *);

typedef struct {
    td_s32   (*create_timeline)(td_void **obj, td_void *priv);
    td_void  (*destroy_timeline)(td_void *obj, td_void *priv);
    td_s32   (*create_fence)(td_void **obj, td_void *priv);
    td_s32   (*create_fence_by_fd)(td_void **obj, td_s32 fd, td_void *priv);
    td_void  (*fd_install)(td_void *obj, td_void *priv);
    td_void  (*destroy_fence)(td_void *obj, td_void *priv);
    td_s32   (*add_callback)(td_void *obj, vb_fence_cb_func cb, td_void *priv);
    td_s32   (*del_callback)(td_void *obj, td_void *priv);
    td_void  (*signal)(td_void *obj, td_bool error, td_void *priv);
    td_bool  (*is_signaled)(td_void *obj, td_void *priv);
    td_bool  (*has_error)(td_void *obj, td_void *priv);
    td_s32   (*sync_fd)(td_void *obj, td_void *priv);
    td_void  (*put)(td_void *obj, td_void *priv);
} vb_fence_ops;

/**
 * ext_vb_ops - vb queue操作结构体描述;
 * @queue_setup:     vb_queue_reqbufs中被调用，可查询用户配置的队列信息及buf个数;
 * @wait_prepare:    vb_queue_dqbuf中被调用，可执行unlock锁操作，避免dqbuf操作死锁;
 * @wait_finish:     vb_queue_dqbuf中被调用，可执行lock锁操作，与wait_prepare匹配操作;
 * @buf_prepare:     vb_queue_qbuf中被调用，可查询已分配的vb buf信息;
 * @buf_finish:      vb_queue_dqbuf中被调用，可查询即将返回用户空间的vb buf信息;
 * @start_streaming: vb_queue_streamon中被调用，通知驱动启动处理, 返回成功后开始enqueue;
 * @stop_streaming:  vb_queue_streamoff中被调用，可执行队列清理退出等操作;
 * @buf_enqueue:     vb_queue_streamon或vb_queue_qbuf中被调用，执行vb enqueue相关操作;
 *
 * 注意，业务驱动必须实现queue_setup与buf_enqueue
 *
 */
typedef struct {
    td_s32  (*queue_setup)(ext_vb_queue *q, td_u32 num_buffers);

    td_void (*wait_prepare)(ext_vb_queue *q);
    td_void (*wait_finish)(ext_vb_queue *q);

    td_s32  (*buf_prepare)(ext_vb_buffer *vb);
    td_void (*buf_finish)(ext_vb_buffer *vb);

    td_s32  (*start_streaming)(ext_vb_queue *q);
    td_void (*stop_streaming)(ext_vb_queue *q);

    td_s32  (*buf_enqueue)(ext_vb_buffer *vb);
} ext_vb_ops;

/**
 * ext_vb_buf_state - vb buf状态描述;
 * @EXT_VB_BUF_STATE_DEQUEUED:   内部使用, buf处于初始状态;
 * @EXT_VB_BUF_STATE_PREPARING:  内部使用, buf处于准备过程状态;
 * @EXT_VB_BUF_STATE_PREPARED:   内部使用, buf处于准备完成状态;
 * @EXT_VB_BUF_STATE_QUEUED:     内部使用, buf已被加入queued_list;
 * @EXT_VB_BUF_STATE_ACTIVE:     内部使用, buf已被enqueue到drv module;
 * @EXT_VB_BUF_STATE_DONE:       vb_queue_buf_done中被使用, buf将返回至用户空间;
 * @EXT_VB_BUF_STATE_ERROR:      vb_queue_buf_done中被使用, 用于退出清理, buf将被归还done_list;
 *
 * 注意，业务驱动必须实现queue_setup与buf_enqueue
 *
 */
typedef enum {
    EXT_VB_BUF_STATE_DEQUEUED,
    EXT_VB_BUF_STATE_PREPARING,
    EXT_VB_BUF_STATE_PREPARED,
    EXT_VB_BUF_STATE_QUEUED,
    EXT_VB_BUF_STATE_ACTIVE,
    EXT_VB_BUF_STATE_DONE,
    EXT_VB_BUF_STATE_ERROR,
} ext_vb_buf_state;

typedef struct {
    /* 内存类型 */
    ext_vb_memory_type  memory;

    /* 逻辑设备使用信息 */
    td_u64 phyaddr;
    td_u32 size;

    /* DMABUF类型申请/释放使用 */
    td_s64   dma_handle;
    td_void *dma_buf;
} ext_vb_plane;

typedef struct {
    td_u32          length;    /* the max lenght of ring buffer    */
    td_char         *viraddr;  /* the address of ring buffer       */
    td_u32          read_pos;  /* the read pointer of ring buffer  */
    td_u32          write_pos; /* the write pointer of ring buffer */
    td_u32          butt_pos;  /* leave a blank for circle round   */
    mcp_osal_spinlock   lock;      /* the lock                         */
    mcp_osal_wait       wait;      /* wait for read                    */
    td_bool         error;     /* generate an error for exiting    */
} ext_vb_log_buffer;

typedef struct {
    td_u32          cnt_queue_try;
    td_u32          cnt_enqueue_try;
    td_u32          cnt_done_try;
    td_u32          cnt_dequeue_try;

    td_u32          cnt_queue_ok;
    td_u32          cnt_enqueue_ok;
    td_u32          cnt_done_ok;
    td_u32          cnt_dequeue_ok;
} ext_vb_debug;

/**
 * _vb_queue - vb queue结构体描述;
 * @name:                   指定vb queue名称, 当前用于PROC跟踪调试，输入参数;
 * @lock:                   drv module上下文mutex锁, 若不涉及设置为NULL, 输入参数;
 * @ops:                    vb queue接口操作集, 需要drv module实现对应的接口, 输入参数;
 * @drv_priv:               cookie, 可用于记录drv module上下文指针,
 *                          ops回调中使用, 输入参数;
 * @buf_struct_size:        值为0则使用默认大小sizeof(ext_vb_drv_buffer)分配vb,
 *                          值不为0则使用自定义大小, 输入参数;
 * @min_buffers_needed:     drv module需要的最小buf数目, 输入参数;
 * @memory:                 buf的内存类型, 输入参数;
 * @bufs:                   记录已分配的vb buf, 内部参数;
 * @num_buffers:            vb buf数目, 内部参数;
 * @idle_list:              队列, 存放状态为queued的vb buf, 内部参数;
 * @owned_by_drv_count:     drv module持有的vb buf数目, 内部参数;
 * @done_list:              队列, 存放状态为done的vb buf, 内部参数;
 * @done_lock:              spinlock锁, 保护done_list队列, 内部参数;
 * @done_wq:                dqbuf阻塞操作, 内部参数;
 * @dfc:                    fence操作相关, 内部参数;
 * @error:                  指示vb queue是否发生致命错误, 内部参数;
 * @waiting_in_dqbuf:       指示vb queue正在wait buf done, 内部参数;
 * @ctx_proc_name:          debugfs proc调试, 记录vb queue上下文信息, 内部参数;
 * @log_proc_name:          debugfs proc调试, 记录vb queue日志过程, 内部参数;
 * @log_buf:                debugfs proc调试, 日志处理, 内部参数;
 * @cnt_queued:             queued成功计数, 内部参数;
 * @cnt_enqueued:           enqueued成功计数, 内部参数;
 * @cnt_done:               done成功计数, 内部参数;
 * @cnt_dequeued:           dequeued成功计数, 内部参数;
 *
 */
#define VB_MAX_FRAME	  (32)
#define VB_MAX_PROC_NAME  (32)
#define VB_MAX_QUEUE_NAME (16)
struct _vb_queue {
    td_char                 name[VB_MAX_QUEUE_NAME];

    mcp_osal_mutex              *lock;

    const ext_vb_ops        *ops;

    td_void                 *drv_priv;
    td_u32                  buf_struct_size;

    ext_vb_memory_type      memory;

    /* private: internal use only */
    ext_vb_buffer           *bufs[VB_MAX_FRAME];
    td_u32                  num_buffers;

    mcp_osal_list_head   idle_list;

    mcp_osal_atomic             owned_by_drv_count;
    mcp_osal_list_head   done_list;
    mcp_osal_spinlock           queue_lock;
    mcp_osal_wait               done_wq;
    mcp_osal_wait               exit_wq;

    td_void                 *timeline; /* dma fence sync timeline */
    const vb_fence_ops      *fence_ops;

    td_bool                 error;
    td_bool                 streaming;
    td_bool                 waiting_in_dqbuf;

    td_char                 ctx_proc_name[VB_MAX_PROC_NAME];
    td_char                 log_proc_name[VB_MAX_PROC_NAME];
    ext_vb_log_buffer       log_buf;

    ext_vb_debug            vb_debug;
};

/**
 * struct _vb_buffer - vb buffer结构体描述;
 * @index:              buffer索引号;
 * @memory:             buffer内存类型;
 * @queue:              buffer所属的vb queue;
 * @state:              buffer状态;
 * @plane:              buffer地址信息;
 * @queued_entry:       queued队列链表entry;
 * @done_entry:         done队列链表entry;;
 * @dfh_in:             in fence操作entry;
 * @dfh_out:            out fence操作entry;
 * @fence_cb_lock:      spinlock锁, 保护callback过程;
 *
 */
struct _vb_buffer {
    /* private: internal use only */
    td_u32                index;
    td_u32                memory;
    ext_vb_queue          *queue;

    ext_vb_buf_state      state;

    ext_vb_plane          plane;
    mcp_osal_list_head idle_entry;
    mcp_osal_list_head done_entry;

    td_void               *dfh_in;  /* dma in fence handle entry */
    td_void               *dfh_out; /* dma out fence handle entry */
    mcp_osal_spinlock         fence_cb_lock;
};

/**
 * ext_vb_drv_buffer - vb drv buf结构体描述;
 * @vb_buf:     记录vb buf描述信息, 该参数必须位于结构体的第一个成员!!!;
 * @video:      记录视频帧描述信息
 * @supplement: 记录视频帧附加信息
 *
 * 注意: 驱动模块可以将ext_vb_drv_buffer作为自己的任务描述结构体,
 * 如果需要扩展一些其它参数, 但必须保证ext_vb_buffer类型参数位于结构体
 * 第一个成员位置, 并必须指定q->buf_struct_size值.
 *
 * for example:
 * typedef struct {
 *     ext_vb_drv_buffer drv_buf; //param must be first
 *     struct osal_list_head list;
 *     ... ;
 * } xxx_vb_buffer;
 *
 * q->buf_struct_size = sizeof(xxx_vb_buffer);
 *
 */
typedef struct {
    /* 'ext_vb_buffer' type param must be first */
    ext_vb_buffer           vb_buf;

    /* drivers are required to populate these fields, return to userspace */
    ext_vb_video            video;
    ext_vb_supplement       supplement;
} ext_vb_drv_buffer;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __EXT_VB_STRUCT_H__ */


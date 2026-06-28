/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
* Description:vb queue header file
* Author: AuthorNameMagicTag
* Create: 2021-9-11
*/

#ifndef __VB_QUEUE_H__
#define __VB_QUEUE_H__

#include "drv_ext_vb_struct.h"

/**
 * vb_queue_init() - 初始化vb queue
 * @q: 'ext_vb_queue'类型指针, 用法参考结构体说明.
 *
 * 'ext_vb_queue'结构体在驱动模块的上下文中被分配, 使用前必须设置初始化值.
 * 其中, q->ops->queue_setup, q->ops->buf_enqueue要求驱动必须实现这类接口,
 * 内部使用的私有参数, 驱动模块不需要关心, 初始化要求赋值为0
 *
 */
td_s32 vb_queue_init(ext_vb_queue *q);

/**
 * vb_queue_release() - 销毁vb queue
 * @q: 'vb_queue_init()'返回成功后的vb queue指针.
 *
 * 调用该接口前, 需要先执行'vb_queue_streamoff', 让驱动主动释放占用的buf,
 * 如果驱动没有释放buf, 直接调用'vb_queue_release'方法可能会导致异常.
 *
 */
td_void vb_queue_release(ext_vb_queue *q);

/**
 * vb_queue_buf_done() - vb buf返回vb queue
 * @q:     'vb_queue_init()'返回成功后的vb queue指针.
 * @state: 'ext_vb_buf_state'类型参数, 用法参考结构体说明.
 *
 * 硬件任务完成后通过该接口将vb buf送入vb queue.
 * 如果驱动模块退出时需要归还vb buf, 'state'状态需要配置为ERROR.
 * 如果驱动模块完成了vb buf处理并期望返回用户, 'state'状态需要配置为DONE.
 * 其它类型的'state'不支持.
 */
td_void vb_queue_buf_done(ext_vb_buffer *vb, ext_vb_buf_state state);

/**
 * vb_queue_reqbufs() - 请求vb buf
 *
 * @q: 'vb_queue_init()'返回成功后的vb queue指针.
 * @req: 'ext_vb_requestbuffers'类型参数, 用法参考结构体说明.
 *
 * 用户期望请求的vb buf个数, vb queue会根据实际的req数目进行内部buf分配,
 * 最大的requestbuffers见'VB_MAX_FRAME'定义.
 *
 */
td_s32 vb_queue_reqbufs(ext_vb_queue *q, ext_vb_requestbuffers *req);

/**
 * vb_queue_querybuf() - 查询vb信息
 *
 * @q: 'vb_queue_init()'返回成功后的vb queue指针.
 * @user_vb: 查询成功返回的vb buf信息描述结构体
 *
 * 获取指定的vb buf相关信息
 *
 */
td_s32 vb_queue_querybuf(ext_vb_queue *q, ext_vb_user_buffer *user_vb);

/**
 * vb_queue_qbuf() - vb queue入队操作
 * @q:       'vb_queue_init()'返回成功后的vb queue指针.
 * @user_vb: 'ext_vb_user_buffer'参数类型, 用法参考结构体说明.
 *
 * 驱动模块qbuf操作接口, 必须保证'q'参数信息的有效性.
 * 如果驱动实现了q->ops->buf_prepare, vb buf准备就绪时会发起回调
 * 如果驱动已调用'vb_queue_streamon', vb buf将直接enqueue到驱动
 */
td_s32 vb_queue_qbuf(ext_vb_queue *q, ext_vb_user_buffer *user_vb);

/**
 * vb_queue_dqbuf() - vb queue出队
 * @q:       'vb_queue_init()'返回成功后的vb queue指针.
 * @user_vb: 'ext_vb_user_buffer'参数类型, 用法参考结构体说明, vb buf返回给用户.
 * @msec:    阻塞超时timeout, 单位ms; -1为阻塞等待.
 *
 * 如果驱动实现了q->ops->buf_finish, vb buf完成时会发起回调.
 * 如果驱动实现了q->ops->wait_prepare/wait_finish, 将被回调用于lock/unlock.
 * user_vb中的video与supplement信息将被填充，返回给用户.
 *
 */
td_s32 vb_queue_dqbuf(ext_vb_queue *q, ext_vb_user_buffer *user_vb, td_s32 msec);

/**
 * vb_queue_streamon - 启动vb queue
 * @q: 'vb_queue_init()'返回成功后的vb queue指针.
 *
 * 如果驱动实现了q->ops->start_streaming, 将被回调.
 * 如果驱动已经执行过'vb_queue_qbuf', 队列中的vb buf将被enqueue.
 * 支持多次streamon, 接口直接返回成功
 *
 */
td_s32 vb_queue_streamon(ext_vb_queue *q);

/**
 * vb_queue_streamoff - 停止vb queue
 * @q: 'vb_queue_init()'返回成功后的vb queue指针.
 *
 * 如果驱动实现了q->ops->stop_streaming, 将被回调.
 * vb queue内部队列reset, 支持再次调用'vb_queue_streamon'.
 * 支持多次streamon, 接口直接返回成功.
 *
 */
td_s32 vb_queue_streamoff(ext_vb_queue *q);

#endif

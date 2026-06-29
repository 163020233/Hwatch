/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#ifndef __MPI_VDEC_OSAL_H__
#define __MPI_VDEC_OSAL_H__

#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef pthread_mutex_t vdec_mutex;
typedef pthread_t       vdec_thread;
#define vdec_uapi_unused(x) ((x) = (x))

#define VDEC_MUTEX_INITIALIZER        PTHREAD_MUTEX_INITIALIZER
#define vdec_mutex_lock(mutex)        (td_void)pthread_mutex_lock(&(mutex))
#define vdec_mutex_unlock(mutex)      (td_void)pthread_mutex_unlock(&(mutex))
#define vdec_mutex_init(mutex)        (td_void)pthread_mutex_init(&(mutex), TD_NULL)
#define vdec_mutex_destroy(mutex)     (td_void)pthread_mutex_destroy(&(mutex))
#define vdec_cond_init(cond)          (td_void)pthread_cond_init(&(cond), TD_NULL)
#define vdec_cond_destroy(cond)       (td_void)pthread_cond_destroy(&(cond))
#define vdec_cond_broadcast(cond)     (td_void)pthread_cond_broadcast(&(cond))
#define vdec_cond_wait(cond, mutex)   (td_void)pthread_cond_wait(&(cond), &(mutex))


#define vdec_thread_create(thread, attr, func, args) pthread_create(thread, attr, func, args)
#define vdec_thread_join(tid, status)                pthread_join(tid, status)

td_u8* vdec_mmap(td_u32 phy_buf, td_u32 size);
td_s32 vdec_umap(td_void *addr, td_u32 size);
td_void vdec_msleep(td_u32 ms);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

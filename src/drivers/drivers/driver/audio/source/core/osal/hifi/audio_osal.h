/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: API of audio debug
 * Author: audio
 * Create: 2019-12-19
 */

#ifndef __AUDIO_OSAL_H__
#define __AUDIO_OSAL_H__

#include "chip_type.h"
#include "audio_type.h"
#include "audio_struct.h"

#include <string.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

// semaphore api
typedef struct {
    td_void *sem;
} audio_osal_semaphore;

// mutex api
typedef struct {
    td_void *mutex;
} audio_osal_mutex;

// irq lock restore
td_u32 audio_osal_irq_lock(td_void);
td_void audio_osal_irq_restore(td_u32 level);

// irq handler
typedef td_s32 (*audio_osal_irq_handler)(td_void *);

td_s32 audio_osal_irq_request(td_u32 irq, audio_osal_irq_handler handler, audio_osal_irq_handler thread_fn,
    const td_char *name, td_void *dev);
td_void audio_osal_irq_free(td_u32 irq, td_void *dev);
td_s32 audio_osal_irq_enable(td_u32 irq);
td_s32 audio_osal_irq_disable(td_u32 irq);

#define audio_osal_sema_init(sem, val)
#define audio_osal_sema_destroy(sem)
#define audio_osal_sema_down(sem)
#define audio_osal_sema_up(sem)
#define audio_osal_sema_down_interruptible(sem) (0)

#define audio_osal_mutex_init(mutex)
#define audio_osal_mutex_deinit(mutex)
#define audio_osal_mutex_lock(mutex)
#define audio_osal_mutex_unlock(mutex)

td_void audio_osal_mdelay(td_u32 ms);
td_void audio_osal_udelay(td_u32 us);

#define audio_osal_msleep(ms) audio_osal_mdelay((ms))
#define audio_osal_usleep(us) audio_osal_udelay((us))
td_u32 audio_osal_gettime(td_void);
td_u32 audio_osal_cali_time_diff_us(td_u32 begin_us, td_u32 end_us);

td_void *audio_osal_kmalloc__(size_t size, const td_char *tag);
td_void *audio_osal_calloc__(size_t size, const td_char *tag);
td_void *audio_osal_realloc__(td_void *addr, size_t size, const td_char *tag);
td_void *audio_osal_kmalloc_aligned__(td_u32 size, td_u32 align, const td_char *tag);

#define audio_osal_malloc(size) malloc(size)
#define audio_osal_free(addr) free(addr)
td_void audio_osal_kfree(td_void *addr);
#define audio_osal_kmalloc(size) audio_osal_kmalloc__(size, FILE_NAME)
#define audio_osal_calloc(size) audio_osal_calloc__(size, FILE_NAME)
#define audio_osal_realloc(addr, size) audio_osal_realloc__(addr, size, FILE_NAME)

td_void audio_osal_kfree_aligned(td_void *addr);
#define audio_osal_kmalloc_aligned(size, align) audio_osal_kmalloc_aligned__(size, align, FILE_NAME)

td_void *audio_osal_address_map(td_ulong phys_addr, td_ulong size);
#define audio_osal_address_unmap(addr, map_addr)

td_uintptr_t audio_osal_virt_to_phys(td_uintptr_t virt_addr);

td_uintptr_t audio_osal_virt_to_logic_phys(td_uintptr_t virt_addr);

td_void audio_osal_dcache_region_wb(td_ulong phys_addr, td_void *virt_addr, td_ulong size);
td_void audio_osal_dcache_region_inv(td_void *addr, td_ulong size);
td_void audio_osal_icache_region_inv(td_void *addr, td_ulong size);

#define audio_osal_strncmp strncmp

td_void audio_osal_system_idle(td_void);

td_s32 audio_osal_mmz_alloc(audio_buffer *mmz_buf, td_u32 size);
td_void audio_osal_mmz_release(audio_buffer *mmz_buf);

td_void audio_osal_mmz_map(audio_buffer *mmz_buf);
td_void audio_osal_mmz_unmap(audio_buffer *mmz_buf);

td_u32 audio_osal_gettime_us(td_void);
td_u64 audio_osal_get_systick_ms(td_void);

// atomic api
typedef struct tag_audio_osal_atomic {
    td_void *atomic;
} audio_osal_atomic;

#define audio_osal_atomic_init(atomic)
#define audio_osal_atomic_destroy(atomic)
#define audio_osal_atomic_read(atomic) (0)
#define audio_osal_atomic_set(atomic, i)
#define audio_osal_atomic_inc_return(atomic) (1)
#define audio_osal_atomic_dec_return(atomic) (0)
#define audio_osal_atomic_dec(atomic)

td_void audio_osal_serial_init(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __AUDIO_OSAL_H__ */

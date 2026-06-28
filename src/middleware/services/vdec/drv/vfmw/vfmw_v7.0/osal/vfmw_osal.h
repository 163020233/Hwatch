/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __VFMW_OSAL__
#define __VFMW_OSAL__

#include "vfmw.h"
#include "securec.h"

#if defined(ENV_ARMLINUX_KERNEL)
#include "osal_ext.h"
typedef osal_irqreturn_t os_irqret;
#else
typedef td_s32 os_irqret;
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OSAL_OK  0
#define OSAL_ERR 1

#define VFMW_ACC
#define VFMW_ACC_BASE
#define VFMW_ACC_FRAME
#define VFMW_ACC_SLICE

#define OS_VA_LIST  va_list
#define OS_VA_START va_start
#define OS_VA_END   va_end

#define PRN_OS PRN_ALWS

#define VFMW_ACC_SLICE_H264 /* VFMW_ACC */
#define VFMW_ACC_SLICE_H265 /* VFMW_ACC */
#define VFMW_ACC_SLICE_VP9  /* VFMW_ACC */
#define VFMW_ACC_SLICE_AVS2 /* VFMW_ACC */
#define VFMW_ACC_SLICE_AVS3 /* VFMW_ACC */

#define VFMW_STM_THREAD_NAME "vdec_vfmw_stm"
#define VFMW_SYN_THREAD_NAME "vdec_vfmw_syn"
#define VFMW_DEC_THREAD_NAME "vdec_vfmw_dec"
#define VFMW_THREAD_WAIT_EXIT_TIME 1

#define OS_OPEN_MODE 0666
typedef enum {
    OS_RDONLY = 00000000,
    OS_WRONLY = 00000001,
    OS_RDWR = 00000002,
    OS_CREATE = 00000100,
    OS_TRUNC = 00001000,
    OS_APPEND = 00002000,
    OS_LARGEFILE = 0400000
} fo_mode;

typedef enum {
    PAGE_SIZE_4K = 0,
    PAGE_SIZE_16K = 1,
    PAGE_SIZE_64K = 2,
    PAGE_SIZE_MAX,
} os_page_size;

typedef td_void *os_file;
typedef td_s32 os_sema;
typedef td_s32 os_event;
typedef td_s32 os_atomic;
typedef td_s32 os_lock;

struct os_list_head {
    struct os_list_head *next;
    struct os_list_head *prev;
};

td_void list_add_(struct os_list_head *new_h, struct os_list_head *prev_h, struct os_list_head *next_h);
td_void list_del_(struct os_list_head *prev, struct os_list_head *next);

#define list_entry_(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

#define list_first_entry_(ptr, type, member) \
    list_entry_((ptr)->next, type, member)

#define list_next_entry_(pos, member) \
    list_entry_((pos)->member.next, __typeof__(*(pos)), member)

#define os_list_init_head(list) do { \
    (list)->next = (list);      \
    (list)->prev = (list);      \
} while (0)

#define os_list_add(new, head) \
    if ((head)->next != TD_NULL) {          \
        list_add_(new, head, (head)->next); \
    }

#define os_list_add_tail(new, head) \
    if ((head)->prev != TD_NULL) {          \
        list_add_(new, (head)->prev, head); \
    }

#define os_list_del(entry) \
    if ((entry)->prev != TD_NULL && (entry)->next != TD_NULL) { \
        list_del_((entry)->prev, (entry)->next); \
    }

#define os_list_empty(head) \
    ((head)->next == (head))

#define os_list_entry(ptr, type, member) \
    list_entry_(ptr, type, member)

#define os_list_first_entry(ptr, type, member) \
    list_first_entry_(ptr, type, member)

#define os_list_for_each_safe(pos, n, head) \
    for ((pos) = (head)->next, (n) = (pos) != TD_NULL ? (pos)->next : TD_NULL; \
         (pos) != TD_NULL && (pos) != (head); \
         (pos) = (n), (n) = (pos) != TD_NULL ? (pos)->next : TD_NULL)

#define os_list_for_each_entry(pos, head, member) \
    for ((pos) = list_first_entry_(head, __typeof__(*(pos)), member); \
         (pos) != TD_NULL && (&(pos)->member != (head)); \
         pos = list_next_entry_(pos, member))

typedef os_irqret (*vfmw_irq_handler_t)(td_s32, td_void *);
typedef td_u32 (*fn_osal_get_time_in_ms)(td_void);
typedef td_u32 (*fn_osal_get_time_in_us)(td_void);
typedef td_u64 (*fn_osal_get_time_us64)(td_void);
typedef td_s32 (*fn_osal_set_time_base)(td_u64, td_u64);
typedef td_u32 (*fn_osal_get_rand)(td_void);
typedef td_s32 (*fn_osal_get_unid)(td_void);
typedef td_s32 (*fn_osal_atomic_init)(td_s32 *, td_s32);
typedef td_void (*fn_osal_atomic_exit)(td_s32);
typedef td_void (*fn_osal_atomic_set)(td_s32, td_s32);
typedef td_s32 (*fn_osal_atomic_read)(td_s32);
typedef td_void (*fn_osal_atomic_add)(td_s32, td_s32);
typedef td_s32 (*fn_osal_atomic_add_ret)(td_s32, td_s32);
typedef td_void (*fn_osal_atomic_sub)(td_s32, td_s32);
typedef td_s32 (*fn_osal_atomic_sub_test)(td_s32, td_s32);
typedef td_s32 (*fn_osal_atomic_sub_ret)(td_s32, td_s32);
typedef td_void (*fn_osal_atomic_inc)(td_s32);
typedef td_s32 (*fn_osal_atomic_inc_test)(td_s32);
typedef td_s32 (*fn_osal_atomic_inc_ret)(td_s32);
typedef td_void (*fn_osal_atomic_dec)(td_s32);
typedef td_s32 (*fn_osal_atomic_dec_test)(td_s32);
typedef td_s32 (*fn_osal_atomic_dec_ret)(td_s32);
typedef td_s32 (*fn_osal_spin_lock_init)(td_s32 *);
typedef td_void (*fn_osal_spin_lock_exit)(td_s32);
typedef td_s32 (*fn_osal_spin_lock)(td_s32, td_ulong *);
typedef td_s32 (*fn_osal_spin_unlock)(td_s32, td_ulong *);
typedef td_s32 (*fn_osal_sema_init)(td_s32 *);
typedef td_void (*fn_osal_sema_exit)(td_s32);
typedef td_s32 (*fn_osal_sema_down)(td_s32);
typedef td_s32 (*fn_osal_sema_try)(td_s32);
typedef td_void (*fn_osal_sema_up)(td_s32);
typedef td_void *(*fn_osal_file_open)(const td_char *, td_s32, td_s32);
typedef td_void (*fn_osal_file_close)(td_void *);
typedef td_s32 (*fn_osal_file_fseek)(long long, int, td_void *);
typedef td_s32 (*fn_osal_file_read)(td_char *, td_u32, td_void *);
typedef td_s32 (*fn_osal_file_write)(const td_char *, td_u32, td_void *);
typedef td_void (*fn_osal_file_sync)(td_void *);
typedef td_void *(*fn_osal_memset)(td_void *, td_s32, td_ulong);
typedef td_void *(*fn_osal_memcpy)(td_void *, const td_void *, td_ulong);
typedef td_void *(*fn_osal_memmov)(td_void *, const td_void *, td_ulong);
typedef td_s32 (*fn_osal_memcmp)(const td_void *, const td_void *, td_u32);
typedef td_s32 (*fn_osal_log_on)(td_u32);
typedef td_void (*fn_osal_log_off)(td_void);
typedef td_void (*fn_osal_rd_log)(td_void);
typedef td_s32 (*fn_osal_wr_log)(const td_char *, ...);
typedef td_void (*fn_osal_print)(const td_char *, ...);
typedef td_s32 (*fn_osal_snprintf)(td_char *, td_ulong, const td_char *, ...);
typedef td_s32 (*fn_osal_sscanf)(const td_char *, const td_char *, ...);
typedef td_s32 (*fn_osal_strncmp)(const td_char *, const td_char *, td_u32);
typedef td_u32 (*fn_osal_strlcpy)(td_char *, const td_char *, td_u32);
typedef td_char *(*fn_osal_strsep)(td_char **, const td_char *);
typedef td_slong (*fn_osal_simple_strtol)(const td_char *, td_char **, td_s32);
typedef td_char *(*fn_osal_strstr)(const td_char *, const td_char *);
typedef td_u32 (*fn_osal_strlen)(const td_char *);
typedef td_u8 *(*fn_osal_ioremap)(UADDR, td_u32);
typedef td_void (*fn_osal_iounmap)(td_u8 *);
typedef td_void (*fn_osal_mb)(td_void);
typedef td_void (*fn_osal_udelay)(td_ulong);
typedef td_void (*fn_osal_msleep)(td_u32);
typedef td_s32 (*fn_osal_event_init)(td_s32 *, td_s32);
typedef td_void (*fn_osal_event_exit)(td_s32);
typedef td_s32 (*fn_osal_event_give)(td_s32);
typedef td_s32 (*fn_osal_event_wait)(td_s32, td_s32);
typedef td_void (*fn_osal_flush_cache)(td_void *, UADDR, td_u32);
typedef td_void (*fn_osal_flush_all_cache)(td_void);
typedef td_s32 (*fn_osal_mem_alloc)(const td_char *, td_u32, td_u32, td_u32, vfmw_mem_desc *);
typedef td_s32 (*fn_osal_mem_free)(const vfmw_mem_desc *);
typedef td_u8 *(*fn_osal_reg_map)(UADDR, td_u32);
typedef td_void (*fn_osal_reg_unmap)(td_u8 *);
typedef td_u8 *(*fn_osal_mmap)(const mem_record *);
typedef td_void (*fn_osal_munmap)(const mem_record *);
typedef td_u8 *(*fn_osal_mmap_cache)(const mem_record *);
typedef td_u8 *(*fn_osal_ns_mmap)(UADDR, td_u32);
typedef td_void (*fn_osal_ns_munmap)(td_u8 *);
typedef td_void (*fn_osal_get_page_table_addr)(UADDR *, UADDR *, UADDR *);
typedef td_void (*fn_osal_get_sec_page_table_addr)(UADDR *, UADDR *, UADDR *);
typedef td_void *(*fn_osal_create_thread)(td_void *, td_void *, td_char *);
typedef td_s32 (*fn_osal_destroy_task)(td_void *, td_bool);
typedef td_s32 (*fn_osal_kthread_should_stop)(td_void);
typedef td_s32 (*fn_osal_request_irq)(td_u32, vfmw_irq_handler_t, td_ulong, const char *, td_void *);
typedef td_void (*fn_osal_free_irq)(td_u32, const char *, td_void *);
typedef td_void *(*fn_osal_phys_to_virt)(td_ulong);
typedef td_void (*fn_osal_dump_stack)(td_void);
typedef td_void *(*fn_osal_alloc_vir_mem)(td_s32, const td_char *, td_s32);
typedef td_void (*fn_osal_free_vir_mem)(td_void *);
typedef td_s32 (*fn_osal_get_vir_mem_num)(td_void);
typedef td_s32 (*fn_osal_proc_init)(td_void);
typedef td_void (*fn_osal_proc_exit)(td_void);
typedef td_s32 (*fn_osal_proc_create)(td_char *, td_void *, td_void *, td_u32);
typedef td_void (*fn_osal_proc_destroy)(td_char *);
typedef td_void (*fn_osal_wakeup_ree)(td_void);
typedef td_u8 *(*fn_osal_mmap_mmz)(mem_record *);
typedef td_void (*fn_osal_munmap_mmz)(td_u8 *);
typedef td_void (*fn_osal_dump_proc)(td_void *, td_s32, td_s32 *, td_s8, const td_char *, ...);
typedef td_u64 (*fn_osal_do_div)(td_u64, td_u32);
typedef td_void (*fn_osal_read_osal)(td_void *, td_s32, td_s32 *);
typedef td_s32 (*fn_osal_mem_init)(td_u32, td_u32);
typedef td_s32 (*fn_osal_mem_deinit)(td_void);
typedef td_void *(*fn_osal_alloc_vir)(td_s32, const td_char *, td_s32);
typedef td_void (*fn_osal_free_vir)(td_void *);
typedef td_s32 (*fn_osal_check)(td_bool *mem_err);
typedef td_void (*fn_osal_record)(td_void);
typedef td_s32 (*fn_osal_malloc)(mem_record *);
typedef td_s32 (*fn_osal_free)(const mem_record *);
typedef td_u8 *(*fn_osal_pre_alloc_map)(mem_record *);
typedef td_void (*fn_osal_pre_alloc_unmap)(const mem_record *);
typedef td_u8 *(*fn_osal_iommu_mmap)(const mem_record *);
typedef td_void (*fn_osal_iommu_unmap)(const mem_record *);
typedef td_s32 (*fn_osal_alloc_mem)(const td_char *, td_u32, td_u32, mem_record *);
typedef td_void (*fn_osal_free_mem)(mem_record *);
typedef td_u8 *(*fn_osal_map_mem)(const mem_record *);
typedef td_void (*fn_osal_unmap_mem)(const mem_record *);
typedef td_s64 (*fn_osal_get_mem_fd)(td_void *);
typedef td_void (*fn_osal_put_mem_fd)(td_s64);
typedef td_void *(*fn_osal_get_dma_buf)(td_s64);
typedef td_void (*fn_osal_put_dma_buf)(td_void *);
typedef td_u64 (*fn_osal_get_phy)(td_void *, td_bool);
typedef td_void (*fn_osal_put_phy)(td_void *, td_u64, td_bool);
typedef td_void *(*fn_osal_kmalloc)(td_u32);
typedef td_void (*fn_osal_kfree)(td_void *);
typedef td_u32 (*fn_osal_get_page_size)(td_void);
typedef td_u32 (*fn_osal_get_page_align)(td_void);

typedef struct {
    td_ulong check1;

    fn_osal_get_time_in_ms get_time_in_ms;
    fn_osal_get_time_in_us get_time_in_us;
    fn_osal_get_time_us64 get_time_us64;
    fn_osal_set_time_base set_time_base;
    fn_osal_get_rand get_rand;
    fn_osal_get_unid get_unid;
    fn_osal_atomic_init atomic_init;
    fn_osal_atomic_exit atomic_exit;
    fn_osal_atomic_set atomic_set_;
    fn_osal_atomic_read atomic_read_;
    fn_osal_atomic_inc_ret atomic_inc_ret;
    fn_osal_atomic_dec_ret atomic_dec_ret;
    fn_osal_spin_lock_init os_spin_lock_init;
    fn_osal_spin_lock_exit os_spin_lock_exit;
    fn_osal_spin_lock os_spin_lock;
    fn_osal_spin_unlock os_spin_unlock;
    fn_osal_sema_init sema_init;
    fn_osal_sema_exit sema_exit;
    fn_osal_sema_down sema_down;
    fn_osal_sema_try sema_try;
    fn_osal_sema_up sema_up;
    fn_osal_file_open file_open;
    fn_osal_file_close file_close;
    fn_osal_file_fseek file_fseek;
    fn_osal_file_read file_read;
    fn_osal_file_write file_write;
    fn_osal_file_sync file_sync;
    fn_osal_memset os_memset;
    fn_osal_memcpy os_memcpy;
    fn_osal_memmov os_memmov;
    fn_osal_memcmp memcmp;
    fn_osal_log_on log_on;
    fn_osal_log_off log_off;
    fn_osal_rd_log rd_log;
    fn_osal_wr_log wr_log;
    fn_osal_print print;
    fn_osal_strncmp strncmp;
    fn_osal_simple_strtol simple_strtol;
    fn_osal_strlcpy strlcpy;
    fn_osal_strsep strsep;
    fn_osal_strstr strstr;
    fn_osal_strlen strlen;
    fn_osal_ioremap ioremap;
    fn_osal_iounmap iounmap;
    fn_osal_mb os_mb;
    fn_osal_udelay udelay;
    fn_osal_msleep msleep;
    fn_osal_event_init event_init;
    fn_osal_event_exit event_exit;
    fn_osal_event_give event_give;
    fn_osal_event_wait event_wait;
    fn_osal_flush_cache flush_cache;
    fn_osal_flush_all_cache flush_all_cache;
    fn_osal_mem_alloc mem_alloc;
    fn_osal_mem_free mem_free;
    fn_osal_reg_map reg_map;
    fn_osal_reg_unmap reg_unmap;
    fn_osal_mmap mmap;
    fn_osal_munmap munmap;
    fn_osal_mmap_cache mmap_cache;
    fn_osal_ns_mmap ns_mmap;
    fn_osal_ns_munmap ns_munmap;
    fn_osal_get_page_table_addr get_page_table_addr;
    fn_osal_get_sec_page_table_addr get_sec_page_table_addr;
    fn_osal_create_thread create_thread;
    fn_osal_destroy_task destroy_thread;
    fn_osal_kthread_should_stop thread_should_stop;
    fn_osal_request_irq request_irq;
    fn_osal_free_irq free_irq;
    fn_osal_phys_to_virt phys_to_virt;
    fn_osal_dump_stack dump_stack;
    fn_osal_alloc_vir_mem alloc_vir_mem;
    fn_osal_free_vir_mem free_vir_mem;
    fn_osal_get_vir_mem_num get_vir_mem_num;
    fn_osal_proc_init proc_init;
    fn_osal_proc_exit proc_exit;
    fn_osal_proc_create proc_create;
    fn_osal_proc_destroy proc_destroy;
    fn_osal_wakeup_ree wakeup_ree;
    fn_osal_mmap_mmz mmap_mmz;
    fn_osal_munmap_mmz munmap_mmz;
    fn_osal_dump_proc dump_proc;
    fn_osal_do_div os_div;
    fn_osal_read_osal read_osal;
    fn_osal_alloc_vir alloc_vir;
    fn_osal_free_vir free_vir;
    fn_osal_mem_init mem_init;
    fn_osal_mem_deinit mem_deinit;
    fn_osal_check check_mem;
    fn_osal_record record_mem;
    fn_osal_malloc malloc;
    fn_osal_free free;
    fn_osal_alloc_mem alloc_mem;
    fn_osal_free_mem free_mem;
    fn_osal_kmalloc kmalloc_mem;
    fn_osal_kfree kfree_mem;
    fn_osal_map_mem map_mem;
    fn_osal_unmap_mem unmap_mem;
    fn_osal_get_mem_fd get_mem_fd;
    fn_osal_put_mem_fd put_mem_fd;
    fn_osal_get_phy get_mem_phy;
    fn_osal_put_phy put_mem_phy;
    fn_osal_get_dma_buf get_dma_buf;
    fn_osal_put_dma_buf put_dma_buf;
    fn_osal_get_page_size get_page_size;
    fn_osal_get_page_align get_page_align;

    td_ulong check2;
} vfmw_osal_ops;

extern vfmw_osal_ops g_vfmw_osal_ops;

#define os_func_void(func, ...) if ((func) != TD_NULL) { (func)(__VA_ARGS__); }
#define os_func_ret(func, ret, ...) ((func) != TD_NULL ? (func)(__VA_ARGS__) : (ret))

#define os_get_rand                g_vfmw_osal_ops.get_rand
#define os_strstr                  g_vfmw_osal_ops.strstr
#define os_ioremap                 g_vfmw_osal_ops.ioremap
#define os_iounmap(...)            os_func_void(g_vfmw_osal_ops.iounmap, __VA_ARGS__)
#define os_get_unid                g_vfmw_osal_ops.get_unid
#define os_atomic_init             g_vfmw_osal_ops.atomic_init
#define os_atomic_exit             g_vfmw_osal_ops.atomic_exit
#define os_atomic_set              g_vfmw_osal_ops.atomic_set_
#define os_atomic_read             g_vfmw_osal_ops.atomic_read_
#define os_atomic_inc_ret          g_vfmw_osal_ops.atomic_inc_ret
#define os_atomic_dec_ret          g_vfmw_osal_ops.atomic_dec_ret
#define os_mb(...)                 os_func_void(g_vfmw_osal_ops.os_mb, __VA_ARGS__)
#define os_udelay(...)             os_func_void(g_vfmw_osal_ops.udelay, __VA_ARGS__)
#define os_msleep(...)             os_func_void(g_vfmw_osal_ops.msleep, __VA_ARGS__)
#define os_event_init              g_vfmw_osal_ops.event_init
#define os_event_exit(...)         os_func_void(g_vfmw_osal_ops.event_exit, __VA_ARGS__)
#define os_event_give(...)         os_func_void(g_vfmw_osal_ops.event_give, __VA_ARGS__)
#define os_event_wait(...)         os_func_void(g_vfmw_osal_ops.event_wait, __VA_ARGS__)
#define os_flush_cache(...)        os_func_void(g_vfmw_osal_ops.flush_cache, __VA_ARGS__)
#define os_flush_all_cache(...)    os_func_void(g_vfmw_osal_ops.flush_all_cache, __VA_ARGS__)
#define os_kmem_alloc              g_vfmw_osal_ops.mem_alloc
#define os_kmem_free(...)          os_func_void(g_vfmw_osal_ops.mem_free, __VA_ARGS__)
#define os_kmap_reg                g_vfmw_osal_ops.reg_map
#define os_kunmap_reg(...)         os_func_void(g_vfmw_osal_ops.reg_unmap, __VA_ARGS__)
#define os_kmap_mmz                g_vfmw_osal_ops.mmap_mmz
#define os_kunmap_mmz(...)         os_func_void(g_vfmw_osal_ops.munmap_mmz, __VA_ARGS__)
#define os_kns_map                 g_vfmw_osal_ops.ns_mmap
#define os_knsun_map               g_vfmw_osal_ops.ns_munmap
#define os_get_mmu_table(...)      os_func_void(g_vfmw_osal_ops.get_page_table_addr, __VA_ARGS__)
#define os_get_sec_mmu_table(...)  os_func_void(g_vfmw_osal_ops.get_sec_page_table_addr, __VA_ARGS__)
#define os_create_thread           g_vfmw_osal_ops.create_thread
#define os_destroy_thread(...)     os_func_void(g_vfmw_osal_ops.destroy_thread, __VA_ARGS__)
#define os_thread_should_stop      g_vfmw_osal_ops.thread_should_stop
#define os_request_irq             g_vfmw_osal_ops.request_irq
#define os_free_irq(...)           os_func_void(g_vfmw_osal_ops.free_irq, __VA_ARGS__)
#define os_get_vir_mem_num         g_vfmw_osal_ops.get_vir_mem_num
#define os_phys_to_virt            g_vfmw_osal_ops.phys_to_virt
#define os_dump_stack(...)         os_func_void(g_vfmw_osal_ops.dump_stack, __VA_ARGS__)
#define os_get_time_ms             g_vfmw_osal_ops.get_time_in_ms
#define os_get_time_us             g_vfmw_osal_ops.get_time_in_us
#define os_get_time_us64           g_vfmw_osal_ops.get_time_us64
#define os_set_time_base(...)      os_func_void(g_vfmw_osal_ops.set_time_base, __VA_ARGS__)
#define os_spin_lock_init(...)     os_func_void(g_vfmw_osal_ops.os_spin_lock_init, __VA_ARGS__)
#define os_spin_lock_exit(...)     os_func_void(g_vfmw_osal_ops.os_spin_lock_exit, __VA_ARGS__)
#define os_spin_lock(...)          os_func_void(g_vfmw_osal_ops.os_spin_lock, __VA_ARGS__)
#define os_spin_unlock(...)        os_func_void(g_vfmw_osal_ops.os_spin_unlock, __VA_ARGS__)
#define os_sema_init(...)          os_func_void(g_vfmw_osal_ops.sema_init, __VA_ARGS__)
#define os_sema_exit(...)          os_func_void(g_vfmw_osal_ops.sema_exit, __VA_ARGS__)
#define os_sema_down(...)          os_func_void(g_vfmw_osal_ops.sema_down, __VA_ARGS__)
#define os_sema_try                g_vfmw_osal_ops.sema_try
#define os_sema_up(...)            os_func_void(g_vfmw_osal_ops.sema_up, __VA_ARGS__)
#define os_memcmp                  g_vfmw_osal_ops.memcmp
#define os_log_on                  g_vfmw_osal_ops.log_on
#define os_log_off                 g_vfmw_osal_ops.log_off
#define os_rd_log                  g_vfmw_osal_ops.rd_log
#define os_wr_log                  g_vfmw_osal_ops.wr_log
#define os_simple_strtol           g_vfmw_osal_ops.simple_strtol
#define os_strlcpy                 g_vfmw_osal_ops.strlcpy
#define os_strsep                  g_vfmw_osal_ops.strsep
#define os_print(...)              os_func_void(g_vfmw_osal_ops.print, __VA_ARGS__)
#define os_strlen                  g_vfmw_osal_ops.strlen
#define os_strncmp                 g_vfmw_osal_ops.strncmp
#define os_fopen                   g_vfmw_osal_ops.file_open
#define os_fclose(...)             os_func_void(g_vfmw_osal_ops.file_close, __VA_ARGS__)
#define os_fread                   g_vfmw_osal_ops.file_read
#define os_fwrite                  g_vfmw_osal_ops.file_write
#define os_fseek                   g_vfmw_osal_ops.file_fseek
#define os_fsync                   g_vfmw_osal_ops.file_sync
#define os_proc_init(...)          os_func_void(g_vfmw_osal_ops.proc_init, __VA_ARGS__)
#define os_proc_exit(...)          os_func_void(g_vfmw_osal_ops.proc_exit, __VA_ARGS__)
#define os_proc_create             g_vfmw_osal_ops.proc_create
#define os_proc_destroy(...)       os_func_void(g_vfmw_osal_ops.proc_destroy, __VA_ARGS__)
#define os_dump_proc(...)          os_func_void(g_vfmw_osal_ops.dump_proc, __VA_ARGS__)
#define os_mmap                    g_vfmw_osal_ops.mmap
#define os_kmmap_cache             g_vfmw_osal_ops.mmap_cache
#define os_unmap(...)              os_func_void(g_vfmw_osal_ops.munmap, __VA_ARGS__)
#define os_wakeup_ree(...)         os_func_void(g_vfmw_osal_ops.wakeup_ree, __VA_ARGS__)
#define os_do_div                  g_vfmw_osal_ops.os_div
#define os_read_osal(...)          os_func_void(g_vfmw_osal_ops.read_osal, __VA_ARGS__)
#define os_alloc_vir               g_vfmw_osal_ops.alloc_vir
#define os_free_vir(...)           os_func_void(g_vfmw_osal_ops.free_vir, __VA_ARGS__)
#define os_mem_init(...)           os_func_void(g_vfmw_osal_ops.mem_init, __VA_ARGS__)
#define os_mem_deinit(...)         os_func_void(g_vfmw_osal_ops.mem_deinit, __VA_ARGS__)
#define os_malloc                  g_vfmw_osal_ops.malloc
#define os_free(...)               os_func_void(g_vfmw_osal_ops.free, __VA_ARGS__)
#define os_check_mem               g_vfmw_osal_ops.check_mem
#define os_record_mem              g_vfmw_osal_ops.record_mem
#define os_alloc_mem               g_vfmw_osal_ops.alloc_mem
#define os_free_mem(...)           os_func_void(g_vfmw_osal_ops.free_mem, __VA_ARGS__)
#define os_map_mem                 g_vfmw_osal_ops.map_mem
#define os_unmap_mem(...)          os_func_void(g_vfmw_osal_ops.unmap_mem, __VA_ARGS__)
#define os_get_mem_fd              g_vfmw_osal_ops.get_mem_fd
#define os_put_mem_fd(...)         os_func_void(g_vfmw_osal_ops.put_mem_fd, __VA_ARGS__)
#define os_get_phy                 g_vfmw_osal_ops.get_mem_phy
#define os_put_phy(...)            os_func_void(g_vfmw_osal_ops.put_mem_phy, __VA_ARGS__)
#define os_get_dma_buf             g_vfmw_osal_ops.get_dma_buf
#define os_put_dma_buf(...)        os_func_void(g_vfmw_osal_ops.put_dma_buf, __VA_ARGS__)
#define os_kmalloc                 g_vfmw_osal_ops.kmalloc_mem
#define os_kfree(...)              os_func_void(g_vfmw_osal_ops.kfree_mem, __VA_ARGS__)
#define os_get_page_size           g_vfmw_osal_ops.get_page_size
#define os_get_page_align          g_vfmw_osal_ops.get_page_align

td_void os_intf_init(td_void);
td_void os_intf_exit(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

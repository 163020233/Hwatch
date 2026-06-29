/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "liteos_osal.h"
#include "vfmw_osal.h"
#include "liteos_proc.h"
#include "vfmw_pdt.h"
#include "liteos_mem.h"
#include "tcxo.h"
#include "osal_adapt.h"
#include "cmsis_os2.h"

#define OS_MAX_CHAN      VFMW_CHAN_NUM
#define OS_MAX_SEMA      (OS_MAX_CHAN * 5 * 5)
#define OS_MAX_EVENT     (OS_MAX_CHAN * 4 * 1)
#define OS_MAX_ATOMIC    (OS_MAX_CHAN * 1 * 1)
#define OS_MAX_RW_LOCK   (OS_MAX_CHAN * 1 * 1)
#define OS_MAX_SPIN_LOCK (OS_MAX_CHAN * 10 * 1)
#define OS_MAX_LIST_HEAD (OS_MAX_CHAN * 1 * 1)

#define OS_CACHE_LINE_SIZE 32

#define VFMW_STACK_SIZE  (4 * 1024)

#define os_get_id_by_handle(handle)   (((td_u32)(handle)) & 0xffff)
#define os_get_unid_by_handle(handle) (((td_u32)(handle)) >> 16)
#define os_make_handle(id, unid)      ((id) + ((unid) << 16))

#define LOG_MSG_STEP_SIZE 100
#define LOG_MSG_DFLT_NUM  1000
#define LOG_MSG_DFLT_SIZE (LOG_MSG_STEP_SIZE * LOG_MSG_DFLT_NUM)
#define LOG_MSG_MAX_NUM   10000
#define LOG_MSG_MAX_SIZE  (LOG_MSG_STEP_SIZE * LOG_MSG_MAX_NUM)

typedef struct {
    td_bool rewind;
    td_u32 num;
    td_u32 max_num;
    td_char *buf;
    vfmw_mem_desc mem;
    td_char *msg[LOG_MSG_MAX_NUM];
} os_log;

static os_log *g_log = TD_NULL;
static osal_irq_spin_lock g_os_lock;
static osal_atomic g_os_atomic_unid;
static osal_atomic g_os_atomic_vm_num;
static osal_sema g_os_sema[OS_MAX_SEMA];
static osal_event_vfmw g_os_event[OS_MAX_EVENT];

static osal_irq_spin_lock g_os_spin_lock[OS_MAX_SPIN_LOCK];

static td_u16 g_sema_used[OS_MAX_SEMA] = { 0 };
static td_u16 g_event_used[OS_MAX_EVENT] = { 0 };
static osal_atomic g_os_atomic[OS_MAX_ATOMIC];
static td_u16 g_atomic_used[OS_MAX_ATOMIC] = { 0 };
static td_u16 g_spin_lock_used[OS_MAX_SPIN_LOCK] = { 0 };

static td_void liteos_spin_lock_irq(osal_irq_spin_lock *lock, td_ulong *lock_flag);
static td_void liteos_spin_unlock_irq(osal_irq_spin_lock *lock, td_ulong *lock_flag);

/* get kernel idle obj id for obj set */
STATIC td_s32 liteos_get_idle_id(td_u16 used[], td_s32 num)
{
    td_s32 id;
    STATIC td_u16 unid = 1;
    td_ulong lock_flag;

    vfmw_check_ret(used != TD_NULL, -1);
    liteos_spin_lock_irq(&g_os_lock, &lock_flag);

    for (id = 0; id < num; id++) {
        if (used[id] == 0) {
            if (unid >= 0x7000) {
                unid = 1;
            }
            used[id] = unid++;
            break;
        }
    }

    liteos_spin_unlock_irq(&g_os_lock, &lock_flag);

    if (id < num) {
        return id;
    } else {
        return -1;
    }
}

STATIC td_u32 liteos_get_time_in_ms(td_void)
{
    td_u32 curr_ms;
    td_u32 curr_us;

    osal_timeval cur_time = { 0 };
    osal_gettimeofday(&cur_time);
    curr_us = (td_u32)(cur_time.tv_sec * 1000000 + cur_time.tv_usec); /* 1000000 index */
    curr_ms = curr_us / 1000; /* 1000 index */

    return curr_ms;
}

STATIC td_u32 liteos_get_time_in_us(td_void)
{
    td_u32 curr_us;

    osal_timeval cur_time = { 0 };
    osal_gettimeofday(&cur_time);
    curr_us = (td_u32)(cur_time.tv_sec * 1000000 + cur_time.tv_usec); /* 1000000 index */

    return curr_us;
}

STATIC td_u64 liteos_get_time_us64(td_void)
{
    td_u64 sys_time;

    osal_timeval cur_time = { 0 };
    osal_gettimeofday(&cur_time);
    sys_time = (td_u64)(cur_time.tv_sec * 1000000 + cur_time.tv_usec); /* 1000000 index */

    return sys_time;
}

STATIC td_u32 liteos_get_rand(td_void)
{
    return (td_u32)rand();
}

STATIC td_s32 liteos_get_unid(td_void)
{
    return osal_atomic_inc_return(&g_os_atomic_unid);
}

STATIC td_s32 liteos_atomic_init(td_s32 *handle, td_s32 val)
{
    td_s32 id;

    vfmw_check_ret(handle != TD_NULL, OSAL_ERR);

    id = liteos_get_idle_id(g_atomic_used, OS_MAX_ATOMIC);
    if (id < 0) {
        return OSAL_ERR;
    }

    osal_atomic_set(&g_os_atomic[id], val);
    *handle = os_make_handle(id, g_atomic_used[id]);

    return OSAL_OK;
}

STATIC td_s32 liteos_strncmp(const td_char *str1, const td_char *str2, td_u32 size)
{
    vfmw_check_ret(str1 != TD_NULL, OSAL_ERR);
    vfmw_check_ret(str2 != TD_NULL, OSAL_ERR);

    return (td_s32)osal_strncmp(str1, str2, size);
}

STATIC td_u32 liteos_strlen(const td_char *str)
{
    vfmw_check_ret(str != TD_NULL, OSAL_ERR);

    return strlen(str);
}

STATIC td_s32 liteos_memcmp(const td_void *buf1, const td_void *buf2, td_u32 size)
{
    vfmw_check_ret(buf1 != TD_NULL, OSAL_ERR);
    vfmw_check_ret(buf2 != TD_NULL, OSAL_ERR);

    return (td_s32)memcmp(buf1, buf2, size);
}

STATIC td_u32 liteos_strlcpy(td_char *buf1, const td_char *buf2, td_u32 size)
{
    vfmw_check_ret(buf1 != TD_NULL, OSAL_ERR);
    vfmw_check_ret(buf2 != TD_NULL, OSAL_ERR);

    return strlcpy(buf1, buf2, size);
}

STATIC td_char *liteos_strsep(td_char **buf1, const td_char *buf2)
{
    return (td_char *)strsep(buf1, buf2);
}

STATIC td_char *liteos_strstr(const td_char *buf1, const td_char *buf2)
{
    vfmw_check_ret(buf1, TD_NULL);
    vfmw_check_ret(buf2, TD_NULL);

    return (td_char *)strstr(buf1, buf2);
}

STATIC td_void liteos_msleep(td_u32 msecs)
{
    osal_msleep(msecs);
}

STATIC td_void liteos_atomic_exit(td_s32 handle)
{
    td_s32 id = os_get_id_by_handle(handle);
    td_u32 unid = os_get_unid_by_handle(handle);
    if (handle == 0 ||
        id >= OS_MAX_ATOMIC ||
        g_atomic_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        return;
    }

    g_atomic_used[id] = 0;
}

STATIC td_void liteos_atomic_set(td_s32 handle, td_s32 val)
{
    td_s32 id = os_get_id_by_handle(handle);
    td_u32 unid = os_get_unid_by_handle(handle);
    if (id >= OS_MAX_ATOMIC ||
        g_atomic_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        return;
    }

    osal_atomic_set(&g_os_atomic[id], val);
}

STATIC td_s32 liteos_atomic_read(td_s32 handle)
{
    td_s32 id = os_get_id_by_handle(handle);
    td_u32 unid = os_get_unid_by_handle(handle);
    if (id >= OS_MAX_ATOMIC ||
        g_atomic_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        return -1;
    }

    return osal_atomic_read(&g_os_atomic[id]);
}

STATIC td_s32 liteos_atomic_inc_return(td_s32 handle)
{
    td_s32 id = os_get_id_by_handle(handle);
    td_u32 unid = os_get_unid_by_handle(handle);
    if (id >= OS_MAX_ATOMIC ||
        g_atomic_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        return -1;
    }

    return osal_atomic_inc_return(&g_os_atomic[id]);
}

STATIC td_s32 liteos_atomic_dec_return(td_s32 handle)
{
    td_s32 id = os_get_id_by_handle(handle);
    td_u32 unid = os_get_unid_by_handle(handle);
    if (id >= OS_MAX_ATOMIC ||
        g_atomic_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        return -1;
    }

    return osal_atomic_dec_return(&g_os_atomic[id]);
}

STATIC td_void *liteos_create_thread(td_void *task_func, td_void *param, td_char *task_name)
{
    osThreadAttr_t attr;
    osThreadId_t task;
    osPriority_t current_prio;

    (td_void)memset_s(&attr, sizeof(attr), 0, sizeof(attr));

    current_prio = osThreadGetPriority(osThreadGetId());
    attr.name = task_name;
    attr.stack_size = VFMW_STACK_SIZE;
    attr.priority = current_prio;
    attr.attr_bits = osThreadDetached;

    task = osThreadNew((osThreadFunc_t)task_func, param, &attr);
    if (task == TD_NULL) {
        dprint(PRN_OS, "%s osThreadNew err %s\n", __func__, task_name);
        return TD_NULL;
    }

    return (td_void *)task;
}

STATIC td_s32 liteos_destroy_thread(td_void *param, td_bool stop_flag)
{
    vfmw_unused(param);

    if (stop_flag == TD_TRUE) {
        dprint(PRN_OS, "%s stop_flag err\n", __func__);
        return OSAL_ERR;
    }

    return OSAL_OK;
}

STATIC td_s32 liteos_kthread_should_stop(td_void)
{
    return 0;
}

STATIC td_s32 liteos_request_irq(td_u32 irq, vfmw_irq_handler_t handler,
    unsigned long flags, const char *name, td_void *dev)
{
    vfmw_unused(flags);
    /* para5 (dev) must be same as last para2 (handler) of osal_free_irq */
    if (osal_irq_request(irq, (osal_irq_handler)handler, TD_NULL, name, dev) != 0) {
        dprint(PRN_ERROR, "%s request irq failed !\n", name);
        return OSAL_ERR;
    }

    osal_irq_enable(irq);

    return OSAL_OK;
}

STATIC td_void liteos_free_irq(td_u32 irq, const char *name, td_void *dev)
{
    vfmw_unused(name);

    osal_irq_disable(irq);
    osal_irq_free(irq, dev);

    return;
}

STATIC td_s32 liteos_init_event(td_s32 *handle, td_s32 init_val)
{
    td_s32 id;
    td_s32 ret;
    osal_event_vfmw *event = TD_NULL;

    vfmw_check_ret(handle != TD_NULL, OSAL_ERR);

    id = liteos_get_idle_id(g_event_used, OS_MAX_EVENT);
    if (id < 0) {
        return OSAL_ERR;
    }

    event = &g_os_event[id];
    event->flag = init_val;

    ret = osal_wait_init(&(event->wait));
    if (ret != 0) {
        g_event_used[id] = 0;
        return OSAL_ERR;
    }

    *handle = os_make_handle(id, g_event_used[id]);

    return OSAL_OK;
}

STATIC td_void liteos_exit_event(td_s32 handle)
{
    td_s32 id = os_get_id_by_handle(handle);
    td_u32 unid = os_get_unid_by_handle(handle);
    osal_event_vfmw *event = TD_NULL;

    if (handle == 0 ||
        id >= OS_MAX_EVENT ||
        g_event_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        return;
    }

    event = &g_os_event[id];
    osal_wait_destroy(&(event->wait));
    g_event_used[id] = 0;
}

STATIC td_s32 liteos_give_event(td_s32 handle)
{
    osal_event_vfmw *event = TD_NULL;
    td_s32 id = os_get_id_by_handle(handle);
    td_u32 unid = os_get_unid_by_handle(handle);
    if (id >= OS_MAX_EVENT ||
        g_event_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        return OSAL_ERR;
    }

    event = &g_os_event[id];
    event->flag = 1;
    osal_wait_wakeup(&(event->wait));

    return OSAL_OK;
}

STATIC td_s32 liteos_wait_call_back(const td_void *args)
{
    osal_event_vfmw *event = TD_NULL;

    event = (osal_event_vfmw *)args;
    vfmw_check_ret(event != TD_NULL, 0);

    return event->flag == 0 ? 0 : 1;
}

STATIC td_s32 liteos_wait_event(td_s32 handle, td_s32 ms_wait_time)
{
    td_s32 ret;
    osal_event_vfmw *event = TD_NULL;
    td_s32 id = os_get_id_by_handle(handle);
    td_u32 unid = os_get_unid_by_handle(handle);
    if (id >= OS_MAX_EVENT ||
        g_event_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        return OSAL_ERR;
    }

    event = &g_os_event[id];

    ret = osal_wait_timeout_interruptible(&event->wait, liteos_wait_call_back, event, (td_u32)ms_wait_time);
    event->flag = 0;

    return (ret == 0) ? OSAL_OK : OSAL_ERR;
}

STATIC td_u8 *liteos_ioremap(UADDR phy_addr, td_u32 len)
{
    td_void *vir_addr = TD_NULL;

    vfmw_unused(len);

    vir_addr = (td_u8 *)(uintptr_t)phy_addr;

    return vir_addr;
}

STATIC td_u8 *liteos_mmap(const mem_record *mem)
{
    td_void *vir_addr = TD_NULL;

    vfmw_check_ret(mem != TD_NULL, TD_NULL);

    vir_addr = (td_u8 *)(uintptr_t)mem->phy_addr;

    return vir_addr;
}

STATIC td_void liteos_unmap(const mem_record *mem)
{
    vfmw_unused(mem);
}

STATIC td_void liteos_iounmap(td_u8 *vir_addr)
{
    vfmw_unused(vir_addr);
    return;
}

STATIC td_void *liteos_fopen(const char *file_name, int flags, int mode)
{
    vfmw_unused(file_name);
    vfmw_unused(flags);
    vfmw_unused(mode);
    return osal_klib_fopen(file_name, flags, mode);
}

STATIC td_void liteos_fclose(td_void *fp)
{
    vfmw_unused(fp);
    osal_klib_fclose(fp);
}

STATIC td_s32 liteos_fseek(long long offset, int whence, td_void *filp)
{
    vfmw_unused(offset);
    vfmw_unused(whence);
    vfmw_unused(filp);

    return osal_klib_fseek(offset, whence, filp);
}

STATIC td_s32 liteos_fread(char *buf, td_u32 len, td_void *fp)
{
    vfmw_unused(buf);
    vfmw_unused(len);
    vfmw_unused(fp);

    return osal_klib_fread(buf, len, fp);
}

STATIC td_s32 liteos_fwrite(const char *buf, td_u32 len, td_void *fp)
{
    vfmw_unused(buf);
    vfmw_unused(len);
    vfmw_unused(fp);

    return osal_klib_fwrite(buf, len, fp);
}

STATIC td_void liteos_fsync(td_void *fp)
{
    osal_klib_fsync(fp);
}

STATIC td_s32 liteos_log_on(td_u32 msg_num)
{
    td_s32 ret;
    td_u32 i, log_size;
    td_ulong lock_flag;
    os_log *log = TD_NULL;

    log_size = LOG_MSG_DFLT_SIZE;
    if (msg_num > 0) {
        if (msg_num > LOG_MSG_MAX_NUM) {
            os_print("MsgNum(%d) out of range[1, %d].\n", msg_num, LOG_MSG_MAX_NUM);
            return OSAL_ERR;
        }
        log_size = msg_num * LOG_MSG_STEP_SIZE;
    }

    liteos_spin_lock_irq(&g_os_lock, &lock_flag);
    log = g_log;
    g_log = TD_NULL;
    liteos_spin_unlock_irq(&g_os_lock, &lock_flag);

    if (log != TD_NULL) {
        if (log->mem.length == log_size) {
            goto out;
        }
        os_kmem_free(&log->mem);
        os_free_vir(log);
        log = TD_NULL;
    }
    vfmw_check_ret(os_alloc_vir != TD_NULL, OSAL_ERR);
    log = os_alloc_vir(0, "plog", sizeof(os_log));
    if (log == TD_NULL) {
        os_print("Alloc g_log size %d failed.\n", sizeof(os_log));
        return OSAL_ERR;
    }
    vfmw_check_sec_func(memset_s(log, sizeof(os_log), 0, sizeof(os_log)));
    ret = os_kmem_alloc("vfmw_log_buf", log_size, 16, 1, &log->mem); /* 16 :a number */
    if (ret != OSAL_OK || log->mem.vir_addr == 0 || log->mem.length < log_size) {
        os_free_vir(log);
        os_print("Alloc Log buffer size %d failed.\n", log_size);
        return OSAL_ERR;
    }

    log->buf = (td_char *)(uint64_ptr(log->mem.vir_addr));
    log->max_num = log_size / LOG_MSG_STEP_SIZE;

    for (i = 0; i < log->max_num; i++) {
        log->msg[i] = log->buf + i * LOG_MSG_STEP_SIZE;
    }

    os_print("Turn on print log(%d) for msg num %d/size %d.\n", log_size, log->max_num, LOG_MSG_STEP_SIZE);

out:
    liteos_spin_lock_irq(&g_os_lock, &lock_flag);
    g_log = log;
    liteos_spin_unlock_irq(&g_os_lock, &lock_flag);

    return OSAL_OK;
}

STATIC td_void liteos_log_off(td_void)
{
    td_ulong lock_flag;
    os_log *log = TD_NULL;

    liteos_spin_lock_irq(&g_os_lock, &lock_flag);
    log = g_log;
    g_log = TD_NULL;
    liteos_spin_unlock_irq(&g_os_lock, &lock_flag);

    if (log != TD_NULL && log->mem.length > 0) {
        os_kmem_free(&log->mem);
    }

    if (log != TD_NULL) {
        os_free_vir(log);
        log = TD_NULL;
        os_print("Turn off print log.\n");
    }
}

STATIC td_s32 liteos_wr_log(const char *msg, ...)
{
    va_list args;
    td_char *start = TD_NULL;
    td_s32 wr_size = 0;
    td_ulong lock_flag;

    liteos_spin_lock_irq(&g_os_lock, &lock_flag);

    if (g_log == TD_NULL || g_log->buf == TD_NULL) {
        goto out;
    }

    start = g_log->msg[g_log->num];

    va_start(args, msg);
    wr_size = vsnprintf_s(start, LOG_MSG_MAX_NUM, LOG_MSG_STEP_SIZE - 1, msg, args);
    va_end(args);

    if (wr_size < 0 || wr_size >= LOG_MSG_STEP_SIZE) {
        os_print("print log error occur.\n");
        goto out;
    }

    start[wr_size] = '\0';
    g_log->msg[g_log->num] = start;

    if (g_log->num + 1 == g_log->max_num) {
        g_log->num = 0;
        g_log->rewind = TD_TRUE;
    } else {
        g_log->num++;
    }

out:
    liteos_spin_unlock_irq(&g_os_lock, &lock_flag);

    return wr_size;
}

STATIC td_void liteos_rd_log(td_void)
{
    td_u32 i;
    td_u32 first_num;
    td_u32 total_num;
    td_ulong lock_flag;

    liteos_spin_lock_irq(&g_os_lock, &lock_flag);

    if (g_log == TD_NULL || g_log->buf == TD_NULL) {
        os_print("Print log not enable.\n");
        goto out;
    }

    if (g_log->rewind == TD_FALSE) {
        first_num = 0;
        total_num = g_log->num;
    } else {
        first_num = g_log->num;
        total_num = g_log->max_num;
    }

    os_print("read log FirstNum %d TotalNum %d.\n", first_num, total_num);
    for (i = first_num; i < total_num; i++) {
        os_print("%s", g_log->msg[i]);
    }
    for (i = 0; i < first_num; i++) {
        os_print("%s", g_log->msg[i]);
    }

out:
    liteos_spin_unlock_irq(&g_os_lock, &lock_flag);

    return;
}

STATIC td_s32 liteos_sema_init(td_s32 *handle)
{
    td_s32 id;
    td_s32 ret;

    vfmw_check_ret(handle != TD_NULL, OSAL_ERR);

    id = liteos_get_idle_id(g_sema_used, OS_MAX_SEMA);
    if (id < 0) {
        os_print("get_idle_id err\n");
        return OSAL_ERR;
    }

    ret = osal_sem_init(&g_os_sema[id], 1);
    if (ret != 0) {
        g_sema_used[id] = 0;
        os_print("sem init err\n");
        return OSAL_ERR;
    }

    *handle = os_make_handle(id, g_sema_used[id]);

    return OSAL_OK;
}

STATIC td_void liteos_sema_exit(td_s32 handle)
{
    td_s32 id = os_get_id_by_handle(handle);
    td_u32 unid = os_get_unid_by_handle(handle);
    if (handle == 0 ||
        id >= OS_MAX_SEMA ||
        g_sema_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        return;
    }

    osal_sem_destroy(&g_os_sema[id]);
    g_sema_used[id] = 0;
}

STATIC td_s32 liteos_sema_down(td_s32 handle)
{
    td_s32 ret;
    td_s32 id = os_get_id_by_handle(handle);
    td_u32 unid = os_get_unid_by_handle(handle);
    if (id >= OS_MAX_SEMA ||
        g_sema_used[id] != unid) {
        osal_dump_stack();
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        return OSAL_ERR;
    }

    ret = osal_sem_down(&g_os_sema[id]);

    return ret;
}

STATIC td_s32 liteos_sema_try(td_s32 handle)
{
    td_s32 ret;
    td_s32 id = os_get_id_by_handle(handle);
    td_u32 unid = os_get_unid_by_handle(handle);
    if (id >= OS_MAX_SEMA ||
        g_sema_used[id] != unid) {
        dprint(PRN_OS, "%s: Handle = %x error \n", __func__, handle);
        return OSAL_ERR;
    }

    ret = osal_sem_trydown(&g_os_sema[id]);

    return ret == 0 ? OSAL_OK : OSAL_ERR;
}

STATIC td_void liteos_sema_up(td_s32 handle)
{
    td_s32 id = os_get_id_by_handle(handle);
    td_u32 unid = os_get_unid_by_handle(handle);
    if (id >= OS_MAX_SEMA ||
        g_sema_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        return;
    }

    osal_sem_up(&g_os_sema[id]);
}

STATIC td_s32 liteos_spin_lock_irq_init(osal_irq_spin_lock *lock)
{
    td_s32 ret;

    vfmw_check_ret(lock != TD_NULL, OSAL_ERR);

    ret = osal_spin_lock_init(&lock->irq_lock);
    if (ret != 0) {
        return OSAL_ERR;
    }

    lock->is_init = 1;

    return OSAL_OK;
}

static inline td_void liteos_spin_lock_irq(osal_irq_spin_lock *lock, td_ulong *lock_flag)
{
    vfmw_check(lock != TD_NULL);
    vfmw_check(lock_flag != TD_NULL);

    if (lock->is_init == 0) {
        osal_spin_lock_init(&lock->irq_lock);
        lock->is_init = 1;
    }
    osal_spin_lock_irqsave(&lock->irq_lock, lock_flag);

    return;
}

static inline td_void liteos_spin_unlock_irq(osal_irq_spin_lock *lock, td_ulong *lock_flag)
{
    vfmw_check(lock != TD_NULL);
    vfmw_check(lock_flag != TD_NULL);

    osal_spin_unlock_irqrestore(&lock->irq_lock, lock_flag);

    return;
}

#define USE_PSRAM_AS_VMALLOC
STATIC td_void *liteos_alloc_vir_mem(td_s32 chan_id, const td_char *name, td_s32 size)
{
    td_void *vir = TD_NULL;

#ifdef USE_PSRAM_AS_VMALLOC
    td_s32 ret;
    mem_record mem_rec = {0};

    vfmw_check_ret(size != 0, TD_NULL);
    mem_rec.unid = (td_u32)chan_id;
    ret = liteos_mem_alloc(name, (td_u32)size, 1, &mem_rec);
    if (ret != OSAL_OK) {
        os_print("%s, %d, err\n", __func__, __LINE__);
        return TD_NULL;
    }
    vir = (td_void *)mem_rec.vir_addr;
#else
    vfmw_check_ret(size != 0, TD_NULL);
    vir = osal_vmalloc(size);
#endif
    if (vir != TD_NULL) {
        osal_atomic_inc_return(&g_os_atomic_vm_num);
    }

    return vir;
}

STATIC td_void liteos_free_vir_mem(td_void *p)
{
    mem_record mem_rec = {0};

    if (p != TD_NULL) {
#ifdef USE_PSRAM_AS_VMALLOC
        mem_rec.phy_addr = (td_u32)ptr_uint64(p);
        mem_rec.vir_addr = p;
        liteos_mem_free(&mem_rec);
#else
        vfmw_unused(mem_rec);
        osal_vfree(p);
#endif
        osal_atomic_dec_return(&g_os_atomic_vm_num);
    } else {
        dprint(PRN_OS, "%s: p = null error\n", __func__);
    }
}

STATIC td_s32 liteos_get_vir_mem_num(td_void)
{
    return osal_atomic_read(&g_os_atomic_vm_num);
}

STATIC td_s32 liteos_alloc_mem(const td_char *name, td_u32 name_length, td_u32 len, mem_record *mem)
{
    td_s32 ret;

    vfmw_unused(name_length);
    ret = liteos_mem_alloc(name, len, 1, mem);
    if (ret != OSAL_OK) {
        os_print("%s, %d, err\n", __func__, __LINE__);
        return OSAL_ERR;
    }

    return OSAL_OK;
}

STATIC td_void liteos_free_mem(mem_record *mem)
{
    liteos_mem_free(mem);
}

STATIC td_s32 liteos_kernel_mem_malloc(const td_char *name, td_u32 len,
    td_u32 align, td_u32 cache, vfmw_mem_desc *mem)
{
    mem_record mem_rec = {0};
    td_s32 ret;

    vfmw_check_ret(name != TD_NULL, OSAL_ERR);
    vfmw_check_ret(mem != TD_NULL, OSAL_ERR);
    vfmw_unused(cache);

    mem_rec.unid = mem->unid;
    ret = liteos_mem_alloc(name, len, align, &mem_rec);
    if (ret != OSAL_OK) {
        os_print("%s, %d, err\n", __func__, __LINE__);
        return OSAL_ERR;
    }

    mem->phy_addr = mem_rec.phy_addr;
    mem->vir_addr = ptr_uint64(mem_rec.vir_addr);
    mem->length = mem_rec.length;

    return OSAL_OK;
}

STATIC td_s32 liteos_kernel_mem_free(const vfmw_mem_desc *mem)
{
    mem_record mem_rec = {0};

    vfmw_check_ret(mem != TD_NULL, OSAL_ERR);
    mem_rec.phy_addr = mem->phy_addr;
    mem_rec.vir_addr = uint64_ptr(mem->vir_addr);

    liteos_free_mem(&mem_rec);

    return OSAL_OK;
}

STATIC td_void liteos_flush_cache(td_void *ptr, UADDR phy_addr, td_u32 len)
{
#ifdef VFMW_PSRAM_CACHE_SUPPORT
    osal_dcache_region_wb((void *)ptr, (td_ulong)phy_addr, (td_u64)len + OS_CACHE_LINE_SIZE);
#else
    vfmw_unused(ptr);
    vfmw_unused(phy_addr);
    vfmw_unused(len);
#endif
}

STATIC td_void liteos_flush_all_cache(td_void)
{
#ifdef VFMW_PSRAM_CACHE_SUPPORT
    osal_dcache_flush_all();
#endif
}

STATIC td_s32 liteos_malloc(mem_record *mem)
{
    td_s32 ret = -1;
    vfmw_mem_desc vfmw_mem_desc = { 0 };
    td_void *vir = TD_NULL;

    vfmw_check_ret(mem != TD_NULL, OSAL_ERR);

    if (mem->is_cached == 0) {
        vfmw_mem_desc.unid = mem->unid;
        vfmw_mem_desc.length = mem->length;
        ret = liteos_kernel_mem_malloc(mem->mem_name, mem->length, 16, 0, &vfmw_mem_desc); /* 16 :a number */
        mem->phy_addr = vfmw_mem_desc.phy_addr;
        mem->vir_addr = uint64_ptr(vfmw_mem_desc.vir_addr);
        mem->length = vfmw_mem_desc.length;
    } else {
        vir = liteos_alloc_vir_mem((td_s32)mem->unid, mem->mem_name, (td_s32)mem->length);
        ret = (vir == TD_NULL) ? OSAL_ERR : OSAL_OK;
        mem->phy_addr = (UADDR)ptr_uint64(vir);
        mem->vir_addr = (td_u8 *)(vir);
    }

    return ret;
}

STATIC td_s32 liteos_free(const mem_record *mem)
{
    td_s32 ret = -1;
    vfmw_mem_desc vfmw_mem_desc = { 0 };

    vfmw_check_ret(mem != TD_NULL, OSAL_ERR);
    if (mem->is_cached == 0) {
        vfmw_mem_desc.length = mem->length;
        vfmw_mem_desc.vir_addr = ptr_uint64(mem->vir_addr);
        vfmw_mem_desc.phy_addr = (mem->phy_addr != 0) ? mem->phy_addr : (UADDR)ptr_uint64(mem->vir_addr);
        ret = liteos_kernel_mem_free(&vfmw_mem_desc);
    } else {
        vfmw_check_ret(mem->vir_addr != TD_NULL, OSAL_ERR);
        liteos_free_vir_mem(mem->vir_addr);
        ret = OSAL_OK;
    }

    return ret;
}

#ifdef VFMW_CHECK_MEM_SUPPORT
STATIC td_s32 liteos_check_mem(td_bool *mem_err)
{
    *mem_err = liteos_mem_check();
    return OSAL_OK;
}

STATIC td_void liteos_record_mem(td_void)
{
    liteos_mem_record();
}
#endif

STATIC td_s32 liteos_spin_lock_init(td_s32 *handle)
{
    td_s32 id;
    td_s32 ret;

    vfmw_check_ret(handle != TD_NULL, OSAL_ERR);

    id = liteos_get_idle_id(g_spin_lock_used, OS_MAX_SPIN_LOCK);
    if (id < 0) {
        return OSAL_ERR;
    }

    ret = liteos_spin_lock_irq_init(&g_os_spin_lock[id]);
    if (ret != OSAL_OK) {
        g_spin_lock_used[id] = 0;
        return OSAL_ERR;
    }

    *handle = os_make_handle(id, g_spin_lock_used[id]);

    return OSAL_OK;
}

STATIC td_void liteos_spin_lock_exit(td_s32 handle)
{
    td_s32 id = os_get_id_by_handle(handle);
    td_u32 unid = os_get_unid_by_handle(handle);
    if (handle == 0 ||
        id >= OS_MAX_SPIN_LOCK ||
        g_spin_lock_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        return;
    }

    osal_spin_lock_destroy(&g_os_spin_lock[id].irq_lock);

    g_spin_lock_used[id] = 0;
}

STATIC td_s32 liteos_spin_lock(td_s32 handle, td_ulong *lock_flag)
{
    td_s32 id = os_get_id_by_handle(handle);
    td_u32 unid = os_get_unid_by_handle(handle);
    if (id >= OS_MAX_SPIN_LOCK ||
        g_spin_lock_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        return OSAL_ERR;
    }

    liteos_spin_lock_irq(&g_os_spin_lock[id], lock_flag);

    return OSAL_OK;
}

STATIC td_s32 liteos_spin_unlock(td_s32 handle, td_ulong *lock_flag)
{
    td_s32 id = os_get_id_by_handle(handle);
    td_u32 unid = os_get_unid_by_handle(handle);
    if (id >= OS_MAX_SPIN_LOCK ||
        g_spin_lock_used[id] != unid) {
        dprint(PRN_OS, "%s: Handle = %x error \n", __func__, handle);
        return OSAL_ERR;
    }

    liteos_spin_unlock_irq(&g_os_spin_lock[id], lock_flag);

    return OSAL_OK;
}

STATIC td_void liteos_mb(td_void)
{
    osal_mb();
}

STATIC td_void liteos_udelay(unsigned long usecs)
{
    /* osal_udelay(usecs) not support in spin lock! */
    uapi_tcxo_delay_us((td_u32)usecs);
}

STATIC td_u64 liteos_do_div(td_u64 dividend, td_u32 divison)
{
    if (divison == 0) {
        return 0;
    }

    return dividend / divison;
}

STATIC td_void* liteos_kmalloc_mem(td_u32 size)
{
    return osal_kmalloc(size, OSAL_GFP_ATOMIC);
}

STATIC td_void liteos_kfree_mem(td_void *vir)
{
    return osal_kfree(vir);
}

STATIC td_void osal_intf_init_part1(vfmw_osal_ops *ops)
{
    ops->get_time_in_ms = liteos_get_time_in_ms;
    ops->get_time_in_us = liteos_get_time_in_us;
    ops->get_time_us64 = liteos_get_time_us64;
    ops->get_rand = liteos_get_rand;
    ops->get_unid = liteos_get_unid;
    ops->atomic_init = liteos_atomic_init;
    ops->atomic_exit = liteos_atomic_exit;
    ops->atomic_set_ = liteos_atomic_set;
    ops->atomic_read_ = liteos_atomic_read;
    ops->atomic_inc_ret = liteos_atomic_inc_return;
    ops->atomic_dec_ret = liteos_atomic_dec_return;
    ops->os_spin_lock_init = liteos_spin_lock_init;
    ops->os_spin_lock_exit = liteos_spin_lock_exit;
    ops->os_spin_lock = liteos_spin_lock;
    ops->os_spin_unlock = liteos_spin_unlock;
    ops->sema_init = liteos_sema_init;
    ops->sema_exit = liteos_sema_exit;
    ops->sema_down = liteos_sema_down;
    ops->sema_try = liteos_sema_try;
    ops->sema_up = liteos_sema_up;
    ops->file_open = liteos_fopen;
    ops->file_close = liteos_fclose;
    ops->file_fseek = liteos_fseek;
    ops->file_read = liteos_fread;
    ops->file_write = liteos_fwrite;
    ops->file_sync = liteos_fsync;
    ops->memcmp = liteos_memcmp;
    ops->log_on = liteos_log_on;
    ops->log_off = liteos_log_off;
    ops->rd_log = liteos_rd_log;
    ops->wr_log = liteos_wr_log;
#ifdef VFMW_DPRINT_SUPPORT
    ops->print = osal_printk;
#endif
    ops->strncmp = liteos_strncmp;
    ops->strlen = liteos_strlen;
    ops->strlcpy = liteos_strlcpy;
    ops->strsep = liteos_strsep;
    ops->strstr = liteos_strstr;
    ops->msleep = liteos_msleep;
}

STATIC td_void osal_intf_init_part2(vfmw_osal_ops *ops)
{
    ops->os_mb = liteos_mb;
    ops->udelay = liteos_udelay;
    ops->event_init = liteos_init_event;
    ops->event_exit = liteos_exit_event;
    ops->event_give = liteos_give_event;
    ops->event_wait = liteos_wait_event;
    ops->flush_cache = liteos_flush_cache;
    ops->flush_all_cache = liteos_flush_all_cache;
    ops->mem_alloc = liteos_kernel_mem_malloc;
    ops->mem_free = liteos_kernel_mem_free;
    ops->reg_map = liteos_ioremap;
    ops->reg_unmap = liteos_iounmap;
    ops->mmap = liteos_mmap;
    ops->munmap = liteos_unmap;
    ops->create_thread = liteos_create_thread;
    ops->destroy_thread = liteos_destroy_thread;
    ops->thread_should_stop = liteos_kthread_should_stop;
    ops->phys_to_virt = (fn_osal_phys_to_virt)osal_phys_to_virt;
    ops->alloc_vir_mem = liteos_alloc_vir_mem;
    ops->free_vir_mem = liteos_free_vir_mem;
    ops->get_vir_mem_num = liteos_get_vir_mem_num;
    ops->request_irq = liteos_request_irq;
    ops->free_irq = liteos_free_irq;
    ops->kmalloc_mem = liteos_kmalloc_mem;
    ops->kfree_mem = liteos_kfree_mem;
    ops->ioremap = liteos_ioremap;
    ops->iounmap = liteos_iounmap;
}

td_void osal_intf_init(vfmw_osal_ops *ops)
{
    td_s32 ret;

    vfmw_check(ops != TD_NULL);
    vfmw_check_sec_func(memset_s(ops, sizeof(vfmw_osal_ops), 0, sizeof(vfmw_osal_ops)));

    ops->check1 = 0x1234abcd;
    ops->check2 = 0x1234abcd;

    ret = liteos_spin_lock_irq_init(&g_os_lock);
    if (ret != OSAL_OK) {
        dprint(PRN_OS, "%s call liteos_spin_lock_irq_init failed!\n", __func__);
    }

    osal_atomic_set(&g_os_atomic_unid, 1);
    osal_atomic_set(&g_os_atomic_vm_num, 0);

    osal_intf_init_part1(ops);
    osal_intf_init_part2(ops);
    liteos_mem_init();

#ifdef VDEC_PROC_SUPPORT
    ops->proc_init = (fn_osal_proc_init)liteos_proc_init;
    ops->proc_exit = (fn_osal_proc_exit)liteos_proc_exit;
    ops->proc_create = (fn_osal_proc_create)liteos_proc_create;
    ops->proc_destroy = (fn_osal_proc_destroy)liteos_proc_destroy;
    ops->dump_proc = (fn_osal_dump_proc)liteos_proc_dump;
#endif
    ops->os_div = liteos_do_div;
    ops->malloc = liteos_malloc;
    ops->free = liteos_free;
#ifdef VFMW_CHECK_MEM_SUPPORT
    ops->check_mem = liteos_check_mem;
    ops->record_mem = liteos_record_mem;
#endif
    ops->alloc_mem = liteos_alloc_mem;
    ops->free_mem = liteos_free_mem;
    ops->map_mem = liteos_mmap;
    ops->unmap_mem = liteos_unmap;
}

td_void osal_intf_exit(vfmw_osal_ops *ops)
{
    liteos_log_off();
    liteos_mem_deinit();
    osal_spin_lock_destroy(&g_os_lock.irq_lock);
    vfmw_check_sec_func(memset_s(ops, sizeof(vfmw_osal_ops), 0, sizeof(vfmw_osal_ops)));
}


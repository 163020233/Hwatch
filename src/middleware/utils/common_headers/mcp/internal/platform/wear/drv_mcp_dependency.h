/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: dis source file define
 * Author: AuthorNameMagicTag
 * Create: 2021-8-28
 */

#ifndef __DRV_MCP_DEPENDENCY__
#define __DRV_MCP_DEPENDENCY__

#include "drv_mcp_module.h"
#include "drv_mcp_errno.h"
#include "drv_mcp_type.h"
#include "drv_mcp_log.h"
#include "osal_ioctl.h"
#include "drv_mcp_math.h"
#include "securec.h"
#include "soc_osal.h"
#include "chip_core_irq.h"     // irq num
#include "tcxo.h"

#if !defined(CONFIG_MCP_UT)
#define MODULE_STATIC static
#define MODULE_INLINE inline
#else
#define MODULE_STATIC
#define MODULE_INLINE
#endif

#define MCP_PROC_CMD_SINGEL_LENGTH_MAX 32
#define MCP_PROC_CMD_NUM_MAX           16
#define MCP_PROC_CMD_ALL_LENGTH_MAX    128

#define MCP_DEV_VGS_MINOR              0
#define MCP_DEV_VB_MINOR               0
#define MCP_DEV_PPU_MINOR              0
#define MCP_DEV_MIPIRX_MINOR           0
#define MCP_DEV_VIU_MINOR              0
#define MCP_DEV_ISP_MINOR              0
#define MCP_DEV_DIS_MINOR              0

// kfile
#define MCP_OSAL_O_ACCMODE             00000003
#define MCP_OSAL_O_RDONLY              00000000
#define MCP_OSAL_O_WRONLY              00000001
#define MCP_OSAL_O_RDWR                00000002
#define MCP_OSAL_O_CREAT               00000100
#define MCP_OSAL_O_APPEND              00002000
#define MCP_OSAL_O_TRUNC               00001000
#define MCP_OSAL_O_CLOEXEC             02000000

#define MCP_OSAL_GFP_KERNEL OSAL_GFP_KERNEL
#define MCP_OSAL_GFP_ATOMIC OSAL_GFP_ATOMIC

#define MCP_OSAL_VERIFY_READ OSAL_VERIFY_READ
#define MCP_OSAL_VERIFY_WRITE OSAL_VERIFY_WRITE

#define MCP_OSAL_LIST_POISON1 OSAL_LIST_POISON1
#define MCP_OSAL_LIST_POISON2 OSAL_LIST_POISON2

#define MCP_OSAL_IRQ_NONE OSAL_IRQ_NONE
#define MCP_OSAL_IRQ_HANDLED OSAL_IRQ_HANDLED
#define MCP_OSAL_IRQ_WAKE_THREAD OSAL_IRQ_WAKE_THREAD

// need del--------
#define MCP_MOD_ID_VGS SOC_ID_VGS
#define MCP_MOD_ID_VB SOC_ID_VB
#define MCP_MOD_ID_PPU SOC_ID_PPU
#define MCP_MOD_ID_ISP SOC_ID_ISP
#define MCP_MOD_ID_GDC SOC_ID_GDC
#define MCP_MOD_ID_VIU SOC_ID_VIU
#define MCP_MOD_ID_DIS SOC_ID_DIS
#define MCP_MOD_ID_MIPIRX SOC_ID_MIPI_RX
#define MCP_MOD_ID_PIPELINE SOC_ID_PIPELINE
#define MCP_MOD_ID_USR SOC_ID_MAX
//-----------------

#define OSAL_BASE_DEC 10
#define OSAL_BASE_HEX 16

#define TD_UNUSED(x) ((x) = (x))

typedef osal_mutex mcp_osal_mutex;
typedef osal_atomic mcp_osal_atomic;
typedef osal_timeval mcp_osal_timeval;
typedef osal_ioctl_cmd mcp_osal_ioctl_cmd;
typedef osal_fileops mcp_osal_fileops;
typedef osal_pmops mcp_osal_pmops;
typedef osal_spinlock mcp_osal_spinlock;
typedef osal_semaphore mcp_osal_semaphore;
typedef osal_workqueue mcp_osal_workqueue;
typedef osal_wait mcp_osal_wait;
typedef osal_poll mcp_osal_poll;
typedef struct osal_list_head mcp_osal_list_head;
//typedef S_CRG_PERIAPBIF_PRST_REGS_TYPE mcp_reg_crg;

// need del--------
typedef soc_mod_id mcp_soc_mod_id;
//-----------------

typedef osal_timer mcp_osal_timer;
typedef osal_rtc_time mcp_osal_rtc_time;

#define mcp_osal_poll_wait osal_poll_wait
#define mcp_osal_rtc_time_to_tm  // osal_rtc_time_to_tm
#define mcp_osal_timer_set osal_timer_mod
#define mcp_osal_timer_del(a)
#define mcp_osal_timer_destroy osal_timer_destroy
#define mcp_osal_msleep osal_msleep
#define mcp_osal_memncmp osal_memcmp
#define mcp_osal_ioremap osal_ioremap
#define mcp_osal_msecs_to_jiffies osal_msecs_to_jiffies
#define mcp_osal_gettimeofday osal_gettimeofday
#define mcp_osal_copy_from_user osal_copy_from_user
#define mcp_osal_copy_to_user osal_copy_to_user
#define mcp_osal_dcache_region_wb osal_dcache_region_wb
#define mcp_osal_get_current_tgid() 0
#define mcp_osal_get_current_pid osal_get_current_pid

#define mcp_osal_workqueue_init osal_workqueue_init
#define mcp_osal_workqueue_destroy osal_workqueue_destroy
#define mcp_osal_workqueue_schedule osal_workqueue_schedule

#define mcp_osal_abs osal_abs
#define mcp_osal_div64_u64 osal_div64_u64
#define mcp_osal_div64_s64 osal_div64_s64
#define mcp_osal_div_s64 osal_div_s64
#define mcp_osal_div_u64 osal_div_u64
#define mcp_time_get_us() (uapi_tcxo_get_us() * 1000)
#define mcp_time_get_cur_us() uapi_tcxo_get_us()
#define socalign_up_64(x, a) (mcp_osal_div_s64(((x) + (a) - 1), (a)) * (a))

#define mcp_osal_proc_print(seqfile, fmt, ...) printf(fmt,##__VA_ARGS__)
#define mcp_osal_proc_echo_helper osal_printk

#define mcp_osal_kmalloc osal_kmalloc
#define mcp_osal_kfree osal_kfree
#define mcp_osal_vmalloc mcp_osal_mem_vmalloc
#define mcp_osal_vfree mcp_osal_mem_vfree

#define mcp_osal_irq_enable osal_irq_enable
#define mcp_osal_irq_disable osal_irq_disable
#define mcp_osal_irq_request osal_irq_request
#define mcp_osal_irq_free osal_irq_free
#define mcp_osal_irq_set_affinity(irq, name, mask) TD_SUCCESS
#define mcp_osal_isb osal_isb
#define mcp_osal_dsb osal_dsb
#define mcp_osal_dmb osal_dmb

#define MCP_OSAL_INIT_LIST_HEAD OSAL_INIT_LIST_HEAD
#define mcp_osal_list_empty osal_list_empty
#define mcp_osal_list_add_tail osal_list_add_tail
#define mcp_osal_list_del osal_list_del
#define mcp_osal_list_entry osal_list_entry
#define mcp_osal_list_first_entry osal_list_first_entry
#define mcp_osal_list_for_each osal_list_for_each
#define mcp_osal_list_for_each_safe osal_list_for_each_safe
#define mcp_osal_list_for_each_entry osal_list_for_each_entry
#define mcp_osal_list_for_each_entry_safe osal_list_for_each_entry_safe

#define mcp_osal_sem_init osal_sem_init
#define mcp_osal_sem_destroy osal_sem_destroy
#define mcp_osal_sem_up osal_sem_up
#define mcp_osal_sem_down osal_sem_down
#define mcp_osal_sem_down_interruptible osal_sem_down_interruptible

#define mcp_osal_wait_init osal_wait_init
#define mcp_osal_wait_destroy osal_wait_destroy
#define mcp_osal_wait_interruptible osal_wait_interruptible
#define mcp_osal_wait_uninterruptible osal_wait_uninterruptible
#define mcp_osal_wait_timeout_uninterruptible osal_wait_timeout_uninterruptible
#define mcp_osal_wait_timeout_interruptible osal_wait_timeout_interruptible
#define mcp_osal_wait_wakeup osal_wait_wakeup

inline static td_s32 mcp_osal_spin_lock_init(mcp_osal_spinlock *lock_param)
{
    (td_void)memset_s(lock_param, sizeof(mcp_osal_spinlock), 0, sizeof(mcp_osal_spinlock));

    return osal_spin_lock_init(lock_param);
}

inline static td_s32 mcp_osal_timer_init(mcp_osal_timer *ph_timer)
{
    ph_timer->timer = NULL;

    return osal_timer_init(ph_timer);
}

#define mcp_osal_spin_lock_destroy osal_spin_lock_destroy
#define mcp_osal_spin_lock_irqsave osal_spin_lock_irqsave
#define mcp_osal_spin_unlock_irqrestore osal_spin_unlock_irqrestore


#define mcp_osal_spin_lock osal_spin_lock
#define mcp_osal_spin_trylock osal_spin_trylock
#define mcp_osal_spin_unlock  osal_spin_unlock
#define mcp_osal_spin_trylock_irq  osal_spin_trylock_irq
#define mcp_osal_spin_trylock_irqsave  osal_spin_trylock_irqsave

#define mcp_osal_atomic_init(a)     0
#define mcp_osal_atomic_destroy(a)

#define mcp_osal_atomic_set osal_atomic_set
#define mcp_osal_atomic_inc_return osal_atomic_inc_return
#define mcp_osal_atomic_dec_return osal_atomic_dec_return
#define mcp_osal_atomic_inc osal_atomic_inc
#define mcp_osal_atomic_dec osal_atomic_dec
#define mcp_osal_atomic_read osal_atomic_read

#define mcp_osal_dev_register osal_dev_register
#define mcp_osal_dev_unregister osal_dev_unregister

#define mcp_module_init(a)
#define mcp_module_exit(a)
#define mcp_module_author(a)
#define mcp_module_license(a)
#define soc_initcall(a)
#define EXPORT_SYMBOL(a)

#define ext_mpi_mem_unmap

#define mcp_osal_vm osal_vm
#define mcp_osal_mutex_init                 osal_mutex_init
#define mcp_osal_mutex_lock_interruptible   osal_mutex_lock_interruptible
#define mcp_osal_mutex_lock                 osal_mutex_lock
#define mcp_osal_mutex_trylock              osal_mutex_trylock
#define mcp_osal_mutex_unlock               osal_mutex_unlock
#define mcp_osal_mutex_is_locked            osal_mutex_is_locked
#define mcp_osal_mutex_destroy              osal_mutex_destroy

#define mcp_osal_klib_fopen osal_klib_fopen
#define mcp_osal_klib_fclose osal_klib_fclose
#define mcp_osal_klib_fwrite osal_klib_fwrite
#define mcp_osal_klib_fread osal_klib_fread

#define mcp_readl(x) (*((volatile unsigned int *)(x)))
#define mcp_writel(v, x) (*((volatile unsigned int *)(x)) = (v))

#define mcp_assert(expr)                                                                                               \
    do {                                                                                                              \
        if (!(expr)) {                                                                                                \
            osal_printk("\nASSERT failed at:\n  >File name: %s\n  >Function : %s\n  >Line No. : %d\n                  \
				>Condition: %s\n", \
                __FILE__, __FUNCTION__, __LINE__, #expr);                                                             \
        }                                                                                                             \
    } while (0)

#define mcp_mk_str(exp) #exp
#define mcp_mk_marco_to_str(exp) mcp_mk_str(exp)
#define MCP_VERSION_STRING \
    ("SDK_VERSION: [" mcp_mk_marco_to_str(SDK_VERSION) "] Build Time: [" __DATE__ ", " __TIME__ "]")

typedef enum {
    MCP_SYS_CAP_TYPE_NONE = 0,
    MCP_SYS_CAP_TYPE_TEE_SUPPORT = 1,
    MCP_SYS_CAP_TYPE_SMMU_SUPPORT = 2,
    MCP_SYS_CAP_TYPE_MAX
} mcp_sys_capability_type;

typedef enum {
    MCP_OSAL_MMZ_TYPE,     /* non secure cma/mmz */
    MCP_OSAL_NSSMMU_TYPE,  /* non secure smmu  */
    MCP_OSAL_SECSMMU_TYPE, /* secure smmu  */
    MCP_OSAL_SECMMZ_TYPE,  /* secure cma/mmz */
    MCP_OSAL_ERROR_TYPE,   /* no support mem type */
} mcp_osal_mem_type;

typedef struct {
    mcp_osal_mem_type mode; /* 内存分配的模式 */
    td_bool is_create_fd;   // 是否create fd,用户态map cpu虚拟地址
    td_bool is_map_viraddr; /* 是否需要映射CPU地址 */
    td_bool is_cache;       /* 是否需要cache */
    td_u32 size;            /* 内存分配的大小 */
    const td_char *name;            /* ion接口不管理名称，杨伟在考虑如何增加这个需求************************************ */
    td_bool mmz_map_to_smmu;  /* 是否需要将MMZ地址映射为SMMU地址 */
} mcp_osal_mem_attr;

typedef struct {
    td_u32 size;     /* 内存分配的大小 */
    td_u64 phy_addr; /* 物理地址，32/64自适应 */
    td_void *vir_addr; /* CPU地址 */
    td_void *buf_obj; /* buf_obj */
    mcp_osal_mem_type mode;
    td_mem_handle_t handle;
    td_u64 dma_buffer;
    td_u64 secinfo_phy_addr;
} mcp_osal_mem_info;

typedef enum {
    MCP_OSAL_PAGE_SIZE_4K = 0,
    MCP_OSAL_PAGE_SIZE_16K = 1,
    MCP_OSAL_PAGE_SIZE_64K = 2,
    MCP_OSAL_PAGE_SIZE_MAX,
} mcp_osal_mem_page_size;

#define MCP_OSAL_PROC_NAME_LEN 50
#define MCP_PROC_CMD_SINGEL_LENGTH 32

typedef struct {
    td_char name[MCP_OSAL_PROC_NAME_LEN];
    td_s32 (*handler)(td_u32 argc, const td_char *argv[], td_void *priv);
} mcp_osal_proc_cmd;

typedef td_s32 (*mcp_osal_proc_func_open)(struct osal_proc_dir_entry *entry);
typedef td_s32 (*mcp_osal_proc_func_read)(struct osal_proc_dir_entry *entry);
typedef td_s32 (*mcp_osal_proc_func_write)(struct osal_proc_dir_entry *entry,
    const td_char *buf, td_s32 count, td_s64*);

typedef struct {
    td_char name[MCP_OSAL_PROC_NAME_LEN];
    mcp_osal_proc_func_open open;
    mcp_osal_proc_func_read read;
    mcp_osal_proc_func_write write;
    mcp_osal_proc_cmd *cmd_list;
    td_u32 cmd_cnt;
    td_void *priv;
    td_void *proc_dir_entry;
    td_void *seqfile;
    struct osal_list_head node;
} mcp_osal_proc_entry;

td_s32 mcp_osal_mem_alloc(mcp_osal_mem_attr *mem_attr, mcp_osal_mem_info *mem_info);
td_void mcp_osal_mem_free(mcp_osal_mem_info *mem_info);
td_s32 mcp_get_mem_by_mem_handle(const mcp_osal_mem_attr *mem_attr, mcp_osal_mem_info *mem_info);
td_void mcp_put_mem_by_mem_handle(mcp_osal_mem_info *mem_info);
td_s64 mcp_osal_mem_create_fd(td_void *dma_buf);
td_s32 mcp_osal_mem_close_fd(td_void *handle);
td_void *mcp_osal_mem_vmalloc(td_s32 size);
td_void mcp_osal_mem_vfree(td_void *addr);
td_void mcp_osal_mem_info_debug(td_void);

/* get nssmmu pgt info  */
td_void mcp_osal_mem_get_nssmmu_pgtinfo(unsigned long *pt_addr, unsigned long *err_rd_addr,
    unsigned long *err_wr_addr);

mcp_osal_mem_page_size mcp_osal_mem_get_page_size(td_void);

// export function
td_s32 mcp_osal_exportfunc_register(td_u32 module_id, const td_char *name, td_void *func);
td_s32 mcp_osal_exportfunc_unregister(td_u32 module_id);
td_s32 mcp_osal_exportfunc_get(td_u32 module_id, td_void **func);

td_s32 mcp_osal_dts_get_u32_byname(const td_char *compatible, const td_char *name, td_u32 *out_value);

// common
td_bool mcp_sys_get_capability(mcp_sys_capability_type cap_type);

td_s32 mcp_osal_proc_add(mcp_osal_proc_entry *entry);
void mcp_osal_proc_remove(const char *name);
td_void mcp_osal_proc_parse_para(const td_char *buf, td_char (*argv)[MCP_PROC_CMD_SINGEL_LENGTH_MAX],
    td_u32 count, td_u32 *argc);

#define MCP_OSAL_PROC_READ_FUNC(module) \
td_s32 drv_##module##_proc_show(struct osal_proc_dir_entry *s)

#define MCP_OSAL_PROC_READ_FUNC_NAME(module) \
drv_##module##_proc_show

#define MCP_OSAL_PROC_WRITE_FUNC(module, cmd_list, cmd_cnt) \
static int drv_##module##_cmd_call(osal_proc_entry *entry, unsigned int argc, \
    char (*argv)[MCP_PROC_CMD_SINGEL_LENGTH_MAX]) \
{ \
    td_u32 i; \
\
    for (i = 0; i < cmd_cnt; i++) { \
        if (osal_strncmp(cmd_list[i].name, argv[0], strlen(argv[0])) == 0) { \
            if (cmd_list[i].handler != NULL) { \
                return cmd_list[i].handler(argc, argv, entry->private); \
            } \
        } \
    } \
\
    return -1; \
} \
static td_s32 mcp_osal_proc_write(struct osal_proc_dir_entry *s, const char *buf, int count, long long *ppos) \
{ \
    td_s32 ret; \
    td_u32 argc = 0; \
    td_char proc_buf[MCP_PROC_CMD_ALL_LENGTH_MAX] = {0}; \
    td_char argv[MCP_PROC_CMD_NUM_MAX][MCP_PROC_CMD_SINGEL_LENGTH_MAX]; \
\
    if ((buf == NULL) || (s == NULL)) { \
        return TD_FAILURE; \
    } \
\
    if ((count >= MCP_PROC_CMD_ALL_LENGTH_MAX) || (count == 0)) { \
        osal_printk("ERROR:count (%lu) is to large or Zero!\n", (unsigned long)count); \
        return TD_FAILURE; \
    } \
\
    if (osal_copy_from_user(proc_buf, buf, count)) { \
        return TD_FAILURE; \
    } \
\
    proc_buf[MCP_PROC_CMD_ALL_LENGTH_MAX - 1] = '\0'; \
\
    (void)memset_s(&argv[0][0], MCP_PROC_CMD_NUM_MAX*MCP_PROC_CMD_SINGEL_LENGTH_MAX, 0, \
        PROC_CMD_NUM_MAX*MCP_PROC_CMD_SINGEL_LENGTH_MAX); \
    mcp_osal_proc_parse_para(proc_buf, &argv[0], count, &argc); \
    if (argc == 0) { \
        mcp_log_err("ERROR:not vaild arg!\n"); \
        return TD_FAILURE; \
    } \
\
    ret = osal_proc_cmd_call(entry, argc, argv); \
    if (ret != 0) { \
        mcp_log_err("Invalid command or Invalid parameter!\n"); \
        argc = 1; \
        (void)strncpy_s(argv[0], MCP_PROC_CMD_SINGEL_LENGTH_MAX, "help", strlen("help")); \
        (void)osal_proc_cmd_call(entry, argc, argv); \
    } \
\
    return count; \
}

#define MCP_OSAL_PMOP_SUSPEND_FUNC(module) \
MODULE_STATIC td_s32 drv_##module##_dev_suspend(td_void *private_data)

#define MCP_OSAL_PMOP_SUSPEND_FUNC_NAME(module) \
drv_##module##_dev_suspend

#define MCP_OSAL_PMOP_RESUME_FUNC(module) \
MODULE_STATIC td_s32 drv_##module##_dev_resume(td_void *private_data)

#define MCP_OSAL_PMOP_RESUME_FUNC_NAME(module) \
drv_##module##_dev_resume

#define MCP_OSAL_DEV_DEFINE(module, p_minor, p_fops, p_pmops) \
static osal_dev g_##module##_dev = {0}; \
\
td_s32 drv_##module##_reg_dev(td_void) \
{ \
    td_s32 ret; \
\
    ret = strcpy_s(g_##module##_dev.name, OSAL_DEV_NAME_LEN, #module); \
    if (ret != EOK) { \
        mcp_log_err("strcpy_s fail, ret:%x, module:%s\n", ret, #module); \
        return ret; \
    } \
    g_##module##_dev.fops = p_fops; \
    g_##module##_dev.minor = p_minor; \
    g_##module##_dev.pmops = p_pmops; \
    ret = osal_dev_register(&g_##module##_dev); \
    if (ret != TD_SUCCESS) { \
        mcp_log_err("osal_registerdevice fail, ret:%x, module:%s\n", ret, #module); \
        return ret; \
    } \
\
    return TD_SUCCESS; \
} \
td_void drv_##module##_unreg_dev(td_void) \
{ \
    osal_dev_unregister(&g_##module##_dev); \
}

#define MCP_OSAL_REG_DEV(module)  drv_##module##_reg_dev()

#define MCP_OSAL_UNREG_DEV(module) drv_##module##_unreg_dev()

typedef struct {
    td_char mod_name[32];
    soc_mod_id mod_id;
    td_void *export_funcs;
} umap_module;

#define MCP_OSAL_MODULE_DEFINE(module, p_mod_id, p_export_funcs) \
static umap_module g_##module##_module = { \
    .mod_id = p_mod_id, \
    .mod_name = #module, \
    .export_funcs = p_export_funcs, \
}

#define MCP_OSAL_REG_MODULE(module) \
    mcp_osal_exportfunc_register(g_##module##_module.mod_id, g_##module##_module.mod_name, \
        g_##module##_module.export_funcs)

#define MCP_OSAL_UNREG_MODULE(mod_id) \
mcp_osal_exportfunc_unregister(mod_id)

td_s32 mcp_osal_access_ok(td_s32 type, const td_void *addr, td_ulong size);
td_void mcp_osal_iounmap(td_void *addr, td_ulong size);

td_u32 mcp_osal_get_pid(td_u32 pid);

#ifndef ISP_WRITE_I2C_THROUGH_MUL_REG
typedef td_s32 (*isp_write_i2c_data)(td_u8, td_u8, td_u32, td_u32, td_u32, td_u32);
/* td_s32 (*pfn_isp_write_i2c_data)(td_u8 i2c_dev, td_u8 dev_addr, td_u32 reg_addr,
    td_u32 reg_addr_byte_num, td_u32 data, td_u32 data_byte_num); */
#else
typedef td_s32 (*isp_write_i2c_data)(td_u8, td_u8, const td_s8 *, td_u8, td_u8, td_u32, td_u32);
/* td_s32 (*pfn_isp_write_i2c_data)(td_u8 i2c_dev, td_u8 dev_addr,
        const td_s8 *data_buf, td_u8 data_len, td_u8 max_len, td_u32 reg_addr_byte_num, td_u32 data_byte_num); */
#endif

td_void mcp_drv_crg_reg_set_bit(td_ulong value, td_ulong offset, td_ulong addr);
td_u32 mcp_drv_crg_reg_read_bit(td_ulong offset, td_ulong addr);

td_s32 drv_mcp_depend_mod_init(td_void);
td_void drv_mcp_depend_mod_exit(td_void);

#endif

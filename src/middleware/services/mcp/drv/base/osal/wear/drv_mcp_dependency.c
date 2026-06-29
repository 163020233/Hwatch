/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vgs intf source define
 * Author: AuthorNameMagicTag
 * Create: 2021-8-28
 */

#include "drv_mcp_dependency.h"
#include "pm_veto.h"

td_void *g_mcp_export_func[SOC_ID_MAX] = {0};
//volatile mcp_reg_crg *g_mcp_crg_reg = TD_NULL;

#define MCP_MEM_NAME_MAX 20
#define MCP_MEM_ALIGN 256
#define MCP_MEM_CHECK_LEN 4
#define MCP_MEM_CHECK_DATA 0x1234abcd

#ifdef CONFIG_CHIP_1A22
#define MCP_MCPU_MEM_ADDR 0xC9800000  // 0xC5A00000
#define MCP_MCPU_MEM_LEN    0x500000  // 0x3500000

#define MCP_MEM_ADDR 0xC9D00000       // 0xC5A00000
#define MCP_MEM_LEN  0x6200000        // 0x3500000

#define MCP_SELF_MANAGE_MEM_SUPPORT
#endif


#define mcp_check_ret(cond, ret) \
    do { \
        if (!(cond)) { \
            return ret; \
        } \
    } while (0)

#define mcp_check_void(cond) \
    do { \
        if (!(cond)) { \
            return; \
        } \
    } while (0)

#define mcp_check_goto(cond, tag) \
    do { \
        if (!(cond)) { \
            goto tag; \
        } \
    } while (0)

#define mcp_align_up(x, a) (((x) + ((a) - 1)) & ~((a) - 1))
#define mcp_align_down(x, a) (((x) / (a)) * (a))
#define mcp_unused(x)               ((td_void)(x))

typedef struct {
    td_u32 psram_addr;
    td_u32 psram_len;
    td_u32 offset;
    td_u32 free_size;
    osal_spinlock lock;
    td_bool is_self_manage;
    td_bool is_psram_cache;
} liteos_mem_ctx;

typedef struct {
    td_char name[MCP_MEM_NAME_MAX];
    td_u32 phy_addr;
    td_u8 *vir_addr;
    td_u32 len;
    td_u32 align;
    td_u32 offset;
    struct osal_list_head node;
} liteos_mem_node;

typedef struct {
    const td_char *name;
    td_u32 length;
    td_u8 *vir_addr;
    td_phys_addr_t phy_addr;
    td_bool is_map;
    td_bool is_cached;
    td_bool kernel_only;
} osal_mem;

static liteos_mem_ctx g_liteos_mem_ctx;
static struct osal_list_head g_list_mem;

static liteos_mem_ctx *liteos_mem_entry(td_void)
{
    return &g_liteos_mem_ctx;
}

static td_void liteos_mem_lock(td_ulong *lock_flag)
{
    osal_spin_lock_irqsave(&(liteos_mem_entry()->lock), lock_flag);
}

static td_void liteos_mem_unlock(td_ulong *lock_flag)
{
    osal_spin_unlock_irqrestore(&(liteos_mem_entry()->lock), lock_flag);
}

static td_void liteos_mem_add_check(td_u8 *vir_addr, td_u32 len)
{
    td_u32 *data = TD_NULL;

    data = (td_u32 *)vir_addr;
    mcp_check_void(data != TD_NULL);

    data[0] = MCP_MEM_CHECK_DATA;
    data = (td_u32 *)(vir_addr + len - MCP_MEM_CHECK_LEN);
    mcp_check_void(data != TD_NULL);

    data[0] = MCP_MEM_CHECK_DATA;
}

static td_s32 liteos_mem_check_data(td_u8 *vir_addr, td_u32 len)
{
    td_bool is_err;
    td_u32 *data = TD_NULL;

    data = (td_u32 *)vir_addr;
    mcp_check_ret(data != TD_NULL, EXT_FAILURE);

    is_err = (data[0] == MCP_MEM_CHECK_DATA);
    mcp_check_ret(is_err == TD_TRUE, EXT_FAILURE);

    data = (td_u32 *)(vir_addr + len - MCP_MEM_CHECK_LEN);
    mcp_check_ret(data != TD_NULL, EXT_FAILURE);

    is_err = (data[0] == MCP_MEM_CHECK_DATA);
    mcp_check_ret(is_err == TD_TRUE, EXT_FAILURE);

    return TD_SUCCESS;
}

#ifdef MCP_SELF_MANAGE_MEM_SUPPORT
static td_s32 liteos_mem_pool_init(td_void)
{
    td_s32 ret;

    ret = osal_pool_mem_init((td_void *)MCP_MEM_ADDR, MCP_MEM_LEN);
    if (ret != OSAL_SUCCESS) {
        mcp_log_err("mcp pool init err\n");
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_s32 liteos_mem_pool_deinit(td_void)
{
    td_s32 ret;

    ret = osal_pool_mem_deinit((td_void *)MCP_MEM_ADDR);
    if (ret != OSAL_SUCCESS) {
        mcp_log_err("mcp pool deinit err\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}
#endif

static td_void liteos_mem_get_real_info(td_u32 size, td_u32 *real_size, td_u32 *offset)
{
    /* The alignment parameter value must be a power of 2 with the minimum value being 4 */
    /* Non-aligned The size of the input parameter size must be four byte-aligned */
    *real_size = (mcp_align_up(MCP_MEM_CHECK_LEN, MCP_MEM_ALIGN) + size + MCP_MEM_CHECK_LEN);
    *offset = mcp_align_up(MCP_MEM_CHECK_LEN, MCP_MEM_ALIGN);
}

static td_void *liteos_mem_pool_alloc(td_u32 size)
{
    liteos_mem_ctx *ctx = liteos_mem_entry();
    return osal_pool_mem_alloc((td_void *)(td_uintptr_t)(ctx->psram_addr), size);
}

static td_void *liteos_mem_pool_alloc_align(td_u32 size, td_u32 align)
{
    liteos_mem_ctx *ctx = liteos_mem_entry();
    return osal_pool_mem_alloc_align((td_void *)(td_uintptr_t)(ctx->psram_addr), size, align);
}

static td_void liteos_mem_pool_free(const td_void *vir_addr)
{
    liteos_mem_ctx *ctx = liteos_mem_entry();
    osal_pool_mem_free((td_void *)(td_uintptr_t)(ctx->psram_addr), vir_addr);
    return;
}

static td_s32 mcp_liteos_init_mem(td_void)
{
#ifndef CONFIG_CHIP_3322
    td_s32 ret;
    liteos_mem_ctx *ctx = liteos_mem_entry();

    ret = memset_s(ctx, sizeof(liteos_mem_ctx), 0, sizeof(liteos_mem_ctx));
    if (ret != EOK) {
        mcp_log_err("%s memset_s err\n", __func__);
        return EXT_FAILURE;
    }

#ifdef MCP_SELF_MANAGE_MEM_SUPPORT
        ctx->psram_addr = MCP_MEM_ADDR;
        ctx->psram_len = MCP_MEM_LEN;
        ctx->offset = 0;
        ctx->free_size = MCP_MEM_LEN;
        ctx->is_self_manage = TD_TRUE;
        ctx->is_psram_cache = TD_FALSE;

        if (ctx->psram_addr == 0 || ctx->psram_len == 0) {
            mcp_log_err("%s get mem err mem_start_addr 0x%x, mem_length 0x%x\n",
                __func__, ctx->psram_addr, ctx->psram_len);
            return EXT_FAILURE;
        }

        liteos_mem_pool_init();
#else
        ctx->psram_addr = uapi_sys_get_memory_pool_addr(MEM_POOL_MCP);
        ctx->psram_len = PSRAM_HEAP_LENGTH;
        ctx->is_self_manage = TD_FALSE;
        ctx->is_psram_cache = uapi_sys_get_memory_pool_cache_attr(MEM_POOL_MCP);
        viu_info_trace("%s, %d ==> psram_addr = 0x%x\n", __func__, __LINE__, ctx->psram_addr);
#endif

    osal_spin_lock_init(&ctx->lock);

    OSAL_INIT_LIST_HEAD(&g_list_mem);

    mcp_log_err("mcp_liteos_init_mem success!");
#endif
    return EXT_SUCCESS;
}

static td_void mcp_liteos_deinit_mem(td_void)
{
#ifndef CONFIG_CHIP_3322
    td_ulong lock_flag = 0;
    liteos_mem_node *node = TD_NULL;
    liteos_mem_node *node_temp = TD_NULL;
    liteos_mem_ctx *ctx = liteos_mem_entry();

    liteos_mem_lock(&lock_flag);
    if (osal_list_empty(&g_list_mem) == 0) {
        osal_list_for_each_entry_safe(node, node_temp, &g_list_mem, node) {
            if (node != TD_NULL) {
                liteos_mem_pool_free(node->vir_addr);
                osal_list_del(&(node->node));
                liteos_mem_pool_free((td_void *)node);
                node = TD_NULL;
            }
        }
    }

#ifdef MCP_SELF_MANAGE_MEM_SUPPORT
    liteos_mem_pool_deinit();
#endif

    liteos_mem_unlock(&lock_flag);
    osal_spin_lock_destroy(&ctx->lock);
#endif
}

td_s32 mcp_liteos_check_mem(td_void)
{
    td_s32 ret = EXT_SUCCESS;
    td_s32 tmp;
    td_ulong lock_flag = 0;
    liteos_mem_node *node = TD_NULL;
    liteos_mem_node *node_temp = TD_NULL;

    liteos_mem_lock(&lock_flag);
    if (osal_list_empty(&g_list_mem) == 0) {
        osal_list_for_each_entry_safe(node, node_temp, &g_list_mem, node) {
            tmp = liteos_mem_check_data(node->vir_addr, node->len);
            if (tmp != EXT_SUCCESS) {
                mcp_log_err("====mem: %s check err==== \r\n", node->name);
                ret = TD_FAILURE;
            }
        }
    }
    liteos_mem_unlock(&lock_flag);

    return ret;
}

static td_s32 mcp_osal_alloc_mem(osal_mem *mem)
{
    td_s32 ret;
    td_ulong lock_flag = 0;
    td_u8 *vir_addr = TD_NULL;
    liteos_mem_node *node = TD_NULL;
    td_u32 real_size;
    td_u32 offset;

    liteos_mem_lock(&lock_flag);

    node = liteos_mem_pool_alloc(sizeof(liteos_mem_node));
    mcp_check_goto(node != TD_NULL, err1);

    liteos_mem_get_real_info(mem->length, &real_size, &offset);
    vir_addr = liteos_mem_pool_alloc_align(real_size, MCP_MEM_ALIGN);
    mcp_check_goto(vir_addr != TD_NULL, err2);

    ret = strcpy_s(node->name, sizeof(node->name), mem->name);
    if (ret != EOK) {
        mcp_log_err("mcp_osal_alloc_mem name %s strcpy failed!\r\n.", mem->name);
    }

    node->vir_addr = vir_addr;
    node->phy_addr = (td_u32)(td_uintptr_t)(vir_addr);

    node->len = real_size;
    node->align = MCP_MEM_ALIGN;
    node->offset = offset;
    liteos_mem_add_check(vir_addr, real_size);
    osal_list_add_tail(&(node->node), &g_list_mem);

    mem->phy_addr = node->phy_addr + offset;
    mem->vir_addr = node->vir_addr + offset;

    liteos_mem_unlock(&lock_flag);

    return EXT_SUCCESS;

err2:
    mcp_log_err("mcp_osal_alloc_mem err2\r\n.");
    liteos_mem_pool_free((td_void *)node);
err1:
    mcp_log_err("mcp_osal_alloc_mem err1\r\n.");
    liteos_mem_unlock(&lock_flag);

    return EXT_FAILURE;
}

static td_void mcp_osal_free_mem(osal_mem *mem)
{
    td_ulong lock_flag = 0;
    td_bool is_found = TD_FALSE;
    liteos_mem_node *node = TD_NULL;
    liteos_mem_node *node_temp = TD_NULL;

    liteos_mem_lock(&lock_flag);

    if (osal_list_empty(&g_list_mem) == 1) {
        mcp_log_err("%s list empty err\r\n", __func__);
        liteos_mem_unlock(&lock_flag);
        return;
    }

    osal_list_for_each_entry_safe(node, node_temp, &g_list_mem, node) {
        if (node->phy_addr + node->offset == mem->phy_addr) {
            osal_list_del(&(node->node));
            is_found = TD_TRUE;
            break;
        }
    }

    if (is_found) {
        liteos_mem_pool_free(node->vir_addr);
        liteos_mem_pool_free((td_void *)node);
        node = TD_NULL;
    } else {
        mcp_log_err("free phy_addr 0x%x err\n", mem->phy_addr);
    }

    liteos_mem_unlock(&lock_flag);
}


static td_void mcp_osal_print_mem_node_info(td_void)
{
    td_ulong lock_flag = 0;
    td_u32 index = 0;
    liteos_mem_node *node = TD_NULL;
    liteos_mem_node *node_temp = TD_NULL;

    liteos_mem_lock(&lock_flag);
    if (osal_list_empty(&g_list_mem) == 1) {
        mcp_log_err("list is empty\n");
        liteos_mem_unlock(&lock_flag);
        return;
    }

    osal_list_for_each_entry_safe(node, node_temp, &g_list_mem, node) {
        mcp_osal_proc_echo_helper("index %-8d, phy_addr 0x%-8x, offset 0x%-8x, len %-8d, name %s\n",
            index, node->phy_addr, node->offset, node->len, node->name);
        index++;
    }

    liteos_mem_unlock(&lock_flag);
}

td_s32 mcp_osal_mem_alloc(mcp_osal_mem_attr *mem_attr, mcp_osal_mem_info *mem_info)
{
    td_s32 ret;
    osal_mem mem = {0};

    if ((mem_attr == TD_NULL) || (mem_info == TD_NULL)) {
        mcp_log_err("mem_attr:%d mem_info:%d\n", (mem_attr != TD_NULL), (mem_info != TD_NULL));
        return TD_FAILURE;
    }

    mem.name = mem_attr->name;
    mem.length = mem_attr->size;
    mem.kernel_only = TD_TRUE;
    mem.is_map = TD_TRUE;
    ret = mcp_osal_alloc_mem(&mem);
    if (ret != TD_SUCCESS) {
        mcp_log_info("mcp_osal_alloc_mem fail, ret:%x.\r\n", ret);
        return ret;
    }

    mem_info->vir_addr = mem.vir_addr;
    mem_info->phy_addr = mem.phy_addr;

    mem_info->dma_buffer = (td_u64)(td_uintptr_t)mem.phy_addr;
    mem_info->handle = (td_mem_handle_t)(td_uintptr_t)mem.phy_addr;

    return (mem_info->phy_addr != 0) ? TD_SUCCESS : TD_FAILURE;
}

td_void mcp_osal_mem_free(mcp_osal_mem_info *mem_info)
{
    osal_mem mem = {0};

    if (mem_info == TD_NULL) {
        mcp_log_err("mem_info is null.\n");
        return;
    }

    mem.phy_addr = mem_info->phy_addr;
    mem.vir_addr = mem_info->vir_addr;
    mcp_osal_free_mem(&mem);
}

td_void mcp_osal_mem_info_debug(td_void)
{
    mcp_osal_print_mem_node_info();
}

td_void *mcp_osal_mem_vmalloc(td_s32 size)
{
#ifdef CONFIG_CHIP_3322
    return osal_vmalloc(size);
#else
    td_s32 ret;
    osal_mem mem = {0};

    mem.length = size;
    mem.kernel_only = TD_TRUE;
    mem.is_map = TD_TRUE;
    mem.name = "vmalloc";

    ret = mcp_osal_alloc_mem(&mem);
    if (ret != TD_SUCCESS) {
        mcp_log_info("mcp_osal_alloc_mem fail, ret:%x.\r\n", ret);
        return TD_NULL;
    }

    return mem.vir_addr;
#endif
}

td_void mcp_osal_mem_vfree(td_void *addr)
{
#ifdef CONFIG_CHIP_3322
    osal_vfree(addr);
#else
    osal_mem mem = {0};

    mem.phy_addr = addr;
    mem.vir_addr = addr;
    mcp_osal_free_mem(&mem);
#endif
}

td_s32 mcp_get_mem_by_mem_handle(const mcp_osal_mem_attr *mem_attr, mcp_osal_mem_info *mem_info)
{
    if ((mem_attr == TD_NULL) || (mem_info == TD_NULL)) {
        mcp_log_err("mem_attr:%d mem_info:%d\n", (mem_attr != TD_NULL), (mem_info != TD_NULL));
        return TD_FAILURE;
    }

    mem_info->phy_addr = mem_info->dma_buffer;
    mem_info->vir_addr = (td_void *)(uintptr_t)(mem_info->dma_buffer);
    return TD_SUCCESS;
}

td_void mcp_put_mem_by_mem_handle(mcp_osal_mem_info *mem_info)
{
    if (mem_info == TD_NULL) {
        mcp_log_err("mem_info is null.\n");
        return;
    }

    return;
}

void mcp_osal_mem_get_nssmmu_pgtinfo(td_ulong *pt_addr, td_ulong *err_rd_addr, td_ulong *err_wr_addr)
{
    *pt_addr = 0;
    *err_rd_addr = 0;
    *err_wr_addr = 0;
}

mcp_osal_mem_page_size mcp_osal_mem_get_page_size(td_void)
{
    return MCP_OSAL_PAGE_SIZE_4K;
}

td_s64 mcp_osal_mem_create_fd(td_void *dma_buf)
{
    return (td_s64)(uintptr_t)dma_buf;
}

td_s32 mcp_osal_mem_close_fd(td_void *handle)
{
    mcp_unused(handle);

    return TD_SUCCESS;
}

td_s32 mcp_osal_exportfunc_register(td_u32 module_id, const td_char *name, td_void *func)
{
    if (name == TD_NULL) {
        mcp_log_err("mcp_osal_exportfunc_register name is null, module_id:%d.\n", module_id);
        return TD_FAILURE;
    } else if (func == TD_NULL) {
        mcp_log_err("mcp_osal_exportfunc_register func is null, module_id:%d.\n", module_id);
        return TD_FAILURE;
    } else if (module_id >= SOC_ID_MAX) {
        mcp_log_err("mcp_osal_exportfunc_get module_id is out of range, module_id:%d.\n", module_id);
    }

    g_mcp_export_func[module_id] = func;
    return TD_SUCCESS;
}

td_s32 mcp_osal_exportfunc_unregister(td_u32 module_id)
{
    g_mcp_export_func[module_id] = NULL;
    return TD_SUCCESS;
}

td_s32 mcp_osal_exportfunc_get(td_u32 module_id, td_void **func)
{
    if (func == TD_NULL) {
        mcp_log_err("mcp_osal_exportfunc_get func is null, module_id:%d.\n", module_id);
        return TD_FAILURE;
    }

    *func = g_mcp_export_func[module_id];
    if (*func == TD_NULL) {
        mcp_log_err("cmpi_get_module_func_by_id fail, module_id:%d.\n", module_id);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

#define CV610_CKSEL_DEF                 0

#define VICAP_TABLE  "huanglong,vicap"
#define VIPROC_TABLE "huanglong,viproc"
#define VGS_TABLE    "huanglong,vgs"
#define PPU_TABLE    "huanglong,ppu"

#define VICAP_IRQ_TAB "vicap_irq_num"
#define VIPROC_IRQ_TAB "viproc_irq_num"
#define VGS_IRQ_TAB    "vgs_irq_num"
#define PPU_IRQ_TAB    "ppu_irq_num"

#define VICAP_CKSEL_TAB "vicap_clk_sel"
#define VIPROC_CKSEL_TAB "viproc_clk_sel"
#define VGS_CKSEL_TAB    "isp_vgs_cksel"
#define PPU_CKSEL_TAB    "ppu_cksel"


typedef struct {
    td_char compatible[32];
    td_char tab_name[32];
    td_u32 value;
} mcp_osal_dts_rec;

#ifdef CONFIG_CHIP_3322
#define CV610_VI_CAP0_IRQ_NR            VICAP_INTR_IRQN
static mcp_osal_dts_rec g_mcp_dts_rec[] = {
    // irq num
    {VICAP_TABLE,  VICAP_IRQ_TAB, CV610_VI_CAP0_IRQ_NR},

    // clk sel
    {VICAP_TABLE,  VICAP_CKSEL_TAB, CV610_CKSEL_DEF},
    // irq affinity
};
#elif defined(CONFIG_CHIP_1A22)
#define CV610_MIPI_IRQ_NR               MIPI_RX_IRQN
#define CV610_VI_CAP0_IRQ_NR            VICAP_0_IRQN
#define CV610_VI_PROC0_IRQ_NR           VIPROC_IRQN
#define CV610_VI_PROC1_IRQ_NR           VIPROC_IRQN
#define CV610_VPSS_IRQ_NR               VPSS_IRQN
#define CV610_VGS0_IRQ_NR               VGS_IRQN

static mcp_osal_dts_rec g_mcp_dts_rec[] = {
    // irq num
    {VICAP_TABLE,  VICAP_IRQ_TAB, CV610_VI_CAP0_IRQ_NR},
    {VIPROC_TABLE, VIPROC_IRQ_TAB, CV610_VI_PROC0_IRQ_NR},
    {VGS_TABLE,    VGS_IRQ_TAB, CV610_VGS0_IRQ_NR},
    {PPU_TABLE,    PPU_IRQ_TAB, CV610_VPSS_IRQ_NR},

    // clk sel
    {VICAP_TABLE,  VICAP_CKSEL_TAB, CV610_CKSEL_DEF},
    {VIPROC_TABLE, VIPROC_CKSEL_TAB, CV610_CKSEL_DEF},
    {VGS_TABLE,    VGS_CKSEL_TAB, CV610_CKSEL_DEF},
    {PPU_TABLE,    PPU_CKSEL_TAB, CV610_CKSEL_DEF},

    // irq affinity
};
#endif

td_s32 mcp_osal_dts_get_u32_byname(const char *compatible, const char *name, td_u32 *out_value)
{
    td_u32 size = sizeof(g_mcp_dts_rec) / sizeof(mcp_osal_dts_rec);
    td_u32 i;

    for (i = 0; i < size; i++) {
        if ((strcmp(compatible, g_mcp_dts_rec[i].compatible) == 0) &&
            (strcmp(name, g_mcp_dts_rec[i].tab_name) == 0)) {
            *out_value = g_mcp_dts_rec[i].value;
            return TD_SUCCESS;
        }
    }

    return TD_FAILURE;
}

td_bool mcp_sys_get_capability(mcp_sys_capability_type cap_type)
{
    mcp_unused(cap_type);

    return TD_FALSE;
}

td_void mcp_osal_proc_parse_para(const td_char *buf, td_char (*argv)[MCP_PROC_CMD_SINGEL_LENGTH_MAX],
    td_u32 count, td_u32 *argc)
{
    td_u32 i;
    td_s32 j = 0;
    td_s32 k = 0;

    if (count >= MCP_PROC_CMD_ALL_LENGTH_MAX) {
        mcp_log_err("ERROR:count (%d) is to large!\n", count);
        return;
    }

    for (i = 0; i < count; i++) {
        /* ignore SPACE */
        if (buf[i] == 0x20) {
            continue;
        }

        if (buf[i] == 0xa) {
            break;
        }

        if ((k >= (MCP_PROC_CMD_SINGEL_LENGTH_MAX - 1)) || (j >= (MCP_PROC_CMD_NUM_MAX - 1))) {
            return;
        }

        argv[j][k] = buf[i];
        k++;

        if ((buf[i + 1] == 0x20) || (buf[i + 1] == 0xa)) {
            argv[j][k] = '\0';
            k = 0;
            j++;
        }
    }

    *argc = j;
}

td_s32 mcp_osal_proc_add(mcp_osal_proc_entry *entry)
{
    mcp_unused(entry);

    return TD_SUCCESS;
}

void mcp_osal_proc_remove(const char *name)
{
    mcp_unused(name);

    return;
}

td_s32 mcp_osal_access_ok(td_s32 type, const td_void *addr, td_ulong size)
{
    return osal_access_ok(type, addr, size);
}

td_void mcp_osal_iounmap(td_void *addr, td_ulong size)
{
    osal_iounmap(addr, size);
}

#define CRG_REGS_ADDR             0x55000000
#define CRG_REGS_ADDR_OFFSET      0x4500
#define CRG_REGS_SIZE             (0x10000 - CRG_REGS_ADDR_OFFSET)

#define CRG_COM_CLKEN_RB_3        0x55003030    // COM_SUB时钟门控使能寄存器3 VI_SUB
#define SOFT_RST_4                0x570000c0    // NAE & VI软复位
#define SOFT_RST_7                0x570000cc    // MEM软复位    not config
#define VI_SUB0_AXI               0x53490030    // VIU SUB priority
#define CRG_VI_IPLL_AXI           0x53490300    // CRG_VI_IPLL_AXI

#define VI_AXI_CLKEN_BIT          14
#define VI_APB_CLKEN_BIT          13
#define NAE_CLKEN_BIT             9

#define SOFT_RST_GLB_NAE_LGC_BIT  8
#define SOFT_RST_GLB_VI_LGC_BIT   5
#define SOFT_RST_GLB_VI_CRG_BIT   4

#define SOFT_RST_GLB_NAE_LGC_BIT  8
#define SOFT_RST_GLB_VI_LGC_BIT   5
#define SOFT_RST_GLB_VI_CRG_BIT   4

#define VI_SUB0_AXI_PRIORITY_M1_BIT  7
#define VI_SUB0_AXI_PRIORITY_M2_BIT  6

td_void mcp_drv_crg_reg_set_bit(td_ulong value, td_ulong offset, td_ulong addr)
{
    td_ulong t, mask;

    mask = 1 << offset;
    t = mcp_readl((const volatile td_void *)addr);
    t &= ~mask;
    t |= (value << offset) & mask;
    mcp_writel(t, (volatile td_void *)addr);
}

td_u32 mcp_drv_crg_reg_read_bit(td_ulong offset, td_ulong addr)
{
    td_u32 t;

    t = mcp_readl((const volatile td_void *)addr);
    t >>= offset;
    t &= 1;

    return t;
}
#ifndef CONFIG_CHIP_3322
static td_s32 drv_mcp_crg_clk_rst_init(td_void)
{
    td_ulong crg_reg;
    const td_u32 clk_en = 1;
    const td_u32 rst_cancle = 1;

    /* sys system reset cancle */
    mcp_log_alert("=====> prepare config 0x%x\n", SOFT_RST_4);
    crg_reg = (td_ulong)mcp_osal_ioremap(SOFT_RST_4, (td_ulong)0x4);
    if (crg_reg == 0) {
        mcp_log_err("reg:SOFT_RST_4(0x%x) ioremap failed!\n", SOFT_RST_4);
        return TD_FAILURE;
    }
    mcp_drv_crg_reg_set_bit(rst_cancle, SOFT_RST_GLB_NAE_LGC_BIT, crg_reg);
    mcp_drv_crg_reg_set_bit(rst_cancle, SOFT_RST_GLB_VI_LGC_BIT, crg_reg);
    mcp_drv_crg_reg_set_bit(rst_cancle, SOFT_RST_GLB_VI_CRG_BIT, crg_reg);
    mcp_osal_iounmap((td_void *)crg_reg, (td_ulong)0x4);
    mcp_log_alert("=====> config 0x%x success!\n", SOFT_RST_4);

    /* sub system clk enable */
    mcp_log_alert("=====> prepare config 0x%x\n", CRG_COM_CLKEN_RB_3);
    crg_reg = (td_ulong)mcp_osal_ioremap(CRG_COM_CLKEN_RB_3, (td_ulong)0x4);
    if (crg_reg == 0) {
        mcp_log_err("reg:CRG_COM_CLKEN_RB_3(0x%x) ioremap failed!\n", CRG_COM_CLKEN_RB_3);
        return TD_FAILURE;
    }
    mcp_drv_crg_reg_set_bit(clk_en, VI_AXI_CLKEN_BIT, crg_reg);
    mcp_drv_crg_reg_set_bit(clk_en, VI_APB_CLKEN_BIT, crg_reg);
    mcp_drv_crg_reg_set_bit(clk_en, NAE_CLKEN_BIT, crg_reg);
    mcp_osal_iounmap((td_void *)crg_reg, (td_ulong)0x4);
    mcp_log_alert("=====> config 0x%x success!\n", CRG_COM_CLKEN_RB_3);

    /* config vi sub axi priority */
    mcp_log_alert("=====> prepare config 0x%x\n", VI_SUB0_AXI);
    crg_reg = (td_ulong)mcp_osal_ioremap(VI_SUB0_AXI, (td_ulong)0x4);
    if (crg_reg == 0) {
        mcp_log_err("reg:VI_SUB0_AXI(0x%x) ioremap failed!\n", VI_SUB0_AXI);
        return TD_FAILURE;
    }
    mcp_drv_crg_reg_set_bit(1, VI_SUB0_AXI_PRIORITY_M1_BIT, crg_reg);
    mcp_drv_crg_reg_set_bit(1, VI_SUB0_AXI_PRIORITY_M2_BIT, crg_reg);
    mcp_osal_iounmap((td_void *)crg_reg, (td_ulong)0x4);
    mcp_log_alert("=====> config 0x%x success!\n", VI_SUB0_AXI);

    /* vi sub pll en */
    mcp_log_alert("=====> prepare config 0x%x\n", CRG_VI_IPLL_AXI);

    crg_reg = (td_ulong)mcp_osal_ioremap(CRG_VI_IPLL_AXI, (td_ulong)0x4);
    if (crg_reg == 0) {
        mcp_log_err("reg:CRG_VI_IPLL_AXI(0x%x) ioremap failed!\n", CRG_VI_IPLL_AXI);
        return TD_FAILURE;
    }
    mcp_drv_crg_reg_set_bit(1, 0x1, crg_reg);   // 1: cfg_vi_ipllpost_clken
    mcp_osal_iounmap((td_void *)crg_reg, (td_ulong)0x4);
    mcp_log_alert("=====> config 0x%x success!\n", CRG_VI_IPLL_AXI);

    return TD_SUCCESS;
}

static td_s32 drv_mcp_crg_clk_rst_deinit(td_void)
{
    td_ulong crg_reg;
    const td_u32 clk_en = 0;
    const td_u32 rst_cancle = 0;

    /* sub system clk disable */
    crg_reg = (td_ulong)mcp_osal_ioremap(CRG_COM_CLKEN_RB_3, (td_ulong)0x4);
    if (crg_reg == 0) {
        mcp_log_err("reg:CRG_COM_CLKEN_RB_3(0x%x) ioremap failed!\n", CRG_COM_CLKEN_RB_3);
        return TD_FAILURE;
    }

    mcp_drv_crg_reg_set_bit(clk_en, VI_AXI_CLKEN_BIT, crg_reg);
    mcp_drv_crg_reg_set_bit(clk_en, VI_APB_CLKEN_BIT, crg_reg);
    mcp_drv_crg_reg_set_bit(clk_en, NAE_CLKEN_BIT, crg_reg);

    mcp_osal_iounmap((td_void *)crg_reg, (td_ulong)0x4);

    /* sys system reset enable */
    crg_reg = (td_ulong)mcp_osal_ioremap(SOFT_RST_4, (td_ulong)0x4);
    if (crg_reg == 0) {
        mcp_log_err("reg:SOFT_RST_4(0x%x) ioremap failed!\n", SOFT_RST_4);
        return TD_FAILURE;
    }

    mcp_drv_crg_reg_set_bit(rst_cancle, SOFT_RST_GLB_NAE_LGC_BIT, crg_reg);
    mcp_drv_crg_reg_set_bit(rst_cancle, SOFT_RST_GLB_VI_LGC_BIT, crg_reg);
    mcp_drv_crg_reg_set_bit(rst_cancle, SOFT_RST_GLB_VI_CRG_BIT, crg_reg);

    mcp_osal_iounmap((td_void *)crg_reg, (td_ulong)0x4);

    /* vi sub pll disable */
    crg_reg = (td_ulong)mcp_osal_ioremap(CRG_VI_IPLL_AXI, (td_ulong)0x4);
    if (crg_reg == 0) {
        mcp_log_err("reg:CRG_VI_IPLL_AXI(0x%x) ioremap failed!\n", CRG_VI_IPLL_AXI);
        return TD_FAILURE;
    }
    mcp_drv_crg_reg_set_bit(0, 0x1, crg_reg);   // 1: cfg_vi_ipllpost_clken
    mcp_osal_iounmap((td_void *)crg_reg, (td_ulong)0x4);

    return TD_SUCCESS;
}
#endif
td_s32 drv_mcp_depend_mod_init(td_void)
{
#ifndef CONFIG_CHIP_3322
    td_s32 ret;

    ret = uapi_pm_add_sleep_veto(PM_ID_MCP);
    if (ret != TD_SUCCESS) {
        mcp_log_err("uapi_pm_add_sleep_veto(ID: %d) failed! ret =  0x%x\r\n", PM_ID_MCP, ret);
        return ret;
    }
#endif

    /* i2c prepare */
    mcp_liteos_init_mem();

    mcp_log_alert("prepare to config crg reg!\n");

#ifndef CONFIG_CHIP_3322
    /* vi sub system clk enable */
    drv_mcp_crg_clk_rst_init();
#endif
    mcp_log_alert("dependency init=====> success!!\n");

    return 0;
}

td_void drv_mcp_depend_mod_exit(td_void)
{
    // exit
    mcp_liteos_deinit_mem();

#ifndef CONFIG_CHIP_3322
    td_s32 ret;

    /* vi sub system clk close */
    drv_mcp_crg_clk_rst_deinit();

    ret = uapi_pm_remove_sleep_veto(PM_ID_MCP);
    if (ret != TD_SUCCESS) {
        mcp_log_err("uapi_pm_remove_sleep_veto(ID: %d) failed! ret =  0x%x\r\n", PM_ID_MCP, ret);
    }
#endif

    mcp_log_alert("dependency deinit=====> success!!\n");
}

td_u32 mcp_osal_get_pid(td_u32 pid)
{
    return pid;
}

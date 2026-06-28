/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2024. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "dbg.h"
#include "soc_osal.h"
#include "vfmw_osal.h"
#include "liteos_proc.h"
#include "vfmw_proc.h"
#include "vfmw_define.h"
#include "vdec_chr.h"
#include "event.h"
#include "liteos_mem.h"

/*-------------------------------- macro define -------------------------------------------*/
#define VFMW_MEM_NAME_MAX_LEN      100
#define VFMW_MEM_ALIGN             16
#define VFMW_MEM_CHECK_DIGIT_LEN   4
#define VFMW_MEM_CHECK_DATA        0x1234abcd

/*-------------------------------- struct define ------------------------------------------*/
typedef struct {
    td_char name[VFMW_MEM_NAME_MAX_LEN];
    td_u32 phy_addr;
    td_u8 *vir_addr;
    td_u32 len;
    td_u32 align;
    td_u32 offset;
    struct osal_list_head node;
} mem_node;

typedef struct {
    td_u32 mem_addr;
    td_u32 mem_len;
    td_u32 mem_used;
    td_u32 mem_free;
    os_lock spin_lock;
    td_bool is_self_manage;
    td_bool is_cache;
} liteos_mem_ctx;

/*-------------------------------- global define ------------------------------------------*/
static liteos_mem_ctx g_liteos_mem_ctx;
static struct osal_list_head g_list_mem;

/*-------------------------------- func declares ------------------------------------------*/
static liteos_mem_ctx *liteos_mem_entry(td_void)
{
    return &g_liteos_mem_ctx;
}

STATIC td_void liteos_mem_lock(td_ulong *lock_flag)
{
    os_spin_lock(liteos_mem_entry()->spin_lock, lock_flag);
}

STATIC td_void liteos_mem_unlock(td_ulong *lock_flag)
{
    os_spin_unlock(liteos_mem_entry()->spin_lock, lock_flag);
}

#ifdef VFMW_CHECK_MEM_SUPPORT
STATIC td_void liteos_mem_add_check_data(td_u8 *vir_addr, td_u32 len)
{
    td_u32 *data = TD_NULL;

    data = (td_u32 *)vir_addr;
    vfmw_check(data != TD_NULL);
    data[0] = VFMW_MEM_CHECK_DATA;
    data = (td_u32 *)(vir_addr + len - VFMW_MEM_CHECK_DIGIT_LEN);
    vfmw_check(data != TD_NULL);
    data[0] = VFMW_MEM_CHECK_DATA;
}

STATIC td_bool liteos_mem_check_mem(td_u8 *vir_addr, td_u32 len)
{
    td_bool is_err;
    td_u32 *data = TD_NULL;

    data = (td_u32 *)vir_addr;
    vfmw_check_ret(data != TD_NULL, TD_TRUE);
    is_err = (data[0] == VFMW_MEM_CHECK_DATA);
    vfmw_check_ret(is_err == TD_TRUE, TD_TRUE);

    data = (td_u32 *)(vir_addr + len - VFMW_MEM_CHECK_DIGIT_LEN);
    vfmw_check_ret(data != TD_NULL, TD_TRUE);
    is_err = (data[0] == VFMW_MEM_CHECK_DATA);
    vfmw_check_ret(is_err == TD_TRUE, TD_TRUE);

    return TD_FALSE;
}
#endif

#ifdef VFMW_MANAGE_MEM_SUPPORT
STATIC td_s32 liteos_mem_pool_init(td_void)
{
    td_s32 ret;

    ret = osal_pool_mem_init((td_void *)VFMW_MEM_ADDR, VFMW_MEM_LEN);
    if (ret != OSAL_SUCCESS) {
        os_print("osal_pool_mem_init failed!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 liteos_mem_pool_deinit(td_void)
{
    td_s32 ret;

    ret = osal_pool_mem_deinit((td_void *)VFMW_MEM_ADDR);
    if (ret != OSAL_SUCCESS) {
        os_print("osal_pool_mem_deinit failed!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}
#endif

STATIC td_void liteos_mem_pool_calc_free(liteos_mem_ctx *ctx)
{
    td_u32 used_len = 0;
    td_u32 node_size = (td_u32)sizeof(mem_node);
    mem_node *p_node = TD_NULL;

    if (osal_list_empty(&g_list_mem) == 0) {
        osal_list_for_each_entry(p_node, &g_list_mem, node) {
            used_len += p_node->len + node_size;
        }
    }

    ctx->mem_used = used_len;
    ctx->mem_free = ctx->mem_len - used_len;
}

STATIC td_void liteos_mem_get_real_info(td_u32 size, td_u32 align, td_u32 *real_size, td_u32 *offset)
{
#ifdef VFMW_CHECK_MEM_SUPPORT
    /* The alignment parameter value must be a power of 2 with the minimum value being 4 */
    /* Non-aligned The size of the input parameter size must be four byte-aligned */
    *real_size = (align == 1) ? vfmw_align_up(2 * VFMW_MEM_CHECK_DIGIT_LEN + size, 4) : /* 2 is mult 4 byte align */
        (vfmw_align_up(VFMW_MEM_CHECK_DIGIT_LEN, align) + size + VFMW_MEM_CHECK_DIGIT_LEN);
    *offset = (align == 1) ? VFMW_MEM_CHECK_DIGIT_LEN : vfmw_align_up(VFMW_MEM_CHECK_DIGIT_LEN, align);
#else
    vfmw_unused(align);
    *real_size = size;
    *offset = 0;
#endif
}

STATIC td_uchar *liteos_mem_pool_alloc(td_u32 size)
{
    liteos_mem_ctx *ctx = liteos_mem_entry();

    return (td_uchar *)osal_pool_mem_alloc(uint64_ptr(ctx->mem_addr), size);
}

STATIC td_uchar *liteos_mem_pool_alloc_align(td_u32 size, td_u32 align)
{
    liteos_mem_ctx *ctx = liteos_mem_entry();

    return (td_uchar *)osal_pool_mem_alloc_align(uint64_ptr(ctx->mem_addr), size, align);
}

STATIC td_void liteos_mem_pool_free(const td_uchar *vir_addr)
{
    liteos_mem_ctx *ctx = liteos_mem_entry();

    osal_pool_mem_free(uint64_ptr(ctx->mem_addr), (td_void *)vir_addr);
    return;
}

/*-------------------------------------- func release -------------------------------------*/

td_s32 liteos_mem_init(td_void)
{
    liteos_mem_ctx *ctx = liteos_mem_entry();

    (td_void)memset_s(ctx, sizeof(liteos_mem_ctx), 0, sizeof(liteos_mem_ctx));
    os_spin_lock_init(&ctx->spin_lock);

#ifdef VFMW_MANAGE_MEM_SUPPORT
    ctx->mem_addr = VFMW_MEM_ADDR;
    ctx->mem_len = VFMW_MEM_LEN;
    ctx->mem_used = 0;
    ctx->is_self_manage = TD_TRUE;
#ifdef VFMW_PSRAM_CACHE_SUPPORT
    ctx->is_cache = TD_TRUE;
#else
    ctx->is_cache = TD_FALSE;
#endif
    if (liteos_mem_pool_init() != EXT_SUCCESS) {
        os_spin_lock_exit(ctx->spin_lock);
        return OSAL_ERR;
    }
#else
    ctx->mem_addr = uapi_sys_get_memory_pool_addr(MEM_POOL_VDEC);
    ctx->mem_len = uapi_sys_get_memory_pool_length(MEM_POOL_VDEC);
    ctx->mem_used = 0;
    ctx->is_self_manage = TD_FALSE;
    ctx->is_cache = uapi_sys_get_memory_pool_cache_attr(MEM_POOL_VDEC);
#endif
    OSAL_INIT_LIST_HEAD(&g_list_mem);

    return OSAL_OK;
}

td_void liteos_mem_deinit(td_void)
{
    td_ulong lock_flag = 0;
    mem_node *p_node = TD_NULL;
    mem_node *p_node_tmp = TD_NULL;
    liteos_mem_ctx *ctx = liteos_mem_entry();

    liteos_mem_lock(&lock_flag);
    if (osal_list_empty(&g_list_mem) == 0) {
        osal_list_for_each_entry_safe(p_node, p_node_tmp, &g_list_mem, node) {
            if (p_node != TD_NULL) {
                liteos_mem_pool_free(p_node->vir_addr);
                osal_list_del(&(p_node->node));
                liteos_mem_pool_free((td_void *)p_node);
                p_node = TD_NULL;
            }
        }
    }

#ifdef VFMW_MANAGE_MEM_SUPPORT
    liteos_mem_pool_deinit();
#endif

    liteos_mem_unlock(&lock_flag);

    os_spin_lock_exit(ctx->spin_lock);
}

td_s32 liteos_mem_proc(td_void *buffer)
{
    td_s32 used = 0;
    const td_s32 len = 0;
    td_ulong lock_flag = 0;
    mem_node *p_node = TD_NULL;
    td_bool mem_err = 0;
    liteos_mem_ctx *ctx = liteos_mem_entry();
#ifdef VFMW_CHECK_MEM_SUPPORT
    const td_bool check_mem_support = 1;
#else
    const td_bool check_mem_support = 0;
#endif

    liteos_mem_lock(&lock_flag);

    os_dump_proc(buffer, len, &used, 0, LOG_MEM_FMT_HEAD);
    os_dump_proc(buffer, len, &used, 0, LOG_FMT_X_X, "mem_phyaddr", ctx->mem_addr,
        "mem_total_size", ctx->mem_len);
    os_dump_proc(buffer, len, &used, 0, LOG_FMT_D_D, "is_cache", ctx->is_cache, "is_self_manage",
        ctx->is_self_manage);

    liteos_mem_pool_calc_free(ctx);
    os_dump_proc(buffer, len, &used, 0, LOG_FMT_X_X, "mem_used", ctx->mem_used, "mem_free", ctx->mem_free);

    os_dump_proc(buffer, len, &used, 0, LOG_FMT_D_N, "check_mem_support", check_mem_support, " ");

    if (osal_list_empty(&g_list_mem) == 0) {
        os_dump_proc(buffer, len, &used, 0, LOG_MEM_FMT_TAIL);
        os_dump_proc(buffer, len, &used, 0, "%-19s|%-19s|%-19s|%-10s|%-8s|\r\n",
            "buf_name", "phy_addr", "buf_len", "align", "mem_err");
        osal_list_for_each_entry(p_node, &g_list_mem, node) {
            mem_err = liteos_mem_check_mem(p_node->vir_addr, p_node->len);
            os_dump_proc(buffer, len, &used, 0, "%-19s|0x%-17x|0x%-17x|%-10u|%-8u|\r\n", p_node->name, p_node->phy_addr,
                p_node->len, p_node->align, mem_err);
        }
    }
    os_dump_proc(buffer, len, &used, 0, LOG_MEM_FMT_TAIL);
    liteos_mem_unlock(&lock_flag);

    return OSAL_OK;
}

td_s32 liteos_mem_alloc(const td_char *name, td_u32 size, td_u32 align, mem_record *mem_rec)
{
    td_s32 ret;
    td_ulong lock_flag = 0;
    td_u8 *vir_addr = TD_NULL;
    mem_node *p_node = TD_NULL;
    td_u32 real_size;
    td_u32 offset;

    liteos_mem_lock(&lock_flag);

    p_node = (mem_node *)liteos_mem_pool_alloc(sizeof(mem_node));
    vfmw_check_goto(p_node != TD_NULL, err1);

    liteos_mem_get_real_info(size, align, &real_size, &offset);

    if (align == 1) {
        vir_addr = liteos_mem_pool_alloc(real_size);
    } else {
        vir_addr = liteos_mem_pool_alloc_align(real_size, align);
    }
    if (vir_addr == TD_NULL) {
        dprint(PRN_ERROR, "name %s, size %d alloc failed\r\n", name, size);
        report_out_of_memory_err((td_s32)mem_rec->unid);
        goto err2;
    }
    ret = strcpy_s(p_node->name, sizeof(p_node->name), name);
    vfmw_check_goto(ret == EOK, err3);
    p_node->vir_addr = vir_addr;
    p_node->phy_addr = (td_u32)ptr_uint64(vir_addr);
    p_node->len = real_size;
    p_node->align = align;
    p_node->offset = offset;
#ifdef VFMW_CHECK_MEM_SUPPORT
    liteos_mem_add_check_data(vir_addr, real_size);
#endif
    osal_list_add_tail(&(p_node->node), &g_list_mem);

    mem_rec->length = size;
    mem_rec->phy_addr = p_node->phy_addr + offset;
    mem_rec->vir_addr = p_node->vir_addr + offset;

#ifdef MEM_DEBUG_PRINT_EN
    os_print("alloc name %s alloc phy_addr (0x%x) p_node %p\r\n", name, mem_rec->phy_addr, p_node);
#endif
    liteos_mem_unlock(&lock_flag);

    return OSAL_OK;

err3:
    liteos_mem_pool_free(vir_addr);
err2:
    liteos_mem_pool_free((td_void *)p_node);
err1:
    liteos_mem_unlock(&lock_flag);
    return OSAL_ERR;
}

td_void liteos_mem_free(mem_record *mem)
{
    td_ulong lock_flag = 0;
    td_u32 is_found = 0;
    mem_node *p_node = TD_NULL;
    mem_node *p_node_tmp = TD_NULL;

    if (osal_list_empty(&g_list_mem) == 1) {
        os_print("%s, %d list empty.free err 0x%x \r\n", __func__, __LINE__, mem->phy_addr);
        return;
    }

    liteos_mem_lock(&lock_flag);

    osal_list_for_each_entry_safe(p_node, p_node_tmp, &g_list_mem, node) {
        if (p_node->phy_addr + p_node->offset == mem->phy_addr) {
            osal_list_del(&(p_node->node));
            is_found = 1;
            break;
        }
    }

    if (is_found == 1) {
#ifdef MEM_DEBUG_PRINT_EN
        os_print("free phy_addr 0x%x p_node %p \r\n", mem->phy_addr, p_node);
#endif
        liteos_mem_pool_free(p_node->vir_addr);
        liteos_mem_pool_free((td_void *)p_node);
        p_node = TD_NULL;
#ifdef MEM_DEBUG_PRINT_EN
        os_print("====free phy_addr ok==== \r\n");
#endif
    } else {
        os_print("%s, %d phy_addr 0x%x is not found !\n", __func__, __LINE__, mem->phy_addr);
    }
    liteos_mem_unlock(&lock_flag);
}

#ifdef VFMW_CHECK_MEM_SUPPORT
td_u32 liteos_mem_check(td_void)
{
    td_u32 is_find = 0;
    td_bool mem_err = TD_FALSE;
    td_ulong lock_flag = 0;
    mem_node *p_node = TD_NULL;

    liteos_mem_lock(&lock_flag);
    if (osal_list_empty(&g_list_mem) == 0) {
        osal_list_for_each_entry(p_node, &g_list_mem, node) {
            mem_err = liteos_mem_check_mem(p_node->vir_addr, p_node->len);
            if (mem_err == TD_TRUE) {
                vdec_record_chr_event(-1, MT_VDEC_KEY_ERROR, MT_VDEC_VFMW_KEY_ERROR, MT_VDEC_VFMW_MEM_ERR);
                os_print("====mem: %s is damaged, pls check!==== \r\n", p_node->name);
                is_find = 1;
            }
        }
    }
    liteos_mem_unlock(&lock_flag);

    return (is_find == 1) ? TD_TRUE : TD_FALSE;
}

td_void liteos_mem_record(td_void)
{
    td_s32 ret;
    td_s32 len;
    td_bool mem_err = TD_FALSE;
    td_ulong lock_flag = 0;
    mem_node *p_node = TD_NULL;
    os_file *tmp_file = TD_NULL;
    td_char file_name[VFMW_MEM_NAME_MAX_LEN] = {0};
    td_char buf[VFMW_MEM_NAME_MAX_LEN];
    td_char *dbg_path = dbg_get_dbg_path();

    ret = snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1, "%s/%s", dbg_path, "vfmw_err_mem.txt");
    vfmw_check(ret >= 0);

    tmp_file = os_fopen(file_name, OS_RDWR | OS_CREATE | OS_TRUNC, OS_OPEN_MODE);
    vfmw_check(tmp_file != TD_NULL);

    ret = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "%-19s|%-19s|%-19s|%-10s|%-8s|\r\n",
            "buf_name", "phy_addr", "buf_len", "align", "mem_err");
    vfmw_check_goto(ret >= 0, out);
    len = os_fwrite(buf, strlen(buf), tmp_file);
    vfmw_check_goto(len == (td_s32)strlen(buf), out);

    liteos_mem_lock(&lock_flag);
    if (osal_list_empty(&g_list_mem) == 0) {
        osal_list_for_each_entry(p_node, &g_list_mem, node) {
            mem_err = liteos_mem_check_mem(p_node->vir_addr, p_node->len);
            if (mem_err == TD_TRUE) {
                os_print("====mem: %s is damaged, pls check: %s!==== \r\n", p_node->name, file_name);
                ret = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "%-19s|0x%-17x|0x%-17x|%-10u|%-8u|\r\n",
                    p_node->name, p_node->phy_addr, p_node->len, p_node->align, mem_err);
                vfmw_check_goto(ret >= 0, out);
                len = os_fwrite(buf, strlen(buf), tmp_file);
                vfmw_check_goto(len == (td_s32)strlen(buf), out);
            }
        }
    }
    os_fsync(tmp_file);
out:
    liteos_mem_unlock(&lock_flag);
    os_fclose(tmp_file);
}
#endif

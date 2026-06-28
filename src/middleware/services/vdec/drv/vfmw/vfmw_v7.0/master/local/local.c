/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "local.h"
#include "vctrl.h"
#include "dbg.h"
#include "vfmw_pdt.h"
#ifdef VFMW_VMM_SUPPORT
#include "vmm_intf.h"
#endif

typedef struct {
    td_s32 ref_count;
    vfmw_mem_attach attach_mem[VFMW_CHAN_NUM];
    fun_vfmw_event_report fp_vdec_report;
} local_entry;

static local_entry g_local_entry;

static INLINE local_entry *local_get_entry(td_void)
{
    return &g_local_entry;
}

#ifdef VFMW_VMM_SUPPORT
STATIC td_void local_proc_alloc_mem(const td_void *args, td_u32 len)
{
    td_s32 ret;
    vmm_cmd_prio proir;
    vfmw_mem_report *mem_report_info = TD_NULL;
    vmm_buffer vmm_buf;

    if (0) {
        dprint(PRN_ERROR, "%s ERROR: Arg len %d != %d\n", __func__, len, sizeof(vfmw_mem_report));
        return;
    }

    mem_report_info = (vfmw_mem_report *)args;

    vfmw_check_sec_func(memset_s(&vmm_buf, sizeof(vmm_buffer), 0, sizeof(vmm_buffer)));

    if (snprintf_s(vmm_buf.buf_name, sizeof(vmm_buf.buf_name),
                   sizeof(vmm_buf.buf_name) - 1, "%s", mem_report_info->buf_name) < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    vmm_buf.buf_name[sizeof(vmm_buf.buf_name) - 1] = '\0';
    vmm_buf.map = mem_report_info->is_map;
    vmm_buf.cache = mem_report_info->is_cache;
    vmm_buf.size = mem_report_info->size;
    vmm_buf.priv_id = mem_report_info->unique_id;
    vmm_buf.mem_type = VMM_MEM_TYPE_SYS_ADAPT;
    vmm_buf.sec_flag = 0;

    proir = (mem_report_info->phy_addr == 1) ? VMM_CMD_PRIO_MAX : VMM_CMD_PRIO_MIN;

    ret = vmm_send_command(VMM_CMD_ALLOC, proir, &vmm_buf);
    if (ret != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s ERROR: VMM_CMD_ALLOC %s size %d failed!\n",
               __func__, vmm_buf.buf_name, vmm_buf.size);
    }

    return;
}

STATIC td_void local_proc_rels_mem(const td_void *args, td_u32 len)
{
    td_s32 ret;
    vfmw_mem_report *mem_report_info = TD_NULL;
    vmm_buffer vmm_buf;

    if (0) {
        dprint(PRN_ERROR, "%s ERROR: Arg len %d != %d\n",
               __func__, len, sizeof(vfmw_mem_report));
        return;
    }

    mem_report_info = (vfmw_mem_report *)args;

    vfmw_check_sec_func(memset_s(&vmm_buf, sizeof(vmm_buffer), 0, sizeof(vmm_buffer)));

    if (snprintf_s(vmm_buf.buf_name, sizeof(vmm_buf.buf_name),
                   sizeof(vmm_buf.buf_name) - 1, "%s", mem_report_info->buf_name) < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    vmm_buf.buf_name[sizeof(vmm_buf.buf_name) - 1] = '\0';
    vmm_buf.dma_buf = mem_report_info->dma_buf;
    vmm_buf.start_phy_addr = mem_report_info->phy_addr;
    vmm_buf.start_vir_addr = mem_report_info->vir_addr;
    vmm_buf.map = mem_report_info->is_map;
    vmm_buf.cache = mem_report_info->is_cache;
    vmm_buf.size = mem_report_info->size;
    vmm_buf.priv_id = mem_report_info->unique_id;
    vmm_buf.mem_type = VMM_MEM_TYPE_SYS_ADAPT;
    vmm_buf.sec_flag = 0;

    ret = vmm_send_command(VMM_CMD_RELEASE, VMM_CMD_PRIO_MIN, &vmm_buf);
    if (ret != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s ERROR: VMM_CMD_RELEASE %s phy 0x%x failed!\n",
               __func__, vmm_buf.buf_name, vmm_buf.start_phy_addr);
    }

    return;
}

STATIC td_s32 local_proc_inner_event(td_s32 type, const td_void *args, td_u32 len)
{
    switch (type) {
        case EVNT_ALLOC_MEM:
            local_proc_alloc_mem(args, len);
            break;

        case EVNT_RELEASE_MEM:
            local_proc_rels_mem(args, len);
            break;

        case EVNT_RELEASE_BURST_MEM:
            break;

        case EVNT_CANCEL_ALLOC:
            break;

        case EVNT_SET_ADDRESS_IN:
            break;

        default:
            return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}
#endif

td_s32 local_set_callback(vfmw_adpt_type type, fun_vfmw_event_report fp_event_report)
{
    local_entry *entry = local_get_entry();

    vfmw_check_ret(fp_event_report != TD_NULL, EXT_FAILURE);

    switch (type) {
        case ADPT_TYPE_VDEC:
            entry->fp_vdec_report = fp_event_report;
            break;

        default:
            dprint(PRN_ERROR, "%s Unkown Adapter type: %d\n", __func__, type);
            return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 local_event_report(td_s32 instance_id, td_s32 type, const td_void *args, td_s32 len)
{
    td_s32 ret;
    vfmw_adpt_type adpt_type = ADPT_TYPE_MAX;
    local_entry *entry = local_get_entry();

    ret = vctrl_control(instance_id, VFMW_CID_INQUIRE_ADPT_TYPE, &adpt_type);
    vfmw_check_ret(ret == EXT_SUCCESS, EXT_FAILURE);

#ifdef VFMW_VMM_SUPPORT
    ret = local_proc_inner_event(type, args, 0);
    if (ret == EXT_SUCCESS) {
        return EXT_SUCCESS;
    }
#endif

    switch (adpt_type) {
        case ADPT_TYPE_VDEC:
            vfmw_check_ret_print(entry->fp_vdec_report != TD_NULL, EXT_FAILURE, "ERROR: fp_vdec_report = TD_NULL\n");
            ret = entry->fp_vdec_report(instance_id, type, args, len);
            break;

        default:
            ret = EXT_FAILURE;
            dprint(PRN_ERROR, "Unsupport adpt_type %d.\n", adpt_type);
            break;
    }

    return ret;
}

STATIC td_s32 local_attach_mem_process(td_s32 chan_id, td_void *args)
{
    td_s32 ret = EXT_FAILURE;
    mem_record mem = { 0 };
    vfmw_mem_attach *chan_mem = TD_NULL;
    local_entry *entry = local_get_entry();

    vfmw_unused(args);

    mem.length = VFMW_MAX_META_USERDATA_SIZE;
    mem.is_cached = 0;
    mem.type = NO_CACHE_MEM_TYPE;
    mem.unid = (td_u32)chan_id;
    mem.mem_name = "vfmw_attach_buf";
    vfmw_check_ret(os_malloc != TD_NULL, EXT_FAILURE);
    if (os_malloc(&mem) != OSAL_OK) {
        dprint(PRN_ERROR, "%s alloc AttachBuf failed!\n", __func__);
        return ret;
    }

    chan_mem = &entry->attach_mem[chan_id];
    vfmw_check_sec_func(memset_s(chan_mem, sizeof(vfmw_mem_attach), 0, sizeof(vfmw_mem_attach)));

    chan_mem->vir_addr = ptr_uint64(mem.vir_addr);
    chan_mem->dma_buf = mem.dma_buf;
    chan_mem->phy_addr = mem.phy_addr;
    chan_mem->type = NOR_MEM_TYPE;
    ret = vctrl_control(chan_id, VFMW_CID_BIND_ATTACH, chan_mem);

    return ret;
}

STATIC td_bool local_is_need_attach(td_s32 chan_id, td_void *args)
{
    vfmw_chan_cfg *chan_cfg = (vfmw_chan_cfg *)args;
    vfmw_mem_attach *chan_mem = TD_NULL;
    local_entry *entry = local_get_entry();
    chan_mem = &entry->attach_mem[chan_id];

    if (chan_mem->fd > 0) {
        return TD_FALSE;
    }

    return pdt_need_attach_mem(chan_cfg->vid_std, TD_NULL);
}

STATIC td_s32 local_after_cfg_process(td_s32 chan_id, td_s32 cmd, td_void *args, td_u32 len)
{
    vfmw_unused(len);
    vfmw_unused(cmd);
    if (local_is_need_attach(chan_id, args) != TD_TRUE) {
        return EXT_SUCCESS;
    }

    return local_attach_mem_process(chan_id, args);
}

STATIC td_s32 local_after_creat_process(td_s32 chan_id, td_s32 cmd, td_void *args, td_u32 len)
{
    vfmw_mem_attach *chan_mem = TD_NULL;
    local_entry *entry = local_get_entry();

    vfmw_unused(cmd);
    vfmw_unused(args);
    vfmw_unused(len);

    chan_mem = &entry->attach_mem[chan_id];

    chan_mem->fd = -1;

    return EXT_SUCCESS;
}

STATIC td_s32 local_after_destory_process(td_s32 chan_id, td_s32 cmd, td_void *args, td_u32 len)
{
    vfmw_mem_attach *chan_att_mem = TD_NULL;
    local_entry *entry = local_get_entry();
    mem_record mem = { 0 };

    vfmw_unused(cmd);
    vfmw_unused(args);
    vfmw_unused(len);

    chan_att_mem = &entry->attach_mem[chan_id];

    if (chan_att_mem->fd > 0) {
        mem.dma_buf = chan_att_mem->dma_buf;
        mem.length = VFMW_MAX_META_USERDATA_SIZE;
        mem.phy_addr = chan_att_mem->phy_addr;
        mem.vir_addr = uint64_ptr(chan_att_mem->vir_addr);
        mem.is_cached = 0;
        os_put_mem_fd(chan_att_mem->fd);
        os_free(&mem);
    }

    return EXT_SUCCESS;
}

STATIC td_s32 local_after_control(td_s32 chan_id, td_s32 cmd, td_void *args, td_u32 len)
{
    td_s32 ret;
    vfmw_check_ret(chan_id >= 0, EXT_FAILURE);
    vfmw_check_ret(chan_id < VFMW_CHAN_NUM, EXT_FAILURE);

    switch (cmd) {
        case VFMW_CID_CFG_CHAN:
            ret = local_after_cfg_process(chan_id, cmd, args, len);
            break;

        case VFMW_CID_CREATE_CHAN:
            ret = local_after_creat_process(chan_id, cmd, args, len);
            break;

        case VFMW_CID_DESTROY_CHAN:
            ret = local_after_destory_process(chan_id, cmd, args, len);
            break;

        default:
            ret = EXT_SUCCESS;
            break;
    }

    return ret;
}

td_s32 local_open(td_void)
{
    local_entry *entry = local_get_entry();

    vfmw_check_sec_func(memset_s(entry, sizeof(local_entry), 0, sizeof(local_entry)));

    return vctrl_open();
}

td_s32 local_close(td_void)
{
    return vctrl_close();
}

td_s32 local_init(td_void *args, td_u32 len)
{
    td_s32 ret;
    local_entry *entry = local_get_entry();
    vfmw_init_param *param = (vfmw_init_param *)args;

    vfmw_unused(len);
    vfmw_check_ret(param != TD_NULL, EXT_FAILURE);
    ret = EXT_FAILURE;
    if (entry->ref_count > 0) {
        goto exit;
    }

    ret = vctrl_init(args);

    vctrl_set_event_interface(&local_event_report);

exit:
    ret = local_set_callback(param->adpt_type, param->fn_callback);
    if (ret == EXT_SUCCESS) {
        entry->ref_count++;
        vfmw_check_ret_print(entry->ref_count < 0x7fffffff,
                             EXT_FAILURE, "ref_count %d invalid!\n", entry->ref_count);
    }

    return ret;
}

td_s32 local_exit(const td_void *args)
{
    local_entry *entry = local_get_entry();

    if (entry->ref_count <= 0) {
        return EXT_FAILURE;
    }

    entry->ref_count--;

    if (entry->ref_count > 0) {
        return EXT_SUCCESS;
    }

    return vctrl_exit(args);
}

td_s32 local_suspend(const td_void *args)
{
    return vctrl_suspend(args);
}

td_s32 local_resume(const td_void *args)
{
    return vctrl_resume(args);
}

td_s32 local_get_image(td_s32 chan_id, td_void *image)
{
    return vctrl_get_image(chan_id, image);
}

td_s32 local_check_image(td_s32 chan_id, td_void *image)
{
    return vctrl_check_image(chan_id, image);
}

td_s32 local_release_image(td_s32 chan_id, const td_void *image)
{
    vfmw_image_rls rls_info;

    vctrl_img_2_rls_info(image, &rls_info);
    return vctrl_release_image(chan_id, &rls_info);
}

STATIC td_void local_before_control(td_s32 cmd, td_void *args)
{
    (td_void)(cmd);
    (td_void)(args);
    return;
}

td_s32 local_control(td_s32 chan_id, td_s32 cmd, td_void *args, td_u32 len)
{
    td_s32 ret;

    local_before_control(cmd, args);

    ret = vctrl_control(chan_id, cmd, args);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    ret = local_after_control(chan_id, cmd, args, len);

    return ret;
}

td_s32 local_check_eos(td_s32 chan_id, td_s32 *flag)
{
    return vctrl_check_eos(chan_id, flag);
}

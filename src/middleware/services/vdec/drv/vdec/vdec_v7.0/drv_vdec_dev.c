/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */
#include "drv_vdec_sdk.h"
#include "drv_ioctl_vdec.h"
#include "drv_vdec_proc.h"
#include "drv_vdec.h"
#include "vdec_priv_type.h"
#include "securec.h"
#include "vdec_chr.h"

#define VDEC_DEV_MAX_CMD_NUM    18

typedef struct {
    td_bool setup;
} vdec_dev;

typedef struct {
    osal_atomic open_cnt;
    td_bool is_inited;
    vdec_dev device;
} vdec_dev_entry;

static vdec_dev_entry g_vdec_entry = { 0 };
static osal_atomic g_vdec_init_refcnt = { 0 };

td_u32 vdec_get_inc_init_refcnt(td_void)
{
    return osal_atomic_inc_return(&g_vdec_init_refcnt);
}

td_u32 vdec_get_dec_init_refcnt(td_void)
{
    return osal_atomic_dec_return(&g_vdec_init_refcnt);
}

STATIC vdec_dev_entry* vdec_dev_get_entry(td_void)
{
    return &g_vdec_entry;
}

STATIC td_s32 vdec_dev_create(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_create *param = (vdec_ioctl_create*)arg;

    vdec_unused(cmd);

    ret = drv_vdec_create(&param->chan_id, private_data);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vdec create fail!\n");
        return EXT_FAILURE;
    }
    vdec_record_chr_event(param->chan_id, MT_VDEC_CREATE, MT_VDEC_CREATE_ID_OK, MT_VDEC_INFO_3_DEFAULT);

    vdec_print_info("chan:%d success!\r\n", param->chan_id);

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_destroy(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_destroy *param = (vdec_ioctl_destroy*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_destroy(param->chan_id);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vdec destroy fail!\n");
        return EXT_FAILURE;
    }

    vdec_print_info("chan:%d success!\n", param->chan_id);

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_start(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_start *param = (vdec_ioctl_start*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_start(param->chan_id);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vdec start fail!\n");
        return EXT_FAILURE;
    }

    vdec_print_info("chan:%d success!\n", param->chan_id);

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_stop(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_stop *param = (vdec_ioctl_stop*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_stop(param->chan_id);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vdec stop fail!\n");
        return EXT_FAILURE;
    }

    vdec_print_info("chan:%d success!\n", param->chan_id);

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_reset(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_reset *param = (vdec_ioctl_reset*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_reset(param->chan_id);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vdec reset fail!\n");
        return EXT_FAILURE;
    }

    vdec_print_info("chan:%d success!\n", param->chan_id);

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_flush(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_flush *param = (vdec_ioctl_flush*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_flush(param->chan_id, &param->option);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vdec flush fail!\n");
        return EXT_FAILURE;
    }

    vdec_print_info("chan:%d success!\n", param->chan_id);

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_decode(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_decode *param = (vdec_ioctl_decode*)arg;
    drv_vdec_decode_param dec_param = {0};

    vdec_unused(cmd);
    vdec_unused(private_data);

    dec_param.frame = &param->frame;
    dec_param.stream = &param->stream;
    dec_param.option = param->option;

    ret = drv_vdec_decode(param->chan_id, &dec_param);
    if (ret != EXT_SUCCESS) {
        vdec_print_err("vdec decode fail!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_get_attr(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_get_attr *param = (vdec_ioctl_get_attr*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_get_attr(param->chan_id, &param->attr);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vdec get attr fail!\n");
        return EXT_FAILURE;
    }
    vdec_record_chr_event(param->chan_id, MT_VDEC_GET_ATTR, MT_VDEC_GET_HW_ATTR, MT_VDEC_INFO_3_DEFAULT);

    vdec_print_info("chan:%d success!\n", param->chan_id);

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_set_attr(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_set_attr *param = (vdec_ioctl_set_attr*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_set_attr(param->chan_id, param->vdec_handle, &param->attr);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vdec set attr fail!\n");
        return EXT_FAILURE;
    }

    vdec_print_info("chan:%d success!\n", param->chan_id);

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_register_event(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_register *param = (vdec_ioctl_register*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_register_event(param->chan_id, param->event_map);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vdec register event fail!\n");
        return EXT_FAILURE;
    }

    vdec_print_info("chan:%d success!\n", param->chan_id);

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_listen_event(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_listen *param = (vdec_ioctl_listen*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);
    ret = drv_vdec_listen_event(param->chan_id, &param->option, &param->event);
    if (ret != EXT_SUCCESS) {
        vdec_print_warn("vdec listen event fail!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_put_stream(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_put_stream *param = (vdec_ioctl_put_stream*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);
    ret = drv_vdec_put_stream(param->chan_id, &param->stream);
    if (ret != EXT_SUCCESS) {
        vdec_print_err("vdec queue stream fail!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_get_stream(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_get_stream *param = (vdec_ioctl_get_stream*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_get_stream(param->chan_id, &param->option, &param->stream);
    if (ret != EXT_SUCCESS) {
        vdec_print_dbg("vdec dequeue stream fail!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_acq_frame(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_acq_frame *param = (vdec_ioctl_acq_frame*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_acquire_frame(param->chan_id, &param->option, &param->frame, &param->ext_frm_info);
    if (ret != EXT_SUCCESS) {
        vdec_print_dbg("vdec acquire frame fail!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_rls_frame(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_rel_frame *param = (vdec_ioctl_rel_frame*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_release_frame(param->chan_id, &param->frame);
    if (ret != EXT_SUCCESS) {
        vdec_print_err("vdec release frame fail!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_get_feature(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_feature *param = (vdec_ioctl_feature*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_get_feature(param->chan_id, &param->feature);
    if (ret != EXT_SUCCESS) {
        vdec_print_err("vdec get feature fail!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_get_status(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_status *param = (vdec_ioctl_status*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_get_status(param->chan_id, &param->status);
    if (ret != EXT_SUCCESS) {
        vdec_print_err("vdec get status fail!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_get_param(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_param *param = (vdec_ioctl_param*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_get_param(param->chan_id, &param->param);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vdec get param fail!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_set_param(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_param *param = (vdec_ioctl_param*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_set_param(param->chan_id, &param->param);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vdec set param fail!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_send_eos(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_send_eos *param = (vdec_ioctl_send_eos*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_send_eos(param->chan_id);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_is_std_support(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_std_support *param = (vdec_ioctl_std_support*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_is_std_support(&param->support_info);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_get_cap(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_capability *param = (vdec_ioctl_capability*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_get_cap(&param->cap);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_command(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_command *param = (vdec_ioctl_command*)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    vdec_check_ret(param->param_size <= VDEC_IOCTL_BUF_SIZE);

    ret = drv_vdec_command(param->chan_id, param->cmd_id, param->param, param->param_size);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_bind_frm_buffer(td_u32 cmd, td_void *arg, td_void *private_data)
{
    td_s32 ret;
    vdec_ioctl_bind_frm_buf *param = (vdec_ioctl_bind_frm_buf *)arg;

    vdec_unused(cmd);
    vdec_unused(private_data);

    ret = drv_vdec_bind_frm_buffer(param->chan_id, &param->buffer);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

osal_ioctl_cmd g_cmd_table[] = {
    {UMAPC_VDEC_CREATE, vdec_dev_create},
    {UMAPC_VDEC_DESTROY, vdec_dev_destroy},
    {UMAPC_VDEC_START, vdec_dev_start},
    {UMAPC_VDEC_STOP, vdec_dev_stop},
    {UMAPC_VDEC_RESET, vdec_dev_reset},
    {UMAPC_VDEC_FLUSH, vdec_dev_flush},
    {UMAPC_VDEC_DECODE, vdec_dev_decode},
    {UMAPC_VDEC_GET_ATTR, vdec_dev_get_attr},
    {UMAPC_VDEC_SET_ATTR, vdec_dev_set_attr},
    {UMAPC_VDEC_REGISTER_EVENT, vdec_dev_register_event},
    {UMAPC_VDEC_LISTEN_EVENT, vdec_dev_listen_event},
    {UMAPC_VDEC_GET_STREAM, vdec_dev_get_stream},
    {UMAPC_VDEC_PUT_STREAM, vdec_dev_put_stream},
    {UMAPC_VDEC_ACQUIRE_FRAME, vdec_dev_acq_frame},
    {UMAPC_VDEC_RELEASE_FRAME, vdec_dev_rls_frame},
    {UMAPC_VDEC_GET_PARAM, vdec_dev_get_param},
    {UMAPC_VDEC_SET_PARAM, vdec_dev_set_param},
    {UMAPC_VDEC_GET_FEATURE, vdec_dev_get_feature},
    {UMAPC_VDEC_GET_STATUS, vdec_dev_get_status},
    {UMAPC_VDEC_IS_STD_SUPPORT, vdec_dev_is_std_support},
    {UMAPC_VDEC_GET_CAP, vdec_dev_get_cap},
    {UMAPC_VDEC_SEND_EOS, vdec_dev_send_eos},
    {UMAPC_VDEC_COMMAND, vdec_dev_command},
    {UMAPC_VDEC_BIND_FRM_BUF, vdec_dev_bind_frm_buffer},
};

#ifdef VDEC_PROC_SUPPORT
STATIC td_void vdec_dev_read_proc(td_void *buf, td_u32 len, td_u32 *used)
{
    vdec_dev_entry *entry = vdec_dev_get_entry();
    vdec_dev *device = &entry->device;

    vdec_drv_proc_dump(buf, len, used, VP_VDEC_HEAD);
    vdec_drv_proc_dump(buf, len, used, VP_D_D,  "open_count", osal_atomic_read(&entry->open_cnt),
        "setup", device->setup);
}

STATIC td_s32 vdec_dev_proc_read(struct osal_proc_dir_entry *entry)
{
    td_u32 used = 0;

    if (entry == TD_NULL) {
        vdec_print_err("vdec_dev_proc_read entry null\n");
        return EXT_FAILURE;
    }

    vdec_dev_read_proc(entry->seqfile, VDEC_PROC_READ_BUF_SIZE, &used);
    vdec_proc_read(entry->seqfile, VDEC_PROC_READ_BUF_SIZE, &used);

    vdec_drv_proc_dump(entry->seqfile, VDEC_PROC_READ_BUF_SIZE, &used, VP_VDEC_TAIL);

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_dev_proc_init(td_void)
{
    td_s32 ret;
    td_u32 cmd_cnt;
    osal_proc_cmd *cmd_list = TD_NULL;

    ret = vdec_proc_get_cmd((td_void **)&cmd_list, &cmd_cnt);
    if (ret != EXT_SUCCESS) {
        vdec_print_err("vdec_proc_get_cmd fail!\n");
        return EXT_FAILURE;
    }

    ret = vdec_drv_create_proc(VDEC_PROC_NAME, &vdec_dev_proc_read, cmd_list, cmd_cnt);
    if (ret != EXT_SUCCESS) {
        vdec_print_err("create vdec proc fail!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_void vdec_dev_proc_deinit(td_void)
{
    vdec_drv_destroy_proc(VDEC_PROC_NAME);
}
#endif

STATIC td_s32 vdec_dev_open(td_void *private_data)
{
    td_s32 ret;
    vdec_dev_entry *entry = vdec_dev_get_entry();
    const td_s32 fd = 0xff;

    vdec_unused(private_data);

    if (osal_atomic_inc_return(&entry->open_cnt) == 1) {
        ret = drv_vdec_open();
        if (ret != EXT_SUCCESS) {
            vdec_print_alert("vdec open fail!\n");
            osal_atomic_dec_return(&entry->open_cnt);
            return VDEC_INVALID_HANDLE;
        }
    }

    return fd;
}

STATIC td_s32 vdec_dev_close(td_void *private_data)
{
    vdec_dev_entry *entry = vdec_dev_get_entry();

    drv_vdec_release(private_data);

    if (osal_atomic_dec_return(&entry->open_cnt) == 0) {
        drv_vdec_close();
    }

    return EXT_SUCCESS;
}

td_s32 vdec_open(const td_char *path_name, td_s32 flags, td_s32 mode)
{
    vdec_dev_entry *entry = vdec_dev_get_entry();

    if (entry->is_inited == TD_FALSE) {
        vdec_print_alert("vdec not init.\n");
        return EXT_FAILURE;
    }

    vdec_unused(path_name);
    vdec_unused(flags);
    vdec_unused(mode);

    return vdec_dev_open(TD_NULL);
}

td_s32 vdec_close(td_s32 module_idx)
{
    vdec_dev_entry *entry = vdec_dev_get_entry();

    if (entry->is_inited == TD_FALSE) {
        vdec_print_alert("vdec not init.\n");
        return EXT_FAILURE;
    }

    vdec_unused(module_idx);

    return vdec_dev_close(TD_NULL);
}

td_s32 vdec_ioctl(td_s32 module_idx, td_ulong ioctl_cmd, td_void *para)
{
    td_s32 ret;
    vdec_dev_entry *entry = vdec_dev_get_entry();

    if (entry->is_inited == TD_FALSE) {
        vdec_print_alert("vdec not init.\n");
        return EXT_FAILURE;
    }

    vdec_unused(module_idx);

    if (ioctl_cmd >= UMAPC_VDEC_MAX) {
        vdec_print_alert("Invalid cmd, cmd = %lu!", ioctl_cmd);
        return EXT_FAILURE;
    }

    if (para == TD_NULL) {
        vdec_print_alert("vdec_ioctl para is null\n");
        return EXT_FAILURE;
    }

    ret = g_cmd_table[ioctl_cmd].handler((td_u32)ioctl_cmd, para, TD_NULL);
    return ret;
}

STATIC td_s32 vdec_dev_suspend(td_void *private_data)
{
    td_s32 ret;

    vdec_unused(private_data);

    ret = drv_vdec_suspend();
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vdec suspend err.\n");
    } else {
        vdec_print_info("vdec suspend ok.\n");
    }

    return ret;
}

STATIC td_s32 vdec_dev_resume(td_void *private_data)
{
    td_s32 ret;

    vdec_unused(private_data);

    ret = drv_vdec_resume();
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("vdec resume err.\n");
    } else {
        vdec_print_info("vdec resume ok.\n");
    }

    return ret;
}

STATIC td_s32 vdec_drv_open(td_void *private_data)
{
    vdec_unused(private_data);

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_drv_close(td_void *private_data)
{
    vdec_unused(private_data);

    return EXT_SUCCESS;
}

STATIC osal_fileops g_vdec_fops = {
    .open = vdec_drv_open,
    .release = vdec_drv_close,
    .cmd_list = g_cmd_table,
    .cmd_cnt = sizeof(g_cmd_table) / sizeof(osal_ioctl_cmd),
};

STATIC osal_pmops g_vdec_pm_ops = {
    .pm_suspend = vdec_dev_suspend,
    .pm_resume = vdec_dev_resume,
};

STATIC osal_dev g_vdec_dev = {
    .minor = 0,
    .fops = &g_vdec_fops,
    .pmops = &g_vdec_pm_ops,
};

STATIC td_s32 vdec_dev_register(void)
{
    td_s32 ret;
    osal_dev *dev = &g_vdec_dev;
    vdec_dev_entry *entry = vdec_dev_get_entry();

    ret = strncpy_s(dev->name, sizeof(dev->name), VDEC_DEV_NAME, strlen(VDEC_DEV_NAME));
    if (ret < 0) {
        vdec_print_err("strncpy_s failed. ret = 0x%x\n", ret);
        return ret;
    }

    ret = osal_dev_register(dev);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("osal_dev_register failed\n");
        return ret;
    }

    entry->device.setup = TD_TRUE;

    return EXT_SUCCESS;
}

STATIC td_void vdec_dev_unregister(void)
{
    vdec_dev_entry *entry = vdec_dev_get_entry();

    osal_dev_unregister(&g_vdec_dev);

    entry->device.setup = TD_FALSE;
}

td_s32 uapi_vdec_suspend(td_void *private_data)
{
    vdec_dev_entry *entry = vdec_dev_get_entry();

    if (entry->is_inited == TD_FALSE) {
        vdec_print_alert("vdec not init.\n");
        return EXT_FAILURE;
    }

    return vdec_dev_suspend(private_data);
}

td_s32 uapi_vdec_resume(td_void *private_data)
{
    vdec_dev_entry *entry = vdec_dev_get_entry();

    if (entry->is_inited == TD_FALSE) {
        vdec_print_alert("vdec not init.\n");
        return EXT_FAILURE;
    }

    return vdec_dev_resume(private_data);
}

td_s32 uapi_vdec_module_init(td_void)
{
    vdec_dev_entry *entry = vdec_dev_get_entry();

    if (vdec_get_inc_init_refcnt() != 1) {
        return EXT_SUCCESS;
    }

    if (entry->is_inited == TD_TRUE) {
        return EXT_SUCCESS;
    }

    if (vdec_dev_register() != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

#ifdef VDEC_PROC_SUPPORT
    if (vdec_dev_proc_init() != EXT_SUCCESS) {
        vdec_print_alert("vdec proc create fail!\n");
        goto err0;
    }
#endif

    drv_vdec_init();

    vdec_record_chr_event(-1, MT_VDEC_COMPONENT_INIT, MT_VDEC_INIT_OK, MT_VDEC_INFO_3_DEFAULT);

    vdec_check_goto(drv_vdec_vfmw_mod_init() == EXT_SUCCESS, err1);

    vdec_record_chr_event(-1, MT_VDEC_COMPONENT_INIT, MT_VDEC_VFMW_INIT_OK, MT_VDEC_INFO_3_DEFAULT);

    osal_atomic_set(&entry->open_cnt, 0);
    entry->is_inited = TD_TRUE;

    vdec_print_notice("Load soc_vdec.ko success.\t\n");

    return EXT_SUCCESS;
err1:
    drv_vdec_deinit();
#ifdef VDEC_PROC_SUPPORT
    vdec_dev_proc_deinit();

err0:
#endif
    vdec_dev_unregister();
    return EXT_FAILURE;
}

td_void uapi_vdec_module_exit(td_void)
{
    vdec_dev_entry *entry = vdec_dev_get_entry();

    if (vdec_get_dec_init_refcnt() != 0) {
        return;
    }

    if (entry->is_inited == TD_FALSE) {
        vdec_print_alert("vdec not init.\n");
        return;
    }

    drv_vdec_vfmw_mod_exit();
    vdec_record_chr_event(-1, MT_VDEC_COMPONENT_DEINIT, MT_VDEC_VFMW_DEINIT_OK, MT_VDEC_INFO_3_DEFAULT);

    drv_vdec_deinit();

#ifdef VDEC_PROC_SUPPORT
    vdec_dev_proc_deinit();
#endif

    vdec_dev_unregister();
    entry->is_inited = TD_FALSE;
    vdec_record_chr_event(-1, MT_VDEC_COMPONENT_DEINIT, MT_VDEC_DEINIT_OK, MT_VDEC_INFO_3_DEFAULT);

    vdec_print_notice("Unload soc_vdec.ko success.\n");
}

td_s32 uapi_vdec_dfx_proc_read(td_s32 argc, const td_char *argv[])
{
#ifdef VDEC_PROC_SUPPORT
    vdec_dev_entry *entry = vdec_dev_get_entry();

    if (entry->is_inited == TD_FALSE) {
        vdec_print_alert("vdec not init.\n");
        return EXT_FAILURE;
    }

    return vdec_drv_proc_read(argc, argv);
#else
    (td_void)(argc);
    (td_void)(argv);
    vdec_print_alert("VDEC_PROC_SUPPORT is not define.\n");
    return EXT_FAILURE;
#endif
}

td_s32 uapi_vdec_dfx_proc_write(td_s32 argc, const td_char *argv[])
{
#ifdef VDEC_PROC_SUPPORT
    vdec_dev_entry *entry = vdec_dev_get_entry();

    if (entry->is_inited == TD_FALSE) {
        vdec_print_alert("vdec not init.\n");
        return EXT_FAILURE;
    }

    return vdec_drv_proc_write(argc, argv);
#else
    (td_void)(argc);
    (td_void)(argv);
    vdec_print_alert("VDEC_PROC_SUPPORT is not define.\n");
    return EXT_FAILURE;
#endif
}

#ifdef MODULE
osal_module_init(uapi_vdec_module_init);
osal_module_exit(uapi_vdec_module_exit);
#endif

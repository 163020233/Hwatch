/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#include "vdec_hal.h"
#include <fcntl.h>
#include <unistd.h>
#ifdef GFX_LINUX
#include <sys/ioctl.h>
#endif
#include "mpi_vdec_sdk.h"
#include "drv_ioctl_vdec.h"
#include "securec.h"
#include "limits.h"

typedef struct {
    const td_char *dev_path;
    td_s32 dev_fd;
    td_u32 open_cnt;
} vdec_hal_entry;

static vdec_hal_entry g_vdec_hal_entry = {
    .dev_path = VDEC_DEV_PATH,
    .dev_fd = VDEC_INVALID_HANDLE,
    .open_cnt = 0,
};

STATIC vdec_hal_entry* vdec_hal_get_entry(td_void)
{
    return &g_vdec_hal_entry;
}

td_s32 vdec_hal_open(td_void)
{
    vdec_hal_entry *entry = vdec_hal_get_entry();
    td_char real_path[PATH_MAX] = {0};
    if (entry->open_cnt == 0 || entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
#ifdef GFX_LINUX
        if (realpath(entry->dev_path, real_path) == TD_NULL) {
            vdec_print_alert("vdec dev path error! errno:%d str:%s:\n", errno, strerror(errno));
            return EXT_FAILURE;
        }
        entry->dev_fd = vdec_open(real_path, O_RDWR | O_NONBLOCK, 0);
        if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
            vdec_print_alert("hal open vdec device fail!\n");
            return EXT_FAILURE;
        }
#else
        entry->dev_fd = vdec_open(real_path, O_RDWR | O_NONBLOCK, 0);
#endif
    }

    entry->open_cnt++;

    return EXT_SUCCESS;
}

td_s32 vdec_hal_close(td_void)
{
    vdec_hal_entry *entry = vdec_hal_get_entry();

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    if (entry->open_cnt == 1) {
        vdec_close(entry->dev_fd);
        entry->dev_fd = VDEC_INVALID_HANDLE;
    }

    entry->open_cnt--;

    return EXT_SUCCESS;
}

td_s32 vdec_hal_create(td_s32 *chan_id)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_create param = {0};

    vdec_check_ret(chan_id != TD_NULL);

    ret = vdec_hal_open();
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    param.chan_id = VDEC_INVALID_HANDLE;
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_CREATE, &param);
    if (ret != EXT_SUCCESS) {
        entry->open_cnt--;
        return EXT_FAILURE;
    }

    *chan_id = param.chan_id;
    return EXT_SUCCESS;
}

td_s32 vdec_hal_destroy(td_s32 chan_id)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_destroy param = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    param.chan_id = chan_id;
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_DESTROY, &param);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    ret = vdec_hal_close();
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_hal_start(td_s32 chan_id)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_start param = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    param.chan_id = chan_id;
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_START, &param);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_hal_stop(td_s32 chan_id)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_stop param = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    param.chan_id = chan_id;
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_STOP, &param);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_hal_reset(td_s32 chan_id)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_reset param = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    param.chan_id = chan_id;
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_RESET, &param);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_hal_flush(td_s32 chan_id, const ext_vdec_opt_flush *option)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_flush param = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    param.chan_id = chan_id;
    (td_void)memcpy_s(&param.option, sizeof(param.option), option, sizeof(ext_vdec_opt_flush));

    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_FLUSH, &param);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_hal_decode(td_s32 chan_id, const ext_vdec_opt_decode *option,
    const ext_vdec_stream *stream, ext_vdec_frame *frame)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_decode param = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_err("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    param.chan_id = chan_id;
    (td_void)memcpy_s(&param.option, sizeof(param.option), option, sizeof(ext_vdec_opt_decode));
    (td_void)memcpy_s(&param.stream, sizeof(param.stream), stream, sizeof(ext_vdec_stream));

    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_DECODE, &param);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    (td_void)memcpy_s(frame, sizeof(ext_vdec_frame), &param.frame, sizeof(ext_vdec_frame));

    return EXT_SUCCESS;
}

td_s32 vdec_hal_get_attr(td_s32 chan_id, ext_vdec_attr *attr)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_get_attr param = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    param.chan_id = chan_id;
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_GET_ATTR, &param);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    (td_void)memcpy_s(attr, sizeof(ext_vdec_attr), &param.attr, sizeof(param.attr));

    return EXT_SUCCESS;
}

td_s32 vdec_hal_set_attr(td_s32 chan_id, td_handle vdec_handle, const ext_vdec_attr *attr)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_set_attr param = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    (td_void)memcpy_s(&param.attr, sizeof(param.attr), attr, sizeof(ext_vdec_attr));
    param.chan_id = chan_id;
    param.vdec_handle = vdec_handle;

    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_SET_ATTR, &param);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_hal_register_event(td_s32 chan_id, td_u64 event_map)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_register param = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    param.chan_id = chan_id;
    param.event_map = event_map;
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_REGISTER_EVENT, &param);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_hal_listen_event(td_s32 chan_id, const ext_vdec_opt_listen *option, ext_vdec_event *event)
{
    td_s32 ret;

    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_listen param;

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_err("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    (td_void)memcpy_s(&param.option, sizeof(ext_vdec_opt_listen), option, sizeof(ext_vdec_opt_listen));
    (td_void)memcpy_s(&param.event, sizeof(ext_vdec_event), event, sizeof(ext_vdec_event));

    param.chan_id = chan_id;
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_LISTEN_EVENT, &param);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    (td_void)memcpy_s(event, sizeof(param.event), &param.event, sizeof(ext_vdec_event));

    return EXT_SUCCESS;
}

td_s32 vdec_hal_put_stream(td_s32 chan_id, const ext_vdec_stream *stream)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_put_stream param = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_err("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    (td_void)memcpy_s(&param.stream, sizeof(param.stream), stream, sizeof(ext_vdec_stream));

    param.chan_id = chan_id;
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_PUT_STREAM, &param);
    if (ret != EXT_SUCCESS) {
        vdec_print_err("hal queue stream fail! ret %d\n", ret);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_hal_get_stream(td_s32 chan_id, const ext_vdec_opt_get_stream *option, ext_vdec_stream *stream)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_get_stream param = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_err("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    (td_void)memcpy_s(&param.option, sizeof(param.option), option, sizeof(ext_vdec_opt_get_stream));

    param.chan_id = chan_id;
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_GET_STREAM, &param);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    (td_void)memcpy_s(stream, sizeof(ext_vdec_stream), &param.stream, sizeof(param.stream));

    return EXT_SUCCESS;
}

td_s32 vdec_hal_acquire_frame(td_s32 chan_id, const ext_vdec_opt_acq_frame *option, ext_vdec_frame *frame)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_acq_frame param = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_err("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    param.chan_id = chan_id;
    param.option.time_out = option->time_out;
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_ACQUIRE_FRAME, &param);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    (td_void)memcpy_s(frame, sizeof(ext_vdec_frame), &param.frame, sizeof(ext_vdec_frame));

    return EXT_SUCCESS;
}

td_s32 vdec_hal_release_frame(td_s32 chan_id, const ext_vdec_frame *frame)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_rel_frame param = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_err("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    (td_void)memcpy_s(&param.frame, sizeof(ext_vdec_frame), frame, sizeof(ext_vdec_frame));

    param.chan_id = chan_id;
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_RELEASE_FRAME, &param);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_hal_get_param(td_s32 chan_id, ext_vdec_param *param)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_param ioctl_param = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    ioctl_param.chan_id = chan_id;
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_GET_PARAM, &ioctl_param);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }
    (td_void)memcpy_s(param, sizeof(ext_vdec_param), &ioctl_param.param, sizeof(ext_vdec_param));

    return EXT_SUCCESS;
}

td_s32 vdec_hal_set_param(td_s32 chan_id, const ext_vdec_param *param)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_param ioctl_param = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    ioctl_param.chan_id = chan_id;
    (td_void)memcpy_s(&ioctl_param.param, sizeof(ext_vdec_param), param, sizeof(ext_vdec_param));
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_SET_PARAM, &ioctl_param);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_hal_get_feature(td_s32 chan_id, ext_vdec_feature *feature)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_feature ioctl_feature = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    ioctl_feature.chan_id = chan_id;
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_GET_FEATURE, &ioctl_feature);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }
    (td_void)memcpy_s(feature, sizeof(ext_vdec_feature), &ioctl_feature.feature, sizeof(ext_vdec_feature));

    return EXT_SUCCESS;
}

td_s32 vdec_hal_get_status(td_s32 chan_id, ext_vdec_status *status)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_status ioctl_status = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    ioctl_status.chan_id = chan_id;
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_GET_STATUS, &ioctl_status);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }
    (td_void)memcpy_s(status, sizeof(ext_vdec_status), &ioctl_status.status, sizeof(ext_vdec_status));

    return EXT_SUCCESS;
}

td_s32 vdec_hal_is_std_support(ext_vdec_opt_is_std_support *support)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_std_support ioctl_std_support = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    ioctl_std_support.support_info.user_dec = support->user_dec;
    ioctl_std_support.support_info.standard = support->standard;
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_IS_STD_SUPPORT, &ioctl_std_support);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }
    support->support = ioctl_std_support.support_info.support;

    return EXT_SUCCESS;
}

td_s32 vdec_hal_get_cap(ext_vdec_capability *cap)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_capability ioctl_cap = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    ioctl_cap.cap.standard = cap->standard;
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_GET_CAP, &ioctl_cap);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }
    (td_void)memcpy_s(cap, sizeof(ext_vdec_capability), &ioctl_cap.cap, sizeof(ext_vdec_capability));

    return EXT_SUCCESS;
}

td_s32 vdec_hal_send_eos(td_s32 chan_id)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_send_eos ioctl_eos = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    ioctl_eos.chan_id = chan_id;
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_SEND_EOS, &ioctl_eos);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_hal_bind_frm_buffer(td_s32 chan_id, const ext_vdec_frame_buf *buffer)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_bind_frm_buf ioctl_param = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    ioctl_param.chan_id = chan_id;
    (td_void)memcpy_s(&ioctl_param.buffer, sizeof(ext_vdec_frame_buf), buffer, sizeof(ext_vdec_frame_buf));
    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_BIND_FRM_BUF, &ioctl_param);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_hal_command(td_s32 chan_id, ext_vdec_cmd_id cmd_id, td_void *param, td_u32 param_size)
{
    td_s32 ret;
    vdec_hal_entry *entry = vdec_hal_get_entry();
    vdec_ioctl_command ioctl_param = {0};

    if (entry->dev_fd == (td_s32)VDEC_INVALID_HANDLE) {
        vdec_print_alert("hal dev_fd is invalid!\n");
        return EXT_FAILURE;
    }

    if (param != TD_NULL && (param_size == 0 || param_size > VDEC_IOCTL_BUF_SIZE)) {
        vdec_print_alert("hal param is invalid!\n");
        return EXT_FAILURE;
    }

    ioctl_param.cmd_id = cmd_id;
    ioctl_param.chan_id = chan_id;
    ioctl_param.param_size = (param == TD_NULL) ? 0 : param_size;
    if (param != TD_NULL) {
        (td_void)memcpy_s(ioctl_param.param, VDEC_IOCTL_BUF_SIZE, param, param_size);
    }

    ret = vdec_ioctl(entry->dev_fd, UMAPC_VDEC_COMMAND, &ioctl_param);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    if (param != TD_NULL) {
        (td_void)memcpy_s(param, param_size, ioctl_param.param, ioctl_param.param_size);
    }

    return EXT_SUCCESS;
}

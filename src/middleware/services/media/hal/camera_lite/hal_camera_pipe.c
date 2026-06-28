/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: camera hal adapter
 */

#include "securec.h"
#include "drv_venc_intf.h"
#include "mpi_viu_ext.h"
#include "soc_venc_api.h"
#include "hal_camera_common.h"
#include "hal_camera_config.h"
#include "hal_camera_pipe.h"

static td_handle g_venc_handle = EXT_VENC_INVALID_HANDLE;
static bool g_is_stream_on = false;
static ext_dvp_sns_obj *g_sensor = NULL;
static const ext_viu_dev_attr g_viu_dev_attr = {
    .intf_mode = EXT_VIU_MODE_DIGITAL_CAMERA,
    .work_mode = EXT_VIU_WORK_MODE_1Multiplex,
    .component_mask = {0xfff00000, 0x00000000},
    .scan_mode = EXT_VIU_SCAN_PROGRESSIVE,
    .data_seq = EXT_VIU_DATA_SEQ_YUYV,
    .syn_cfg.vsync = EXT_VIU_VSYNC_PULSE,
    .syn_cfg.hsync = EXT_VIU_HSYNC_PULSE,
    .syn_cfg.vsync_valid = EXT_VIU_VSYNC_NORM_PULSE, // may be no use
    .syn_cfg.vsync_valid_neg = EXT_VIU_VSYNC_VALID_NEG_LOW, // may be no use
    .syn_cfg.vsync_neg = EXT_VIU_VSYNC_NEG_HIGH,
    .syn_cfg.hsync_neg = EXT_VIU_HSYNC_NEG_HIGH,
    .input_data_type = EXT_VIU_DATA_TYPE_YUV,
    .data_reverse = TD_FALSE,
    .size.width = CAM_CFG_OUT_WIDTH,
    .size.height = CAM_CFG_OUT_HEIGHT,
    .data_rate = EXT_ISP_DATA_RATE_X1,
};

static const ext_viu_pipe_attr g_viu_pipe_attr = {
    .pipe_bypass_mode = EXT_VIU_PIPE_BYPASS_BE,
    .isp_bypass = TD_TRUE,
    .compress_mode = EXT_COMPRESS_MODE_NONE,
    .frame_rate.src_frame_rate = -1,
    .frame_rate.dst_frame_rate = -1,
    .pix_fmt = EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .bit_width = EXT_BIT_DEPTH_8,
    .max_w = CAM_CFG_OUT_WIDTH,
    .max_h = CAM_CFG_OUT_HEIGHT,
    .yuv_skip = TD_TRUE,
    .nr_en = TD_FALSE,
};

static const ext_venc_attr g_venc_attr = {
    .chn_attr = {
        .standard = EXT_VENC_STD_JPEG,
        .pic_width = CAM_CFG_OUT_WIDTH,
        .pic_height = CAM_CFG_OUT_HEIGHT,
        .max_pic_width = CAM_CFG_OUT_WIDTH,
        .max_pic_height = CAM_CFG_OUT_HEIGHT,
        .buf_size = CAM_CFG_JPEG_BUF_MAX,
        .profile = 0,
    },
    .gop_attr = {
        .gop_mode = EXT_VENC_GOP_MODE_I,
    },
    .rc_attr = {
        .standard = EXT_VENC_STD_JPEG,
        .rc_mode = EXT_VENC_RC_MODE_FIXQP,
        .src_frame_rate = 0,
        .dst_frame_rate = 0,
        .gop = 0,
        .fixqp_attr = {
            .qfactor = 25, // 25 fixed value
        },
    },
};

static void isp_sensor_init(void)
{
    if (g_sensor == NULL) {
        g_sensor = CAM_CFG_GET_SENSOR();
        ERR_IF(g_sensor == NULL, return, "get sensor function error");
    }
    ERR_IF(g_sensor->pfn_cmos_sensor_init == NULL, return, "sensor init not support");
    g_sensor->pfn_cmos_sensor_init();
}

static void isp_sensor_deinit(void)
{
    ERR_IF(g_sensor == NULL, return, "sensor not init");
    ERR_IF(g_sensor->pfn_cmos_sensor_exit == NULL, return, "sensor init not support");
    g_sensor->pfn_cmos_sensor_exit();
}

int cam_pipe_init(void)
{
    IF_FUNC_ERR(uapi_venc_module_init(), return -1, "venc init err");
    IF_FUNC_ERR(ext_mpi_viu_init(), goto err_venc, "viu init err");
    return 0;

err_venc:
    uapi_venc_module_exit();
    return -1;
}

int cam_pipe_deinit(void)
{
    LOG_FUNC_ERR(ext_mpi_viu_deinit(), "viu deinit err");
    uapi_venc_module_exit();
    return 0;
}

int cam_pipe_create(uint32_t cam_id)
{
    ext_venc_opt_create venc_opt = {};
    td_handle venc;

    UNUSED(cam_id);
    isp_sensor_init();

    LOG_FUNC_ERR(ext_mpi_viu_set_dev_attr(CAM_CFG_VI_DEV_ID, &g_viu_dev_attr), "set dev attr err");
    IF_FUNC_ERR(ext_mpi_viu_enable_dev(CAM_CFG_VI_DEV_ID), goto err1, "dev enable err");

    ext_viu_dev_timing_attr timing_attr = {TD_FALSE, CAM_CFG_CAM_FPS};
    IF_FUNC_ERR(ext_mpi_viu_set_dev_timing_attr(CAM_CFG_VI_DEV_ID, &timing_attr), goto err2, "set dev timing err");
    IF_FUNC_ERR(ext_mpi_viu_dev_attach(CAM_CFG_VI_DEV_ID, CAM_CFG_VI_PIPE_ID), goto err2, "vi dev attach err");

    IF_FUNC_ERR(ext_mpi_viu_create_pipe(CAM_CFG_VI_PIPE_ID, &g_viu_pipe_attr), goto err3, "create pipe err");

    ext_viu_dump_attr dump_attr = {TD_TRUE, 1, EXT_VIU_DUMP_TYPE_YUV};
    IF_FUNC_ERR(ext_mpi_viu_set_pipe_dump_attr(CAM_CFG_VI_PIPE_ID, &dump_attr), goto err4, "set pipe dump attr err");

    venc_opt.mode = EXT_VENC_WORK_AUTO;
    venc_opt.callback.src_handle = CAM_CFG_VI_PIPE_ID;
    IF_FUNC_ERR(uapi_venc_create(&venc, &venc_opt), goto err4, "venc create err");
    IF_FUNC_ERR(uapi_venc_set_attr(venc, &g_venc_attr), goto err5, "venc set attr err");
    LOG_FUNC_ERR(ext_mpi_viu_pipe_attach(CAM_CFG_VI_PIPE_ID, 0, SOC_ID_VENC, 0), "viu pipe attach err");
    g_venc_handle = venc;
    return 0;

err5:
    LOG_FUNC_ERR(uapi_venc_destroy(venc), "venc destroy err");
err4:
    LOG_FUNC_ERR(ext_mpi_viu_destroy_pipe(CAM_CFG_VI_PIPE_ID), "destroy pipe err");
err3:
    LOG_FUNC_ERR(ext_mpi_viu_dev_detach(CAM_CFG_VI_DEV_ID, CAM_CFG_VI_PIPE_ID), "vi dev detach err");
err2:
    LOG_FUNC_ERR(ext_mpi_viu_disable_dev(CAM_CFG_VI_DEV_ID), "dev disable err");
err1:
    isp_sensor_deinit();
    return -1;
}

int cam_pipe_destroy(uint32_t cam_id)
{
    UNUSED(cam_id);
    LOG_FUNC_ERR(ext_mpi_viu_pipe_detach(CAM_CFG_VI_PIPE_ID, 0, SOC_ID_VENC, 0), "viu pipe detach err");
    if (g_venc_handle != EXT_VENC_INVALID_HANDLE) {
        LOG_FUNC_ERR(uapi_venc_destroy(g_venc_handle), "venc destroy err");
        g_venc_handle = EXT_VENC_INVALID_HANDLE;
    }
    LOG_FUNC_ERR(ext_mpi_viu_destroy_pipe(CAM_CFG_VI_PIPE_ID), "destroy pipe err");
    LOG_FUNC_ERR(ext_mpi_viu_dev_detach(CAM_CFG_VI_DEV_ID, CAM_CFG_VI_PIPE_ID), "vi dev detach err");
    LOG_FUNC_ERR(ext_mpi_viu_disable_dev(CAM_CFG_VI_DEV_ID), "dev disable err");
    isp_sensor_deinit();
    return 0;
}

int cam_pipe_stream_on(uint32_t cam_id)
{
    UNUSED(cam_id);
    ERR_IF(g_venc_handle == EXT_VENC_INVALID_HANDLE, return -1, "stream not create");

    IF_FUNC_ERR(ext_mpi_viu_start_pipe(CAM_CFG_VI_PIPE_ID), return -1, "start pipe err");

    ext_venc_opt_start start_option = {.recv_pic_num = -1};
    IF_FUNC_ERR(uapi_venc_start(g_venc_handle, &start_option), goto err_pipe, "venc start err");
    g_is_stream_on = true;
    return 0;

err_pipe:
     LOG_FUNC_ERR(ext_mpi_viu_stop_pipe(CAM_CFG_VI_PIPE_ID), "stop pipe err");
    return -1;
}

int cam_pipe_stream_off(uint32_t cam_id)
{
    UNUSED(cam_id);
    g_is_stream_on = false;
    if (g_venc_handle != EXT_VENC_INVALID_HANDLE) {
        LOG_FUNC_ERR(uapi_venc_stop(g_venc_handle), "venc stop err");
    }
    LOG_FUNC_ERR(ext_mpi_viu_stop_pipe(CAM_CFG_VI_PIPE_ID), "stop pipe err");
    return 0;
}

int cam_pipe_get_stream(uint32_t cam_id, ext_venc_stream *buffer, int32_t timeout_ms)
{
    ERR_IF(g_venc_handle == EXT_VENC_INVALID_HANDLE, return -1, "stream not create");
    ERR_IF(!g_is_stream_on, return -1, "stream not on");
    ext_venc_opt_get_stream venc_opt = {};
    venc_opt.time_out = timeout_ms;
    CAM_TRACE_BEGIN("VENC-DQBUF");
    td_s32 ret = uapi_venc_get_stream(g_venc_handle, buffer, &venc_opt);
    CAM_TRACE_END();
    if (ret != 0) {
        CAM_ERR("venc get stream error! ret: %d\n", ret);
        ext_venc_status status = {};
        LOG_FUNC_ERR(uapi_venc_get_status(g_venc_handle, &status), "venc get status err");
        CAM_ERR("status_get_frame_cnt: %u\n", status.status_get_frame_cnt);
        CAM_ERR("status_free_frame_cnt: %u\n", status.status_free_frame_cnt);
        CAM_ERR("status_slice_int_cnt: %u\n", status.status_slice_int_cnt);
        CAM_ERR("frame_wait_encode_num: %u\n", status.frame_wait_encode_num);
        CAM_ERR("frame_left_receive_num: %u\n", status.frame_left_receive_num);
        CAM_ERR("frame_left_encode_num: %u\n", status.frame_left_encode_num);
        CAM_ERR("stream_wait_receive_packs: %u\n", status.stream_wait_receive_packs);
        return -1;
    }
    return 0;
}

int cam_pipe_put_stream(uint32_t cam_id, ext_venc_stream *buffer)
{
    ERR_IF(g_venc_handle == EXT_VENC_INVALID_HANDLE, return -1, "stream not create");
    ERR_IF(!g_is_stream_on, return -1, "stream not on");
    CAM_TRACE_BEGIN("VENC-QBUF");
    IF_FUNC_ERR(uapi_venc_put_stream(g_venc_handle, buffer), goto err, "venc put stream err");
    CAM_TRACE_END();
    return 0;

err:
    CAM_TRACE_END();
    return -1;
}

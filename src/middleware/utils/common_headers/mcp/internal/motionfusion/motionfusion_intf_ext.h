/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description : motionfusion_intf_ext.h , init draft
 * Author : ISP SW
 * Create : 2018-12-22
 */
#ifndef MOTIONFUSION_INTF_EXT_H
#define MOTIONFUSION_INTF_EXT_H

#include "ot_common_motionfusion.h"
#include "osal_ioctl.h"
#include "drv_mcp_log.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define IOC_TYPE_MFUSION  11

/* debug print format  */
#define mfusion_emerg_trace(fmt, ...) \
    mcp_alert_fatal(HI_ID_MOTIONFUSION, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define mfusion_alert_trace(fmt, ...) \
    mcp_alert_fatal(HI_ID_MOTIONFUSION, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define mfusion_crit_trace(fmt, ...) \
    mcp_err_trace(HI_ID_MOTIONFUSION, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define mfusion_err_trace(fmt, ...) \
    mcp_err_trace(HI_ID_MOTIONFUSION, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define mfusion_warn_trace(fmt, ...) \
    mcp_warn_trace(HI_ID_MOTIONFUSION, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define mfusion_notice_trace(fmt, ...) \
    mcp_info_trace(HI_ID_MOTIONFUSION, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define mfusion_info_trace(fmt, ...) \
    mcp_info_trace(HI_ID_MOTIONFUSION, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define mfusion_debug_trace(fmt, ...) \
    mcp_debug_trace(HI_ID_MOTIONFUSION, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

typedef struct {
    td_s32 sample_rate;
    td_s32 gyro_data_range; /* Range of gyro data, unit is dps. i.e. for 59DV BMI 160, range is 250dps */
    /*
     * Data bit width used for full range. i.e. for 59DV BMI 160,
     * 15bits is used to represent 250 dps -> (1<<15) = 250dps
     */
    td_u8 gyro_data_precbits;
    td_s32 temp_data_min; /* Min of Mag data */
    td_s32 temp_data_max; /* Max of Mag data */
    td_u8 bit_width;      /* Data bit width used for full range. */
} mfusion_dev_gyro_cfg;

typedef struct {
    td_s32 sample_rate;
    td_s32 acc_data_range;   /* Range of Acc data */
    td_u8 acc_data_precbits; /* Data bit width used for full range. */

    td_s32 temp_data_min; /* Min of Mag data */
    td_s32 temp_data_max; /* Max of Mag data */
    td_u8 bit_width;      /* Data bit width used for full range. */
} mfusion_dev_acc_cfg;

typedef struct {
    td_s32 sample_rate;
    td_s32 magn_data_range;   /* Range of Acc data,  */
    td_u8 magn_data_precbits; /* Data bit width used for full range. */

    td_s32 temp_data_min; /* Min of Mag data */
    td_s32 temp_data_max; /* Max of Mag data */
    td_u8 bit_width;
} mfusion_dev_magn_cfg;

typedef union {
    mfusion_dev_gyro_cfg gyro_cfg;
    mfusion_dev_acc_cfg acc_cfg;
    mfusion_dev_magn_cfg magn_cfg;
} mfusion_dev_cfg;

typedef struct {
    td_s32 roll;  /* RW; Range: [-18000, 18000]; Roll angle. with 2 decimal numbers */
    td_s32 pitch; /* RW; Range: [-18000, 18000]; Pitch angle. with 2 decimal numbers */
    td_s32 yaw;   /* RW; Range: [-18000, 18000]; Yaw angle. with 2 decimal numbers */
} mfusion_euler;

typedef enum {
    MFUSION_MODE_STABLE = 0, /* Stable mode. */
    MFUSION_MODE_RECTIFY,    /* Rectify mode. */
    MFUSION_MODE_CAMSTEADY,  /* Camera Steady mode. */
    MFUSION_MODE_BUTT
} mfusion_mode;

typedef enum {
    MFUSION_USECASE_NORMAL = 0, /* Normal mode. no stitching */
    MFUSION_USECASE_STITCH,     /* Stitching mode. */
    MFUSION_USECASE_BUTT
} mfusion_usecase;

typedef struct {
    td_bool en;
    td_s32 matrix[OT_MFUSION_MATRIX_NUM];
    td_u32 fusion_id;
} mfusion_gyro_sixside_cal;

typedef td_s32 fn_mfusion_init(const td_u32 user_id);
typedef td_s32 fn_mfusion_deinit(const td_u32 user_id);
typedef td_s32 fn_mfusion_set_usecase(const td_u32 fusion_id, const mfusion_usecase usecase);
typedef td_s32 fn_mfusion_get_usecase(const td_u32 fusion_id, mfusion_usecase *usecase);
typedef td_s32 fn_mfusion_set_mode(const td_u32 fusion_id, const mfusion_mode mode);
typedef td_s32 fn_mfusion_get_mode(const td_u32 fusion_id, mfusion_mode *mode);

#ifdef GET_FUSION_ANGLE_SWITCH
typedef td_s32 fn_mfusion_get_fusion_angle(const td_u32 fusion_id,
                                           const td_u64 begin_pts,
                                           const td_u64 end_pts,
                                           mfusion_euler *euler_buf);
#endif

typedef td_s32 fn_mfusion_get_gyro_samples(const td_u32 user_id,
                                           const td_u64 begin_pts,
                                           const td_u64 end_pts,
                                           ot_mfusion_gyro_buf *gyro_buffer,
                                           ot_mfusion_acc_buf *acc_buffer,
                                           td_bool rotation_compensation_en,
                                           td_bool *is_acc_data_valid);
typedef td_s32 fn_mfusion_get_dev_config(const td_u32 fusion_id,
                                         const td_u32 mf_dev,
                                         mfusion_dev_cfg *dev_cfg);
typedef td_s32 fn_mfusion_get_quaternion(const td_u32 fusion_id, const td_u64 begin_pts, const td_u64 end_pts,
    ot_mfusion_quaternion_buf *buf);
typedef td_s32 fn_mfusion_get_rotation_compensation(const td_u32 fusion_id, td_u32 user_id,
    ot_mfusion_rotation_compensation *compensation, td_bool is_inner_only);
typedef td_s32 fn_mfusion_get_position_data(const td_u32 fusion_id, const td_u64 begin_pts, const td_u64 end_pts,
    ot_mfusion_position_data *position_data);

typedef struct {
    fn_mfusion_init *init;
    fn_mfusion_deinit *deinit;
    fn_mfusion_set_usecase *set_usecase;
    fn_mfusion_get_usecase *get_usecase;
    fn_mfusion_set_mode *set_mode;
    fn_mfusion_get_mode *get_mode;
#ifdef GET_FUSION_ANGLE_SWITCH
    fn_mfusion_get_fusion_angle *get_fusion_angle;
#endif
    fn_mfusion_get_gyro_samples *get_gyro_samples;
    fn_mfusion_get_dev_config *get_dev_config;
    fn_mfusion_get_quaternion *get_quaternion;
    fn_mfusion_get_rotation_compensation *get_rotation_compensation;
    fn_mfusion_get_position_data *get_position_data;
} mfusion_export_func;

typedef enum {
    IOC_NR_MOTIONFUSION_GET_ATT = 0,
    IOC_NR_MOTIONFUSION_SET_ATT,

    IOC_NR_MOTIONFUSION_GET_GYRODRIFT,
    IOC_NR_MOTIONFUSION_SET_GYRODRIFT,
    IOC_NR_MOTIONFUSION_GET_GYROSIXSIDECAL,
    IOC_NR_MOTIONFUSION_SET_GYROSIXSIDECAL,
    IOC_NR_MOTIONFUSION_GET_GYROTEMPDRIFT,
    IOC_NR_MOTIONFUSION_SET_GYROTEMPDRIFT,
    IOC_NR_MOTIONFUSION_GET_GYROONLINETEMPDRIFT,
    IOC_NR_MOTIONFUSION_SET_GYROONLINETEMPDRIFT,
    IOC_NR_MOTIONFUSION_GET_GYROONLINEDRIFT,
    IOC_NR_MOTIONFUSION_SET_GYROONLINEDRIFT,

    IOC_NR_MOTIONFUSION_GET_ACCDRIFT,
    IOC_NR_MOTIONFUSION_SET_ACCDRIFT,
    IOC_NR_MOTIONFUSION_GET_ACCSIXSIDECAL,
    IOC_NR_MOTIONFUSION_SET_ACCSIXSIDECAL,
#ifdef GET_FUSION_ANGLE_SWITCH
    IOC_NR_MOTIONFUSION_GET_ACCTEMPDRIFT,
    IOC_NR_MOTIONFUSION_SET_ACCTEMPDRIFT,
#endif

    IOC_NR_MOTIONFUSION_GET_SAMPLEDATA,

#ifdef GET_FUSION_ANGLE_SWITCH
    IOC_NR_MOTIONFUSION_GET_FUSIONDATA,
    IOC_NR_MOTIONFUSION_GET_MODE,
    IOC_NR_MOTIONFUSION_SET_MODE,
    IOC_NR_MOTIONFUSION_GET_USECASE,
    IOC_NR_MOTIONFUSION_SET_USECASE,
#endif
    IOC_NR_MOTIONFUSION_INIT_ROTATION,
    IOC_NR_MOTIONFUSION_DEINIT_ROTATION,

    IOC_NR_MOTIONFUSION_GET_ROTATION,
    IOC_NR_MOTIONFUSION_SET_ROTATION,

    IOC_NR_MOTIONFUSION_GET_QUATERNION,
    IOC_NR_MOTIONFUSION_SET_QUATERNION,

    IOC_NR_MOTIONFUSION_GET_ROTATION_DATA,
    IOC_NR_MOTIONFUSION_SET_ROTATION_DATA,

    IOC_NR_MOTIONFUSION_BUTT
} ioc_nr_motionfusion;

typedef struct {
    td_bool en;
    td_s32 drift[OT_MFUSION_AXIS_NUM];
    td_u32 fusion_id;
} mfusion_gyro_drift;

typedef struct {
    td_bool en;
    ot_mfusion_temperature_drift drift;
    td_u32 fusion_id;
} mfusion_gyro_temp_drift;

typedef struct {
    td_bool en;
    td_s32 matrix[OT_MFUSION_MATRIX_NUM];
    td_u32 fusion_id;
} mfusion_acc_sixside_cal;

typedef struct {
    td_bool en;
    td_s32 drift[OT_MFUSION_AXIS_NUM];
    td_u32 fusion_id;
} mfusion_acc_drift;

typedef struct {
    td_bool en;
    ot_mfusion_temperature_drift drift;
    td_u32 fusion_id;
} mfusion_acc_temp_drift;

typedef struct {
    td_u64 begin_pts;
    td_u64 end_pts;
    ot_mfusion_gyro_buf *gyro_buffer;
    td_u32 user_id;
} mfusion_gyro_sample_info;

typedef struct {
    td_u64 begin_pts;
    td_u64 end_pts;
    mfusion_euler euler;
    td_u32 fusion_id;
} mfusion_gyro_fusion_info;

typedef struct {
    ot_mfusion_attr attr;
    td_u32 fusion_id;
} mfusion_ctl_attr;

typedef struct {
    mfusion_mode mode;
    td_u32 fusion_id;
} mfusion_mode_info;

typedef struct himfusion_ctrl_compensation_info {
    ot_mfusion_rotation_compensation compensation;
    td_u32 fusion_id;
} mfusion_ctrl_compensation_info;

typedef struct himfusion_ctrl_rotation_info {
    ot_mfusion_rotation_cfg cfg;
    td_u32 fusion_id;
} mfusion_ctrl_rotation_info;

typedef struct himfusion_ctrl_quaternion_buf_info {
    ot_mfusion_quaternion_buf quaternion_buf;
    td_u32 fusion_id;
    td_u64 begin_pts;
    td_u64 end_pts;
} mfusion_ctrl_quaternion_buf_info;

typedef struct himfusion_ctrl_position_data_info {
    ot_mfusion_position_data position_data;
    td_u32 fusion_id;
    td_u64 begin_pts;
    td_u64 end_pts;
} mfusion_ctrl_position_data_info;

typedef struct himfusion_usecase_info {
    mfusion_usecase usecase;
    td_u32 fusion_id;
} mfusion_usecase_info;

#define MOTIONFUSION_GET_ATT_CTRL _IOWR(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_GET_ATT, \
    mfusion_ctl_attr)
#define MOTIONFUSION_SET_ATT_CTRL _IOW(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_SET_ATT, \
    mfusion_ctl_attr)
#define MOTIONFUSION_GET_GYRODRIFT_CTRL _IOWR(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_GET_GYRODRIFT, \
    mfusion_gyro_drift)
#define MOTIONFUSION_SET_GYRODRIFT_CTRL _IOW(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_SET_GYRODRIFT, \
    mfusion_gyro_drift)
#define MOTIONFUSION_GET_GYROSIXSIDECAL_CTRL _IOWR(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_GET_GYROSIXSIDECAL, \
    mfusion_gyro_sixside_cal)
#define MOTIONFUSION_SET_GYROSIXSIDECAL_CTRL _IOW(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_SET_GYROSIXSIDECAL, \
    mfusion_gyro_sixside_cal)
#define MOTIONFUSION_GET_GYROTEMPDRIFT_CTRL _IOWR(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_GET_GYROTEMPDRIFT, \
    mfusion_gyro_temp_drift)
#define MOTIONFUSION_SET_GYROTEMPDRIFT_CTRL _IOW(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_SET_GYROTEMPDRIFT, \
    mfusion_gyro_temp_drift)

#define MOTIONFUSION_GET_GYROONLINETEMPDRIFT_CTRL _IOWR(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_GET_GYROONLINETEMPDRIFT, \
    mfusion_gyro_temp_drift)
#define MOTIONFUSION_SET_GYROONLINETEMPDRIFT_CTRL _IOW(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_SET_GYROONLINETEMPDRIFT, \
    mfusion_gyro_temp_drift)
#define MOTIONFUSION_GET_GYROONLINEDRIFT_CTRL _IOWR(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_GET_GYROONLINEDRIFT, \
    mfusion_gyro_drift)
#define MOTIONFUSION_SET_GYROONLINEDRIFT_CTRL _IOW(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_SET_GYROONLINEDRIFT, \
    mfusion_gyro_drift)

#define MOTIONFUSION_GET_ACCDRIFT_CTRL _IOWR(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_GET_ACCDRIFT, \
    mfusion_acc_drift)
#define MOTIONFUSION_SET_ACCDRIFT_CTRL _IOW(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_SET_ACCDRIFT, \
    mfusion_acc_drift)
#define MOTIONFUSION_GET_ACCSIXSIDECAL_CTRL _IOWR(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_GET_ACCSIXSIDECAL, \
    mfusion_acc_sixside_cal)
#define MOTIONFUSION_SET_ACCSIXSIDECAL_CTRL _IOW(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_SET_ACCSIXSIDECAL, \
    mfusion_acc_sixside_cal)

#ifdef GET_FUSION_ANGLE_SWITCH
#define MOTIONFUSION_GET_ACCTEMPDRIFT_CTRL _IOWR(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_GET_ACCTEMPDRIFT, \
    mfusion_acc_temp_drift)
#define MOTIONFUSION_SET_ACCTEMPDRIFT_CTRL _IOW(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_SET_ACCTEMPDRIFT, \
    mfusion_acc_temp_drift)
#endif

#define MOTIONFUSION_GET_SAMPLEDATA _IOW(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_GET_SAMPLEDATA, \
    mfusion_acc_temp_drift)

#ifdef GET_FUSION_ANGLE_SWITCH
#define MOTIONFUSION_GET_FUSIONDATA _IOW(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_GET_FUSIONDATA, \
    mfusion_acc_temp_drift)
#define MOTIONFUSION_SET_MODE _IOW(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_SET_MODE, \
    mfusion_mode_info)
#define MOTIONFUSION_GET_MODE _IOWR(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_GET_MODE, \
    mfusion_mode_info)
#define MOTIONFUSION_SET_USECASE _IOW(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_SET_USECASE, \
    mfusion_usecase_info)
#define MOTIONFUSION_GET_USECASE _IOWR(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_GET_USECASE, \
    mfusion_usecase_info)
#endif

#define MOTIONFUSION_INIT_ROTATION _IOW(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_INIT_ROTATION, \
    mfusion_ctrl_rotation_info)
#define MOTIONFUSION_DEINIT_ROTATION _IOW(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_DEINIT_ROTATION, \
    mfusion_ctrl_rotation_info)
#define MOTIONFUSION_GET_ROTATION _IOWR(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_GET_ROTATION, \
   mfusion_ctrl_compensation_info)
#define MOTIONFUSION_SET_ROTATION _IOW(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_SET_ROTATION, \
    mfusion_ctrl_compensation_info)
#define MOTIONFUSION_GET_QUATERNION _IOWR(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_GET_QUATERNION, \
   mfusion_ctrl_quaternion_buf_info)
#define MOTIONFUSION_SET_QUATERNION _IOW(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_SET_QUATERNION, \
   mfusion_ctrl_quaternion_buf_info)
#define MOTIONFUSION_GET_ROTATION_DATA _IOWR(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_GET_ROTATION_DATA, \
   mfusion_ctrl_position_data_info)
#define MOTIONFUSION_SET_ROTATION_DATA _IOW(IOC_TYPE_MFUSION, IOC_NR_MOTIONFUSION_SET_ROTATION_DATA, \
   mfusion_ctrl_position_data_info)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef MOTIONFUSION_INTF_EXT_H */


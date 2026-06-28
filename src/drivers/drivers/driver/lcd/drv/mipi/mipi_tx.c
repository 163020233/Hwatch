/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: mipi tx file.
 *
 * Create: 2022-04-14
 */

#include "securec.h"
#include "common_def.h"
#include "soc_osal.h"
#include "cmsis_os2.h"
#include "mipi_tx_hal.h"
#include "soc_mipi_tx.h"
#include "soc_errno.h"
#include "mipi_tx_dfx.h"
#include "mipi_tx.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DISABLE_CONFIG 0

typedef struct {
    uint32_t (*write) (uint32_t s);
    uint32_t (*read) (uint32_t s);
} mipi_tx_config;

typedef struct {
    osal_mutex mipi_tx_mux;
    mipi_tx_config cfg_ctrl;
    bool is_inited;
    bool en_dev;
    bool en_dev_cfg;
} mipi_tx_driver_attr;

mipi_tx_driver_attr g_mipi_tx_attr = {0};
combo_dev_cfg_t g_mipi_tx_dev_cfg = {0};

PLT_SEC_ITCM_T static INLINE uint32_t mipi_tx_init_status_check(void)
{
    if (g_mipi_tx_attr.is_inited == TD_FALSE) {
        mipi_tx_err("mipi_tx not init!\r\n");
        return EXT_ERR_MIPI_NOT_INIT;
    }

    return EXT_ERR_SUCCESS;
}

PLT_SEC_ITCM_T static INLINE uint32_t mipi_tx_dev_cfg_check(void)
{
    if (g_mipi_tx_attr.is_inited == TD_FALSE) {
        mipi_tx_err("mipi_tx not init!\r\n");
        return EXT_ERR_MIPI_NOT_INIT;
    }

    if (!g_mipi_tx_attr.en_dev_cfg) {
        mipi_tx_err("mipi_tx dev has not configed!\r\n");
        return EXT_ERR_MIPI_DEV_CFG_FAILED;
    }

    return EXT_ERR_SUCCESS;
}

static INLINE uint32_t mipi_tx_enable_check(void)
{
    uint32_t ret;

    ret = mipi_tx_dev_cfg_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    if (!g_mipi_tx_attr.en_dev) {
        mipi_tx_err("mipi_tx has not enabled!\r\n");
        return EXT_ERR_MIPI_DEV_NOT_ENABLE;
    }
    return EXT_ERR_SUCCESS;
}
PLT_SEC_ITCM_T static INLINE uint32_t mipi_tx_mutex_init(void)
{
    uint32_t ret;
    if (g_mipi_tx_attr.mipi_tx_mux.mutex != NULL) {
        return EXT_ERR_SUCCESS;
    }

    ret = (uint32_t)osal_mutex_init(&(g_mipi_tx_attr.mipi_tx_mux));
    if (ret != EXT_ERR_SUCCESS) {
        g_mipi_tx_attr.mipi_tx_mux.mutex = NULL;
        return ret;
    }
    if (g_mipi_tx_attr.mipi_tx_mux.mutex == NULL) {
        return EXT_ERR_MIPI_CREATE_MUX_FAIL;
    }
    return EXT_ERR_SUCCESS;
}

static uint32_t mipi_tx_down_mutex(void)
{
    if (g_mipi_tx_attr.mipi_tx_mux.mutex == NULL) {
        return EXT_ERR_MIPI_CREATE_MUX_FAIL;
    }
    return (uint32_t)osal_mutex_lock(&(g_mipi_tx_attr.mipi_tx_mux));
}

static void mipi_tx_up_mutex(void)
{
    if (g_mipi_tx_attr.mipi_tx_mux.mutex == NULL) {
        return;
    }
    osal_mutex_unlock(&(g_mipi_tx_attr.mipi_tx_mux));
}

PLT_SEC_ITCM_T uint32_t mipi_tx_get_lane_num(const int16_t lane_id[], uint32_t lane_id_len)
{
    uint32_t lane_num = 0;
    uint32_t index;

    for (index = 0; index < lane_id_len; index++) {
        if (lane_id[index] != MIPI_TX_DISABLE_LANE_ID) {
            lane_num++;
        }
    }
    return lane_num;
}

PLT_SEC_ITCM_T static INLINE uint32_t mipi_tx_check_temp_lane_id(uint32_t lane_index, uint32_t valid_lane_num,
    const int16_t lane_id[])
{
    uint32_t index;
    int16_t temp_id;
    temp_id = lane_id[lane_index];

    /* lane id in {0, 1, 2, 3, -1} */
    if ((temp_id < MIPI_TX_DISABLE_LANE_ID) || (temp_id >= LANE_MAX_NUM)) {
        mipi_tx_err("lane_id[%u] is invalid value %d.\r\n", lane_index, temp_id);
        return EXT_ERR_MIPI_INVALID_PARAM;
    }

    if (temp_id == MIPI_TX_DISABLE_LANE_ID) {
        return EXT_ERR_SUCCESS;
    }

    /*
     * lanex_id and lane num:
     * 1 lane: lane0_id, lane1_id must be different from lane0_id
     */
    if ((uint32_t)temp_id > valid_lane_num) {
        mipi_tx_err("lane_id[%d]=%d is illegal, lane%d_id can't be set in %d lane mode\r\n", lane_index, temp_id,
            temp_id, valid_lane_num);
        return EXT_ERR_MIPI_INVALID_PARAM;
    }

    /* valid lane id must be different. */
    for (index = lane_index + 1; index < LANE_MAX_NUM; index++) {
        if (temp_id == lane_id[index]) {
            mipi_tx_err("lane_id[%u] can't be same value %d as lane_id[%u]\r\n", lane_index, temp_id, index);
            return EXT_ERR_MIPI_INVALID_PARAM;
        }
    }

    return EXT_ERR_SUCCESS;
}

PLT_SEC_ITCM_T static INLINE uint32_t mipi_tx_check_dev_cfg_lane_id(const combo_dev_cfg_t *dev_cfg)
{
    uint32_t index;
    uint32_t max_lane_num = LANE_MAX_NUM;
    uint32_t lane_num;

    lane_num = mipi_tx_get_lane_num(dev_cfg->lane_id, LANE_MAX_NUM);
    if (lane_num == 0) {
        mipi_tx_err("all lane_id is invalid!\r\n");
        return EXT_ERR_MIPI_INVALID_PARAM;
    }

    for (index = 0; index < max_lane_num; index++) {
        if (mipi_tx_check_temp_lane_id(index, lane_num, dev_cfg->lane_id) != EXT_ERR_SUCCESS) {
            return EXT_ERR_MIPI_INVALID_PARAM;
        }
    }

    return EXT_ERR_SUCCESS;
}

PLT_SEC_ITCM_T static INLINE uint32_t mipi_tx_check_dev_cfg_out_mode(const combo_dev_cfg_t *dev_cfg)
{
    uint32_t not_support = (dev_cfg->out_mode != OUT_MODE_DSI_VIDEO) && (dev_cfg->out_mode != OUT_MODE_DSI_CMD);
    if (not_support == 1) {
        mipi_tx_err("mipi_tx dev output_mode(%d) err!\r\n", dev_cfg->out_mode);
        return EXT_ERR_MIPI_INVALID_MODE;
    }
    return EXT_ERR_SUCCESS;
}

PLT_SEC_ITCM_T static INLINE uint32_t mipi_tx_check_dev_cfg_video_mode(const combo_dev_cfg_t *dev_cfg)
{
    uint32_t not_support = (dev_cfg->video_mode != BURST_MODE) && (dev_cfg->video_mode != NON_BURST_MODE_SYNC_PULSES) &&
        (dev_cfg->video_mode != NON_BURST_MODE_SYNC_EVENTS);
    if (not_support == 1) {
        mipi_tx_err("mipi_tx dev video_mode(%d) err!\r\n", dev_cfg->video_mode);
        return EXT_ERR_MIPI_INVALID_MODE;
    }
    return EXT_ERR_SUCCESS;
}

PLT_SEC_ITCM_T static INLINE uint32_t mipi_tx_check_dev_cfg_out_format(const combo_dev_cfg_t *dev_cfg)
{
    uint32_t not_support =
        (dev_cfg->out_format != OUT_FORMAT_RGB_16BIT) &&
        (dev_cfg->out_format != OUT_FORMAT_RGB_18BIT) &&
        (dev_cfg->out_format != OUT_FORMAT_RGB_18BIT_LOOSELY) &&
        (dev_cfg->out_format != OUT_FORMAT_RGB_24BIT) &&
        (dev_cfg->out_format != OUT_FORMAT_YUV420_12BIT) &&
        (dev_cfg->out_format != OUT_FORMAT_YUV422_16BIT) &&
        (dev_cfg->out_format != OUT_FORMAT_RGB_30BIT);
    if (not_support == 1) {
        mipi_tx_err("mipi_tx dev format(%d) err!\r\n", dev_cfg->out_format);
        return EXT_ERR_MIPI_INVALID_FORMAT;
    }
    return EXT_ERR_SUCCESS;
}

PLT_SEC_ITCM_T static INLINE uint32_t mipi_tx_check_dev_cfg_pck_en(const combo_dev_cfg_t *dev_cfg)
{
    uint32_t not_support = (dev_cfg->pck_en.eotp_tx_en != 0) && (dev_cfg->pck_en.eotp_tx_en != 1);
    if (not_support == 1) {
        mipi_tx_err("mipi_tx dev eotp_tx_en(%u) err!\r\n", dev_cfg->pck_en.eotp_tx_en);
        return EXT_ERR_MIPI_DEV_CFG_FAILED;
    }

    not_support = (dev_cfg->pck_en.eotp_rx_en != 0) && (dev_cfg->pck_en.eotp_rx_en != 1);
    if (not_support == 1) {
        mipi_tx_err("mipi_tx dev eotp_rx_en(%u) err!\r\n", dev_cfg->pck_en.eotp_rx_en);
        return EXT_ERR_MIPI_DEV_CFG_FAILED;
    }
    return EXT_ERR_SUCCESS;
}

PLT_SEC_ITCM_T static INLINE uint32_t mipi_tx_check_comb_dev_cfg(const combo_dev_cfg_t *dev_cfg)
{
    uint32_t ret;

    if (dev_cfg->devno != 0) {
        mipi_tx_err("mipi_tx dev devno err!\r\n");
        return EXT_ERR_MIPI_DEV_CFG_FAILED;
    }

    ret = mipi_tx_check_dev_cfg_pck_en(dev_cfg);
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    ret = mipi_tx_check_dev_cfg_lane_id(dev_cfg);
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    ret = mipi_tx_check_dev_cfg_out_mode(dev_cfg);
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    ret = mipi_tx_check_dev_cfg_video_mode(dev_cfg);
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    return mipi_tx_check_dev_cfg_out_format(dev_cfg);
}

PLT_SEC_ITCM_T static INLINE uint32_t mipi_tx_set_combo_dev_cfg(const combo_dev_cfg_t *dev_cfg)
{
    if (dev_cfg == NULL) {
        mipi_tx_err("NULL pointer\r\n");
        return EXT_ERR_MIPI_INVALID_PARAM;
    }

    uint32_t ret = mipi_tx_check_comb_dev_cfg(dev_cfg);
    if (ret != EXT_ERR_SUCCESS) {
        mipi_tx_err("mipi_tx check combo_dev config failed!\r\n");
        return ret;
    }

    /* set controler config */
    mipi_tx_drv_set_controller_cfg(dev_cfg);

    /* set phy config */
    mipi_tx_drv_set_phy_cfg(dev_cfg);

    g_mipi_tx_dev_cfg = *dev_cfg;
    g_mipi_tx_attr.en_dev_cfg = true;

    return ret;
}

static uint32_t mipi_tx_check_set_cmd_info(const cmd_info_t *cmd_info)
{
    if (cmd_info->devno != 0) {
        mipi_tx_err("mipi_tx devno(%u) illegal!\r\n", cmd_info->devno);
        return EXT_ERR_MIPI_INVALID_PARAM;
    }

    if ((cmd_info->work_mode != MIPI_TX_WORK_MODE_LP) &&
        (cmd_info->work_mode != MIPI_TX_WORK_MODE_HS)) {
        mipi_tx_err("mipi_tx work_mode(%d) is illegal!\r\n", cmd_info->work_mode);
        return EXT_ERR_MIPI_INVALID_MODE;
    }

    /* When cmd is not NULL,cmd_size means the length of cmd or it means cmd and addr */
    if (cmd_info->cmd != NULL) {
        if (cmd_info->cmd_size > MIPI_TX_SET_DATA_SIZE || cmd_info->cmd_size == 0) {
            mipi_tx_err("mipi_tx dev cmd_size(%u) is illegal!\r\n", cmd_info->cmd_size);
            return EXT_ERR_MIPI_INVALID_PARAM;
        }
    }

    return EXT_ERR_SUCCESS;
}

static td_bool mipi_tx_is_in_data_trans(void)
{
    uint32_t ulps_exit_cnt = mipi_tx_hal_get_ulps_exit_count();
    mipi_tx_dfx_info *dfx_info = mipi_tx_dfx_get_info();
    if (ulps_exit_cnt > dfx_info->exit_ulps_cnt_ext) {
        dfx_info->cmd_in_data_trans_cnt++;
#ifdef MIPI_TX_STA_DET_DFX
        if (mipi_tx_dfx_get_info()->cmd_in_data_trans_cnt <= MIPI_TX_STA_ERR_PRINT_MAX_CNT) {
            mipi_tx_err("ulps_exit_cnt %d exit_ulps_cnt_ext %#x\r\n", ulps_exit_cnt, dfx_info->exit_ulps_cnt_ext);
        }
#endif
        return TD_TRUE;
    }

    return TD_FALSE;
}

static uint32_t mipi_tx_set_cmd(const cmd_info_t *cmd_info)
{
    if (cmd_info == NULL) {
        mipi_tx_err("NULL pointer\r\n");
        return EXT_ERR_MIPI_INVALID_PARAM;
    }

    mipi_tx_down_mutex();

    uint32_t ret = mipi_tx_check_set_cmd_info(cmd_info);
    if (ret != EXT_ERR_SUCCESS) {
        mipi_tx_up_mutex();
        mipi_tx_err("mipi_tx check set cmd info failed!\r\n");
        return ret;
    }

    if (mipi_tx_is_in_data_trans() == TD_TRUE) {
        uint16_t cmd = (cmd_info->cmd == TD_NULL) ? (cmd_info->cmd_size) : cmd_info->cmd[0];
        mipi_tx_cmd_info dfx_cmd_info = {.cmd_type = MIPI_TX_CMD_TYPE_WRITE, .cmd = cmd};
        mipi_tx_dfx_record_cmd_in_data_trans(dfx_cmd_info);
#ifdef MIPI_TX_STA_DET_DFX
        if (mipi_tx_dfx_get_info()->cmd_in_data_trans_cnt <= MIPI_TX_STA_ERR_PRINT_MAX_CNT) {
            uint16_t cmd_size = (cmd_info->cmd == TD_NULL) ? 0x2 : cmd_info->cmd_size;
            mipi_tx_err("mipi_tx is in data trans! write cmd %#x: work_mode %d data_type %#x cmd_size %d\r\n",
                cmd, cmd_info->work_mode, cmd_info->data_type, cmd_size);
        }
#endif
    }
    ret = mipi_tx_drv_set_cmd_info(cmd_info);
    mipi_tx_up_mutex();
    return ret;
}

static uint32_t mipi_tx_check_get_cmd_info(const get_cmd_info_t *get_cmd_info)
{
    if (get_cmd_info->devno != 0) {
        mipi_tx_err("mipi_tx devno(%u) illegal!\r\n", get_cmd_info->devno);
        return EXT_ERR_MIPI_INVALID_PARAM;
    }

    if ((get_cmd_info->work_mode != MIPI_TX_WORK_MODE_LP) &&
        (get_cmd_info->work_mode != MIPI_TX_WORK_MODE_HS)) {
        mipi_tx_err("mipi_tx work_mode(%d) is illegal!\r\n", get_cmd_info->work_mode);
        return EXT_ERR_MIPI_INVALID_MODE;
    }

    if ((get_cmd_info->get_data_size == 0) || (get_cmd_info->get_data_size > MIPI_TX_GET_DATA_SIZE)) {
        mipi_tx_err("mipi_tx dev get_data_size(%u) illegal!\r\n", get_cmd_info->get_data_size);
        return EXT_ERR_MIPI_INVALID_PARAM;
    }

    if (get_cmd_info->get_data == NULL) {
        mipi_tx_err("mipi_tx dev get_data is null!\r\n");
        return EXT_ERR_MIPI_GET_DATA_NULL;
    }

    return EXT_ERR_SUCCESS;
}

static uint32_t mipi_tx_get_cmd(get_cmd_info_t *get_cmd_info)
{
    if (get_cmd_info == NULL) {
        mipi_tx_err("NULL pointer\r\n");
        return EXT_ERR_MIPI_INVALID_PARAM;
    }

    mipi_tx_down_mutex();

    uint32_t ret = mipi_tx_check_get_cmd_info(get_cmd_info);
    if (ret != EXT_ERR_SUCCESS) {
        mipi_tx_up_mutex();
        mipi_tx_err("mipi_tx check get cmd info failed!\r\n");
        return ret;
    }

    if (mipi_tx_is_in_data_trans() == TD_TRUE) {
        mipi_tx_cmd_info cmd = {.cmd_type = MIPI_TX_CMD_TYPE_READ, .cmd = get_cmd_info->data_param};
        mipi_tx_dfx_record_cmd_in_data_trans(cmd);
#ifdef MIPI_TX_STA_DET_DFX
        if (mipi_tx_dfx_get_info()->cmd_in_data_trans_cnt <= MIPI_TX_STA_ERR_PRINT_MAX_CNT) {
            mipi_tx_err("mipi_tx is in data trans! read cmd %#x: work_mode %d data_type %#x get_data_size %d\r\n",
                get_cmd_info->data_param, get_cmd_info->work_mode, get_cmd_info->data_type,
                get_cmd_info->get_data_size);
        }
#endif
    }

    ret = mipi_tx_drv_get_cmd_info(get_cmd_info);
    mipi_tx_up_mutex();

    return ret;
}

PLT_SEC_ITCM_T static INLINE uint32_t mipi_tx_enable(void)
{
    mipi_tx_drv_enable_input(&g_mipi_tx_dev_cfg);
    g_mipi_tx_attr.en_dev = true;

    return EXT_ERR_SUCCESS;
}

static uint32_t mipi_tx_disable(void)
{
    uint32_t ret = mipi_tx_drv_disable_input();
    g_mipi_tx_attr.en_dev = false;

    return ret;
}

uint32_t mipi_tx_partial_refresh(sync_info_t *dev_sync, uint32_t pixel_clk)
{
    uint32_t ret = mipi_tx_enable_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    if (dev_sync == NULL) {
        mipi_tx_err("NULL pointer\r\n");
        return EXT_ERR_MIPI_INVALID_PARAM;
    }

    mipi_tx_down_mutex();

    g_mipi_tx_dev_cfg.sync_info = *dev_sync;
    g_mipi_tx_dev_cfg.pixel_clk = pixel_clk;

    /* set controler config */
    mipi_tx_drv_set_partial_refresh(&g_mipi_tx_dev_cfg);

    mipi_tx_up_mutex();

    return ret;
}

static uint32_t mipi_tx_proc_dev_show(void)
{
    combo_dev_cfg_t *pdev_cfg = NULL;
    sync_info_t *psync_info = NULL;
    pdev_cfg = &g_mipi_tx_dev_cfg;
    psync_info = &g_mipi_tx_dev_cfg.sync_info;
    char *out_mode[] = {"video", "command"};
    char *video_mode[] = {"burst", "non_burst_pulses", "non_burst_event"};
    char *out_format[] = {"rgb565", "rgb666", "rgb666-loosely", "rgb888",
                          "yuv420-12bit", "yuv422-16bit"};

    /* mipi tx device config */
    mipi_proc_print("----------mipi_tx dev config---------------------------\r\n");
    mipi_proc_print("%-25s:%-6u\r\n", "devno", pdev_cfg->devno);
    mipi_proc_print("%-25s:%-6d\r\n", "lane0", pdev_cfg->lane_id[0]);
    mipi_proc_print("%-25s:%-6d\r\n", "lane1", pdev_cfg->lane_id[1]);
    mipi_proc_print("%-25s:%-6s\r\n", "output_mode", out_mode[pdev_cfg->out_mode - 1]);
    mipi_proc_print("%-25s:%-6u\r\n", "phy_data_rate(Mbps)", mipi_tx_get_actual_phy_data_rate());
    mipi_proc_print("%-25s:%-6u\r\n", "pixel_clk(KHz)", pdev_cfg->pixel_clk);
    mipi_proc_print("%-25s:%-6s\r\n", "video_mode", video_mode[pdev_cfg->video_mode]);
    mipi_proc_print("%-25s:%-6s\r\n", "output_fmt", out_format[pdev_cfg->out_format]);
    mipi_proc_print("%-25s:%-6u\r\n", "eotp_tx_en", pdev_cfg->pck_en.eotp_tx_en);
    mipi_proc_print("%-25s:%-6u\r\n", "eotp_rx_en", pdev_cfg->pck_en.eotp_rx_en);
    mipi_proc_print("\r\n");

    /* mipi tx device sync config */
    mipi_proc_print("----------mipi_tx sync config---------------------------\r\n");
    mipi_proc_print("%-25s:%-6u\r\n", "hact", psync_info->hact_pixels);
    mipi_proc_print("%-25s:%-6u\r\n", "hbp", psync_info->hbp_pixels);
    mipi_proc_print("%-25s:%-6u\r\n", "hfp", psync_info->hfp_pixels);
    mipi_proc_print("%-25s:%-6u\r\n", "hsa", psync_info->hsa_pixels);
    mipi_proc_print("%-25s:%-6u\r\n", "vact", psync_info->vact_lines);
    mipi_proc_print("%-25s:%-6u\r\n", "vbp", psync_info->vbp_lines);
    mipi_proc_print("%-25s:%-6u\r\n", "vfp", psync_info->vfp_lines);
    mipi_proc_print("%-25s:%-6u\r\n", "vsa", psync_info->vsa_lines);
    mipi_proc_print("\r\n");
    return EXT_ERR_SUCCESS;
}

static uint32_t mipi_tx_proc_dev_status_show(void)
{
    uint32_t ret;
    mipi_tx_dev_phy_t mipi_tx_phy_ctx = {0};
    ret = mipi_tx_init_status_check();
    if (ret == EXT_ERR_SUCCESS) {
        mipi_tx_drv_get_dev_status(&mipi_tx_phy_ctx);
    }

    /* mipi tx phy status */
    mipi_proc_print("----------mipi_tx dev status---------------------------\r\n");
    mipi_proc_print("%-25s:%-6u\r\n", "hact_det", mipi_tx_phy_ctx.hact_det);
    mipi_proc_print("%-25s:%-6u\r\n", "vact_det", mipi_tx_phy_ctx.vact_det);
    mipi_proc_print("%-25s:%-6s\r\n", "hall_det", "hact+hbp+hfp+hsa-1");
    mipi_proc_print("%-25s:%-6u\r\n", "hall_det", mipi_tx_phy_ctx.hall_det);
    mipi_proc_print("%-25s:%-6s\r\n", "vall_det(command)", "vact+vbp+vfp+vsa+1");
    mipi_proc_print("%-25s:%-6u\r\n", "vall_det", mipi_tx_phy_ctx.vall_det);
    mipi_proc_print("%-25s:%-6u\r\n", "hbp_det", mipi_tx_phy_ctx.hbp_det);
    mipi_proc_print("%-25s:%-6u\r\n", "hsa_det", mipi_tx_phy_ctx.hsa_det);
    mipi_proc_print("%-25s:%-6u\r\n", "vsa_det", mipi_tx_phy_ctx.vsa_det);
    mipi_proc_print("%-25s:%-6u\r\n", "ppi_vall", mipi_tx_phy_ctx.vall_ppi);
    mipi_proc_print("%-25s:%-6s\r\n", "ppi_vact(command)", "vact-1");
    mipi_proc_print("%-25s:%-6u\r\n", "ppi_vact", mipi_tx_phy_ctx.vact_ppi);
    mipi_proc_print("%-25s:%-6s\r\n", "ppi_hact(command)", "(vact*3+3)/lane_num+1");
    mipi_proc_print("%-25s:%-6s\r\n", "ppi_hact(video)", "(vact*3+2)/lane_num+1");
    mipi_proc_print("%-25s:%-6u\r\n", "ppi_hact", mipi_tx_phy_ctx.hact_ppi);
    mipi_proc_print("%-25s:%-6u\r\n", "lane_num", mipi_tx_phy_ctx.lane_num);
    mipi_proc_print("\r\n");

    return EXT_ERR_SUCCESS;
}

static void mipi_tx_proc_int_status(void)
{
    uint32_t int_status_dfx[MIPI_INT_DFX_NUM] = {0};
    mipi_tx_drv_get_int_status(int_status_dfx, sizeof(int_status_dfx));
    mipi_tx_dfx_info *dfx_info = mipi_tx_dfx_get_info();
    mipi_proc_print("----------mipi_tx int status---------------------------\r\n");
    mipi_proc_print("%-25s:0x%-6X\r\n", "int0_status", int_status_dfx[0]);
    mipi_proc_print("%-25s:0x%-6X\r\n", "int1_status", int_status_dfx[1]);
    mipi_proc_print("%-25s:0x%-6X\r\n", "pld_err_cnt", dfx_info->pld_err_cnt);

    mipi_proc_print("\r\n");
}

mipi_tx_lane_status mipi_tx_get_lane_state(const mipi_tx_lane_state_t *state)
{
    mipi_tx_lane_status lane_state;
    if (state->idle_active == 0 && state->exit_ulps == 1 && state->exit_sdlp == 1) {
        lane_state = MIPI_TX_LANE_STATE_NON_IDLE;
    } else if (state->idle_active == 1 && state->exit_ulps == 1 && state->exit_sdlp == 1) {
        lane_state = MIPI_TX_LANE_STATE_IDLE;
    } else if (state->idle_active == 0 && state->exit_ulps == 1 && state->exit_sdlp == 0) {
        lane_state = MIPI_TX_LANE_STATE_SDLP;
    } else if (state->idle_active == 0 && state->exit_ulps == 0 && state->exit_sdlp == 1) {
        lane_state = MIPI_TX_LANE_STATE_ULPS;
    } else {
        lane_state = MIPI_TX_LANE_STATE_INAVLID;
    }
    return lane_state;
}

static uint32_t mipi_tx_proc_ulps_status(void)
{
    mipi_tx_lane_state_t clk_lane = {0};
    mipi_tx_lane_state_t data_lane0 = {0};
    mipi_tx_dev_phy_t mipi_tx_phy_ctx = {0};
    uint32_t clk_stat;
    uint32_t data0_stat;
    char *lane_mode[] = {"non_idle", "idle", "sdlp", "ulps"};
    uint32_t ulps_entry_cnt = mipi_tx_hal_get_ulps_entry_count();
    uint32_t ulps_exit_cnt = mipi_tx_hal_get_ulps_exit_count();

    mipi_tx_hal_get_lane_state(MIPI_TX_LANE_DATA0, &data_lane0);
    mipi_tx_hal_get_lane_state(MIPI_TX_LANE_CLK, &clk_lane);
    mipi_tx_drv_get_dev_status(&mipi_tx_phy_ctx);

    clk_stat = mipi_tx_get_lane_state(&clk_lane);
    data0_stat = mipi_tx_get_lane_state(&data_lane0);

    mipi_proc_print("----------mipi_tx ulps status--------------------------\r\n");
    mipi_proc_print("%-25s:%-6u\r\n", "ulps_entry_cnt", ulps_entry_cnt);
    mipi_proc_print("%-25s:%-6d\r\n", "ulps_exit_cnt", ulps_exit_cnt);
    if (clk_stat < MIPI_TX_LANE_STATE_INAVLID) {
        mipi_proc_print("%-25s:%-6s\r\n", "clk_lane status", lane_mode[clk_stat]);
    } else {
        mipi_proc_print("%-35s:%-6u/%-6u/%-6u\r\n", "[ERR]clk_lane idle/sdlp/ulps",
            clk_lane.idle_active, clk_lane.exit_sdlp, clk_lane.exit_ulps);
    }
    if (data0_stat < MIPI_TX_LANE_STATE_INAVLID) {
        mipi_proc_print("%-25s:%-6s\r\n", "data_lane0 status", lane_mode[data0_stat]);
    } else {
        mipi_proc_print("%-35s:%-6u/%-6u/%-6u\r\n", "[ERR]data_lane0 idle/sdlp/ulps",
            data_lane0.idle_active, data_lane0.exit_sdlp, data_lane0.exit_ulps);
    }
#if !defined(GPU_1A21) && !defined(GPU_3322)
    mipi_proc_print("%-25s:%-6u\r\n", "mipi_rdy", mipi_tx_hal_get_mipi2dpu_rdy());
#endif
    mipi_tx_dfx_info *dfx_info = mipi_tx_dfx_get_info();
    mipi_proc_print("%-25s:%-6u\r\n", "exit_ulps_cnt_ext", dfx_info->exit_ulps_cnt_ext);
    mipi_proc_print("%-25s:%-6u\r\n", "sus_sta_err_cnt", dfx_info->sus_sta_err_cnt);
    mipi_proc_print("%-25s:%-6u\r\n", "sus_sta_err_cnt_ext", dfx_info->sus_sta_err_cnt_ext);
    mipi_proc_print("%-25s:%-6u\r\n", "cmd_in_data_trans_cnt", dfx_info->cmd_in_data_trans_cnt);

    if (dfx_info->cmd_in_data_trans_cnt > 0) {
        uint32_t index = (dfx_info->cmd_in_data_trans_cnt - 1) % CMD_IN_DATA_TRANS_CMD_MAX_NUM;
        mipi_tx_cmd_type cmd_type = dfx_info->cmd_in_data_trans[index].cmd_type;
        char *cmd_type_str[] = {"R", "W"};
        mipi_proc_print("%-25s:%s/%04#x\r\n", "last_cmd_in_data_trans", cmd_type_str[cmd_type],
            dfx_info->cmd_in_data_trans[index].cmd);
    }

    mipi_proc_print("\r\n");

    return EXT_ERR_SUCCESS;
}

#if !defined(GPU_1A21) && !defined(GPU_3322)
static uint32_t mipi_tx_proc_pkg_cnt(void)
{
    mipi_proc_print("----------mipi_tx pkg cnt--------------------------\r\n");
    mipi_proc_print("%-25s:%-6u\r\n", "cmd_mode_frm_cnt", mipi_tx_hal_dfx_get_frame_cnt());
    mipi_proc_print("%-25s:%-6d\r\n", "lp_pkt_cnt", mipi_tx_hal_dfx_get_lp_pkt_cnt());
    mipi_proc_print("%-25s:%-6d\r\n", "hs_pkt_cnt", mipi_tx_hal_dfx_get_hs_pkt_cnt());
    mipi_proc_print("%-25s:%-6d\r\n", "dpu_pkt_cnt", mipi_tx_hal_dfx_get_dpu_pkt_cnt());

    return EXT_ERR_SUCCESS;
}
#endif

uint32_t mipi_tx_proc_show(void)
{
    uint32_t ret;

    ret = mipi_tx_init_status_check();
    if (ret != EXT_ERR_SUCCESS) {
        mipi_proc_print("\r\nMIPI_TX Module is not inited!\r\n");
        return 0;
    }
    mipi_proc_print("\r\nModule: [MIPI_TX], Build Time[Mar 26 2022, 19:39:41]\r\n");
    mipi_tx_proc_dev_show();
    mipi_tx_proc_dev_status_show();
    mipi_tx_proc_int_status();
    mipi_tx_proc_ulps_status();
#if !defined(GPU_1A21) && !defined(GPU_3322)
    mipi_tx_proc_pkg_cnt();
#endif
    return EXT_ERR_SUCCESS;
}

#ifdef CONFIG_MIPI_PROC_SUPPORT
static int32_t mipi_get_proc_info(int32_t argc, const int8_t *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    mipi_tx_proc_show();
    return EXT_ERR_SUCCESS;
}
#endif

uint32_t mipi_tx_cfg_show(void)
{
    uint32_t ret = mipi_tx_init_status_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }
#ifdef CONFIG_MIPI_PROC_SUPPORT
    g_mipi_tx_attr.cfg_ctrl.read(0);
#endif

    return EXT_ERR_SUCCESS;
}

PLT_SEC_ITCM_T uint32_t uapi_mipi_tx_init(void)
{
    uint32_t ret = EXT_ERR_SUCCESS;
    if (g_mipi_tx_attr.is_inited == TD_TRUE) {
        return EXT_ERR_MIPI_ALREADY_INIT;
    }

    g_mipi_tx_attr.is_inited = 0;
    g_mipi_tx_attr.en_dev = 0;
    g_mipi_tx_attr.en_dev_cfg = 0;

#ifdef CONFIG_MIPI_PROC_SUPPORT
    g_mipi_tx_attr.cfg_ctrl.read = mipi_tx_proc_show;
    g_mipi_tx_attr.cfg_ctrl.write = NULL;
#endif
    ret = mipi_tx_drv_init();
    if (ret != EXT_ERR_SUCCESS) {
        mipi_tx_err("mipi_tx_drv_init failed!\r\n");
        return ret;
    }

    ret = mipi_tx_mutex_init();
    if (ret != EXT_ERR_SUCCESS) {
        mipi_tx_drv_exit();
        mipi_tx_err("init sema error!\r\n");
        ret = EXT_ERR_MIPI_CREATE_MUX_FAIL;
        return ret;
    }

#ifdef CONFIG_MIPI_PROC_SUPPORT
    OsCmdReg("mipi_proc", "get mipi_tx proc", mipi_get_proc_info);
#endif
    g_mipi_tx_attr.is_inited = TD_TRUE;
    mipi_tx_print("init mipi_tx successful!\r\n");
    return EXT_ERR_SUCCESS;
}

uint32_t uapi_mipi_tx_deinit(void)
{
    if (g_mipi_tx_attr.is_inited == TD_FALSE) {
        return EXT_ERR_SUCCESS;
    }

    mipi_tx_drv_exit();
    g_mipi_tx_attr.is_inited = TD_FALSE;

    mipi_tx_print("unload mipi_tx driver ok!\r\n");

    return EXT_ERR_SUCCESS;
}

PLT_SEC_ITCM_T uint32_t uapi_mipi_tx_set_dev_cfg(combo_dev_cfg_t *dev_cfg)
{
    uint32_t ret = mipi_tx_init_status_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    if (g_mipi_tx_attr.en_dev) {
        mipi_tx_err("mipi_tx has been enable!\r\n");
        return EXT_ERR_MIPI_DEV_ALREADY_ENABLE;
    }

    return mipi_tx_set_combo_dev_cfg(dev_cfg);
}

uint32_t uapi_mipi_tx_set_cmd(cmd_info_t *cmd_info)
{
    uint32_t ret = mipi_tx_dev_cfg_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    return mipi_tx_set_cmd(cmd_info);
}

uint32_t uapi_mipi_tx_get_cmd(get_cmd_info_t *cmd_info)
{
    uint32_t ret = mipi_tx_dev_cfg_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    return mipi_tx_get_cmd(cmd_info);
}

PLT_SEC_ITCM_T uint32_t uapi_mipi_tx_enable(void)
{
    uint32_t ret = mipi_tx_dev_cfg_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    if (g_mipi_tx_attr.en_dev) {
        mipi_tx_err("mipi_tx has been enable!\r\n");
        return EXT_ERR_MIPI_DEV_ALREADY_ENABLE;
    }

#ifdef MIPI_ULPS_SUPPORT
    mipi_tx_hal_autoulps_en(1);
#endif
    return mipi_tx_enable();
}

uint32_t uapi_mipi_tx_disable(void)
{
    uint32_t ret = mipi_tx_init_status_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    return mipi_tx_disable();
}

static uint32_t mipi_tx_colorbar_check(colorbar_orien_mode orien, colorbar_input_mode mode)
{
    uint32_t ret = mipi_tx_dev_cfg_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    if (orien >= ORIEN_MODE_BUTT) {
        mipi_tx_err("invalid colorbar orien!\r\n");
        return EXT_ERR_MIPI_INVALID_COLORBAR_ORIEN;
    }

    if (mode >= INPUT_MODE_BUTT) {
        mipi_tx_err("invalid colorbar input mode!\r\n");
        return EXT_ERR_MIPI_INVALID_COLORBAR_INPUT_MODE;
    }

    return EXT_ERR_SUCCESS;
}

uint32_t mipi_tx_set_colorbar(colorbar_orien_mode orien, colorbar_input_mode mode)
{
    uint32_t ret = mipi_tx_colorbar_check(orien, mode);
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    mipi_tx_down_mutex();

    mipi_tx_hal_colorbar_mode(&g_mipi_tx_dev_cfg, orien, mode);
    g_mipi_tx_attr.en_dev = true;

    mipi_tx_up_mutex();

    return EXT_ERR_SUCCESS;
}

uint32_t mipi_tx_unset_colorbar(void)
{
    uint32_t ret = mipi_tx_init_status_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    mipi_tx_down_mutex();

    mipi_tx_hal_unset_colorbar(&g_mipi_tx_dev_cfg);
    g_mipi_tx_attr.en_dev = true;

    mipi_tx_up_mutex();

    return EXT_ERR_SUCCESS;
}

uint32_t uapi_mipi_tx_auto_enter_sdlp(void)
{
    uint32_t ret;

    ret = mipi_tx_dev_cfg_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    if (g_mipi_tx_attr.en_dev) {
        mipi_tx_hal_auto_enter_sdlp();
    }

    return EXT_ERR_SUCCESS;
}

uint32_t uapi_mipi_tx_exit_sdlp(void)
{
    uint32_t ret;

    ret = mipi_tx_dev_cfg_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    if (!g_mipi_tx_attr.en_dev) {
        return EXT_ERR_SUCCESS;
    }

    return mipi_tx_hal_exit_sdlp();
}

uint32_t uapi_mipi_tx_exit_ulps(void)
{
    uint32_t ret;

    ret = mipi_tx_dev_cfg_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    mipi_tx_down_mutex();
    mipi_tx_hal_exit_ulps();
    mipi_tx_up_mutex();
    return EXT_ERR_SUCCESS;
}

uint32_t uapi_mipi_tx_clear_exit_ulps(void)
{
    uint32_t ret;

    ret = mipi_tx_dev_cfg_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    mipi_tx_down_mutex();
    mipi_tx_hal_set_ulps_entry_delay(MIPI_TX_DEFAULT_ENTRY_ULPS_CYLE_NUM);
    mipi_tx_hal_clear_exit_ulps();
    mipi_tx_up_mutex();
    return EXT_ERR_SUCCESS;
}

uint32_t uapi_mipi_tx_set_auto_exit_ulps_en(td_bool en)
{
    uint32_t ret;

    ret = mipi_tx_dev_cfg_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    mipi_tx_down_mutex();
    mipi_tx_hal_set_auto_exit_ulps_en(en);
    mipi_tx_up_mutex();
    return EXT_ERR_SUCCESS;
}

uint32_t uapi_mipi_tx_force_exit_ulps(void)
{
    uint32_t ret;

    ret = mipi_tx_dev_cfg_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    mipi_tx_down_mutex();

    mipi_tx_hal_exit_ulps();
    mipi_tx_hal_gen_ulps_flg_pulse();

    mipi_tx_up_mutex();
    return EXT_ERR_SUCCESS;
}

uint32_t uapi_mipi_tx_hal_set_ulpsflg2te_time(uint32_t ulpsflg2te_time_us)
{
    uint32_t ret = mipi_tx_dev_cfg_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    mipi_tx_down_mutex();
    mipi_tx_hal_set_ulpsflg2te_time(ulpsflg2te_time_us);
    mipi_tx_up_mutex();
    return EXT_ERR_SUCCESS;
}

uint32_t uapi_mipi_tx_pld_err_det(td_bool *is_err)
{
    uint32_t ret = mipi_tx_dev_cfg_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    if (mipi_tx_hal_is_pld_err() == TD_TRUE) {
        mipi_tx_dfx_info *dfx_info = mipi_tx_dfx_get_info();
        dfx_info->pld_err_cnt++;
        /* reset mipi_tx */
        mipi_tx_reset();
        mipi_tx_hal_set_request_hsclk_en(1);
        mipi_tx_hal_set_clklane_continue_en(1);
        *is_err = TD_TRUE;
        mipi_tx_err("mipi pld err\n");
        return EXT_ERR_SUCCESS;
    }

    *is_err = TD_FALSE;
    return EXT_ERR_SUCCESS;
}

uint32_t uapi_mipi_tx_ulps_entry_suspend(td_bool en)
{
    uint32_t ret;

    ret = mipi_tx_dev_cfg_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    mipi_tx_down_mutex();

    mipi_tx_hal_set_auto_ulps_entry_suspend(en);

    mipi_tx_up_mutex();
    return EXT_ERR_SUCCESS;
}

uint32_t mipi_tx_set_cmd_video_mode(td_u32 module)
{
    uint32_t ret;

    ret = mipi_tx_dev_cfg_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }
    mipi_tx_hal_set_cmd_video_mode(module);
    return EXT_ERR_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}

#endif
#endif

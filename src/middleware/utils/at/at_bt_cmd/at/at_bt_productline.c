/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: At bt function \n
 *
 */

#include "at_bt_productline.h"
#include "at_bt.h"
#include "at.h"
#include "at_bt_productline_cmd_table.h"
#include "at_bt_cmd_register.h"
#include "at_bt_extern.h"
#include "td_type.h"
#include "std_def.h"
#include "sle_factory_manager.h"
#include "debug_print.h"
#ifdef EFUSE_BIT_OPERATION
#include "efuse.h"
#endif
#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
#include "nv.h"
#endif

/* 注册回调 */
/* CALLBACK */
at_ret_t bt_at_enable_sle_cmd(void)
{
    if (enable_sle() != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

/* BLE REGISTER CALLBACK */
at_ret_t bt_at_ble_register_callback_cmd(void)
{
    if (ble_at_cmd_factory_register_callbacks() != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

/* BLE REGISTER CALLBACK */
at_ret_t bt_at_sle_register_callback_cmd(void)
{
    if (sle_at_cmd_factory_register_cbks() != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

/* BT SINGLE TONE */
at_ret_t bt_at_rf_single_tone_cmd(const rf_single_tone_param_args_t *args)
{
    ble_rf_single_tone_t param = { 0 };
    param.rf_freq = (uint8_t)args->rf_freq;
    param.rf_mode = (uint8_t)args->rf_mode;
    ble_factory_rf_single_tone(&param);
    return AT_RET_OK;
}
/* XO频偏校准 */
// 设置频偏校准值
at_ret_t bt_at_pdl_xo_trim_cmd(bt_pdl_xo_trim_t *args)
{
    ble_vendor_productline_cmd_t pdl_param = { 0 };
    pdl_param.sub_opcode = BTH_PRODUCTLINE_XO_TRIM;
    pdl_param.xo_trim.coarse_val = (uint8_t)(args->coarse_val);
    pdl_param.xo_trim.fine_val = (uint8_t)(args->fine_val);
    if (ble_factory_vendor_productline_cmd(&pdl_param) != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

// 读XO频偏校准值
at_ret_t bt_at_pdl_xo_trim_rd_cmd(void)
{
    ble_vendor_productline_cmd_t pdl_param = { 0 };
    pdl_param.sub_opcode = BTH_PRODUCTLINE_XO_TRIM_RD_VAL;
    if (ble_factory_vendor_productline_cmd(&pdl_param) != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

// 写XO频偏校准efuse
at_ret_t bt_at_pdl_xo_trim_wr_efuse_cmd(void)
{
    ble_vendor_productline_cmd_t pdl_param = { 0 };
    pdl_param.sub_opcode = BTH_PRODUCTLINE_EFUSE_WRITE_XO_TRIM;
    if (ble_factory_vendor_productline_cmd(&pdl_param) != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

// 读XO频偏校准efuse
at_ret_t bt_at_pdl_xo_trim_rd_efuse_cmd(void)
{
    ble_vendor_productline_cmd_t pdl_param = { 0 };
    pdl_param.sub_opcode = BTH_PRODUCTLINE_EFUSE_READ_XO_TRIM;
    if (ble_factory_vendor_productline_cmd(&pdl_param) != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

// 获取温度
at_ret_t bt_at_pdl_xo_trim_get_temp_cmd(void)
{
    ble_vendor_productline_cmd_t pdl_param = { 0 };
    pdl_param.sub_opcode = BTH_PRODUCTLINE_GET_TSENSOR_TEMPERATURE;
    if (ble_factory_vendor_productline_cmd(&pdl_param) != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

// 写温度efuse
at_ret_t bt_at_pdl_temp_wr_efuse_cmd(bt_pdl_temp_wr_efuse_t *args)
{
    ble_vendor_productline_cmd_t pdl_param = { 0 };
    pdl_param.sub_opcode = BTH_PRODUCTLINE_EFUSE_WRITE_TEMPERATURE;
    pdl_param.efuse_wr_temp.temp = args->temp;
    if (ble_factory_vendor_productline_cmd(&pdl_param) != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

// 读温度efuse
at_ret_t bt_at_pdl_temp_rd_efuse_cmd(void)
{
    ble_vendor_productline_cmd_t pdl_param = { 0 };
    pdl_param.sub_opcode = BTH_PRODUCTLINE_EFUSE_READ_TEMPERATURE;
    if (ble_factory_vendor_productline_cmd(&pdl_param) != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

/* 功率校准 */
// 下发目标和实测功率
at_ret_t bt_at_pdl_pwr_cali_set_pwr_cmd(bt_pdl_pwr_cali_set_pwr_t *args)
{
    ble_vendor_productline_cmd_t pdl_param = { 0 };
    pdl_param.sub_opcode = BTH_PRODUCTLINE_PWR_CALI_SET_MEASSURED_PWR;
    pdl_param.pwr_info.target_pwr = (int16_t)(args->target_pwr);
    pdl_param.pwr_info.pwr = (int16_t)(args->msrd_pwr);
    if (ble_factory_vendor_productline_cmd(&pdl_param) != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

// 获取校准结果
at_ret_t bt_at_pdl_pwr_cali_get_val_cmd(void)
{
    ble_vendor_productline_cmd_t pdl_param = { 0 };
    pdl_param.sub_opcode = BTH_PRODUCTLINE_PWR_CALI_GET_COMP_RESULT;
    if (ble_factory_vendor_productline_cmd(&pdl_param) != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

// 应用校准结果
at_ret_t bt_at_pdl_pwr_cali_apply_cmd(void)
{
    ble_vendor_productline_cmd_t pdl_param = { 0 };
    pdl_param.sub_opcode = BTH_BTH_PRODUCTLINE_PWR_CALI_APPLY_COMP;
    if (ble_factory_vendor_productline_cmd(&pdl_param) != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

// 写功率校准efuse
at_ret_t bt_at_pdl_pwr_cali_wr_efuse_cmd(void)
{
    ble_vendor_productline_cmd_t pdl_param = { 0 };
    pdl_param.sub_opcode = BTH_PRODUCTLINE_EFUSE_WRITE_PWR_COMP;
    if (ble_factory_vendor_productline_cmd(&pdl_param) != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

// 读功率校准efuse
at_ret_t bt_at_pdl_pwr_cali_rd_efuse_cmd(void)
{
    ble_vendor_productline_cmd_t pdl_param = { 0 };
    pdl_param.sub_opcode = BTH_PRODUCTLINE_EFUSE_READ_PWR_COMP;
    if (ble_factory_vendor_productline_cmd(&pdl_param) != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

/* TX/RX测试 */
/* BLE RESET */
at_ret_t bt_at_ble_reset_cmd(void)
{
    if (ble_factory_rf_reset() != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

/* BLE常发 */
at_ret_t bt_at_ble_rf_tx_cmd(const ble_rf_tx_param_args_t *args)
{
    ble_rf_tx_start_t rf_tx_start = { 0 };
    rf_tx_start.tx_freq = (uint8_t)args->tx_freq;
    rf_tx_start.test_data_len = (uint8_t)args->test_data_len;
    rf_tx_start.payload_type = (uint8_t)args->pk_payload_type;
    rf_tx_start.tx_phy = (uint8_t)args->tx_phy;
    if (ble_factory_rf_tx_start(&rf_tx_start) != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

/* BLE常收 */
at_ret_t bt_at_ble_rf_rx_cmd(const ble_rf_rx_param_args_t *args)
{
    ble_rf_rx_start_t rf_rx_start = { 0 };
    rf_rx_start.rx_freq = (uint8_t)args->rx_freq;
    rf_rx_start.rx_phy = (uint8_t)args->rx_phy;
    rf_rx_start.modulation_index = (uint8_t)args->modulation_index;
    if (ble_factory_rf_rx_start(&rf_rx_start) != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

/* BLE TRX END */
at_ret_t bt_at_ble_rf_trxend_cmd(void)
{
    if (ble_factory_rf_trx_end() != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

/* SLE RESET */
at_ret_t bt_at_sle_reset_cmd(void)
{
    if (sle_rf_reset() != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

/* SLE常发 */
at_ret_t bt_at_sle_rf_tx_cmd(const sle_rf_tx_param_args_t *args)
{
    sle_rf_tx_start_t rf_tx_start = { 0 };
    rf_tx_start.tx_freq = (uint8_t)args->tx_freq;
    rf_tx_start.tx_power = (uint8_t)args->tx_power;
    rf_tx_start.test_data_len = (uint16_t)args->test_data_len;
    rf_tx_start.pk_payload_type = (uint8_t)args->pk_payload_type;
    rf_tx_start.tx_phy = (uint8_t)args->tx_phy;
    rf_tx_start.tx_format = (uint8_t)args->tx_format;
    rf_tx_start.tx_rate = (uint8_t)args->tx_rate;
    rf_tx_start.tx_pilot_ratio = (uint8_t)args->tx_pilot_ratio;
    rf_tx_start.tx_polar_r = (uint8_t)args->tx_polar_r;
    rf_tx_start.tx_interval = (uint16_t)args->tx_interval;
    if (sle_rf_tx_start(&rf_tx_start) != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

/* SLE常收 */
at_ret_t bt_at_sle_rf_rx_cmd(const sle_rf_rx_param_args_t *args)
{
    sle_rf_rx_start_t rf_rx_start = { 0 };
    rf_rx_start.rx_freq = (uint8_t)args->rx_freq;
    rf_rx_start.rx_phy = (uint8_t)args->rx_phy;
    rf_rx_start.rx_format = (uint8_t)args->rx_format;
    rf_rx_start.rx_pilot_ratio = (uint8_t)args->rx_pilot_ratio;
    rf_rx_start.rx_interval = (uint16_t)args->rx_interval;
    if (sle_rf_rx_start(&rf_rx_start) != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

/* SLE TRX END */
at_ret_t bt_at_sle_rf_trxend_cmd(void)
{
    if (sle_rf_trx_end() != MPC_BT_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

#ifdef EFUSE_BIT_OPERATION
#define EFUSE_BSLE_PWR_CALI_FLAG_ID 77
#define EFUSE_BSLE_PWR_CALI_FLAG_ID_OFFSET 1
#ifndef NV_ID_SYSTEM_FACTORY_BSLE_FLAG
#define NV_ID_SYSTEM_FACTORY_BSLE_FLAG 0x0026
#endif
#ifndef NV_ID_SYSTEM_FACTORY_FLASH_ENABLE
#define NV_ID_SYSTEM_FACTORY_FLASH_ENABLE 0x0030
#endif
#ifndef MFG_CALI_PARAM_IN_FLASH
// 产测存储参数标记, 1表示写入Flash
#define MFG_CALI_PARAM_IN_FLASH 1
#endif
at_ret_t cmd_get_bsle_pwr_cali_flag(void)
{
    uint8_t flag = 0;
#ifdef CONFIG_MIDDLEWARE_SUPPORT_NV
    uint8_t nv_mfg_in_flash = 0;
    uint16_t read_len = 0;
    uint32_t nv_ret;
    nv_ret = uapi_nv_read(NV_ID_SYSTEM_FACTORY_FLASH_ENABLE, sizeof(uint8_t), &read_len, &nv_mfg_in_flash);
    // 判断是否使用了NV来存储产测数据，等价于is_mfg_in_flash()
    if (((nv_ret == ERRCODE_SUCC)) && (nv_mfg_in_flash == MFG_CALI_PARAM_IN_FLASH)) {
        // 等价于is_mfg_in_flash()==true，从nv读取flag
        if (uapi_nv_read(NV_ID_SYSTEM_FACTORY_BSLE_FLAG, sizeof(uint8_t), &read_len, &flag) != ERRCODE_SUCC) {
            return AT_RET_SYNTAX_ERROR;
        }
        flag = flag & 0x1; // bit0有效
    } else // 等价于is_mfg_in_flash()==false，从efuse读取flag
#endif
    if (uapi_efuse_read_bit(&flag, EFUSE_BSLE_PWR_CALI_FLAG_ID, EFUSE_BSLE_PWR_CALI_FLAG_ID_OFFSET) != AT_RET_OK) {
        return AT_RET_SYNTAX_ERROR;
    }
    uapi_at_print("ble power flag = %x\r\n", flag);
    return AT_RET_OK;
}
#endif

/* AT PRODUCTLINE CMD REGISTER */
#define AT_BT_PDL_FUNC_NUM (sizeof(at_bt_productline_cmd_parse_table) / sizeof(at_bt_productline_cmd_parse_table[0]))

td_void los_at_bt_productline_cmd_register(td_void)
{
    uapi_at_bt_register_cmd(at_bt_productline_cmd_parse_table, AT_BT_PDL_FUNC_NUM);
}

/* AT PRODUCTLINE CMD REGISTER */
#define AT_SLE_PDL_FUNC_NUM (sizeof(at_sle_productline_cmd_parse_table) / sizeof(at_sle_productline_cmd_parse_table[0]))

td_void los_at_sle_productline_cmd_register(td_void)
{
    uapi_at_bt_register_cmd(at_sle_productline_cmd_parse_table, AT_SLE_PDL_FUNC_NUM);
}
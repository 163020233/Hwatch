/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: driver of APC module.
 *
 * Create: 2021-01-13
 */
#include "hal_fapc.h"
#include "hal_fapc_reg.h"
#include "crypto_drv_common.h"

#define clr_region_val(reg, region)                   (reg) &= ~(0x03U << ((region) * 2U))
#define set_region_val(reg, region, val)              (reg) |= ((val) & 0x03U) << ((region) * 2U)

td_s32 hal_fapc_set_region_addr(td_u32 region, td_u32 start_addr, td_u32 end_addr)
{
    crypto_hal_func_enter();
    crypto_chk_return(region >= FAPC_REGION_NUM, TD_FAILURE, "invalid region\n");
    /* need to check address */
    fapc_reg_write(FAPC_CFG_START_ADDR(region), start_addr);
    fapc_reg_write(FAPC_CFG_END_ADDR(region), end_addr);
    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_fapc_set_region_permission(td_u32 region, td_u32 read_mid, td_u32 write_mid)
{
    crypto_hal_func_enter();
    crypto_chk_return(region >= FAPC_REGION_NUM, TD_FAILURE, "invalid region\n");

    fapc_reg_write(FAPC_CFG_ALLOW_RD_ID(region), read_mid);
    fapc_reg_write(FAPC_CFG_ALLOW_WR_ID(region), write_mid);

    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_fapc_set_region_mac_addr(td_u32 region, td_u32 mac_addr)
{
    crypto_unused(region);
    crypto_unused(mac_addr);
    return TD_SUCCESS;
}

#define FAPC_REGION_MODE_NORMAL_VAL     0   /* Flash bypass mode. */
#define FAPC_REGION_MODE_DEC_VAL        2   /* Flash online decryption mode. */
td_s32 hal_fapc_set_region_mode(td_u32 region, crypto_fapc_region_mode region_mode)
{
    td_u32 region_mode_reg_value = 0;
    td_u32 mode_cfg = 0;
    crypto_hal_func_enter();
    crypto_chk_return(region >= FAPC_REGION_NUM, TD_FAILURE, "invalid region\n");

    switch (region_mode) {
        case CRYPTO_FAPC_REGION_MODE_NORMAL:
            region_mode_reg_value = FAPC_REGION_MODE_NORMAL_VAL;
            break;
        case CRYPTO_FAPC_REGION_MODE_DEC:
            region_mode_reg_value = FAPC_REGION_MODE_DEC_VAL;
            break;
        default:
            crypto_log_err("Invalid region_mode\n");
            return TD_FAILURE;
    }

    mode_cfg = fapc_reg_read(FAPC_DEC_AUTH_CFG);
    clr_region_val(mode_cfg, region);
    set_region_val(mode_cfg, region, region_mode_reg_value);
    fapc_reg_write(FAPC_DEC_AUTH_CFG, mode_cfg);

    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_fapc_region_enable(td_u32 region, td_bool enable)
{
    crypto_hal_func_enter();
    crypto_chk_return(region >= FAPC_REGION_NUM, TD_FAILURE, "invalid region\n");

    if (enable == TD_FALSE) {
        fapc_reg_write(FAPC_CFG_APC_EN(region), 0xA);
    } else {
        fapc_reg_write(FAPC_CFG_APC_EN(region), 0x5);
    }
    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_fapc_region_lock(td_u32 region)
{
    td_u32 lock_cfg = 0;
    crypto_hal_func_enter();
    crypto_chk_return(region >= FAPC_REGION_NUM, TD_FAILURE, "invalid region\n");

    lock_cfg = fapc_reg_read(FAPC_ONE_WAY_LOCK);
    lock_cfg |= 1 << region;
    fapc_reg_write(FAPC_ONE_WAY_LOCK, lock_cfg);

    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_fapc_set_region_iv(crypto_fapc_iv_type iv_type, const td_u8 *iv, td_u32 iv_length)
{
    td_u32 iv_word;
    td_u32 i;
    uint32_t base_addr;
    crypto_hal_func_enter();
    crypto_chk_return(iv == TD_NULL, TD_FAILURE, "iv is NULL\n");
    crypto_chk_return(iv_length != CRYPTO_IV_LEN_IN_BYTES, TD_FAILURE, "invalid iv_length\n");

    if (iv_type == CRYPTO_FAPC_IV_TYPE_ACPU) {
        base_addr = FAPC_ACORE_IV_BASE_ADDR;
    } else if (iv_type == CRYPTO_FAPC_IV_TYPE_TEE) {
        base_addr = FAPC_SCORE_IV_BASE_ADDR;
    } else {
        crypto_log_err("invalid iv_type!\n");
        return ERROR_INVALID_PARAM;
    }

    for (i = 0; i < iv_length / CRYPTO_WORD_WIDTH; i++) {
        iv_word = 0;
        crypto_u8_to_word(iv, iv_word, i);
        fapc_reg_write(base_addr + i * CRYPTO_WORD_WIDTH, iv_word);
    }
    /* config iv valid. */
    fapc_reg_write(FAPC_IV_VALID, 0x1);

    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_fapc_set_region_iv_start_addr(td_u32 region, td_u32 start_addr)
{
    crypto_hal_func_enter();
    crypto_chk_return(region >= FAPC_REGION_NUM, TD_FAILURE, "invalid region\n");

    fapc_reg_write(FAPC_IV_ACPU_START_ADDR(region), start_addr);
    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_fapc_set_cmd_lut0(td_u32 num, td_u32 allow_ids, td_u32 lut0_ins)
{
    fapc_cmd_lut0_reg_t cmd_lut0 = {0};
    crypto_hal_func_enter();
    crypto_chk_return(num >= MAX_CMD_LUT0_NUM, TD_FAILURE, "invalid lut0 num\n");

    cmd_lut0.bits.cmd_lut0_allow_ids = allow_ids;
    cmd_lut0.bits.cmd_lut0_ins = lut0_ins;
    fapc_reg_write(FAPC_CMD_LUT0(num), cmd_lut0.u32);

    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_fapc_set_cmd_lut0_enable(td_u32 num, td_bool enable)
{
    fapc_cmd_lut_en_reg_t cmd_lut_en;
    crypto_hal_func_enter();
    crypto_chk_return(num >= MAX_CMD_LUT0_NUM, TD_FAILURE, "invalid lut0 num\n");

    cmd_lut_en.u32 = fapc_reg_read(FAPC_CMD_LUT_EN);
    if (enable) {
        cmd_lut_en.bits.lut0_en |= ((td_u32)1 << num);
    } else {
        cmd_lut_en.bits.lut0_en &= ~((td_u32)1 << num);
    }
    fapc_reg_write(FAPC_CMD_LUT_EN, cmd_lut_en.u32);

    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_fapc_set_cmd_lut1(td_u32 num, td_u32 flash_size, td_u32 lut1_ins, td_bool is_checked_by_wr_id)
{
    fapc_cmd_lut1_reg_t cmd_lut1 = {0};
    td_u32 lut1_rw;

    crypto_hal_func_enter();
    crypto_chk_return(num >= MAX_CMD_LUT1_NUM, TD_FAILURE, "invalid lut1 num\n");

    cmd_lut1.bits.cmd_lut1_len = flash_size;
    cmd_lut1.bits.cmd_lut1_ins = lut1_ins;
    fapc_reg_write(FAPC_CMD_LUT1(num), cmd_lut1.u32);

    lut1_rw = fapc_reg_read(FAPC_CMD_LUT1_RW);
    if (is_checked_by_wr_id == TD_TRUE) {
        lut1_rw |= ((td_u32)1 << num);
    } else {
        lut1_rw &= ~((td_u32)1 << num);
    }
    fapc_reg_write(FAPC_CMD_LUT1_RW, lut1_rw);

    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_fapc_set_cmd_lut1_enable(td_u32 num, td_bool enable)
{
    fapc_cmd_lut_en_reg_t cmd_lut_en;

    crypto_hal_func_enter();
    crypto_chk_return(num >= MAX_CMD_LUT1_NUM, TD_FAILURE, "invalid lut1 num\n");

    cmd_lut_en.u32 = fapc_reg_read(FAPC_CMD_LUT_EN);
    if (enable) {
        cmd_lut_en.bits.lut1_en |= ((td_u32)1 << num);
    } else {
        cmd_lut_en.bits.lut1_en &= ~((td_u32)1 << num);
    }
    fapc_reg_write(FAPC_CMD_LUT_EN, cmd_lut_en.u32);

    crypto_hal_func_exit();
    return TD_SUCCESS;
}

td_s32 hal_fapc_set_cmd_lut_lock(void)
{
    crypto_hal_func_enter();

    fapc_reg_write(FAPC_CMD_LUT_CFG_LOCK, 0x1); // 1: lock; 0: unlock; once lock, only when hardware reset can unlock

    crypto_hal_func_exit();
    return TD_SUCCESS;
}

void hal_fapc_debug(void)
{
#if defined(CONFIG_FAPC_DEBUG_ENABLE)
    unsigned int i;
    unsigned int u32;
    for (i = 0; i < FAPC_REGION_NUM; i++) {
        crypto_print("========== Region %u ==========\n", i);
        crypto_print("start_addr is 0x%x, end_addr is 0x%x\n",
            fapc_reg_read(FAPC_CFG_START_ADDR(i)), fapc_reg_read(FAPC_CFG_END_ADDR(i)));
        if (fapc_reg_read(FAPC_CFG_ALLOW_RD_ID(i)) & 0x1) { // bit0: TEE mst
            crypto_print("Read: TEE enable, ");
        }  else {
            crypto_print("Read: TEE disable, ");
        }
        if (fapc_reg_read(FAPC_CFG_ALLOW_RD_ID(i)) & 0x2) { // bit1: REE mst
            crypto_print("REE enable\n");
        } else {
            crypto_print("REE disable\n");
        }
        if (fapc_reg_read(FAPC_CFG_ALLOW_WR_ID(i)) & 0x1) { // bit0: TEE mst
            crypto_print("Write: TEE enable, ");
        }  else {
            crypto_print("Write: TEE disable, ");
        }
        if (fapc_reg_read(FAPC_CFG_ALLOW_WR_ID(i)) & 0x2) { // bit1: REE mst
            crypto_print("REE enable\n");
        } else {
            crypto_print("REE disable\n");
        }
        crypto_print("FAPC_CFG_APC_EN is 0x%x\n", fapc_reg_read(FAPC_CFG_APC_EN(i)));
        u32 = ((fapc_reg_read(FAPC_DEC_AUTH_CFG) >> (i * 2)) & 0x3); // 2: 2 bits for mode.
        if (u32 == 0x00) {
            crypto_print("Mode: bypass\n");
        } else {
            crypto_print("Mode: decrypt\n");
        }
        crypto_print("FAPC_IV_ACPU_START_ADDR is 0x%x\n", fapc_reg_read(FAPC_IV_ACPU_START_ADDR(i)));
        crypto_print("SFC_FAPC_SADDR_STATUS is 0x%x\n", fapc_reg_read(SFC_FAPC_SADDR_STATUS));
        if ((fapc_reg_read(FAPC_ONE_WAY_LOCK) >> i) & 0x1) {
            crypto_print("Lock!\n");
        } else {
            crypto_print("Unlock!\n");
        }

        crypto_print("========== Region %u End ==========\n", i);
    }
    crypto_print("FAPC_IV_VALID is 0x%x\n", fapc_reg_read(FAPC_IV_VALID));
    for (i = 0; i < 16 / CRYPTO_WORD_WIDTH; i++) {  // 16: iv_length
        crypto_print("iv[%u] is 0x%x\n", i, fapc_reg_read(FAPC_SCORE_IV_BASE_ADDR + i * CRYPTO_WORD_WIDTH));
    }
#endif
}

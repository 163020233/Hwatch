/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides cipher driver common utils. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */

#include "hal_symc_romable.h"
#include "hal_hash_romable.h"
#include "drv_trng.h"
#include "crypto_drv_common.h"

/* crypto dump functions. */
#define MAX_DUMP_LENGHT     (512)
#define BYTES_IN_ONE_LINE   (16)
#ifndef KERN_CONT
#define KERN_CONT
#endif

void crypto_dump_data(const char *name, const td_u8 *data, td_u32 data_len)
{
    td_u32 i;
    td_u32 dump_length = data_len;
    crypto_unused(name);
    crypto_unused(data);
    crypto_print("%s(size is %d Bytes):\r\n", name, data_len);
    if (data == TD_NULL) {
        crypto_print("NULL\r\n");
        return;
    }

    if (dump_length > MAX_DUMP_LENGHT) {
        dump_length = MAX_DUMP_LENGHT;
    }
    for (i = 0; i < dump_length; i++) {
        crypto_print(KERN_CONT"%02x ", data[i]);
        if ((i + 1) % BYTES_IN_ONE_LINE == 0) {
            crypto_print("\r\n");
        }
    }
    if (dump_length % BYTES_IN_ONE_LINE != 0) {
        crypto_print("\r\n");
    }
}

td_s32 crypto_get_value_by_index(const crypto_table_item *table, td_u32 table_size,
    td_u32 index, td_u32 *value)
{
    const crypto_table_item *item = TD_NULL;
    td_u32 i;

    crypto_chk_return(table == TD_NULL, CRYPTO_FAILURE, "table is NULL\n");
    crypto_chk_return(value == TD_NULL, CRYPTO_FAILURE, "value is NULL\n");

    for (i = 0; i < table_size; i++) {
        item = &table[i];
        if (item->index == index) {
            *value = item->value;
            return TD_SUCCESS;
        }
    }
    crypto_log_err("Invalid Index!\n");
    return CRYPTO_FAILURE;
}

__attribute__((__weak__)) int32_t crypto_km_common_get_hard_key_type_item(crypto_keyslot_hard_key_type hard_key_type,
    uint32_t *rkp_cmd_cfg_val, uint32_t *kl_com_ctrl_val)
{
    crypto_unused(hard_key_type);
    crypto_unused(rkp_cmd_cfg_val);
    crypto_unused(kl_com_ctrl_val);

    return ERROR_UNSUPPORT;
}

void __attribute__((weak)) drv_cipher_open_clk(unsigned int module_id)
{
    hal_sec_vote_enable(module_id);
    hal_sec_open_clk(module_id);
    hal_sec_vote(TD_TRUE, module_id);
}

void __attribute__((weak)) drv_cipher_close_clk(unsigned int module_id)
{
    hal_sec_vote(TD_FALSE, module_id);
}

void __attribute__((weak)) drv_cipher_close_all_clk(void)
{
    hal_sec_close_all_clk();
}

/* crypto common atomic. */
int32_t crypto_common_pri_atomic_read(crypto_common_pri_atomic *atomic)
{
    if (atomic == NULL) {
        return CRYPTO_FAILURE;
    }
    return (*atomic);
}

void crypto_common_pri_atomic_inc(crypto_common_pri_atomic *atomic)
{
    if ((atomic == NULL) || (*atomic == CRYPTO_INT32_MAX)) {
        return;
    }
    (*atomic)++;
}

void crypto_common_pri_atomic_dec(crypto_common_pri_atomic *atomic)
{
    if ((atomic == NULL) || (*atomic == 0)) {
        return;
    }
    (*atomic)--;
}

int32_t crypto_common_pri_atomic_inc_return(crypto_common_pri_atomic *atomic)
{
    if ((atomic == NULL) || (*atomic == CRYPTO_INT32_MAX)) {
        return CRYPTO_FAILURE;
    }

    (*atomic)++;
    return (*atomic);
}

int32_t crypto_common_pri_atomic_dec_return(crypto_common_pri_atomic *atomic)
{
    if ((atomic == NULL) || (*atomic == 0)) {
        return CRYPTO_FAILURE;
    }

    (*atomic)--;
    return (*atomic);
}

/*
 * crypto resume & suspend.
 * remove it later.
*/
void __attribute__((weak)) crypto_drv_resume(unsigned int module_id)
{
    (void)module_id;
}

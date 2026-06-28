/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke rsa common mod implement.
 *
 * Create: 2025-01-07
*/
#include "hal_pke_alg.h"

#include "crypto_pke_common.h"
#include "hal_pke_romable.h"
#include "hal_pke_reg.h"
#include "pke_alg_inner.h"

#define pke_compat_instr(opcode, apos, bpos, tpos, rpos)    \
    ((opcode) | ((apos) << APOS_OFFSET) | ((bpos) << BPOS_OFFSET) |   \
        ((tpos) << TPOS_OFFSET) | ((rpos) << RPOS_OFFSET))

#define MAX_SUB_MOD_TIMES       (100000)
#define PKE_MONT_LEN_IN_WORD    4

static int inner_rsa_mod(const drv_pke_data *a, const drv_pke_data *p, const drv_pke_data *c)
{
    int32_t ret;

    hal_pke_set_work_len_romable(p->length);

    /* set mont_param register. */
    ret = inner_update_rsa_modulus(p->data, p->length);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret);

    hal_pke_set_ram_romable(rsa_addr_n, p->data, p->length);
    hal_pke_set_ram_word_romable(rsa_addr_const_1, p->length, -1, 0x1);
    /* set high bit into rsa_addr_t1, set low bit into rsa_addr_t0. */
    if (a->length > p->length) {
        hal_pke_set_ram_romable(rsa_addr_t1, a->data, a->length - p->length);
        hal_pke_set_ram_romable(rsa_addr_t0, a->data + a->length - p->length, p->length);
    } else {
        hal_pke_set_ram_romable(rsa_addr_t0, a->data, a->length);
        hal_pke_set_ram_word_all_romable(rsa_addr_t1, p->length, 0);
    }

    ret = inner_batch_instr_process(&instr_rsa_mod, 0);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret);

    hal_pke_get_ram_romable(rsa_addr_a, c->data, c->length);

    return CRYPTO_SUCCESS;
}

int hal_pke_alg_rsa_mod(const drv_pke_data *a, const drv_pke_data *p, const drv_pke_data *c)
{
    int32_t ret;
    crypto_pke_print("========== Start Compute c = a mod p ==========\n");
    crypto_pke_dump_data("a", a->data, a->length);
    crypto_pke_dump_data("p", p->data, p->length);

    ret = inner_pke_alg_resume();
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_pke_alg_resume failed\n");

    ret = inner_rsa_mod(a, p, c);
    inner_pke_alg_suspend();

    crypto_pke_dump_data("c = a mod p", c->data, c->length);
    crypto_pke_print("========== End Compute c = a mod p ==========\n");
    return ret;
}

/* bit c len will no more than bit_a_len + bit_b_len */
static int inner_rsa_mul(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *c)
{
    int ret;
    unsigned int mont_param_const_1[PKE_MONT_LEN_IN_WORD] = {0, 0, 0, 1};

    /* 2. set data to DRAM */
    hal_pke_set_ram_romable(rsa_addr_t0, a->data, a->length);
    hal_pke_set_ram_romable(rsa_addr_t1, b->data, b->length);
    hal_pke_set_ram_word_all_romable(rsa_addr_n, c->length, 0xFFFFFFFF);  // 0xFFFFFFFF: max number

    /* 3. set montgomery mod multiplication parameter 0x1. */
    hal_pke_set_mont_para_romable(mont_param_const_1);

    /* 4. start calculate. */
    ret = inner_batch_instr_process(&instr_rsa_regular_mul, 0);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_batch_instr_process failed\n");

    /* 5. get result data from DRAM. */
    hal_pke_get_ram_romable(rsa_addr_s, c->data, c->length);

    return ret;
}

int hal_pke_alg_rsa_mul(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *c)
{
    int32_t ret;
    uint32_t work_len = crypto_max(crypto_max(a->length, b->length), c->length);

    crypto_pke_print("========== Start Compute c = a * b ==========\n");
    crypto_pke_dump_data("a", a->data, a->length);
    crypto_pke_dump_data("b", b->data, b->length);

    ret = inner_pke_alg_resume();
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_pke_alg_resume failed\n");

    hal_pke_set_work_len_romable(work_len);

    ret = inner_rsa_mul(a, b, c);
    inner_pke_alg_suspend();

    crypto_pke_dump_data("c = a * b", c->data, c->length);
    crypto_pke_print("========== End Compute c = a * b ==========\n");
    return ret;
}

typedef enum {
    RSA_MOD_ADD,
    RSA_MOD_SUB,
    RSA_MOD_MUL
} rsa_mod_mode;

typedef struct {
    unsigned int ram_section_a;   /* input data a ram section. */
    unsigned int ram_section_b;   /* input data b ram section. */
    unsigned int ram_section_c;   /* out data ram section. */
    unsigned int ram_section_tp;  /* intermediate parameter ram section. */
} instr_data_ram;

static int inner_rsa_mod_common_process(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    rsa_mod_mode mode)
{
    uint32_t size = p->length;

    /* Step 1. set data into DRAM */
    hal_pke_set_ram_romable(rsa_addr_n, p->data, size);
    hal_pke_set_ram_romable(rsa_addr_t0, a->data, size);
    hal_pke_set_ram_romable(rsa_addr_t1, b->data, size);

    /* Step 2. start calculation processing. */
    if (mode == RSA_MOD_ADD) {
        return inner_batch_instr_process(&instr_rsa_mod_add, 0);
    } else {
        return inner_batch_instr_process(&instr_rsa_mod_sub, 0);
    }
}

static int inner_rsa_add_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c)
{
    int ret;
    unsigned int out_instr = pke_compat_instr(CMD_SUB_MOD, rsa_addr_s, rsa_addr_n, rsa_addr_tp, rsa_addr_s);
    uint32_t times = 0;

    ret = inner_rsa_mod_common_process(a, b, p, RSA_MOD_ADD);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_rsa_mod_common_process failed\n");

    do {
        /* Step 4. get result from DRAM */
        hal_pke_get_ram_romable(rsa_addr_s, c->data, c->length);

        ret = hal_pke_single_compute_romable(out_instr);
        crypto_chk_return(ret != CRYPTO_SUCCESS, ret);
        times++;
        if (times > MAX_SUB_MOD_TIMES) {
            return ERROR_PKE_WAIT_DONE_TIMEOUT;
        }
    } while (crypto_pke_common_data_cmp(c, p) >= 0);
    hal_pke_get_ram_romable(rsa_addr_s, c->data, c->length);

    return CRYPTO_SUCCESS;
}

int hal_pke_alg_rsa_add_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c)
{
    int32_t ret;
    uint32_t work_len = crypto_max(crypto_max(crypto_max(a->length, b->length), p->length), c->length);

    crypto_pke_print("========== Start Compute c = (a + b) mod p ==========\n");
    crypto_pke_dump_data("a", a->data, a->length);
    crypto_pke_dump_data("b", b->data, b->length);
    crypto_pke_dump_data("p", p->data, p->length);

    ret = inner_pke_alg_resume();
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_pke_alg_resume failed\n");

    hal_pke_set_work_len_romable(work_len);

    ret = inner_rsa_add_mod(a, b, p, c);
    inner_pke_alg_suspend();

    crypto_pke_dump_data("c = (a + b) mod p", c->data, c->length);
    crypto_pke_print("========== End Compute c = (a + b) mod p ==========\n");
    return ret;
}

static int inner_rsa_sub_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c)
{
    int ret;
    unsigned int out_instr = pke_compat_instr(CMD_SUB_MOD, rsa_addr_s, rsa_addr_n, rsa_addr_tp, rsa_addr_s);
    uint32_t times = 0;

    ret = inner_rsa_mod_common_process(a, b, p, RSA_MOD_SUB);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_rsa_mod_common_process failed\n");

    do {
        hal_pke_get_ram_romable(rsa_addr_s, c->data, c->length);

        ret = hal_pke_single_compute_romable(out_instr);
        crypto_chk_return(ret != CRYPTO_SUCCESS, ret);
        times++;
        if (times > MAX_SUB_MOD_TIMES) {
            return ERROR_PKE_WAIT_DONE_TIMEOUT;
        }
    } while (crypto_pke_common_data_cmp(c, p) >= 0);

    return CRYPTO_SUCCESS;
}

int hal_pke_alg_rsa_sub_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c)
{
    int32_t ret;
    uint32_t work_len = crypto_max(crypto_max(crypto_max(a->length, b->length), p->length), c->length);

    crypto_pke_print("========== Start Compute c = (a - b) mod p ==========\n");
    crypto_pke_dump_data("a", a->data, a->length);
    crypto_pke_dump_data("b", b->data, b->length);
    crypto_pke_dump_data("p", p->data, p->length);

    ret = inner_pke_alg_resume();
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_pke_alg_resume failed\n");

    hal_pke_set_work_len_romable(work_len);

    ret = inner_rsa_sub_mod(a, b, p, c);
    inner_pke_alg_suspend();

    crypto_pke_dump_data("c = (a - b) mod p", c->data, c->length);
    crypto_pke_print("========== End Compute c = (a - b) mod p ==========\n");
    return ret;
}

static int inner_rsa_mul_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c)
{
    int ret;
    uint32_t size = p->length;

    /* 1. set mont_param register. */
    ret = inner_update_rsa_modulus(p->data, p->length);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret);

    /* 2. set data into DRAM */
    hal_pke_set_ram_romable(rsa_addr_n, p->data, p->length);
    hal_pke_set_ram_romable(rsa_addr_t0, a->data, a->length);
    hal_pke_set_ram_romable(rsa_addr_t1, b->data, b->length);
    hal_pke_set_ram_word_romable(rsa_addr_const_1, size, -1, 0x1);

    /* 3. start calculation processing. */
    ret = inner_batch_instr_process(&instr_rsa_mod_mul, 0);
    crypto_chk_func_return(inner_batch_instr_process, ret);

    /* 4. get result from DRAM */
    hal_pke_get_ram_romable(rsa_addr_s, c->data, c->length);

    return CRYPTO_SUCCESS;
}

int hal_pke_alg_rsa_mul_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c)
{
    int32_t ret;
    uint32_t work_len = crypto_max(crypto_max(crypto_max(a->length, b->length), p->length), c->length);

    crypto_pke_print("========== Start Compute c = (a * b) mod p ==========\n");
    crypto_pke_dump_data("a", a->data, a->length);
    crypto_pke_dump_data("b", b->data, b->length);
    crypto_pke_dump_data("p", p->data, p->length);

    ret = inner_pke_alg_resume();
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_pke_alg_resume failed\n");

    hal_pke_set_work_len_romable(work_len);

    ret = inner_rsa_mul_mod(a, b, p, c);
    inner_pke_alg_suspend();

    crypto_pke_dump_data("c = (a * b) mod p", c->data, c->length);
    crypto_pke_print("========== End Compute c = (a * b) mod p ==========\n");
    return ret;
}

/* p is required to be prime number. */
int hal_pke_alg_rsa_inv_mod(const drv_pke_data *a, const drv_pke_data *p, const drv_pke_data *c)
{
    int ret;
    uint8_t *p_2 = NULL;
    drv_pke_data p_2_data;
    crypto_drv_func_enter();
    crypto_pke_print("========== Start Compute c = a^-1 mod p ==========\n");
    crypto_pke_dump_data("a", a->data, a->length);
    crypto_pke_dump_data("p", p->data, p->length);

    p_2 = crypto_malloc_coherent(p->length);
    crypto_chk_return(p_2 == NULL, ERROR_MALLOC, "malloc failed\n");

    p_2_data.data = p_2;
    p_2_data.length = p->length;
    /* Step 1. calculate p-2. */
    ret = memcpy_s(p_2, p->length, p->data, p->length);
    crypto_chk_goto_with_ret(ret, ret != EOK, exit_free, ERROR_MEMCPY_S, "memcyp_ss failed!\n");

    crypto_pke_common_bn_sub_one(p_2, p->length);
    crypto_pke_common_bn_sub_one(p_2, p->length);

    /* Step 2. calculate by exp_mod. */
    ret = hal_pke_alg_rsa_exp_mod(p, &p_2_data, a, c);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_free, "hal_pke_alg_rsa_exp_mod failed!\n");

    crypto_pke_dump_data("c = a^-1 mod p", c->data, c->length);
    crypto_pke_print("========== End Compute c = a^-1 mod p ==========\n");

exit_free:
    (void)memset_s(p_2, p->length, 0, p->length);
    crypto_free_coherent(p_2);
    return ret;
}
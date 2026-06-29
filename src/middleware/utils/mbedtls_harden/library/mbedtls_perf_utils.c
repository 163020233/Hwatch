/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: Provides perf utils implement for crypto.
 *
 */
#if defined(MBEDTLS_PERF_UTILS_OPEN)
#include "tcxo.h"
#include "securec.h"
#include "soc_osal.h"
#include "mbedtls_perf_utils.h"

static uint64_t g_current;
static uint64_t g_timer[MBEDTLS_PERF_TIMER_ID_CNT];

void mbedtls_perf_start(void)
{
    g_current = uapi_tcxo_get_us();
}

void mbedtls_perf_start_with_id(mbedtls_perf_timer_id_e timer_id)
{
    g_timer[timer_id] = uapi_tcxo_get_us();
}

static uint64_t mbedtls_perf_get_cost(void)
{
    uint64_t current_time = uapi_tcxo_get_us();
    uint64_t cost_time = current_time - g_current;
    g_current = current_time;
    return cost_time;
}

static uint64_t mbedtls_perf_get_cost_with_id(mbedtls_perf_timer_id_e timer_id)
{
    uint64_t current_time = uapi_tcxo_get_us();
    uint64_t cost_time = current_time - g_timer[timer_id];
    g_timer[timer_id] = current_time;
    return cost_time;
}

/* aes perf utils. */
typedef struct {
    uint32_t call_times;        /* 接口调用次数. */
    uint64_t total_time;        /* 总耗时. */
    uint64_t total_data_len;    /* 总数据长度. */
} mbedtls_perf_aes_item_t;
static mbedtls_perf_aes_item_t g_aes_perf_item[MBEDTLS_PERF_AES_TYPE_CNT];
/* 统计单接口调用的耗时，与数据量无关. */
void mbedtls_perf_aes_end1(mbedtls_perf_aes_type_e aes_type)
{
    uint64_t cost_time_in_us = mbedtls_perf_get_cost();
    g_aes_perf_item[aes_type].call_times++;
    g_aes_perf_item[aes_type].total_time += cost_time_in_us;
}

/* 统计单接口调用的耗时，与数据量有关. */
void mbedtls_perf_aes_end2(mbedtls_perf_aes_type_e aes_type, size_t data_len)
{
    mbedtls_perf_aes_end1(aes_type);
    g_aes_perf_item[aes_type].total_data_len += data_len;
}

void mbedtls_perf_aes_dump(void)
{
    uint32_t i;
    mbedtls_perf_aes_item_t *item = NULL;
    for (i = 0; i < MBEDTLS_PERF_AES_TYPE_CNT; i++) {
        item = &g_aes_perf_item[i];
        if (item->call_times != 0) {
            if (i < MBEDTLS_PERF_AES_TYPE_CRYPT_SMALL_CHUNK) {
                osal_printk("aes_type is %u, call_times is %u, total_time is %llu us\n",
                    i, item->call_times, item->total_time);
            } else {
                osal_printk("aes_type is %u, total_data_len is %llu, call_times is %u, total_time is %llu us\n",
                    i, item->total_data_len, item->call_times, item->total_time);
            }
        }
    }
}
/* bignum perf utils. */
typedef struct {
    uint32_t call_times;        /* 接口调用次数. */
    uint64_t total_time;        /* 总耗时. */
} mbedtls_perf_bignum_item_t;
typedef struct {
    mbedtls_alt_ecp_len_e bignum_len;
    mbedtls_perf_bignum_type_e bignum_type;
} mbedtls_perf_bignum_table_t;
static mbedtls_perf_bignum_item_t g_bignum_perf_item[MBEDTLS_PERF_BIGNUM_TYPE_CNT];
static mbedtls_perf_bignum_table_t g_bignum_perf_table[] = {
    {MBEDTLS_ALT_ECP_LEN_192, MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_192},
    {MBEDTLS_ALT_ECP_LEN_256, MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_256},
    {MBEDTLS_ALT_ECP_LEN_384, MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_320},
    {MBEDTLS_ALT_ECP_LEN_448, MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_384},
    {MBEDTLS_ALT_ECP_LEN_512, MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_448},
    {MBEDTLS_ALT_ECP_LEN_1024, MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_512},
    {MBEDTLS_ALT_ECP_LEN_1536, MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_1024},
    {MBEDTLS_ALT_ECP_LEN_2048, MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_2048},
    {MBEDTLS_ALT_ECP_LEN_3072, MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_3072},
    {MBEDTLS_ALT_ECP_LEN_4096, MBEDTLS_PERF_BIGNUM_TYPE_EXP_MOD_4096},
};
void mbedtls_perf_bignum_end(mbedtls_alt_ecp_len_e ecp_len)
{
    for (unsigned int i = 0; i < sizeof(g_bignum_perf_table) / sizeof(g_bignum_perf_table[0]); i++) {
        if (ecp_len == g_bignum_perf_table[i].bignum_len) {
            g_bignum_perf_item[g_bignum_perf_table[i].bignum_type].call_times++;
            g_bignum_perf_item[g_bignum_perf_table[i].bignum_type].total_time += mbedtls_perf_get_cost();
            break;
        }
    }
}

void mbedtls_perf_bignum_dump(void)
{
    uint32_t i;
    mbedtls_perf_bignum_item_t *item = NULL;
    for (i = 0; i < MBEDTLS_PERF_BIGNUM_TYPE_CNT; i++) {
        item = &g_bignum_perf_item[i];
        if (item->call_times != 0) {
            osal_printk("bignum_type is %u, call_times is %u, total_time is %llu us\n",
                i, item->call_times, item->total_time);
        }
    }
}

/* ccm perf utils. */
typedef struct {
    uint32_t call_times;        /* 接口调用次数. */
    uint64_t total_time;        /* 总耗时. */
    uint64_t total_data_len;    /* 总数据长度. */
} mbedtls_perf_ccm_item_t;
static mbedtls_perf_ccm_item_t g_ccm_perf_item[MBEDTLS_PERF_CCM_TYPE_CNT];
/* 统计单接口调用的耗时，与数据量无关. */
void mbedtls_perf_ccm_end1(mbedtls_perf_ccm_type_e ccm_type)
{
    g_ccm_perf_item[ccm_type].call_times++;
    g_ccm_perf_item[ccm_type].total_time += mbedtls_perf_get_cost();
}

/* 统计单接口调用的耗时，与数据量有关. */
void mbedtls_perf_ccm_end2(mbedtls_perf_ccm_type_e ccm_type, size_t data_len)
{
    mbedtls_perf_ccm_end1(ccm_type);
    g_ccm_perf_item[ccm_type].total_data_len += data_len;
}

void mbedtls_perf_ccm_dump(void)
{
    uint32_t i;
    mbedtls_perf_ccm_item_t *item = NULL;
    for (i = 0; i < MBEDTLS_PERF_CCM_TYPE_CNT; i++) {
        item = &g_ccm_perf_item[i];
        if (item->call_times != 0) {
            if (i != MBEDTLS_PERF_CCM_TYPE_UPDATE_AD &&
                i != MBEDTLS_PERF_CCM_TYPE_UPDATE) {
                osal_printk("ccm_type is %u, call_times is %u, total_time is %llu us\n",
                    i, item->call_times, item->total_time);
            } else {
                osal_printk("ccm_type is %u, total_data_len is %llu, call_times is %u, total_time is %llu us\n",
                    i, item->total_data_len, item->call_times, item->total_time);
            }
        }
    }
}

/* cmac perf utils. */
typedef struct {
    uint32_t call_times;        /* 接口调用次数. */
    uint64_t total_time;        /* 总耗时. */
    uint64_t total_data_len;    /* 总数据长度. */
} mbedtls_perf_cmac_item_t;
static mbedtls_perf_cmac_item_t g_cmac_perf_item[MBEDTLS_PERF_CMAC_TYPE_CNT];
/* 统计单接口调用的耗时，与数据量无关. */
void mbedtls_perf_cmac_end1(mbedtls_perf_cmac_type_e cmac_type)
{
    uint64_t cost_time_in_us = mbedtls_perf_get_cost();
    g_cmac_perf_item[cmac_type].call_times++;
    g_cmac_perf_item[cmac_type].total_time += cost_time_in_us;
}

/* 统计单接口调用的耗时，与数据量有关. */
void mbedtls_perf_cmac_end2(mbedtls_perf_cmac_type_e cmac_type, size_t data_len)
{
    mbedtls_perf_cmac_end1(cmac_type);
    g_cmac_perf_item[cmac_type].total_data_len += data_len;
}

void mbedtls_perf_cmac_dump(void)
{
    uint32_t i;
    mbedtls_perf_cmac_item_t *item = NULL;
    for (i = 0; i < MBEDTLS_PERF_CMAC_TYPE_CNT; i++) {
        item = &g_cmac_perf_item[i];
        if (item->call_times != 0) {
            if (i < MBEDTLS_PERF_CMAC_TYPE_UPDATE_SMALL_CHUNK) {
                osal_printk("cmac_type is %u, call_times is %u, total_time is %llu us\n",
                    i, item->call_times, item->total_time);
            } else {
                osal_printk("cmac_type is %u, total_data_len is %llu, call_times is %u, total_time is %llu us\n",
                    i, item->total_data_len, item->call_times, item->total_time);
            }
        }
    }
}

/* ecdsa perf utils. */
typedef struct {
    uint32_t call_times;        /* 接口调用次数. */
    uint64_t total_time;        /* 总耗时. */
} mbedtls_perf_ecdsa_item_t;
static mbedtls_perf_ecdsa_item_t g_ecdsa_perf_item[MBEDTLS_ECP_DP_CURVE448][MBEDTLS_PERF_ECDSA_OP_CNT];

void mbedtls_perf_ecdsa_end(mbedtls_ecp_group_id grp_id, mbedtls_perf_ecdsa_op_e ecdsa_op)
{
    if (grp_id == 0) {
        return;
    }
    g_ecdsa_perf_item[grp_id - 1][ecdsa_op].call_times++;
    g_ecdsa_perf_item[grp_id - 1][ecdsa_op].total_time += mbedtls_perf_get_cost();
}

void mbedtls_perf_ecdsa_dump(void)
{
    uint32_t i, j;
    mbedtls_perf_ecdsa_item_t *item = NULL;
    for (i = 0; i < MBEDTLS_ECP_DP_CURVE448; i++) {
        for (j = 0; j < MBEDTLS_PERF_ECDSA_OP_CNT; j++) {
            item = &g_ecdsa_perf_item[i][j];
            if (item->call_times != 0) {
                osal_printk("grp_id is %u, ecdsa_op is %u, call_times is %u, total_time is %llu us\n",
                    i + 1, j, item->call_times, item->total_time);
            }
        }
    }
}

/* ecp perf utils. */
typedef struct {
    uint32_t call_times;        /* 接口调用次数. */
    uint64_t total_time;        /* 总耗时. */
} mbedtls_perf_ecp_item_t;

static mbedtls_perf_ecp_item_t g_ecp_perf_item1[MBEDTLS_PERF_ECP_TYPE1_CNT];
static mbedtls_perf_ecp_item_t g_ecp_perf_item2[MBEDTLS_ECP_DP_CURVE448][MBEDTLS_PERF_ECP_TYPE2_CNT];

/* 统计单接口调用的耗时，与曲线类型无关. */
void mbedtls_perf_ecp_end1_with_id(mbedtls_perf_timer_id_e timer_id,
    mbedtls_perf_ecp_type1_e ecp_type)
{
    g_ecp_perf_item1[ecp_type].call_times++;
    g_ecp_perf_item1[ecp_type].total_time += mbedtls_perf_get_cost_with_id(timer_id);
}

/* 统计单接口调用的耗时，与曲线类型有关. */
void mbedtls_perf_ecp_end2_with_id(mbedtls_perf_timer_id_e timer_id,
    mbedtls_ecp_group_id grp_id,
    mbedtls_perf_ecp_type2_e ecp_type)
{
    if (grp_id == 0) {
        return;
    }
    g_ecp_perf_item2[grp_id - 1][ecp_type].call_times++;
    g_ecp_perf_item2[grp_id - 1][ecp_type].total_time += mbedtls_perf_get_cost_with_id(timer_id);
}

void mbedtls_perf_ecp_dump(void)
{
    uint32_t i, j;
    mbedtls_perf_ecp_item_t *item = NULL;
    for (i = 0; i < MBEDTLS_PERF_ECP_TYPE1_CNT; i++) {
        item = &g_ecp_perf_item1[i];
        if (item->call_times != 0) {
            osal_printk("ecp_type1 is %u, call_times is %u, total_time is %llu us\n",
                i, item->call_times, item->total_time);
        }
    }
    for (i = 0; i < MBEDTLS_ECP_DP_CURVE448; i++) {
        for (j = 0; j < MBEDTLS_PERF_ECP_TYPE2_CNT; j++) {
            item = &g_ecp_perf_item2[i][j];
            if (item->call_times != 0) {
                osal_printk("grp_id is %u, ecp_type2 is %u, call_times is %u, total_time is %llu us\n",
                    i + 1, j, item->call_times, item->total_time);
            }
        }
    }
}

/* gcm perf utils. */
typedef struct {
    uint32_t call_times;        /* 接口调用次数. */
    uint64_t total_time;        /* 总耗时. */
    uint64_t total_data_len;    /* 总数据长度. */
} mbedtls_perf_gcm_item_t;
static mbedtls_perf_gcm_item_t g_gcm_perf_item[MBEDTLS_PERF_GCM_TYPE_CNT];

/* 统计单接口调用的耗时，与数据量无关. */
void mbedtls_perf_gcm_end1(mbedtls_perf_gcm_type_e gcm_type)
{
    g_gcm_perf_item[gcm_type].call_times++;
    g_gcm_perf_item[gcm_type].total_time += mbedtls_perf_get_cost();
}

/* 统计单接口调用的耗时，与数据量有关. */
void mbedtls_perf_gcm_end2(mbedtls_perf_gcm_type_e gcm_type, size_t data_len)
{
    mbedtls_perf_gcm_end1(gcm_type);
    g_gcm_perf_item[gcm_type].total_data_len += data_len;
}

void mbedtls_perf_gcm_dump(void)
{
    uint32_t i;
    mbedtls_perf_gcm_item_t *item = NULL;
    for (i = 0; i < MBEDTLS_PERF_GCM_TYPE_CNT; i++) {
        item = &g_gcm_perf_item[i];
        if (item->call_times != 0) {
            if (i != MBEDTLS_PERF_GCM_TYPE_UPDATE_AD &&
                i != MBEDTLS_PERF_GCM_TYPE_UPDATE) {
                osal_printk("gcm_type is %u, call_times is %u, total_time is %llu us\n",
                    i, item->call_times, item->total_time);
            } else {
                osal_printk("gcm_type is %u, total_data_len is %llu, call_times is %u, total_time is %llu us\n",
                    i, item->total_data_len, item->call_times, item->total_time);
            }
        }
    }
}

/* sha perf utils. */
typedef struct {
    uint32_t call_times;        /* 接口调用次数. */
    uint64_t total_time;        /* 总耗时. */
    uint64_t total_data_len;    /* 总数据长度. */
} mbedtls_perf_sha_item_t;
static mbedtls_perf_sha_item_t g_sha_perf_item[MBEDTLS_PERF_SHA_TYPE_CNT];

/* 统计单接口调用的耗时，与数据量无关. */
void mbedtls_perf_sha_end1(mbedtls_perf_sha_type_e sha_type)
{
    uint64_t cost_time_in_us = mbedtls_perf_get_cost();
    g_sha_perf_item[sha_type].call_times++;
    g_sha_perf_item[sha_type].total_time += cost_time_in_us;
}

/* 统计单接口调用的耗时，与数据量有关. */
void mbedtls_perf_sha_end2(mbedtls_perf_sha_type_e sha_type, size_t data_len)
{
    mbedtls_perf_sha_end1(sha_type);
    g_sha_perf_item[sha_type].total_data_len += data_len;
}

void mbedtls_perf_sha_dump(void)
{
    uint32_t i;
    mbedtls_perf_sha_item_t *item = NULL;
    for (i = 0; i < MBEDTLS_PERF_SHA_TYPE_CNT; i++) {
        item = &g_sha_perf_item[i];
        if (item->call_times != 0) {
            if (i < MBEDTLS_PERF_SHA_TYPE_UPDATE_SMALL_CHUNK) {
                osal_printk("sha_type is %u, call_times is %u, total_time is %llu us\n",
                    i, item->call_times, item->total_time);
            } else {
                osal_printk("sha_type is %u, total_data_len is %llu, call_times is %u, total_time is %llu us\n",
                    i, item->total_data_len, item->call_times, item->total_time);
            }
        }
    }
}

void mbedtls_perf_init(void)
{
    (void)memset_s(g_aes_perf_item, sizeof(g_aes_perf_item), 0, sizeof(g_aes_perf_item));
    (void)memset_s(g_bignum_perf_item, sizeof(g_bignum_perf_item), 0, sizeof(g_bignum_perf_item));
    (void)memset_s(g_ccm_perf_item, sizeof(g_ccm_perf_item), 0, sizeof(g_ccm_perf_item));
    (void)memset_s(g_cmac_perf_item, sizeof(g_cmac_perf_item), 0, sizeof(g_cmac_perf_item));
    (void)memset_s(g_ecdsa_perf_item, sizeof(g_ecdsa_perf_item), 0, sizeof(g_ecdsa_perf_item));
    (void)memset_s(g_gcm_perf_item, sizeof(g_gcm_perf_item), 0, sizeof(g_gcm_perf_item));
    (void)memset_s(g_sha_perf_item, sizeof(g_sha_perf_item), 0, sizeof(g_sha_perf_item));
}
#endif
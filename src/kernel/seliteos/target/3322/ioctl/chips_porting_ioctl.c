/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: chips ioctl implement for SELiteOS.
 */

#include "chips_porting_ioctl.h"
#include "chips_porting.h"
#include "los_base.h"
#include "tee_efuse.h"
#include "los_mem.h"
#include "pmp_mgr.h"
#include "securec.h"

#ifdef CONFIG_TEE_EFUSE_SUPPORT
#include "efuse_porting.h"
#include "hal_efuse.h"
#endif

#define ROOT_PUBLIC_KEY_STRUCTURE_LENGTH 0x80
#define KEY_AREA_STRUCTURE_LENGTH        0x100
#define CODE_INFO_STRUCTURE_LENGTH       0x200
#define PUBLIC_KEY_LEN                   64
#define SIG_LEN                          64
#define EXT_SIG_LEN                      64

#define DIE_ID_LEN                      16      /* DIEID has 160 bits, but we just use 128 bits */
#define HASH_LEN                        32
#define PROTECT_KEY_LEN                 16
#define IV_LEN                          16
#define OEM_NAME_LEN                    28
#define SM2_ID_LEN                      16

/* Key area, size is 0x100 */
typedef struct {
    uint32_t      image_id;
    uint32_t      structure_version;
    uint32_t      structure_length;
    uint32_t      signature_length;
    uint32_t      key_owner_id;
    uint32_t      key_id;
    uint32_t      key_alg;            /* 0x2A13C812: ECC256;  0x2A13C823: SM2 */
    uint32_t      ecc_curve_type;     /* 0x2A13C812: RFC 5639, BrainpoolP256r1 */
    uint32_t      key_length;
    uint32_t      key_version_ext;
    uint32_t      mask_key_version_ext;
    uint32_t      msid_ext;
    uint32_t      mask_msid_ext;
    uint32_t      maintenance_mode;   /* 0x2A13C812: enable */
    uint8_t       die_id[DIE_ID_LEN];
    int32_t       code_info_addr;   /* 0 means followed image Key Area, -xx: up offset, +xx:down offset. */
    uint8_t       reserved[KEY_AREA_STRUCTURE_LENGTH - 76 - PUBLIC_KEY_LEN - SIG_LEN]; /* 76 bytes above */
    uint8_t       ext_public_key[PUBLIC_KEY_LEN];
    uint8_t       sig_key_area[SIG_LEN];
} image_key_area_t;

/* Code area info, size is 0x200 */
typedef struct {
    uint32_t      image_id;
    uint32_t      structure_version;
    uint32_t      structure_length;
    uint32_t      signature_length;
    uint32_t      version_ext;
    uint32_t      mask_version_ext;
    uint32_t      msid_ext;
    uint32_t      mask_msid_ext;
    int32_t       code_area_addr;   /* 0 means followed image code info, -xx: up offset, +xx:down offset. */
    uint32_t      code_area_len;
    uint8_t       code_area_hash[HASH_LEN];
    /* ======members, only meaningful for general image. start====== */
    uint32_t      code_enc_flag;    /* 0x3C7896E1: code area not enc, others: enc */
    uint8_t       protection_key_l1[PROTECT_KEY_LEN];
    uint8_t       protection_key_l2[PROTECT_KEY_LEN];
    uint8_t       iv[IV_LEN];
    uint32_t      code_compress_flag; /* 0x3C7896E1: is compressed */
    uint32_t      code_uncompress_len;
    uint32_t      text_segment_size;
    /* ======members, only meaningful for general image. end====== */
    uint8_t       reserved[CODE_INFO_STRUCTURE_LENGTH - 136 - SIG_LEN - EXT_SIG_LEN];  /* 136 bytes above */
    uint8_t       sig_code_info[SIG_LEN];
    uint8_t       sig_code_info_ext[EXT_SIG_LEN];
} image_code_info_t;

int32_t dispatch_read_tee_version(uint32_t cmd, void *arg)
{
    get_tee_ver_arg_t *local_arg = arg;

    los_chk_return(cmd != CMD_READ_TEE_VER, ERRCODE_INVALID_PARAM, "cmd is invalid!\n");
    los_chk_return(local_arg->version == NULL, ERRCODE_INVALID_PARAM, "key_ver is null!\n");
    los_chk_return(check_is_not_in_tee_range((uintptr_t)local_arg->version, sizeof(uint32_t)) != LOS_SUCCESS,
                   ERRCODE_INVALID_PARAM, "version is tee addr!\n");
    *(local_arg->version) = 0x200; // security tee version

    return ERRCODE_SUCC;
}

int32_t dispatch_read_tee_key_and_code_ver(uint32_t cmd, void *arg)
{
    get_key_code_ver_arg_t *local_arg = arg;

    los_chk_return(cmd != CMD_READ_TEE_KEY_CODE_VER_NR, ERRCODE_INVALID_PARAM, "cmd is invalid!\n");
    los_chk_return(local_arg->key_ver == NULL, ERRCODE_INVALID_PARAM, "key_ver is null!\n");
    los_chk_return(local_arg->code_ver == NULL, ERRCODE_INVALID_PARAM, "code_ver is null!\n");
    los_chk_return(check_is_not_in_tee_range((uintptr_t)local_arg->key_ver, sizeof(uint32_t)) != LOS_SUCCESS,
                   ERRCODE_INVALID_PARAM, "key_ver is tee addr!\n");
    los_chk_return(check_is_not_in_tee_range((uintptr_t)local_arg->code_ver, sizeof(uint32_t)) != LOS_SUCCESS,
                   ERRCODE_INVALID_PARAM, "code_ver is tee addr!\n");

    image_key_area_t *key_area = (image_key_area_t *)(uintptr_t)SELITEOS_FLASH_REGION_START;
    image_code_info_t *code_info = (image_code_info_t *)(uintptr_t)(SELITEOS_FLASH_REGION_START +
        KEY_AREA_STRUCTURE_LENGTH);

    *(local_arg->key_ver) = key_area->key_version_ext;
    *(local_arg->code_ver) = code_info->version_ext;

    return ERRCODE_SUCC;
}

int32_t dispatch_read_ssb_key_and_code_ver(uint32_t cmd, void *arg)
{
    get_key_code_ver_arg_t *local_arg = arg;

    los_chk_return(cmd != CMD_READ_SSB_KEY_CODE_VER_NR, ERRCODE_INVALID_PARAM, "cmd is invalid!\n");
    los_chk_return(local_arg->key_ver == NULL, ERRCODE_INVALID_PARAM, "key_ver is null!\n");
    los_chk_return(local_arg->code_ver == NULL, ERRCODE_INVALID_PARAM, "code_ver is null!\n");
    los_chk_return(check_is_not_in_tee_range((uintptr_t)local_arg->key_ver, sizeof(uint32_t)) != LOS_SUCCESS,
                   ERRCODE_INVALID_PARAM, "key_ver is tee addr!\n");
    los_chk_return(check_is_not_in_tee_range((uintptr_t)local_arg->code_ver, sizeof(uint32_t)) != LOS_SUCCESS,
                   ERRCODE_INVALID_PARAM, "code_ver is tee addr!\n");

    image_key_area_t *key_area = (image_key_area_t *)(uintptr_t)SSB_FLASH_REGION_START;
    image_code_info_t *code_info = (image_code_info_t *)(uintptr_t)(SSB_FLASH_REGION_START +
        KEY_AREA_STRUCTURE_LENGTH);

    *(local_arg->key_ver) = key_area->key_version_ext;
    *(local_arg->code_ver) = code_info->version_ext;

    return ERRCODE_SUCC;
}

#ifdef CONFIG_TEE_EFUSE_SUPPORT
int32_t dispatch_efuse_read_byte(uint32_t cmd, void *arg)
{
    int32_t ret = ERRCODE_SUCC;
    efuse_read_byte_arg_t *local_arg = arg;
    uint8_t value = 0;

    los_chk_return(cmd != CMD_EFUSE_READ_BYTE, ERRCODE_INVALID_PARAM, "cmd is invalid!\n");

    hal_efuse_funcs_t *hal_funcs = hal_efuse_get_funcs();
    ret = hal_funcs->read_byte(local_arg->byte_address, &value);
    los_chk_return(ret != ERRCODE_SUCC, ret, "hal_funcs->read_byte failed!\n");

    local_arg->value = value;
    return ret;
}

static const efuse_range_t g_forbid_efuse_addr_range[] = {
    {.efuse_addr = EFUSE_LIFECYCLE_ADDR, .efuse_addr_len = EFUSE_LIFECYCLE_ADDR_LEN},
    {.efuse_addr = FLASHBOOT_VER_ADDR, .efuse_addr_len = FLASHBOOT_VER_ADDR_LEN},
    {.efuse_addr = EFUSE_TEE_VER_ADDR, .efuse_addr_len = EFUSE_TEE_VER_ADDR_LEN},
};

static bool check_efuse_address(uint32_t byte_address, uint32_t write_len)
{
    for (uint32_t i = 0; i < sizeof(g_forbid_efuse_addr_range) / sizeof(g_forbid_efuse_addr_range[0]); ++i) {
        if (byte_address + write_len > g_forbid_efuse_addr_range[i].efuse_addr &&
            byte_address < g_forbid_efuse_addr_range[i].efuse_addr + g_forbid_efuse_addr_range[i].efuse_addr_len) {
            return false;
        }
    }

    return true;
}

int32_t dispatch_efuse_write_op(uint32_t cmd, void *arg)
{
    int32_t ret = ERRCODE_SUCC;
    efuse_write_op_arg_t *local_arg = arg;

    los_chk_return(cmd != CMD_EFUSE_WRITE_OP, ERRCODE_INVALID_PARAM, "cmd is invalid!\n");
    if (local_arg->byte_address == EFUSE_LIFECYCLE_ADDR) {
        los_chk_return(((local_arg->value & 0x20) != 0), ERRCODE_INVALID_PARAM, "cannot change lifecycle to rma!\n");
    } else if (local_arg->byte_address == FLASHBOOT_VER_ADDR) {
        los_chk_return(((local_arg->value & 0xF0) != 0), ERRCODE_INVALID_PARAM, "cannot change flashboot_ver!\n");
    } else if (local_arg->byte_address == FLASHBOOT_VER_ADDR + 1) {
        los_chk_return(((local_arg->value & 0x0F) != 0), ERRCODE_INVALID_PARAM, "cannot change flashboot_ver!\n");
    } else {
        los_chk_return(!check_efuse_address(local_arg->byte_address, 1), ERRCODE_INVALID_PARAM,
            "byte_address is invalid!\n");
    }

    hal_efuse_funcs_t *hal_funcs = hal_efuse_get_funcs();
    ret = hal_funcs->write_op(local_arg->byte_address, local_arg->value, local_arg->region);
    los_chk_return(ret != ERRCODE_SUCC, ret, "hal_funcs->write_op failed!\n");

    return ret;
}

int32_t dispatch_efuse_write_buffer_op(uint32_t cmd, void *arg)
{
    int32_t ret = ERRCODE_SUCC;
    efuse_write_buffer_op_arg_t *local_arg = arg;
    uint8_t *data = NULL;

    los_chk_return(cmd != CMD_EFUSE_WRITE_BUFFER_OP, ERRCODE_INVALID_PARAM, "cmd is invalid!\n");
    los_chk_return(local_arg->buffer == NULL, ERRCODE_INVALID_PARAM, "buffer is null!\n");
    los_chk_return(check_is_not_in_tee_range((uintptr_t)local_arg->buffer, local_arg->length) != LOS_SUCCESS,
                   ERRCODE_INVALID_PARAM, "buffer is tee addr!\n");
    los_chk_return(local_arg->length > EFUSE_REGION_MAX_BYTES, ERRCODE_INVALID_PARAM, "invalid write length!\n");
    los_chk_return(!check_efuse_address(local_arg->address, local_arg->length), ERRCODE_INVALID_PARAM,
                   "byte_address is invalid!\n");

    data = los_kmalloc(local_arg->length);
    los_chk_return(data == NULL, ERRCODE_MALLOC, "los_kmalloc failed!\n");

    ret = memcpy_s(data, local_arg->length, local_arg->buffer, local_arg->length);
    los_chk_goto(ret != EOK, free_data, "memcpy_s failed!\n");

    hal_efuse_funcs_t *hal_funcs = hal_efuse_get_funcs();
    ret = hal_funcs->write_buffer_op(local_arg->address, data, local_arg->length);
    los_chk_goto(ret != ERRCODE_SUCC, free_data, "hal_funcs->write_buffer_op failed!\n");
free_data:
    (void)memset_s(data, local_arg->length, 0, local_arg->length);
    los_kfree(data);
    return ret;
}

int32_t dispatch_efuse_get_die_id(uint32_t cmd, void *arg)
{
    int32_t ret = ERRCODE_SUCC;
    efuse_get_die_id_arg_t *local_arg = arg;

    los_chk_return(cmd != CMD_EFUSE_GET_DIE_ID, ERRCODE_INVALID_PARAM, "cmd is invalid!\n");
    los_chk_return(local_arg->buffer == NULL, ERRCODE_INVALID_PARAM, "buffer is null!\n");
    los_chk_return(check_is_not_in_tee_range((uintptr_t)local_arg->buffer, local_arg->length) != LOS_SUCCESS,
                   ERRCODE_INVALID_PARAM, "buffer is tee addr!\n");

    ret = tee_efuse_get_die_id(local_arg->buffer, local_arg->length);
    los_chk_return(ret != ERRCODE_SUCC, ret, "get_die_id failed!\n");

    return ret;
}

static const tesvec_ioctl_cmd_t g_chips_porting_ioctl_dispatch_func[] = {
    {CMD_EFUSE_READ_BYTE, dispatch_efuse_read_byte},
    {CMD_EFUSE_WRITE_OP, dispatch_efuse_write_op},
    {CMD_EFUSE_WRITE_BUFFER_OP, dispatch_efuse_write_buffer_op},
    {CMD_EFUSE_GET_DIE_ID, dispatch_efuse_get_die_id},
    {CMD_READ_TEE_VER, dispatch_read_tee_version},
    {CMD_READ_TEE_KEY_CODE_VER_NR, dispatch_read_tee_key_and_code_ver},
    {CMD_READ_SSB_KEY_CODE_VER_NR, dispatch_read_ssb_key_and_code_ver},
};
#else
static const tesvec_ioctl_cmd_t g_chips_porting_ioctl_dispatch_func[] = {
    {CMD_EFUSE_READ_BYTE, NULL},
    {CMD_EFUSE_WRITE_OP, NULL},
    {CMD_EFUSE_WRITE_BUFFER_OP, NULL},
    {CMD_EFUSE_GET_DIE_ID, NULL},
    {CMD_READ_TEE_VER, dispatch_read_tee_version},
    {CMD_READ_TEE_KEY_CODE_VER_NR, dispatch_read_tee_key_and_code_ver},
    {CMD_READ_SSB_KEY_CODE_VER_NR, dispatch_read_ssb_key_and_code_ver},
};
#endif

const tesvec_ioctl_cmd_t *get_chips_porting_dispatch_func_list(void)
{
    return g_chips_porting_ioctl_dispatch_func;
}

uint32_t get_chips_porting_max_nr(void)
{
    return CHIPS_PORTING_IOC_MAX_NR;
}

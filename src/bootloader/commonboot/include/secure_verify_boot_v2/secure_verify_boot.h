/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Secure Verify for Loaderboot and Flashboot
 *
 * Create: 2023-03-09
 */

#ifndef SECURE_VERIFY_BOOT_H
#define SECURE_VERIFY_BOOT_H

#include <stdint.h>
#include "boot_errcode.h"
#include "boot_serial.h"
#include "efuse_porting.h"

#if defined(CONFIG_BOOT_SUPPORT_ECC_VERIFY) || defined(CONFIG_BOOT_SUPPORT_SM2_VERIFY)  || \
    defined(CONFIG_BOOT_NO_SEC_VERIFY)
#define ROOT_PUBLIC_KEY_STRUCTURE_LENGTH 0x80
#define KEY_AREA_STRUCTURE_LENGTH        0x100
#define CODE_INFO_STRUCTURE_LENGTH       0x200
#define PUBLIC_KEY_LEN                   64
#define SIG_LEN                          64
#define EXT_SIG_LEN                      64
#elif defined(CONFIG_BOOT_SUPPORT_RSA3072_VERIFY)
#define ROOT_PUBLIC_KEY_STRUCTURE_LENGTH            0x200
#define KEY_AREA_STRUCTURE_LENGTH                   0x400
#define CODE_INFO_STRUCTURE_LENGTH                  0x400
#define PUBLIC_KEY_LEN                              388    /* n:384 bytes, e:4 bytes */
#define SIG_LEN                                     384
#define EXT_SIG_LEN                                 384
#elif defined(CONFIG_BOOT_SUPPORT_RSA4096_VERIFY)
#define ROOT_PUBLIC_KEY_STRUCTURE_LENGTH            0x400
#define KEY_AREA_STRUCTURE_LENGTH                   0x500
#define CODE_INFO_STRUCTURE_LENGTH                  0x300
#define PUBLIC_KEY_LEN                         516    /* n:512 bytes, e:4 bytes */
#define SIG_LEN                                512
#define EXT_SIG_LEN                            0      /* No external signatures are reserved for RSA4096 */
#endif

#define DIE_ID_LEN                      16      /* DIEID has 160 bits, but we just use 128 bits */
#define HASH_LEN                        32
#define PROTECT_KEY_LEN                 16
#define IV_LEN                          16
#define OEM_NAME_LEN                    28
#define SM2_ID_LEN                      16

#define ROOT_KEY_NOT_VERIFIED   0xa
#define ROOT_KEY_VERIFIED       0x5
#define NOT_CHECK_VERSION   0xFFFFFFFF

#define TRUST_CHAIN_ROM         0x0
#define TRUST_CHAIN_OEM         0x1
#define TRUST_CHAIN_MODULE      0x2
#define TRUST_CHAIN_TEE         0x3
#define TRUST_CHAIN_MASK        0x0000FF00
#define TRUST_CHAIN_SHIFT       8

#define TRUST_IMAGE_LEVEL_ROOT         0x0
#define TRUST_IMAGE_LEVEL_SUB          0x1
#define TRUST_KEY_LEVEL_MASK           0x00FF0000
#define TRUST_KEY_LEVEL_SHIFT          16

#define BASE_IMAGE_TYPE_LOADER_BOOT              0x0
#define BASE_IMAGE_TYPE_PARAMS                   0x1
#define BASE_IMAGE_TYPE_REE_BOOT                 0x2
#define BASE_IMAGE_TYPE_RECOVERY                 0x3
#define BASE_IMAGE_TYPE_MCU                      0x4
#define BASE_IMAGE_TYPE_BT                       0x5
#define BASE_IMAGE_TYPE_DSP                      0x6
#define BASE_IMAGE_TYPE_CFCT                     0x7
#define BASE_IMAGE_TYPE_TEE_BOOT                 0x8
#define BASE_IMAGE_TYPE_TEE_RUNTIME_FW           0X9
#define BASE_IMAGE_TYPE_DEBUG_CERT               0XA
#define BASE_IMAGE_TYPE_DSP1                     0xB
#define BASE_IMAGE_TYPE_SENSOR                   0xC
#define BASE_IMAGE_TYPE_GNSS                     0xD
#define BASE_IMAGE_TYPE_WIFI0                    0xE
#define BASE_IMAGE_TYPE_WIFI1                    0xF
#define BASE_IMAGE_TYPE_WIFI2                    0x10
#define BASE_IMAGE_TYPE_WIFI3                    0x11
#define BASE_IMAGE_TYPE_UPG_FLASHBOOT            0x12
#define BASE_IMAGE_TYPE_UPG_TEE_FW               0x13
#define BASE_IMAGE_TYPE_SELITEOS                 0x14
#define BASE_IMAGE_TYPE_WIFI                     0x15
#define BASE_IMAGE_TYPE_CCORE                    0x16
#define BASE_IMAGE_TYPE_BTOFF                    0x17
#define BASE_IMAGE_TYPE_MASK                     0x000000FF
#define BASE_IMAGE_TYPE_SHIFT                    0

#define compat_image_type(key_level, trust_chain, base_image_type) \
    ((((key_level) & 0xFF) << TRUST_KEY_LEVEL_SHIFT) | (((trust_chain) & 0xFF) << TRUST_CHAIN_SHIFT) | \
        ((base_image_type) & 0xFF)) \

#define get_key_level(image_type) \
    (((image_type) & TRUST_KEY_LEVEL_MASK) >> TRUST_KEY_LEVEL_SHIFT) \

#define get_image_owner(image_type) \
    (((image_type) & TRUST_CHAIN_MASK) >> TRUST_CHAIN_SHIFT) \

#define get_base_image_type(image_type) \
    ((image_type) & BASE_IMAGE_TYPE_MASK) \

typedef enum {
    OEM_LOADER_BOOT_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_ROOT, TRUST_CHAIN_OEM, BASE_IMAGE_TYPE_LOADER_BOOT),
    OEM_PARAMS_BOOT_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_SUB, TRUST_CHAIN_OEM, BASE_IMAGE_TYPE_PARAMS),
    OEM_FLASH_BOOT_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_ROOT, TRUST_CHAIN_OEM, BASE_IMAGE_TYPE_REE_BOOT),
    OEM_FLASH_RECOVERY_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_SUB, TRUST_CHAIN_OEM, BASE_IMAGE_TYPE_RECOVERY),
    OEM_SELTEOS_BOOT_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_SUB, TRUST_CHAIN_OEM, BASE_IMAGE_TYPE_SELITEOS),
    OEM_MCU_BOOT_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_SUB, TRUST_CHAIN_OEM, BASE_IMAGE_TYPE_MCU),
    MODULE_PARAMS_BOOT_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_ROOT, TRUST_CHAIN_MODULE, BASE_IMAGE_TYPE_PARAMS),
    MODULE_MCU_BOOT_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_ROOT, TRUST_CHAIN_MODULE, BASE_IMAGE_TYPE_MCU),
    ROM_CFCT_BOOT_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_SUB, TRUST_CHAIN_ROM, BASE_IMAGE_TYPE_CFCT),
    TEE_LOADER_BOOT_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_ROOT, TRUST_CHAIN_TEE, BASE_IMAGE_TYPE_LOADER_BOOT),
    TEE_FLASH_BOOT_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_ROOT, TRUST_CHAIN_TEE, BASE_IMAGE_TYPE_TEE_BOOT),
    TEE_RUNTIME_FW_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_SUB, TRUST_CHAIN_TEE, BASE_IMAGE_TYPE_TEE_RUNTIME_FW),
    OEM_DEBUG_CERT_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_SUB, TRUST_CHAIN_OEM, BASE_IMAGE_TYPE_DEBUG_CERT),
    OEM_BT_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_SUB, TRUST_CHAIN_OEM, BASE_IMAGE_TYPE_BT),
    OEM_DSP0_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_SUB, TRUST_CHAIN_OEM, BASE_IMAGE_TYPE_DSP),
    OEM_DSP1_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_SUB, TRUST_CHAIN_OEM, BASE_IMAGE_TYPE_DSP1),
    OEM_MCU_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_ROOT, TRUST_CHAIN_OEM, BASE_IMAGE_TYPE_MCU),
    ROM_BT_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_SUB, TRUST_CHAIN_ROM, BASE_IMAGE_TYPE_BT),
    ROM_DSP_FW_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_SUB, TRUST_CHAIN_ROM, BASE_IMAGE_TYPE_DSP),
    ROM_DSP_LIB_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_SUB, TRUST_CHAIN_ROM, BASE_IMAGE_TYPE_DSP),
    OEM_TEE_FIRMWARE_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_SUB, TRUST_CHAIN_OEM, BASE_IMAGE_TYPE_TEE_RUNTIME_FW),
    OEM_FLASH_BOOT_UPG_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_ROOT, TRUST_CHAIN_OEM, BASE_IMAGE_TYPE_UPG_FLASHBOOT),
    OEM_TEE_FIRMWARE_UPG_TYPE = compat_image_type(TRUST_IMAGE_LEVEL_SUB, TRUST_CHAIN_OEM, BASE_IMAGE_TYPE_UPG_TEE_FW),
    INVALID_BOOT_TYPE = 0xFFFFFFFF,
} image_type_t;

typedef enum {
    KEY_EREA_TYPE = 0,
    CODE_INFO_TYPE = 1,
    CFCT_KEY_AREA_TYPE = 4,
    CFCT_TABLE_TYPE = 5,
    INVALID_EREA_TYPE = 0xFFFFFFFF,
} area_type_t;

/**
 * @if Eng
 * @brief  Flash online decryption mode, determining the mode used after the key is sent.
 * @else
 * @brief  Flash 在线解密模式，决定送 key 后使用哪种模式
 * @endif
 */
typedef enum {
    FLASH_KEY_TYPE_REE_DEC = 0x00,  /* REE flash online decryption key */
    FLASH_KEY_TYPE_TEE_DEC,         /* TEE flash online decryption key */
    FLASH_KEY_TYPE_TEE_AUT,         /* TEE flash online authentication key */
    FLASH_KEY_TYPE_INVALID,
} flash_key_type_t;

/* root public key area, size is 0x80 */
typedef struct {
    uint32_t      image_id;
    uint32_t      structure_version;  /* currently version is 0x00010000 */
    uint32_t      structure_length;
    uint32_t      key_owner_id;
    uint32_t      key_id;
    uint32_t      key_alg;            /* 0x2A13C812: ECC256;  0x2A13C823: SM2 */
    uint32_t      ecc_curve_type;     /* 0x2A13C812: RFC 5639, BrainpoolP256r1 */
    uint32_t      key_length;
    uint8_t       reserved[ROOT_PUBLIC_KEY_STRUCTURE_LENGTH - 32 - PUBLIC_KEY_LEN]; /* 32 bytes above */
    uint8_t       root_public_key[PUBLIC_KEY_LEN];
} root_public_key_area_t;

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
    uint32_t      cfct_key_version_ext;
    uint32_t      mask_cfct_key_version_ext;
    uint32_t      msid_ext;
    uint32_t      mask_msid_ext;
    uint32_t      maintenance_mode;   /* 0x2A13C812: enable */
    uint8_t       die_id[DIE_ID_LEN];   /* chip DIE_ID low 128bits, if not enough, padding 0 on high bits. */
    int32_t       cfct_addr;   /* 0 means followed CFCT Key Area.-xx:up offset, +xx:down offset. */
    uint8_t       rom_boot_pub_key_sel_ext;
    uint8_t       reserved[KEY_AREA_STRUCTURE_LENGTH - 77 - PUBLIC_KEY_LEN - SIG_LEN]; /* 77 bytes above */
    uint8_t       ext_public_key[PUBLIC_KEY_LEN];
    uint8_t       sig_key_area[SIG_LEN];
} cfct_key_area_t;

typedef struct {
    uint32_t      image_id;
    uint32_t      structure_version;
    uint32_t      structure_length;
    uint32_t      signature_length;
    uint8_t       reverved_0[12]; /* 12 bytes reverved */
    uint32_t      authorization_mode;
    uint8_t       auth_id[DIE_ID_LEN];
    uint32_t      oem_id;
    uint8_t       oem_name[OEM_NAME_LEN];
    uint8_t       reverved_1[48]; /* 48 bytes reverved */
    uint32_t      data_length;
} cfct_table_info_t;

typedef struct {
    image_type_t image_type; /* 镜像类型,分为Loaderboot、Flashboot、Appboot等镜像,bootram->Loaderboot,firmware->Appboot */
    uint32_t public_key_addr; /* bootram验签为根公钥的起始地址, firmware验签为bootram key_area密钥起始地址 */
    uint32_t key_area_addr; /* 镜像头的起始地址 */
} verify_info_t;

typedef struct {
    image_type_t image_type;
    area_type_t erea_type;
    uint32_t image_id;
#ifdef CONFIG_BOOT_SUPPORT_CHECK_VERSION
    efuse_idx efuse_type;
#endif
} verify_table_item_t;

typedef errcode_t (*secure_authenticate_func)(const uint8_t *key, const uint8_t *data, uint32_t data_length,
    const uint8_t *sign_buff, uint32_t sign_length);

typedef errcode_t (*rom_hash_func)(const unsigned char *data, unsigned int data_length, unsigned char *hash,
    unsigned int hash_length);

typedef errcode_t (*rom_efuse_func)(efuse_idx efuse_id, uint8_t *data, uint16_t data_len);

void register_verify_info(const verify_table_item_t *verify_table, uint32_t verify_table_len,
    secure_authenticate_func authenticate_func, rom_hash_func hash_func, rom_efuse_func efuse_func);

/* only OEM/MODULE root_public_key need to check hash. */
errcode_t verify_public_rootkey(uint32_t rootkey_buff_addr, uint32_t trust_chain_type);

errcode_t verify_cfct_key_area(cfct_key_area_t *key_area);

errcode_t verify_cfct_table_area(cfct_table_info_t *table_info, const uint8_t *public_key);

/* standard image. */
errcode_t verify_image_key_area(image_type_t image_type,
    const image_key_area_t *key_area, const uint8_t *public_key);

errcode_t verify_image_code_info(image_type_t image_type,
    const image_code_info_t *code_info, const uint8_t *public_key);

errcode_t verify_image_code_area(const image_code_info_t *code_info, const uint8_t *code_addr);

errcode_t flash_online_crypto_config(uint32_t image_addr, uint32_t image_size, flash_key_type_t flash_key_type);

#ifdef CONFIG_SECURE_VERIFY_BOOT_DEBUG_ON
void secure_verify_boot_debug_messege(const char *fmt, ...);
void secure_verify_dump_data(const char *name, const uint8_t *data, uint32_t data_len);
#define secure_verify_boot_print secure_verify_boot_debug_messege
#define secure_verify_boot_dump_data secure_verify_dump_data
#else
#define secure_verify_boot_print(fmt, ...)
#define secure_verify_boot_dump_data(name, data, data_len)
#endif
#define secure_verify_boot_msg1 boot_msg1

#endif

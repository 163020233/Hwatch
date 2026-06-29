/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Secure Verify for Loaderboot and Flashboot
 * Author: HiSilicon
 * Create: 2023-03-09
 */

#ifndef SECURE_VERIFY_BOOT_CHECK_H
#define SECURE_VERIFY_BOOT_CHECK_H

#include "secure_verify_boot.h"

errcode_t check_verify_enable(uint32_t image_owner);

errcode_t init_rootkey_status(uint32_t trust_chain_type);

errcode_t update_rootkey_status(uint32_t trust_chain_type);

errcode_t check_rootkey_status(image_type_t image_type);

errcode_t check_rootkey_hash(uint32_t trust_chain_type, const root_public_key_area_t *rootkey_buff);

errcode_t check_image_id(image_type_t image_type, area_type_t erea_type, uint32_t image_id);

errcode_t check_version(image_type_t image_type, uint32_t version, uint32_t version_mask);

errcode_t check_msid(uint32_t msid, uint32_t msid_mask);

errcode_t check_rootkey_id(uint32_t trust_chain_type, uint32_t image_id);

errcode_t check_die_id(uint8_t *die_id, uint32_t die_id_length);

errcode_t secure_authenticate(const uint8_t *key, const uint8_t *data, uint32_t data_length,
    const uint8_t *sign_buff, uint32_t sign_length);

errcode_t rom_hash(const unsigned char *data, unsigned int data_length, unsigned char *hash,
    unsigned int hash_length);

errcode_t rom_efuse(efuse_idx efuse_id, uint8_t *data, uint16_t data_len);

#endif

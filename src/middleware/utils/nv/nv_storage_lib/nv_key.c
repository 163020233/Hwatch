/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: KV Storage Library key access module implementation
 */

#include "nv_key.h"
#include "string.h"
#include "nv_porting.h"
#include "uapi_crc.h"
#include "nv_nvregion.h"
#include "common_def.h"
#if defined(CONFIG_NV_SUPPORT_KEY_INDEX_MAP) && (CONFIG_NV_SUPPORT_KEY_INDEX_MAP == NV_YES)
#include "nv_hash.h"
#endif

#define KV_CLEARTEXT_PADDING_SIZE       (1UL << 2)   /* Currently these both need to be a power of 2... */
#define KV_CIPHERTEXT_PADDING_SIZE      (1UL << 4)   /* ...due to implementation of kv_padded_length    */
#define kv_padded_length(d, s)          (((d) + ((s) - 1)) & ~((s) - 1))
#define kv_padded_cleartext_length(d)   kv_padded_length(d, KV_CLEARTEXT_PADDING_SIZE)
#define kv_padded_ciphertext_length(d)  kv_padded_length(d, KV_CIPHERTEXT_PADDING_SIZE)
#define KV_COPY_SIZE_ALIGN          32

uint8_t g_nv_header_magic = KV_KEY_MAGIC;
/* 0xFF is initial value */
static const kv_key_header_t g_header_cmp = {0xFF, 0xFF, 0xFFFF, 0xFF, 0xFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFFFFFF};

/* IOT-16672 Use ROM Lib function instead (enhanced to support KV requirements, as here) */
errcode_t kv_key_helper_copy_flash(uint32_t dest_location, uint32_t src_location, uint16_t length)
{
    if (length == 0) {
        return ERRCODE_NV_ZERO_LENGTH_COPY;
    }
    uint32_t src_addr = src_location - FLASH_PHYSICAL_ADDR_START;
    uint16_t length_rounded = length & ~(KV_COPY_SIZE_ALIGN - 1);
    uint16_t length_remainder = length & (KV_COPY_SIZE_ALIGN - 1);
    uint16_t dest_offset = 0;
    while (length_rounded > 0) {
        if (kv_flash_read((uint32_t)(uintptr_t)src_addr, KV_COPY_SIZE_ALIGN,
            (uint8_t *)(uintptr_t)(dest_location + dest_offset)) != ERRCODE_SUCC) {
            return ERRCODE_NV_READ_FLASH_ERR;
        }
        dest_offset += KV_COPY_SIZE_ALIGN;
        src_addr += KV_COPY_SIZE_ALIGN;
        length_rounded -= KV_COPY_SIZE_ALIGN;
    }

    if (length_remainder > 0) {
        if (kv_flash_read((uint32_t)(uintptr_t)src_addr, length_remainder,
            (uint8_t *)(uintptr_t)(dest_location + dest_offset)) != ERRCODE_SUCC) {
            return ERRCODE_NV_READ_FLASH_ERR;
        }
    }
    return ERRCODE_SUCC;
}

errcode_t kv_key_write_flash(uint32_t dest, uint32_t length, const uint8_t *src)
{
    uint32_t dest_w = dest;
    if ((dest >= FLASH_PHYSICAL_ADDR_START) && ((dest + length) <= FLASH_PHYSICAL_ADDR_END)) {
        dest_w -= FLASH_PHYSICAL_ADDR_START;
    } else {
        return ERRCODE_FLASH_INVALID_PARAM_BEYOND_ADDR;
    }

    if (kv_flash_write(dest_w, length, src, false) != ERRCODE_SUCC) {
        return ERRCODE_NV_READ_FLASH_ERR;
    }

    return ERRCODE_SUCC;
}

errcode_t kv_key_erase_flash(uint32_t dest, const uint32_t size)
{
    uint32_t dest_w = dest;
    if ((dest >= FLASH_PHYSICAL_ADDR_START) && ((dest + size) <= FLASH_PHYSICAL_ADDR_END)) {
        dest_w -= FLASH_PHYSICAL_ADDR_START;
    } else {
        return ERRCODE_FLASH_INVALID_PARAM_BEYOND_ADDR;
    }

    if (kv_flash_erase(dest_w, size) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

STATIC errcode_t kv_key_read_data_from_flash(uintptr_t dest, const uintptr_t src, uint32_t length,
    uint32_t crypto_handle)
{
    unused(crypto_handle);
    errcode_t ret;
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    /* 加密NV需要保证length 16字节对齐 */
    if (crypto_handle != INVAILD_CRYPTO_HANDLE &&
        kv_key_padded_data_length(KV_ATTRIBUTE_ENCRYPTED, (uint16_t)length) != length) {
        return ERRCODE_NV_INVALID_PARAM;
    }
#endif

    ret = kv_key_helper_copy_flash((uint32_t)dest, (uint32_t)src, (uint16_t)length);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    if (crypto_handle != INVAILD_CRYPTO_HANDLE) {
        ret = nv_crypto_decode(crypto_handle, dest, dest, length);
    }
#endif
    return ret;
}

STATIC errcode_t kv_helper_compare_key_data_chunks(const kv_key_handle_t *RESTRICT key, const uint8_t *RESTRICT compare_data,
                                                   kv_helper_compare_key_data_info_t *RESTRICT info)
{
    errcode_t res = ERRCODE_SUCC;

    uintptr_t key_data_location = (uintptr_t)key->key_location + (uintptr_t)sizeof(kv_key_header_t);
    uint32_t key_data_offset = 0;
    while (key_data_offset < key->header.length) {
        const uint16_t chunk_len = (uint16_t)uapi_min(NV_KEY_DATA_CHUNK_LEN, key->header.length - key_data_offset);
        res = kv_key_helper_copy_flash((uintptr_t)info->key_data_chunk, (key_data_location + key_data_offset),
                                       chunk_len);
        if (res != ERRCODE_SUCC) {
            break;
        }

        uint32_t compare_length = uapi_min(chunk_len, key->header.length - key_data_offset);
        if (info->compare_data_chunk != NULL) {
            /* Compare data in flash */
            res = kv_key_helper_copy_flash((uint32_t)(uintptr_t)info->compare_data_chunk,
                                           (uint32_t)(uintptr_t)compare_data + key_data_offset,
                                           (uint16_t)compare_length);
            if (res != ERRCODE_SUCC) {
                break;
            }
            if (memcmp(info->key_data_chunk, info->compare_data_chunk, compare_length) != 0) {
                return ERRCODE_NV_DATA_MISMATCH;
            }
        } else {
            /* Compare data in RAM */
            if (memcmp(info->key_data_chunk, compare_data + key_data_offset, compare_length) != 0) {
                return ERRCODE_NV_DATA_MISMATCH;
            }
        }
        key_data_offset += chunk_len;
    }

    return res;
}

errcode_t kv_helper_compare_key_data(kv_key_handle_t *RESTRICT key, const uint8_t *RESTRICT compare_data, uint16_t compare_length)
{
    kv_helper_compare_key_data_info_t info = {INVAILD_CRYPTO_HANDLE, NULL, NULL};
    errcode_t res;

    if (key->header.length != compare_length) {
        return ERRCODE_NV_LENGTH_MISMATCH;
    }

    /* Key data *will* be in flash and could be encrypted */
    info.key_data_chunk = (uint8_t *)kv_malloc(NV_KEY_DATA_CHUNK_LEN);
    if (info.key_data_chunk == NULL) {
        return ERRCODE_MALLOC;
    }

    /* Compare data *could* be in flash and will not be encrypted */
    if (
#ifdef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
        ((uintptr_t)(compare_data + compare_length) >= FLASH_PHYSICAL_ADDR_START) &&
#endif
        (uintptr_t)(compare_data + compare_length) <= FLASH_PHYSICAL_ADDR_END)  {
        info.compare_data_chunk = (uint8_t *)kv_malloc(NV_KEY_DATA_CHUNK_LEN);
        if (info.compare_data_chunk == NULL) {
            res = ERRCODE_MALLOC;
            /* Ensure there is not a memory leak */
            goto ret_free;
        }
    }

    res = kv_helper_compare_key_data_chunks(key, compare_data, &info);

    kv_free(info.compare_data_chunk);
ret_free:
    kv_free(info.key_data_chunk);
    return res;
}

errcode_t kv_key_set_invalid(const kv_key_handle_t *key)
{
    /* 如果key还是初始化时的状态，说明NV区域没有这个key，不需要将原来的key设置为无效, 返回成功不影响后续流程 */
    if (key->key_location == 0) {
        return ERRCODE_SUCC;
    }

    /* 计算valid相对于key起始的位置，即要写入数据在flash中相对于key_location的偏移量 */
    uint32_t offset = ((uint32_t)(uintptr_t)&key->header.valid) - ((uint32_t)(uintptr_t)key);
    uint32_t write_position = (uint32_t)(uintptr_t)key->key_location + offset - FLASH_PHYSICAL_ADDR_START;
    uint32_t write_length;
    uint16_t write_data = 0;
    if (g_nv_header_magic == KV_KEY_MAGIC) {
        write_length = (uint32_t)sizeof(uint8_t);
    } else {
        write_length = (uint32_t)sizeof(uint16_t);
    }

    errcode_t ret = kv_flash_write(write_position, write_length, (uint8_t*)&write_data, false);
#if defined(CONFIG_NV_SUPPORT_KEY_INDEX_MAP) && (CONFIG_NV_SUPPORT_KEY_INDEX_MAP == NV_YES)
    if (ret == ERRCODE_SUCC) {
        kv_store_t store;
        (void)kv_nvregion_get_store_from_location(key->key_location, &store);
        kv_key_handle_t delete_key;
        if (nv_hash_key_find(store, key->header.key_id, &delete_key) == ERRCODE_SUCC) {
            if (key->key_location == delete_key.key_location) {
                nv_hash_key_erase(store, key->header.key_id);
                osal_printk("[NV] key erase. key id = 0x%x key->key_location = 0x%x.\r\n",
                    key->header.key_id, key->key_location);
            }
        }
    }
#endif
    return ret;
}

bool kv_key_is_erased(const kv_key_handle_t *key)
{
    if ((key->header.magic != g_nv_header_magic) ||
        (key->header.valid != KV_KEY_VALID) ||
        (key->header.length == 0xFFFF) ||
        (key->header.length == 0)) {
        return true;
    }

    return false;
}

bool kv_key_is_valid(kv_key_handle_t *key)
{
    bool res = kv_key_is_erased(key);
    if (res == true) {
        return false;
    }

    return (kv_key_validate_integrity(key, false) == ERRCODE_SUCC);
}

/**
 * Attempt to match pertinent bits in a key with a search_pattern
 * @param key_handle      Handle of a valid key
 * @param search_pattern  The pattern of bits to match in the key
 * @param search_mask     A mask to select the pertinent bits in the key for matching
 * @return                true if pattern matches, false otherwise
 */
static errcode_t kv_key_does_pattern_match(const kv_key_handle_t *key, kv_key_id search_pattern, kv_key_id search_mask)
{
    /* Match pertinent bits in the key with the search_pattern */
    if ((key->header.key_id & search_mask) != search_pattern) {
        return ERRCODE_NV_SEARCH_PATTERN_MISMATCH;
    }
    return ERRCODE_SUCC;
}

errcode_t kv_key_does_filter_match(kv_key_handle_t *RESTRICT key, kv_key_filter_t *RESTRICT search_filter)
{
    /* If no search filter is given then match every key */
    if (search_filter != NULL) {
        errcode_t res;
        res = kv_key_does_pattern_match(key, search_filter->pattern, search_filter->mask);
        if (res != ERRCODE_SUCC) {
            return res;
        }

        if (((uint32_t)search_filter->type & (uint32_t)KV_KEY_FILTER_TYPE_ANY) != (uint32_t)KV_KEY_FILTER_TYPE_ANY) {
            /* Determine key type */
            kv_key_filter_type_t key_type = KV_KEY_FILTER_TYPE_NORMAL;
            if ((((uint32_t)kv_key_attributes(key)) & ((uint32_t)KV_ATTRIBUTE_PERMANENT)) != 0) {
                key_type = KV_KEY_FILTER_TYPE_PERMANENT;
            }
            if (((uint32_t)search_filter->type & (uint32_t)key_type) == 0) {
                return ERRCODE_NV_SEARCH_KEY_TYPE_MISMATCH;
            }
        }

        if (((uint32_t)search_filter->state & (uint32_t)KV_KEY_FILTER_STATE_ANY) != (uint32_t)KV_KEY_FILTER_STATE_ANY) {
            /* Determine key state */
            kv_key_filter_state_t key_state = KV_KEY_FILTER_STATE_INVALID;
            if (kv_key_is_valid(key)) {
                key_state = KV_KEY_FILTER_STATE_VALID;
            }
            if (((uint32_t)search_filter->state & (uint32_t)key_state) == 0) {
                return ERRCODE_NV_SEARCH_KEY_STATE_MISMATCH;
            }
        }
    }

    return ERRCODE_SUCC;
}

STATIC bool kv_key_header_is_full_ff(kv_key_header_t *key_header, uint8_t len)
{
    uint8_t cmp_len = (len > (uint8_t)sizeof(kv_key_header_t)) ? ((uint8_t)sizeof(kv_key_header_t)) : len;

    if (memcmp(key_header, &g_header_cmp, cmp_len) == 0) {
        return true;
    }
    return false;
}

errcode_t kv_key_get_handle_from_location(kv_key_location key_location, kv_key_handle_t *key)
{
    key->header.length = 0xFFFF;  /* Would not expect a flash read to fail, but just in case... */
    errcode_t res = kv_key_helper_copy_flash((uint32_t)(uintptr_t)&key->header, (uint32_t)(uintptr_t)key_location,
                                             sizeof(kv_key_header_t));
    if (res != ERRCODE_SUCC) {
        return res;
    }

    key->key_location = key_location;

    if (kv_key_header_is_full_ff(&key->header, (uint8_t)sizeof(kv_key_header_t))) {
        /* Last key has been read */
        return ERRCODE_NV_KEY_NOT_FOUND;
    }

    if (g_nv_header_magic == KV_KEY_MAGIC && key->header.magic != KV_KEY_MAGIC) {
        /* Wrong key has been read */
        return ERRCODE_NV_INVALID_KEY_HEADER;
    }
    return ERRCODE_SUCC;
}

errcode_t kv_key_locations_in_same_page(kv_key_location first_key_location, kv_key_location second_key_location)
{
    uintptr_t first_page_location = (uintptr_t)first_key_location & ~(KV_PAGE_SIZE - 1);
    uintptr_t second_page_location = ((uintptr_t)second_key_location +
        (uintptr_t)sizeof(kv_key_header_t)) & ~(KV_PAGE_SIZE - 1);
    if (first_page_location != second_page_location) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

#if (CONFIG_NV_SUPPORT_SKIP_CORRUPT_KEY == NV_YES)
#define KV_KEY_ALIGN    4u
#define KV_KEY_ALIGN_UP(v, a) (((v) + ((a) - 1u)) & ~((a) - 1u))
errcode_t kv_key_get_next_magic_position(kv_key_location key_location, kv_key_handle_t *key)
{
    uint32_t src = (uint32_t)(uintptr_t)key_location;
    uint32_t start_page_loc = nv_get_region_area()->nv_data_addr - FLASH_PHYSICAL_ADDR_START;
    uint32_t next_page_loc =  ((((src - start_page_loc) / KV_PAGE_SIZE) + 1) * KV_PAGE_SIZE) + start_page_loc;

    /* 从传入位置的下一位开始读 */
    src = KV_KEY_ALIGN_UP(src + 1u, KV_KEY_ALIGN);
    bool found_ending = false;
    kv_key_location tmp_location;

    for (; src < next_page_loc - sizeof(kv_key_header_t); src += KV_KEY_ALIGN) {
        (void)memset_s(&(key->header), sizeof(kv_key_header_t), 0, sizeof(kv_key_header_t));
        if (kv_key_helper_copy_flash((uint32_t)(uintptr_t)&key->header, src, sizeof(kv_key_header_t)) != ERRCODE_SUCC) {
            return ERRCODE_NV_READ_FLASH_ERR;
        }

        if (key->header.magic == KV_KEY_MAGIC &&
            (key->header.valid == KV_KEY_INVALID || key->header.valid == KV_KEY_VALID)) {
            key->key_location = (kv_key_location)(uintptr_t)src;
            /* 找到有效的Key */
            if (kv_key_validate_integrity(key, true) == ERRCODE_SUCC) {
                nv_log_debug("[NV][magic] Get valid next key (location = 0x%x).\r\n", key->key_location);
                return ERRCODE_SUCC;
            }
        }

        if (kv_key_header_is_full_ff(&key->header, (uint8_t)sizeof(kv_key_header_t))) {
            /* 找到一块全F区域，暂时标记为数据结尾 */
            if (!found_ending) {
                found_ending = true;
                tmp_location = (kv_key_location)(uintptr_t)src;
            }
            /* 若此段为全F，直接跳到下一段进行读取，不以4的步长行进 */
            src += (uint32_t)sizeof(kv_key_header_t) - KV_KEY_ALIGN;
        } else if (found_ending) {
            /* 如又找到一块非全F区域，则上次标记的位置不是数据结尾，取消标记 */
            found_ending = false;
        }
    }
    if (found_ending) {
        key->key_location = tmp_location;
    } else {
        /* 如未找到数据结尾， 则在最后继续查找更短一些的全FF数据 */
        for (src = next_page_loc - (uint32_t)sizeof(kv_key_header_t); src < next_page_loc; src += KV_KEY_ALIGN) {
            uint32_t read_len = next_page_loc - src;
            if (kv_key_helper_copy_flash((uint32_t)(uintptr_t)&key->header, src, (uint16_t)read_len) != ERRCODE_SUCC) {
                return ERRCODE_NV_READ_FLASH_ERR;
            }
            if (kv_key_header_is_full_ff(&key->header, (uint8_t)read_len)) {
                key->key_location = (kv_key_location)(uintptr_t)src;
                nv_log_debug("[NV][magic] Get next key on end 2 (location = 0x%x).\r\n", key->key_location);
                return ERRCODE_NV_KEY_NOT_FOUND;
            }
        }
        key->key_location = (kv_key_location)(uintptr_t)src;
    }

    return ERRCODE_NV_KEY_NOT_FOUND;
}

STATIC errcode_t kv_key_get_next_key_from_location(kv_key_location cur_key_loc, kv_key_location next_key_loc,
    kv_key_handle_t *key)
{
    if (kv_key_locations_in_same_page(cur_key_loc, next_key_loc) != ERRCODE_SUCC) {
        /* Reached the end of the KV page */
        return ERRCODE_NV_KEY_NOT_FOUND;
    }

    return kv_key_get_handle_from_location(next_key_loc, key);
}

STATIC errcode_t kv_key_get_next_handle_with_corrupt_key(kv_key_handle_t *key, kv_key_validate_status_t key_status)
{
    kv_key_handle_t cur_key;
    errcode_t res = ERRCODE_SUCC;

    /* 如果当前KEY是错误Key，直接需要通过magic找到下一个key */
    if (key_status == KV_KEY_VALIDATE_WRONG) {
        return kv_key_get_next_magic_position(key->key_location, key);
    }

    if (kv_key_header_is_full_ff(&key->header, (uint8_t)sizeof(kv_key_header_t))) {
        /* Last key has been read */
        return ERRCODE_NV_KEY_NOT_FOUND;
    }

    if (key->header.magic != KV_KEY_MAGIC) {
        /* corrupt key has been read, then get next key by magic */
        nv_log_info("[NV] header magic error(location = 0x%x)! Get next key by magic.\r\n", key->key_location);
        return kv_key_get_next_magic_position(key->key_location, key);
    }

    uint16_t offset = kv_key_flash_size(key);
    kv_key_location next_key_location = (kv_key_location)((uintptr_t)key->key_location + offset);

    (void)memcpy_s(&cur_key, sizeof(kv_key_handle_t), key, sizeof(kv_key_handle_t));

    /* 通过当前KEY位置+key_length获取到Next key位置，如果Next Key的Magic恰好正确，说明找到了Next Key，直接返回success。
       否则，有如下三种情况：
       1. Next Key的位置与当前Key不在一个page内
       2. Next key的位置指向数据结尾（全FF）
       3. Next Key的位置指向非全FF数据，但Magic错误
       这三种情况下，首先验证当前key，如果当前KEY校验成功，则说明key_length是可信的。
           情况1和情况2，说明已经找到了当前page的数据结尾，返回ERRCODE_NV_KEY_NOT_FOUND；
           情况3，说明找到了Next Key(本函数不关心Next key正确性)，返回success
       如果当前KEY校验不成功，则说明key_length可能是不正确的，则需要通过magic找到下一个key。
    */
    res = kv_key_get_next_key_from_location(key->key_location, next_key_location, key);
    if (res != ERRCODE_SUCC) {
        if (key_status == KV_KEY_VALIDATE_CORRECT || (kv_key_validate_integrity(&cur_key, true) == ERRCODE_SUCC)) {
            key->key_location = next_key_location;
            if (res == ERRCODE_NV_INVALID_KEY_HEADER) {
                res = ERRCODE_SUCC;
            }
            return res;
        } else {
            /* 如果当前KEY校验不成功，则说明key length可能是不正确的，则需要通过magic找到下一个KEY */
            nv_log_info("[NV] validate failed (location = 0x%x)! Get next key by magic.\r\n", cur_key.key_location);
            return kv_key_get_next_magic_position(cur_key.key_location, key);
        }
    }

    return res;
}
#endif /* #ifdef CONFIG_NV_SUPPORT_SKIP_CORRUPT_KEY */

STATIC errcode_t kv_key_get_next_handle_direct(kv_key_handle_t *key)
{
    if (key->header.length == 0xFFFF) {
        /* Last key has been read */
        return ERRCODE_NV_KEY_NOT_FOUND;
    }

    uint16_t offset = kv_key_flash_size(key);
    kv_key_location new_key_location = (kv_key_location)((uintptr_t)key->key_location + offset);
    if (kv_key_locations_in_same_page(key->key_location, new_key_location) != ERRCODE_SUCC) {
        /* Reached the end of the KV page */
        key->key_location = (key->key_location + KV_PAGE_SIZE - 1) & ~(KV_PAGE_SIZE - 1);
        return ERRCODE_NV_KEY_NOT_FOUND;
    }

    return kv_key_get_handle_from_location(new_key_location, key);
}

errcode_t kv_key_get_next_handle(kv_key_handle_t *key, kv_key_validate_status_t key_status)
{
    unused(key_status);
#if (CONFIG_NV_SUPPORT_SKIP_CORRUPT_KEY == NV_YES)
    if (g_nv_header_magic == KV_KEY_MAGIC) {
        return kv_key_get_next_handle_with_corrupt_key(key, key_status);
    } else {
        return kv_key_get_next_handle_direct(key);
    }
#else
    return kv_key_get_next_handle_direct(key);
#endif
}

STATIC void kv_key_fix_header_for_validate(kv_key_header_t *key_header, bool ignor_invalid)
{
    if (ignor_invalid && key_header->valid == KV_KEY_INVALID) {
        key_header->valid = KV_KEY_VALID;
        if (g_nv_header_magic == KV_KEY_NO_MAGIC) {
            key_header->magic = g_nv_header_magic;
        }
    }
}

#if (CONFIG_NV_SUPPORT_HASH_FOR_CRYPT == NV_YES)
STATIC errcode_t kv_key_hash_verify(uint8_t *read_hash, uint8_t *cal_hash)
{
    if (memcmp(cal_hash, read_hash, KV_CRYPTO_HASH_SIZE - KV_CRYPTO_CRC_SIZE) != 0) {
        return ERRCODE_NV_HASH_MISMATCH;
    }
    return ERRCODE_SUCC;
}
#endif

STATIC void nv_key_verification_start(const kv_key_handle_t *key, nv_key_verfication_ctrl_t *key_verify)
{
    key_verify->crc_ret = 0;
    if (key->header.enc_key != AES_KDFKEY_SDRK_TYPE) {
        return;
    }

#if ((CONFIG_NV_SUPPORT_ENCRYPT == NV_YES) && (CONFIG_NV_SUPPORT_HASH_FOR_CRYPT == NV_YES))
    (void)nv_crypto_start_hash(&(key_verify->hash_handle));
    (void)memset_s(key_verify->cal_hash, KV_CRYPTO_HASH_SIZE, 0, KV_CRYPTO_HASH_SIZE);
#endif
}

STATIC void kv_key_verification_update(const kv_key_handle_t *key, nv_key_verfication_ctrl_t *key_verify,
    uint8_t *data_chunk, uint32_t data_len)
{
#if (defined(CONFIG_NV_SUPPORT_CRC16_VERIFY) && (CONFIG_NV_SUPPORT_CRC16_VERIFY == NV_YES))
    key_verify->crc_ret = (uint16_t)uapi_crc16((uint16_t)key_verify->crc_ret, data_chunk, data_len);
#else
    key_verify->crc_ret = uapi_crc32(key_verify->crc_ret, data_chunk, data_len);
#endif
    if (key->header.enc_key != AES_KDFKEY_SDRK_TYPE) {
        return;
    }

#if ((CONFIG_NV_SUPPORT_ENCRYPT == NV_YES) && (CONFIG_NV_SUPPORT_HASH_FOR_CRYPT == NV_YES))
    nv_crypto_update_hash(key_verify->hash_handle, data_chunk, data_len);
#endif
}

STATIC void nv_key_verification_finish(const kv_key_handle_t *key, nv_key_verfication_ctrl_t *key_verify)
{
    key_verify->crc_ret = kv_crc32_swap(key_verify->crc_ret);
    if (key->header.enc_key != AES_KDFKEY_SDRK_TYPE) {
        return;
    }

#if ((CONFIG_NV_SUPPORT_ENCRYPT == NV_YES) && (CONFIG_NV_SUPPORT_HASH_FOR_CRYPT == NV_YES))
    nv_crypto_complete_hash(key_verify->hash_handle, key_verify->cal_hash);
#endif
}

STATIC errcode_t nv_key_verification_check(const kv_key_handle_t *key, nv_key_verfication_ctrl_t *key_verify,
    uint8_t *check_data)
{
    errcode_t res = ERRCODE_SUCC;

    /* 不加密KEY, 比较CRC */
    if (key->header.enc_key != AES_KDFKEY_SDRK_TYPE) {
        if (memcmp(&(key_verify->crc_ret), check_data, KV_CRYPTO_CRC_SIZE) != 0) {
            res = ERRCODE_NV_HASH_MISMATCH;
        }
        return res;
    }

#if (CONFIG_NV_SUPPORT_ENCRYPT != NV_YES)
    /* 不支持加密的情况下直接返回失败 */
    return ERRCODE_NV_HASH_MISMATCH;
#else
    if (key_verify->crypto_handle == INVAILD_CRYPTO_HANDLE) {
        /* 加密KEY，只校验完整性场景，最后4个字节与密文CRC比较 */
        if (memcmp(&(key_verify->crc_ret),
            check_data + KV_CRYPTO_HASH_SIZE - KV_CRYPTO_CRC_SIZE, KV_CRYPTO_CRC_SIZE) != 0) {
            res = ERRCODE_NV_HASH_MISMATCH;
        }
        return res;
    }
#if (CONFIG_NV_SUPPORT_HASH_FOR_CRYPT == NV_YES)
    /* 使用HASH校验的场景，校验HASH的前28个字节 */
    return kv_key_hash_verify(check_data, key_verify->cal_hash);
#else
    /* 使用GCM TAG的场景，直接校验16字节的TAG */
    return nv_crypto_validate_tag(key_verify->crypto_handle);
#endif /* (CONFIG_NV_SUPPORT_HASH_FOR_CRYPT == NV_YES)) */
#endif /* #if (CONFIG_NV_SUPPORT_ENCRYPT != NV_YES) */
}

STATIC errcode_t nv_key_read_header_with_verification(kv_key_handle_t *key, nv_key_verfication_ctrl_t *key_verify)
{
    errcode_t res;
    uint8_t key_header[sizeof(kv_key_header_t)];
    uint32_t key_location = (uint32_t)key->key_location;

    nv_key_verification_start(key, key_verify);

    /* Read header first, which is never encrypted */
    res = kv_key_helper_copy_flash((uintptr_t)key_header, key_location, sizeof(kv_key_header_t));
    if (res != ERRCODE_SUCC) {
        return res;
    }

    kv_key_fix_header_for_validate((kv_key_header_t *)(uintptr_t)key_header, key_verify->ignor_invalid);

    kv_key_verification_update(key, key_verify, key_header, sizeof(kv_key_header_t));
    return ERRCODE_SUCC;
}

#if ((CONFIG_NV_SUPPORT_ENCRYPT == NV_YES) && (CONFIG_NV_SUPPORT_HASH_FOR_CRYPT == NV_NO))
STATIC void nv_key_preprocess_crypto_tag(kv_key_handle_t *key, nv_key_verfication_ctrl_t *key_verify)
{
    /* 在使用GCM TAG时，需要先读取一次TAG并设置给解密模块 */
    if (key_verify->crypto_handle != INVAILD_CRYPTO_HANDLE) {
        uint32_t padded_data_len = kv_key_padded_data_length(kv_key_attributes(key), key->header.length);
        uint32_t hash_location = (uint32_t)key->key_location + (uint32_t)sizeof(kv_key_header_t) + padded_data_len;
        uint8_t tag[KV_CRYPTO_HASH_SIZE] = {0};
        (void)kv_key_helper_copy_flash((uintptr_t)tag, hash_location, KV_CRYPTO_HASH_SIZE);
        (void)nv_crypto_set_tag(key_verify->crypto_handle, (uint8_t *)tag, NV_AES_GCM_TAG_LENGTH);
    }
}
#endif /* #if ((CONFIG_NV_SUPPORT_ENCRYPT == NV_YES) && (CONFIG_NV_SUPPORT_HASH_FOR_CRYPT == NV_NO)) */

STATIC errcode_t nv_key_read_data_with_verification(kv_key_handle_t *key, nv_key_verfication_ctrl_t *key_verify,
    uint8_t *dest_buff)
{
    errcode_t res = ERRCODE_SUCC;
    uint8_t *read_data_chunk = NULL;
    uint8_t *dest_location = dest_buff;
    uint32_t key_data_offset = 0;
    uint32_t key_data_location = (uint32_t)key->key_location + (uint32_t)sizeof(kv_key_header_t);
    uint32_t key_data_len = key->header.length;
    uint32_t padded_data_len = kv_key_padded_data_length(kv_key_attributes(key), key->header.length);

#if ((CONFIG_NV_SUPPORT_ENCRYPT == NV_YES) && (CONFIG_NV_SUPPORT_HASH_FOR_CRYPT == NV_NO))
    nv_key_preprocess_crypto_tag(key, key_verify);
#endif

    /*
     * 在dest_buff非NULL情况下，直接使用dest_buff的空间保存KEY data，以尽量避免malloc内存。
     * 但由于dest_buff的长度只有key->header.length，不包含padding数据，因此在key->header.length不满足对齐要求时，
     * 需要在处理最后一个chunk时，先将数据读取至read_data_chunk缓存中，再memcpy至dest_buff。
     */
    if ((dest_buff == NULL || padded_data_len != key_data_len)) {
        read_data_chunk = (uint8_t *)kv_zalloc(NV_KEY_DATA_CHUNK_LEN);
        if (read_data_chunk == NULL) {
            return ERRCODE_MALLOC;
        }
    }

    /* Read key data, using AES engine if encrypted */
    while (key_data_offset < key_data_len && key_data_offset < padded_data_len) {
        uint32_t chunk_len = uapi_min(NV_KEY_DATA_CHUNK_LEN, padded_data_len - key_data_offset);
        uint32_t actual_len = uapi_min(NV_KEY_DATA_CHUNK_LEN, key_data_len - key_data_offset);
        if (read_data_chunk != NULL) {
            dest_location = read_data_chunk;
        } else {
            dest_location = dest_buff + key_data_offset;
        }

        res = kv_key_read_data_from_flash((uintptr_t)dest_location, (uintptr_t)key_data_location + key_data_offset,
                                          chunk_len, key_verify->crypto_handle);
        if (res != ERRCODE_SUCC) {
            break;
        }

        kv_key_verification_update(key, key_verify, dest_location, chunk_len);

        if (dest_buff != NULL && read_data_chunk != NULL) {
            if (memcpy_s(dest_buff + key_data_offset, key_data_len - key_data_offset,
                read_data_chunk, actual_len) != EOK) {
                res = ERRCODE_MEMCPY;
                break;
            }
        }

        key_data_offset += chunk_len;
    }

    if (read_data_chunk != NULL) {
        kv_free(read_data_chunk);
        read_data_chunk = NULL;
    }

    return res;
}

STATIC errcode_t nv_key_read_tail_with_verification(kv_key_handle_t *key, nv_key_verfication_ctrl_t *key_verify)
{
    errcode_t res;
    uint32_t padded_data_len = kv_key_padded_data_length(kv_key_attributes(key), key->header.length);
    uint32_t tail_location = (uint32_t)key->key_location + (uint32_t)sizeof(kv_key_header_t) + padded_data_len;
    uint32_t tail_len = (key->header.enc_key == AES_KDFKEY_SDRK_TYPE) ? KV_CRYPTO_HASH_SIZE : KV_CRYPTO_CRC_SIZE;

    uint8_t *read_data_chunk = (uint8_t *)kv_zalloc(KV_CRYPTO_HASH_SIZE);
    if (read_data_chunk == NULL) {
        return ERRCODE_MALLOC;
    }

    /* Read Tail, which is unencrypted */
    res = kv_key_helper_copy_flash((uint32_t)(uintptr_t)read_data_chunk, tail_location, tail_len);
    if (res != ERRCODE_SUCC) {
        goto exit;
    }

    nv_key_verification_finish(key, key_verify);

    res = nv_key_verification_check(key, key_verify, read_data_chunk);
exit:
    kv_free(read_data_chunk);
    return res;
}

/* 直接读取不加密KEY数据(无校验) */
STATIC errcode_t kv_key_read_uncrypted_data(kv_key_handle_t *key, uint8_t *dest_location)
{
    errcode_t res = ERRCODE_SUCC;
    uint32_t key_data_offset = 0;
    uint32_t key_data_location = (uint32_t)key->key_location + (uint32_t)sizeof(kv_key_header_t);
    uint32_t key_data_len = key->header.length;

    while (key_data_offset < key_data_len) {
        uint32_t chunk_len = uapi_min(NV_KEY_DATA_CHUNK_LEN, key_data_len - key_data_offset);

        res = kv_key_helper_copy_flash((uint32_t)(uintptr_t)(dest_location + key_data_offset),
            key_data_location + key_data_offset, chunk_len);
        if (res != ERRCODE_SUCC) {
            break;
        }
        key_data_offset += chunk_len;
    }
    return res;
}

#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
/* 读取加密KEY数据并检验 */
STATIC errcode_t kv_key_read_encrypted_data(kv_key_handle_t *key, uint8_t *dest_location)
{
    errcode_t res = ERRCODE_SUCC;
    nv_key_verfication_ctrl_t verfication_ctrl = {
        .crc_ret = 0,
        .crypto_handle = INVAILD_CRYPTO_HANDLE,
        .ignor_invalid = 0,
    };

    /* 加密KEY，需要在读取数据时校验 */
    res = nv_crypto_claim_aes(&(verfication_ctrl.crypto_handle), &(key->header), false);
    if (res != ERRCODE_SUCC) {
        return res;
    }

    res = nv_key_read_header_with_verification(key, &verfication_ctrl);
    if (res != ERRCODE_SUCC) {
        goto exit;
    }

    res = nv_key_read_data_with_verification(key, &verfication_ctrl, dest_location);
    if (res != ERRCODE_SUCC) {
        goto exit;
    }

    res = nv_key_read_tail_with_verification(key, &verfication_ctrl);
    if (res != ERRCODE_SUCC) {
        /* 校验失败，清空读buff */
        (void)memset_s(dest_location, key->header.length, 0, key->header.length);
    }

exit:
    if (verfication_ctrl.crypto_handle != INVAILD_CRYPTO_HANDLE) {
        nv_crypto_release_aes(verfication_ctrl.crypto_handle);
    }
    return res;
}
#endif /* CONFIG_NV_SUPPORT_ENCRYPT == NV_YES */

errcode_t kv_key_read_data(kv_key_handle_t *key, uint8_t *dest_location)
{
    if (key->header.enc_key == AES_KDFKEY_SDRK_TYPE) {
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
        return kv_key_read_encrypted_data(key, dest_location);
#else
        unused(key);
        unused(dest_location);
        return ERRCODE_NOT_SUPPORT;
#endif
    } else {
        return kv_key_read_uncrypted_data(key, dest_location);
    }
}

/*
 * 校验KEY的完整性。
 * 非加密KEY, 校验CRC;
 * 加密KEY, 校验密文的CRC(最后4个字节)
 */
errcode_t kv_key_validate_integrity(kv_key_handle_t *key, bool ignor_invalid)
{
    errcode_t res = ERRCODE_SUCC;
    /* 完整性校验不解密，只校验密文CRC，crypto_handle置为无效值 */
    nv_key_verfication_ctrl_t verfication_ctrl = {
        .crc_ret = 0,
        .crypto_handle = INVAILD_CRYPTO_HANDLE,
        .ignor_invalid = ignor_invalid,
    };

    if (key->header.length > NV_NORMAL_KVALUE_MAX_LEN) {
        return ERRCODE_NV_INVALID_KEY_HEADER;
    }

    res = nv_key_read_header_with_verification(key, &verfication_ctrl);
    if (res != ERRCODE_SUCC) {
        return res;
    }

    res = nv_key_read_data_with_verification(key, &verfication_ctrl, NULL);
    if (res != ERRCODE_SUCC) {
        return res;
    }

    return nv_key_read_tail_with_verification(key, &verfication_ctrl);
}

/*
 * 校验KEY的正确性：
 * 非加密KEY校验CRC
 * 加密KEY, 如CONFIG_NV_SUPPORT_HASH_FOR_CRYPT为YES，校验明文HASH；
 *          如CONFIG_NV_SUPPORT_HASH_FOR_CRYPT为NO，则校验GCM Tag。
 */
errcode_t kv_key_validation(kv_key_handle_t *key, bool ignor_invalid)
{
    errcode_t res = ERRCODE_SUCC;
    nv_key_verfication_ctrl_t verfication_ctrl = {
        .crc_ret = 0,
        .crypto_handle = INVAILD_CRYPTO_HANDLE,
        .ignor_invalid = ignor_invalid,
    };

    if (key->header.length > NV_NORMAL_KVALUE_MAX_LEN) {
        return ERRCODE_NV_INVALID_KEY_HEADER;
    }

#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    if (key->header.enc_key == AES_KDFKEY_SDRK_TYPE) {
        res = nv_crypto_claim_aes(&(verfication_ctrl.crypto_handle), &(key->header), false);
        if (res != ERRCODE_SUCC) {
            return res;
        }
    }
#endif

    res = nv_key_read_header_with_verification(key, &verfication_ctrl);
    if (res != ERRCODE_SUCC) {
        goto exit;
    }

    res = nv_key_read_data_with_verification(key, &verfication_ctrl, NULL);
    if (res != ERRCODE_SUCC) {
        goto exit;
    }

    res = nv_key_read_tail_with_verification(key, &verfication_ctrl);

exit:
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    if (verfication_ctrl.crypto_handle != INVAILD_CRYPTO_HANDLE) {
        nv_crypto_release_aes(verfication_ctrl.crypto_handle);
    }
#endif
    return res;
}

kv_attributes_t kv_key_attributes(const kv_key_handle_t *key)
{
    kv_attributes_t attributes = (kv_attributes_t)0;

    if (key->header.type != KV_KEY_TYPE_NORMAL) {
        attributes = (kv_attributes_t)((uint32_t)attributes | (uint32_t)KV_ATTRIBUTE_PERMANENT);
    }
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    if (key->header.enc_key != 0) {
        attributes = (kv_attributes_t)((uint32_t)attributes | (uint32_t)KV_ATTRIBUTE_ENCRYPTED);
    }
#endif
    if (key->header.upgrade != KV_KEY_TYPE_NORMAL) {
        attributes = (kv_attributes_t)((uint32_t)attributes | (uint32_t)KV_ATTRIBUTE_NON_UPGRADE);
    }
    return attributes;
}

uint16_t kv_key_padded_data_length(kv_attributes_t attributes, uint16_t unpadded_length)
{
    unused(attributes);
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    if (((uint32_t)attributes & (uint32_t)KV_ATTRIBUTE_ENCRYPTED) != 0) {
        return kv_padded_ciphertext_length(unpadded_length);
    }
#endif
    return kv_padded_cleartext_length(unpadded_length);
}

uint16_t kv_key_flash_size(kv_key_handle_t *key)
{
    uint16_t size;
    kv_attributes_t attributes;
    uint16_t hash_crc_size = KV_CRYPTO_CRC_SIZE;
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    if (key->header.enc_key == AES_KDFKEY_SDRK_TYPE) {
        hash_crc_size = KV_CRYPTO_HASH_SIZE;
    }
#endif
    size = (uint16_t)sizeof(kv_key_header_t) + hash_crc_size;

    attributes = kv_key_attributes(key);
    size += kv_key_padded_data_length(attributes, key->header.length);

    return size;
}

void kv_key_build_from_new(kv_key_handle_t *RESTRICT key, const kv_key_details_t *RESTRICT new_key, kv_key_location key_location)
{
    key->header.key_id = new_key->key_id;
    key->header.length = (uint16_t)new_key->kvalue_length;
    key->header.magic = g_nv_header_magic;
    key->header.valid = KV_KEY_VALID;
    key->header.type = KV_KEY_TYPE_NORMAL;
    if (((uint32_t)new_key->attributes & (uint32_t)KV_ATTRIBUTE_PERMANENT) != 0) {
        key->header.type = KV_KEY_TYPE_PERMANENT;
    }
    key->header.upgrade = KV_KEY_TYPE_NORMAL;
    if (((uint32_t)new_key->attributes & (uint32_t)KV_ATTRIBUTE_NON_UPGRADE) != 0) {
        key->header.upgrade = 0;
        key->header.version = key->header.version + 1;
    }
    key->header.enc_key = 0;
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    if (((uint32_t)new_key->attributes & (uint32_t)KV_ATTRIBUTE_ENCRYPTED) != 0) {
        key->header.enc_key = AES_KDFKEY_SDRK_TYPE;
        nv_crypto_generate_random(&key->header.rnd);
        key->header.version = key->header.version + 1;
    }
#endif
    key->key_location = key_location;
}

uint32_t kv_crc32_swap(uint32_t crc)
{
    return ((crc >> CRC_SWAP_SIZE24) | ((crc >> CRC_SWAP_SIZE8) & 0xFF00) |
        ((crc << CRC_SWAP_SIZE8) & 0xFF0000) | (crc << CRC_SWAP_SIZE24));
}
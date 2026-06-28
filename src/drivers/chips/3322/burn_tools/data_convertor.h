/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Provides burn uart port template \n
 * History: \n
 */
#ifndef DATA_CONVERTOR_H
#define DATA_CONVERTOR_H

#include "burn_types.h"

#ifdef __cplusplus
extern "C" {
#endif

u32 dc_str_to_u32_dec_hex(const char *str, int *ok);
int dc_is_visible_ascii(u8 ch);
int dc_find_char(const u8 *data, u32 len, u8 ch);

int dc_parse_fwpkg(const char *path, bin_info_t *out, u32 max_count, u32 *out_count,
                   char *version, u32 version_len);
int dc_fill_single_bin(const char *path, bin_info_t *out, u32 *out_count, u32 max_count,
                       u32 burn_addr, u32 burn_size, image_type_tt type);

#ifdef __cplusplus
}
#endif

#endif /* DATA_CONVERTOR_H */

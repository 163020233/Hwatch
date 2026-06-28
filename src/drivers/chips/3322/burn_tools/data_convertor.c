/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Provides burn uart port template \n
 * History: \n
 * 2022-06-06， Create file. \n
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "securec.h"
#include "seboot_channel.h"
#include "data_convertor.h"

#pragma pack(push, 1)
typedef struct {
    u8 name[32];
    u32 start_index;
    u32 image_size;
    u32 burn_addr;
    u32 burn_size;
    u32 type;
} image_info_t;

typedef struct {
    u32 flag;
    u16 crc;
    u16 image_num;
    u32 total_file_size;
    image_info_t info[0];
} fwpkg_head_t;

typedef struct {
    u8 name[260];
    u32 start_index;
    u32 image_size;
    u32 burn_addr;
    u32 burn_size;
    u32 type;
} image_info_new_t;

typedef struct {
    u32 flag;
    u16 crc;
    u16 image_num;
    u32 total_file_size;
    u8 name[260]; // len 260
    image_info_new_t info[0];
} fwpkg_head_new_t;
#pragma pack(pop)

static void dc_zero_str(char *s, u32 len)
{
    if (s && len > 0) {
        s[0] = '\0';
    }
}

static void dc_copy_str(char *dst, u32 dst_len, const char *src, u32 src_len)
{
    if (dst == NULL || dst_len == 0) {
        return;
    }
    if (src == NULL) {
        dst[0] = '\0';
        return;
    }
    u32 tmp_len = src_len;
    if (tmp_len >= dst_len) {
        tmp_len = dst_len - 1;
    }
    (void)memcpy_s(dst, tmp_len, src, tmp_len);
    dst[tmp_len] = '\0';
}

static const char *dc_basename(const char *path)
{
    const char *p = path;
    const char *last = path;
    if (path == NULL) {
        return "";
    }
    while (*p) {
        if (*p == '/' || *p == '\\') {
            last = p + 1;
        }
        p++;
    }
    return last;
}

u32 dc_str_to_u32_dec_hex(const char *str, int *ok)
{
    if (ok) {
        *ok = 0;
    }
    if (str == NULL) {
        return 0;
    }
    while (*str == ' ' || *str == '\t') {
        str++;
    }
    int base = 10; // base 10
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        base = 16; // base 16
    }
    char *end = NULL;
    unsigned long val = strtoul(str, &end, base);
    if (end != str) {
        if (ok) {
            *ok = 1;
        }
    }
    return (u32)val;
}

int dc_is_visible_ascii(u8 ch)
{
    if (ch == 8 || ch == 9 || ch == 10 || ch == 13) { // char 8 9 10 13
        return 1;
    }
    if (ch >= 0x20 && ch <= 0x7f) {
        return 1;
    }
    return 0;
}

int dc_find_char(const u8 *data, u32 len, u8 ch)
{
    if (data == NULL) {
        return 0;
    }
    for (u32 i = 0; i < len; i++) {
        if (data[i] == ch) {
            return 1;
        }
    }
    return 0;
}

static int dc_add_bin(bin_info_t *out, u32 max_count, u32 *out_count, const bin_info_t *src,
                      int insert_front)
{
    if (out == NULL || out_count == NULL || src == NULL) {
        return -1;
    }
    if (*out_count >= max_count) {
        return -1;
    }
    if (insert_front) {
        for (u32 i = *out_count; i > 0; i--) {
            out[i] = out[i - 1];
        }
        out[0] = *src;
        (*out_count)++;
        return 0;
    }
    out[*out_count] = *src;
    (*out_count)++;
    return 0;
}

int dc_fill_single_bin(const char *path, bin_info_t *out, u32 *out_count, u32 max_count,
                       u32 burn_addr, u32 burn_size, image_type_tt type)
{
    if (path == NULL || out == NULL || out_count == NULL) {
        return -1;
    }
    if (max_count == 0) {
        return -1;
    }
    char *real_path = realpath(path, NULL);
    if (real_path == NULL) {
        return -1;
    }
    FILE *f = fopen(real_path, "rb");
    if (f == NULL) {
        return -1;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        (void)fclose(f);
        return -1;
    }
    long file_len = ftell(f);
    (void)fclose(f);
    if (file_len <= 0) {
        return -1;
    }
    bin_info_t info = {0};
    dc_copy_str(info.path, sizeof(info.path), path, (u32)strlen(path));
    dc_copy_str(info.file_name, sizeof(info.file_name), dc_basename(path), (u32)strlen(dc_basename(path)));
    info.file_index = 0;
    info.file_size = (u32)file_len;
    info.burn_addr = burn_addr;
    info.burn_size = burn_size;
    info.type = type;
    *out_count = 0;
    return dc_add_bin(out, max_count, out_count, &info, 0);
}

static int dc_parse_fwpkg_proc(const char *path, fwpkg_head_new_t *head, bin_info_t *out,
    char *version, u32 version_len, u32 max_count, u32 *out_count)
{
    u32 crc_len = (u32)(sizeof(fwpkg_head_new_t) - 6 + sizeof(image_info_new_t) * head->image_num); // len 6
    u16 calc = seboot_crc16((const u8 *)&head->image_num, crc_len);
    if (calc != head->crc) {
        return -1;
    }
    if (version && version_len > 0) {
        head->name[259] = 0; // offset 259
        dc_copy_str(version, version_len, (const char *)head->name, (u32)strlen((const char *)head->name));
    }
    for (u16 i = 0; i < head->image_num; i++) {
        bin_info_t info = {0};
        dc_copy_str(info.path, sizeof(info.path), path, (u32)strlen(path));
        info.file_index = head->info[i].start_index;
        info.file_size = head->info[i].image_size;
        info.burn_addr = head->info[i].burn_addr;
        info.burn_size = head->info[i].burn_size;
        info.type = (image_type_tt)head->info[i].type;
        head->info[i].name[259] = 0; // offset 259
        dc_copy_str(info.file_name, sizeof(info.file_name), (const char *)head->info[i].name,
                    (u32)strlen((const char *)head->info[i].name));
        int insert_front = (info.type == IMAGE_TYPE_LOADER);
        dc_add_bin(out, max_count, out_count, &info, insert_front);
    }
    return 0;
}

int dc_parse_fwpkg(const char *path, bin_info_t *out, u32 max_count, u32 *out_count,
                   char *version, u32 version_len)
{
    if (path == NULL || out == NULL || out_count == NULL) {
        return -1;
    }
    *out_count = 0;
    dc_zero_str(version, version_len);
    char *real_path = realpath(path, NULL);
    if (real_path == NULL) {
        return -1;
    }
    FILE *f = fopen(real_path, "rb");
    if (f == NULL) {
        return -1;
    }
    u32 flag = 0;
    u16 crc = 0;
    u16 image_num = 0;
    u32 total_file_size = 0;
    size_t read = fread(&flag, 1, sizeof(flag), f);
    if (read != sizeof(flag)) {
        (void)fclose(f);
        return -1;
    }
    (void)fread(&crc, 1, sizeof(crc), f);
    (void)fread(&image_num, 1, sizeof(image_num), f);
    (void)fread(&total_file_size, 1, sizeof(total_file_size), f);
    if (flag >= 0xefbeadd0 && flag < 0xefbeaddf) {
        u32 head_size = sizeof(fwpkg_head_new_t) + sizeof(image_info_new_t) * image_num;
        u8 *buf = (u8 *)malloc(head_size);
        if (buf == NULL) {
            (void)fclose(f);
            return -1;
        }
        fseek(f, 0, SEEK_SET);
        read = fread(buf, 1, head_size, f);
        (void)fclose(f);
        if (read != head_size) {
            free(buf);
            return -1;
        }
        fwpkg_head_new_t *head = (fwpkg_head_new_t *)buf;
        int ret = dc_parse_fwpkg_proc(path, head, out, version, version_len, max_count, out_count);
        free(buf);
        return ret;
    }
    (void)fclose(f);
    return -1;
}

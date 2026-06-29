/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Provides burn uart port template \n
 * History: \n
 * 2022-06-06， Create file. \n
 */
#ifndef SEBOOT_CHANNEL_H
#define SEBOOT_CHANNEL_H

#include "burn_types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    SEBOOT_ACK_LEN = 10,
    CHL_PACKET_START_FLAG = 0xED,
    CHL_PACKET_END_FLAG = 0xEE,
    DECODE_PACKET_SUCCESS = 0,
    DECODE_PACKET_PART = 1,
    DECODE_PACKET_MORE_THAN_ONE = 2,
    DECODE_PACKET_ERROR = 3,
};

#pragma pack(push, 1)

typedef struct {
    u32 start_flag;
    u16 packet_size;
    u8 frame_type;
    u8 frame_type_reverse;
    u32 baud_rate;
    u8 data_bits;
    u8 stop_bits;
    u8 parity;
    u8 flowctrl;
    u16 check_sum;
} seboot_shake_hands_frame_t;

typedef struct {
    u32 start_flag;
    u16 packet_size;
    u8 frame_type;
    u8 frame_type_reverse;
    u8 result;
    u8 error_code;
    u16 check_sum;
} seboot_ack_frame_t;

typedef struct {
    u8 flag;
    u8 packet_num;
    u8 packet_num_reserve;
    u8 data[128];
    u8 crc[2];
} ymodem_start_end_packet_t;

typedef struct {
    u8 flag;
    u8 packet_num;
    u8 packet_num_reserve;
    u8 data[1024];
    u8 crc[2];
} ymodem_info_packet_large_t;

typedef struct {
    u8 flag;
    u8 packet_num;
    u8 packet_num_reserve;
    u8 data[8192];
    u8 crc[2];
} ymodem_info_packet_8k_t;

typedef struct {
    u8 flag;
    u8 packet_num;
    u8 packet_num_reserve;
    u8 data[128];
    u8 crc[2];
} ymodem_info_packet_small_t;

typedef struct {
    u32 start_flag;
    u16 packet_size;
    u8 frame_type;
    u8 frame_type_reverse;
    u32 file_addr;
    u32 file_len;
    u32 erase_size;
    u8 formal;
    u8 formal_reverse;
    u16 check_sum;
} seboot_download_flashimage_t;

typedef struct {
    u32 start_flag;
    u16 packet_size;
    u8 frame_type;
    u8 frame_type_reverse;
    u32 file_len;
    u16 check_sum;
} seboot_download_otp_efuse_t;

typedef struct {
    u32 start_flag;
    u16 packet_size;
    u8 frame_type;
    u8 frame_type_reverse;
    u16 start_bit;
    u16 bit_width;
    u16 check_sum;
} seboot_read_otp_efuse_t;

typedef struct {
    u32 start_flag;
    u16 packet_size;
    u8 frame_type;
    u8 frame_type_reverse;
    u32 file_len;
    u32 file_addr;
    u16 check_sum;
} seboot_upload_data_t;

typedef struct {
    u32 start_flag;
    u16 packet_size;
    u8 frame_type;
    u8 frame_type_reverse;
    u16 param;
    u16 check_sum;
} seboot_flash_lock_t;

typedef struct {
    u32 start_flag;
    u16 packet_size;
    u8 frame_type;
    u8 frame_type_reverse;
    u16 param;
    u16 check_sum;
} seboot_reset_t;

typedef struct {
    u32 start_flag;
    u16 packet_size;
    u8 frame_type;
    u8 frame_type_reverse;
    u32 file_addr;
    u32 file_len;
    u32 erase_size;
    u8 formal;
    u8 formal_reverse;
    u16 check_sum;
} seboot_download_factorybin_t;

typedef struct {
    u32 start_flag;
    u16 packet_size;
    u8 frame_type;
    u8 frame_type_reverse;
    u32 file_len;
    u16 check_sum;
} seboot_download_version_t;

typedef struct {
    u32 start_flag;
    u16 packet_size;
    u8 frame_type;
    u8 frame_type_reverse;
    u32 file_addr;
    u32 file_len;
    u32 erase_size;
    u16 enc_item_cnt;
    u16 flag;
    u16 check_sum;
} seboot_download_nv_t;

typedef struct {
    u32 start_flag;
    u16 packet_size;
    u8 frame_type;
    u8 frame_type_reverse;
    u16 flag;
    u16 check_sum;
} seboot_switch_dfu_t;

#pragma pack(pop)

u16 seboot_crc16(const u8 *data, u32 len);

int seboot_decode_ack(const u8 *src, u32 src_len, seboot_ack_frame_t *out, u32 *res_len);
int ymodem_decode_head_packet(const u8 *src, u32 src_len, ymodem_start_end_packet_t *out, u32 *res_len, u32 *packet_size);
int ymodem_decode_large_packet(const u8 *src, u32 src_len, ymodem_info_packet_large_t *out, u32 *res_len);
int ymodem_decode_8k_packet(const u8 *src, u32 src_len, ymodem_info_packet_8k_t *out, u32 *res_len);
int ymodem_decode_small_packet(const u8 *src, u32 src_len, ymodem_info_packet_small_t *out, u32 *res_len);

int seboot_get_ymodem_stx(u32 size);

#ifdef __cplusplus
}
#endif

#endif /* SEBOOT_CHANNEL_H */

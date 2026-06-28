/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Provides burn uart port template \n
 * History: \n
 * 2022-06-06， Create file. \n
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "seboot_channel.h"
#include "debug_print.h"
#include "securec.h"
#include "watchdog.h"
#include "ymodem.h"
#include "osal_interrupt.h"
#include "burn_ctrl.h"

#define BURN_MAX_RETRY 30
#define BURN_MIN(a, b) ((a) < (b) ? (a) : (b))

void burn_context_init(burn_context_t *ctx, burn_io_t *io)
{
    if (ctx == NULL) {
        return;
    }
    ctx->io = io;
    ctx->start_flag = 0xDEADBEEF;
    ctx->ymodem_packet_size = MODEM_PACKET_SIZE;
    ctx->erase_mode = 0;
    ctx->only_erase_all = 0;
    ctx->timeout_handshake_ms = 20000; // 20000 ms
    ctx->timeout_ack_ms = 15000; // 15000 ms
    ctx->timeout_c_ms = 5000; // 5000 ms
}

void burn_context_set_log(burn_context_t *ctx, burn_log_fn fn, void *user)
{
    if (ctx == NULL) {
        return;
    }
    ctx->log_fn = fn;
    ctx->log_user = user;
}

int burn_open(burn_context_t *ctx, const burn_serial_cfg_t *cfg)
{
    if (ctx == NULL || ctx->io == NULL || cfg == NULL) {
        return BURN_ERR_INVALID_PARAM;
    }
    if (ctx->io->open == NULL) {
        return BURN_ERR_COM_INVALID;
    }
    int ret = ctx->io->open(cfg->baud);
    return (ret == 0) ? BURN_ERR_OK : BURN_ERR_COM_INVALID;
}

void burn_close(burn_context_t *ctx)
{
    reset_2131_cat1_module();
    if (ctx == NULL || ctx->io == NULL || ctx->io->close == NULL) {
        return;
    }
    ctx->io->close(ctx->io->user);
}

static int burn_append_rx(burn_context_t *ctx, const u8 *data, u32 len)
{
    if (ctx == NULL || data == NULL || len == 0) {
        return 0;
    }
    if (ctx->rx_len + len > sizeof(ctx->rx_buf)) {
        return -1;
    }
    (void)memcpy_s(ctx->rx_buf + ctx->rx_len, len, data, len);
    ctx->rx_len += len;
    return 0;
}

static int burn_read_more(burn_context_t *ctx, u32 timeout_ms)
{
    if (ctx == NULL || ctx->io == NULL || ctx->io->read == NULL) {
        return -1;
    }
    u8 tmp[64] = {0}; // size 64
    int read = ctx->io->read(ctx->io->user, tmp, sizeof(tmp), timeout_ms);
    if (read > 0) {
        if (burn_append_rx(ctx, tmp, (u32)read) != 0) {
            return -1;
        }
    }
    return read;
}

static int burn_find_and_consume_char(burn_context_t *ctx, u8 ch)
{
    if (ctx == NULL) {
        return 0;
    }
    for (u32 i = 0; i < ctx->rx_len; i++) {
        if (ctx->rx_buf[i] != ch) {
            continue;
        }
        u32 remain = ctx->rx_len - (i + 1);
        if (remain > 0) {
            if (memmove_s(ctx->rx_buf, BURN_RX_BUF_SIZE, ctx->rx_buf + i + 1, remain) != 0) {
                printf("move failed ! \r\n");
            }
        }
        ctx->rx_len = remain;
        return 1;
    }
    return 0;
}

static int burn_find_and_consume_last_char(burn_context_t *ctx, u8 ch)
{
    if (ctx == NULL || ctx->rx_len == 0) {
        return 0;
    }
    if (ctx->rx_buf[ctx->rx_len - 1] == ch) {
        // Found at the end, clear the entire buffer as previous data are logs
        ctx->rx_len = 0;
        return 1;
    }
    return 0;
}

static int burn_wait_for_char(burn_context_t *ctx, u8 ch, u32 timeout_ms)
{
    if (ctx == NULL || ctx->io == NULL || ctx->io->now_ms == NULL) {
        return -1;
    }
    u64 start = ctx->io->now_ms();
    while ((ctx->io->now_ms() - start) < timeout_ms) {
        int found = 0;

        if (ch == MODEM_C) {
            found = burn_find_and_consume_last_char(ctx, ch);
        } else {
            found = burn_find_and_consume_char(ctx, ch);
        }
        if (found) {
            return 0;
        }
        int r = burn_read_more(ctx, 10);
        if (r < 0) {
            return -1;
        }
    }
    return -1;
}

static int burn_wait_for_ack_or_nak(burn_context_t *ctx, u32 timeout_ms, u8 *out)
{
    if (ctx == NULL || ctx->io == NULL || ctx->io->now_ms == NULL || out == NULL) {
        return -1;
    }
    u64 start = ctx->io->now_ms();
    while ((ctx->io->now_ms() - start) < timeout_ms) {
        if (burn_find_and_consume_char(ctx, MODEM_ACK)) {
            *out = MODEM_ACK;
            return 0;
        }
        if (burn_find_and_consume_char(ctx, MODEM_NAK)) {
            *out = MODEM_NAK;
            return 0;
        }
        if (burn_find_and_consume_char(ctx, MODEM_C)) {
            *out = MODEM_C;
            return 0;
        }
        int r = burn_read_more(ctx, 10);
        if (r < 0) {
            return -1;
        }
    }
    return -1;
}

static void copy_uart_rx_buffer(burn_context_t *ctx, u32 res_len)
{
    if (ctx == NULL) {
        return;
    }
    u32 consumed = ctx->rx_len - res_len;
    if (consumed > 0) {
        if (res_len > 0) {
            if (memmove_s(ctx->rx_buf, BURN_RX_BUF_SIZE, ctx->rx_buf + consumed, res_len) != 0) {
                printf("move failed ! \r\n");
            };
        }
        ctx->rx_len = res_len;
    }
}

static int burn_wait_for_ack_frame(burn_context_t *ctx, u32 timeout_ms, seboot_ack_frame_t *out)
{
    if (ctx == NULL || ctx->io == NULL || ctx->io->now_ms == NULL || out == NULL) {
        return -1;
    }
    u64 start = ctx->io->now_ms();
    while ((ctx->io->now_ms() - start) < timeout_ms) {
        u32 res_len = ctx->rx_len;
        seboot_ack_frame_t ack;
        int decode = seboot_decode_ack(ctx->rx_buf, ctx->rx_len, &ack, &res_len);
        if (decode == DECODE_PACKET_SUCCESS || decode == DECODE_PACKET_MORE_THAN_ONE) {
            copy_uart_rx_buffer(ctx, res_len);
            *out = ack;
            return 0;
        }
        int r = burn_read_more(ctx, 10);
        if (r < 0) {
            return -1;
        }
    }
    return -1;
}

static void burn_build_shake_frame(seboot_shake_hands_frame_t *out, const burn_serial_cfg_t *cfg, u32 start_flag)
{
    out->start_flag = start_flag;
    out->packet_size = (u16)sizeof(seboot_shake_hands_frame_t);
    out->frame_type = 0xF0;
    out->frame_type_reverse = (u8)~out->frame_type;
    out->baud_rate = (cfg != NULL) ? cfg->baud : 115200; // baud 115200
    out->data_bits = (cfg != NULL) ? (u8)cfg->databits : 8; // bits 8
    out->stop_bits = (cfg != NULL) ? (u8)cfg->stopbits : 1;
    out->parity = (cfg != NULL) ? (u8)cfg->parity : 0;
    out->flowctrl = (cfg != NULL) ? (u8)cfg->flowctrl : 0;
    out->check_sum = seboot_crc16((const u8 *)out, out->packet_size - sizeof(out->check_sum));
}
#define HANDSHAKE_BAUD  115200

static int burn_handshake_success(burn_context_t *ctx, u32 target_baud)
{
    PRINT("Handshake OK\n");
    // Switch to target baud rate after successful handshake
    if (ctx->io->configure != NULL && target_baud != HANDSHAKE_BAUD) {
        PRINT("Switching to target baud rate %u\n", target_baud);
        if (ctx->io->configure(target_baud) != 0) {
            PRINT("Failed to switch baud rate %u, continuing with %u\n", target_baud, HANDSHAKE_BAUD);
        }
    }
    return BURN_ERR_OK;
}

static int burn_handshake_failed(burn_context_t *ctx, u32 target_baud)
{
    // Restore original baud rate before returning
    if (ctx->io->configure != NULL && target_baud != HANDSHAKE_BAUD) {
        ctx->io->configure(target_baud);
    }
    return BURN_ERR_SELOADER_ACK_IS_FAIL;
}

int burn_handshake(burn_context_t *ctx, const burn_serial_cfg_t *cfg)
{
    if (ctx == NULL || ctx->io == NULL || ctx->io->write == NULL || ctx->io->now_ms == NULL) {
        return BURN_ERR_INVALID_PARAM;
    }
    ctx->rx_len = 0;
    seboot_shake_hands_frame_t shake = {0};
    burn_build_shake_frame(&shake, cfg, ctx->start_flag);
    PRINT("Handshake baud rate: %d, target baud %d \n", HANDSHAKE_BAUD, cfg->baud);
    u32 target_baud = cfg->baud;
    if (ctx->io->configure != NULL && ctx->io->configure(HANDSHAKE_BAUD) != 0) {
        PRINT("Failed to switch baud rate to %u, continuing with current baud\n", HANDSHAKE_BAUD);
    }
    u64 start = ctx->io->now_ms();
    u64 next_send = 0;
    u32 count = 0;
    u32 status = osal_irq_lock();
    while ((ctx->io->now_ms() - start) < ctx->timeout_handshake_ms) {
        u64 now = ctx->io->now_ms();
        // 发送 打断报文
        if (count++ % 3 == 0) { // 每 3 次失败重新复位一次
            reset_2131_cat1_module();
        }
        if (now >= next_send) {
            ctx->io->write(ctx->io->user, (const u8 *)&shake, sizeof(shake));
            next_send = now + 2; // 2 ms
        }
        seboot_ack_frame_t ack;
        if (burn_wait_for_ack_frame(ctx, 4, &ack) == 0) { // 4 ms
            if (ack.start_flag != ctx->start_flag) {
                continue;
            }
            if (ack.result == 0xA5) {
                PRINT("Handshake failed, error=0x%02X\n", ack.error_code);
                return burn_handshake_failed(ctx, target_baud);
            }
            osal_irq_restore(status);
            return burn_handshake_success(ctx, target_baud);
        }
    }
    osal_irq_restore(status);
    PRINT("Handshake timeout\n");
    // Restore original baud rate before returning
    if (ctx->io->configure != NULL && target_baud != HANDSHAKE_BAUD) {
        ctx->io->configure(target_baud);
    }
    return BURN_ERR_TIMEOUT_WAIT_HISILICON;
}

static int burn_send_frame(burn_context_t *ctx, const void *data, u32 len)
{
    if (ctx == NULL || ctx->io == NULL || ctx->io->write == NULL || data == NULL) {
        return -1;
    }
    // 记录发送的buffer
    int written = ctx->io->write(ctx->io->user, (const u8 *)data, len);
    return (written == (int)len) ? 0 : -1;
}

static int burn_send_cmd_wait_ack(burn_context_t *ctx, const void *frame, u32 len)
{
    if (burn_send_frame(ctx, frame, len) != 0) {
        return BURN_ERR_COM_INVALID;
    }
    seboot_ack_frame_t ack;
    if (burn_wait_for_ack_frame(ctx, ctx->timeout_ack_ms, &ack) != 0) {
        return BURN_ERR_TIMEOUT_WAIT_SELOADER_ACK;
    }
    if (ack.result == 0xA5) {
        PRINT("SEBoot ACK fail, code=0x%02X\n", ack.error_code);
        return BURN_ERR_SELOADER_ACK_IS_FAIL;
    }
    return BURN_ERR_OK;
}

static void burn_fill_download_flash(seboot_download_flashimage_t *st, u32 start_flag)
{
    st->start_flag = start_flag;
    st->packet_size = 0x18;
    st->frame_type = 0xD2;
    st->frame_type_reverse = (u8)~st->frame_type;
    st->formal_reverse = 0xFF;
}

static void burn_fill_download_nv(seboot_download_nv_t *st, u32 start_flag)
{
    st->start_flag = start_flag;
    st->packet_size = 0x1A;
    st->frame_type = 0x4B;
    st->frame_type_reverse = (u8)~st->frame_type;
}

static u32 burn_align_erase(u32 size)
{
    if (size == 0 || size >= 0xF0FFFFFF) {
        return size;
    }
    if ((size % 0x1000) != 0) {
        return (0x1000 * (size / 0x1000 + 1));
    }
    return size;
}

static int make_ymodem_start_message(burn_context_t *ctx, const char *file_name, u32 file_size)
{
    u8 header[133] = {0};
    header[0] = MODEM_SOH;
    header[1] = 0x00;
    header[2] = 0xFF; // offset 2
    u32 idx = 3; // offset 3
    const char *name = (file_name && file_name[0] != '\0') ? file_name : "image.bin";
    size_t name_len = strlen(name);
    if (name_len > 0 && idx + name_len < 3 + 128) { // offset 3 len 128
        (void)memcpy_s(header + idx, name_len, name, name_len);
        idx += (u32)name_len;
    }
    header[idx++] = 0x00;
    char size_str[32] = {0}; // len 32
    (void)snprintf_s(size_str, sizeof(size_str), sizeof(size_str), "%u", file_size);
    size_t size_len = strlen(size_str);
    if (idx + size_len < 3 + 128) { // len 3 + 128
        (void)memcpy_s(header + idx, size_len, size_str, size_len);
    }
    u16 crc = ymodem_crc16(header + 3, 128); // offset 3 len 128
    header[131] = (u8)(crc >> 8); // offset 131
    header[132] = (u8)(crc & 0xFF);  // offset 132
    if (burn_send_frame(ctx, header, sizeof(header)) != 0) {
        return BURN_ERR_COM_INVALID;
    }
    u8 ack = 0;
    if (burn_wait_for_ack_or_nak(ctx, ctx->timeout_ack_ms, &ack) != 0 || ack != MODEM_ACK) {
        return BURN_ERR_TIMEOUT_WAIT_BURN_INIT_ACK;
    }
    if (burn_wait_for_char(ctx, MODEM_C, ctx->timeout_c_ms) != 0) {
        return BURN_ERR_TIMEOUT_WAIT_BURN_INIT_ACK;
    }
    return BURN_ERR_OK;
}

static void send_ymodem_end_message(burn_context_t *ctx)
{
    u8 endpkt[133] = {0}; // len 133
    endpkt[0] = MODEM_SOH;
    endpkt[1] = 0x00;
    endpkt[2] = 0xFF; // offset 2
    u16 end_crc = ymodem_crc16(endpkt + 3, 128); // offset 3 len 128
    endpkt[131] = (u8)(end_crc >> 8); // offset 131 bits 8
    endpkt[132] = (u8)(end_crc & 0xFF); // offset 132
    burn_send_frame(ctx, endpkt, sizeof(endpkt));
}

static int send_ymodem_file_message(burn_context_t *ctx, FILE *f, u32 file_size)
{
    u8 ack = 0;
    u32 packet_size = ctx->ymodem_packet_size;
    u8 stx = (u8)seboot_get_ymodem_stx(packet_size);
    u8 *packet = (u8 *)malloc(packet_size + 5); // len 5
    if (packet == NULL) {
        (void)fclose(f);
        return BURN_ERR_FAILURE;
    }
    u8 pkt_no = 1;
    u32 remaining = file_size;
    while (remaining > 0) {
        packet[0] = stx;
        packet[1] = pkt_no;
        packet[2] = (u8)(0xFF - pkt_no); // offset 2
        u32 to_read = (remaining > packet_size) ? packet_size : remaining;
        size_t read = fread(packet + 3, 1, to_read, f); // len 3
        if (read != to_read) {
            free(packet);
            return BURN_ERR_FILE_ACCESS_FAIL;
        }
        u16 pkt_crc = ymodem_crc16(packet + 3, packet_size); // len 3
        packet[packet_size + 3] = (u8)(pkt_crc >> 8); // offset 3 8
        packet[packet_size + 4] = (u8)(pkt_crc & 0xFF); // offset 4
        int retry = 0;
        while (retry < BURN_MAX_RETRY) {
            if (burn_send_frame(ctx, packet, packet_size + 5) != 0) { // len 5
                free(packet);
                return BURN_ERR_COM_INVALID;
            }
            if (burn_wait_for_ack_or_nak(ctx, ctx->timeout_ack_ms, &ack) == 0 && ack == MODEM_ACK) {
                break;
            }
            retry++;
        }
        if (retry >= BURN_MAX_RETRY) {
            free(packet);
            return BURN_ERR_TIMEOUT_WAIT_BURN_INTER_ACK;
        }
        pkt_no++;
        remaining -= to_read;
    }
    free(packet);
    return BURN_ERR_OK;
}

static int burn_send_ymodem(burn_context_t *ctx, const char *path, const char *file_name,
                            u32 file_size, u32 file_offset)
{
    if (ctx == NULL || path == NULL) {
        return BURN_ERR_INVALID_PARAM;
    }
    char *real_path = realpath(path, NULL);
    if (real_path == NULL) {
        return BURN_ERR_INVALID_PARAM;
    }
    FILE *f = fopen(real_path, "rb");
    if (f == NULL) {
        return BURN_ERR_OPEN_FILE;
    }
    if (fseek(f, (long)file_offset, SEEK_SET) != 0) {
        (void)fclose(f);
        return BURN_ERR_OPEN_FILE;
    }
    if (burn_wait_for_char(ctx, MODEM_C, ctx->timeout_c_ms) != 0) {
        (void)fclose(f);
        return BURN_ERR_TIMEOUT_WAIT_BURN_START_C;
    }
    int rets = make_ymodem_start_message(ctx, file_name, file_size);
    if (rets != BURN_ERR_OK) {
        (void)fclose(f);
        return rets;
    }
    rets = send_ymodem_file_message(ctx, f, file_size);
    if (rets != BURN_ERR_OK) {
        (void)fclose(f);
        return rets;
    }
    u8 ack = 0;
    u8 eot = MODEM_EOT;
    burn_send_frame(ctx, &eot, 1);
    if (burn_wait_for_ack_or_nak(ctx, ctx->timeout_ack_ms, &ack) != 0 || ack != MODEM_ACK) {
        (void)fclose(f);
        return BURN_ERR_TIMEOUT_WAIT_BURN_EOT_C;
    }
    if (burn_wait_for_char(ctx, MODEM_C, ctx->timeout_c_ms) != 0) {
        (void)fclose(f);
        return BURN_ERR_TIMEOUT_WAIT_BURN_EOT_C;
    }
    send_ymodem_end_message(ctx);
    if (burn_wait_for_ack_or_nak(ctx, ctx->timeout_ack_ms, &ack) != 0 || ack != MODEM_ACK) {
        (void)fclose(f);
        return BURN_ERR_TIMEOUT_WAIT_ZERO_ACK;
    }
    (void)fclose(f);
    return BURN_ERR_OK;
}

static int burn_loader_bin(burn_context_t *ctx, const bin_info_t *info)
{
    PRINT("Send loader %s\n", info->file_name);
    int ret = burn_send_ymodem(ctx, info->path, info->file_name, info->file_size, info->file_index);
    if (ret != BURN_ERR_OK) {
        return ret;
    }
    seboot_ack_frame_t ack;
    if (burn_wait_for_ack_frame(ctx, ctx->timeout_ack_ms, &ack) == 0) {
        if (ack.result == 0xA5) {
            PRINT("Post-send ACK fail, code=0x%02X\n", ack.error_code);
            return BURN_ERR_SELOADER_ACK_IS_FAIL;
        }
    }
    PRINT("Send loader success !\n");
    return BURN_ERR_OK;
}

static int burn_kv_nv_bin(burn_context_t *ctx, const bin_info_t *info)
{
    seboot_download_nv_t st = {0};
    burn_fill_download_nv(&st, ctx->start_flag);
    int erase_all_done = 0;
    st.file_len = info->file_size;
    st.file_addr = info->burn_addr;
    st.erase_size = burn_align_erase(info->burn_size);
    if (ctx->erase_mode == 1) {
        st.erase_size = erase_all_done ? 0 : 0xFFFFFFFF;
        erase_all_done = 1;
        st.flag = 1;
    } else if (ctx->erase_mode == 2) { // mode 2
        st.erase_size = 0;
        st.flag = 0;
    } else {
        st.flag = 0;
    }
    st.check_sum = seboot_crc16((const u8 *)&st, st.packet_size - sizeof(st.check_sum));
    return burn_send_cmd_wait_ack(ctx, &st, st.packet_size);
}

static int burn_general_type_bin(burn_context_t *ctx, const bin_info_t *info)
{
    int erase_all_done = 0;
    seboot_download_flashimage_t st = {0};
    burn_fill_download_flash(&st, ctx->start_flag);
    st.file_len = info->file_size;
    st.file_addr = info->burn_addr;
    st.erase_size = burn_align_erase(info->burn_size);
    if (ctx->erase_mode == 1) {
        st.erase_size = erase_all_done ? 0 : 0xFFFFFFFF;
        erase_all_done = 1;
    } else if (ctx->erase_mode == 2) { // mode 2
        st.erase_size = 0;
    }
    if (info->type == IMAGE_TYPE_ROM) {
        st.formal = 2; // mode rom 2
    } else if (info->type == IMAGE_TYPE_EMMC) {
        st.formal = 3; // mode emmc 3
    }
    st.formal_reverse = (u8)~st.formal;
    st.check_sum = seboot_crc16((const u8 *)&st, st.packet_size - sizeof(st.check_sum));
    return burn_send_cmd_wait_ack(ctx, &st, st.packet_size);
}

int burn_execute(burn_context_t *ctx, const bin_info_t *bins, u32 bin_count, u32 radio)
{
    if (ctx == NULL || bins == NULL || bin_count == 0) {
        return BURN_ERR_INVALID_PARAM;
    }
    int ret = BURN_ERR_OK;
    for (u32 i = 0; i < bin_count; i++) {
        const bin_info_t *info = &bins[i];
        if (info->type == IMAGE_TYPE_LOADER) {
            ret = burn_loader_bin(ctx, info);
            continue;
        }
        if (info->type == IMAGE_TYPE_RADIOCONFIG && radio == 0) {
            continue;
        }
        if (info->type == IMAGE_TYPE_DATABASE) {
            continue;
        }
        if (info->type == IMAGE_TYPE_KV_NV) {
            ret = burn_kv_nv_bin(ctx, info);
        } else {
            PRINT("burn request start name = %s \n", info->file_name);
            ret = burn_general_type_bin(ctx, info);
            PRINT("burn request end \n");
        }
        if (ret != BURN_ERR_OK) {
            return ret;
        }
        PRINT("Send size = 0x%x, burn addr = 0x%x, size = 0x%x\n",
            info->file_size, info->burn_addr, info->burn_size);
        ret = burn_send_ymodem(ctx, info->path, info->file_name, info->file_size, info->file_index);
        if (ret != BURN_ERR_OK) {
            return ret;
        }
        seboot_ack_frame_t ack;
        if (burn_wait_for_ack_frame(ctx, ctx->timeout_ack_ms, &ack) == 0) {
            if (ack.result == 0xA5) {
                PRINT("Post-send ACK fail, code=0x%02X\n", ack.error_code);
                return BURN_ERR_SELOADER_ACK_IS_FAIL;
            }
        }
        uapi_watchdog_kick();
    }
    return BURN_ERR_OK;
}

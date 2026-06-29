/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: diag channel adapt api header file
 * This file should be changed only infrequently and with great care.
 */
#ifndef DIAG_CHANNEL_H
#define DIAG_CHANNEL_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "diag_common.h"
#include "dfx_write_interface.h"
#include "dfx_resource_id.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define DIAG_SER_MAX_DATA_LEN_DEFAULT   2048
#define DIAG_SER_DATA_MFS_DEFAULT       1024

#define DIAG_CHANNEL_ATTR_RX_BUF_MASK   0x03

/*
 * 通道属性. 0表示无属性，bit0~bit1标识是否需要RX buffer, 其他bit暂时预留。
 */
typedef enum {
    DIAG_CHANNEL_ATTR_NONE              = 0x0,  /* 无属性 */
    DIAG_CHANNEL_ATTR_NEED_RX_BUF       = 0x1,  /* 需要RX buffer，由diag内部分配 */
    DIAG_CHANNEL_ATTR_NEED_FIXED_RX_BUF = 0x2,  /* 需要RX buffer，由使用者通过uapi_diag_channel_set_rx_buf传入 */
} diag_channel_attribute_t;

typedef int32_t (*diag_channel_tx_hook)(void *fd, dfx_data_type_t data_type, uint8_t *data[], uint16_t len[],
                                        uint8_t cnt);
typedef errcode_t (*diag_channel_notify_hook)(uint32_t id, uint32_t data);

errcode_t uapi_diag_channel_init(diag_channel_id_t id, uint32_t attribute);

/*
 * 设置RX buffer地址及其长度。
 * 必须在uapi_diag_channel_init之后调用。
 * 如初始化attribute为DIAG_CHANNEL_ATTR_NEED_FIXED_RX_BUF，必须调用此函数，传入外部分配的rx_buf地址和长度。
 * 如初始化attribute为DIAG_CHANNEL_ATTR_NEED_RX_BUF，此函数可选调用，如不调用，初始化时已按默认大小分配rx_buf，
 * 如调用此函数，将按新的长度重新分配rx_buf。
 */
errcode_t uapi_diag_channel_set_rx_buf(diag_channel_id_t id, uint8_t *buf, uint32_t buf_len);

errcode_t uapi_diag_channel_deinit(diag_channel_id_t id);

errcode_t uapi_diag_channel_set_tx_hook(diag_channel_id_t id, diag_channel_tx_hook hook);

errcode_t uapi_diag_channel_set_notify_hook(diag_channel_id_t id, diag_channel_notify_hook hook);

/* 字符串数据接收函数，data中是字符数据，可能需要组包(形成完整Diag帧数据)，也可能包含多个帧 */
int32_t uapi_diag_channel_rx_mux_char_data(diag_channel_id_t id, uint8_t *data, uint16_t size);

/* Diag帧数据接收函数，data中是一个完整的Diag帧数据，无需组包 */
errcode_t uapi_diag_channel_rx_frame_data(diag_channel_id_t id, uint8_t *data, uint16_t size);

errcode_t uapi_diag_channel_set_connect_hso_addr(diag_channel_id_t id, uint8_t hso_addr);

/*
 * 设置通道可支持的最大传输能力(MFS) ，当业务层的包长超过MFS时，链路层经已MFS为单位对数据包进行拆包，默认值为1024字节。
 * mfs 最大传输能力。
 * max_serv_pkg_len 最大可支持的业务层数据长度，默认值为2048字节。
 */
errcode_t uapi_diag_channel_set_mfs(diag_channel_id_t id, uint16_t mfs, uint32_t max_serv_pkg_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* DIAG_CHANNEL_H */

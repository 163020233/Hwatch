/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 */

#ifndef SERVICE_STACK_ADAPTER_BASE_H
#define SERVICE_STACK_ADAPTER_BASE_H

#include "cmsis_os2.h"
#if defined(UX_BS_BT_SUPPORT)
#include "bs_api_stru.h"
#endif

#define UUID_SIZE 16
#define BD_ADDR_SIZE    6
#define BD_NAME_MAX_SIZE 32

typedef enum {
    SERVICE_BT_STATUS_SUCCESS,
    SERVICE_BT_STATUS_FAIL,
    SERVICE_BT_STATUS_NOT_READY,
    SERVICE_BT_STATUS_NOMEM,
    SERVICE_BT_STATUS_BUSY,
    SERVICE_BT_STATUS_DONE,
    SERVICE_BT_STATUS_UNSUPPORTED,
    SERVICE_BT_STATUS_PARM_INVALID,
    SERVICE_BT_STATUS_UNHANDLED,
    SERVICE_BT_STATUS_AUTH_FAILURE,         /* remote accepts AUTH request, but AUTH failure */
    SERVICE_BT_STATUS_RMT_DEV_DOWN,         /* remote device not in BT range */
    SERVICE_BT_STATUS_AUTH_REJECTED,        /* remote rejects AUTH request */
    SERVICE_BT_STATUS_RMT_DEV_TERMINATE     /* remote disconnect the link actively */
} service_bt_status;

typedef unsigned char uuid[UUID_SIZE];

typedef unsigned char bd_addr[BD_ADDR_SIZE];

#if !defined(UX_BS_BT_SUPPORT)
/* * Bluetooth ACL connection state */
typedef enum {
    SERVICE_BT_ACL_STATE_CONNECTED = 0,       /* ACL or LE connected */
    SERVICE_BT_SPP_STATE_CONNECTED = 1,       /* SPP connected */
    SERVICE_BT_ACL_STATE_DISCONNECTED = 2,    /* ACL or LE disconnected */
    SERVICE_BT_ACL_STATE_BR_DISCONNECTED = 3  /* br disconnected */
} service_bt_acl_state;
#endif

/* * Bluetooth Bond state */
typedef enum {
    SERVICE_BT_BOND_STATE_NONE,
    SERVICE_BT_BOND_STATE_BONDING,
    SERVICE_BT_BOND_STATE_BONDED,
    SERVICE_BT_BOND_STATE_SDP_DONE
} service_bt_bond_state;

/* * Bluetooth link mode */
typedef enum {
    SERVICE_BT_MODE_ACTIVE,
    SERVICE_BT_MODE_SNIFF
} service_bt_link_mode;

/* * Profile Connection State */
typedef enum {
    SERVICE_PROFILE_DISCONNECTED,
    SERVICE_PROFILE_CONNECTING,
    SERVICE_PROFILE_CONNECTED,
    SERVICE_PROFILE_DISCONNECTING
} service_profile_connection_state;

#if !defined(UX_BS_BT_SUPPORT)
/* AVDTP Codec Type */
typedef enum {
    SERVICE_AVDTP_CODEC_TYPE_SBC,
    SERVICE_AVDTP_CODEC_TYPE_MPEG1_2_AUDIO,
    SERVICE_AVDTP_CODEC_TYPE_MPEG2_4_AAC,
    SERVICE_AVDTP_CODEC_TYPE_ATRAC,
    SERVICE_AVDTP_CODEC_TYPE_OPUS,
    SERVICE_AVDTP_CODEC_TYPE_H263,
    SERVICE_AVDTP_CODEC_TYPE_MPEG4_VSP,
    SERVICE_AVDTP_CODEC_TYPE_H263_PROF3,
    SERVICE_AVDTP_CODEC_TYPE_H263_PROF8,
    SERVICE_AVDTP_CODEC_TYPE_LHDC,
    SERVICE_AVDTP_CODEC_TYPE_LC3,
    SERVICE_AVDTP_CODEC_TYPE_L2HC,
    SERVICE_AVDTP_CODEC_TYPE_HAC,
    SERVICE_AVDTP_CODEC_TYPE_NON_A2DP
} service_acdtp_codec_type;

/* PCM channel mode */
typedef enum {
    SERVICE_CHANNEL_MONO,
    SERVICE_CHANNEL_STEREO
} secvice_channel_mode;

/* PCM encoding format */
typedef enum {
    SERVICE_BIT_WIDTH_8,
    SERVICE_BIT_WIDTH_16
} service_bit_width;

/* Stream config */
typedef struct {
    uint32_t sample_rate;
    uint8_t codec;
    uint8_t channel;
    uint8_t bit_width;
    uint8_t reserved;
} service_a2dp_stream_config_s;

/* * SCO Connection State */
typedef enum {
    SERVICE_HFP_SCO_CONNECTED,
    SERVICE_HFP_SCO_DISCONNECTED,
    SERVICE_HFP_SCO_UNKNOWN
} service_hfp_sco_state;

/* * Codec Type */
typedef enum {
    SERVICE_HFP_CODEC_UNKONWN,              /* init state */
    SERVICE_HFP_CODEC_MSBC,
    SERVICE_HFP_CODEC_CVSD,
    SERVICE_HFP_CODEC_LC3,
} service_hfp_codec_type;

/* hfp config */
typedef struct {
    uint32_t sample_rate;
    uint8_t codec; /* service_hfp_codec_type */
    uint8_t bit_width; /* service_bit_width */
    uint8_t reserved1;
    uint8_t reserved2;
} service_hfp_config_s;
#endif

/* SPP Connection Request Type */
typedef enum {
    SERVICE_SPP_CONNECTED_REQUEST,
    SERVICE_SPP_DISCONNECTED_REQUEST
} service_spp_connection_request;

// ************ux and stack common end************
#endif

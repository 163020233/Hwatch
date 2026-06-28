/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * File          ux_state_tws.h
 * Description:  Audio ux tws state
 */
#ifndef __UX_STATE_TWS_H__
#define __UX_STATE_TWS_H__

#include "ux_types.h"
#include "bt_service.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t in_or_out_box;           /* in_or_out_box */
    uint8_t in_or_out_ear;           /* ux_in_or_out_ear */
    uint8_t other_in_or_out_box;     /* in_or_out_box */
    uint8_t other_in_or_out_ear;     /* ux_in_or_out_ear */
    uint8_t box_open_or_close;       /* ux_cover_open_or_close */
    uint8_t earphone_position;       /* earphone_pos_state */
    uint8_t in_box_or_not;           /* whether earphone is in box or not before factory */
    uint8_t wear_side;               /* ux_api_tws_wear_side_t */
    uint8_t other_wear_side;         /* ux_api_tws_wear_side_t */
} ux_tws_position_state;

typedef struct {
    ux_tws_position_state position_state;
} ux_tws_state;

ux_tws_state *ux_get_tws_state(void);
void ux_tws_state_default_init(void);

uint8_t ux_get_earphone_position(void);
void ux_set_earphone_position(earphone_pos_state state);
ux_tws_position_state *ux_get_position_state(void);
ux_position_to_bt get_tws_position_state(bool is_local);

#ifdef __cplusplus
}
#endif

#endif

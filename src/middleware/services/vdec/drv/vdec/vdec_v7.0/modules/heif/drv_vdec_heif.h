/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2026. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2025-03-19
 */

#ifndef __DRV_VDEC_HEIF_H__
#define __DRV_VDEC_HEIF_H__

#include "drv_vdec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VDEC_HEIF_MAX_ITEM_NUM       3
#define VDEC_HEIF_MAX_EXTENT_NUM     1
#define VDEC_HEIF_MAX_THMB_REF_NUM   1
#define VDEC_HEIF_MAX_INFE_NUM       1
#define VDEC_HEIF_MAX_HVCC_NUM       5
#define VDEC_HEIF_MAX_ISPE_NUM       5
#define VDEC_HEIF_MAX_IROT_NUM       1
#define VDEC_HEIF_MAX_IMIR_NUM       1
#define VDEC_HEIF_MAX_CLAP_NUM       1
#define VDEC_HEIF_MAX_ARRAY_NUM      3
#define VDEC_HEIF_MAX_NALU_NUM       3
#define VDEC_HEIF_MAX_ASSO_NUM       6

#define VDEC_HEIF_ITEM_TYPE_LEN      5

typedef struct {
    td_u8 *start;
    td_u32 length;
    td_u32 offset;
} drv_vdec_heif_stream;

typedef struct {
    td_bool is_used;
    td_u32 primary_item;
} drv_vdec_heif_pitm;

typedef struct {
    td_u16 item_id;
    td_u16 extent_count;
    td_u32 extent_offset[VDEC_HEIF_MAX_EXTENT_NUM];
    td_u32 extent_length[VDEC_HEIF_MAX_EXTENT_NUM];
} drv_vdec_heif_item_extent;

typedef struct {
    td_bool is_used;
    drv_vdec_heif_item_extent item_extent[VDEC_HEIF_MAX_ITEM_NUM];
} drv_vdec_heif_iloc;

typedef struct {
    td_bool is_used;
    td_u32 from_item_id;
    td_u32 to_item_id[VDEC_HEIF_MAX_THMB_REF_NUM];
} drv_vdec_heif_thmb;

typedef struct {
    td_bool is_used;
    td_u32  item_id;
    td_char item_type[VDEC_HEIF_ITEM_TYPE_LEN];
} drv_vdec_heif_infe;

typedef struct {
    td_u8  nal_unit_type;
    td_u16 num_of_nalus;
    td_u16 nal_unit_len[VDEC_HEIF_MAX_NALU_NUM];
    td_u8 *nal_unit_start[VDEC_HEIF_MAX_NALU_NUM];
} drv_vdec_heif_nal_array;

typedef struct {
    td_bool is_used;
    td_u16 prop_index;
    td_u8 num_of_array;
    drv_vdec_heif_nal_array nal_array[VDEC_HEIF_MAX_ARRAY_NUM];
} drv_vdec_heif_hvcc;

typedef struct {
    td_bool is_used;
    td_u16 prop_index;
    td_u32 image_width;
    td_u32 image_height;
} drv_vdec_heif_ispe;

typedef struct {
    td_bool is_used;
    td_u16 prop_index;
    td_u8  angle;
} drv_vdec_heif_irot;

typedef struct {
    td_bool is_used;
    td_u16 prop_index;
    td_u8  axis;
} drv_vdec_heif_imir;

typedef struct {
    td_bool is_used;
    td_u16 prop_index;
    td_u32 clean_aperture_width;
    td_u32 clean_aperture_height;
    td_s32 hor_off;
    td_s32 ver_off;
} drv_vdec_heif_clap;

typedef struct {
    td_u32 item_id;
    td_u32 asso_count;
    td_u8  essential[VDEC_HEIF_MAX_ASSO_NUM];
    td_u16 prop_index[VDEC_HEIF_MAX_ASSO_NUM];
} drv_vdec_heif_asso;

typedef struct {
    td_bool is_used;
    drv_vdec_heif_asso asso[VDEC_HEIF_MAX_ITEM_NUM];
} drv_vdec_heif_ipma;

typedef struct {
    td_bool is_used;
    td_u8 *start;
    td_u32 offset;
    td_u32 length;
} drv_vdec_heif_mdat;

typedef struct {
    drv_vdec_heif_pitm pitm;
    drv_vdec_heif_iloc iloc;
    drv_vdec_heif_thmb thmb;
    drv_vdec_heif_infe infe[VDEC_HEIF_MAX_INFE_NUM];
    drv_vdec_heif_hvcc hvcc[VDEC_HEIF_MAX_HVCC_NUM];
    drv_vdec_heif_ispe ispe[VDEC_HEIF_MAX_ISPE_NUM];
    drv_vdec_heif_irot irot[VDEC_HEIF_MAX_IROT_NUM];
    drv_vdec_heif_imir imir[VDEC_HEIF_MAX_IMIR_NUM];
    drv_vdec_heif_clap clap[VDEC_HEIF_MAX_CLAP_NUM];
    drv_vdec_heif_ipma ipma;
    drv_vdec_heif_mdat mdat;
} drv_vdec_heif_boxes;

typedef struct {
    td_u32 width;
    td_u32 height;
    td_u32 total_stream_len;
} drv_vdec_heif_pic;

typedef struct {
    ext_vdec_orientation orientation;
    ext_vdec_frame_rect frame_rect;
} drv_vdec_heif_format;

typedef struct {
    drv_vdec_heif_boxes boxes;
    drv_vdec_heif_pic orig_pic;
    drv_vdec_heif_pic thmb_pic;
    drv_vdec_heif_format format;
} drv_vdec_heif_info;

td_s32 drv_vdec_heif_parse_info(drv_vdec_heif_stream *stream, drv_vdec_heif_info *heif_info);
td_s32 drv_vdec_heif_fill_stream(ext_vdec_heif_select select, const drv_vdec_heif_info *heif_info, drv_vdec_heif_stream *stream);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


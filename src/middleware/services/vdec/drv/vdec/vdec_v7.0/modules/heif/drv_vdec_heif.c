/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2026. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2025-03-19
 */

#include "drv_vdec_heif.h"
#include "securec.h"

#define HEIF_START_CODE_LEN    4
#define HEIF_TIFF_HEADER_LEN   2

#define HEIF_SIZE_BYTE_0       0
#define HEIF_SIZE_BYTE_4       4
#define HEIF_SIZE_BYTE_8       8

#define TIFF_BIG_END_MM        0x4D4D
#define TIFF_LIT_END_II        0x4949
#define TIFF_MAGIC_NUM         42

#define TIFF_TAG_ORIENTATION   0x0112

#ifdef VDEC_DEBUG_SUPPORT
#define vdec_heif_trace(fmt, args...)    printf(fmt, ##args)
#else
#define vdec_heif_trace(fmt, args...)    vdec_print_info(fmt, ##args)
#endif

typedef enum {
    HEIF_VERSION_0 = 0,
    HEIF_VERSION_1,
    HEIF_VERSION_2,
    HEIF_VERSION_3,
    HEIF_VERSION_MAX
} vdec_heif_version;

typedef enum {
    HEIF_FIELD_SIZE_0 = 0,
    HEIF_FIELD_SIZE_1,
    HEIF_FIELD_SIZE_2,
    HEIF_FIELD_SIZE_3,
    HEIF_FIELD_SIZE_4,
    HEIF_FIELD_SIZE_5,
    HEIF_FIELD_SIZE_6,
    HEIF_FIELD_SIZE_7,
    HEIF_FIELD_SIZE_8,
    HEIF_FIELD_SIZE_MAX
} vdec_heif_field_size;

typedef struct {
    td_char type[HEIF_FIELD_SIZE_4 + 1];      // Box类型（4字节+1结束符）
    td_u64 size;                              // Box总大小
    td_u64 header_size;                       // Box头部大小
    td_u64 content_size;                      // Box内容大小（不含头部）
    td_u8  version;                           // Box版本号
    td_u32 flags;                             // Box标志位
} drv_vdec_heif_box_header;

STATIC td_u16 be16_to_host(td_u16 val)
{
    return ((val & 0xFF00) >> 8) | ((val & 0x00FF) << 8);  // 8 bit num
}

STATIC td_u32 be32_to_host(td_u32 val)
{
    return ((val & 0xFF000000) >> 24) |  // 24 bit num
           ((val & 0x00FF0000) >> 8)  |  // 8  bit num
           ((val & 0x0000FF00) << 8)  |  // 8  bit num
           ((val & 0x000000FF) << 24);   // 24 bit num
}

STATIC td_u64 be64_to_host(td_u64 val)
{
    td_u32 high = (td_u32)(val >> 32);  // 32 bit num
    td_u32 low = (td_u32)val;

    return ((td_u64)be32_to_host(low) << 32) | be32_to_host(high);  // 32 bit num
}

STATIC td_s32 drv_vdec_heif_check_offset(drv_vdec_heif_stream *parser, td_u64 need_size)
{
    if (parser->offset + need_size > parser->length) {
        vdec_print_err("heif overflow: offset %u + need_size %llu > file length %u.\n", parser->offset, need_size, parser->length);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_read(drv_vdec_heif_stream *parser, td_void *buf, td_u64 size)
{
    const td_u8 *src_ptr = TD_NULL;

    if (size == 0) {
        return EXT_SUCCESS;
    }

    vdec_check_func_ret(drv_vdec_heif_check_offset(parser, size));

    src_ptr = parser->start + parser->offset;

    (td_void)memcpy_s(buf, size, src_ptr, size);

    parser->offset += size;

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_read_be_cvt(drv_vdec_heif_stream *parser, td_void *buf, td_u64 size)
{
    const td_u8 *src_ptr = TD_NULL;

    if (size == 0) {
        return EXT_SUCCESS;
    }

    vdec_check_func_ret(drv_vdec_heif_check_offset(parser, size));

    src_ptr = parser->start + parser->offset;

    switch (size) {
        case 1: // 1 bytes: 8位数据，直接拷贝
            *((td_u8*)buf) = *src_ptr;
            break;

        case 2: // 2 bytes: 16位大端转主机序
            *((td_u16*)buf) = be16_to_host(*((const td_u16*)src_ptr));
            break;

        case 4: // 4 bytes: 32位大端转主机序
            *((td_u32*)buf) = be32_to_host(*((const td_u32*)src_ptr));
            break;

        case 8: // 8 bytes: 64位大端转主机序
            *((td_u64*)buf) = be64_to_host(*((const td_u64*)src_ptr));
            break;

        default: // 其他长度：报错拦截
            vdec_print_err("not support size %llu big end convert.\n", size);
            return EXT_FAILURE;
    }

    parser->offset += size;

    return EXT_SUCCESS;
}

STATIC td_char *drv_vdec_heif_read_string(drv_vdec_heif_stream *parser)
{
    td_u32 offset;
    td_char *string = TD_NULL;

    string = (td_char *)(parser->start + parser->offset);
    if ((strlen(string) == 0)) {
        parser->offset += 1;
        return TD_NULL;
    }

    offset = strlen(string) + 1;

    vdec_check_ret_val(drv_vdec_heif_check_offset(parser, offset) == EXT_SUCCESS, TD_NULL);

    parser->offset += offset;

    return string;
}

STATIC td_s32 drv_vdec_heif_skip(drv_vdec_heif_stream *parser, td_u64 size)
{
    if (size == 0) {
        return EXT_SUCCESS;
    }

    vdec_check_func_ret(drv_vdec_heif_check_offset(parser, size));

    parser->offset += size;

    return EXT_SUCCESS;
}

STATIC td_bool drv_vdec_heif_is_full_box(td_char *box_type)
{
    if (strncmp(box_type, "meta", HEIF_FIELD_SIZE_4) == 0 ||
        strncmp(box_type, "hdlr", HEIF_FIELD_SIZE_4) == 0 ||
        strncmp(box_type, "pitm", HEIF_FIELD_SIZE_4) == 0 ||
        strncmp(box_type, "iloc", HEIF_FIELD_SIZE_4) == 0 ||
        strncmp(box_type, "iinf", HEIF_FIELD_SIZE_4) == 0 ||
        strncmp(box_type, "infe", HEIF_FIELD_SIZE_4) == 0 ||
        strncmp(box_type, "iref", HEIF_FIELD_SIZE_4) == 0 ||
        strncmp(box_type, "ispe", HEIF_FIELD_SIZE_4) == 0 ||
        strncmp(box_type, "ipma", HEIF_FIELD_SIZE_4) == 0 ||
        strncmp(box_type, "pixi", HEIF_FIELD_SIZE_4) == 0) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

STATIC td_s32 drv_vdec_heif_parse_box_header(drv_vdec_heif_stream *parser, drv_vdec_heif_box_header *header)
{
    td_bool is_full_box = TD_FALSE;
    td_u32 size32;

    (td_void)memset_s(header, sizeof(drv_vdec_heif_box_header), 0, sizeof(drv_vdec_heif_box_header));

    // 读取4字节大小
    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &size32, HEIF_FIELD_SIZE_4));
    header->header_size += HEIF_FIELD_SIZE_4;

    // 读取4字节Box类型
    vdec_check_func_ret(drv_vdec_heif_read(parser, header->type, HEIF_FIELD_SIZE_4));
    header->type[HEIF_FIELD_SIZE_4] = '\0';
    header->header_size += HEIF_FIELD_SIZE_4;

    // 处理Full Box类型
    if (drv_vdec_heif_is_full_box(header->type) == TD_TRUE) {
        td_u8 flags[HEIF_FIELD_SIZE_3];

        is_full_box = TD_TRUE;
        vdec_check_func_ret(drv_vdec_heif_read(parser, &header->version, HEIF_FIELD_SIZE_1));
        if (header->version > HEIF_VERSION_3) {
            vdec_print_err("full box %.4s version %u out onf range[%u,%u].\n", header->type, header->version, HEIF_VERSION_1, HEIF_VERSION_3);
            return EXT_FAILURE;
        }

        vdec_check_func_ret(drv_vdec_heif_read(parser, &flags[0x0], HEIF_FIELD_SIZE_1));
        vdec_check_func_ret(drv_vdec_heif_read(parser, &flags[0x1], HEIF_FIELD_SIZE_1));
        vdec_check_func_ret(drv_vdec_heif_read(parser, &flags[0x2], HEIF_FIELD_SIZE_1));
        header->flags = (flags[0x0] << 16) | (flags[0x1] << 8) | flags[0x2];  // 16,8 bit shift

        header->header_size += HEIF_FIELD_SIZE_4;
    }

    // 处理超大Box（size = 1，需要额外8字节长度）
    if (size32 == 1) {
        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &header->size, HEIF_FIELD_SIZE_8));
        header->header_size += HEIF_FIELD_SIZE_8;
    } else {
        header->size = (td_u64)size32;
    }

    if (header->size < header->header_size) {
        vdec_print_err("box %.4s size %llu <= header_size %llu invalid.\n", header->type, header->size, header->header_size);
        return EXT_FAILURE;
    }

    header->content_size = header->size - header->header_size;

    // 校验合法性
    vdec_check_func_ret(drv_vdec_heif_check_offset(parser, header->content_size));

    // 打印Box信息（可选）
    if (is_full_box == TD_FALSE) {
        vdec_heif_trace("box: %.4s | size: %llu | content: %llu\n", header->type, header->size, header->content_size);
    } else {
        vdec_heif_trace("full box: %.4s | size: %llu | content: %llu | version: %u\n", header->type, header->size, header->content_size, header->version);
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_parse_ftyp(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info, const drv_vdec_heif_box_header *header)
{
    td_u32 major_brand;      // 主品牌（4 字符）
    td_u32 minor_version;    // 版本号
    td_u32 compatible_brand; // 兼容品牌
    td_u32 remaining_bytes;
    td_u64 content_size = header->content_size;

    (td_void)(heif_info);

    vdec_check_func_ret(drv_vdec_heif_read(parser, &major_brand, HEIF_FIELD_SIZE_4));
    vdec_heif_trace("- major_brand: %.4s\n", (td_char *)&major_brand);

    vdec_check_func_ret(drv_vdec_heif_read(parser, &minor_version, HEIF_FIELD_SIZE_4));
    vdec_heif_trace("- minor_version: %u\n", minor_version);

    remaining_bytes = content_size - HEIF_FIELD_SIZE_4 - HEIF_FIELD_SIZE_4;
    vdec_heif_trace("- compatible brands: ");
    while (remaining_bytes >= HEIF_FIELD_SIZE_4) {
        vdec_check_func_ret(drv_vdec_heif_read(parser, &compatible_brand, HEIF_FIELD_SIZE_4));
        vdec_heif_trace("%.4s ", (td_char *)&compatible_brand);
        remaining_bytes -= HEIF_FIELD_SIZE_4;
    }
    vdec_heif_trace("\n");

    vdec_check_func_ret(drv_vdec_heif_skip(parser, remaining_bytes));

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_parse_hdlr(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info, const drv_vdec_heif_box_header *header)
{
    td_u32 handler_type;
    td_u64 content_size = header->content_size;

    (td_void)(heif_info);

    vdec_check_func_ret(drv_vdec_heif_skip(parser, HEIF_FIELD_SIZE_4));

    vdec_check_func_ret(drv_vdec_heif_read(parser, &handler_type, HEIF_FIELD_SIZE_4));
    vdec_heif_trace("- handler_type: %.4s\n", (td_char *)&handler_type);

    vdec_check_func_ret(drv_vdec_heif_skip(parser, content_size - HEIF_FIELD_SIZE_8));

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_parse_pitm(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info, const drv_vdec_heif_box_header *header)
{
    td_u32 primary_item;
    td_u8  version = header->version;
    td_u64 content_size = header->content_size;
    drv_vdec_heif_pitm *pitm = &heif_info->boxes.pitm;

    vdec_check_ret(pitm->is_used == TD_FALSE);

    if (version == HEIF_VERSION_0) {
        td_u16 primary_item_16;
        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &primary_item_16, HEIF_FIELD_SIZE_2));
        primary_item = (td_u32)primary_item_16;
    } else {
        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &primary_item, HEIF_FIELD_SIZE_4));
    }

    vdec_heif_trace("- primary_item: %u\n", primary_item);

    vdec_check_func_ret(drv_vdec_heif_skip(parser, content_size - HEIF_FIELD_SIZE_2));

    pitm->primary_item = primary_item;
    pitm->is_used = TD_TRUE;

    return EXT_SUCCESS;
}

static td_s32 drv_vdec_heif_parse_iloc(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info, const drv_vdec_heif_box_header *header)
{
    td_u8  byte1, byte2, offset_size, length_size, base_offset_size, index_size;
    td_u16 i, j, item_count, item_id, extent_count;
    td_u32 base_offset = 0, extent_index, extent_offset = 0, extent_length = 0;
    td_u8  version = header->version;
    drv_vdec_heif_iloc *iloc = &heif_info->boxes.iloc;
    drv_vdec_heif_pitm *pitm = &heif_info->boxes.pitm;

    vdec_check_ret(iloc->is_used == TD_FALSE);

    vdec_check_func_ret(drv_vdec_heif_read(parser, &byte1, HEIF_FIELD_SIZE_1));
    vdec_check_func_ret(drv_vdec_heif_read(parser, &byte2, HEIF_FIELD_SIZE_1));

    offset_size = (byte1 >> 4) & 0x0F;  // 4 bit num
    length_size = byte1 & 0x0F;
    base_offset_size = (byte2 >> 4) & 0x0F;  // 4 bit num
    index_size = byte2 & 0x0F;

    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &item_count, HEIF_FIELD_SIZE_2));
    vdec_heif_trace("- item_count: %u\n", item_count);

    for (i = 0; i < item_count; i++) {
        drv_vdec_heif_item_extent *item_extent = TD_NULL;

        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &item_id, HEIF_FIELD_SIZE_2));
        vdec_heif_trace("  > item_id: %u\n", item_id);
        if (i < VDEC_HEIF_MAX_ITEM_NUM) {
            item_extent = &iloc->item_extent[i];
        } else {
            item_extent = (pitm->is_used == TD_TRUE && pitm->primary_item == item_id) ? &iloc->item_extent[0] : TD_NULL;
        }

        if (version == HEIF_VERSION_1) {
            vdec_check_func_ret(drv_vdec_heif_skip(parser, HEIF_FIELD_SIZE_2)); // skip construction_method
        }

        vdec_check_func_ret(drv_vdec_heif_skip(parser, HEIF_FIELD_SIZE_2)); // skip data_reference_index

        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &base_offset, base_offset_size));

        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &extent_count, HEIF_FIELD_SIZE_2));
        if (extent_count > VDEC_HEIF_MAX_EXTENT_NUM) {
            vdec_print_err("extent_count %u > max_extent_num %u unsupport.\n", extent_count, VDEC_HEIF_MAX_EXTENT_NUM);
            return EXT_FAILURE;
        }

        for (j = 0; j < extent_count; j++) {
            if (version == HEIF_VERSION_1) {
                vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &extent_index, index_size));
            }

            vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &extent_offset, offset_size));
            vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &extent_length, length_size));
            vdec_heif_trace("    extent %u base_offset %u extent_offset %u length %u\n", j, base_offset, extent_offset, extent_length);

            if (item_extent != TD_NULL) {
                item_extent->extent_offset[j] = base_offset + extent_offset;
                item_extent->extent_length[j] = extent_length;
            }
        }

        if (item_extent != TD_NULL) {
            item_extent->item_id = item_id;
            item_extent->extent_count = extent_count;
        }
    }

    iloc->is_used = TD_TRUE;

    return EXT_SUCCESS;
}

static td_s32 drv_vdec_heif_parse_infe(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info, const drv_vdec_heif_box_header *header)
{
    td_u32  item_id;
    td_char item_type[VDEC_HEIF_ITEM_TYPE_LEN];
    td_u16 i, item_protection_index;
    char *item_name, *content_type, *content_encoding;
    td_u8 version = header->version;
    td_u64 content_size = header->content_size;
    td_u32 start_offset = parser->offset;

    if (version == HEIF_VERSION_2) {
        td_u16 item_id_16;
        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &item_id_16, HEIF_FIELD_SIZE_2));
        item_id = (td_u32)item_id_16;
    } else if (version == HEIF_VERSION_3) {
        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &item_id, HEIF_FIELD_SIZE_4));
    } else {
        vdec_print_err("unsupported infe version: %d\n", version);
        return EXT_FAILURE;
    }
    vdec_heif_trace("- item_id: %u\n", item_id);

    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &item_protection_index, HEIF_FIELD_SIZE_2));

    vdec_check_func_ret(drv_vdec_heif_read(parser, item_type, HEIF_FIELD_SIZE_4));
    item_type[VDEC_HEIF_ITEM_TYPE_LEN - 1] = '\0';
    vdec_heif_trace("- item_type: %.4s\n", item_type);

    item_name = drv_vdec_heif_read_string(parser);
    if (item_name != TD_NULL) {
        vdec_heif_trace("- item_name: %s\n", item_name);
    }

    if (strncmp(item_type, "mime", HEIF_FIELD_SIZE_4) == 0) {
        content_type = drv_vdec_heif_read_string(parser);
        if (content_type != TD_NULL) {
            vdec_heif_trace("- content_type: %s\n", content_type);
        }

        if (parser->offset - start_offset < content_size) {
            content_encoding = drv_vdec_heif_read_string(parser);
            if (content_encoding != TD_NULL) {
                vdec_heif_trace("- content_encoding: %s\n", content_encoding);
            }
        }
    } else if (strncmp(item_type, "Exif", HEIF_FIELD_SIZE_4) == 0) {
        drv_vdec_heif_infe *infe = TD_NULL;

        for (i = 0; i < VDEC_HEIF_MAX_INFE_NUM; i++) {
            if (heif_info->boxes.infe[i].is_used == TD_FALSE) {
                infe = &heif_info->boxes.infe[i];
                break;
            }
        }

        vdec_check_ret(infe != TD_NULL);

        infe->item_id = item_id;
        (td_void)memcpy_s(infe->item_type, sizeof(infe->item_type), item_type, sizeof(item_type));
        infe->is_used = TD_TRUE;
    } else if (strncmp(item_type, "grid", HEIF_FIELD_SIZE_4) == 0 || strncmp(item_type, "iden", HEIF_FIELD_SIZE_4) == 0) {
        vdec_print_err("item_type %.4s unsupport\n", item_type);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_parse_iinf(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info, const drv_vdec_heif_box_header *header)
{
    td_s32 ret;
    td_u32 i, entry_count;
    td_u8  version = header->version;
    drv_vdec_heif_box_header sub_header = {0};

    if (version == HEIF_VERSION_0) {
        td_u16 entry_count_16;
        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &entry_count_16, HEIF_FIELD_SIZE_2));
        entry_count = (td_u32)entry_count_16;
    } else {
        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &entry_count, HEIF_FIELD_SIZE_4));
    }
    vdec_heif_trace("- entry_count: %u\n", entry_count);

    for (i = 0; i < entry_count; i++) {
        ret = drv_vdec_heif_parse_box_header(parser, &sub_header);
        vdec_check_ret(ret == EXT_SUCCESS);

        if (strncmp(sub_header.type, "infe", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_infe(parser, heif_info, &sub_header);
            vdec_check_ret(ret == EXT_SUCCESS);
        } else {
            vdec_heif_trace("unsupport box %.4s\n", sub_header.type);
            ret = drv_vdec_heif_skip(parser, sub_header.content_size);
            vdec_check_ret(ret == EXT_SUCCESS);
        }
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_parse_thmb(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info, const drv_vdec_heif_box_header *header)
{
    td_bool use_thmb;
    td_u16 i, reference_count;
    td_u32 from_item_id;
    td_u32 to_item_id[VDEC_HEIF_MAX_THMB_REF_NUM];
    td_u8  version = header->version;
    drv_vdec_heif_thmb *thmb = &heif_info->boxes.thmb;
    drv_vdec_heif_pitm *pitm = &heif_info->boxes.pitm;

    (td_void)(header);

    use_thmb = (pitm->is_used == TD_FALSE) ? TD_TRUE : TD_FALSE;

    if (version == HEIF_VERSION_0) {
        td_u16 from_item_id_16;
        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &from_item_id_16, HEIF_FIELD_SIZE_2));
        from_item_id = from_item_id_16;
    } else {
        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &from_item_id, HEIF_FIELD_SIZE_4));
    }

    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &reference_count, HEIF_FIELD_SIZE_2));
    if (reference_count > VDEC_HEIF_MAX_THMB_REF_NUM) {
        vdec_print_err("reference_count %u > max_reference_num %u unsupport.\n", reference_count, VDEC_HEIF_MAX_THMB_REF_NUM);
        return EXT_FAILURE;
    }

    vdec_heif_trace("- from item_id %u\n", from_item_id);
    if (pitm->primary_item == from_item_id) {
        use_thmb = TD_TRUE;
    }

    for (i = 0; i < reference_count; i++) {
        if (version == HEIF_VERSION_0) {
            td_u16 to_item_id_16;
            vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &to_item_id_16, HEIF_FIELD_SIZE_2));
            to_item_id[i] = to_item_id_16;
        } else {
            vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &to_item_id[i], HEIF_FIELD_SIZE_4));
        }
        vdec_heif_trace("  > to item_id %u\n", to_item_id[i]);
        if (pitm->primary_item == to_item_id[i]) {
            use_thmb = TD_TRUE;
        }
    }

    if (use_thmb == TD_TRUE && thmb->is_used == TD_FALSE) {
        thmb->is_used = TD_TRUE;
        thmb->from_item_id = from_item_id;
        for (i = 0; i < reference_count; i++) {
            thmb->to_item_id[i] = to_item_id[i];
        }
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_parse_cdsc(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info, const drv_vdec_heif_box_header *header)
{
    td_u16 i, reference_count;
    td_u32 from_item_id, to_item_id;
    td_u8  version = header->version;

    (td_void)(heif_info);

    if (version == HEIF_VERSION_0) {
        td_u16 from_item_id_16;
        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &from_item_id_16, HEIF_FIELD_SIZE_2));
        from_item_id = from_item_id_16;
    } else {
        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &from_item_id, HEIF_FIELD_SIZE_4));
    }

    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &reference_count, HEIF_FIELD_SIZE_2));

    vdec_heif_trace("- from item_id %u\n", from_item_id);

    for (i = 0; i < reference_count; i++) {
        if (version == HEIF_VERSION_0) {
            td_u16 to_item_id_16;
            vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &to_item_id_16, HEIF_FIELD_SIZE_2));
            to_item_id = to_item_id_16;
        } else {
            vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &to_item_id, HEIF_FIELD_SIZE_4));
        }

        vdec_heif_trace("  > to item_id %u\n", to_item_id);
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_parse_iref(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info, const drv_vdec_heif_box_header *header)
{
    td_s32 ret;
    td_u32 start_offset, consume_size;
    drv_vdec_heif_box_header sub_header = {0};

    start_offset = parser->offset;

    if (header->content_size == 0) {
        return EXT_SUCCESS;
    }

    do {
        ret = drv_vdec_heif_parse_box_header(parser, &sub_header);
        if (ret != EXT_SUCCESS) {
            break;
        }

        sub_header.version = header->version;

        if (strncmp(sub_header.type, "thmb", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_thmb(parser, heif_info, &sub_header);
        } else if (strncmp(sub_header.type, "cdsc", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_cdsc(parser, heif_info, &sub_header);
        } else {
            vdec_heif_trace("unsupport box %.4s\n", sub_header.type);
            ret = drv_vdec_heif_skip(parser, sub_header.content_size);
        }

        vdec_check_ret(ret == EXT_SUCCESS);

        consume_size = parser->offset - start_offset;
    } while (consume_size < header->content_size);

    return ret;
}

static td_s32 drv_vdec_heif_parse_hvcc(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info, const drv_vdec_heif_box_header *header, td_u16 prop_index)
{
    td_u32 i, j;
    td_u8 configuration_version, nal_unit_type, num_of_arrays;
    td_u16 num_of_nalus, nal_unit_len;
    drv_vdec_heif_hvcc *hvcc = TD_NULL;

    for (i = 0; i < VDEC_HEIF_MAX_HVCC_NUM; i++) {
        if (heif_info->boxes.hvcc[i].is_used == TD_FALSE) {
            hvcc = &heif_info->boxes.hvcc[i];
            break;
        }
    }

    vdec_check_ret(hvcc != TD_NULL);

    (td_void)(header);

    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &configuration_version, HEIF_FIELD_SIZE_1));
    vdec_heif_trace("- configuration_version: %u\n", configuration_version);

    vdec_check_func_ret(drv_vdec_heif_skip(parser, HEIF_FIELD_SIZE_1)); // skip profile_space/tier_flag/profile_idc
    vdec_check_func_ret(drv_vdec_heif_skip(parser, HEIF_FIELD_SIZE_4)); // skip profile_compatibility_flags
    vdec_check_func_ret(drv_vdec_heif_skip(parser, HEIF_FIELD_SIZE_6)); // skip constraint_indicator_flags
    vdec_check_func_ret(drv_vdec_heif_skip(parser, HEIF_FIELD_SIZE_1)); // skip level_idc
    vdec_check_func_ret(drv_vdec_heif_skip(parser, HEIF_FIELD_SIZE_2)); // skip min_spatial_segmentation_idc
    vdec_check_func_ret(drv_vdec_heif_skip(parser, HEIF_FIELD_SIZE_1)); // skip parallelism_type
    vdec_check_func_ret(drv_vdec_heif_skip(parser, HEIF_FIELD_SIZE_1)); // skip chroma_format_idc
    vdec_check_func_ret(drv_vdec_heif_skip(parser, HEIF_FIELD_SIZE_1)); // skip bit_depth_luma_minus8
    vdec_check_func_ret(drv_vdec_heif_skip(parser, HEIF_FIELD_SIZE_1)); // skip bit_depth_chroma_minus8
    vdec_check_func_ret(drv_vdec_heif_skip(parser, HEIF_FIELD_SIZE_2)); // skip avg_frame_rate
    vdec_check_func_ret(drv_vdec_heif_skip(parser, HEIF_FIELD_SIZE_1)); // skip constant_frame_rate/num_temporal_id_nested/length_size_minus1

    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &num_of_arrays, HEIF_FIELD_SIZE_1));
    if (num_of_arrays > VDEC_HEIF_MAX_ARRAY_NUM) {
        vdec_print_err("num_of_arrays %u > max_array_num %u unsupport.\n", num_of_arrays, VDEC_HEIF_MAX_ARRAY_NUM);
        return EXT_FAILURE;
    }
    vdec_heif_trace("- num_of_arrays: %u\n", num_of_arrays);
    hvcc->num_of_array = num_of_arrays;

    for (i = 0; i < num_of_arrays; i++) {
        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &nal_unit_type, HEIF_FIELD_SIZE_1));
        nal_unit_type = nal_unit_type & 0x3F;

        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &num_of_nalus, HEIF_FIELD_SIZE_2));
        if (num_of_nalus > VDEC_HEIF_MAX_NALU_NUM) {
            vdec_print_err("num_of_nalus %u > max_nalu_num %u unsupport.\n", num_of_nalus, VDEC_HEIF_MAX_NALU_NUM);
            return EXT_FAILURE;
        }

        hvcc->nal_array[i].nal_unit_type = nal_unit_type;
        hvcc->nal_array[i].num_of_nalus = num_of_nalus;

        vdec_heif_trace("  > nal_unit_type %u num_of_nalus %u\n", nal_unit_type, num_of_nalus);

        for (j = 0; j < num_of_nalus; j++) {
            vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &nal_unit_len, HEIF_FIELD_SIZE_2));
            vdec_heif_trace("    nalu %u len %u\n", j, nal_unit_len);

            hvcc->nal_array[i].nal_unit_len[j] = nal_unit_len;
            hvcc->nal_array[i].nal_unit_start[j] = parser->start + parser->offset;

            vdec_check_func_ret(drv_vdec_heif_skip(parser, nal_unit_len)); // nalu unit payload
        }
    }

    hvcc->prop_index = prop_index;
    hvcc->is_used = TD_TRUE;

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_parse_ispe(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info, const drv_vdec_heif_box_header *header, td_u16 prop_index)
{
    td_u32 i, image_width, image_height;
    drv_vdec_heif_ispe *ispe = TD_NULL;

    for (i = 0; i < VDEC_HEIF_MAX_ISPE_NUM; i++) {
        if (heif_info->boxes.ispe[i].is_used == TD_FALSE) {
            ispe = &heif_info->boxes.ispe[i];
            break;
        }
    }

    vdec_check_ret(ispe != TD_NULL);

    (td_void)(header);

    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &image_width, HEIF_FIELD_SIZE_4));
    vdec_heif_trace("- image_width: %u\n", image_width);

    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &image_height, HEIF_FIELD_SIZE_4));
    vdec_heif_trace("- image_height: %u\n", image_height);

    ispe->image_width = image_width;
    ispe->image_height = image_height;
    ispe->prop_index = prop_index;
    ispe->is_used = TD_TRUE;

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_parse_irot(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info, const drv_vdec_heif_box_header *header, td_u16 prop_index)
{
    td_u8 i, angle;
    drv_vdec_heif_irot *irot = TD_NULL;

    (td_void)(header);

    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &angle, HEIF_FIELD_SIZE_1));
    angle = angle & 0x3;
    vdec_heif_trace("- angle: %u\n", angle);

    for (i = 0; i < VDEC_HEIF_MAX_IROT_NUM; i++) {
        if (heif_info->boxes.irot[i].is_used == TD_FALSE) {
            irot = &heif_info->boxes.irot[i];
            irot->angle = angle;
            irot->prop_index = prop_index;
            irot->is_used = TD_TRUE;
            break;
        }
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_parse_imir(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info, const drv_vdec_heif_box_header *header, td_u16 prop_index)
{
    td_u8 i, axis;
    drv_vdec_heif_imir *imir = TD_NULL;

    (td_void)(header);

    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &axis, HEIF_FIELD_SIZE_1));
    axis = axis & 0x1;
    vdec_heif_trace("- axis: %u\n", axis);

    for (i = 0; i < VDEC_HEIF_MAX_IMIR_NUM; i++) {
        if (heif_info->boxes.imir[i].is_used == TD_FALSE) {
            imir = &heif_info->boxes.imir[i];
            imir->axis = axis;
            imir->prop_index = prop_index;
            imir->is_used = TD_TRUE;
            break;
        }
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_parse_clap(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info, const drv_vdec_heif_box_header *header, td_u16 prop_index)
{
    td_u8  i;
    td_u32 clean_aperture_width_n, clean_aperture_width_d, clean_aperture_height_n, clean_aperture_height_d;
    td_u32 clean_aperture_width, clean_aperture_height;
    td_u32 hor_off_n, hor_off_d, ver_off_n, ver_off_d;
    td_s32 hor_off, ver_off;
    drv_vdec_heif_clap *clap = TD_NULL;

    (td_void)(header);

    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &clean_aperture_width_n, HEIF_FIELD_SIZE_4));
    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &clean_aperture_width_d, HEIF_FIELD_SIZE_4));
    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &clean_aperture_height_n, HEIF_FIELD_SIZE_4));
    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &clean_aperture_height_d, HEIF_FIELD_SIZE_4));
    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &hor_off_n, HEIF_FIELD_SIZE_4));
    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &hor_off_d, HEIF_FIELD_SIZE_4));
    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &ver_off_n, HEIF_FIELD_SIZE_4));
    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &ver_off_d, HEIF_FIELD_SIZE_4));

    vdec_check_ret(clean_aperture_width_d != 0);
    vdec_check_ret(clean_aperture_height_d != 0);
    vdec_check_ret(hor_off_d != 0);
    vdec_check_ret(ver_off_d != 0);

    clean_aperture_width  = clean_aperture_width_n  / clean_aperture_width_d;
    clean_aperture_height = clean_aperture_height_n / clean_aperture_height_d;
    hor_off = (td_s32)hor_off_n / (td_s32)hor_off_d;
    ver_off = (td_s32)ver_off_n / (td_s32)ver_off_d;

    for (i = 0; i < VDEC_HEIF_MAX_CLAP_NUM; i++) {
        if (heif_info->boxes.clap[i].is_used == TD_FALSE) {
            clap = &heif_info->boxes.clap[i];
            clap->clean_aperture_width  = clean_aperture_width;
            clap->clean_aperture_height = clean_aperture_height;
            clap->hor_off = hor_off;
            clap->ver_off = ver_off;
            clap->prop_index = prop_index;
            clap->is_used = TD_TRUE;
            break;
        }
    }

    vdec_heif_trace("- clean aperture: %ux%u @ offset %ux%u\n", clean_aperture_width, clean_aperture_height, hor_off, ver_off);

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_parse_pixi(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info, const drv_vdec_heif_box_header *header, td_u16 prop_index)
{
    td_u8 i, num_channels, bits_per_channel;

    (td_void)(heif_info);
    (td_void)(header);
    (td_void)(prop_index);

    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &num_channels, HEIF_FIELD_SIZE_1));
    vdec_heif_trace("- num_channels: %u\n", num_channels);

    for (i = 0; i < num_channels; i++) {
        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &bits_per_channel, HEIF_FIELD_SIZE_1));
        vdec_heif_trace("- channel %u: bits %u\n", i, bits_per_channel);
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_parse_ipco(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info, const drv_vdec_heif_box_header *header)
{
    td_s32 ret;
    td_u16 property_count;
    td_u32 start_offset, consume_size;
    drv_vdec_heif_box_header sub_header = {0};

    if (header->content_size == 0) {
        return EXT_SUCCESS;
    }

    start_offset = parser->offset;
    property_count = 1;

    do {
        ret = drv_vdec_heif_parse_box_header(parser, &sub_header);
        if (ret != EXT_SUCCESS) {
            break;
        }

        if (strncmp(sub_header.type, "hvcC", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_hvcc(parser, heif_info, &sub_header, property_count);
        } else if (strncmp(sub_header.type, "ispe", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_ispe(parser, heif_info, &sub_header, property_count);
        } else if (strncmp(sub_header.type, "irot", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_irot(parser, heif_info, &sub_header, property_count);
        } else if (strncmp(sub_header.type, "imir", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_imir(parser, heif_info, &sub_header, property_count);
        } else if (strncmp(sub_header.type, "clap", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_clap(parser, heif_info, &sub_header, property_count);
        } else if (strncmp(sub_header.type, "pixi", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_pixi(parser, heif_info, &sub_header, property_count);
        } else {
            vdec_heif_trace("unsupport box %.4s\n", sub_header.type);
            ret = drv_vdec_heif_skip(parser, sub_header.content_size);
        }

        vdec_check_ret(ret == EXT_SUCCESS);

        property_count++;
        consume_size = parser->offset - start_offset;
    } while (consume_size < header->content_size);

    return ret;
}

static td_s32 drv_vdec_heif_parse_ipma(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info, const drv_vdec_heif_box_header *header)
{
    td_u32 i, j, item_count, item_id;
    td_u8  asso_count, essential;
    td_u16 property_index;
    td_u8  version = header->version;
    drv_vdec_heif_ipma *ipma = &heif_info->boxes.ipma;
    drv_vdec_heif_pitm *pitm = &heif_info->boxes.pitm;

    vdec_check_ret(ipma->is_used == TD_FALSE);

    vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &item_count, HEIF_FIELD_SIZE_4));
    vdec_heif_trace("- item_count = %u\n", item_count);

    for (i = 0; i < item_count; i++) {
        drv_vdec_heif_asso *asso = TD_NULL;

        if (version < 1) {
            td_u16 item_id_16;
            vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &item_id_16, HEIF_FIELD_SIZE_2));
            item_id = item_id_16;
        } else {
            vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &item_id, HEIF_FIELD_SIZE_4));
        }
        vdec_heif_trace("  > id = %u, index = ", item_id);

        if (pitm->is_used == TD_TRUE && pitm->primary_item == item_id) {
            asso = (i < VDEC_HEIF_MAX_ITEM_NUM) ? &ipma->asso[i] : &ipma->asso[0];
        } else {
            asso = (i < VDEC_HEIF_MAX_ITEM_NUM) ? &ipma->asso[i] : TD_NULL;
        }

        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &asso_count, HEIF_FIELD_SIZE_1));
        if (asso_count > VDEC_HEIF_MAX_ASSO_NUM) {
            vdec_print_err("asso_count %u > max_asso_num %u unsupport.\n", asso_count, VDEC_HEIF_MAX_ASSO_NUM);
            return EXT_FAILURE;
        }

        for (j = 0; j < asso_count; j++) {
            if (header->flags & 1) {
                td_u16 data16;
                vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &data16, HEIF_FIELD_SIZE_2));
                essential = (data16 >> 15) & 0x1; // 15 bit shift
                property_index = (data16 & 0x7FFF);
            } else {
                td_u8 data8;
                vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &data8, HEIF_FIELD_SIZE_1));
                essential = (data8 >> 7) & 0x1; // 7 bit shift
                property_index = (td_u16)(data8 & 0x7F);
            }

            vdec_heif_trace("%u(%u) ", property_index, essential);

            if (asso != TD_NULL) {
                asso->essential[j] = essential;
                asso->prop_index[j] = property_index;
            }
        }
        vdec_heif_trace("\n");

        if (asso != TD_NULL) {
            asso->item_id = item_id;
            asso->asso_count = asso_count;
        }
    }

    ipma->is_used = TD_TRUE;

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_parse_mdat(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info, const drv_vdec_heif_box_header *header)
{
    td_u64 content_size = header->content_size;
    drv_vdec_heif_mdat *mdat = &heif_info->boxes.mdat;

    vdec_check_ret(mdat->is_used == TD_FALSE);

    mdat->start = parser->start;
    mdat->offset = parser->offset;
    mdat->length = content_size;
    mdat->is_used = TD_TRUE;

    vdec_heif_trace("- skip mdat content_size %llu\n", content_size);
    vdec_check_func_ret(drv_vdec_heif_skip(parser, content_size)); // nalu unit payload

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_parse_boxes(drv_vdec_heif_stream *parser, drv_vdec_heif_info *heif_info)
{
    td_s32 ret;
    td_bool error_occur = TD_FALSE;
    drv_vdec_heif_box_header header = {0};

    while (1) {
        if (drv_vdec_heif_parse_box_header(parser, &header) != EXT_SUCCESS) {
            break;
        }

        if (strncmp(header.type, "ftyp", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_ftyp(parser, heif_info, &header);
        } else if (strncmp(header.type, "hdlr", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_hdlr(parser, heif_info, &header);
        } else if (strncmp(header.type, "pitm", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_pitm(parser, heif_info, &header);
        } else if (strncmp(header.type, "iloc", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_iloc(parser, heif_info, &header);
        } else if (strncmp(header.type, "iinf", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_iinf(parser, heif_info, &header);
        } else if (strncmp(header.type, "iref", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_iref(parser, heif_info, &header);
        } else if (strncmp(header.type, "ipco", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_ipco(parser, heif_info, &header);
        } else if (strncmp(header.type, "ipma", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_ipma(parser, heif_info, &header);
        } else if (strncmp(header.type, "mdat", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_mdat(parser, heif_info, &header);
        } else if (strncmp(header.type, "meta", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_boxes(parser, heif_info);
        } else if (strncmp(header.type, "iprp", HEIF_FIELD_SIZE_4) == 0) {
            ret = drv_vdec_heif_parse_boxes(parser, heif_info);
        } else {
            vdec_heif_trace("skip box %.4s\n", header.type);
            ret = drv_vdec_heif_skip(parser, header.content_size);
        }

        if (ret != EXT_SUCCESS) {
            vdec_print_err("failed to parse box %.4s, stop parsing\n", header.type);
            error_occur = TD_TRUE;
            break;
        }

        if (parser->offset < parser->length) {
            continue;
        }

        if (parser->offset > parser->length) {
            vdec_print_err("offset %d exceed file length %d.\n", parser->offset, parser->length);
            error_occur = TD_TRUE;
        }
        break;
    }

    return (error_occur == TD_TRUE) ? EXT_FAILURE : EXT_SUCCESS;
}

STATIC td_u32 drv_vdec_heif_calc_hvcc_nalu(const drv_vdec_heif_hvcc *hvcc)
{
    td_u32 i, j;
    td_u32 length = 0;
    const drv_vdec_heif_nal_array *nal_array = TD_NULL;

    for (i = 0; i < hvcc->num_of_array; i++) {
        nal_array = &hvcc->nal_array[i];
        for (j = 0; j < nal_array->num_of_nalus; j++) {
            length += HEIF_START_CODE_LEN + nal_array->nal_unit_len[j];
        }
    }

    return length;
}

STATIC td_u32 drv_vdec_heif_calc_item_extent(td_u32 item_id, const drv_vdec_heif_info *heif_info)
{
    td_u32 i, j;
    td_u32 length = 0;
    const drv_vdec_heif_item_extent *extent = TD_NULL;
    const drv_vdec_heif_iloc *iloc = &heif_info->boxes.iloc;

    vdec_check_ret(iloc->is_used == TD_TRUE);

    for (i = 0; i < VDEC_HEIF_MAX_ITEM_NUM; i++) {
        extent = &iloc->item_extent[i];
        if (extent->item_id != item_id) {
            continue;
        }

        for (j = 0; j < extent->extent_count; j++) {
            length += extent->extent_length[j];
        }
        break;
    }

    if (i >= VDEC_HEIF_MAX_ITEM_NUM) {
        vdec_print_err("no iloc record for item_id %u.\n", item_id);
    }

    return length;
}

STATIC td_void drv_vdec_heif_query_prop(td_u32 prop_index, const drv_vdec_heif_info *heif_info, drv_vdec_heif_pic *pic)
{
    td_u32 i;
    const drv_vdec_heif_hvcc *hvcc = TD_NULL;
    const drv_vdec_heif_ispe *ispe = TD_NULL;

    for (i = 0; i < VDEC_HEIF_MAX_HVCC_NUM; i++) {
        hvcc = &heif_info->boxes.hvcc[i];
        if (hvcc->is_used == TD_TRUE && hvcc->prop_index == prop_index) {
            pic->total_stream_len += drv_vdec_heif_calc_hvcc_nalu(hvcc);
            break;
        }
    }

    for (i = 0; i < VDEC_HEIF_MAX_ISPE_NUM; i++) {
        ispe = &heif_info->boxes.ispe[i];
        if (ispe->is_used == TD_TRUE && ispe->prop_index == prop_index) {
            pic->width  = ispe->image_width;
            pic->height = ispe->image_height;
            break;
        }
    }
}

STATIC td_s32 drv_vdec_heif_query_header(td_u32 item_id, const drv_vdec_heif_info *heif_info, drv_vdec_heif_pic *pic)
{
    td_u32 i;
    td_u16 prop_index;
    const drv_vdec_heif_ipma *ipma = &heif_info->boxes.ipma;
    const drv_vdec_heif_asso *asso = TD_NULL;

    vdec_check_ret(ipma->is_used == TD_TRUE);

    for (i = 0; i < VDEC_HEIF_MAX_ITEM_NUM; i++) {
        asso = &ipma->asso[i];
        if (asso->item_id == item_id) {
            break;
        }
    }

    vdec_check_ret(asso != TD_NULL);

    for (i = 0; i < asso->asso_count; i++) {
        prop_index = asso->prop_index[i];
        drv_vdec_heif_query_prop(prop_index, heif_info, pic);
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_query_data(td_u32 item_id, const drv_vdec_heif_info *heif_info, drv_vdec_heif_pic *pic)
{
    td_u32 length;
    const drv_vdec_heif_mdat *mdat = &heif_info->boxes.mdat;

    vdec_check_ret(mdat->is_used == TD_TRUE);

    length = drv_vdec_heif_calc_item_extent(item_id, heif_info);
    if (length == 0 || length > mdat->length) {
        vdec_print_err("length %u = 0 or larger than mdat length %u invalid.\n", length, mdat->length);
        return EXT_FAILURE;
    }

    pic->total_stream_len += length;

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_query_orig_item(const drv_vdec_heif_info *heif_info, td_u32 *orig_item)
{
    const drv_vdec_heif_pitm *pitm = &heif_info->boxes.pitm;
    const drv_vdec_heif_thmb *thmb = &heif_info->boxes.thmb;

    *orig_item = 0;

    vdec_check_ret(pitm->is_used == TD_TRUE);

    if (thmb->is_used == TD_TRUE && thmb->from_item_id == pitm->primary_item) {
        *orig_item = thmb->to_item_id[0];
    } else {
        *orig_item = pitm->primary_item;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_query_original(const drv_vdec_heif_info *heif_info, drv_vdec_heif_pic *pic)
{
    td_s32 ret;
    td_u32 orig_item;

    ret = drv_vdec_heif_query_orig_item(heif_info, &orig_item);
    vdec_check_ret(ret == EXT_SUCCESS);

    ret = drv_vdec_heif_query_header(orig_item, heif_info, pic);
    vdec_check_ret(ret == EXT_SUCCESS);

    ret = drv_vdec_heif_query_data(orig_item, heif_info, pic);
    vdec_check_ret(ret == EXT_SUCCESS);

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_query_thmb_item(const drv_vdec_heif_info *heif_info, td_u32 prim_item, td_u32 *thmb_item)
{
    td_u32 i;
    const drv_vdec_heif_thmb *thmb = &heif_info->boxes.thmb;

    *thmb_item = 0;

    if (thmb->is_used == TD_FALSE) {
        vdec_heif_trace("no thumbnail found in this heif file.\n");
        return EXT_FAILURE;
    }

    if (thmb->from_item_id == prim_item) {
        *thmb_item = thmb->from_item_id;
        return EXT_SUCCESS;
    }

    for (i = 0; i < VDEC_HEIF_MAX_THMB_REF_NUM; i++) {
        if (thmb->to_item_id[i] == prim_item) {
            *thmb_item = thmb->from_item_id;
            return EXT_SUCCESS;
        }
    }

    vdec_print_err("prim_item %u has no associated thumbnail.\n", prim_item);

    return EXT_FAILURE;
}

STATIC td_s32 drv_vdec_heif_query_thumbnail(const drv_vdec_heif_info *heif_info, drv_vdec_heif_pic *pic)
{
    td_s32 ret;
    td_u32 prim_item, thmb_item;
    const drv_vdec_heif_pitm *pitm = &heif_info->boxes.pitm;

    vdec_check_ret(pitm->is_used == TD_TRUE);

    prim_item = pitm->primary_item;
    ret = drv_vdec_heif_query_thmb_item(heif_info, prim_item, &thmb_item);
    if (ret == EXT_SUCCESS) {
        ret = drv_vdec_heif_query_header(thmb_item, heif_info, pic);
        vdec_check_goto(ret == EXT_SUCCESS, err_exit);

        ret = drv_vdec_heif_query_data(thmb_item, heif_info, pic);
        vdec_check_goto(ret == EXT_SUCCESS, err_exit);
    }

    return EXT_SUCCESS;

err_exit:
    (td_void)memset_s(pic, sizeof(drv_vdec_heif_pic), 0, sizeof(drv_vdec_heif_pic));

    return EXT_FAILURE;
}

STATIC td_s32 drv_vdec_heif_tiff_read(drv_vdec_heif_stream *parser, td_void *buf, td_u64 size, td_bool is_big_end)
{
    if (is_big_end == TD_TRUE) {
        return drv_vdec_heif_read_be_cvt(parser, buf, size);
    } else {
        return drv_vdec_heif_read(parser, buf, size);
    }
}

STATIC td_s32 drv_vdec_heif_parser_byte_order(drv_vdec_heif_stream *parser, td_bool *is_big_end)
{
    td_u16 byte_order;
    td_u32 exif_header_offset;

    byte_order = *((td_u16 *)parser->start);
    if (byte_order == TIFF_BIG_END_MM) {  // 'MM'
        *is_big_end = TD_TRUE;
        vdec_heif_trace("warinig: missing heif exif header, start with big-endian('MM').\n");
        vdec_check_func_ret(drv_vdec_heif_skip(parser, HEIF_FIELD_SIZE_2));
    } else if (byte_order == TIFF_LIT_END_II) {  // 'II'
        *is_big_end = TD_FALSE;
        vdec_heif_trace("warinig: missing heif exif header, start with little-endian('II').\n");
        vdec_check_func_ret(drv_vdec_heif_skip(parser, HEIF_FIELD_SIZE_2));
    } else {
        vdec_check_func_ret(drv_vdec_heif_read_be_cvt(parser, &exif_header_offset, HEIF_FIELD_SIZE_4));
        vdec_heif_trace("exif_header_offset %u.\n", exif_header_offset);

        vdec_check_func_ret(drv_vdec_heif_skip(parser, exif_header_offset));

        vdec_check_func_ret(drv_vdec_heif_read(parser, &byte_order, HEIF_FIELD_SIZE_2));
        if (byte_order == TIFF_BIG_END_MM) {  // 'MM'
            *is_big_end = TD_TRUE;
        } else if (byte_order == TIFF_LIT_END_II) {  // 'II'
            *is_big_end = TD_FALSE;
        } else {
            vdec_print_err("unsupport byte_order %x.\n", byte_order);
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_query_exif_entry(drv_vdec_heif_stream *parser, td_bool is_big_end, drv_vdec_heif_format *format)
{
    td_u16 tag, type, data16;
    td_u32 num;

    vdec_check_func_ret(drv_vdec_heif_tiff_read(parser, &tag,  HEIF_FIELD_SIZE_2, is_big_end));
    vdec_check_func_ret(drv_vdec_heif_tiff_read(parser, &type, HEIF_FIELD_SIZE_2, is_big_end));
    vdec_check_func_ret(drv_vdec_heif_tiff_read(parser, &num,  HEIF_FIELD_SIZE_4, is_big_end));

    switch (tag) {
        case TIFF_TAG_ORIENTATION:
            vdec_check_func_ret(drv_vdec_heif_tiff_read(parser, &data16, HEIF_FIELD_SIZE_2, is_big_end));
            vdec_check_func_ret(drv_vdec_heif_skip(parser, HEIF_FIELD_SIZE_2));
            vdec_heif_trace("tag 0x%x - orientation %u\n", tag, data16);
            format->orientation = data16 - 1;  // record minus 1
            break;

        default:
            vdec_check_func_ret(drv_vdec_heif_skip(parser, HEIF_FIELD_SIZE_4));
            vdec_heif_trace("tag 0x%x skipped.\n", tag);
            break;
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_query_exif_ifd(drv_vdec_heif_stream *parser, td_bool is_big_end, drv_vdec_heif_format *format)
{
    td_s32 ret;
    td_u16 i, entry_count;
    td_u32 next_ifd_offset;

    do {
        vdec_check_func_ret(drv_vdec_heif_tiff_read(parser, &entry_count, HEIF_FIELD_SIZE_2, is_big_end));

        for (i = 0; i < entry_count; i++) {
            ret = drv_vdec_heif_query_exif_entry(parser, is_big_end, format);
            vdec_check_ret(ret == EXT_SUCCESS);
        }

        vdec_check_func_ret(drv_vdec_heif_tiff_read(parser, &next_ifd_offset, HEIF_FIELD_SIZE_4, is_big_end));
        if (next_ifd_offset == 0) {
            break;
        }
        vdec_check_func_ret(drv_vdec_heif_skip(parser, next_ifd_offset - HEIF_FIELD_SIZE_4));
    } while (1);

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_query_exif_data(const drv_vdec_heif_info *heif_info, td_u32 offset, td_u32 length, drv_vdec_heif_format *format)
{
    td_s32 ret;
    td_u16 magic_num;
    td_u32 ifd_offset;
    td_bool is_big_end;
    drv_vdec_heif_stream parser = {0};
    const drv_vdec_heif_mdat *mdat = &heif_info->boxes.mdat;

    vdec_check_ret(mdat->is_used == TD_TRUE);

    if (offset + length > mdat->offset + mdat->length) {
        vdec_print_err("exif offset %u + length %u > mdat offset %u + length %u invalid.\n", offset, length, mdat->offset, mdat->length);
        return EXT_FAILURE;
    }

    parser.start = mdat->start + offset;
    parser.length = length;
    parser.offset = 0;

    vdec_check_func_ret(drv_vdec_heif_parser_byte_order(&parser, &is_big_end));

    vdec_check_func_ret(drv_vdec_heif_tiff_read(&parser, &magic_num, HEIF_FIELD_SIZE_2, is_big_end));
    if (magic_num != TIFF_MAGIC_NUM) {
        vdec_print_err("magic_num %u != %u invalid.\n", magic_num, TIFF_MAGIC_NUM);
        return EXT_FAILURE;
    }

    vdec_check_func_ret(drv_vdec_heif_tiff_read(&parser, &ifd_offset, HEIF_FIELD_SIZE_4, is_big_end));
    vdec_check_func_ret(drv_vdec_heif_skip(&parser, ifd_offset - HEIF_FIELD_SIZE_8));

    ret = drv_vdec_heif_query_exif_ifd(&parser, is_big_end, format);
    vdec_check_ret(ret == EXT_SUCCESS);

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_query_exif_item(td_u32 item_id, const drv_vdec_heif_info *heif_info, drv_vdec_heif_format *format)
{
    td_s32 ret;
    td_u32 i, extent_offset, extent_length;
    const drv_vdec_heif_item_extent *extent = TD_NULL;
    const drv_vdec_heif_iloc *iloc = &heif_info->boxes.iloc;

    vdec_check_ret(iloc->is_used == TD_TRUE);

    for (i = 0; i < VDEC_HEIF_MAX_ITEM_NUM; i++) {
        extent = &iloc->item_extent[i];
        if (extent->item_id != item_id) {
            continue;
        }

        if (extent->extent_count > 1) {
            vdec_print_err("Exif extent_count %u > 1 invalid.\n", extent->extent_count);
            return EXT_FAILURE;
        }

        extent_offset = extent->extent_offset[0];
        extent_length = extent->extent_length[0];

        ret = drv_vdec_heif_query_exif_data(heif_info, extent_offset, extent_length, format);
        vdec_check_ret(ret == EXT_SUCCESS);
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_query_exif(const drv_vdec_heif_info *heif_info, drv_vdec_heif_format *format)
{
    td_u32 i;
    const drv_vdec_heif_infe *infe = TD_NULL;

    for (i = 0; i < VDEC_HEIF_MAX_INFE_NUM; i++) {
        infe = &heif_info->boxes.infe[i];
        if (infe->is_used == TD_FALSE || strncmp(infe->item_type, "Exif", HEIF_FIELD_SIZE_4) != 0) {
            continue;
        }

        return drv_vdec_heif_query_exif_item(infe->item_id, heif_info, format);
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_query_format(const drv_vdec_heif_info *heif_info, drv_vdec_heif_format *format)
{
    td_s32 ret;

    ret = drv_vdec_heif_query_exif(heif_info, format);
    vdec_check_ret(ret == EXT_SUCCESS);

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_query_info(drv_vdec_heif_info *heif_info)
{
    td_s32 ret;

    ret = drv_vdec_heif_query_original(heif_info, &heif_info->orig_pic);
    vdec_check_ret(ret == EXT_SUCCESS);

    (td_void)drv_vdec_heif_query_thumbnail(heif_info, &heif_info->thmb_pic);
    (td_void)drv_vdec_heif_query_format(heif_info, &heif_info->format);

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_check_info(const drv_vdec_heif_info *heif_info)
{
    const drv_vdec_heif_pic *prim_pic = &heif_info->orig_pic;

    vdec_check_ret(prim_pic->width  != 0);
    vdec_check_ret(prim_pic->height != 0);
    vdec_check_ret(prim_pic->total_stream_len > 0);

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_extract_hvcc_nalu(const drv_vdec_heif_hvcc *hvcc, drv_vdec_heif_stream *heif_strm)
{
    td_u32 i, j;
    td_u32 nalu_len;
    td_u8 *nalu_start = TD_NULL;
    td_u8 start_code[HEIF_START_CODE_LEN] = {0, 0, 0, 1};
    const drv_vdec_heif_nal_array *nal_array = TD_NULL;

    for (i = 0; i < hvcc->num_of_array; i++) {
        nal_array = &hvcc->nal_array[i];
        for (j = 0; j < nal_array->num_of_nalus; j++) {
            nalu_start = nal_array->nal_unit_start[j];
            nalu_len = nal_array->nal_unit_len[j];

            (td_void)memcpy_s(heif_strm->start + heif_strm->offset, heif_strm->length - heif_strm->offset, start_code,
                HEIF_START_CODE_LEN);
            heif_strm->offset += HEIF_START_CODE_LEN;

            (td_void)memcpy_s(heif_strm->start + heif_strm->offset, heif_strm->length - heif_strm->offset, nalu_start, nalu_len);
            heif_strm->offset += nalu_len;
        }
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_extract_pic_extent(const drv_vdec_heif_info *heif_info, td_u32 extent_offset, td_u32 extent_length, drv_vdec_heif_stream *heif_strm)
{
    td_u32 nalu_len, used_len;
    td_u8 *nalu_start = TD_NULL;
    td_u8 start_code[HEIF_START_CODE_LEN] = {0x00, 0x00, 0x00, 0x01};
    const drv_vdec_heif_mdat *mdat = &heif_info->boxes.mdat;

    vdec_check_ret(mdat->is_used == TD_TRUE);

    used_len = 0;

    do {
        nalu_start = mdat->start + extent_offset + used_len;
        nalu_len = be32_to_host(*((const td_u32*)nalu_start));
        if (nalu_len == 0) {
            break;
        }

        if (used_len + nalu_len + HEIF_FIELD_SIZE_4 > extent_length) {
            vdec_print_err("used_len %u + nalu_len %u + start_code_len %u > extent_length %u invalid.\n", used_len, nalu_len, HEIF_FIELD_SIZE_4, extent_length);
            return EXT_FAILURE;
        }

        nalu_start += HEIF_FIELD_SIZE_4;

        (td_void)memcpy_s(heif_strm->start + heif_strm->offset, heif_strm->length - heif_strm->offset, start_code, HEIF_START_CODE_LEN);
        heif_strm->offset += HEIF_START_CODE_LEN;

        (td_void)memcpy_s(heif_strm->start + heif_strm->offset, heif_strm->length - heif_strm->offset, nalu_start, nalu_len);
        heif_strm->offset += nalu_len;

        used_len += (nalu_len + HEIF_FIELD_SIZE_4);
    } while (used_len < extent_length);

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_extract_prop(td_u32 prop_index, const drv_vdec_heif_info *heif_info, drv_vdec_heif_stream *heif_strm)
{
    td_u32 i;
    const drv_vdec_heif_hvcc *hvcc = TD_NULL;

    for (i = 0; i < VDEC_HEIF_MAX_HVCC_NUM; i++) {
        hvcc = &heif_info->boxes.hvcc[i];
        if (hvcc->is_used == TD_TRUE && hvcc->prop_index == prop_index) {
            vdec_check_func_ret(drv_vdec_heif_extract_hvcc_nalu(hvcc, heif_strm));
            break;
        }
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_extract_header(td_u32 item_id, const drv_vdec_heif_info *heif_info, drv_vdec_heif_stream *heif_strm)
{
    td_u32 i;
    td_s32 ret;
    td_u16 prop_index;
    const drv_vdec_heif_ipma *ipma = &heif_info->boxes.ipma;
    const drv_vdec_heif_asso *asso = TD_NULL;

    vdec_check_ret(ipma->is_used == TD_TRUE);

    for (i = 0; i < VDEC_HEIF_MAX_ITEM_NUM; i++) {
        asso = &ipma->asso[i];
        if (asso->item_id == item_id) {
            break;
        }
    }

    vdec_check_ret(asso != TD_NULL);

    for (i = 0; i < asso->asso_count; i++) {
        if (asso->essential[i] == 0) {
            continue;
        }

        prop_index = asso->prop_index[i];

        ret = drv_vdec_heif_extract_prop(prop_index, heif_info, heif_strm);
        if (ret != EXT_SUCCESS) {
            vdec_print_err("item_id %u can't find property %u.\n", item_id, prop_index);
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_extract_data(td_u32 item_id, const drv_vdec_heif_info *heif_info, drv_vdec_heif_stream *heif_strm)
{
    td_s32 ret;
    td_u32 i, j;
    td_u32 extent_offset, extent_length;
    const drv_vdec_heif_item_extent *extent = TD_NULL;
    const drv_vdec_heif_iloc *iloc = &heif_info->boxes.iloc;

    vdec_check_ret(iloc->is_used == TD_TRUE);

    for (i = 0; i < VDEC_HEIF_MAX_ITEM_NUM; i++) {
        extent = &iloc->item_extent[i];
        if (extent->item_id != item_id) {
            continue;
        }

        for (j = 0; j < extent->extent_count; j++) {
            extent_offset = extent->extent_offset[j];
            extent_length = extent->extent_length[j];

            ret = drv_vdec_heif_extract_pic_extent(heif_info, extent_offset, extent_length, heif_strm);
            if (ret != EXT_SUCCESS) {
                vdec_print_err("item_id %u extent %u extract failed.\n", item_id, j);
                return EXT_FAILURE;
            }
        }
    }

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_extract_original(const drv_vdec_heif_info *heif_info, drv_vdec_heif_stream *heif_strm)
{
    td_s32 ret;
    td_u32 orig_item;

    ret = drv_vdec_heif_query_orig_item(heif_info, &orig_item);
    vdec_check_ret(ret == EXT_SUCCESS);

    ret = drv_vdec_heif_extract_header(orig_item, heif_info, heif_strm);
    vdec_check_ret(ret == EXT_SUCCESS);

    ret = drv_vdec_heif_extract_data(orig_item, heif_info, heif_strm);
    vdec_check_ret(ret == EXT_SUCCESS);

    vdec_heif_trace("fill original picture stream length %u\n", heif_strm->offset);

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_extract_thumbnail(const drv_vdec_heif_info *heif_info, drv_vdec_heif_stream *heif_strm)
{
    td_s32 ret;
    td_u32 prim_item, thmb_item;
    const drv_vdec_heif_pitm *pitm = &heif_info->boxes.pitm;

    vdec_check_ret(pitm->is_used == TD_TRUE);

    prim_item = pitm->primary_item;
    ret = drv_vdec_heif_query_thmb_item(heif_info, prim_item, &thmb_item);
    vdec_check_ret(ret == EXT_SUCCESS);

    ret = drv_vdec_heif_extract_header(thmb_item, heif_info, heif_strm);
    vdec_check_ret(ret == EXT_SUCCESS);

    ret = drv_vdec_heif_extract_data(thmb_item, heif_info, heif_strm);
    vdec_check_ret(ret == EXT_SUCCESS);

    vdec_heif_trace("fill thumbnail picture stream length %u\n", heif_strm->offset);

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_init_info(drv_vdec_heif_info *heif_info)
{
    vdec_check_ret(heif_info != TD_NULL);

    (td_void)memset_s(heif_info, sizeof(drv_vdec_heif_info), 0, sizeof(drv_vdec_heif_info));

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vdec_heif_check_stream(const drv_vdec_heif_stream *stream)
{
    vdec_check_ret(stream != TD_NULL);
    vdec_check_ret(stream->start != TD_NULL);
    vdec_check_ret(stream->length > 0);
    vdec_check_ret(stream->offset == 0);

    return EXT_SUCCESS;
}

td_s32 drv_vdec_heif_parse_info(drv_vdec_heif_stream *stream, drv_vdec_heif_info *heif_info)
{
    td_s32 ret;

    ret = drv_vdec_heif_check_stream(stream);
    vdec_check_ret(ret == EXT_SUCCESS);

    ret = drv_vdec_heif_init_info(heif_info);
    vdec_check_ret(ret == EXT_SUCCESS);

    ret = drv_vdec_heif_parse_boxes(stream, heif_info);
    vdec_check_ret(ret == EXT_SUCCESS);

    ret = drv_vdec_heif_query_info(heif_info);
    vdec_check_ret(ret == EXT_SUCCESS);

    ret = drv_vdec_heif_check_info(heif_info);
    vdec_check_ret(ret == EXT_SUCCESS);

    return EXT_SUCCESS;
}

td_s32 drv_vdec_heif_fill_stream(ext_vdec_heif_select select, const drv_vdec_heif_info *heif_info, drv_vdec_heif_stream *stream)
{
    td_s32 ret;

    vdec_check_ret(heif_info != TD_NULL);

    ret = drv_vdec_heif_check_stream(stream);
    vdec_check_ret(ret == EXT_SUCCESS);

    if (select == EXT_VDEC_HEIF_THUMBNAIL) {
        ret = drv_vdec_heif_extract_thumbnail(heif_info, stream);
        vdec_check_ret(ret == EXT_SUCCESS);
    } else { // default original picture
        ret = drv_vdec_heif_extract_original(heif_info, stream);
        vdec_check_ret(ret == EXT_SUCCESS);
    }

    return EXT_SUCCESS;
}


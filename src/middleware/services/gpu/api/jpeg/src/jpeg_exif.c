/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: jpeg exif
 */

#include "jpeg_exif.h"
#include "securec.h"
#include "jpeg_stream.h"
#include "jpeg_mem.h"

/*-------------------------------------- macro define -------------------------------------*/
#define JPEG_EXIF_2BYTES               2
#define JPEG_EXIF_4BYTES               4
#define JPEG_EXIF_TAG_SIZE             6
#define JPEG_EXIF_COMPRESS_FLAG        6
#define JPEG_EXIF_TIME_STR_LENGTH      8
#define JPEG_EXIF_DATE_STR_LENGTH      10
#define JPEG_EXIF_ENTRY_LENGTH         12
#define JPEG_EXIF_MIN_SIZE             18
#define JPEG_EXIF_ENTRY_DATA_SIZE      20
#define JPEG_EXIF_TIME_LENGTH          20

#define JPEG_EXIF_ENDIAN_LITTLE        0x4949
#define JPEG_EXIF_ENDIAN_BIG           0x4D4D
#define JPEG_EXIF_ORIENTATION          0x0112
#define JPEG_EXIF_DATE_TIME            0x0132
#define JPEG_EXIF_SUB_IFD              0x8769
#define JPEG_EXIF_GPS_IFD              0x8825
#define JPEG_EXIF_IMAGE_COMPRESS       0x0103
#define JPEG_EXIF_THUMBNAIL_OFFSET     0x0201
#define JPEG_EXIF_THUMBNAIL_SIZE       0x0202

/*-------------------------------------- struct info --------------------------------------*/
typedef struct {
    td_u8 *exif_buf;
    td_u32 exif_size; /* exif data size */
    td_bool is_big_endian;
    my_jpeg_timeval time;
    my_jpeg_thumbnail thumbnail;
    jpeg_orientation orientation;
} jpeg_exif_entry;

#pragma pack(1)
typedef struct {
    td_u16 tag_id;
    td_u16 data_type;
    td_u32 count;
    td_u32 value_or_offset;
} jpeg_exif_directory_entry;
#pragma pack()

typedef enum {
    JPEG_IFD_TYPE_0 = 0,
    JPEG_IFD_TYPE_1,
    JPEG_IFD_TYPE_SUB,
    JPEG_IFD_TYPE_GPS,
    JPEG_IFD_TYPE_MAX,
} jpeg_exif_ifd_type;

typedef struct {
    td_u32 offset;
    jpeg_exif_ifd_type type;
} jpeg_exif_ifd_task;

/*-------------------------------------- func define --------------------------------------*/
STATIC td_u16 jpeg_exif_swap16(td_u16 val)
{
    return (val << 8) | (val >> 8); /* 8 num */
}

STATIC td_u32 jpeg_exif_swap32(td_u32 val)
{
    return ((val << 24)) | ((val << 8) & 0x00FF0000) | ((val >> 8) & 0x0000FF00) | ((val >> 24)); /* 8 24 num */
}

STATIC td_u16 jpeg_exif_read16(td_u16 val, td_s32 is_be)
{
    return is_be ? jpeg_exif_swap16(val) : val;
}

STATIC td_u32 jpeg_exif_read32(td_u32 val, td_s32 is_be)
{
    return is_be ? jpeg_exif_swap32(val) : val;
}

STATIC td_s32 jpeg_exif_data_size(td_u32 cnt, td_u16 type, td_u32 *data_size)
{
    td_u8 type_to_bytes[] = {0, 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8};

    if (type == 0 || type >= sizeof(type_to_bytes) / sizeof(td_u8)) {
        return EXT_FAILURE;
    }
    *data_size = cnt * type_to_bytes[type];
    return EXT_SUCCESS;
}

STATIC td_void jpeg_exif_ifd0_process_tag(jpeg_exif_entry *entry,
    const jpeg_exif_directory_entry *dir_entries, td_s32 i, jpeg_exif_ifd_task *task_queue, td_s32 *tail)
{
    td_s32 ret;
    td_u16 tag, type;
    const td_u8 *data_ptr;
    td_u16 orientation_val;
    td_u32 cnt, val, data_size;
    jpeg_exif_ifd_task ifd_task;

    tag = jpeg_exif_read16(dir_entries[i].tag_id, entry->is_big_endian);
    type = jpeg_exif_read16(dir_entries[i].data_type, entry->is_big_endian);
    cnt = jpeg_exif_read32(dir_entries[i].count, entry->is_big_endian);
    val = jpeg_exif_read32(dir_entries[i].value_or_offset, entry->is_big_endian);

    ret = jpeg_exif_data_size(cnt, type, &data_size);
    if (ret != EXT_SUCCESS || (data_size > JPEG_EXIF_4BYTES && val >= entry->exif_size)) {
        gpu_err("%s, %d, jpeg_exif_data_size error\r\n", __func__, __LINE__);
        return;
    }
    data_ptr = (data_size > JPEG_EXIF_4BYTES) ? (entry->exif_buf + val) :
        (const td_u8 *)&(dir_entries[i].value_or_offset);
    switch (tag) {
        case JPEG_EXIF_ORIENTATION:
            orientation_val =  entry->is_big_endian ? ((data_ptr[0] << 8) | data_ptr[1]) : /* 8 num */
                ((data_ptr[1] << 8) | data_ptr[0]); /* 8 num */
            if (orientation_val < JPEG_ORI_MAX && orientation_val >= JPEG_ORI_NORMAL) {
                entry->orientation = (jpeg_orientation)orientation_val;
            }
            break;
        case JPEG_EXIF_DATE_TIME:
            if (sscanf_s((td_char *)data_ptr, "%hu:%hhu:%hhu %hhu:%hhu:%hhu", &entry->time.year, &entry->time.month,
                &entry->time.day, &entry->time.hour, &entry->time.minute, &entry->time.second) <= 0) {
                gpu_err("%s, %d, sscanf_s error\r\n", __func__, __LINE__);
            }
            break;
        case JPEG_EXIF_SUB_IFD:
            if (*tail < JPEG_IFD_TYPE_MAX - 1) {
                ifd_task.type = JPEG_IFD_TYPE_SUB;
                ifd_task.offset = val;
                task_queue[(*tail)++] = ifd_task;
            }
            break;
        case JPEG_EXIF_GPS_IFD:
            if (*tail < JPEG_IFD_TYPE_MAX - 1) {
                ifd_task.type = JPEG_IFD_TYPE_GPS;
                ifd_task.offset = val;
                task_queue[(*tail)++] = ifd_task;
            }
            break;
        default:
            break;
    }
}

STATIC td_void jpeg_exif_process_ifd0(jpeg_exif_entry *entry, jpeg_exif_ifd_task *current_task,
    jpeg_exif_ifd_task *task_queue, td_s32 *tail)
{
    td_s32 i;
    td_u16 entry_count;
    td_u32 next_ifd_offset;
    jpeg_exif_ifd_task ifd_task;
    const td_u8 *ifd_ptr = TD_NULL;
    const jpeg_exif_directory_entry *dir_entries = TD_NULL;

    ifd_ptr = entry->exif_buf + current_task->offset;
    entry_count = jpeg_exif_read16(*(const td_u16 *)ifd_ptr, entry->is_big_endian);
    dir_entries = (const jpeg_exif_directory_entry *)(ifd_ptr + JPEG_EXIF_2BYTES);
    next_ifd_offset = jpeg_exif_read32(*(const td_u32 *)((td_u8 *)dir_entries + entry_count * JPEG_EXIF_ENTRY_LENGTH),
        entry->is_big_endian);
    if ((next_ifd_offset != 0) && (*tail < JPEG_IFD_TYPE_MAX - 1)) {
        ifd_task.type = JPEG_IFD_TYPE_1;
        ifd_task.offset = next_ifd_offset;
        task_queue[(*tail)++] = ifd_task;
    }

    for (i = 0; i < entry_count; i++) {
        jpeg_exif_ifd0_process_tag(entry, dir_entries, i, task_queue, tail);
    }
}

STATIC td_void jpeg_exif_process_ifd1(jpeg_exif_entry *entry, jpeg_exif_ifd_task *current_task)
{
    td_s32 ret, i;
    td_u16 tag, type;
    td_u32 cnt, val;
    td_u32 data_size;
    td_u16 entry_count;
    td_u16 compress_val;
    const td_u8 *data_ptr;
    td_bool is_compress = 0;
    const td_u8 *ifd_ptr = TD_NULL;
    const jpeg_exif_directory_entry *dir_entries = TD_NULL;

    ifd_ptr = entry->exif_buf + current_task->offset;
    entry_count = jpeg_exif_read16(*(const td_u16 *)ifd_ptr, entry->is_big_endian);
    dir_entries = (const jpeg_exif_directory_entry *)(ifd_ptr + JPEG_EXIF_2BYTES);
    for (i = 0; i < entry_count; i++) {
        tag = jpeg_exif_read16(dir_entries[i].tag_id, entry->is_big_endian);
        type = jpeg_exif_read16(dir_entries[i].data_type, entry->is_big_endian);
        cnt = jpeg_exif_read32(dir_entries[i].count, entry->is_big_endian);
        val = jpeg_exif_read32(dir_entries[i].value_or_offset, entry->is_big_endian);

        ret = jpeg_exif_data_size(cnt, type, &data_size);
        if (ret != EXT_SUCCESS || (data_size > JPEG_EXIF_4BYTES && val >= entry->exif_size)) {
            continue;
        }
        data_ptr = (data_size > JPEG_EXIF_4BYTES) ? (entry->exif_buf + val) :
            (const td_u8 *)&(dir_entries[i].value_or_offset);

        switch (tag) {
            case JPEG_EXIF_IMAGE_COMPRESS:
                compress_val =  entry->is_big_endian ? ((data_ptr[0] << 8) | data_ptr[1]) : /* 8 num */
                    ((data_ptr[1] << 8) | data_ptr[0]); /* 8 num */
                is_compress = (compress_val == JPEG_EXIF_COMPRESS_FLAG) ? TD_TRUE : TD_FALSE;
                entry->thumbnail.is_compress = is_compress;
                break;
            case JPEG_EXIF_THUMBNAIL_OFFSET:
                entry->thumbnail.thumb_offset = val;
                break;
            case JPEG_EXIF_THUMBNAIL_SIZE:
                entry->thumbnail.thumb_length = val;
                break;
            default:
                break;
        }
    }
}

STATIC td_void jpeg_exif_task_process(jpeg_exif_entry *entry, jpeg_exif_ifd_task *current_task,
    jpeg_exif_ifd_task *task_queue, td_s32 *tail)
{
    if (current_task->type == JPEG_IFD_TYPE_0) {
        jpeg_exif_process_ifd0(entry, current_task, task_queue, tail);
    } else if (current_task->type == JPEG_IFD_TYPE_1) {
        jpeg_exif_process_ifd1(entry, current_task);
    }
}

STATIC td_void jpeg_parse_exif_process(jpeg_exif_entry *entry)
{
    td_s32 head = 0;
    td_s32 tail = 0;
    td_u32 first_ifd_offset;
    td_bool is_big_endian = TD_FALSE;
    jpeg_exif_ifd_task ifd_task;
    jpeg_exif_ifd_task current_task;
    jpeg_exif_ifd_task task_queue[JPEG_IFD_TYPE_MAX];

    is_big_endian = (entry->exif_buf[0] == 0x4D) && (entry->exif_buf[1] == 0x4D);
    entry->is_big_endian = is_big_endian;

    /* In the standard JPEG Exif structure, the first IFD must be IFD0. */
    first_ifd_offset = jpeg_exif_read32(*(const td_u32 *)(entry->exif_buf + 4), entry->is_big_endian);
    if (tail < JPEG_IFD_TYPE_MAX - 1) {
        ifd_task.offset = first_ifd_offset;
        ifd_task.type = JPEG_IFD_TYPE_0;
        task_queue[tail++] = ifd_task;
    }

    while (head < tail) {
        current_task = task_queue[head++];
        if (current_task.offset == 0 || current_task.offset >= entry->exif_size) {
            continue;
        }
        jpeg_exif_task_process(entry, &current_task, task_queue, &tail);
    }
}

STATIC td_bool jpeg_exif_exist(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry)
{
    td_u8 i;
    td_s32 code;
    td_bool exist_exif = TD_TRUE;
    const td_u8 exif_tag[JPEG_EXIF_TAG_SIZE] = {0x45, 0x78, 0x69, 0x66, 0x0, 0x0};

    code = jpeg_stream_read_two_byte(par);
    entry->exif_size = (td_u32)((code > JPEG_EXIF_2BYTES) ? (code - JPEG_EXIF_2BYTES) : (0));
    if (entry->exif_size < JPEG_EXIF_MIN_SIZE) {
        jpeg_stream_skip_bytes(par, entry->exif_size);
        return TD_FALSE;
    }

    for (i = 0; i < JPEG_EXIF_TAG_SIZE; i++) {
        code = jpeg_stream_read_one_byte(par);
        if ((code < 0) || (code != exif_tag[i])) {
            exist_exif = TD_FALSE;
            break;
        }
    }

    entry->exif_size = (entry->exif_size > JPEG_EXIF_TAG_SIZE) ? (entry->exif_size - JPEG_EXIF_TAG_SIZE) : (0);
    if (exist_exif != TD_TRUE) {
        jpeg_stream_skip_bytes(par, entry->exif_size);
    }
    return exist_exif;
}

STATIC td_bool jpeg_exif_get_data(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry)
{
    td_u32 i;
    td_s32 ret, code;
    jpeg_mem_mgr mem = {0};

    if (entry->exif_size == 0 || par->src.is_mem == TD_FALSE) {
        gpu_err("%s, %d, jpeg_exif_get_data err (exif_size = %d, is_mem = %d)\r\n", __func__, __LINE__,
            entry->exif_size, par->src.is_mem);
        return TD_FALSE;
    }

    mem.size = entry->exif_size;
    mem.align = JPEG_MEM_STREAM_ALIGN;
    ret = jpeg_mem_alloc_large(&mem);
    if (ret != EXT_SUCCESS) {
        jpeg_stream_skip_bytes(par, entry->exif_size);
        return TD_FALSE;
    }

    entry->exif_buf = (td_u8 *)mem.vir_buf;
    for (i = 0; i < entry->exif_size; i++) {
        code = jpeg_stream_read_one_byte(par);
        if (code < 0) {
            jpeg_mem_free_large(&mem);
            entry->exif_buf = TD_NULL;
            jpeg_stream_skip_bytes(par, entry->exif_size);
            return TD_FALSE;
        }
        *(entry->exif_buf + i) = (td_u8)code;
    }

    par->exif.base_vir_buf = mem.vir_buf;
    par->exif.base_phy_buf = mem.phy_buf;

    return TD_TRUE;
}

static td_void jpeg_exif_get_thumb_info(jpeg_exif_entry *entry, jpeg_decompress_par_ptr par)
{
    if (entry->thumbnail.thumb_length != 0 && entry->thumbnail.thumb_offset != 0 &&
        entry->thumbnail.is_compress == TD_TRUE) {
        par->exif.buffer  = (td_char*)(par->exif.base_vir_buf + entry->thumbnail.thumb_offset);
        par->exif.phy_buf = (td_u32)(par->exif.base_phy_buf + entry->thumbnail.thumb_offset);
        par->exif.size    = entry->thumbnail.thumb_length;
    }
}

static td_void jpeg_exif_get_time_info(jpeg_exif_entry *entry, jpeg_decompress_par_ptr par)
{
    par->time.year = entry->time.year;
    par->time.month = entry->time.month;
    par->time.day = entry->time.day;
    par->time.hour = entry->time.hour;
    par->time.minute = entry->time.minute;
    par->time.second = entry->time.second;
}

static td_void jpeg_exif_get_info(jpeg_exif_entry *entry, jpeg_decompress_par_ptr par)
{
    if (par->thumbnail_en == TD_TRUE) {
        jpeg_exif_get_thumb_info(entry, par);
    }

    jpeg_exif_get_time_info(entry, par);

    par->orientation = entry->orientation;
}

/*-------------------------------------- func release -------------------------------------*/
td_void jpeg_exif_init(jpeg_decompress_par_ptr par)
{
    par->exif.buffer = TD_NULL;
    par->exif.phy_buf = 0;
    par->exif.size = 0;
    par->exif.base_vir_buf = TD_NULL;
    par->exif.base_phy_buf = 0;
    return;
}

td_void jpeg_exif_deinit(jpeg_decompress_par_ptr par)
{
    jpeg_mem_mgr mem = {0};

    mem.vir_buf = par->exif.base_vir_buf;
    mem.phy_buf = par->exif.base_phy_buf;
    jpeg_mem_free_large(&mem);

    par->exif.buffer = TD_NULL;
    par->exif.phy_buf = 0;
    par->exif.size = 0;
    par->exif.base_vir_buf = TD_NULL;
    par->exif.base_phy_buf = 0;
    return;
}

td_void jpeg_exif_parse(jpeg_decompress_par_ptr par)
{
    td_bool ret = TD_FALSE;
    jpeg_exif_entry entry = {0};

    ret = jpeg_exif_exist(par, &entry);
    if (ret != TD_TRUE) {
        gpu_err("%s, %d jpeg_exif_exist err\r\n", __func__, __LINE__);
        return;
    }

    ret = jpeg_exif_get_data(par, &entry);
    if (ret != TD_TRUE) {
        gpu_err("%s, %d jpeg_exif_get_data err\r\n", __func__, __LINE__);
        return;
    }

    jpeg_parse_exif_process(&entry);

    jpeg_exif_get_info(&entry, par);

    return;
}

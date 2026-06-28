/******************************************************************************
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * File          tlv.c
 * Description:  MBB tlv struct handler.
 ******************************************************************************/
#ifndef MBB_TLV_H
#define MBB_TLV_H

#include <stdint.h>
#include <soc_osal.h>
#include "osal_addr.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define CHILD_MASK  0x80
#define BIGLEN_MASK 0x80

#define TYPESIZE  1
#define BUFSIZE   100
#define UINT16LEN 2
#define UINT32LEN 4

typedef struct tag_tlv_item {
    uint8_t type;
    uint16_t length;
    uint8_t *value;
    struct tag_tlv_item *brother_item;
    struct tag_tlv_item *child_item;
    struct tag_tlv_item *parent_item;
} tlv_item_t;

static inline void *port_malloc(uint32_t size)
{
    return osal_vmalloc(size);
}

static inline void port_free(void *mem)
{
    osal_vfree(mem);
}

// To add Tlv data to a specific memory area
typedef struct {
    uint8_t* pos; // memory address
    uint32_t size; // memory size
    uint32_t offset; // memory offset, should be set 0 in the FIRST CALL in tlv mem function.
} tlv_mem_buff;

/**
 * tlv_mem_buff_init - Initialize tlv_mem_buff object.
 * @param mem_buff - Uninitialized tlv_item_t object address
 * @param addr - memory address which is used as a buffer
 * @param size  - memory size
 */
void tlv_mem_buff_init(tlv_mem_buff* mem_buff, uint8_t* addr, uint32_t size);

/**
 * tlv_mem_buff_reset - Clear all data in the buffer.
 * @param mem_buff - Used tlv_mem_buff object.
 */
void tlv_mem_buff_reset(tlv_mem_buff* mem_buff);

/**
 * tlv_mem_buff_get_written_length - Get the length of written data.
 * @param mem_buff - Used tlv_mem_buff object.
 */
uint32_t tlv_mem_buff_get_written_length(tlv_mem_buff* mem_buff);

/**
 * TlvItem_AddItemToMemRegion - Add Tlv item to a specific memory area.
 * @param item - tlv_item_t object with type, length and value address
 * @param mem_buff - tlv_mem_buff object which contains a specific memory area
 * @return - EOK: success; others: fail
 */
uint8_t tlv_get_item_with_type_from_buff(uint8_t type, tlv_item_t *item, uint8_t *buff, uint32_t size);

uint8_t tlv_add_item_to_mem_region(tlv_item_t* item, tlv_mem_buff* mem_buff);

uint32_t tlv_item_get_len_size(uint32_t len);
void tlv_trans_item_to_buf(tlv_item_t *item, uint8_t *buf, uint32_t length);

tlv_item_t *tlv_malloc_item_tree(uint32_t len, uint8_t *buf);
tlv_item_t *tlv_malloc_item_tree_with_long_msg(uint32_t len, uint8_t *buf);

void tlv_free_item_tree(tlv_item_t *tree);

tlv_item_t *tlv_new_item(uint8_t type, uint32_t len, uint8_t *value);

tlv_item_t *tlv_new_item_with_int32_value(uint8_t type, uint32_t value);

tlv_item_t *tlv_new_item_with_int16_value(uint8_t type, uint16_t value);

void tlv_free_item(tlv_item_t *item);

void tlv_item_add_child(tlv_item_t *parent, tlv_item_t *child);

void tlv_item_add_brother(tlv_item_t *brother, tlv_item_t *me);

tlv_item_t *tlv_get_item_with_type(tlv_item_t *root, uint8_t type);

uint8_t *tlv_get_item_value_with_type(tlv_item_t *root, uint8_t type, uint32_t *len);

tlv_item_t *tlv_get_item_brother(tlv_item_t *root);

tlv_item_t *tlv_get_item_child(tlv_item_t *root);

uint8_t tlv_get_item_type(tlv_item_t *item);

uint8_t *tlv_get_item_value(tlv_item_t *item, uint32_t *length);

uint8_t *tlv_malloc_str_from_item(tlv_item_t *item, uint32_t *length);

uint8_t tlv_get_item_int16_value(tlv_item_t *item, uint16_t *out_param);

uint8_t tlv_get_item_int32_value(tlv_item_t *item, uint32_t *out_param);
uint32_t tlv_get_item_mem_size(tlv_item_t *item);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

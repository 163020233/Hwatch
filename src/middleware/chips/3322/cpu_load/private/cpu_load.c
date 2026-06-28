/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:   CPU load
 * Author:
 * Create: 2021-04-06
 */
#include "cpu_load.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "debug_print.h"
#include "chip_io.h"
#include "tcxo.h"
#include "memory_config_common.h"
#ifdef BUILD_APPLICATION_STANDARD
#include "diag_log.h"
#endif
#ifdef LOAD_IMAGE_FROM_FS
#ifndef IMAGE_BT_PATH
#define IMAGE_BT_PATH        "/boot/bt/bt_signed.bin"
#endif
#define MMC_MAX_COPY_SIZE     0x2000
#define INFO_READ_LEN        sizeof(section_head_t)
#endif

#define IMAGE_COPY_LEN       4
#define ADDR_TRANS           4
#define STACK_DEFAULT_VALUE  0xdeadbeef
#define BSS_DEFAULT_VALUE    0x0
#define RESEVED_VALUE        0xffffffff

#ifdef MASTER_LOAD_SLAVE
//lint -esym(752,*)
extern uint32_t g_system_stack_begin;
extern uint32_t g_system_stack_end;
extern uint32_t __ramtext_begin__;
extern uint32_t __ramtext_end__;
extern uint32_t __ramtext_load__;
extern uint32_t __data_begin__;
extern uint32_t __data_end__;
extern uint32_t __data_load__;
extern uint32_t __bss_begin__;
extern uint32_t __bss_end__;
extern uint32_t __rom_data_begin__;
extern uint32_t __rom_data_end__;
extern uint32_t __rom_data_load__;
extern uint32_t __rom_bss_begin__;
extern uint32_t __rom_bss_end__;
extern uint32_t __text_begin__;
extern uint32_t __text_end__;
extern uint32_t __text_load__;

#ifndef CONFIG_BT_SUPPORT_REAL_ROM
extern uint32_t __rom_text_begin__;
extern uint32_t __rom_text_end__;
extern uint32_t __rom_text_load__;
#endif

extern uint32_t __patch_data_begin__;
extern uint32_t __patch_data_end__;
extern uint32_t __patch_data_load__;

extern uint32_t __rom_ram_begin__;
extern uint32_t __rom_ram_end__;
extern uint32_t __rom_ram_load__;

extern uint32_t __l2mtext_begin__;
extern uint32_t __l2mtext_end__;
extern uint32_t __l2mtext_load__;

#if CORE == BT

#ifndef CONFIG_BT_SUPPORT_REAL_ROM
#define SECTION_NUMBER  11
#else
#define SECTION_NUMBER  10
#endif

static const section_head_t g_section_information __attribute__((section(".build_info"))) = {
    SECTION_NUMBER,
    0,
    {
        {
            SECTION_TYPE_INIT,
            &g_system_stack_begin,
            &g_system_stack_end,
            NULL,
            RESEVED_VALUE
        },
        {
            SECTION_TYPE_RELOAD,
            &__rom_data_begin__,
            &__rom_data_end__,
            (const uint32_t *)((uint32_t)(&__rom_data_load__) - BT_BUILD_INFO_ORIGIN),
            RESEVED_VALUE
        },
        {
            SECTION_TYPE_CLEAR,
            &__rom_bss_begin__,
            &__rom_bss_end__,
            NULL,
            RESEVED_VALUE
        },
#ifndef CONFIG_BT_SUPPORT_REAL_ROM
        {
            SECTION_TYPE_RELOAD,
            &__rom_text_begin__,
            &__rom_text_end__,
            (const uint32_t *)(BT_ROM_FLASH_REGION_START - BT_BUILD_INFO_ORIGIN),
            RESEVED_VALUE
        },
#endif
        {
            SECTION_TYPE_RELOAD,
            &__patch_data_begin__,
            &__patch_data_end__,
            (const uint32_t *)((uint32_t)(&__patch_data_load__) - BT_BUILD_INFO_ORIGIN),
            RESEVED_VALUE
        },
        {
            SECTION_TYPE_RELOAD,
            &__rom_ram_begin__,
            &__rom_ram_end__,
            (const uint32_t *)((uint32_t)(&__rom_ram_load__) - BT_BUILD_INFO_ORIGIN),
            RESEVED_VALUE
        },
        {
            SECTION_TYPE_RELOAD,
            NULL,
            (const uint32_t *)BT_VECTORS_LENGTH,
            (const uint32_t *)(BT_VECTORS_LOAD_ORIGIN - BT_BUILD_INFO_ORIGIN),
            RESEVED_VALUE
        },
        {
            SECTION_TYPE_RELOAD,
            &__l2mtext_begin__,
            &__l2mtext_end__,
            (const uint32_t *)((uint32_t)(&__l2mtext_load__) - BT_BUILD_INFO_ORIGIN),
            RESEVED_VALUE
        },
        {
            SECTION_TYPE_RELOAD,
            &__ramtext_begin__,
            &__ramtext_end__,
            (const uint32_t *)((uint32_t)(&__ramtext_load__) - BT_BUILD_INFO_ORIGIN),
            RESEVED_VALUE
        },
        {
            SECTION_TYPE_RELOAD,
            &__data_begin__,
            &__data_end__,
            (const uint32_t *)((uint32_t)(&__data_load__) - BT_BUILD_INFO_ORIGIN),
            RESEVED_VALUE
        },
        {
            SECTION_TYPE_CLEAR,
            &__bss_begin__,
            &__bss_end__,
            NULL,
            RESEVED_VALUE
        }
    }
};
#else
static const section_head_t g_section_information __attribute__((section(".build_info"))) = {
    0,
    0,
    {{0}}
};
#endif
#endif

void cpu_load_image_process(uint32_t type, uint32_t *src, uint32_t *des, uint32_t size, uint32_t head_addr)
{
    if (((type == SECTION_TYPE_RELOAD) && (src == NULL)) || (des == NULL) || (size & 0x3) || (size == 0)) {
#ifdef BUILD_APPLICATION_STANDARD
        PRINT("[LOW_POWER]img download size:0x%x is not 4 byte aligned, type:%d\r\n", size, type);
#endif
        return;
    }
    switch (type) {
        case SECTION_TYPE_INIT:
            for (uint32_t offset = 0; offset < size; offset += IMAGE_COPY_LEN) {
                writel((uintptr_t)des + offset, STACK_DEFAULT_VALUE);
            }
            break;
        case SECTION_TYPE_CLEAR:
            for (uint32_t offset = 0; offset < size; offset += IMAGE_COPY_LEN) {
                writel((uintptr_t)des + offset, BSS_DEFAULT_VALUE);
            }
            break;
        case SECTION_TYPE_RELOAD:
            for (uint32_t offset = 0; offset < size; offset += IMAGE_COPY_LEN) {
                writel((uintptr_t)des + offset, readl((uintptr_t)src + offset + head_addr));
            }
            break;
        default:
            break;
    }
}

const section_head_t *cpu_load_get_section_info(core_images_e image)
{
    UNUSED(image);
#if defined(MASTER_LOAD_SLAVE)
    return (section_head_t *)&g_section_information;
#else
    return NULL;
#endif
}

static uint32_t cpu_load_caculate_addr(uint32_t section_run_begin, core_images_e image)
{
    if (image == CORE_IMAGES_BT) {
        if (section_run_begin == (uint32_t)BT_L2M_MEM_ORIGIN) {
            return section_run_begin + BT_L2M_MEM_ORIGIN_APP_MAPPING;
        }
        return (section_run_begin + BT_RAM_ORIGIN_APP_MAPPING);
    }
    return section_run_begin;
}

uint32_t* cpu_load_translate_into_mcu_xip_addr(uint32_t *addr, core_images_e image)
{
    /* loadaddr must be non-zero for translate */
    if ((image == CORE_IMAGES_BT) && (addr != NULL)) {
        return (uint32_t*)((uint32_t)addr);
    } else {
#ifdef BUILD_APPLICATION_STANDARD
        PRINT("[%d]core flash address[0x%x] translate int mcu flash.\r\n", image, addr);
#endif
        return NULL;
    }
}

void cpu_load_image(const section_head_t *load_info, core_images_e image)
{
    section_head_t *head = NULL;
    uint32_t type, section_size;
    uint32_t *run_addr_begin = NULL;
    uint32_t *run_addr_end = NULL;
    uint32_t *load_addr = NULL;

    head = (section_head_t *)load_info;
    if ((head == NULL) || (head->section_num == 0xFFFFFFFF)) {
        PRINT("img head is null\n");
        return;
    }
    PRINT("sections num = %x\r\n", head->section_num);
#ifdef SUPPORT_LOAD_TIME_COUNT
    uint32_t start_time, end_time;
    start_time = (uint32_t)uapi_tcxo_get_us();
#endif
    for (uint32_t i = 0; i < head->section_num; i++) {
        type = head->section_info[i].section_type;
        if (type >= SECTION_TYPE_MAX) {
            continue;
        }
        run_addr_begin = (uint32_t *)((char *)head->section_info[i].section_run_begin);
        run_addr_end = (uint32_t *)((char *)head->section_info[i].section_run_end);
        load_addr = cpu_load_translate_into_mcu_xip_addr((uint32_t*)head->section_info[i].section_load_addr, image);
        section_size = (uint32_t)((run_addr_end - run_addr_begin) * sizeof(uint32_t));
        run_addr_begin = (uint32_t *)cpu_load_caculate_addr((uint32_t)run_addr_begin, image);
#ifndef SUPPORT_LOAD_TIME_COUNT
        PRINT("copy sections num = %x, type = %x, load_addr = %x, run_addr_gegin = %x, section_size = %x\r\n",
              i, type, load_addr, run_addr_begin, section_size);
#endif
        cpu_load_image_process(type, load_addr, run_addr_begin, section_size, (uint32_t)load_info);
    }

#ifdef SUPPORT_LOAD_TIME_COUNT
    end_time = (uint32_t)uapi_tcxo_get_us();
    PRINT("------Load CORE[%d], take %lld us-----------\r\n", image, end_time - start_time);
#endif
}

#ifdef LOAD_IMAGE_FROM_FS
static void copy_buff_from_fs(FILE *file_handle, uint32_t *des, uint32_t size)
{
    char *buff = (char *)malloc(MMC_MAX_COPY_SIZE);
    if (buff == NULL) {
        PRINT("malloc error.\r\n");
        return;
    }
    uint32_t read_len = fread((void *)buff, sizeof(uint8_t), size, file_handle);
    if (read_len != size) {
        PRINT("copy_buff_from_fs error. read_len:%d.\r\n", read_len);
        free(buff);
        return;
    }
    for (uint32_t offset = 0; offset * IMAGE_COPY_LEN < size; offset++) {
        writel((int *)des + offset, ((int *)buff)[offset]);
    }
    free(buff);
}

static void cpu_load_image_process_fs(FILE *file_handle, uint32_t type, uint32_t offset, uint32_t *des, uint32_t size)
{
    if ((des == NULL) || (size & 0x3) || (size == 0)) {
        PRINT("img download size:0x%x is not 4 byte aligned, type:%d\r\n", size, type);
        return;
    }
    uint32_t seek_offset, left_size, copy_size = 0;
    switch (type) {
        case SECTION_TYPE_INIT:
            for (uint32_t offsets = 0; offsets < size; offsets += IMAGE_COPY_LEN) {
                writel((uintptr_t)des + offsets, STACK_DEFAULT_VALUE);
            }
            break;
        case SECTION_TYPE_CLEAR:
            for (uint32_t offsets = 0; offsets < size; offsets += IMAGE_COPY_LEN) {
                writel((uintptr_t)des + offsets, BSS_DEFAULT_VALUE);
            }
            break;
        case SECTION_TYPE_RELOAD:
            seek_offset = (uint32_t)fseek(file_handle, offset, SEEK_SET);
            if (seek_offset != 0) {
                PRINT("lseek error. return value is %d.\r\n", seek_offset);
                return;
            }
            if (size <= MMC_MAX_COPY_SIZE) {
                copy_buff_from_fs(file_handle, des, size);
                return;
            }
            left_size = size;
            copy_size = MMC_MAX_COPY_SIZE;
            while (left_size != 0) {
                copy_size = (left_size >= MMC_MAX_COPY_SIZE) ? MMC_MAX_COPY_SIZE : left_size;
                copy_buff_from_fs(file_handle, des, copy_size);
                des += (copy_size / ADDR_TRANS);
                left_size -= copy_size;
                seek_offset = (uint32_t)fseek(file_handle, offset + (size - left_size), SEEK_SET);
                if (seek_offset != 0) {
                    PRINT("lseek error. return value is %d.\r\n", seek_offset);
                    return;
                }
            }
            break;
        default:
            break;
    }
}

static void copy_section_from_fs(section_head_t *head, FILE* fp, core_images_e image, uint64_t image_size)
{
    uint32_t type;
    uint32_t *run_addr_begin = NULL;
    uint32_t *run_addr_end = NULL;
    uint32_t *load_addr = NULL;
    uint32_t section_size;
    for (uint32_t i = 0; i < head->section_num && i < SECTION_MAX; i++) {
        type = head->section_info[i].section_type;
        run_addr_begin = (uint32_t *)((char *)head->section_info[i].section_run_begin);
        run_addr_end = (uint32_t *)((char *)head->section_info[i].section_run_end);
        load_addr = cpu_load_translate_into_mcu_xip_addr((uint32_t*)head->section_info[i].section_load_addr, image);
        section_size = (uint32_t)((run_addr_end - run_addr_begin) * sizeof(uint32_t));
        run_addr_begin = (uint32_t *)(uintptr_t)cpu_load_caculate_addr((uint32_t)(uintptr_t)run_addr_begin, image);
        uint32_t seek_offset = (type == SECTION_TYPE_RELOAD) ?
            ((uint32_t)(uintptr_t)load_addr + FLASH_SIGN_HEADER) : 0;
#ifndef SUPPORT_LOAD_TIME_COUNT
        PRINT("index = %x, type = %x, load = %x, run = %x, section_size = %d, seek_offset = %d\r\n",
            i, type, load_addr, run_addr_begin, section_size, seek_offset);
#endif
        if (seek_offset >= image_size) {
            continue;
        }
        cpu_load_image_process_fs(fp, type, seek_offset, run_addr_begin, section_size);
    }
}

static void close_image_file(FILE* fp)
{
    if (fp == NULL) {
        return;
    }
    if (fclose(fp) != 0) {
        PRINT("image close error.\r\n");
    }
}

void cpu_load_image_from_fs(uint16_t image_sign_length, core_images_e image)
{
    FILE* fp = NULL;
    switch (image) {
        case CORE_IMAGES_BT:
            fp = fopen(IMAGE_BT_PATH, "r");
            break;
        default:
            break;
    }
    if (fp == NULL) {
        PRINT("image open error. fp:%p.\r\n", fp);
        return;
    }
    // 获取镜像大小
    if (fseek(fp, 0, SEEK_END) != 0) {
        close_image_file(fp);
        return;
    }
    uint64_t image_size = (uint64_t)ftell(fp);
    // 签名信息忽略
    if (fseek(fp, image_sign_length, SEEK_SET)) {
        close_image_file(fp);
        return;
    }
    uint8_t buff[INFO_READ_LEN] = {0};
    uint32_t read_len = fread((void*)buff, sizeof(uint8_t), INFO_READ_LEN, fp);
    if (read_len != INFO_READ_LEN) {
        close_image_file(fp);
        PRINT("read image error, image_size(%llu), read_len(%d).\r\n", image_size, read_len);
        return;
    }
    section_head_t *head = (section_head_t *)buff;
    if ((head == NULL) || (head->section_num == 0xFFFFFFFF)) {
        close_image_file(fp);
        PRINT("The mirroring header information is invalid.\n");
        return;
    }
    copy_section_from_fs(head, fp, image, image_size);
    close_image_file(fp);
}
#endif

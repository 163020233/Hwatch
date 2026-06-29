/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: crash data
 *
 * Create:
 */

#ifndef CRASH_DATA_H
#define CRASH_DATA_H

#include "panic.h"
#include "exception.h"

#ifdef SUPPORT_CRASH_DATA_RAM

#include "dfx_feature_config.h"

#ifndef CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM
#define CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM      CONFIG_DFX_SUPPORT_FILE_SYSTEM
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


#define CRASH_MAX_CALL_STACK            64
#define MAX_SAVE_ITEM_NAME_LEN          24
#define CRASH_SAVE_REGION_MAGIC         0x45584345
#define CRASH_SAVE_REGION_END_MAGIC     0x43524148

#if (defined(CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS) && (CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS == DFX_YES))
#define DUMP_INFO_MAX_NUM       5
#define CRASH_DATA_EXE_FILE_PREFIX       "dumpinfo"
#endif

typedef struct {
    uint8_t core;
    uint8_t save_after_reboot;
    uint8_t padding[2];
    uint32_t time_s;
    exception_info_t exception_info;
    panic_desc_t panic;
    exc_info_save_t exc_info_save;
    uint8_t stack[CRASH_MAX_CALL_STACK * EXC_STACK_INFO_SIZE];
} crash_data_t;

typedef struct {
    const char* path;   /* 保存至文件系统时，必须为文件系统中完整路径; 无文件系统时可只有文件名 */
    uint32_t addr;      /* 内存起始地址 */
    uint32_t size;      /* 内存大小 */
    uint8_t type;       /* 内存类型 */
    uint8_t compress;   /* 是否压缩 */
    uint16_t need_buf;   /* 是否需要缓存 */
} crash_data_mem_t;

typedef enum {
    CRASH_DATA_MEM_TYPE_SHARED,
    CRASH_DATA_MEM_TYPE_MALLOC,
    CRASH_DATA_MEM_TYPE_TRACE,
    CRASH_DATA_MEM_TYPE_MMC,
} crash_data_mem_type_t;

typedef struct {
    uint8_t name[MAX_SAVE_ITEM_NAME_LEN];   /* 该部分名称 */
    uint32_t offset : 28;                   /* 该部分在flash中dump存储区中的偏移 */
    uint32_t reserved_1 : 4;                /* 预留字段 */
    uint32_t size : 28;                     /* 该部分长度(实际长度) */
    uint32_t compress : 1;                  /* 是否压缩 */
    uint32_t reserved_2 : 3;                /* 预留字段 */
    uint8_t data[0];                        /* 在flash中，data长度4字节对齐，但size不含padding部分 */
} crash_data_save_header_t;

typedef void (*crash_data_reboot_cb)(int32_t);

#ifdef SAVE_EXC_INFO
#if (CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM == DFX_YES)
/**
 * @brief  Write exc info to the file system.
 * @return The length of file written if success or -1 if failed.
 */
int32_t crash_data_write_to_file(const char *path, const uint8_t *buf, uint32_t size, bool new_file);
#endif

errcode_t crash_data_save(uint8_t core);
#endif
crash_data_mem_t *crash_data_get_info(uint8_t core, uint32_t *num);
crash_data_t *crash_data_get_by_core(uint8_t core);

void crash_data_save_proc(const uint8_t *data, uint16_t size);
void crash_data_clear(void);
void crash_data_register_reboot_handle(crash_data_reboot_cb func);
void crash_data_set_save_after_reboot(void);

void crash_data_set_time_s(void);

exc_info_save_t *crash_data_get_exc_info_save(void);
void crash_data_set_exc_info_save(exc_info_save_t *exc_info_save, uint16_t stack_size);

exception_info_t *crash_data_get_exception_info(void);
void crash_data_set_exception_info(exception_info_t *exception_info);

panic_desc_t *crash_data_get_panic(void);
void crash_data_set_panic(panic_desc_t *panic);
#if (defined(CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS) && (CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS == DFX_YES))
int crash_data_get_file_index(void);
#endif
void crash_data_os_info_display(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* SUPPORT_CRASH_DATA_RAM */

#endif /* CRASH_DATA_H */

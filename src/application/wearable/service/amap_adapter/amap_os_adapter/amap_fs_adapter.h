/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Create Date : 2025-11
 */

#ifndef AMAP_FS_ADAPTER
#define AMAP_FS_ADAPTER

#include <stdbool.h>
#include <stdint.h>
#include "awk_adapter.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

void *amap_file_open(const char *filename, const char *mode);
int amap_file_close(void *handler);
size_t amap_file_read(void *ptr, size_t size, void *handler);
size_t amap_file_write(void *ptr, size_t size, void *handler);
long amap_file_tell(void *handler);
int amap_file_seek(void *handler, long offset, int where);
int amap_file_flush(void *handler);
bool amap_file_exists(const char *path);
int amap_file_remove(const char *path);
bool amap_file_dir_exists(const char *path);
int amap_file_mkdir(const char *path, uint16_t model);
int amap_file_rmdir(const char *path);
void *amap_file_opendir(const char *path);
int amap_file_closedir(void *dir);
bool amap_file_readdir(void *dir, awk_readdir_result *result);
size_t amap_file_get_size(const char *path);
long amap_file_get_last_access(const char *path);
int amap_file_rename(const char *old_name, const char *new_name);
bool amap_file_unzip(const char *zip_file, const char *out_dir);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // AMAP_FS_ADAPTER
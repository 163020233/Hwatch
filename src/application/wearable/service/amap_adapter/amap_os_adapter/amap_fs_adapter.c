/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Create Date : 2025-11
 */

#include "amap_fs_adapter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include "securec.h"
#include "amap_log_adapter.h"
#include "map_config.h"

#define BYTE_UNIT 1
#define FILE_NODE_CAPACITY 32
#define PATH_BUFFER_SIZE 512  // 目录/文件路径缓冲区大小

typedef enum { AMAP_FS_OK = 0, AMAP_FS_ERR = -1 } AmapFsResult;

typedef struct {
    char *filename;
    char *out_path;
    char *buffer;
} unzip_buffers;

typedef struct {
    DIR *dp;
    char path[PATH_BUFFER_SIZE];
} amap_dir_t;

static int IsPathUnderBaseDir(const char *path)
{
    if (path == NULL) {
        return AMAP_FS_ERR;
    }

    char realBase[PATH_MAX] = {0};
    if (realpath(MAP_BASE_DIR, realBase) == NULL) {
        AMAP_LOGE("realpath failed for base dir");
        return AMAP_FS_ERR;
    }

    // 提取父目录
    char parentPath[PATH_MAX] = {0};
    if (strncpy_s(parentPath, sizeof(parentPath), path, PATH_MAX - 1) != 0) {
        AMAP_LOGE("strncpy_s failed for path: %s", path);
        return AMAP_FS_ERR;
    }
    char *lastSlash = strrchr(parentPath, '/');
    if (lastSlash == NULL) {
        AMAP_LOGE("Invalid path format: %s", path);
        return AMAP_FS_ERR;
    }
    *lastSlash = '\0';  // 截断为父目录

    // 解析父目录(必须存在)
    char realParent[PATH_MAX] = {0};
    if (realpath(parentPath, realParent) == NULL) {
        AMAP_LOGE("realpath failed for parent: %s", parentPath);
        return AMAP_FS_ERR;
    }

    // 校验父目录是否在 MAP_BASE_DIR 下
    size_t baseLen = strlen(realBase);
    if (strncmp(realParent, realBase, baseLen) != 0) {
        return AMAP_FS_ERR;
    }
    if (realParent[baseLen] != '/' && realParent[baseLen] != '\0') {
        return AMAP_FS_ERR;
    }

    return AMAP_FS_OK;
}

void *amap_file_open(const char *filename, const char *mode)
{
    AMAP_LOGD("filename: %s ", filename);
    if (IsPathUnderBaseDir(filename) != AMAP_FS_OK) {
        AMAP_LOGE("Access denied, invalid path: %s", filename);
        return NULL;
    }
    FILE *fp = fopen(filename, mode);
    if (!fp) {
        AMAP_LOGE("Failed to open file: %s, errno=%d", filename, errno);
        return NULL;
    }
    return fp;
}

int amap_file_close(void *handler)
{
    if (!handler) {
        AMAP_LOGE("Invalid file handler in close");
        return AMAP_FS_ERR;
    }
    return fclose((FILE *)handler);
}

size_t amap_file_read(void *ptr, size_t size, void *handler)
{
    if (!handler) {
        AMAP_LOGE("Invalid file handler in read");
        return AMAP_FS_OK;
    }
    size_t ret = fread(ptr, BYTE_UNIT, size, (FILE *)handler);
    AMAP_LOGD("awk_file_read_adapter %zu %zu\n", size, ret);
    return ret;
}

size_t amap_file_write(void *ptr, size_t size, void *handler)
{
    if (!handler) {
        AMAP_LOGE("Invalid file handler in write");
        return AMAP_FS_OK;
    }
    size_t ret = fwrite(ptr, BYTE_UNIT, size, (FILE *)handler);
    return ret;
}

long amap_file_tell(void *handler)
{
    if (!handler) {
        AMAP_LOGE("Invalid file handler in tell");
        return AMAP_FS_ERR;
    }
    return ftell((FILE *)handler);
}

int amap_file_seek(void *handler, long offset, int where)
{
    if (!handler) {
        AMAP_LOGE("Invalid file handler in seek");
        return AMAP_FS_ERR;
    }
    return fseek((FILE *)handler, offset, where);
}

int amap_file_flush(void *handler)
{
    if (!handler) {
        AMAP_LOGE("Invalid file handler in flush");
        return AMAP_FS_ERR;
    }
    return fflush((FILE *)handler);
}

bool amap_file_exists(const char *path)
{
    AMAP_LOGD("path = %s", path);
    struct stat st;
    int ret = stat(path, &st) == AMAP_FS_OK && S_ISREG(st.st_mode);
    return ret;
}

int amap_file_remove(const char *path)
{
    return remove(path);
}

bool amap_file_dir_exists(const char *path)
{
    AMAP_LOGD("path = %s", path);
    struct stat st;
    int ret = stat(path, &st) == AMAP_FS_OK && S_ISDIR(st.st_mode);
    return ret;
}

int amap_file_mkdir(const char *path, uint16_t mode)
{
    return mkdir(path, mode);
}

static int remove_entry(const char *parent, const char *name)
{
    char full_path[PATH_BUFFER_SIZE];
    int ret = snprintf_s(full_path, PATH_BUFFER_SIZE, PATH_BUFFER_SIZE - 1, "%s/%s", parent, name);
    if (ret < 0) {
        AMAP_LOGE("Path formatting failed for: %s", name);
        return AMAP_FS_ERR;
    }

    struct stat st;
    if (stat(full_path, &st) != AMAP_FS_OK) {
        AMAP_LOGE("stat failed: %s", full_path);
        return AMAP_FS_ERR;
    }

    int rm_ret = S_ISDIR(st.st_mode) ? amap_file_rmdir(full_path) : amap_file_remove(full_path);
    if (rm_ret != AMAP_FS_OK) {
        AMAP_LOGE("Failed to remove: %s", full_path);
    }
    return rm_ret;
}

static int RemoveDirEntries(const char *path, void *dir)
{
    awk_readdir_result result = {0};

    if (!amap_file_readdir(dir, &result)) {
        AMAP_LOGE("readdir failed");
        return AMAP_FS_ERR;
    }
    int status = AMAP_FS_OK;
    for (size_t i = 0; i < result.size; ++i) {
        if (remove_entry(path, result.nodes[i].file_name) != AMAP_FS_OK) {
            status = AMAP_FS_ERR;
        }
    }

    free(result.nodes);
    return status;
}

int amap_file_rmdir(const char *path)
{
    AMAP_LOGD("directory: %s", path);

    int status = AMAP_FS_OK;
    void *dir = amap_file_opendir(path);
    if (dir != NULL) {
        status = RemoveDirEntries(path, dir);
        amap_file_closedir(dir);
    }
    if (status != AMAP_FS_OK) {
        return AMAP_FS_ERR;
    }
    return (rmdir(path) == 0) ? status : AMAP_FS_ERR;
}

void *amap_file_opendir(const char *path)
{
    AMAP_LOGD("opendir: %s", path);
    DIR *dp = opendir(path);
    if (!dp) {
        AMAP_LOGE("Failed to open directory: %s, errno = %d", path, errno);
        return NULL;
    }

    amap_dir_t *adir = (amap_dir_t *)malloc(sizeof(amap_dir_t));
    if (!adir) {
        AMAP_LOGE("Failed to allocate memory for directory struct");
        closedir(dp);
        return NULL;
    }

    adir->dp = dp;
    errno_t err = strncpy_s(adir->path, sizeof(adir->path), path, sizeof(adir->path) - 1);
    if (err != AMAP_FS_OK) {
        AMAP_LOGE("Failed to copy path: %s, error = %d", path, err);
        free(adir);
        closedir(dp);
        return NULL;
    }

    return adir;
}

int amap_file_closedir(void *dir)
{
    if (!dir) {
        AMAP_LOGE("Invalid directory handler in closedir");
        return AMAP_FS_ERR;
    }
    amap_dir_t *adir = (amap_dir_t *)dir;
    AMAP_LOGD("closedir: %s", adir->path);
    int ret = closedir(adir->dp);
    free(adir);
    return ret;
}

static bool ExpandFileNodes(awk_readdir_result *result, size_t *capacity)
{
    size_t newCapacity = (*capacity) * 2;

    awk_file_node *newNodes = (awk_file_node *)realloc(result->nodes, newCapacity * sizeof(awk_file_node));
    if (newNodes == NULL) {
        AMAP_LOGE("Failed to realloc file nodes");
        return false;
    }

    size_t oldSize = (*capacity) * sizeof(awk_file_node);
    size_t extendSize = (newCapacity - (*capacity)) * sizeof(awk_file_node);

    errno_t err = memset_s(((uint8_t *)newNodes) + oldSize, extendSize, 0, extendSize);
    if (err != EOK) {
        AMAP_LOGE("memset_s failed");
        free(newNodes);
        return false;
    }

    result->nodes = newNodes;
    *capacity = newCapacity;

    return true;
}

static void FillFileNode(amap_dir_t *adir, struct dirent *entry, awk_file_node *node)
{
    errno_t err = strncpy_s(node->file_name, sizeof(node->file_name), entry->d_name, sizeof(node->file_name) - 1);
    if (err != EOK) {
        AMAP_LOGE("Failed to copy filename: %s", entry->d_name);
        node->file_name[0] = '\0';
    }

    char fullPath[PATH_BUFFER_SIZE];
    int ret = snprintf_s(fullPath, PATH_BUFFER_SIZE, PATH_BUFFER_SIZE - 1, "%s/%s", adir->path, entry->d_name);
    if (ret < 0) {
        AMAP_LOGE("Path formatting failed for: %s/%s", adir->path, entry->d_name);
        node->file_size = 0;
        return;
    }

    struct stat st;
    if (stat(fullPath, &st) == AMAP_FS_OK) {
        node->file_size = (size_t)st.st_size;
    } else {
        AMAP_LOGE("Failed to stat file: %s", fullPath);
        node->file_size = 0;
    }
}

bool amap_file_readdir(void *dir, awk_readdir_result *result)
{
    if (dir == NULL || result == NULL) {
        AMAP_LOGE("Invalid directory handler or result pointer");
        return false;
    }

    amap_dir_t *adir = (amap_dir_t *)dir;
    size_t capacity = FILE_NODE_CAPACITY;
    size_t count = 0;
    result->nodes = (awk_file_node *)calloc(capacity, sizeof(awk_file_node));
    if (result->nodes == NULL) {
        AMAP_LOGE("Failed to allocate memory for file nodes");
        return false;
    }

    struct dirent *entry = NULL;
    while ((entry = readdir(adir->dp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (count >= capacity) {
            if (!ExpandFileNodes(result, &capacity)) {
                AMAP_LOGW("Memory limit reached, partial results returned");
                break;
            }
        }
        FillFileNode(adir, entry, &result->nodes[count]);
        ++count;
    }

    result->size = count;
    if (count == 0) {
        free(result->nodes);
        result->nodes = NULL;
    }

    AMAP_LOGD("readdir done, path: %s size: %zu", adir->path, result->size);
    return true;
}

size_t amap_file_get_size(const char *path)
{
    AMAP_LOGD("path: %s", path);
    struct stat st;
    if (stat(path, &st) == AMAP_FS_OK) {
        return (size_t)st.st_size;
    }

    AMAP_LOGE("Failed to get file size: %s", path);
    return AMAP_FS_OK;
}

long amap_file_get_last_access(const char *path)
{
    return AMAP_FS_OK;
}

int amap_file_rename(const char *old_name, const char *new_name)
{
    return rename(old_name, new_name);
}

bool amap_file_unzip(const char *zip_file, const char *out_dir)
{
    return true;
}

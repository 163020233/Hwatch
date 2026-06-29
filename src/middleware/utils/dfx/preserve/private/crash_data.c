/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: crash data
 */

#include "crash_data.h"
#ifdef SUPPORT_CRASH_DATA_RAM
#include "common_def.h"
#include "memory_config.h"
#ifdef SAVE_EXC_INFO
#include "dfx_adapt_layer.h"
#if (CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM == DFX_YES)
#include "fcntl.h"
#include "unistd.h"
#include "dfx_file_operation.h"
#include "sys/stat.h"
#if defined(CFG_DRIVERS_NANDFLASH) && !defined(FMC_SUPPORT)
#include "nandflash_config.h"
#endif /* CFG_DRIVERS_NANDFLASH */
#endif /* #if (CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM == DFX_YES) */
#endif /* SAVE_EXC_INFO */

#if defined(CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE) && (CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE == DFX_YES) \
    && CORE == MASTER_BY_ALL
#include "log_file.h"
#endif

#include "systick.h"
#include "securec.h"
#include "soc_osal.h"

#define APP_CORE_CRASH_OFFSET 0x0
#define BT_CORE_CRASH_OFFSET        (APP_CORE_CRASH_OFFSET      +   0x400)
#define GNSS_CORE_CRASH_OFFSET      (BT_CORE_CRASH_OFFSET       +   0x400)
#define SENSOR_CORE_CRASH_OFFSET    (GNSS_CORE_CRASH_OFFSET     +   0x400)
#define CO_CORE_CRASH_OFFSET        (SENSOR_CORE_CRASH_OFFSET   +   0x400)

#ifdef SAVE_EXC_INFO

#ifndef DFX_CRASH_FILE_MAX_LEN
#define DFX_CRASH_FILE_MAX_LEN          48
#endif

#ifndef DFX_CRASH_DATA_BUFFER_MAX_LEN
#define DFX_CRASH_DATA_BUFFER_MAX_LEN   8192
#endif

#define CRASH_FILE_BUF_LEN              64

__attribute__((weak)) void crash_data_os_info_display(void)
{
    return;
}

#if ((defined(CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS) && (CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS == DFX_YES)) \
    || (CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM == DFX_NO))
/* 获取路径中的文件名 */
STATIC const char *crash_data_extract_filename(const char* path)
{
    if (path == NULL) {
        return NULL;
    }
    const char *last_data = strrchr(path, '/');
    if (last_data == NULL) {
        /* path中只有文件名，没有路径 */
        return path;
    }
    return last_data + 1;
}
#endif

#if (CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM == DFX_YES) /* 保存crash data至文件系统 */

#if (defined(CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS) && (CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS == DFX_YES))
/*
 * 当支持保存多份crash记录时，在路径path的最后一级目录创建若干个文件夹，每份记录保存在一个文件夹中。
 * 文件件命名为dumpinfo_序号，如：dumpinfo_0、dumpinfo_1、…… 最大数据数量为DUMP_INFO_MAX_NUM个，循环覆盖。
 * 如：/user/exc/bt_exc_info.bin 实际保存路径为/user/exc/dumpinfo_0/bt_exc_info.bin。
 * 并在/user/exc/目录下生成crash_index_record.idx文件，其中保存当前最新的死机信息序号。
 */
static int g_file_index = 0;
static char *g_crash_data_index = "crash_index_record.idx";

int crash_data_get_file_index(void)
{
    return g_file_index;
}

static errcode_t crash_data_extract_directory(const char* path, char* result, int len)
{
    int dir_len;

    if (path == NULL || result == NULL) {
        return ERRCODE_FAIL;
    }

    if (strcmp(path, "/") == 0) {
        return ERRCODE_SUCC;
    }

    const char *file_name = crash_data_extract_filename(path);
    if (file_name == NULL || file_name == path) {
        return ERRCODE_FAIL;
    }

    dir_len = file_name - path;
    if (memcpy_s(result, len, path, dir_len) != EOK) {
        return ERRCODE_FAIL;
    }
    if (dir_len < len) {
        result[dir_len] = '\0';
    }

    return ERRCODE_SUCC;
}

static int32_t crash_file_mkdir(char *path)
{
    int path_len = (int)strlen(path);
    if (path_len <= 0) {
        return -1;
    }

    for (int i = 0; i < path_len; i++) {
        if (i > 0 && path[i] == '/') {
            path[i] = '\0';
            if (access(path, F_OK) == 0) {
                path[i] = '/';
                continue;
            }

            if (mkdir(path, S_IREAD | S_IWRITE) != 0) {
                PRINT("[ERR]mkdir %s failed, errno is %d. \r\n", path, get_errno());
                return -1;
            }
            path[i] = '/';
        }
    }

    return 0;
}

/* 创建dump_info文件夹 */
static void crash_data_create_folder(const char* path, char *temp_path, int len)
{
    int fd;
    int ret;
    int idx = 0;
    char temp_file[CRASH_FILE_BUF_LEN];

    if (crash_data_extract_directory(path, temp_path, len) != ERRCODE_SUCC) {
        PRINT("%s open failed\r\n", path);
        return;
    }

    ret = crash_file_mkdir(temp_path);
    if (ret < 0) {
        PRINT("mkdir %s failed. \r\n", temp_path);
        return;
    }

    ret = snprintf_s(temp_file, CRASH_FILE_BUF_LEN, CRASH_FILE_BUF_LEN - 1, "%s%s", temp_path, g_crash_data_index);
    if (ret < 0) {
        return;
    }

    /* 打开crash_index_record.idx文件，并写入当前索引 */
    fd = open(temp_file, O_CREAT | O_RDWR, S_IREAD | S_IWRITE);
    if (fd < 0) {
        PRINT("%s open failed errno is %d\r\n", temp_file, get_errno());
        return;
    }

    if (lseek(fd, 0, SEEK_END) == 0) {
        write(fd, &idx, sizeof(int));
        g_file_index = idx;
    } else {
        lseek(fd, 0, SEEK_SET);
        read(fd, &idx, sizeof(int));
        idx++;
        idx = idx % DUMP_INFO_MAX_NUM;
    }

    ret = snprintf_s(temp_path, len, len - 1, "%s/%s_%d", temp_path, CRASH_DATA_EXE_FILE_PREFIX, idx);
    if (ret < 0) {
        close(fd);
        return;
    }

    /* 创建dump_info_x目录 */
    ret = mkdir(temp_path, S_IRWXU | S_IRWXG | S_IRWXO);
    if (ret < 0 && errno != EEXIST) {
        close(fd);
        return;
    }

    lseek(fd, 0, SEEK_SET);
    write(fd, &idx, sizeof(int));

    g_file_index = idx;

    fsync(fd);
    close(fd);

    return;
}

static errcode_t crash_data_get_full_path(const char* path, char *full_path, char *temp_path, int len)
{
    int ret;
    const char *crash_data_file;

    crash_data_file = crash_data_extract_filename(path);
    if (crash_data_file == NULL) {
        return ERRCODE_FAIL;
    }

    ret = snprintf_s(full_path, len, len - 1, "%s/%s", temp_path, crash_data_file);
    if (ret < 0) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}
#endif /* CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS */

int32_t crash_data_write_to_file(const char *path, const uint8_t *buf, uint32_t size, bool new_file)
{
    int fd;
    int ret;
    ssize_t length;
    int flag;

    if (new_file) {
        flag = O_RDWR | O_CREAT | O_TRUNC;
    } else {
        flag = O_RDWR | O_CREAT | O_APPEND;
    }

    fd = open(path, flag,
        S_IRUSR | S_IWUSR |    // 用户读写 (rw-)
        S_IRGRP | S_IWGRP |    // 组读写   (rw-)
        S_IROTH | S_IWOTH);    // 其他读写 (rw-)
    if (fd < 0) {
        if (dfx_file_mkdir(path) != 0) {
            return -1;
        }
        fd = open(path, flag,
            S_IRUSR | S_IWUSR |    // 用户读写 (rw-)
            S_IRGRP | S_IWGRP |    // 组读写   (rw-)
            S_IROTH | S_IWOTH);    // 其他读写 (rw-)
        if (fd < 0) {
            return -1;
        }
    }
    int file_pos = (int)lseek(fd, 0, SEEK_END);
    if (file_pos < 0) {
        close(fd);
        return -1;
    }
    length = write(fd, buf, size);
    if (length < 0) {
        close(fd);
        return -1;
    }
    ret = close(fd);
    if (ret < 0) {
        return -1;
    }
    return length;
}

#else /* CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM */

/* 保存crash data至Norflash */
#define DUMP_INFO_LENGTH    0x4000

static uint32_t g_crash_data_max_num = 0;
static uint32_t g_crash_data_cur_offset = 0;
static uint32_t g_prev_crash_header_offset = 0;
static uint32_t g_prev_crash_data_size = 0;
static crash_data_save_header_t g_prev_crash_header = {0};

__attribute__((weak)) uint32_t dfx_flash_get_region_size(uint8_t opt_type)
{
    if (opt_type == FLASH_OP_TYPE_DUMP_INFO) {
        return DUMP_INFO_LENGTH;
    } else {
        return 0;
    }
}

STATIC errcode_t crash_data_prepare_flash(uint32_t crash_data_num)
{
    uint32_t dump_len = dfx_flash_get_region_size(FLASH_OP_TYPE_DUMP_INFO);
    if (dump_len == 0) {
        return ERRCODE_FAIL;
    }

    errcode_t ret = dfx_flash_erase(FLASH_OP_TYPE_DUMP_INFO, 0, dump_len);
    if (ret != ERRCODE_SUCC) {
        PRINT("[crash] erase flash failed, ret = 0x%x\n", ret);
        return ret;
    }

    /*
     * Crash 存储结构：
     * start magic              (size: 4 bytes)
     * actual file num          (size: 4 bytes)
     * crash_header             (size: crash_item_num * sizeof(crash_data_save_header_t))
     * crash_data_file_1
     * crash_data_file_2
     *        ……
     * crash_data_file_[num-1]
     * end magic                (size: 4 bytes)
     */

    /* 写开头魔术字 */
    uint32_t crash_magic = CRASH_SAVE_REGION_MAGIC;
    (void)dfx_flash_write(FLASH_OP_TYPE_DUMP_INFO, g_crash_data_cur_offset, (const uint8_t *)(uintptr_t)&crash_magic,
        sizeof(uint32_t), 0);

    (void)memset_s(&g_prev_crash_header, sizeof(crash_data_save_header_t), 0, sizeof(crash_data_save_header_t));

    /* crash_header 位置 */
    g_prev_crash_header_offset = sizeof(uint32_t) + sizeof(uint32_t);

    /* 第一个crash data 位置 */
    g_crash_data_cur_offset = g_prev_crash_header_offset + (crash_data_num * sizeof(crash_data_save_header_t));

    g_crash_data_max_num = 0;
    g_prev_crash_data_size = 0;
    return ERRCODE_SUCC;
}

STATIC int32_t crash_data_write_header_to_flash(const char *path, bool compress)
{
    int32_t write_len = 0;
    int32_t total_len = 0;

    /* 新文件第一次写入，对当前offset做4字节对齐 */
    g_crash_data_cur_offset = ((g_crash_data_cur_offset + 3) & ~3); /* 3:  align 4 bytes */

    if (g_prev_crash_header.offset != 0) {
        /* 写入前一个文件的crash_data_header */
        write_len = dfx_flash_write(FLASH_OP_TYPE_DUMP_INFO, g_prev_crash_header_offset,
            (const uint8_t *)(uintptr_t)&g_prev_crash_header, sizeof(crash_data_save_header_t), 0);
        total_len += write_len;
        g_prev_crash_header_offset += sizeof(crash_data_save_header_t);

        /* 清空crash header buffer */
        (void)memset_s(&g_prev_crash_header, sizeof(crash_data_save_header_t), 0, sizeof(crash_data_save_header_t));
        g_crash_data_max_num++;
    }

    /* 记录当前文件的header信息 */
    g_prev_crash_header.offset = g_crash_data_cur_offset;
    g_prev_crash_header.compress = compress;
    const char *file_name = crash_data_extract_filename(path);
    uint32_t name_len = (file_name != NULL) ? strlen(file_name) : 0;
    name_len = uapi_min(name_len, MAX_SAVE_ITEM_NAME_LEN - 1);
    if (file_name != NULL && strlen(file_name) != 0) {
        if (memcpy_s(g_prev_crash_header.name, MAX_SAVE_ITEM_NAME_LEN - 1, file_name, name_len) != EOK) {
            ;
        }
    }
    return total_len;
}

STATIC int32_t crash_data_write_data_to_flash(const uint8_t *buf, uint32_t size)
{
    int32_t write_len = 0;

    /* 写入当前文件的crash data */
    write_len = dfx_flash_write(FLASH_OP_TYPE_DUMP_INFO, g_crash_data_cur_offset, buf, size, 0);
    g_crash_data_cur_offset += (uint32_t)write_len;

    /* 更新crash header中的size（使用原始size而不是实际写入的write_len） */
    g_prev_crash_header.size += (uint32_t)size;

    return write_len;
}

STATIC int32_t crash_data_write_end_to_flash(void)
{
    int32_t write_len = 0;
    int32_t total_len = 0;

    /* 写crash_data_header */
    write_len = dfx_flash_write(FLASH_OP_TYPE_DUMP_INFO, g_prev_crash_header_offset,
        (const uint8_t *)(uintptr_t)&g_prev_crash_header, sizeof(crash_data_save_header_t), 0);
    total_len += write_len;
    g_prev_crash_header_offset += sizeof(crash_data_save_header_t);
    g_crash_data_max_num++;

    /* 清空Header buffer */
    (void)memset_s(&g_prev_crash_header, sizeof(crash_data_save_header_t), 0, sizeof(crash_data_save_header_t));

    /* 写data number */
    write_len = dfx_flash_write(FLASH_OP_TYPE_DUMP_INFO, sizeof(uint32_t),
        (const uint8_t *)(uintptr_t)&g_crash_data_max_num, sizeof(uint32_t), 0);
    total_len += write_len;

    /* 写结尾魔术字 */
    uint32_t crash_magic = CRASH_SAVE_REGION_END_MAGIC;
    write_len = dfx_flash_write(FLASH_OP_TYPE_DUMP_INFO, g_crash_data_cur_offset,
        (const uint8_t *)(uintptr_t)&crash_magic, sizeof(uint32_t), 0);
    total_len += write_len;
    g_crash_data_cur_offset += (uint32_t)write_len;
    return total_len;
}

#endif /* CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM */

#ifdef SUPPORT_EXCEPTION_EXPORT_COMPRESS
#include "lz4.h"

#define CHUNK_SIZE (256 * 1024) // 每次处理256KB的数据
#define MIN_CHUNK_SIZE (16 * 1024)
int lz4_compress_bound(int isize)
{
    return isize + (isize / 255) + 16;  // 计算压缩后的最大大小isize + (isize / 255) + 16
}
static void *compress_temp_mem_alloc(unsigned int *allocSize, unsigned int *chunk_size)
{
    unsigned int max_compressed_size = lz4_compress_bound(CHUNK_SIZE);
    (void)max_compressed_size;

    unsigned int try_size = CHUNK_SIZE;
    while (try_size >= MIN_CHUNK_SIZE) {
        unsigned int max_compressed_size_t = lz4_compress_bound(try_size);
        char *compressed_buffer = (char *)malloc(max_compressed_size_t + 4);    // 4用于保存数据长度
        if (compressed_buffer == NULL) {
            try_size = try_size / 2;  // 1/2大小递减
            continue;
        }
        *allocSize = max_compressed_size_t;
        *chunk_size = try_size;
        return compressed_buffer;
    }
    return NULL;
}

uint32_t compress_dumpmem_write_file(char *filepath, uintptr_t start_addr, uint32_t length)
{
    unsigned int max_compressed_size = 0;
    unsigned int chunk_size = 0;
    char *compressed_buffer = compress_temp_mem_alloc(&max_compressed_size, &chunk_size);
    if (compressed_buffer == NULL) {
        return 0;
    }
    unsigned int current_addr = start_addr;
    unsigned int remaining = length;
    unsigned int already_write = 0;
    PRINT("prepare_write file:%s total %d \r\n", filepath, length);
    // 分段压缩并写入文件
    while (remaining > 0) {
        chunk_size = (remaining > chunk_size) ? chunk_size : remaining;

        // 使用LZ4进行压缩
        int compressed_size = lz4_compress((void *)current_addr, compressed_buffer + 4,
            chunk_size, max_compressed_size);
        if (compressed_size <= 0) {
            PRINT("compress fail:ret = 0x%x\n", compressed_size);
            already_write = 0;
            break;
        }
        PRINT("LZ4: addr:0x%x old(%d)->new(%d)\n", current_addr, chunk_size, compressed_size);
        *(unsigned int *)compressed_buffer = compressed_size;

        int32_t written = 0;
#if (CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM == DFX_YES)
        // 将压缩后的内容写入文件
        bool new_file_flag = true;
        if (remaining != length) {
            new_file_flag = false;
        }
        written = crash_data_write_to_file(filepath, (const uint8_t *)compressed_buffer,
            compressed_size + 4, new_file_flag); // 4字节中保存数据长度
#else
        written = crash_data_write_data_to_flash((const uint8_t *)compressed_buffer, compressed_size + 4);
#endif
        if (written != (int32_t)compressed_size + 4) {     // 4字节中保存数据长度
            PRINT("write compress dump mem fail, already_write %d Bytes\n", already_write);
            already_write = 0;
            break;
        }
        already_write += written;
        current_addr += chunk_size;
        remaining -= chunk_size;
        PRINT("total(%d)/write %d(%d), remain %d u32:0x%x, u32:0x%x\r\n",
            length, chunk_size, written, remaining,
            *(unsigned int *)compressed_buffer, *(unsigned int *)(compressed_buffer + 4));    // 4字节中保存数据长度
    }
    PRINT("file:%s write complete %d\r\n", filepath, already_write);
    free(compressed_buffer);
    return already_write;
}

#define COMPRESS_ALGORITHM_SUFFIX "_lz4"
#define MAX_FILE_NAME_LENGTH 100
STATIC int32_t crash_data_compresssave(const char *path, const uint8_t *buf, uint32_t size, bool last_data)
{
    unused(last_data);
    char crash_compress_name[MAX_FILE_NAME_LENGTH] = {0};
    int ret = snprintf_s(crash_compress_name, MAX_FILE_NAME_LENGTH, MAX_FILE_NAME_LENGTH - 1, "%s_lz4.bin", path);
    if (ret == -1) {
        return 0;
    }

    PRINT("file_name = %s, size = %u\n", crash_compress_name, size);

    int32_t length = 0;
#if (CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM == DFX_NO)
    length += crash_data_write_header_to_flash(crash_compress_name, true);
#endif /* CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM == DFX_NO */

    length += compress_dumpmem_write_file(crash_compress_name, (uintptr_t)buf, size);

#if (CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM == DFX_NO)
    if (last_data) {
        length += crash_data_write_end_to_flash();
    }
#endif /* CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM == DFX_NO */
    return length;
}
#endif /* SUPPORT_EXCEPTION_EXPORT_COMPRESS */

STATIC int32_t crash_data_save_to_device(crash_data_mem_t *crash_info, const char *path, bool new_file, bool last_data)
{
    PRINT("file_name = %s, size = %u\n", path, crash_info->size);
    int32_t len = 0;
    int32_t remaining = (int32_t)crash_info->size;
    int32_t chunks_size = (int32_t)crash_info->size;
    uint32_t written = 0;

    uint8_t *crash_buf = (uint8_t *)(uintptr_t)crash_info->addr;
    if (crash_info->need_buf == true) {
        chunks_size = uapi_min((int32_t)crash_info->size, DFX_CRASH_DATA_BUFFER_MAX_LEN);
        crash_buf = (uint8_t *)dfx_malloc(0, chunks_size);
        if (crash_buf == NULL) {
            return 0;
        }
    }

#if (CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM == DFX_YES)
    unused(last_data);

    while (remaining > 0) {
        if (crash_info->need_buf == true) {
            chunks_size = (remaining > chunks_size) ? chunks_size : remaining;
            (void)memcpy_s(crash_buf, chunks_size, (uint8_t *)(uintptr_t)(crash_info->addr + written), chunks_size);
        }
        len += crash_data_write_to_file(path, crash_buf, chunks_size,
            (new_file && (remaining == (int32_t)crash_info->size)));

        remaining -= chunks_size;
        written += (uint32_t)chunks_size;
    }

#else /* CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM */
    if (new_file) {
        len += crash_data_write_header_to_flash(path, false);
    }

    while (remaining > 0) {
        if (crash_info->need_buf == true) {
            chunks_size = (remaining > chunks_size) ? chunks_size : remaining;
            (void)memcpy_s(crash_buf, chunks_size, (uint8_t *)(uintptr_t)(crash_info->addr + written), chunks_size);
        }
        len += crash_data_write_data_to_flash(crash_buf, chunks_size);
        remaining -= chunks_size;
        written += (uint32_t)chunks_size;
    }

    if (last_data) {
        len += crash_data_write_end_to_flash();
    }
#endif /* CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM == DFX_YES */

    if (crash_info->need_buf == true) {
        dfx_free(0, (void *)crash_buf);
    }

    return len;
}

STATIC bool crash_data_is_new_file(const char *prev_file_path, const char *path)
{
    if ((prev_file_path == NULL) || (path == NULL)) {
        return true;
    }

    return (strncmp(prev_file_path, path, CRASH_FILE_BUF_LEN) == 0) ? false : true;
}

static int crash_data_export(crash_data_mem_t *crash_data_info, char *crash_data_path, bool new_file, bool last_data)
{
    int len = 0;
#ifdef SUPPORT_EXCEPTION_EXPORT_COMPRESS
    if (crash_data_info->compress == 1) {
        uint8_t *buf = (uint8_t *)(uintptr_t)crash_data_info->addr;
        if (crash_data_info->need_buf == true) {
            /* exporting TRACE data, need to copy to PSRAM, prevent instruction alignment failure. */
            uint8_t *newbuf = (uint8_t *)dfx_malloc(0, crash_data_info->size);
            if (newbuf == NULL) {
                goto COMPRESS_END;
            }
            memcpy_s(newbuf, crash_data_info->size, (uint8_t *)(uintptr_t)crash_data_info->addr, crash_data_info->size);
            buf = newbuf;
        }
        len = crash_data_compresssave(crash_data_path, buf, crash_data_info->size, last_data);
        if (crash_data_info->need_buf == true) {
            dfx_free(0, buf);
        }
    }
COMPRESS_END:
#endif /* SUPPORT_EXCEPTION_EXPORT_COMPRESS */
    if (len <= 0) {
        len = crash_data_save_to_device(crash_data_info, crash_data_path, new_file, last_data);
    }

    if (crash_data_info->type == CRASH_DATA_MEM_TYPE_MALLOC) {
        dfx_free(0, (void *)(uintptr_t)crash_data_info->addr);
    }
    return len;
}

errcode_t crash_data_save(uint8_t core)
{
    uint32_t data_num;
    crash_data_mem_t *crash_data_info = crash_data_get_info(core, &data_num);
    const char *prev_file_path = NULL;

    if (data_num == 0 || crash_data_info == NULL) {
        PRINT("[ERR] crash_data_save data_num = %d, crash_data_info = 0x%x\n", data_num, crash_data_info);
        return ERRCODE_FAIL;
    }

#if (defined(CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS) && (CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS == DFX_YES))
    /* 保存多份死机信息的场景下，获取当前目录名称，并创建目录（dump_info_x） */
    char temp_path[CRASH_FILE_BUF_LEN];
    char full_path[CRASH_FILE_BUF_LEN];
    crash_data_create_folder(crash_data_info[0].path, temp_path, CRASH_FILE_BUF_LEN);
#endif

#if (CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM == DFX_YES)
#if defined(CFG_DRIVERS_NANDFLASH) && !defined(FMC_SUPPORT)
    /* 设置nand为SPEED_SLOW和TRANS_BY_CPU_SINGLE_LINE模式，以便可以支持异常中使用文件系统接口 */
    nand_driver_init(SPEED_SLOW);
    set_trans_type(TRANS_BY_CPU_SINGLE_LINE);
#endif /* CFG_DRIVERS_NANDFLASH */
#else
    if (crash_data_prepare_flash(data_num) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
#endif /* CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM */

    for (uint32_t i = 0; i < data_num; i++) {
#if (defined(CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS) && (CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS == DFX_YES))
        const char *crash_data_path = full_path;
        if (crash_data_get_full_path(crash_data_info[i].path, full_path, temp_path, CRASH_FILE_BUF_LEN) !=
            ERRCODE_SUCC) {
            continue;
        }
#else
        const char *crash_data_path = crash_data_info[i].path;
#endif

        bool new_file = crash_data_is_new_file(prev_file_path, crash_data_info[i].path);

        int32_t len = 0;
        len = crash_data_export(&crash_data_info[i], (char *)crash_data_path, new_file, (i == (data_num - 1)));
        uapi_watchdog_kick();
        if (len <= 0) {
            PRINT("[crash] write data failed, file type = %d, path = %s, addr = 0x%x, len = %d\n",
                  crash_data_info[i].type, crash_data_path, crash_data_info[i].addr, crash_data_info[i].size);
            return ERRCODE_FAIL;
        }
        prev_file_path = crash_data_info[i].path;
    }
    PRINT("save crash data done!\r\n");
    return ERRCODE_SUCC;
}
#endif /* SAVE_EXC_INFO */

#if MCU_ONLY
static crash_data_reboot_cb g_reboot_mcu_handle = NULL;
#endif

__attribute__((weak)) crash_data_t *crash_data_get_by_core(uint8_t core)
{
    if (core == APPS) {
        return (crash_data_t *)(uintptr_t)(CRASH_MEM_REGION_START + APP_CORE_CRASH_OFFSET);
    } else if (core == BT) {
        return (crash_data_t *)(uintptr_t)(CRASH_MEM_REGION_START + BT_CORE_CRASH_OFFSET);
#if CORE == GNSS
    } else if (core == GNSS) {
        return (crash_data_t *)(uintptr_t)(CRASH_MEM_REGION_START + GNSS_CORE_CRASH_OFFSET);
#endif
#if CORE == SENSOR
    } else if (core == SENSOR) {
        return (crash_data_t *)(uintptr_t)(CRASH_MEM_REGION_START + SENSOR_CORE_CRASH_OFFSET);
#endif
#if CORE == CCORE
    } else if (core == CCORE) {
        return (crash_data_t *)(uintptr_t)(CRASH_MEM_REGION_START + CO_CORE_CRASH_OFFSET);
#endif
    }
    return NULL;
}

void crash_data_save_proc(const uint8_t *data, uint16_t size)
{
    unused(data);
    unused(size);
#if MCU_ONLY && defined(SAVE_EXC_INFO)
    uint8_t core = (uint8_t)*data;
    errcode_t ret = crash_data_save(core);
    if (ret != ERRCODE_SUCC) {
        PRINT("[crash] save failed, ret = 0x%x\n", ret);
    }
#if defined(CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE) && (CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE == DFX_YES)
    uapi_logfile_fsync(STORE_DIAG);
#endif
    if (g_reboot_mcu_handle != NULL) {  // when registering g_reboot_mcu_handle, need to restart chip by yourself.
        g_reboot_mcu_handle(core);
        return;
    }
#endif
}

void crash_data_clear(void)
{
    crash_data_t *crash_data = crash_data_get_by_core(CORE);
    if (crash_data != NULL) {
        memset_s(crash_data, sizeof(crash_data_t), 0, sizeof(crash_data_t));
        crash_data->core = CORE;
    }
}

void crash_data_set_save_after_reboot(void)
{
    crash_data_t *crash_data = crash_data_get_by_core(CORE);
    if (crash_data != NULL) {
        crash_data->save_after_reboot = 1;
    }
}

void crash_data_set_time_s(void)
{
    crash_data_t *crash_data = crash_data_get_by_core(CORE);
    if (crash_data != NULL) {
        crash_data->time_s = (uint32_t)uapi_systick_get_s();
    }
}

exc_info_save_t *crash_data_get_exc_info_save(void)
{
    crash_data_t *crash_data = crash_data_get_by_core(CORE);
    if (crash_data != NULL) {
        return &crash_data->exc_info_save;
    }
    return NULL;
}

void crash_data_set_exc_info_save(exc_info_save_t *exc_info_save, uint16_t stack_size)
{
    crash_data_t *crash_data = crash_data_get_by_core(CORE);
    if (crash_data != NULL) {
        if (stack_size > CRASH_MAX_CALL_STACK) {
            return;
        }

        if (exc_info_save == NULL) {
            return;
        }
        if (memcpy_s(&crash_data->exc_info_save, EXC_INFO_SAVE_SIZE + CRASH_MAX_CALL_STACK * EXC_STACK_INFO_SIZE,
            exc_info_save, EXC_INFO_SAVE_SIZE + stack_size * EXC_STACK_INFO_SIZE) != EOK) {
            ;
        }
    }
}

exception_info_t *crash_data_get_exception_info(void)
{
    crash_data_t *crash_data = crash_data_get_by_core(CORE);
    if (crash_data != NULL) {
        return &crash_data->exception_info;
    }
    return NULL;
}

void crash_data_set_exception_info(exception_info_t *exception_info)
{
    crash_data_t *crash_data = crash_data_get_by_core(CORE);
    if (crash_data != NULL) {
        if (memcpy_s(&crash_data->exception_info, sizeof(exception_info_t), exception_info,
            sizeof(exception_info_t)) != EOK) {
            ;
        }
    }
}

panic_desc_t *crash_data_get_panic(void)
{
    crash_data_t *crash_data = crash_data_get_by_core(CORE);
    if (crash_data != NULL) {
        return &crash_data->panic;
    }
    return NULL;
}

void crash_data_set_panic(panic_desc_t *panic)
{
    crash_data_t *crash_data = crash_data_get_by_core(CORE);
    if (crash_data != NULL) {
        if (memcpy_s(&crash_data->panic, sizeof(panic_desc_t), panic, sizeof(panic_desc_t)) != EOK) {
            ;
        }
    }
}

void crash_data_register_reboot_handle(crash_data_reboot_cb func)
{
#if MCU_ONLY
    if (func != NULL) {
        g_reboot_mcu_handle = func;
    }
#else
    unused(func);
#endif
}
#endif /* SUPPORT_CRASH_DATA_RAM */

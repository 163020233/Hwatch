/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Provides fs test api
 */
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "sys/statfs.h"
#ifdef CFG_DRIVERS_NANDFLASH
#include "nand.h"
#include "nand_fmc.h"
#endif
#include "common_def.h"
#include "soc_osal.h"
#include "watchdog.h"
#include "debug_print.h"
#include "dirent.h"
#ifdef CFG_DRIVERS_MMC
#include "dpal.h"
#include "block.h"
#include "soc/mmc.h"
#endif
#ifdef CFG_DRIVERS_SPINOR
#include "littlefs_init.h"
#endif
#include "at_adapter.h"
#include "tcxo.h"
#include "at_wear.h"
#include "diag_msg.h"
#include "at_wear_fs.h"
#include "efuse.h"

#define KB_SIZE                 (1024)
#define TEST_BUFFER_LEN         64
#define FILE_MAX_NUM            32
#define FS_NODE_INFO_SPACE      16

#if defined(CFG_DRIVERS_NANDFLASH)
#define FS_VOLUME_STRS "/boot/", "/system/", "/user/"
static const char *g_volume_names[] = {FS_VOLUME_STRS};
#elif defined(CFG_DRIVERS_MMC)
#define FS_VOLUME_STRS "/boot/", "/system/", "/inner/", "/systembk/", "/user/"
static const char *g_volume_names[] = {FS_VOLUME_STRS};
#elif defined(CFG_DRIVERS_SPINOR)
#define LFS_VOLUME_STRS "/user/"
static const char *g_volume_names[] = {LFS_VOLUME_STRS};
#endif
#define FS_VOLUMES_NUM (sizeof(g_volume_names) / sizeof(const char *))

typedef struct {
    uint32_t             para_map;
    uint32_t             block;
    uint32_t             page;
    uint32_t             pagenum;
} fmcnand_args_t;

int test_libc_write(FILE *fp, uint16_t rw_times)
{
    uint32_t len = 0;
    uint16_t index;
    uint8_t* write_buf = NULL;
    if (fp == NULL) {
        return 0;
    }

    write_buf = (uint8_t*)malloc(BYTES_LEN_4K);
    if (write_buf == NULL) {
        PRINT("[W]write malloc write buf error\r\n");
        return 0;
    }

    for (index = 0; index < BYTES_LEN_4K; ++index) {
        write_buf[index] = index;
    }

    uapi_watchdog_kick();
    for (index = 0; index < rw_times; ++index) {
        if (fwrite(write_buf, BYTES_LEN_4K, 1, fp) != 1) {
            PRINT("[W]test_libc_write fail! errno is %d\r\n", get_errno());
            continue;
        }
        len++;
    }

    len = len * BYTES_LEN_4K;
    free(write_buf);
    PRINT("[W]fwrite len:[%d]B.\r\n", len);
    return len;
}

int test_libc_read(FILE *fp, uint16_t rw_times)
{
    uint32_t len = 0;
    uint16_t index;
    uint8_t* write_buf = NULL;
    uint8_t* read_buf = NULL;
    if (fp == NULL) {
        return 0;
    }

    write_buf = (uint8_t*)malloc(BYTES_LEN_4K);
    if (write_buf == NULL) {
        PRINT("[R]write malloc write buf error\r\n");
        return 0;
    }

    read_buf = (uint8_t*)malloc(BYTES_LEN_4K);
    if (read_buf == NULL) {
        PRINT("[R]read malloc read buf error\r\n");
        free(write_buf);
        return 0;
    }

    for (index = 0; index < BYTES_LEN_4K; ++index) {
        write_buf[index] = index;
    }

    uapi_watchdog_kick();
    for (index = 0; index < rw_times; ++index) {
        if (fread(read_buf, BYTES_LEN_4K, 1, fp) != 1) {
            PRINT("[R]test_libc_read fail! errno is %d\r\n", get_errno());
            len--;
        }
        len++;
        if (memcmp(write_buf, read_buf, BYTES_LEN_4K) != 0) {
            PRINT("[R]rw compare fail, time = %d/%d\r\n", len, rw_times);
            break;
        }
    }
    len = len * BYTES_LEN_4K;
    free(write_buf);
    free(read_buf);
    PRINT("[R]fread len:[%d]B.\r\n",  len);
    return len;
}

int test_libc_rw(FILE *fp)
{
    uint32_t len, tmp_len;
    uint32_t rw_times = FS_TEST_1M_BYTES / BYTES_LEN_4K;

    rewind(fp);
    tmp_len = test_libc_write(fp, rw_times);
    rewind(fp);
    len = test_libc_read(fp, rw_times);
    if (tmp_len == len) {
        PRINT("[RW]rw check all data success!!\r\n");
        return AT_RET_OK;
    } else {
        PRINT("[RW]rw check all data fail!!\r\n");
        return AT_RET_SYNTAX_ERROR;
    }
}

#define MAX_DIR_SIZE 10
char g_file_dir[][MAX_DIR_SIZE] = {"boot", "user", "inner", "systembk", "system", "lfs0"};
at_ret_t wear_fs_write(const fs_write_check_args_t *argc)
{
    char *file_name = argc->name;
    uint32_t file_len = (uint32_t)argc->len;
    uint32_t times;
    uint32_t len;
    char file_dir[MAX_FILE_PATH_SIZE] = { 0 };
    uint8_t i;
    char *start = strchr(file_name, '/');
    char *end = strchr(start + 1, '/');
    int32_t ret = 0;

    strncpy_s(file_dir, MAX_FILE_PATH_SIZE, start + 1, end - start - 1);

    PRINT("wear_fs_write name=%s len=%d\r\n", file_name, file_len);
    for (i = 0; i < sizeof(g_file_dir) / sizeof(g_file_dir[0]); i++) {
        len = strlen(g_file_dir[i]);
        if (strncmp(file_dir, g_file_dir[i], len) == 0) {
            break;
        }
    }
    if (i >= sizeof(g_file_dir) / sizeof(g_file_dir[0])) {
        PRINT("file dir error %s\r\n", file_dir);
        return AT_RET_SYNTAX_ERROR;
    }

    if (file_len == 0) { return AT_RET_SYNTAX_ERROR; }

    if (access(file_name, F_OK) == 0) {
        ret = remove(file_name);
    }
    if (ret != 0) {
        PRINT("remove file name=%s, ret=%d", file_name, ret);
    }
    if (file_len > FS_TEST_100M_BYTES) { file_len = FS_TEST_100M_BYTES; }
    times = (file_len / BYTES_LEN_4K) ? (file_len / BYTES_LEN_4K) : 1;

    FILE *fp = NULL;
    fp = fopen(file_name, "w+");
    if (fp == NULL) {
        PRINT("open fail!path:%s\r\n", file_name);
        return AT_RET_SYNTAX_ERROR;
    }

    len = test_libc_write(fp, times);
    ret = fclose(fp);
    if (ret != 0) {
        PRINT("fclose error fp=%d", fp);
    }
    if (len == times * BYTES_LEN_4K) {
        return AT_RET_OK;
    } else {
        return AT_RET_SYNTAX_ERROR;
    }
}

at_ret_t wear_fs_check(const fs_write_check_args_t *argc)
{
    char *file_name = argc->name;
    uint32_t file_len = argc->len;
    uint32_t len, times;
    char file_dir[MAX_FILE_PATH_SIZE] = { 0 };
    uint8_t i;
    char *start = strchr(file_name, '/');
    char *end = strchr(start + 1, '/');
    FILE *fp = NULL;
    int32_t ret;

    strncpy_s(file_dir, MAX_FILE_PATH_SIZE, start + 1, end - start - 1);

    for (i = 0; i < sizeof(g_file_dir); i++) {
        len = strlen(g_file_dir[i]);
        if (strncmp(file_dir, g_file_dir[i], len) == 0) {
            break;
        }
    }
    if (i >= sizeof(g_file_dir)) {
        PRINT("file dir error %s\r\n", file_dir);
        return AT_RET_SYNTAX_ERROR;
    }

    if (file_len == 0) { return AT_RET_SYNTAX_ERROR; }

    if (file_len > FS_TEST_1M_BYTES) { file_len = FS_TEST_1M_BYTES; }
    times = (file_len / BYTES_LEN_4K) ? (file_len / BYTES_LEN_4K) : 1;

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        PRINT("open fail!path:%s\r\n", file_name);
        return AT_RET_SYNTAX_ERROR;
    }

    len = test_libc_read(fp, times);
    ret = fclose(fp);
    if (ret != 0) {
        PRINT("fclose error fp=%d", fp);
    }
    if (len == times * BYTES_LEN_4K) {
        return AT_RET_OK;
    } else {
        return AT_RET_SYNTAX_ERROR;
    }
}


static int32_t rm_dir(const char *pathname)
{
    struct dirent *dirent = NULL;
    struct stat stat_info;
    DIR *d = NULL;
    char *full_path = NULL;
    int ret;

    (void)memset_s(&stat_info, sizeof(stat_info), 0, sizeof(struct stat));
    if (stat(pathname, &stat_info) != 0) {
        PRINT("read file info fail! %s, err=%d\n", pathname, get_errno());
        return -1;
    }

    if (S_ISREG(stat_info.st_mode)) {
        PRINT("rm path: %s \n", pathname);
        return unlink(pathname);
    }
    d = opendir(pathname);
    while (1) {
        dirent = readdir(d);
        if (dirent == NULL) {
            break;
        }
        if (strcmp(dirent->d_name, "..") && strcmp(dirent->d_name, ".")) {
            size_t full_path_buf_Size = strlen(pathname) + strlen(dirent->d_name) + 2; // 2 is sep eof length
            full_path = (char *)malloc(full_path_buf_Size);
            if (full_path == NULL) {
                PRINT("malloc failure!\n");
                (void)closedir(d);
                return -1;
            }
            ret = snprintf_s(full_path, full_path_buf_Size, full_path_buf_Size - 1, "%s/%s", pathname, dirent->d_name);
            if (ret < 0) {
                PRINT("name is too long!\n");
                free(full_path);
                (void)closedir(d);
                return -1;
            }
            (void)rm_dir(full_path);
            free(full_path);
        }
    }
    (void)closedir(d);
    return rmdir(pathname);
}

at_ret_t wear_fs_rm(const fs_write_check_args_t *argc)
{
    char *file_name = (char *)argc->name;
    uint32_t len;
    char file_dir[MAX_FILE_PATH_SIZE] = { 0 };
    uint8_t i;
    char *start = strchr(file_name, '/');
    char *end = strchr(start + 1, '/');
    strncpy_s(file_dir, MAX_FILE_PATH_SIZE, start + 1, end - start - 1);

    for (i = 0; i < sizeof(g_file_dir); i++) {
        len = strlen(g_file_dir[i]);
        if (strncmp(file_dir, g_file_dir[i], len) == 0) {
            break;
        }
    }
    if (i >= sizeof(g_file_dir)) {
        PRINT("file dir error %s\r\n", file_dir);
        return AT_RET_SYNTAX_ERROR;
    }

    PRINT("excute rm path:%s\r\n", file_name);
    if (rm_dir(file_name) >= 0) {
        return AT_RET_OK;
    } else {
        return AT_RET_SYNTAX_ERROR;
    }
}

#if defined (CFG_DRIVERS_NANDFLASH)
at_ret_t wear_fs_format_force(void)
{
    nand_erase_force();
    return AT_RET_OK;
}
#endif

at_ret_t wear_fs_format(void)
{
    int ret = AT_RET_OK;

    /* 解决后台文件打开导致格式化失败的问题，先关闭可能打开的后台文件 */
    for (int i = 0; i < FILE_MAX_NUM; i++) {
        close(i);
    }
    /* 如果离线日志打开，则关闭离线日志 */
    uapi_zdiag_set_offline_log_enable(false);

#ifdef CFG_DRIVERS_MMC
    PRINT("\r\nemmc format,wait a moment!\r\n");
    ret = emmc_drv_format();
    if (ret != ENOERR) {
        ret = AT_RET_SYNTAX_ERROR;
    } else {
        ret = AT_RET_OK;
    }
#endif
#ifdef CFG_DRIVERS_NANDFLASH
    PRINT("\r\nnand format,wait a moment!\r\n");
    nandflash_format();
#endif
#ifdef CFG_DRIVERS_SPINOR
    PRINT("\r\nnorflash(littlefs) format,wait a moment!\r\n");
    ret = littlefs_format();
#endif
    return ret;
}

at_ret_t wear_fs_rw_test(void)
{
    int ret;
    char path[] = "/user/user_test.log";
    FILE *fp = NULL;

    PRINT("wear_fs_rw_test begin\r\n");
    fp = fopen(path, "w+");
    if (fp == NULL) {
        PRINT("open fail\r\n");
        return AT_RET_CMD_PARA_ERROR;
    }
    ret = test_libc_rw(fp);
    ret = fclose(fp);
    if (ret != 0) {
        PRINT("fclose error fp=%d, ret=%d", fp, ret);
    }
    ret = remove(path);
    if (ret != 0) {
        PRINT("remove error path=%s, ret=%d", path, ret);
    }
    return ret;
}

at_ret_t wear_fs_info(void)
{
    struct statfs file_buf = {0};
    for (unsigned int i = 0; i < FS_VOLUMES_NUM; i++) {
        /* 获取文件系统状态 */
        if (statfs(g_volume_names[i], &file_buf) != 0) {
            return AT_RET_CHANNEL_PARA_ERROR;
        }
        if (file_buf.f_bfree >= FS_NODE_INFO_SPACE) {
            file_buf.f_bfree = file_buf.f_bfree - FS_NODE_INFO_SPACE;
        } else {
            file_buf.f_bfree = 0;
        }
        at_adapter_print("%s:\r\n", g_volume_names[i]);
        at_adapter_print("total size=%uKB used size=%uKB free size=%uKB\r\n",
            ((uint32_t)(file_buf.f_blocks * file_buf.f_bsize) / KB_SIZE),
            ((uint32_t)((file_buf.f_blocks - file_buf.f_bfree) * file_buf.f_bsize) / KB_SIZE),
            ((uint32_t)(file_buf.f_bfree * file_buf.f_bsize) / KB_SIZE));
    }
    return AT_RET_OK;
}

uint32_t fs_test_rw_speed(const char* path, uint16_t width)
{
    uint64_t start_time, end_time;
    uint32_t total_time;
    char* buf = osal_kmalloc(width, OSAL_GFP_KERNEL);
    if (buf == NULL) {
        return ERRCODE_FAIL;
    }

    int fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fd < 0) {
        at_adapter_print("fs test open first fail!\r\n");
        osal_kfree(buf);
        return ERRCODE_FAIL;
    }
    uint32_t times = FS_TEST_1M_BYTES / width;
    uint32_t irq_sts = osal_irq_lock();
    start_time = uapi_tcxo_get_us();
    for (uint32_t i = 0; i < times; i++) {
        (void)write(fd, buf, width);
    }
    end_time = uapi_tcxo_get_us();
    osal_irq_restore(irq_sts);
    total_time = (uint32_t)(end_time - start_time);
    uint32_t bw = (ONE_SECOND * BYTES_LEN_1K / total_time);
    at_adapter_print("[fs test]fs write width:%d time=%dus bw:%d KB/s\r\n", width, total_time, bw);
    (void)close(fd);

    fd = open(path, O_RDWR, 0666);
    if (fd < 0) {
        at_adapter_print("fs test open second fail!\r\n");
        osal_kfree(buf);
        return ERRCODE_FAIL;
    }
    irq_sts = osal_irq_lock();
    start_time = uapi_tcxo_get_us();
    for (uint32_t i = 0; i < times; i++) {
        (void)read(fd, buf, width);
    }
    end_time = uapi_tcxo_get_us();
    osal_irq_restore(irq_sts);
    total_time = end_time - start_time;
    bw = (ONE_SECOND * BYTES_LEN_1K / total_time);

    at_adapter_print("[fs test]fs read width:%d time=%dus bw:%d KB/s\r\n", width, total_time, bw);
    (void)close(fd);
    osal_kfree(buf);

    return ERRCODE_SUCC;
}

#if defined (CFG_DRIVERS_NANDFLASH)
boot_nand_file_desc g_nand_file[] = {
    {"/user/rootkey_partition_cfg.bin", 0 },
    {"/user/ssb_signed.bin",            0x1000 },
    {"/user/ssb_backup_signed.bin",     0x11000 },
};

at_ret_t wear_fs_boot_nand_write(void)
{
    FILE *file = NULL;
    int32_t ret;
    size_t bytes_read;
    int32_t start;
    int32_t block_size = g_nand_mtd->erasesize;
    int32_t page_size = g_nand_mtd->writesize;

    char *buffer = (char*)osal_kmalloc(page_size * 2, 0);
    if (buffer == NULL) {
        at_adapter_print("wear_fs_boot_nand_write malloc fail\n");
        return AT_RET_MALLOC_ERROR;
    }
    char *buffer_w = buffer;
    char *buffer_r = buffer + page_size;

    ret = fmc_nand_erase(0, block_size);
    at_adapter_print("erase length : 0x%x ret = %u\n", block_size, ret);

    for (uint32_t i = 0; i < array_size(g_nand_file); ++i) {
        file = fopen(g_nand_file[i].name, "rb");
        if (file == NULL) {
            at_adapter_print("open file %s fail\n", g_nand_file[i].name);
            return AT_RET_SYNTAX_ERROR;
        }
        start = g_nand_file[i].start;
        memset_s(buffer_w, page_size, 0xff, page_size);
        memset_s(buffer_r, page_size, 0xff, page_size);
        while ((bytes_read = fread(buffer_w, 1, page_size, file)) > 0) {
            (void)fmc_nand_write((void *)buffer_w, start, page_size);
            (void)fmc_nand_read((void *)buffer_r, start, page_size);
            ret = (uint32_t)memcmp(buffer_w, buffer_r, bytes_read);
            if (ret == 0) {
                at_adapter_print("write file %s start = 0x%x, bytes = 0x%x ok!\n",
                                 g_nand_file[i].name, start, bytes_read);
            } else {
                at_adapter_print("write file %s start = 0x%x, bytes = 0x%x fail!\n",
                                 g_nand_file[i].name, start, bytes_read);
                break;
            }
            start += bytes_read;
            memset_s(buffer_w, page_size, 0xff, page_size);
            memset_s(buffer_r, page_size, 0xff, page_size);
        }
        fclose(file);
    }
    osal_kfree(buffer);
    return AT_RET_OK;
}
#endif

#define HASH_LEN                    32
#define MSID_LEN                    4

static uint8_t g_oem_root_pubk[0x4][HASH_LEN] = {
    {
        0x8B, 0x27, 0x05, 0x08, 0x70, 0x49, 0x78, 0xAB,
        0x4E, 0x40, 0x90, 0xCA, 0xA9, 0x4F, 0xAA, 0x32,
        0x78, 0xE9, 0x9B, 0x36, 0x6F, 0x7A, 0xFF, 0xAE,
        0xFB, 0xC1, 0x2F, 0xBB, 0x2D, 0xC8, 0x68, 0xA7
    },
    {
        0x2C, 0x1D, 0x17, 0xE0, 0xA8, 0xA6, 0xD5, 0xAD,
        0xF2, 0x5C, 0x73, 0x23, 0xAA, 0xA6, 0x7B, 0x89,
        0x0D, 0x45, 0xBE, 0x76, 0x47, 0xED, 0x85, 0x30,
        0xC5, 0xF0, 0x1B, 0xC5, 0x1E, 0x87, 0x39, 0xB3
    },
    {
        0x62, 0xCC, 0xCC, 0x9D, 0x71, 0x6F, 0x09, 0xA4,
        0x85, 0x48, 0x17, 0x9F, 0x4C, 0x7B, 0xBA, 0x58,
        0x1F, 0x17, 0xBB, 0x9D, 0x82, 0xE4, 0x56, 0xA7,
        0x9E, 0x0C, 0x77, 0x6B, 0xA5, 0x48, 0x0B, 0x00
    },
    {
        0x5C, 0x06, 0x65, 0x62, 0xFF, 0x27, 0xE9, 0xE9,
        0x7C, 0xFC, 0x4C, 0x28, 0x7D, 0xE8, 0xB2, 0x75,
        0xC9, 0xFB, 0xB5, 0x2A, 0xF7, 0xEB, 0xEE, 0x96,
        0x1B, 0x32, 0xFB, 0x4D, 0xCB, 0xA8, 0xDE, 0xFA,
    },
};

#define PUB_KEY_HASH                (1728 / 8)
#define PUB_KEY_LOCK                (1644 / 8)
#define MSID                        (1664 / 8)
#define SM_EN                       (1642 / 8)
#define MSID_LOCK                   (1643 / 8)
#define SFC_DEFAULT_STS_CTL         (304 / 8)
#define LIFE_CYCLE_EFUSE_START_BYTE (1634 / 8)
#define SEC_BOOT_EN_START_BYTE      (715 / 8)
#define EFUSE_NAND_BOOT_DIS_BYTE    (695 / 8)
#define EFUSE_NAND_BOOT_EN_BYTE     (696 / 8)
#define _2K                         (2048)
#define _4K                         (4096)
#define _128K                       (0x20000UL)
#define _256K                       (0x40000UL)

at_ret_t at_set_secure_boot(void)
{
    // hash oem pub key
    uint8_t boot_nand_dis = 0;
    int32_t ret = uapi_efuse_read_bit(&boot_nand_dis, EFUSE_NAND_BOOT_DIS_BYTE, 7);
    if (ret != ERRCODE_SUCC) {
        return AT_RET_PROC_SET_FUNC_MISSING;
    }

    uint8_t boot_nand_en = 0;
    ret = uapi_efuse_read_bit(&boot_nand_en, EFUSE_NAND_BOOT_EN_BYTE, 0);
    if (ret != ERRCODE_SUCC) {
        return AT_RET_PROC_SET_FUNC_MISSING;
    }

    uint8_t *buffer = g_oem_root_pubk[0];
#ifdef CFG_DRIVERS_NANDFLASH
    if ((boot_nand_dis == 0) && (boot_nand_en == 1)) {
        int32_t page_size = g_nand_mtd->writesize;
        int32_t block_size = g_nand_mtd->erasesize;
        if ((page_size == _2K) && (block_size == _128K)) {
            buffer = g_oem_root_pubk[0];
        } else if ((page_size == _2K) && (block_size == _256K)) {
            buffer = g_oem_root_pubk[1];
        } else if ((page_size == _4K) && (block_size == _256K)) {
            buffer = g_oem_root_pubk[2];
        } else {
            buffer = g_oem_root_pubk[3];
        }
    }
#endif
    ret = uapi_efuse_write_buffer(PUB_KEY_HASH, buffer, HASH_LEN);
    if (ret != ERRCODE_SUCC) {
        at_adapter_print("rootkey hash fail 0x%x", ret);
        return AT_RET_PROC_SET_FUNC_MISSING;
    }

    // pub key lock
    ret = uapi_efuse_write_bit(PUB_KEY_LOCK, 0x4);
    if (ret != ERRCODE_SUCC) {
        at_adapter_print("pubkey lock fail 0x%x", ret);
        return AT_RET_PROC_SET_FUNC_MISSING;
    }

    // msid
    uint8_t sec_msid[MSID_LEN] = {0xCD, 0xAB, 0x21, 0x34};
    ret = uapi_efuse_write_buffer(MSID, sec_msid, MSID_LEN);
    if (ret != ERRCODE_SUCC) {
        at_adapter_print("msid fail 0x%x", ret);
        return AT_RET_PROC_SET_FUNC_MISSING;
    }

    // msid lock
    ret = uapi_efuse_write_bit(MSID_LOCK, 0x3);
    if (ret != ERRCODE_SUCC) {
        at_adapter_print("msid lock fail 0x%x", ret);
        return AT_RET_PROC_SET_FUNC_MISSING;
    }

    // secure boot enable
    ret = uapi_efuse_write_bit(SEC_BOOT_EN_START_BYTE, 0x3);
    if (ret != ERRCODE_SUCC) {
        at_adapter_print("secure failed 0x%x", ret);
        return AT_RET_PROC_SET_FUNC_MISSING;
    }

    // bit3 设置为出厂状态  bit4 量产状态  bit5 销毁状态
    ret = uapi_efuse_write_bit(LIFE_CYCLE_EFUSE_START_BYTE, 0x3);
    if (ret != ERRCODE_SUCC) {
        at_adapter_print("lifecycle failed 0x%x", ret);
        return AT_RET_PROC_SET_FUNC_MISSING;
    }

    // fapc
    ret = uapi_efuse_write_bit(SFC_DEFAULT_STS_CTL, 0x0);
    if (ret != ERRCODE_SUCC) {
        at_adapter_print("write fapc failed 0x%x", ret);
        return AT_RET_PROC_SET_FUNC_MISSING;
    }
    return AT_RET_OK;
}

at_ret_t at_get_secure_boot(void)
{
    uint8_t sec_boot_enable_efuse;
    errcode_t ret = uapi_efuse_read_buffer(&sec_boot_enable_efuse, SEC_BOOT_EN_START_BYTE, 1);
    if (ret != ERRCODE_SUCC) {
        return AT_RET_PROC_SET_FUNC_MISSING;
    }
    sec_boot_enable_efuse = (sec_boot_enable_efuse & 0x78) >> 0x3;

    uint8_t life_cycle;
    ret = uapi_efuse_read_buffer(&life_cycle, LIFE_CYCLE_EFUSE_START_BYTE, 1);
    if (ret != ERRCODE_SUCC) {
        return AT_RET_PROC_SET_FUNC_MISSING;
    }
    life_cycle = (life_cycle & 0x3C) >> 0x2;

    bool sec_boot_enable = true;
    if (((sec_boot_enable_efuse == 0) && (life_cycle == 0x0)) || (sec_boot_enable_efuse == 0xA)) {
        sec_boot_enable = false;
    }
    at_adapter_print("sec boot status is %s"NEWLINE, sec_boot_enable ? "enable" : "disable");

    at_adapter_print("life cycle is ");
    if (life_cycle == 0) {
        at_adapter_print("Blank state"NEWLINE);
    } else if (life_cycle & 0x8) {
        at_adapter_print("Decommission state"NEWLINE);
    } else if (life_cycle & 0x4) {
        at_adapter_print("End user state"NEWLINE);
    } else if (life_cycle & 0x2) {
        at_adapter_print("OEM provisioned state"NEWLINE);
    } else {
        at_adapter_print("error state"NEWLINE);
    }

    // pub key lock
    uint8_t pub_key_lock = 0;
    ret = uapi_efuse_read_bit(&pub_key_lock, PUB_KEY_LOCK, 0x4);
    if (ret != ERRCODE_SUCC) {
        return AT_RET_PROC_SET_FUNC_MISSING;
    }
    at_adapter_print("pubkey lock is 0x%x"NEWLINE, pub_key_lock);

    if (pub_key_lock != 0) {
        return AT_RET_OK;
    }

    // root pub key hash
    uint8_t root_pubk[HASH_LEN];
    ret = uapi_efuse_read_buffer(root_pubk, PUB_KEY_HASH, HASH_LEN);
    if (ret != ERRCODE_SUCC) {
        return AT_RET_PROC_SET_FUNC_MISSING;
    }
    at_adapter_print("pubkey hash is:");
    for (uint8_t i = 0; i < HASH_LEN; i++) {
        at_adapter_print("0x%x ", root_pubk[i]);
    }
    at_adapter_print(NEWLINE);
    return AT_RET_OK;
}

#ifdef CFG_DRIVERS_NANDFLASH
#define PAGENUM_PER_BLOCK (64)
at_ret_t wear_fs_fmc_nand_erase_test(const fmcnand_args_t *para)
{
    unused(para);
    int32_t ret;
    at_adapter_print("block: 0x%x ,page= 0x%x, pagenum=0x%x.\r\n", para->block, para->page, para->pagenum);
    int32_t page_size = g_nand_mtd->writesize;
    uint64_t start = para->block * PAGENUM_PER_BLOCK * page_size;
    if (para->pagenum == 0xffffffff) {
        ret = fmc_nand_erase(0, (uint64_t)g_nand_mtd->size);
    } else {
        ret = fmc_nand_erase(start, (uint64_t)(para->pagenum * page_size));
    }

    at_adapter_print("erase length : 0x%x ret = %u\n", g_nand_mtd->size, ret);
    return AT_RET_OK;
}

at_ret_t wear_fs_fmc_nand_writeoob_test(const fmcnand_args_t *para)
{
#define PAGENUM_PER_BLOCK (64)
    at_adapter_print("block: 0x%x ,page= 0x%x, pagenum=0x%x.\r\n", para->block, para->page, para->pagenum);

    int32_t block_size = g_nand_mtd->erasesize;
    int32_t page_size = g_nand_mtd->writesize;
    int32_t oob_size = g_nand_mtd->oobsize;

    char *nand_oob_w = (char *)osal_kmalloc(page_size + oob_size, 0);
    if (nand_oob_w == NULL) {
        at_adapter_print("wear_fs_fmc_nand_writeoob_test malloc fail\n");
        return AT_RET_MALLOC_ERROR;
    }
    for (int i = 0; i < page_size + oob_size; i++) {
        if (i == page_size || i == (page_size + 1)) {
            nand_oob_w[i] = 0xff;
        } else {
            nand_oob_w[i] = (uint8_t)(i + 1);
        }
    }
    int64_t start = para->block * PAGENUM_PER_BLOCK * page_size + para->page * page_size;
    size_t len = page_size + oob_size;
    size_t outlen = len;
    for (uint32_t i = 0; i < para->pagenum; i++) {
        g_nand_mtd->write_oob(g_nand_mtd, start, len, &outlen, (char *)nand_oob_w);
        at_adapter_print("outlen:%u start:%u.\r\n", outlen, start);
        start += page_size;
    }
    osal_kfree(nand_oob_w);
    return AT_RET_OK;
}

#define TEST_NAND_PRINT_MAX (512)
#define TEST_NAND_PRINT_COLUMN (4)
#define COLUMN_1 (1)
#define COLUMN_2 (2)
#define COLUMN_3 (3)
at_ret_t wear_fs_fmc_nand_readoob_test(const fmcnand_args_t *para)
{
    at_adapter_print("block: 0x%x ,page= 0x%x, pagenum=0x%x.\r\n", para->block, para->page, para->pagenum);
    int32_t block_size = g_nand_mtd->erasesize;
    int32_t page_size = g_nand_mtd->writesize;
    int32_t oob_size = g_nand_mtd->oobsize;
    uint32_t len = page_size + oob_size;
    uint32_t outlen = len;

    char *nand_oob_w = (char *)osal_kmalloc(page_size + oob_size, 0);
    char *nand_oob_r = (char *)osal_kmalloc(page_size + oob_size, 0);
    if (nand_oob_r == NULL) {
        if (nand_oob_w == NULL) {
            osal_kfree(nand_oob_w);
        }
        return AT_RET_MALLOC_ERROR;
    }

    for (int i = 0; i < page_size + oob_size; i++) {
        if (i == page_size || i == (page_size + 1)) {
            nand_oob_w[i] = 0xff;
        } else {
            nand_oob_w[i] = (uint8_t)(i + 1);
        }
    }

    memset_s(nand_oob_r, page_size + oob_size, 0xff, page_size + oob_size);
    int64_t start = (int64_t)(para->block * PAGENUM_PER_BLOCK * page_size + para->page * page_size);
    for (int j = 0; j < (int)para->pagenum; j++) {
        g_nand_mtd->read_oob(g_nand_mtd, start, len, &outlen, (char *)nand_oob_r);
        int32_t ret = (uint32_t)memcmp(nand_oob_w, nand_oob_r, page_size + oob_size);
        if (ret == 0) {
            at_adapter_print(" compare ok, start = 0x%x, bytes = 0x%x!\n", start, outlen);
        } else {
            at_adapter_print(" compare fail, start = 0x%x, outlen = 0x%x ok!\n", start, outlen);
            for (int32_t i = 0; i < TEST_NAND_PRINT_MAX; i = i + TEST_NAND_PRINT_COLUMN) {
                at_adapter_print("write: %x-%x-%x-%x.\r\n", nand_oob_w[i], nand_oob_w[i + COLUMN_1],
                    nand_oob_w[i + COLUMN_2], nand_oob_w[i + COLUMN_3]);
                at_adapter_print("read: %x-%x-%x-%x.\r\n", nand_oob_r[i], nand_oob_r[i + COLUMN_1],
                    nand_oob_r[i + COLUMN_2], nand_oob_r[i + COLUMN_3]);
            }
            for (int32_t i = page_size; i < page_size + oob_size; i = i + TEST_NAND_PRINT_COLUMN) {
                at_adapter_print("write: %x-%x-%x-%x.\r\n", nand_oob_w[i], nand_oob_w[i + COLUMN_1],
                    nand_oob_w[i + COLUMN_2], nand_oob_w[i + COLUMN_3]);
                at_adapter_print("read: %x-%x-%x-%x.\r\n", nand_oob_r[i], nand_oob_r[i + COLUMN_1],
                    nand_oob_r[i + COLUMN_2], nand_oob_r[i + COLUMN_3]);
            }
        }
        start += page_size;
    }
    osal_kfree(nand_oob_w);
    osal_kfree(nand_oob_r);
    return AT_RET_OK;
}
#endif

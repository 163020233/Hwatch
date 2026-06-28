/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: IPC file system common. \n
 *
 * History: \n
 * 2024-08-25, Create file. \n
 */
#ifndef IPC_FS_COMMON_H
#define IPC_FS_COMMON_H

#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define FILE_NAME_MAX  256

#define IPC_FS_RW_MODE_IPC 0
#define IPC_FS_RW_MODE_CPU 1
#define IPC_FS_RW_MODE_DMA 2

typedef enum {
    IPC_FS_OP_STAT,
    IPC_FS_OP_FCREATE,
    IPC_FS_OP_FOPEN,
    IPC_FS_OP_FCLOSE,
    IPC_FS_OP_FWRITE,
    IPC_FS_OP_FREAD,
    IPC_FS_OP_FSEEK,
    IPC_FS_OP_FTELL,
    IPC_FS_OP_FEOF,
    IPC_FS_OP_FERROR,
    /* DIR */
    IPC_FS_OP_OPENDIR,
    IPC_FS_OP_CLOSEDIR,
    IPC_FS_OP_READDIR,
    IPC_FS_OP_MAX
} ipc_fs_op;

typedef struct {
    uint8_t d_type;
    char d_name[FILE_NAME_MAX];
} ipc_fs_dirent;

/* Requests */
typedef struct {
    uint32_t path_len;
    char path[];
} ipc_fs_stat_req, ipc_fs_opendir_req;


typedef struct {
    uint32_t path_len;
    uint32_t size;
    char path[];
} ipc_fs_fcreate_req;

typedef struct {
    uint32_t path_len;
    uint32_t mode_len;
    char str[];
} ipc_fs_fopen_req;

typedef struct {
    uint64_t filp;
} ipc_fs_fclose_req, ipc_fs_ftell_req, ipc_fs_feof_req, ipc_fs_ferror_req;

typedef struct {
    uint32_t mode;
    uint32_t size;
    uint32_t nmemb;
    uint64_t filp;
    uint64_t addr;
    uint8_t data[];
} ipc_fs_fwrite_req;

typedef struct {
    uint32_t mode;
    uint32_t size;
    uint32_t nmemb;
    uint64_t filp;
    uint64_t addr;
} ipc_fs_fread_req;

typedef struct {
    uint64_t filp;
    long int offset;
    int whence;
} ipc_fs_fseek_req;

typedef struct {
    uint64_t dirp;
} ipc_fs_closedir_req;

typedef struct {
    uint64_t dirp;
    uint64_t dirent_addr;
} ipc_fs_readdir_req;

/* Acks */
typedef struct {
    int32_t ret;
    struct stat buff;
} ipc_fs_stat_ack;

typedef struct {
    uint64_t filp;
} ipc_fs_fopen_ack;

typedef struct {
    uint64_t dirp;
} ipc_fs_opendir_ack;

typedef struct {
    int32_t ret;
} ipc_fs_fclose_ack, ipc_fs_fseek_ack, ipc_fs_feof_ack, ipc_fs_ferror_ack, ipc_fs_closedir_ack, ipc_fs_fcreate_ack;

typedef struct {
    long int offset;
} ipc_fs_ftell_ack;

typedef struct {
    uint32_t size;
} ipc_fs_fwrite_ack;

typedef struct {
    uint32_t size;
    uint8_t data[];
} ipc_fs_fread_ack;

typedef struct {
    int32_t is_null;
    ipc_fs_dirent dirent;
} ipc_fs_readdir_ack;

/* base */
#pragma pack(1)
typedef struct {
    uint8_t src_core;
    uint8_t op_type;
    uint16_t length;
    union {
        ipc_fs_stat_req stat_req;
        ipc_fs_fcreate_req fcreate_req;
        ipc_fs_fopen_req fopen_req;
        ipc_fs_fclose_req fclose_req;
        ipc_fs_fwrite_req fwrite_req;
        ipc_fs_fread_req fread_req;
        ipc_fs_fseek_req fseek_req;
        ipc_fs_ftell_req ftell_req;
        ipc_fs_feof_req feof_req;
        ipc_fs_ferror_req ferror_req;
        ipc_fs_opendir_req opendir_req;
        ipc_fs_closedir_req closedir_req;
        ipc_fs_readdir_req readdir_req;
    } req;
} ipc_fs_req;

typedef struct {
    uint8_t src_core;
    uint8_t op_type;
    uint16_t length;
    union {
        ipc_fs_stat_ack stat_ack;
        ipc_fs_fcreate_ack fcreate_ack;
        ipc_fs_fopen_ack fopen_ack;
        ipc_fs_fclose_ack fclose_ack;
        ipc_fs_fwrite_ack fwrite_ack;
        ipc_fs_fseek_ack fseek_ack;
        ipc_fs_ftell_ack ftell_ack;
        ipc_fs_feof_ack feof_ack;
        ipc_fs_ferror_ack ferror_ack;
        ipc_fs_opendir_ack opendir_ack;
        ipc_fs_closedir_ack closedir_ack;
        ipc_fs_readdir_ack readdir_ack;
        ipc_fs_fread_ack fread_ack;
    } ack;
} ipc_fs_ack;
#pragma pack()

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description:  Application Core OS Initialize Interface Header for Standard
 */

#ifndef FILE_IF_H
#define FILE_IF_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MAX_FILE_NAME_LEN           178
#define MAX_DIR_NAME_LEN            88

typedef enum {
    SEEK_FILE_BEGIN = 0,
    SEEK_CUR_POS,
    SEEK_FILE_END,
    SEEK_TYPE_BUTT
} seek_type;

typedef struct {
    char name[MAX_FILE_NAME_LEN];
} file_name_t;

typedef struct {
    uint32_t file_idx;
} file_t;

int32_t file_sys_init(uint32_t file_sys_id, uint32_t file_sys_addr, uint32_t file_sys_len);
int32_t file_sys_create(uint32_t file_sys_id, uint32_t file_sys_addr, uint32_t file_sys_len);
void file_sys_deinit(void);

/* 功能说明: 创建文件，为其开辟flash空间
 * 参数: 1) filename: 入参，带完整路径的文件名
 *       2) size: 文件最大占用空间
 * 返回值: 0, succ; other, fail
*/
int32_t file_create(char *filename, size_t size);

/* 功能说明: 打开文件
 * 参数: 1) filename: 入参，带完整路径的文件名
 *       2) mode: 入参，'r':只读， 'w':可写且OFFSET指向文件开始， 'a':可写且OFFSET指向文件尾
 * 返回值: 文件句柄, 失败返回NULL
*/
file_t *lib_file_open(const char *filename, const char *mode);

/* 功能说明: 关闭文件
 * 参数: 1) file: 入参，文件句柄
 * 返回值: 0, succ; other, fail
*/
int32_t lib_file_close(file_t *file);

/* 功能说明: 从文件offset处读取size*nmemb字节数据至dst中
 * 参数: 1) dst: 入参，读取到的目标地址，最小尺寸 size*nmemb
 *       2) size: 入参，读取的每个元素的大小，以字节为单位
 *       3) nmemb: 入参，元素的个数，每个元素的大小为 size 字节
 *       4) file: 入参，文件句柄
 * 返回值: 读取元素的个数，等于nmemb表示成功，否则表示失败
*/
size_t lib_file_read(void *dst, size_t size, size_t nmemb, file_t *file);

/* 功能说明: 将src指向的数据写入文件offset指向的位置
 * 参数: 1) src: 入参，指向被写入区域
 *       2) size: 入参，被写入的每个元素的大小，以字节为单位
 *       3) nmemb: 入参，元素的个数，每个元素的大小为 size 字节
 *       4) file: 入参，文件句柄
 * 返回值: 写入元素的个数，等于nmemb表示成功，否则表示失败
*/
size_t lib_file_write(const void *src, size_t size, size_t nmemb, file_t *file);

/* 功能说明: 设置文件offse至指定位置
 * 参数: 1) file: 入参，文件句柄
 *       2) offset: 入参，相对 whence 的偏移量
 *       3) whence : 入参，开始添加偏移 offset 的位置, 见seek_type
 * 返回值: 0, succ; other, fail
*/
int32_t lib_file_seek(file_t *file, long offset, int32_t whence);

/* 功能说明: 获取位置标识符的当前值
 * 参数: 1) file: 入参，文件句柄
 * 返回值: 位置标识符的当前值。如果发生错误，则返回 -1
*/
long int file_tell(file_t *file);

/* 功能说明: 测试该文件的结束标识符
 * 参数: 1) file: 入参，文件句柄
 * 返回值: 0, 非end of file; 1, end of file; 如果发生错误，则返回 -1
*/
int file_eof(file_t *file);

/* 功能说明: 测试该文件的操作错误标识符
 * 参数: 1) file: 入参，文件句柄
 * 返回值: 0, 无异常; 非0, 发生错误
*/
int file_error(file_t *file);

int32_t file_dir_get_file_num(char *dir_name, uint32_t *file_num);
int32_t file_dir_get_list(char *dir_name, file_name_t *file_list, uint32_t file_num);
int32_t show_file_list(void);
int32_t show_file_content(char *file_name);
uint32_t get_file_size_by_name(char *file_name);
int32_t file_dir_get_file_name_by_index(char *dir_name, file_name_t *file_name, uint32_t file_index);

static inline file_t *file_open(const char *filename, const char *mode)
{
    return lib_file_open(filename, mode);
}

static inline int32_t file_close(file_t *file)
{
    return lib_file_close(file);
}

static inline size_t file_read(void *dst, size_t size, size_t nmemb, file_t *file)
{
    return lib_file_read(dst, size, nmemb, file);
}

static inline size_t file_write(const void *src, size_t size, size_t nmemb, file_t *file)
{
    return lib_file_write(src, size, nmemb, file);
}

static inline int32_t file_seek(file_t *file, long offset, int32_t whence)
{
    return lib_file_seek(file, offset, whence);
}
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 */


#ifndef UTILS_FILE_H_
#define UTILS_FILE_H_

#include <stdio.h>
#include <stdint.h>

#ifdef NPU_FILE_SYS
#include "file_if.h"
#define utils_fopen file_open
#define utils_fread file_read
#define utils_fwrite file_write
#define utils_fclose file_close
#define utils_feof file_eof
#define utils_fseek file_seek
typedef file_t utils_file;
#else
#define utils_fopen fopen
#define utils_fread fread
#define utils_fwrite fwrite
#define utils_fclose fclose
#define utils_feof feof
#define utils_fseek fseek
#define utils_fgets fgets
typedef FILE utils_file;
#endif

#endif /* UTILS_FILE_H_ */
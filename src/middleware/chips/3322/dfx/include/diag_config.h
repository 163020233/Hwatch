/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: diag usr config
 * This file should be changed only infrequently and with great care.
 */

#ifndef DIAG_CONFIG_H
#define DIAG_CONFIG_H

#include <stdint.h>
#ifdef _PRE_FEATURE_WS53_DEVICE_MODE
#include "diag_config_ws53.h"
#endif

#define DFX_MSG_MAX_SIZE                24 /* msg element size of msg queue */
#define DFX_MSG_ID_LEN                  sizeof(uint32_t)
#define DFX_QUEUE_MAX_SIZE              512

#define CONFIG_DIAG_CMD_TBL_NUM         10      /* diag: Maximum number of cmd tbl cnt */
#define CONFIG_DIAG_IND_TBL_NUM         3       /* diag: Maximum number of ind tbl cnt */
#define CONFIG_STAT_CMD_LIST_NUM        10      /* diag: Maximum number of stat tbl cnt */
#define CONFIG_DIAG_RX_BUF_SIZE         0x400   /* diag: rx buf size */
#define CONFIG_DIAG_TRANSMIT_ITEM_CNT   3       /* diag: Maximum transfer item cnt */
#define CONFIG_DIAG_FILTER_GROUP_NUM    16      /* diag: Maximum number of diag file group */

#define LOG_LIMIT_RESET_TIMEOUT         (30 * 60 * 1000)  /* 日志限流重置时间： 30 minute */

#ifndef _PRE_FEATURE_WS53_DEVICE_MODE
#ifdef CFG_NORFLASH_ONLY
#define DFX_OFFLINE_LOG_MUTI_FILE_NUM   1                       /* 离线日志最大文件个数 */
/* 离线日志每个文件最大字节数 */
#if (CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM == DFX_YES)
#define DFX_OFFLINE_LOG_FILE_SIZE       (768 * 1024)
#else
#define DFX_OFFLINE_LOG_FILE_SIZE       diag_adapt_flash_get_region_size(FLASH_OP_TYPE_LOG_FILE)
#endif
#define DFX_OFFLINE_LOG_PATH            "/user/log/"            /* 离线日志存放路径 */
#define DFX_OFFLINE_LOG_NAME            "log_file_diag.bin"     /* 离线日志文件名 */
#define DFX_OFFLINE_LOG_CACHE_SIZE      1024                    /* 离线日志缓存大小 */
#if (CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM == DFX_YES)
#define DFX_OFFLINE_LOG_CACHE_ENABLE    0                       /* 离线日志缓存开关 */
#else
#define DFX_OFFLINE_LOG_CACHE_ENABLE    1                       /* 离线日志缓存开关 非文件系统必须打开 */
#endif
#else /* CFG_NORFLASH_ONLY */
#define DFX_OFFLINE_LOG_MUTI_FILE_NUM   10                      /* 离线日志最大文件个数 */
#define DFX_OFFLINE_LOG_FILE_SIZE       (1024 * 1024)           /* 离线日志每个文件最大字节数 */
#define DFX_OFFLINE_LOG_PATH            "/user/log/"            /* 离线日志存放路径 */
#define DFX_OFFLINE_LOG_NAME            "log_file_diag.bin"     /* 离线日志文件名 */
#define DFX_OFFLINE_LOG_CACHE_SIZE      1024                    /* 离线日志缓存大小 */
#define DFX_OFFLINE_LOG_CACHE_ENABLE    0                       /* 离线日志缓存开关 */
#endif /* CFG_NORFLASH_ONLY */
#endif /* _PRE_FEATURE_WS53_DEVICE_MODE */

#ifdef CFG_NORFLASH_ONLY
#define MASSDATA_MUTI_FILE_NUM          1
#define MASSDATA_FILE_SIZE              0x8000
#define MASSDATA_FILE_PATH              ""
#define MASSDATA_FILE_NAME              ""
#define MASSDATA_CACHE_SIZE             1024
#define MASSDATA_CACHE_ENABLE           1
#define MASSDATA_RECORD_MAX_SIZE        160
#else /* CFG_NORFLASH_ONLY */
#define MASSDATA_MUTI_FILE_NUM          3
#define MASSDATA_FILE_SIZE              0x8000
#define MASSDATA_FILE_PATH              "/user/log/"
#define MASSDATA_FILE_NAME              "event.bin"
#define MASSDATA_CACHE_SIZE             0
#define MASSDATA_CACHE_ENABLE           0
#define MASSDATA_RECORD_MAX_SIZE        160
#endif /* CFG_NORFLASH_ONLY */

#endif /* DIAG_CONFIG_H */

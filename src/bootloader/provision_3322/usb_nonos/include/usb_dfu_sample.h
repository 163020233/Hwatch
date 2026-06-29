/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2018. All rights reserved.
 * Description: dfu test cases
 * Author: zhaojingjing
 * Create: 2018-3-11
 */
#ifndef IT_PERFORMANCE_DFU_H
#define IT_PERFORMANCE_DFU_H
#include "stdint.h"
#include "gadget/f_dfu.h"
#include "kal_atomic.h"

#ifndef Atomic
#define Atomic KalAtomic
#endif

#ifndef VOID
#define VOID void
#endif

#ifndef UINT32
#define UINT32 unsigned int
#endif

#ifndef LOS_OK
#define LOS_OK 0
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef DEBUG_PRINT
#define DEBUG_PRINT(fmt, ...)
#endif

#ifdef LOSCFG_PLATFORM_HI3892WV100ES_APP
#define DFU_MEM_ADDR_START     FLASH_BASE_ADDR + 0x3000000 // DFU缓存的起始地址，根据实际内存大小调整为适当的值
#define DFU_MEM_SIZE           0xC800 // DFU缓存空间大小
#define GET_IMAGE_SIZE         0x4000 // 每次获取的APP数据块大小
#define IMAGE_UPDATE_FLASHADDR 0x180000
#elif LOSCFG_PLATFORM_HI1162_APP || LOSCFG_PLATFORM_BS21
#define DFU_MEM_ADDR_START     (UINTPTR)g_dfuMusicBuf
#define GET_IMAGE_SIZE         0x100
#define DFU_MEM_SIZE           DFU_MUSIC_SIZE
#define IMAGE_UPDATE_FLASHADDR (UINTPTR)g_dfuMusicBuf
#else
#define DFU_MEM_ADDR_START     SYS_MEM_BASE + 0x3000000
#define DFU_MEM_SIZE           0x1E6800
#define GET_IMAGE_SIZE         0x40000
#define IMAGE_UPDATE_FLASHADDR 0x180000
#endif

#define BUF_LEN               128
#ifdef LOSCFG_DRIVERS_USB_DEVICE_CUSTOM_CTRLREQ_BUF_SIZE
#define REQ_BUF_MAX_LEN         LOSCFG_DRIVERS_USB_DEVICE_CTRLREQ_BUF_SIZE
#else
#define REQ_BUF_MAX_LEN         4096
#endif

/* *初始化配置DFU使用参数* */
#define ENV_INTERFACE    "ram"
#define ENV_INIT_NAME    "dfu_load ram"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef LOSCFG_DRIVERS_MTD_SPI_NOR
extern struct mtd_info *g_mtd;
extern int spinor_init(void);
#endif
#ifdef LOSCFG_DRIVERS_USB_DFU_DOWNLOAD_CALLBACK
extern Atomic g_dfuTransFlag;
extern Atomic g_TestsucCount;
#endif
extern VOID ItUsbDfu001(VOID);
extern VOID ItUsbDfu002(VOID);
extern VOID ItUsbDfu003(VOID);
extern VOID ItUsbDfu004(VOID);
#ifdef LOSCFG_DRIVERS_USB_DFU_DOWNLOAD_CALLBACK
extern VOID ItUsbDfu005(VOID);
extern VOID ItUsbDfu006(VOID);
extern VOID ItUsbDfu007(VOID);
extern VOID ItUsbDfu008(VOID);
extern VOID ItUsbDfu009(VOID);
extern UINT32 InitDfuMemory(UINT32 reqBuf);
extern VOID CleanupDfu(VOID);
extern VOID UsbDfuUpdate(VOID);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif
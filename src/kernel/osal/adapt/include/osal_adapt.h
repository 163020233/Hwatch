/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: osal adapt header \n
 * Author: @CompanyNameTag \n
 */

#ifndef OSAL_ADAPT_H
#define OSAL_ADAPT_H
#include <stdbool.h>
#include "osal_task.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef void (*osal_idle_handler_hook)(void);

typedef struct {
    void *pool_status;
    unsigned int uwTotalUsedSize;
    unsigned int uwTotalFreeSize;
    unsigned int uwMaxFreeNodeSize;
    unsigned int uwUsedNodeNum;
    unsigned int uwFreeNodeNum;
#ifdef LOSCFG_MEM_TASK_STAT
    unsigned int uwUsageWaterLine;  /**< this structure member is defined only when LOSCFG_MEM_TASK_STAT is defined. */
#endif
} osal_mem_pool_status;

typedef struct {
    unsigned short status;   // Task status.
    unsigned int usage;      // CPU usage (0 to 1000)
} osal_cpup_info_t;

#define OSAL_TASK_NAMELEN   32
typedef struct {
    char                acName[OSAL_TASK_NAMELEN];   /**< Task name, the default value of
	                                                     #OSAL_TASK_NAMELEN  = LOS_TASK_NAMELEN  */
    unsigned int        uwTaskID;                   /**< Task ID                  */
    unsigned short      usTaskStatus;               /**< Task status              */
    unsigned short      usTaskPrio;                 /**< Task priority            */
    void                *pTaskSem;                  /**< Semaphore pointer        */
    void                *pTaskMux;                  /**< Mutex pointer            */
    unsigned int        uwEventMask;                /**< Event mask               */
    unsigned int        uwStackSize;                /**< Task stack size          */
    void                *uwTopOfStack;               /**< Task stack top           */
    void                *uwBottomOfStack;            /**< Task stack bottom        */
    void                *uwSP;                       /**< Task SP pointer          */
    unsigned int        uwCurrUsed;                 /**< Current task stack usage */
    unsigned int        uwPeakUsed;                 /**< Task stack usage peak    */
    bool                bOvf;                       /**< Flag that indicates whether a task stack overflow
	                                                     occurs or not            */
} osal_tsk_info_s;

typedef enum {
    OSAL_CPUP_LAST_MULIT_RECORD = 0, /**< Display CPU usage in the last multi sample record, the value is 0. */
    OSAL_CPUP_LAST_ONE_RECORD = 1,   /**< Display CPU usage in the last one sample record, the value is 1. */
    OSAL_CPUP_ALL_TIME = 0xffff      /**< Display CPU usage from system startup to now, the value is 0xffff. */
} osal_cpup_mode;

typedef enum {
    OSAL_RBUF_UNINIT,          /**< Ringbuf is not inited. */
    OSAL_RBUF_INITED           /**< Ringbuf is inited. */
} osal_ringbuf_status;

typedef enum {
    OSAL_RBUF_NORMAL,          /**< Ringbuf is normal. */
    OSAL_RBUF_OVERWRITE        /**< Ringbuf can be overwritten. */
} osal_ringbuf_flag;

typedef struct {
    unsigned int        startIdx;   /**< Ringbuf read index */
    unsigned int        endIdx;     /**< Ringbuf write index */
    unsigned int        size;       /**< Ringbuf total size */
    unsigned int        remain;     /**< Ringbuf free size */
    void                *lock;      /**< Lock for read and write */
    osal_ringbuf_status status;     /**< Ringbuf status */
    osal_ringbuf_flag   flag;       /**< Ringbuf flag */
    char                *fifo;                  /**< Buf to store data */
} osal_ringbuf;

typedef void (*print_cb_t)(const char *fmt, ...);
typedef void (*osal_task_switch_hook)(unsigned int oldtaskId, unsigned int newtaskId);
typedef void (*osal_hwi_hook)(unsigned int hwi_num);
typedef void (*osal_fsmempoolhook)(unsigned int *pool, unsigned int *size);

extern void test_suite_uart_send(const char *str);
void osal_dcache_flush_all(void);
void osal_register_print(print_cb_t print);
unsigned int osal_allcpu_usage(unsigned short maxNum, osal_cpup_info_t *cpupInfo,
                               unsigned int mode, unsigned short flag);
void osal_cpup_reset(void);
void osal_cpup_stop(void);
void osal_cpup_start(void);
unsigned int osal_history_task_cpup_usage(osal_task *task, unsigned int mode);
unsigned int osal_cpup_history_cpu_usage(unsigned int mode);
void osal_get_cpu_cycle(unsigned int *highCnt, unsigned int *lowCnt);
void osal_trace_event_mask_set(unsigned int mask);
unsigned int osal_trace_start(void);
void osal_trace_stop(void);

unsigned int osal_mem_node_size_check(void *pool, void *ptr, unsigned int *totalSize, unsigned int *availSize);
unsigned int osal_mem_info_get(void *pool, osal_mem_pool_status *poolStatus);
unsigned int osal_mem_pool_size_get(const void *pool);
void osal_idle_handler_hook_reg(osal_idle_handler_hook hook);

void osal_task_switch_hook_reg(osal_task_switch_hook swihook);
void osal_kthread_backtrace(unsigned long taskId);
#ifdef LOSCFG_HWI_PRE_POST_PROCESS
void osal_hwi_posthook_reg(osal_hwi_hook intposthook);
void osal_hwi_prehook_reg(osal_hwi_hook intprehook);
#endif


void osal_reset_caller_ra(void);
void osal_save_caller_ra(void *caller_ra);

bool osal_dyn_mem_pool_set(void *mem_pool);
void *osal_find_sym_by_name(void *handle, char *name);
int osal_module_unload(void *handle);
void *osal_so_load(const char *elf_filename);
int osal_bcache_sync_by_name(const char *name);

unsigned int osal_ms2tick(unsigned int ms);
unsigned int osal_taskinfo_get(unsigned int taskId, osal_tsk_info_s *taskInfo);
unsigned int osal_ringbuf_init(osal_ringbuf *ringbuf, char *fifo, unsigned int size, osal_ringbuf_flag flag);
unsigned int osal_ringbuf_write(osal_ringbuf *ringbuf, const char *buf, unsigned int size);
unsigned int osal_ringbuf_read(osal_ringbuf *ringbuf, char *buf, unsigned int size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

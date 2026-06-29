/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: CMSIS HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2024-03-20
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#ifndef _CMSIS_OS1_H
#define _CMSIS_OS1_H

#include <stdint.h>
#include <stddef.h>

#ifdef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
#include "arch/task.h"
#include "los_membox.h"
#include "los_queue.h"
#endif

#ifdef LOSCFG_COMPAT_CMSIS_VER_2
#include "cmsis_os2.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
/*
 * The external definition of user exposure is the same as that of open source.
 * ref:https://arm-software.github.io/CMSIS_5/RTOS/html/index.html
 */
/* defines */
#define osKernelSystemId "Huawei LiteOS"

#define osFeature_Signals      8       /* maximum number of Signal Flags available per thread */

#define osMemAlign(p, align_sz)   (((UINTPTR)(p) + (align_sz) - 1) & ~((UINTPTR)((align_sz) - 1)))
#define osMemAlignSz              sizeof(UINTPTR)

#ifdef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
#define osThreadStackAlignSz                  LOSCFG_STACK_POINT_ALIGN_SIZE
#define osMessageMemSz(item_sz, queue_sz)     LOS_QUEUEMEM_SIZE(item_sz, queue_sz)
#define osPoolMemSz(item_sz, pool_sz)         LOS_MEMBOX_SIZE(item_sz, pool_sz)
#endif
#define osWaitForever                       0xFFFFFFFFU
#define osKernelSysTickFrequency            100000000
#define osKernelSysTickMicroSec(microsec)   (((UINT64)(microsec) * (osKernelSysTickFrequency)) / 1000000)

typedef void (*os_pthread) (void const *argument);
typedef void (*os_ptimer) (void const *argument);

typedef void *osThreadId;           /* LosTaskCB */
typedef void *osTimerId;            /* LosSwtmrCB */
typedef void *osMutexId;            /* LosMuxCB */
typedef void *osSemaphoreId;        /* LosSemCB */

typedef struct os_pool_cb *osPoolId;
typedef struct os_messageQ_cb *osMessageQId;
typedef struct os_mailQ_cb *osMailQId;

/* Enumeration */
#ifdef LOSCFG_COMPAT_CMSIS_VER_2
#if (LOSCFG_BASE_CORE_TICK_PER_SECOND != 1000)
#error MUST Follow 1 tick = 1 ms. otherwise osDelay and osTimerStart will be not accurate.
#endif

#define osPriority              osPriority_t

#define osStatus                osStatus_t

#define osEventSignal           ((osStatus)8)
#define osEventMessage          ((osStatus)16)
#define osEventMail             ((osStatus)32)
#define osEventTimeout          ((osStatus)64)
#define osErrorISRRecursive     ((osStatus)-126)
#define osErrorValue            ((osStatus)-127)
#define osErrorPriority         ((osStatus)-128)
#define osErrorOS               ((osStatus)osError)
#define osErrorTimeoutResource  ((osStatus)osErrorTimeout)

#define os_timer_type           osTimerType_t
#define osTimerDelay            2
#else
typedef enum  {
    osPriorityIdle          = -3,         /* priority: lowest */
    osPriorityLow           = -2,         /* priority: low */
    osPriorityBelowNormal   = -1,         /* priority: below normal */
    osPriorityNormal        =  0,         /* priority: normal */
    osPriorityAboveNormal   = +1,         /* priority: above normal */
    osPriorityHigh          = +2,         /* priority: high */
    osPriorityRealtime      = +3,         /* priority: realtime (highest) */
    osPriorityError         =  0x84       /* system cannot determine priority or thread has illegal priority */
} osPriority;

typedef enum  {
    osOK                    =     0,       /* no error */
    osEventSignal           =  0x08,       /* signal event occurred. */
    osEventMessage          =  0x10,       /* message event occurred. */
    osEventMail             =  0x20,       /* mail event occurred. */
    osEventTimeout          =  0x40,       /* timeout occurred. */
    osErrorParameter        =  0x80,       /* parameter error */
    osErrorResource         =  0x81,       /* resource not available */
    osErrorTimeoutResource  =  0xC1,       /* resource not available within given time */
    osErrorISR              =  0x82,       /* not allowed in ISR context */
    osErrorISRRecursive     =  0x83,       /* function called multiple times from ISR with same object. */
    osErrorPriority         =  0x84,       /* system cannot determine priority or thread has illegal priority. */
    osErrorNoMemory         =  0x85,       /* system is out of memory */
    osErrorValue            =  0x86,       /* value of a parameter is out of range. */
    osErrorOS               =  0xFF,       /* unspecified error。 */
    os_status_reserved      =  0x7FFFFFFF  /* prevent from enum down-size compiler optimization. */
} osStatus;

typedef enum  {
    osTimerOnce             =     0,       /* one-shot timer once */
    osTimerPeriodic         =     1,       /* repeating timer */
    osTimerDelay            =     2        /* one-shot timer each time */
} os_timer_type;
#endif /* LOSCFG_COMPAT_CMSIS_VER_2 */

/* structures */
typedef struct os_thread_def  {
    char                       *name;    /* task name */
    os_pthread               pthread;    /* thread entry function */
    osPriority             tpriority;    /* initial thread priority */
    uint32_t               instances;    /* maximum number of instances of that thread function */
    uint32_t               stacksize;    /* stack size; 0 is default stack size */
#ifdef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
    void                  *stackmem;     /* external stack memory address */
#endif
} osThreadDef_t;

typedef struct os_timer_def  {
    os_ptimer                 ptimer;    /* start address of a timer function */
} osTimerDef_t;

typedef struct os_mutex_def  {
    void                      *dummy;
} osMutexDef_t;

typedef struct os_semaphore_def  {
    void                      *dummy;
} osSemaphoreDef_t;

typedef struct os_pool_def  {
    uint32_t                 pool_sz;    /* number of items (elements) in the pool */
    uint32_t                 item_sz;    /* size of an item */
    void                       *pool;    /* pointer to memory for pool */
} osPoolDef_t;

typedef struct os_messageQ_def  {
    uint32_t                queue_sz;    /* number of elements in the queue */
    uint32_t                 item_sz;    /* size of an item */
    void                       *pool;    /* memory array for messages */
} osMessageQDef_t;

typedef struct os_mailQ_def  {
    uint32_t                queue_sz;    /* number of elements in the queue */
    uint32_t                 item_sz;    /* size of an item */
    void                       *pool;    /* memory array for mail */
} osMailQDef_t;

typedef struct  {
    osStatus                 status;     /* event or error information */
    union  {
        uint32_t                    v;   /* message as 32-bit value */
        void                       *p;   /* message or mail as void pointer */
        int32_t               signals;   /* signal flags */
    } value;                             /* event value */
    union  {
        osMailQId             mail_id;   /* mail id */
        osMessageQId       message_id;   /* queue id */
    } def;
} osEvent;

struct osMailQ {
    uint32_t                  queue;
    void                      *pool;
#ifdef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
    uint32_t                   m_sz;
    uint32_t                   q_sz;
#endif
};

#ifdef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
#define osThreadDef(name, priority, instances, stacksz)                                                               \
                                __attribute__((aligned(osThreadStackAlignSz)))                                        \
                                UINT8 os_thread_stack_##name[osMemAlign((stacksz), osThreadStackAlignSz)];            \
                                static const osThreadDef_t os_thread_def_##name = {                                   \
                                                                                    #name, (os_pthread)(name),        \
                                                                                    (priority), (instances),          \
                                                                                    sizeof(os_thread_stack_##name),   \
                                                                                    os_thread_stack_##name            \
                                                                                    }

#define osPoolDef(name, no, type)   __attribute__((aligned(osMemAlignSz)))                                            \
                                    UINT8 os_pool_m_##name[osPoolMemSz(sizeof(type), (no))];                          \
                                    const osPoolDef_t os_pool_def_##name = {                                          \
                                                                            sizeof(os_pool_m_##name),                 \
                                                                            sizeof(type),                             \
                                                                            os_pool_m_##name                          \
                                                                            }

#define osMessageQDef(name, queue_sz, type)  __attribute__((aligned(osMemAlignSz)))                                  \
                                            UINT8 os_messageQ_q_##name[osMessageMemSz(sizeof(UINT32), (queue_sz))];  \
                                            const osMessageQDef_t os_messageQ_def_##name = {                         \
                                                                                            (queue_sz),              \
                                                                                            sizeof(UINT32),          \
                                                                                            os_messageQ_q_##name     \
                                                                                            }

#define osMailQDef(name, queue_sz, type)                                                                             \
    __attribute__((aligned(osMemAlignSz)))                                                                           \
    UINT8 os_mailQ_m_q_##name[osPoolMemSz(sizeof(type), (queue_sz)) + osMessageMemSz(sizeof(void *), (queue_sz))];   \
    struct osMailQ os_mailQ_p_##name = {                                                                             \
                                        0, os_mailQ_m_q_##name,                                                      \
                                        osPoolMemSz(sizeof(type), (queue_sz)), osMessageMemSz(sizeof(void *),        \
                                        (queue_sz))};                                                                \
    const osMailQDef_t os_mailQ_def_##name = {(queue_sz), sizeof(type), &os_mailQ_p_##name}
#else
#define osThreadDef(name, priority, instances, stacksz)                                                               \
                                                const osThreadDef_t os_thread_def_##name = {                          \
                                                                                    #name, (os_pthread)(name),        \
                                                                                    (priority), (instances),          \
                                                                                    (stacksz)                         \
                                                                                    }

#define osPoolDef(name, no, type)   const osPoolDef_t os_pool_def_##name = {(no), sizeof(type), NULL}

#define osMessageQDef(name, queue_sz, type)    const osMessageQDef_t os_messageQ_def_##name =                         \
                                                                                {(queue_sz), sizeof(uint32_t), NULL}

#define osMailQDef(name, queue_sz, type)    struct osMailQ os_mailQ_p_##name = { 0, NULL };                           \
                                            const osMailQDef_t os_mailQ_def_##name = {                                \
                                                                                        (queue_sz), sizeof(type),     \
                                                                                        &os_mailQ_p_##name            \
                                                                                    }
#endif /* LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION */

#define osThread(name)              &os_thread_def_##name
#define osPool(name)                &os_pool_def_##name
#define osMessageQ(name)            &os_messageQ_def_##name
#define osMailQ(name)               &os_mailQ_def_##name

#define osTimerDef(name, function)  const osTimerDef_t os_timer_def_##name = {(function)}
#define osTimer(name)               &os_timer_def_##name

#define osMutexDef(name)            const osMutexDef_t os_mutex_def_##name = {0}
#define osMutex(name)               &os_mutex_def_##name

#define osSemaphoreDef(name)        const osSemaphoreDef_t os_semaphore_def_##name = {0}
#define osSemaphore(name)           &os_semaphore_def_##name

/* Kernel Control Functions Api */
osStatus osKernelInitialize(void);
osStatus osKernelStart(void);
#ifdef LOSCFG_COMPAT_CMSIS_VER_2
int32_t osKernelIsRunning(void);
#define osKernelRunning() osKernelIsRunning()
#else
int32_t osKernelRunning(void);
#endif
uint32_t osKernelSysTick(void);

/* Thread Management Api */
osThreadId osThreadGetId(void);
osThreadId osThreadCreate(const osThreadDef_t *thread_def, void *argument);
osStatus osThreadTerminate(osThreadId thread_id);
osStatus osThreadYield(void);
osStatus osThreadSetPriority(osThreadId thread_id, osPriority priority);
osPriority osThreadGetPriority(osThreadId thread_id);

/* Generic Wait Functions Api */
osStatus osDelay(uint32_t millisec);
osEvent osWait(uint32_t millisec);

/* Timer Api */
osTimerId osTimerCreate(const osTimerDef_t *timer_def, os_timer_type type, void *argument);
osStatus osTimerStart(osTimerId timer_id, uint32_t millisec);
osStatus osTimerStop(osTimerId timer_id);
osStatus osTimerRestart(osTimerId timer_id, uint32_t millisec, uint8_t strict);
osStatus osTimerDelete(osTimerId timer_id);

/* Signal Management Api */
int32_t osSignalSet(osThreadId thread_id, int32_t signals);
int32_t osSignalClear(osThreadId thread_id, int32_t signals);
osEvent osSignalWait(int32_t signals, uint32_t millisec);

/* Mutex Management Api */
osMutexId osMutexCreate(const osMutexDef_t *mutex_def);
osStatus osMutexWait(osMutexId mutex_id, uint32_t millisec);
osStatus osMutexRelease(osMutexId mutex_id);
osStatus osMutexDelete(osMutexId mutex_id);

/* Semaphore Management */
osSemaphoreId osSemaphoreCreate(const osSemaphoreDef_t *semaphore_def, int32_t count);
osSemaphoreId osBinarySemaphoreCreate(const osSemaphoreDef_t *semaphore_def, int32_t count);
int32_t osSemaphoreWait(osSemaphoreId semaphore_id, uint32_t millisec);
osStatus osSemaphoreRelease(osSemaphoreId semaphore_id);
osStatus osSemaphoreDelete(osSemaphoreId semaphore_id);

/* Memory Pool Management */
osPoolId osPoolCreate(const osPoolDef_t *pool_def);
void *osPoolAlloc(osPoolId pool_id);
void *osPoolCAlloc(osPoolId pool_id);
osStatus osPoolFree(osPoolId pool_id, void *block);

/* Message Queue Management Api */
osMessageQId osMessageCreate(const osMessageQDef_t *queue_def, osThreadId thread_id);
osStatus osMessagePut(osMessageQId queue_id, uint32_t info, uint32_t millisec);
osEvent osMessageGet(osMessageQId queue_id, uint32_t millisec);

/* Mail Queue Management */
osMailQId osMailCreate(const osMailQDef_t *queue_def, osThreadId thread_id);
void *osMailAlloc(osMailQId queue_id, uint32_t millisec);
void *osMailCAlloc(osMailQId queue_id, uint32_t millisec);
osStatus osMailPut(osMailQId queue_id, void *mail);
osEvent osMailGet(osMailQId queue_id, uint32_t millisec);
osStatus osMailFree(osMailQId queue_id, void *mail);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _CMSIS_OS1_H */

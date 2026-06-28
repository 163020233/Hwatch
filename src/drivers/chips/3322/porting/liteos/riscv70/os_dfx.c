/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: 3322 application core os memory dfx
 */
#include "errcode.h"
#include "common_def.h"

#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
#include "los_memory.h"
#include "los_task.h"
#include "los_task_pri.h"
#include "soc_osal.h"
#include "los_memory_pri.h"
#include "osal_adapt.h"
#include "exception.h"
#ifdef CONFIG_PSRAM_SUPPORT
#include "psram.h"
#endif

#undef OS_SYS_MEM_ADDR
#undef OS_EXTEND_MEM_ADDR
#define OS_SYS_MEM_ADDR     ((void *)&g_intheap_begin)
#define OS_EXTEND_MEM_ADDR  ((void *)&g_extend_heap_begin)

typedef struct {
    uint32_t taskId;
    uint32_t totalCostSize;
    uint32_t totalPoolUsedSize;
    uint32_t statCount;
    CallerRaMemStat *stats;
    LosMemDynNode *endNode;
} InitialCallerRastate;

static InitialCallerRastate g_initialStates[2] = {0};

static void osal_print_os_sys_task_heap(void *pool, unsigned int tid)
{
    osal_printk("========== [task_id: %d heap] pool start: 0x%x ==========\n", tid, pool);
    LOS_MemTaskHeapInfoGet(pool, tid);
}

static void dfx_print_os_all_sys_task_heap(void *pool)
{
    osal_printk("========== [all task heap] pool start: 0x%x ==========\n", pool);
    for (uint8_t idx = 0; idx < g_taskMaxNum + 1; idx++) {
        osal_print_os_sys_task_heap(pool, idx);
    }
}

static bool OsMemNodeIsUsed(LosMemDynNode *node)
{
    if (node == NULL) {
        return FALSE;
    }
    return ((OS_MEM_NODE_GET_USED_FLAG(node->selfNode.sizeAndFlag) == OS_MEM_NODE_USED_FLAG) ? TRUE : FALSE);
}

static bool updateRaStats(CallerRaMemStat *raStats, uint32_t *raStatsCnt, uintptr_t callerRa, uint32_t size)
{
    uint32_t raCnt = *raStatsCnt;
    for (uint32_t i = 0; i < raCnt; i++) {
        if (raStats[i].callerRa == callerRa) {
            raStats[i].totalSize += size;
            raStats[i].blockCount++;
            return TRUE;
        }
    }
    if (raCnt < RA_BATCH_SIZE) {
        raStats[raCnt].callerRa = callerRa;
        raStats[raCnt].totalSize = size;
        raStats[raCnt].blockCount = 1;
        (raCnt)++;
        *raStatsCnt = raCnt;
        return TRUE;
    }
    return FALSE;
}

static uint32_t collectCallerRaStats(LosMemDynNode *endNode, uint8_t taskId, CallerRaMemStat *raStats,
    uint32_t *totalCostSize, uint32_t *totalPoolUsedSize)
{
    uint32_t raStatsCnt = 0;
    LosMemDynNode *tmpNode;
    uintptr_t callerRa;

    for (tmpNode = endNode->selfNode.preNode; tmpNode != endNode; tmpNode = tmpNode->selfNode.preNode) {
        if ((tmpNode->selfNode.taskId != taskId) || (!OsMemNodeIsUsed(tmpNode))) {
            continue;
        }
        uint32_t size = OS_MEM_NODE_GET_SIZE(tmpNode->selfNode.sizeAndFlag);
        callerRa = tmpNode->selfNode.callerRa;

        bool ret = updateRaStats(raStats, &raStatsCnt, callerRa, size);
        if (ret) {
            *totalCostSize += OS_MEM_NODE_HEAD_SIZE;
            *totalPoolUsedSize += size;
        }
    }
    return raStatsCnt;
}

static void get_all_sys_callerRa(void *pool, uint32_t tid, int poolIndex)
{
    if (pool == NULL) {
        osal_printk("ERROR: Invalid NULL pool pointer for pool %d, task %d\n", poolIndex, tid);
        return;
    }
    uint32_t intSave;
    MEM_LOCK(intSave, pool);
    LosMemPoolInfo *poolInfo = (LosMemPoolInfo *)pool;
    LosMemDynNode *endNode = OS_MEM_END_NODE(pool, poolInfo->poolSize);
    if (g_initialStates[poolIndex].stats != NULL) {
        free(g_initialStates[poolIndex].stats);
        g_initialStates[poolIndex].stats = NULL;
    }
    g_initialStates[poolIndex].stats = (CallerRaMemStat *)malloc(sizeof(CallerRaMemStat) * 0xFF);
    if (g_initialStates[poolIndex].stats == NULL) {
        MEM_UNLOCK(intSave, pool);
        osal_printk("Memory allocation failed.\n");
        return;
    }

    uint32_t totalCostSize = 0;
    uint32_t totalPoolUsedsize = 0;

    g_initialStates[poolIndex].statCount = collectCallerRaStats(endNode, tid, g_initialStates[poolIndex].stats,
        &totalCostSize, &totalPoolUsedsize);
    g_initialStates[poolIndex].taskId = tid;
    g_initialStates[poolIndex].totalCostSize = totalCostSize;
    g_initialStates[poolIndex].totalPoolUsedSize = totalPoolUsedsize;
    g_initialStates[poolIndex].endNode = endNode;
    osal_printk("Initial state recorded for task %d: %d unique callerRa\n", tid, g_initialStates[poolIndex].statCount);
    MEM_UNLOCK(intSave, pool);
}

static int find_matching_callerRa_index(InitialCallerRastate *initialState, uintptr_t callerRa)
{
    for (uint32_t i = 0; i < initialState->statCount; i++) {
        if (callerRa == initialState->stats[i].callerRa) {
            return i;
        }
    }
    return -1;
}

static void compare_callRa_state_changes(InitialCallerRastate *initialState, CallerRaMemStat *currentStats,
    uint32_t currentStatCount)
{
    bool hasNewCallerRa = false;
    for (uint32_t i = 0; i < currentStatCount; i++) {
        int matchindex = find_matching_callerRa_index(initialState, currentStats[i].callerRa);
        if (matchindex >= 0) {
            if (currentStats[i].blockCount > initialState->stats[matchindex].blockCount) {
                osal_printk("WARNING: CallerRa 0x%lx blockCount increased from %u to %u, total size from %u to %u\n",
                            currentStats[i].callerRa,
                            initialState->stats[matchindex].blockCount,
                            currentStats[i].blockCount,
                            initialState->stats[matchindex].totalSize,
                            currentStats[i].totalSize);
                hasNewCallerRa = true;
            }
        } else {
            osal_printk("WARNING: New CallerRa 0x%lx with count %u and total size %u\n",
                        currentStats[i].callerRa,
                        currentStats[i].blockCount,
                        currentStats[i].totalSize);
            hasNewCallerRa = true;
        }
    }
    if (!hasNewCallerRa) {
        osal_printk("No memory leak.\n");
    }
}

void compare_callerRa_state(void *pool, uint32_t tid, int poolIndex)
{
    if (pool == NULL) {
        osal_printk("ERROR: Invalid NULL pool pointer for pool %d, task %d\n", poolIndex, tid);
        return;
    }
    uint32_t intSave;
    MEM_LOCK(intSave, pool);
    LosMemPoolInfo *poolInfo = (LosMemPoolInfo *)pool;
    LosMemDynNode *endNode = OS_MEM_END_NODE(pool, poolInfo->poolSize);
    InitialCallerRastate *initialState = &g_initialStates[poolIndex];
    if (initialState->taskId != tid || initialState->stats == NULL) {
        MEM_UNLOCK(intSave, pool);
        osal_printk("Error: No initial state recorded for pool %d, task %d\n", poolIndex, tid);
        return;
    }

    if (initialState->endNode != endNode) {
        osal_printk("Warning: endNode dosen't match recorded state for pool %d\n", poolIndex);
    }
    
    CallerRaMemStat currentStats[0xFF];
    uint32_t currentTotalCostSize = 0;
    uint32_t currentTotalPoolUsedsize = 0;
    uint32_t currentStatCount = collectCallerRaStats(endNode, tid, currentStats,
        &currentTotalCostSize, &currentTotalPoolUsedsize);
    osal_printk("Comparing states for pool %s, task %d:\n",
        poolIndex == 0 ? "OS_EXTEND_MEM_ADDR" : "OS_SYS_MEM_ADDR", tid);
    osal_printk("Initial: %d unique callerRa, Current: %d unique callerRa\n",
        initialState->statCount, currentStatCount);
    compare_callRa_state_changes(initialState, currentStats, currentStatCount);
    MEM_UNLOCK(intSave, pool);
}

void cleanup_callerRa_state(int poolIndex)
{
    if (g_initialStates[poolIndex].stats != NULL) {
        free(g_initialStates[poolIndex].stats);
        g_initialStates[poolIndex].stats = NULL;
    }
    g_initialStates[poolIndex].statCount = 0;
    g_initialStates[poolIndex].taskId = 0;
    g_initialStates[poolIndex].endNode = NULL;
}

static void dfx_print_heap_statistics_riscv(void *pool)
{
    osal_printk("========== [statistics heap] pool start: 0x%x ==========\n", pool);
    osal_mem_pool_status status;

    osal_mem_info_get(pool, &status);
#if defined(PM_MCPU_MIPS_STATISTICS_ENABLE) && (PM_MCPU_MIPS_STATISTICS_ENABLE == YES)
    oml_pf_log_print4(LOG_BCORE_PLT_INFO_HEAP, LOG_NUM_INFO_HEAP, LOG_LEVEL_INFO,
                      "[HEAP_STAT1] total:0x%x, used:0x%x, free:0x%x, usage waterline:0x%x",
                      (uint32_t)(status.uwTotalFreeSize + status.uwTotalUsedSize),
                      (uint32_t)(status.uwTotalUsedSize), (uint32_t)(status.uwTotalFreeSize),
                      (uint32_t)(status.uwUsageWaterLine));
#endif
    osal_printk("[SysHeap stat] total:0x%x, used:0x%x, current free:0x%x, peak usage:0x%x, peak free:0x%x\r\n", \
                (uint32_t)(status.uwTotalFreeSize + status.uwTotalUsedSize), \
                (uint32_t)(status.uwTotalUsedSize), (uint32_t)(status.uwTotalFreeSize), \
                (uint32_t)(status.uwUsageWaterLine), \
                (uint32_t)(status.uwTotalFreeSize + status.uwTotalUsedSize - status.uwUsageWaterLine));
    osal_printk("Done\r\n");
}

static void dfx_print_os_task_id_and_name(void)
{
    osal_tsk_info_s taskinfo;
    uint32_t ret = 0;
    for (uint32_t loop = 0; loop < g_taskMaxNum + 1; loop++) {
        ret = osal_taskinfo_get(loop, &taskinfo);
        if (ret != OSAL_SUCCESS) {
            continue;
        }
        osal_printk("task_id: %d, task_name: %s\n", taskinfo.uwTaskID, taskinfo.acName);
    }
}

static void dfx_print_stack_waterline_riscv(void)
{
    osal_tsk_info_s taskinfo;
    uint32_t ret = 0;

    osal_printk("task_id  taskName          stackTop   stackLen   peakUsage    sp       peakRatio\r\n");
    for (uint32_t loop = 0; loop < g_taskMaxNum + 1; loop++) {
        ret = osal_taskinfo_get(loop, &taskinfo);
        if (ret != OSAL_SUCCESS) {
            continue;
        }
        if ((taskinfo.usTaskStatus & OS_TASK_STATUS_UNUSED) != 0) {
            continue;
        }
#if defined(PM_MCPU_MIPS_STATISTICS_ENABLE) && (PM_MCPU_MIPS_STATISTICS_ENABLE == YES)
        oml_pf_log_print_alter(LOG_BCORE_PLT_INFO_STACK, LOG_NUM_INFO_STACK, LOG_LEVEL_INFO, \
            "[STACK] id:%d, top:0x%x, size:0x%x, task usage peak:0x%x, sp:0x%x", FIVE_ARG, \
            (uint32_t)(taskinfo.uwTaskID), (uint32_t)(taskinfo.uwTopOfStack), \
            (uint32_t)(taskinfo.uwStackSize), (uint32_t)(taskinfo.uwPeakUsed), (uint32_t)((uintptr_t)(taskinfo.uwSP)));
#endif
        // 不能使用osal_printk，其会调用print_str打印，%%会由于两次格式化而失败，导致无法打印
        print_str("%02d      %-18s 0x%08X 0x%08X 0x%08X 0x%08X %02d%%\r\n", \
            taskinfo.uwTaskID, taskinfo.acName, \
            taskinfo.uwTopOfStack, taskinfo.uwStackSize, taskinfo.uwPeakUsed, (uint32_t)((uintptr_t)(taskinfo.uwSP)), \
            taskinfo.uwPeakUsed * 100 / taskinfo.uwStackSize); // * 100 for calculate percent.
    }
}

uint32_t dfx_show_task_heap(uint32_t tid)
{
    osal_print_os_sys_task_heap(OS_EXTEND_MEM_ADDR, tid);
    osal_print_os_sys_task_heap(OS_SYS_MEM_ADDR, tid);
#ifdef CONFIG_SUPPORT_FS_INDEPENDENT_MEMORY_POOL
    osal_print_os_sys_task_heap((void *)PSRAM_FS_ADDR, tid);
#endif
    return ERRCODE_SUCC;
}

uint32_t dfx_show_all_task_heap(void)
{
    dfx_print_os_all_sys_task_heap(OS_EXTEND_MEM_ADDR);
    dfx_print_os_all_sys_task_heap(OS_SYS_MEM_ADDR);
#ifdef CONFIG_SUPPORT_FS_INDEPENDENT_MEMORY_POOL
    dfx_print_os_all_sys_task_heap((void *)PSRAM_FS_ADDR);
#endif
    return ERRCODE_SUCC;
}

uint32_t dfx_get_callerRa(uint32_t tid)
{
    get_all_sys_callerRa(OS_EXTEND_MEM_ADDR, tid, 0);
    get_all_sys_callerRa(OS_SYS_MEM_ADDR, tid, 1);
    return ERRCODE_SUCC;
}

uint32_t dfx_detect_callerRa(uint32_t tid)
{
    compare_callerRa_state(OS_EXTEND_MEM_ADDR, tid, 0);
    compare_callerRa_state(OS_SYS_MEM_ADDR, tid, 1);
    return ERRCODE_SUCC;
}

uint32_t cleanup_all_callerRa_state(void)
{
    cleanup_callerRa_state(0);
    cleanup_callerRa_state(1);
    return ERRCODE_SUCC;
}


uint32_t dfx_show_heap_statistic(void)
{
    dfx_print_heap_statistics_riscv(OS_EXTEND_MEM_ADDR);
    dfx_print_heap_statistics_riscv(OS_SYS_MEM_ADDR);
#ifdef CONFIG_SUPPORT_FS_INDEPENDENT_MEMORY_POOL
    dfx_print_heap_statistics_riscv((void *)PSRAM_FS_ADDR);
#endif
    return ERRCODE_SUCC;
}

uint32_t dfx_show_mem_info(void)
{
    dfx_print_stack_waterline_riscv();
    dfx_print_heap_statistics_riscv(OS_EXTEND_MEM_ADDR);
    dfx_print_os_all_sys_task_heap(OS_EXTEND_MEM_ADDR);
    dfx_print_heap_statistics_riscv(OS_SYS_MEM_ADDR);
    dfx_print_os_all_sys_task_heap(OS_SYS_MEM_ADDR);
#ifdef CONFIG_SUPPORT_FS_INDEPENDENT_MEMORY_POOL
    dfx_print_heap_statistics_riscv((void *)PSRAM_FS_ADDR);
    dfx_print_os_all_sys_task_heap((void *)PSRAM_FS_ADDR);
#endif
    return ERRCODE_SUCC;
}

uint32_t dfx_show_task_info(void)
{
    dfx_print_os_task_id_and_name();
    return ERRCODE_SUCC;
}

uint32_t dfx_show_stack_waterline(void)
{
    dfx_print_stack_waterline_riscv();
    return ERRCODE_SUCC;
}
#endif

/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 */

#ifndef NPU_DRV_ACL_IF_H
#define NPU_DRV_ACL_IF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
    uint32_t *inputSize;
    uint32_t *outputSize;
    uint32_t fifoSize;
    void *fifoBase;
    uint16_t inputNum;
    uint16_t outputNum;
    uint32_t modelID;
    uintptr_t reserve;
} modelDumpInfo;

/**
 * @ingroup TSDRV
 * @brief query status of model exec by ID
 *
 * @param userModelExecId [IN]      ID of the model exec
 *
 * @retval -1 The model exec no found or no finish.
 * @retval 0  The model exec is finished and run successed.
 * @retval >0 The model exec is finished but run failed.
 */
int32_t aclModelExecStatusQuery(uint16_t userModelExecId);

/**
 * @ingroup TSDRV
 * @brief save data of model exec
 *
 * @param info [IN]                 num and size of model exec
 * @param cachedStatesNum [IN]      num of cached frames
 *
 * @retval 0 The function is successfully executed.
 * @retval OtherValues Failure.
 */
int32_t aclModelExecIOADump(modelDumpInfo *info, uint16_t cachedStatesNum);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

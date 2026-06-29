/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: 差分升级内存类型相关功能
 * Author: LibCK项目组
 * Create: 2025-04-12
 * History: 2025-04-12 LibCK项目组
 */
#include "sre_ota_internal.h"

/*
 * 描述: 校验内存类型
 */
OS_SEC_L4_TEXT U32 OsOtaVerifyMemType(U32 memType)
{
    U32 idx;
    uintptr_t intSave = OsIntLock();

    for (idx = 0; idx < g_otaDesc.memTypeMaxNum; idx++) {
        if (g_otaDesc.memTypeInfo[idx].type == memType) {
            OsIntRestore(intSave);
            return DRB_OK;
        }
    }

    /* 恢复中断 */
    OsIntRestore(intSave);

    /* 内存类型未注册 */
    return OS_ERRNO_OTA_MEM_TYPE_NO_REG;
}

/*
 * 描述: 注册内存类型信息
 */
OS_SEC_L4_TEXT U32 OsOtaRegisterMemType(U32 memType, const struct OtaRegMemTypePara *para)
{
    U32 idx;
    U32 idle = OS_MAX_U32;
    uintptr_t intSave;

    /* 校验入参 */
    if (para == NULL) {
        return OS_ERRNO_OTA_PTR_NULL;
    }

    if (para->attr.directRead == 0 && para->read == NULL) {
        return OS_ERRNO_OTA_READ_MEM_FUNC_NO_REG;
    }

    if (para->attr.directWrite == 0 && para->write == NULL) {
        return OS_ERRNO_OTA_WRITE_MEM_FUNC_NO_REG;
    }

    if ((para->pageSize & (para->pageSize - 1)) != 0) {
        return OS_ERRNO_OTA_MEM_PAGE_SIZE_NOT_POWER_OF_TWO;
    }

    /* 关中断 */
    intSave = OsIntLock();

    for (idx = 0; idx < g_otaDesc.memTypeMaxNum; idx++) {
        if (g_otaDesc.memTypeInfo[idx].type == memType) {
            /* 内存类型已经注册过 */
            OsIntRestore(intSave);
            return OS_ERRNO_OTA_MEM_TYPE_REPEAT_REG;
        }

        if (g_otaDesc.memTypeInfo[idx].type == OS_OTA_MEM_TYPE_INVALID && idle == OS_MAX_U32) {
            /* 找到空闲的索引 */
            idle = idx;
            break;
        }
    }

    if (idle != OS_MAX_U32) {
        /* 保存内存类型信息 */
        g_otaDesc.memTypeInfo[idle].type = memType;
        g_otaDesc.memTypeInfo[idle].attr.value = para->attr.value;
        g_otaDesc.memTypeInfo[idle].pageSize = para->pageSize;
        g_otaDesc.memTypeInfo[idle].write = para->write;
        g_otaDesc.memTypeInfo[idle].read = para->read;

        OsIntRestore(intSave);
        return DRB_OK;
    }

    /* 恢复中断 */
    OsIntRestore(intSave);

    /* 内存类型已注册满 */
    return OS_ERRNO_OTA_MEM_TYPE_REG_FULL;
}

/*
 * 描述: 注册内存类型信息
 */
OS_SEC_L4_TEXT U32 DRB_OtaRegisterMemType(U32 memType, const struct OtaRegMemTypePara *para)
{
    return OsOtaRegisterMemType(memType, para);
}

/*
 * 描述: 获取指定内存类型的内存信息
 */
OS_SEC_L4_TEXT U32 OsOtaGetMemTypeInfo(U32 memType, struct OsOtaMemTypeInfo *memTypeInfo)
{
    U32 ret;
    U32 idx;
    uintptr_t intSave;

    /* 校验内存类型 */
    ret = OsOtaVerifyMemType(memType);
    if (ret != DRB_OK) {
        return ret;
    }

    /* 关中断 */
    intSave = OsIntLock();

    for (idx = 0; idx < g_otaDesc.memTypeMaxNum; idx++) {
        if (g_otaDesc.memTypeInfo[idx].type == memType) {
            /* 获取内存类型信息 */
            memTypeInfo->type = g_otaDesc.memTypeInfo[idx].type;
            memTypeInfo->attr.value = g_otaDesc.memTypeInfo[idx].attr.value;
            memTypeInfo->pageSize = g_otaDesc.memTypeInfo[idx].pageSize;
            memTypeInfo->write = g_otaDesc.memTypeInfo[idx].write;
            memTypeInfo->read = g_otaDesc.memTypeInfo[idx].read;

            OsIntRestore(intSave);
            return DRB_OK;
        }
    }

    /* 恢复中断 */
    OsIntRestore(intSave);

    /* 内存类型未注册 */
    return OS_ERRNO_OTA_MEM_TYPE_NO_REG;
}

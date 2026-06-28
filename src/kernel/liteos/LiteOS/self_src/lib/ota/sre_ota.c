/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: 差分升级相关功能
 * Author: LibCK项目组
 * Create: 2025-04-12
 * History: 2025-04-12 LibCK项目组
 */
#include "sre_ota_internal.h"
#include "./epatch/sre_ota_epatch_internal.h"
#include "sre_mem_external.h"
#include "sre_cpu_external.h"
#include "sre_lib_external.h"
#if defined(OS_OPTION_INIT)
#include "sre_init_external.h"
#include "sre_hw.h"
#endif
#include "drb_typedef.h"
#include "securec.h"

/* ota模块描述 */
OS_SEC_BSS struct OsOtaDesc g_otaDesc;

/*
 * 描述: ota模块信息注册
 */
#if !defined(OS_OPTION_INIT)
OS_SEC_L4_TEXT U32 OsOtaRegister(const struct OtaModInfo *modInfo);
OS_SEC_L4_TEXT U32 OsOtaRegister(const struct OtaModInfo *modInfo)
{
    if (modInfo == NULL) {
        return OS_ERRNO_OTA_PTR_NULL;
    }

    g_otaDesc.memTypeMaxNum = modInfo->memTypeMaxNum;
    g_otaDesc.cpreArithMaxNum = modInfo->cpreArithMaxNum;
    g_otaDesc.memPtNo = modInfo->memPtNo;
    g_otaDesc.memPtAddr = modInfo->memPtAddr;
    g_otaDesc.memPtSize = modInfo->memPtSize;
    return DRB_OK;
}
#else
static INIT_SEC_L4_TEXT void OsOtaConfigReg(void)
{
    uintptr_t addr = g_coreRegPara->otaRegInfo.memPtAddr;
    size_t size = g_coreRegPara->otaRegInfo.memPtSize;

    g_otaDesc.memTypeMaxNum = g_coreRegPara->otaRegInfo.memTypeMaxNum;
    g_otaDesc.cpreArithMaxNum = g_coreRegPara->otaRegInfo.cpreArithMaxNum;
    g_otaDesc.memPtNo = g_coreRegPara->otaRegInfo.memPtNo;

    OsConfigAddrSizeGet(OS_FSC_PT_MEM, addr, (uintptr_t)size, &g_otaDesc.memPtAddr, (uintptr_t *)&g_otaDesc.memPtSize);
}
#endif

/*
 * 描述: 内存类型初始化
 */
OS_SEC_L4_TEXT U32 OsOtaMemTypeInit(void)
{
    U32 idx;
    size_t size;

    size = g_otaDesc.memTypeMaxNum * sizeof(struct OsOtaMemTypeInfo);

    /* 申请存放内存类型信息的内存 */
    g_otaDesc.memTypeInfo = (struct OsOtaMemTypeInfo *)OS_OTA_MEM_ALLOC(size);
    if (g_otaDesc.memTypeInfo == NULL) {
        return OS_ERRNO_OTA_MEM_ALLOC_FAIL;
    }

    if (memset_s(g_otaDesc.memTypeInfo, size, 0, size) != EOK) {
        OS_GOTO_SYS_ERROR();
    }

    /* 初始化内存类型信息 */
    for (idx = 0; idx < g_otaDesc.memTypeMaxNum; idx++) {
        g_otaDesc.memTypeInfo[idx].type = OS_OTA_MEM_TYPE_INVALID;
        g_otaDesc.memTypeInfo[idx].attr.value = 0;
        g_otaDesc.memTypeInfo[idx].pageSize = 0;
        g_otaDesc.memTypeInfo[idx].write = NULL;
        g_otaDesc.memTypeInfo[idx].read = NULL;
    }

    return DRB_OK;
}

/*
 * 描述: 压缩算法初始化
 */
OS_SEC_L4_TEXT U32 OsOtaCpreArithInit(void)
{
    U32 idx;
    size_t size;

    size = g_otaDesc.cpreArithMaxNum * sizeof(struct OsOtaCpreArithInfo);

    /* 申请存放压缩算法信息的内存 */
    g_otaDesc.cpreArithInfo = (struct OsOtaCpreArithInfo *)OS_OTA_MEM_ALLOC(size);
    if (g_otaDesc.cpreArithInfo == NULL) {
        return OS_ERRNO_OTA_MEM_ALLOC_FAIL;
    }

    if (memset_s(g_otaDesc.cpreArithInfo, size, 0, size) != EOK) {
        OS_GOTO_SYS_ERROR();
    }

    /* 初始化压缩算法信息 */
    for (idx = 0; idx < g_otaDesc.cpreArithMaxNum; idx++) {
        g_otaDesc.cpreArithInfo[idx].type = OS_OTA_CPRE_ARITH_INVALID;
        g_otaDesc.cpreArithInfo[idx].compress = NULL;
        g_otaDesc.cpreArithInfo[idx].decompress = NULL;
        g_otaDesc.cpreArithInfo[idx].compressBound = NULL;
    }

    return DRB_OK;
}

/*
 * 描述: ota模块初始化
 */
OS_FUNCTION_INIT(OsOtaConfigInit, OS_LEVEL_OTA_INIT);
OS_SEC_L4_TEXT U32 OsOtaConfigInit(void)
{
    U32 ret;
    struct MemPtCreate ptPara;

#if defined(OS_OPTION_INIT)
    OsOtaConfigReg();
#endif

    /* 创建内存分区 */
    ptPara.arith = MEM_ARITH_FSC;
    ptPara.ptAddr = (void *)g_otaDesc.memPtAddr;
    ptPara.ptSize = (uintptr_t)g_otaDesc.memPtSize;
    ret = DRB_MemCreatePt(&ptPara, g_otaDesc.memPtNo);
    if (ret != DRB_OK) {
        return ret;
    }

    /* 内存类型初始化 */
    ret = OsOtaMemTypeInit();
    if (ret != DRB_OK) {
        return ret;
    }

    /* 压缩算法初始化 */
    ret = OsOtaCpreArithInit();
    if (ret != DRB_OK) {
        return ret;
    }

    return DRB_OK;
}

/*
 * 描述: old镜像参数校验
 */
OS_SEC_L4_TEXT U32 OsOtaOldImageCheckPara(const struct OtaPara *para)
{
    U32 ret;
    struct OsOtaMemTypeInfo memTypeInfo;

    /*
     * 校验old镜像如下信息
     * 1.校验old镜像的内存类型是否合法
     * 2.校验old镜像地址是否为0
     * 3.校验old镜像的内存起始地址是否按内存页大小来对齐
     * 4.校验old镜像的大小是否为0
     * 5.校验old镜像地址与大小是否会出现地址反转
     */
    ret = OsOtaVerifyMemType(para->oldImageMemType);
    if (ret != DRB_OK) {
        return ret;
    }

    ret = OsOtaGetMemTypeInfo(para->oldImageMemType, &memTypeInfo);
    if (ret != DRB_OK) {
        return ret;
    }

    if (para->oldImageAddr == 0) {
        return OS_ERRNO_OTA_OLD_IMAGE_ADDR_ZERO;
    }

    if (OS_NOT_ALIGN_CHK(para->oldImageAddr, memTypeInfo.pageSize)) {
        return OS_ERRNO_OTA_OLD_IMAGE_ADDR_UNALIGN;
    }

    if (para->oldImageSize == 0) {
        return OS_ERRNO_OTA_OLD_IMAGE_SIZE_ZERO;
    }

    if ((para->oldImageAddr + para->oldImageSize) < para->oldImageAddr) {
        return OS_ERRNO_OTA_OLD_IMAGE_ADDR_OVERFLOW;
    }

    return DRB_OK;
}

/*
 * 描述: patch差分包参数校验
 */
OS_SEC_L4_TEXT U32 OsOtaPatchedCheckPara(const struct OtaPara *para)
{
    U32 ret;
    struct OsOtaMemTypeInfo memTypeInfo;

    /*
     * 校验patch文件如下信息
     * 1.校验patch文件的内存类型是否合法
     * 2.校验patch文件的压缩算法是否合法
     * 3.校验patch文件的分块压缩大小是否为0
     * 4.校验patch文件的镜像地址是否为0
     * 5.校验patch文件的内存起始地址是否按内存页大小来对齐
     * 6.校验patch文件的大小是否为0
     * 7.校验patch镜像地址与大小是否会出现地址反转
     */
    ret = OsOtaVerifyMemType(para->patchedMemType);
    if (ret != DRB_OK) {
        return ret;
    }

    ret = OsOtaVerifyCpreArith(para->patchedCpreArith);
    if (ret != DRB_OK) {
        return ret;
    }

    if (para->patchedChunkCpreSize == 0) {
        return OS_ERRNO_OTA_PATCHED_CHUNK_CPRE_SIZE_ZERO;
    }

    if (para->patchedAddr == 0) {
        return OS_ERRNO_OTA_PATCH_IMAGE_ADDR_ZERO;
    }

    ret = OsOtaGetMemTypeInfo(para->patchedMemType, &memTypeInfo);
    if (ret != DRB_OK) {
        return ret;
    }

    if (OS_NOT_ALIGN_CHK(para->patchedAddr, memTypeInfo.pageSize)) {
        return OS_ERRNO_OTA_PATCHED_ADDR_UNALIGN;
    }

    if (para->patchedSize == 0) {
        return OS_ERRNO_OTA_PATCHED_SIZE_ZERO;
    }

    if ((para->patchedAddr + para->patchedSize) < para->patchedAddr) {
        return OS_ERRNO_OTA_PATCH_IMAGE_ADDR_OVERFLOW;
    }

    return DRB_OK;
}

/*
 * 描述: new镜像参数校验
 */
OS_SEC_L4_TEXT U32 OsOtaNewImageCheckPara(const struct OtaPara *para)
{
    U32 ret;
    struct OsOtaMemTypeInfo memTypeInfo;

    /*
     * 校验new镜像如下信息
     * 1.校验new镜像的内存类型是否合法
     * 2.如果生成的new镜像要进行压缩存储，则校验压缩算法是否合法
     * 3.如果生成的new镜像要进行压缩存储，则校验分块压缩大小是否为0
     * 4.校验new镜像地址是否为0
     * 5.校验new镜像的内存起始地址是否按内存页大小来对齐
     * 6.校验存放new镜像的内存最大大小是否为0
     * 7.校验new镜像地址与大小是否会出现地址反转
     */
    ret = OsOtaVerifyMemType(para->newImageMemType);
    if (ret != DRB_OK) {
        return ret;
    }

    if (para->newImageIsCpreStore) {
        ret = OsOtaVerifyCpreArith(para->newImageCpreArith);
        if (ret != DRB_OK) {
            return ret;
        }

        if (para->newImageChunkCpreSize == 0) {
            return OS_ERRNO_OTA_NEW_IMAGE_CHUNK_CPRE_SIZE_ZERO;
        }
    }

    if (para->newImageAddr == 0) {
        return OS_ERRNO_OTA_NEW_IMAGE_ADDR_ZERO;
    }

    ret = OsOtaGetMemTypeInfo(para->newImageMemType, &memTypeInfo);
    if (ret != DRB_OK) {
        return ret;
    }

    if (OS_NOT_ALIGN_CHK(para->newImageAddr, memTypeInfo.pageSize)) {
        return OS_ERRNO_OTA_NEW_IMAGE_ADDR_UNALIGN;
    }

    if (para->newImageMaxSize == 0) {
        return OS_ERRNO_OTA_NEW_IMAGE_MAX_SIZE_ZERO;
    }

    if ((para->newImageAddr + para->newImageMaxSize) < para->newImageAddr) {
        return OS_ERRNO_OTA_NEW_IMAGE_ADDR_OVERFLOW;
    }

    return DRB_OK;
}

/*
 * 描述: ota参数校验
 */
OS_SEC_L4_TEXT U32 OsOtaCheckPara(const struct OtaPara *para)
{
    U32 ret;
    bool overlap0;
    bool overlap1;
    bool overlap2;

    /* 校验差分算法 */
    if (para->diffArith >= OTA_DIFF_ARITH_BUTT) {
        return OS_ERRNO_OTA_DIFF_ARITH_INVALID;
    }

    /* 校验old镜像参数信息 */
    ret = OsOtaOldImageCheckPara(para);
    if (ret != DRB_OK) {
        return ret;
    }

    /* 校验patched参数信息 */
    ret = OsOtaPatchedCheckPara(para);
    if (ret != DRB_OK) {
        return ret;
    }

    /* 校验new镜像参数信息 */
    ret = OsOtaNewImageCheckPara(para);
    if (ret != DRB_OK) {
        return ret;
    }

    /* 校验old镜像、patch文件、new镜像的内存之间是否有重叠 */
    overlap0 = OsOtaVerifyMemAddrOverlap(para->oldImageAddr, para->oldImageSize, para->patchedAddr, para->patchedSize);

    overlap1 = OsOtaVerifyMemAddrOverlap(para->oldImageAddr, para->oldImageSize, para->newImageAddr,
                                         para->newImageMaxSize);

    overlap2 = OsOtaVerifyMemAddrOverlap(para->patchedAddr, para->patchedSize, para->newImageAddr,
                                         para->newImageMaxSize);
    if (overlap0 || overlap1 || overlap2) {
        return OS_ERRNO_OTA_MEM_OVERLAP;
    }

    return DRB_OK;
}

/*
 * 描述: 通过old镜像和patch差分包生成new镜像
 */
OS_SEC_L4_TEXT U32 OsOtaDiffUpgrade(const struct OtaPara *para, U32 *newImageSize)
{
    U32 ret;

    /* 校验入参信息 */
    if (para == NULL || newImageSize == NULL) {
        return OS_ERRNO_OTA_PTR_NULL;
    }

    ret = OsOtaCheckPara(para);
    if (ret != DRB_OK) {
        return ret;
    }

    /* 根据差分算法生成new镜像 */
    switch (para->diffArith) {
        case OTA_DIFF_ARITH_EDIFF:
            ret = OsOtaEpatch(para, newImageSize);
            if (ret != DRB_OK) {
                return ret;
            }
            break;

        default:
            break;
    }

    return DRB_OK;
}

/*
 * 描述: 通过old镜像和patch差分包生成new镜像
 */
OS_SEC_L4_TEXT U32 DRB_OtaDiffUpgrade(const struct OtaPara *para, U32 *newImageSize)
{
    return OsOtaDiffUpgrade(para, newImageSize);
}

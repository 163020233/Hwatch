/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: UPG config file
 */
#ifndef UPG_DEFAULT_CONFIG_H
#define UPG_DEFAULT_CONFIG_H

#ifndef UPG_PRODUCT_CONFIG_H
#error "Include "upg_config.h" if necessary. Do not include this file directly! "
#endif

/* 特性: 支持打开UPG调试开关 */
#ifndef UPG_CFG_DEBUG_ENABLED
#define UPG_CFG_DEBUG_ENABLED                   NO
#endif

/* 特性: 支持打开UPG调试日志 */
#ifndef UPG_CFG_DEBUG_PRINT_ENABLED
#define UPG_CFG_DEBUG_PRINT_ENABLED             NO
#endif

/*
 * 特性: 支持Flash直接访问
 * 说明: 当支持通过总线地址直接访问Flash时, 可设置为YES
 */
#ifndef UPG_CFG_DIRECT_FLASH_ACCESS
#define UPG_CFG_DIRECT_FLASH_ACCESS             NO
#endif

/* 特性: 支持升级进度通知 */
#ifndef UPG_CFG_PROCESS_NOTIFY_SUPPORT
#define UPG_CFG_PROCESS_NOTIFY_SUPPORT          NO
#endif

/*
 * 特性: 支持升级包校验
 * 说明: 强烈建议打开升级校验功能，升级过程中可能会出现各种异常，校验功能可以保证升级过程的可靠性，仅在调试时可临时关闭。
 */
#ifndef UPG_CFG_VERIFICATION_SUPPORT
#define UPG_CFG_VERIFICATION_SUPPORT            YES
#endif

/*
 * 特性: 是否支持签名校验。
 * 说明: 当支持签名校验时，设为YES，只支持完整性校验（如HASH）时，设为NO。
 */
#ifndef UPG_CFG_SIGNATURE_VERIFICATION_ENABLED
#define UPG_CFG_SIGNATURE_VERIFICATION_ENABLED  NO
#endif

/* 特性: 支持RSA签名校验算法 */
#ifndef UPG_CFG_RSA_VERIFICATION_SUPPORT
#define UPG_CFG_RSA_VERIFICATION_SUPPORT        NO
#endif

/* 特性: 支持ECC签名校验算法 */
#ifndef UPG_CFG_ECC_VERIFICATION_SUPPORT
#define UPG_CFG_ECC_VERIFICATION_SUPPORT        NO
#endif

/* 特性: 支持SM2&SM3签名校验算法 */
#ifndef UPG_CFG_SM2_SM3_VERIFICATION_SUPPORT
#define UPG_CFG_SM2_SM3_VERIFICATION_SUPPORT    NO
#endif

/* 签名校验开启的情况下，必须至少支持一种签名算法 */
#if ((UPG_CFG_SIGNATURE_VERIFICATION_ENABLED == YES) && \
    ((UPG_CFG_RSA_VERIFICATION_SUPPORT + UPG_CFG_ECC_VERIFICATION_SUPPORT + UPG_CFG_SM2_SM3_VERIFICATION_SUPPORT) < 1))
#error "At least one signature algorithm must be supported."
#endif

/* 特性: 支持NV升级进度通知 */
#ifndef UPG_CFG_NV_UPGRADE_SUPPORT
#define UPG_CFG_NV_UPGRADE_SUPPORT              NO
#endif

/* 特性: 支持差分升级 */
#ifndef UPG_CFG_DIFF_UPGRADE_SUPPORT
#define UPG_CFG_DIFF_UPGRADE_SUPPORT            YES
#endif

/* 特性: 支持升级包加解密 */
#ifndef UPG_CFG_IMGAE_DECRYPT_SUPPORT
#define UPG_CFG_IMGAE_DECRYPT_SUPPORT           NO
#endif

/* 特性: 支持进度恢复 */
#ifndef UPG_CFG_PROGRESS_RECOVERY_SUPPORT
#define UPG_CFG_PROGRESS_RECOVERY_SUPPORT       YES
#endif

/* 特性: 支持防回滚 */
#ifndef UPG_CFG_ANTI_ROLLBACK_SUPPORT
#define UPG_CFG_ANTI_ROLLBACK_SUPPORT           NO
#endif

/*
 * 特性: 支持文件系统
 * 说明: 当升级包保存在文件系统中时，设为YES
 */
#ifndef UPG_CFG_SUPPORT_FILE_SYSTEM
#define UPG_CFG_SUPPORT_FILE_SYSTEM             YES
#endif

#if ((UPG_CFG_DIFF_UPGRADE_SUPPORT == YES) && (UPG_CFG_PROGRESS_RECOVERY_SUPPORT != YES))
#error "MUST SUPPORT PROGRESS RECOVERY IN DIFF UPGRADE !"
#endif

/*
 * 特性: 支持镜像在文件系统中
 * 说明: 当某些需要升级的镜像保存在文件系统中时，设为YES
 */
#ifndef UPG_CFG_SUPPORT_IMAGE_ON_FILE_SYSTEM
#define UPG_CFG_SUPPORT_IMAGE_ON_FILE_SYSTEM    NO
#endif

/* 特性: 支持资源文件升级 */
#ifndef UPG_CFG_SUPPORT_RESOURCES_FILE
#define UPG_CFG_SUPPORT_RESOURCES_FILE          NO
#endif

/*
 * 特性: 支持事先擦除整个镜像区
 * 说明: 设为NO时，在写入新镜像数据时，是擦除一个block（一般为4K大小），写入一个block的数据。
 *       设为YES时，事先将整个镜像区全部擦除，然后再依次按block写入新的镜像数据。（差分升级不支持YES的模式）
 *
 */
#ifndef UPG_CFG_SUPPORT_ERASE_WHOLE_IMAGE
#define UPG_CFG_SUPPORT_ERASE_WHOLE_IMAGE       NO
#endif

#ifndef UPG_CFG_SUPPORT_BOOT_IMAGE_PROTECTION
#define UPG_CFG_SUPPORT_BOOT_IMAGE_PROTECTION   NO
#endif

#ifndef UPG_CFG_SUPPORT_CUSTOM_UPDATE
#define UPG_CFG_SUPPORT_CUSTOM_UPDATE           NO
#endif

#ifndef UPG_CFG_SUPPORT_VERIFICATION_IN_REQUEST
#define UPG_CFG_SUPPORT_VERIFICATION_IN_REQUEST YES
#endif

#ifndef UPG_CFG_SUPPORT_VERIFICATION_IN_IMAGE
#define UPG_CFG_SUPPORT_VERIFICATION_IN_IMAGE   YES
#endif

#endif /* UPG_DEFAULT_CONFIG_H */
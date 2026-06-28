/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides configuration definitions related to partitions. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-01, Create file. \n
 */
#ifndef PARTITION_RESOURCE_ID_H
#define PARTITION_RESOURCE_ID_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* partition image id, must be same as flash partition bin */
#define PARTITION_IMAGE_ID  0x4B87A54B
/**
 * @defgroup middleware_chips_partition Partition
 * @ingroup  middleware_chips
 * @{
 */

/**
 * @if Eng
 * @brief  Partition ids.
 * @else
 * @brief  分区ID
 * @endif
 */
typedef enum {
    PARTITION_SSB_IMAGE = 0x0,
    PARTITION_SSB_BACKUP = 0x1,
    PARTITION_RECOVERY_IMAGE = 0x20,
    PARTITION_SELITEOS_IMAGE = 0x21,
    PARTITION_ACPU_IMAGE = 0x22,
    PARTITION_BT_IMAGE = 0x23,
    PARTITION_DSP_MAIN_IMAGE = 0x24,
    PARTITION_DSP0_IMAGE = PARTITION_DSP_MAIN_IMAGE,
    PARTITION_DSP_OVERLAY_IMAGE = 0x25,
    PARTITION_DSP1_IMAGE = PARTITION_DSP_OVERLAY_IMAGE,
    PARTITION_NV_DATA = 0x30,
    PARTITION_FOTA_DATA = 0x31,
    PARTITION_SSB_UPG_IMAGE = 0x32,
    PARTITION_SELITEOS_UPG_IMAGE = 0x33,
    PARTITION_DFX_DATA = 0x34,
    PARTITION_LOG_FILE = 0x35,
    PARTITION_DUMP_INFO = 0x36,
    PARTITION_FILE_SYSTEM = 0x37,

    PARTITION_MAX_CNT = 15 /*!< @if Eng Maximum number of partitions.
        The value is recorded only as the number of partitions and is not used to determine the ID validity.
                                @else   分区的数量，这个枚举只用来记录分区的数量，不作为分区ID有效性的判断 @endif */
} partition_ids_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* PARTITION_RESOURCE_ID_H */

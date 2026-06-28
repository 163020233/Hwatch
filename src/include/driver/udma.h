/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Provides uDMA driver api \n
 *
 * History: \n
 * 2025-7-16， Create file. \n
 */
#ifndef UDMA_H
#define UDMA_H

#include <stdint.h>
#include "errcode.h"
#include "udma_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define UDMA_CH_MAX_TRANSFER_NUM (0x1UL << 10) /* 1024 */

/** UDMA channel cycle type
 * UDMA cycle: All the UDMA transfers that the controller must perform,
 * to transfer the N data packets.
 * UDMA cycle type: The operating mode of the UDMA cycle.
 */
typedef enum {
    UDMA_CH_CYCLE_TYPE_SINGLE       = 0,   /*!< Single UDMA cycle of a continuous address scope */
    UDMA_CH_CYCLE_TYPE_BATCHED_PART = 1,   /*!< One of Batched UDMA cycles preceding the end one */
    /* < Batched UDMA cycle as following:
     * < We transfer multiple parts which each one identifes
     * < one scope of addresses transferring and usually are
     * < incontinuous address scopes with each other.
     * <
     * < +------+   +------+   +------+   +------+
     * < | Part |-->| Part |-->| Part |-->| End  |
     * < +------+   +------+   +------+   +------+
     */
    UDMA_CH_CYCLE_TYPE_BATCHED_END  = 2,   /*!< The end of Batched UDMA cycles */
    UDMA_CH_CYCLE_TYPE_MAX
} udma_ch_cycle_type_t;

/** UDMA Interrupt type
 */
typedef enum {
    UDMA_INTERRUPT_DONE,
    UDMA_INTERRUPT_ERR,
    UDMA_INTERRUPT_MAX
} udma_interrupt_t;

/** UDMA Channel transfer type
 */
typedef enum {
    UDMA_CH_TRANSFER_TYPE_MEM_TO_MEM        = 0,
    UDMA_CH_TRANSFER_TYPE_MEM_TO_PERIPHERAL = 1,
    UDMA_CH_TRANSFER_TYPE_PERIPHERAL_TO_MEM = 2,
    UDMA_CH_TRANSFER_TYPE_MAX
} udma_ch_transfer_type_t;

/** The power of 2 indicating how many UDMA transfers responding to one burst request
 * UDMA transfer: The action of transferring a single byte, halfword, or word.
 */
typedef enum {
    UDMA_POWER_OF_BURST_0    = 0,   /*!< Run each UDMA transfer in one burst */
    UDMA_POWER_OF_BURST_1    = 1,   /*!< Run 2 UDMA transfers in one burst */
    UDMA_POWER_OF_BURST_2    = 2,   /*!< Run 4 UDMA transfers in one burst */
    UDMA_POWER_OF_BURST_3    = 3,   /*!< Run 8 UDMA transfers in one burst */
    UDMA_POWER_OF_BURST_4    = 4,   /*!< Run 16 UDMA transfers in one burst */
    UDMA_POWER_OF_BURST_5    = 5,   /*!< Run 32 UDMA transfers in one burst */
    UDMA_POWER_OF_BURST_6    = 6,   /*!< Run 64 UDMA transfers in one burst */
    UDMA_POWER_OF_BURST_7    = 7,   /*!< Run 128 UDMA transfers in one burst */
    UDMA_POWER_OF_BURST_8    = 8,   /*!< Run 256 UDMA transfers in one burst */
    UDMA_POWER_OF_BURST_9    = 9,   /*!< Run 512 UDMA transfers in one burst */
    UDMA_POWER_OF_BURST_10   = 10,  /*!< Run 1024 UDMA transfers in one burst */
} udma_power_of_burst_t;

/**
 * @if Eng
 * @brief  UDMA channel user configuration for memory-to-memory type transfers
 * @else
 * @brief  内存到内存类型的UDMA通道用户配置
 * @endif
 */
typedef struct {
    /** @if Eng
     *  The source address of this transfer.
     *  @else
     *  传输的源地址。
     *  @endif */
    uint32_t             src;
    /** @if Eng
     *  The destination address of this transfer.
     *  @else
     *  传输的目的地址。
     *  @endif */
    uint32_t             dst;
    /** @if Eng
     *  Number of bytes/halfwords/words to transfer, based on the minimum transfer unit.
     *  @else
     *  传输的字节/半字/字的数量，基于最小传输单元。
     *  @endif */
    uint16_t             transfer_num;
    /** @if Eng
     *  Channel cycle type, specified by the user to indicate whether the transfer is single or cyclic.
     *  @else
     *  通道循环类型，由用户指定，指示传输是单次还是循环。
     *  @endif */
    udma_ch_cycle_type_t cycle_type;
} udma_ch_user_memory_config_t;

/**
 * @if Eng
 * @brief  UDMA channel user configuration for non-memory-to-memory types
 * @else
 * @brief  非内存到内存类型的UDMA通道用户配置（例如内存到外设或外设到内存）
 * @endif
 */
typedef struct {
    /** @if Eng
     *  The memory start address of this transfer. It is the source address if the transfer type is
     *  memory-to-peripheral, or the destination address if the transfer type is peripheral-to-memory.
     *  @else
     *  传输的内存起始地址。如果是内存到外设类型，addr是源地址；如果是外设到内存类型，addr是目的地址。
     *  @endif */
    uint32_t                addr;
    /** @if Eng
     *  Number of bytes/halfwords/words to transfer, based on the minimum transfer unit.
     *  @else
     *  传输的字节/半字/字的数量，基于最小传输单元。
     *  @endif */
    uint16_t                transfer_num;
    /** @if Eng
     *  Channel transfer type, indicating whether the transfer is memory-to-peripheral or peripheral-to-memory.
     *  @else
     *  通道传输类型，指示传输是内存到外设还是外设到内存。
     *  @endif */
    udma_ch_transfer_type_t transfer_type;
    /** @if Eng
     *  Channel cycle type, specified by the user to indicate whether the transfer is single or cyclic.
     *  @else
     *  通道循环类型，由用户指定，指示传输是单次还是循环。
     *  @endif */
    udma_ch_cycle_type_t    cycle_type;
} udma_ch_user_peripheral_config_t;

/**
 * @if Eng
 * @brief  UDMA peripheral feature configuration
 * @else
 * @brief  UDMA外设特性配置
 * @endif
 */
typedef struct {
    /** @if Eng
     *  Power of 2 for one time of burst transfers specific to the connected peripheral
     *  @else
     *  与连接的外设相关的单次突发传输的2的幂次方
     *  @endif */
    udma_power_of_burst_t power_of_burst;
    /** @if Eng
     *  The controller only responds to udma_req[C] requests and performs 2^POWER_OF_BURST transfers
     *  @else
     *  控制器仅响应udma_req[C]请求，并执行2^POWER_OF_BURST次传输
     *  @endif */
    bool                  use_burst;
    /** @if Eng
     *  Channel connected peripheral address
     *  @else
     *  连接的外设地址
     *  @endif */
    uint32_t              periph_addr;
} udma_peripheral_feature_t;

/** Prepare to transfer data through UDMA channel based on the associated peripheral
 */
typedef void (*udma_transfer_peripheral_prepare_callback_t)(udma_peripheral_t peripheral);

/** Finish to transfer data through UDMA channel based on the associated peripheral
 */
typedef void (*udma_transfer_peripheral_finish_callback_t)(udma_peripheral_t peripheral);

/** UDMAC channel transfer done/error callback function pointer type of user
 */
typedef void (*udma_transfer_callback_t)(udma_channel_t ch, udma_interrupt_t intr);

/**
 * @if Eng
 * @brief  UDMA peripheral configuration structure supplied by the peripheral driver
 * @else
 * @brief  由外设驱动程序提供的UDMA外设配置结构体
 * @endif
 */
typedef struct {
    /** @if Eng
     *  UDMA peripheral ID, used to uniquely identify the peripheral in UDMA operations
     *  @else
     *  UDMA外设ID，在UDMA操作中唯一标识外设
     *  @endif */
    udma_peripheral_t                           peripheral;
    /** @if Eng
     *  Peripheral feature configuration for UDMA transfer, including parameters like power_of_burst and use_burst
     *  @else
     *  UDMA传输相关的外设特性配置，包括power_of_burst和use_burst等参数
     *  @endif */
    udma_peripheral_feature_t                   feature;
    /** @if Eng
     *  Transfer prepare callback function provided by the peripheral driver, called before the transfer starts
     *  @else
     *  传输准备回调函数，由外设驱动提供，在传输开始前调用
     *  @endif */
    udma_transfer_peripheral_prepare_callback_t transfer_prepare;
    /** @if Eng
     *  Transfer finish callback function provided by the peripheral driver, called after the transfer completes
     *  @else
     *  传输完成回调函数，由外设驱动提供，在传输完成后调用
     *  @endif */
    udma_transfer_peripheral_finish_callback_t  transfer_finish;
} udma_peripheral_config_t;

/** Initialize the UDMA module
 */
errcode_t uapi_udma_init(void);

/** Deinitialize the UDMA module
 */
void uapi_udma_deinit(void);

/**
 * @brief Get a free previously claimed UDMA Controller's channel for data transfer.
 * @param ch the UDMAC channel ID
 * @param pri the minimum priority of the expected opened channel
 * @param config the UDMA peripheral configurations set if not NULL, and indicates that
 *   the UDMA channel is doing memory-to-memory transfers if the cfg is NULL
 * @return OK if the channle opened succeeded or corresponding error code otherwise
 */
errcode_t uapi_udma_ch_open(udma_channel_t *ch, udma_ch_priority_t pri, const udma_peripheral_config_t *config);

/**
 * @brief Close the specific UDMA Channel
 * @param ch the UDMAC channel ID
 * @return OK if the channle closed succeeded or corresponding error code otherwise
 */
errcode_t uapi_udma_ch_close(udma_channel_t ch);

/**
 * @brief Transfer bytes of type memory-to-memory through UDMA channel
 * @param ch the UDMAC channel ID
 * @param user_cfg UDMAC channel transfer configuration of user
 * @param callback channel transfer done/error callback function
 * @return OK if the operation succeeded or corresponding error code otherwise
 */
errcode_t uapi_udma_transfer_memory_bytes(udma_channel_t ch, const udma_ch_user_memory_config_t *user_cfg,
                                          udma_transfer_callback_t callback);

/**
 * @brief Transfer halfwords of type memory-to-memory through UDMA channel
 * @param ch the UDMAC channel ID
 * @param user_cfg UDMAC channel transfer configuration of user
 * @param callback channel transfer done/error callback function
 * @return OK if the operation succeeded or corresponding error code otherwise
 */
errcode_t uapi_udma_transfer_memory_halfwords(udma_channel_t ch, const udma_ch_user_memory_config_t *user_cfg,
                                              udma_transfer_callback_t callback);

/**
 * @brief Transfer words of type memory-to-memory through UDMA channel
 * @param ch the UDMAC channel ID
 * @param user_cfg UDMAC channel transfer configuration of user
 * @param callback channel transfer done/error callback function
 * @return OK if the operation succeeded or corresponding error code otherwise
 */
errcode_t uapi_udma_transfer_memory_words(udma_channel_t ch, const udma_ch_user_memory_config_t *user_cfg,
                                          udma_transfer_callback_t callback);

/**
 * @brief Transfer bytes of type memory-to-peripheral or peripheral-to-memory through UDMA channel
 * @param ch the UDMAC channel ID
 * @param user_cfg UDMAC channel transfer configuration of user
 * @param callback channel transfer done/error callback function
 * @return OK if the operation succeeded or corresponding error code otherwise
 */
errcode_t uapi_udma_transfer_peripheral_bytes(udma_channel_t ch, const udma_ch_user_peripheral_config_t *user_cfg,
                                              udma_transfer_callback_t callback);

/**
 * @brief Transfer halfwords of type memory-to-peripheral or peripheral-to-memory through UDMA channel
 * @param ch the UDMAC channel ID
 * @param user_cfg UDMAC channel transfer configuration of user
 * @param callback channel transfer done/error callback function
 * @return OK if the operation succeeded or corresponding error code otherwise
 */
errcode_t uapi_udma_transfer_peripheral_halfwords(udma_channel_t ch, const udma_ch_user_peripheral_config_t *user_cfg,
                                                  udma_transfer_callback_t callback);

/**
 * @brief Transfer words of type memory-to-peripheral or peripheral-to-memory through UDMA channel
 * @param ch the UDMAC channel ID
 * @param user_cfg UDMAC channel transfer configuration of user
 * @param callback channel transfer done/error callback function
 * @return OK if the operation succeeded or corresponding error code otherwise
 */
errcode_t uapi_udma_transfer_peripheral_words(udma_channel_t ch, const udma_ch_user_peripheral_config_t *user_cfg,
                                              udma_transfer_callback_t callback);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Provides hal uDMA \n
 *
 * History: \n
 * 2025-7-16， Create file. \n
 */
#ifndef HAL_UDMA_H
#define HAL_UDMA_H

#include "hal_udma_regs_def.h"
#include "udma.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/** @defgroup DRIVER_HAL_UDMA HAL UDMA Driver
 * HAL UDMA Driver
 * @ingroup DRIVER_HAL
 * @{
 **/
#define HAL_UDMA_CH_TRANSFER_QUEUE_MAX_SIZE              10UL
#define HAL_UDMA_CH_MAX_TRANSFER_NUM                     (0x1UL << 10) /* 1024 */

/* Maximum Scatter-Gather mode configs number in a group of Scatter-Gather UDMA configs
 * which starts with a Memory/Peripheral alternate Scatter-Gather config, ends with a
 * Basic config.
 */
#define HAL_UDMA_MAX_SUPPORTED_SCATTER_GATHER_CONFIGS    9

/** Compute the UDMA transfers of arbitration occurs based on the r_power value */
#define hal_udma_r_power_transfers(r_power)              (1UL << (r_power))

/** UDMA real priority recognised by the UDMAC, the real priority value less, the priority higher,
 *  as follows(there are 4 channels in each UDMAC):
 *  CH ID   |   PriorityLevelSetting   |    Real Priority
 *    0     |          High            |       Highest
 *    1     |          High            |          |
 *    2     |          High            |          |
 *    3     |          High            |          |Descending
 *    0     |          Default         |          |
 *    1     |          Default         |          |
 *    2     |          Default         |          V
 *    3     |          Default         |        Lowest
 */
#define hal_udma_ch_real_priority(ch_id, level)          ((level) * UDMA_CHANNEL_MAX_NUM + (ch_id))
#define hal_udma_ch_cycle_type_scatter_gather_alternate_to_primary(mode) (hal_udma_ch_cycle_type_t)((mode) - 1)

#define UDMA_CONTROLLER_ENABLE_MASK              0x1

/** -- Macros definitions. */
#define HAL_UDMA_CH_CTRL_DATA_WORDS_POWER_OF_2   2
#define HAL_UDMA_CYCLE_TYPE_MASK                 0x7UL
#define HAL_UDMA_N_MINUS_1_MASK                  hal_udma_n_minus_1_bits(0x3FFUL)

#define hal_udma_dst_inc_bits(inc)               ((inc) << 30)
#define hal_udma_dst_size_bits(size)             ((size) << 28)
#define hal_udma_src_inc_bits(inc)               ((inc) << 26)
#define hal_udma_src_size_bits(size)             ((size) << 24)

#define hal_udma_r_power_bits(power)             ((power) << 14)
#define hal_udma_n_minus_1_bits(num)             ((num) << 4)
#define hal_udma_next_use_burst_bits(bur)        ((bur) << 3)
#define hal_udma_cycle_type_bits(type)           (type)

#define CHANS_CTRL_ALIGNED_SIZE                 (4 * 4 * UDMA_CHANNEL_MAX_NUM * 2)
#define CHANS_CTRL_NUM                          (UDMA_CHANNEL_MAX_NUM * 2)

/* The maximum scatter-gather mode configs num before meeting the end config(s).
 * Since the end config supplied by user maybe be splitted to two configs, we reserves 2 configs here.
 */
#define UDMA_MAX_SUPPORT_BATCHED_CONFIGS_BEFORE_END      (HAL_UDMA_MAX_SUPPORTED_SCATTER_GATHER_CONFIGS - 2)
#define udma_power_of_burst_map_to_hal_r_power(power)    (hal_udma_r_power_t)(power)

typedef enum {
    UDMA_INTERRUPT_STATE_DISABLED, /*!< Interrupt Disabled */
    UDMA_INTERRUPT_STATE_ENABLED,  /*!< Interrupt Enabled  */
} udma_interrupt_state_t;

/** UDMA Channel priority
 */
typedef enum {
    HAL_UDMA_CH_PRIORITY_LEVEL_HIGH,
    HAL_UDMA_CH_PRIORITY_LEVEL_DEFAULT,
} hal_udma_ch_priority_level_t;

/** UDMA transfer data width
 */
typedef enum {
    HAL_UDMA_DATA_WIDTH_BYTE,
    HAL_UDMA_DATA_WIDTH_HALFWORD,
    HAL_UDMA_DATA_WIDTH_WORD,
    HAL_UDMA_DATA_WIDTH_RESERVED,
} hal_udma_data_width_t;

/** UDMA src/dst address increment
 */
typedef enum {
    HAL_UDMA_ADDRESS_INC_BYTE,
    HAL_UDMA_ADDRESS_INC_HALFWORD,
    HAL_UDMA_ADDRESS_INC_WORD,
    HAL_UDMA_ADDRESS_INC_NO_INC,
    HAL_UDMA_ADDRESS_INC_TYPES
} hal_udma_address_inc_t;

/** The power of 2 indicating how many UDMA transfers before rearbitrating
 * UDMA transfer: The action of transferring a single byte, halfword, or word.
 */
typedef enum {
    HAL_UDMA_R_POWER_0,    /*!< Arbitrates after each UDMA transfer */
    HAL_UDMA_R_POWER_1,    /*!< Arbitrates after 2 UDMA transfers */
    HAL_UDMA_R_POWER_2,    /*!< Arbitrates after 4 UDMA transfers */
    HAL_UDMA_R_POWER_3,    /*!< Arbitrates after 8 UDMA transfers */
    HAL_UDMA_R_POWER_4,    /*!< Arbitrates after 16 UDMA transfers */
    HAL_UDMA_R_POWER_5,    /*!< Arbitrates after 32 UDMA transfers */
    HAL_UDMA_R_POWER_6,    /*!< Arbitrates after 64 UDMA transfers */
    HAL_UDMA_R_POWER_7,    /*!< Arbitrates after 128 UDMA transfers */
    HAL_UDMA_R_POWER_8,    /*!< Arbitrates after 256 UDMA transfers */
    HAL_UDMA_R_POWER_9,    /*!< Arbitrates after 512 UDMA transfers */
    HAL_UDMA_R_POWER_10,  /*!< Arbitrates after 1024 UDMA transfers */
    HAL_UDMA_R_POWER_DEFAULT = HAL_UDMA_R_POWER_0
} hal_udma_r_power_t;

/** Determine whether the remaining peripheral scatter-gather mode
 * UDMA transfers use burst mode while completing a time of UDMA
 * transfer that uses the alternate data structure
 */
typedef enum {
    /*!< Remaining UDMA cycles both responding to burst transfers request and single transfer request */
    HAL_UDMA_NEXT_USE_BURST_DISABLE,
    /*!< Remaining UDMA cycles only responding to burst transfers request */
    HAL_UDMA_NEXT_USE_BURST_ENABLE
} hal_udma_next_use_burst_t;

/** UDMA Channel cycle type
 * UDMA cycle: All the UDMA transfers that the controller must perform,
 * to transfer the N data packets.
 * UDMA cycle type: The operating mode of the UDMA cycle.
 */
typedef enum {
    /*!< Channel control data structure is invalid */
    HAL_UDMA_CH_CYCLE_TYPE_STOP,
    /*!< Basic UDMA transfer */
    HAL_UDMA_CH_CYCLE_TYPE_BASIC,
    /*!< Auto-request */
    HAL_UDMA_CH_CYCLE_TYPE_AUTO_REQUEST,
    /*!< Ping-pong */
    HAL_UDMA_CH_CYCLE_TYPE_PING_PONG,
    /*!< Memory scatter-gather using the primary data structure */
    HAL_UDMA_CH_CYCLE_TYPE_SCATTER_GATHER_MEM_PRI,
    /*!< Memory scatter-gather using the alternate data structure */
    HAL_UDMA_CH_CYCLE_TYPE_SCATTER_GATHER_MEM_ALT,
    /*!< Peripheral scatter-gather using the primary data structure */
    HAL_UDMA_CH_CYCLE_TYPE_SCATTER_GATHER_PERIPHERAL_PRI,
    /*!< Peripheral scatter-gather using the alternate data structure */
    HAL_UDMA_CH_CYCLE_TYPE_SCATTER_GATHER_PERIPHERAL_ALT,
} hal_udma_ch_cycle_type_t;

/** UDMA channel working state
 */
typedef enum {
    HAL_UDMA_CH_STATE_CLOSED,    /*!< The UDMA channel is closed */
    HAL_UDMA_CH_STATE_IDLE,      /*!< The UDMA channel has been opend but its logic configuration queue has
                                      no configurations to be sent */
    HAL_UDMA_CH_STATE_ACTIVE,    /*!< The UDMA channel is transferring data or has been done the data transferring */
    HAL_UDMA_CH_STATE_FINISHED,  /*!< Useless now */
    HAL_UDMA_CH_STATE_ERROR      /*!< The UDMA channel has had error while transferring data */
} hal_udma_ch_state_t;

/** UDMA Interrupt type
 */
typedef enum {
    HAL_UDMA_INTERRUPT_DONE,
    HAL_UDMA_INTERRUPT_ERR,
    HAL_UDMA_INTERRUPT_MAX
} hal_udma_interrupt_t;

/** UDMAC channel transfer done/error callback function pointer type of user
 */
typedef void (*hal_udma_transfer_callback_t)(udma_channel_t ch, hal_udma_interrupt_t intr);

/** UDMA Channel peripheral feature
 */
typedef struct {
    bool use_burst;
} hal_udma_ch_periph_feature_t;

/* Channel Control structure */
typedef struct {
    uint32_t src_end_addr;
    uint32_t dest_end_addr;
    volatile uint32_t ctrl;
    uint32_t empty;
} udma_ch_ctrl_t;

/** Prepare to transfer data through UDMA channel based on the associated peripheral
 */
typedef void (*hal_udma_transfer_peripheral_prepare_callback_t)(udma_peripheral_t peripheral);

/** UDMA Channel configuration
 */
typedef struct hal_udma_ch_cfg {
    uint32_t                     src;                    /*!< Start address of source */
    uint32_t                     dst;                    /*!< Start address of destination */
    hal_udma_data_width_t        data_width;             /*!< src and dst data width */
    hal_udma_address_inc_t       src_inc;                /*!< source address increment */
    hal_udma_address_inc_t       dst_inc;                /*!< destination address increment */
    hal_udma_next_use_burst_t    next_use_burst;         /*!< Whether the next transfer after this time transfer using
                                                            mode when using Peripheral Scatter-Gather transfer */
    uint16_t                     transfer_num;           /*!< Total UDMA transfers of the configuraion in unit of
                                                            data_width */
    hal_udma_ch_cycle_type_t     cycle_type;             /*!< UDMA cycle type */
    hal_udma_r_power_t           r_power;                /*!< Control how many UDMA transfers can occur before the
                                                            controller rearbitrates */
    uint8_t                      scatter_gather_configs; /*!< Configs num while in the scatter-gather mode transfer */
    bool                         ready;                  /*!< This configuration has been ready to be transferred */
    uint8_t                      index;                  /*!< The self index in the config buffer */
    hal_udma_transfer_callback_t callback;               /*!< Transfer done/error callback of user */
} hal_udma_ch_configuration_t;

/** UDMA Interrupt Service Routine Callback Type
 */
typedef void (*hal_udma_isr_callback_t)(udma_channel_t ch);

/**
 * Internal UDMA interrupt callbacks type
 */
typedef struct {
    hal_udma_isr_callback_t done_isr;
    hal_udma_isr_callback_t err_isr;
} hal_udma_isr_callbacks_t;

/* UDMA channel informations */
typedef struct {
    hal_udma_ch_priority_level_t priority_level;           /*!< Channel priority level */
    udma_peripheral_t            periph;                   /*!< Channel associated peripheral */
    hal_udma_ch_state_t          state;                    /*!< Channel transferring state */
    hal_udma_ch_periph_feature_t periph_feature;           /*!< Channel feature relevant to the associated peripheral */
    hal_udma_isr_callbacks_t     isr_callbacks;            /*!< Channel ISR callback functions */
    hal_udma_ch_configuration_t  *config_buffer;           /*!< Pre-allocated array of configurations using by user */
    uint8_t                      config_buffer_head_index; /*!< The first used config index in the config buffer */
    uint8_t                      used_configs;             /*!< The count of used configs in the config buffer */
} hal_udma_ch_info_t;

/** UDMA Channel common user configuration which type is memory-to-memory/
 * memory-to-peripheral/peripheral-to-memory
 */
typedef struct {
    udma_ch_transfer_type_t transfer_type; /*!< Channel transfer type */
    hal_udma_data_width_t   data_width;    /*!< Channel transfer data width */
    uint32_t                src;           /*!< The source address of this transfer */
    uint32_t                dst;           /*!< The destination address of this transfer */
    uint16_t                transfer_num;  /*!< transfer number of the bytes/halfwords/words
                                              based on your minimum transfer unit */
    udma_ch_cycle_type_t    cycle_type;    /*!< channel cycle type that user indicated */
} udma_ch_user_common_config;

/** UDMA channel configurations adding FSM State
 * [NOTE] The config queue following indicates the logic config queue storing the
 *        UDMA channel configs to be sent in the future. The implementation of the
 *        logic config queue can be any format data structure that can decribe the
 *        feature of the logic config queue, such as linked list queue, array queue,
 *        etc.
 */
typedef enum {
    /*!< The state of adding single cycle type UDMA channel configuration to the config queue */
    UDMA_CH_CONFIGS_ADDING_STATE_SINGLE,
    /*!< The state of adding first batched part cycle type UDMA channel configuration to the config queue */
    UDMA_CH_CONFIGS_ADDING_STATE_BATCHED_START,
    /*!< The state of adding non-first batched part cycle type UDMA channel configuration to the config queue */
    UDMA_CH_CONFIGS_ADDING_STATE_BATCHED_RUNNING,
    /*!< The state of adding batched end cycle type UDMA channel configuration to the config queue */
    UDMA_CH_CONFIGS_ADDING_STATE_BATCHED_COMPLETE,
    /*!< The UDMA channel configuraion adding FSM state number */
    UDMA_CH_CONFIGS_ADDING_STATE_NUM
} udma_ch_configs_adding_state;

/** UDMA channel configurations adding FSM
 */
typedef struct {
    /*!< The UDMA channel configuration adding FSM state */
    udma_ch_configs_adding_state state;
    /*!< Current UDMA channel configuraion transfer type */
    udma_ch_transfer_type_t      transfer_type;
    /*!< The number of configurations in current batched UDMA channel configurations, which increases while
         pushing the configuration to the current batch */
    uint8_t                      batched_configs_num;
    /*!< Whether the buffer for UDMA batched cycle type had been allocated */
    bool                         batched_buffer_allocated;
    /*!< The first configuration of current batched UDMA channel configurations */
    hal_udma_ch_configuration_t  *batched_configs_head;
} udma_ch_config_adding_fsm_t;

/** UDMA Channel configuring control data for transferring data
 */
typedef struct {
    uint8_t                     error_times;       /*!< The current error times of channel */
    udma_peripheral_t           peripheral;        /*!< Channel connected peripheral */
    udma_peripheral_feature_t   periph_feature;    /*!< Channel connected peripheral feature */
    udma_ch_config_adding_fsm_t configs_fsm;       /*!< UDMA channel configuration adding FSM */
} udma_ch_config_control;

/** UDMA configuation adding FSM state transit handler
 */
typedef void (*udma_configs_adding_fsm_transit_handler)(udma_channel_t ch, udma_ch_configs_adding_state current_state);

/** UDMA configuation adding FSM state handler
 */
typedef errcode_t (*udma_configs_adding_fsm_handler)(udma_channel_t ch, hal_udma_ch_configuration_t *cfg);

/**
 * @brief Initialize the UDMA Controller
 */
void hal_udma_init(void);

/**
 * @brief Deinitialise the UDMA controller
 */
void hal_udma_deinit(void);

/**
 * @brief Claim the UDMA controller
 * @return OK if the controller claim succeeded or corresponding error code otherwise
 */
errcode_t hal_udma_claim(void);

/**
 * @brief Check wheher the UDMA controller had been claimed
 * @return true if the controller had been claimed or the error code otherwise
 */
bool hal_udma_is_claimed(void);

/**
 * @brief Register the transfer prepare callback of the driver for the peripheral
 *        that the UDMA channel associated with
 * @param ch UDMAC channel ID
 * @param callback the transfer prepare callback of the peripheral driver
 */
void hal_udma_ch_register_peripheral_prepare_callback(udma_channel_t ch,
                                                      hal_udma_transfer_peripheral_prepare_callback_t callback);

/**
 * @brief Deregister the transfer prepare callback of the driver for the peripheral
 *         that the UDMA channel associated with
 * @param ch UDMAC channel ID
 */
void hal_udma_ch_deregister_peripheral_prepare_callback(udma_channel_t ch);

/**
 * @brief Get a free previously claimed UDMA Controller's channel for data transfer.
 * @param ch the UDMAC channel ID
 * @param pri the minnum priority of the expected opened channel
 * @param periph the peripheral ID that would associated with that channel
 * @return true if the channle opened succeeded or false otherwise
 */
bool hal_udma_ch_open(udma_channel_t *ch, udma_ch_priority_t pri, udma_peripheral_t periph);

/**
 * @brief Disable the specific UDMA Channel and set the channel's state to closed
 * @param ch the UDMAC channel ID
 */
void hal_udma_ch_close(udma_channel_t ch);

/**
 * @brief Check whether the UDMA channel is closed
 * @param ch UDMA channel ID
 * @return true if the channel is closed or false otherwise
 */
bool hal_udma_ch_is_closed(udma_channel_t ch);

/**
 * @brief Check whether the UDMA channel is opened
 * @param ch UDMA channel ID
 * @return true if the channel is opened or false otherwise
 */
bool hal_udma_ch_is_opened(udma_channel_t ch);

/**
 * @brief Configure the UDMA channel configuration feature related to the peripheral associated
 * @param ch UDMA channel ID
 * @param feature UDMA channel peripheral feature
 */
void hal_udma_ch_configure_peripheral_feature(udma_channel_t ch, const hal_udma_ch_periph_feature_t *feature);

/**
 * @brief Get the UDMA channel state
 * @param ch UDMA channel ID
 * @return the conrresponding UDMA channel state
 */
hal_udma_ch_state_t hal_udma_ch_state_get(udma_channel_t ch);

/**
 * @brief Set the UDMA channel state
 * @param ch UDMA channel ID
 * @param state setting channel state
 */
void hal_udma_ch_state_set(udma_channel_t ch, hal_udma_ch_state_t state);

/**
 * @brief Initialise the UDMA channel configuration buffer
 * @param ch UDMA channel ID
 */
errcode_t hal_udma_ch_config_buffer_init(udma_channel_t ch);

/**
 * @brief De-initialise the UDMA channel configuration buffer
 * @param ch UDMA channel ID
 */
void hal_udma_ch_config_buffer_deinit(udma_channel_t ch);

/**
 * @brief Get the N-th free config from the config buffer
 * @param ch UDMA channel ID
 * @param idx   The index of the free configs in the config buffer which start
 *              from Index 0 for the first free one, Index 1 for the second free one, and so on
 * @return Pointer to the N-th free config in the config buffer
 */
hal_udma_ch_configuration_t *hal_udma_ch_config_buffer_get_free_config(udma_channel_t ch, uint8_t idx);

/**
 * @brief Get the first used config in the config buffer
 * @param ch UDMA channel ID
 * @return The pointer to the first used config in the config buffer
 */
hal_udma_ch_configuration_t *hal_udma_ch_config_buffer_get_first_used(udma_channel_t ch);

/**
 * @brief Add a used(configured) config to the config buffer
 * @param ch The UDMA channel ID
 */
void hal_udma_ch_config_buffer_add_config(udma_channel_t ch);

/**
 * @brief Remove N used configs from the config buffer
 * @param ch The UDMA channel ID
 * @param dropped_count The number of removed used configs
 */
void hal_udma_ch_config_buffer_remove_config(udma_channel_t ch, uint8_t dropped_count);

/**
 * @brief Check whether the UDMA channel transfer configuration is OK
 * @param config UDMAC channel transfer configuration
 * @return true if the configuration is OK or false otherwise
 */
bool hal_udma_check_ch_transfer_config(const hal_udma_ch_configuration_t *config);

/**
 * @brief The core function to start transfer the data through the UDMA channel
 * @param ch UDMA channel ID
 * @param cfg UDMA channel transfer configuration
 * @return true if the transfer starting succeeded or false otherwise
 */
bool hal_udma_transfer(udma_channel_t ch, hal_udma_ch_configuration_t *cfg);

/**
 * @brief Free the specific UDMA channel's scatter_gather alternate configuration buffer in memory
 * @param ch UDMA channel ID
 */
void hal_udma_ch_scatter_gather_alternate_config_buffer_free(udma_channel_t ch);

/**
 * @brief Allocate the specific UDMA channel's scatter_gather alternate configuration buffer in memory
 * @param ch UDMA channel ID
 */
errcode_t hal_udma_ch_scatter_gather_alternate_config_buffer_alloc(udma_channel_t ch);

/**
 * @brief Configure the UDMA interrupt callback function and turn on/off the corresponding interrput
 *        based on the arguments
 * @param ch DAM channel ID
 * @param interrupt_type which type of interrupt to be configured
 * @param callback the configured callback function
 * @param initial_state determine to turn on or turn off the interrupt
 */
void hal_udma_configure_interrupt(udma_channel_t ch, hal_udma_interrupt_t interrupt_type,
                                  hal_udma_isr_callback_t callback);

/**
 * @brief Clear the UDMA channel interrupt status in the register
 * @param ch UDMA channel ID
 * @param interrupt_type which type of interrupt to be cleared
 */
void hal_udma_clear_interrupt(udma_channel_t ch, hal_udma_interrupt_t interrupt_type);

/**
 * @brief Set the priority level for the corresponding UDMA channel
 * @param ch UDMA channel ID
 * @param priority_level priority level
 */
void hal_udma_ch_set_priority_level(udma_channel_t ch, hal_udma_ch_priority_level_t priority_level);

/**
 * @brief Select the primary data structure for the corresponding UDMA channel
 * @param ch UDMA channel ID
 */
void hal_udma_ch_select_primary(udma_channel_t ch);

/**
 * @brief Disable a specific UDMA channel
 * @param ch UDMA channel ID
 */
void hal_udma_disable_ch(udma_channel_t ch);

/**
 * @brief Check whether a specific UDMA channel is enabled
 * @param ch UDMA channel ID
 * @return true if the UDMA channel is enabled or false otherwise
 */
bool hal_udma_ch_is_enabled(udma_channel_t ch);

/**
 * @brief Check whethe the specific UDMA channel is busy
 * @param ch UDMA channel ID
 * @return ture if the UDMA channel is busy or false otherwise
 */
bool hal_udma_ch_is_busy(udma_channel_t ch);

/**
 * @brief The interrupt handler function of UDMA done
 */
void hal_irq_udma_handler(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

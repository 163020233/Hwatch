/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides gpio port template \n
 *
 * History: \n
 * 2022-07-26， Create file. \n
 */
#include "gpio_porting.h"
#include "hal_gpio_v150.h"
#include "platform_types.h"
#include "platform_core.h"
#include "chip_core_irq.h"
#include "chip_io.h"
#include "arch_port.h"
#include "soc_osal.h"

#define GPIO_CHANNEL_0_BASE_ADDR GPIO0_BASE_ADDR
#if (GPIO_CHANNELS_NUM > 1)
#define GPIO_CHANNEL_1_BASE_ADDR GPIO1_BASE_ADDR
#endif
#if (GPIO_CHANNELS_NUM > 2)
#define GPIO_CHANNEL_2_BASE_ADDR GPIO2_BASE_ADDR
#endif
#if (GPIO_CHANNELS_NUM > 3)
#define GPIO_CHANNEL_3_BASE_ADDR GPIO3_BASE_ADDR
#endif
#if (GPIO_CHANNELS_NUM > 4)
#define GPIO_CHANNEL_ULP_BASE_ADDR ULP_GPIO_BASE_ADDR
#endif

static uintptr_t const g_gpio_base_addrs[GPIO_CHANNEL_MAX_NUM] = {
    (uintptr_t)GPIO_CHANNEL_0_BASE_ADDR,
#if (GPIO_CHANNELS_NUM > 1)
    (uintptr_t)GPIO_CHANNEL_1_BASE_ADDR,
#endif
#if (GPIO_CHANNELS_NUM > 2)
    (uintptr_t)GPIO_CHANNEL_2_BASE_ADDR,
#endif
#if (GPIO_CHANNELS_NUM > 3)
    (uintptr_t)GPIO_CHANNEL_3_BASE_ADDR,
#endif
#if (GPIO_CHANNELS_NUM > 4)
    (uintptr_t)GPIO_CHANNEL_ULP_BASE_ADDR,
#endif
};

static pin_to_gpio_t const g_pin2gpio_idx_map[GPIO_PIN_NUM] = {
#if (CORE == APPS)
    {S_HGPIO0, S_EGPIO0 + MEM_HS_GPIO_12},
    {S_HGPIO1, S_EGPIO0 + MEM_HS_GPIO_13},
    {S_HGPIO2, S_EGPIO0 + MEM_HS_GPIO_14},
    {S_HGPIO3, S_EGPIO0 + MEM_HS_GPIO_15},
    {S_HGPIO4, S_EGPIO0 + MEM_HS_GPIO_16},
    {S_HGPIO5, S_EGPIO0 + MEM_HS_GPIO_17},
    {S_HGPIO6, S_EGPIO0 + MEM_HS_GPIO_18},
    {S_HGPIO7, S_EGPIO0 + MEM_HS_GPIO_19},
    {S_HGPIO8, S_EGPIO0 + MEM_HS_GPIO_20},
    {S_HGPIO9, S_EGPIO0 + MEM_HS_GPIO_21},
    {S_HGPIO10, S_EGPIO0 + MEM_HS_GPIO_22},
    {S_HGPIO11, S_EGPIO0 + MEM_HS_GPIO_23},
    {S_HGPIO12, S_EGPIO0 + MEM_HS_GPIO_24},
    {S_HGPIO13, S_EGPIO0 + MEM_HS_GPIO_25},
    {S_HGPIO14, S_EGPIO0 + MEM_HS_GPIO_26},
    {S_HGPIO15, S_EGPIO0 + MEM_HS_GPIO_27},
#endif
};

#if defined(CONFIG_GPIO_SELECT_CORE)
static pin_to_core_sel_t const g_pin2core_select_map[] = {
    /* mcore/bt_core */
    {S_AGPIO0, SEL_BIT_0},
    {S_AGPIO1, SEL_BIT_1},
    {S_AGPIO2, SEL_BIT_2},
    {S_AGPIO3, SEL_BIT_3},
    {S_AGPIO4, SEL_BIT_4},
    {S_AGPIO5, SEL_BIT_5},
    {S_AGPIO6, SEL_BIT_6},
    {S_AGPIO7, SEL_BIT_7},
    {S_AGPIO8, SEL_BIT_8},
    {S_AGPIO9, SEL_BIT_9},
    {S_AGPIO10, SEL_BIT_10},
    {S_AGPIO11, SEL_BIT_11},
    {S_AGPIO12, SEL_BIT_12},
    {S_AGPIO13, SEL_BIT_13},
    {S_AGPIO14, SEL_BIT_14},
    {S_AGPIO15, SEL_BIT_15},
    {S_AGPIO16, SEL_BIT_16},
    {S_AGPIO17, SEL_BIT_17},
    {S_AGPIO18, SEL_BIT_18},
    {S_AGPIO19, SEL_BIT_19},
    /* mcore/dsp_core */
    {S_AGPIO20, SEL_BIT_0},
    {S_AGPIO21, SEL_BIT_1},
    {S_AGPIO22, SEL_BIT_2},
    {S_AGPIO23, SEL_BIT_3},
    {S_AGPIO24, SEL_BIT_4},
    {S_AGPIO25, SEL_BIT_5},
    {S_AGPIO26, SEL_BIT_6},
    {S_AGPIO27, SEL_BIT_7},
    {S_AGPIO28, SEL_BIT_8},
    {S_AGPIO29, SEL_BIT_9},
    {S_AGPIO30, SEL_BIT_10},
    {S_AGPIO31, SEL_BIT_11},
    {S_AGPIO32, SEL_BIT_12},
    {S_AGPIO33, SEL_BIT_13},
    {S_AGPIO34, SEL_BIT_14},
};
#endif

uint8_t g_pins_of_srvcore[GROUP_MAX_PIN_NUM][SRV_CHN_MAX] = {
};

uint32_t g_srvcore_base_addr[SRV_CHN_MAX] = {
    BT_GPIO_BASE_ADDR
};

/**
 * @brief  GPIO group info table.
 *         Developer should adapt GPIO info here.
 * @note   This is const information and does not change during code running.
 */
#if (CORE == APPS)
static hal_gpio_group_info_t const g_gpio_channel_0_group_info[GPIO_CHANNEL_0_GROUP_NUM] = {
    // S_EGPIO0 ~ S_EGPIO11, S_HGPIO0 ~ S_HGPIO15.
    {
        .start_pin_id = S_EGPIO0,
        .pin_num = GPIO_CHANNEL_0_GROUP_0_PIN_NUM,
        .start_callback_id = GPIO_CHANNEL_0_GROUP_0_CB_START_ID,
    },
};

static hal_gpio_group_info_t const g_gpio_channel_1_group_info[GPIO_CHANNEL_1_GROUP_NUM] = {
    // S_MGPIO0 ~ S_MGPIO26.
    {
        .start_pin_id = S_MGPIO0,
        .pin_num = GPIO_CHANNEL_1_GROUP_0_PIN_NUM,
        .start_callback_id = GPIO_CHANNEL_1_GROUP_0_CB_START_ID,
    },
};


static hal_gpio_group_info_t const g_gpio_channel_2_group_info[GPIO_CHANNEL_2_GROUP_NUM] = {
    // S_AGPIO0 ~ S_AGPIO31.
    {
        .start_pin_id = S_AGPIO0,
        .pin_num = GPIO_CHANNEL_2_GROUP_0_PIN_NUM,
        .start_callback_id = GPIO_CHANNEL_2_GROUP_0_CB_START_ID,
    },
};

static hal_gpio_group_info_t const g_gpio_channel_3_group_info[GPIO_CHANNEL_3_GROUP_NUM] = {
    // S_AGPIO32 ~ S_AGPIO34.
    {
        .start_pin_id = S_AGPIO32,
        .pin_num = GPIO_CHANNEL_3_GROUP_0_PIN_NUM,
        .start_callback_id = GPIO_CHANNEL_3_GROUP_0_CB_START_ID,
    },
};

static hal_gpio_group_info_t const g_gpio_channel_4_group_info[GPIO_CHANNEL_4_GROUP_NUM] = {
    // S_UGPIO0 ~ S_UGPIO1.
    {
        .start_pin_id = S_UGPIO0,
        .pin_num = GPIO_CHANNEL_4_GROUP_0_PIN_NUM,
        .start_callback_id = GPIO_CHANNEL_4_GROUP_0_CB_START_ID,
    },
};
#endif

#if (CORE == BT)
static hal_gpio_group_info_t const g_gpio_channel_0_group_info[GPIO_CHANNEL_0_GROUP_NUM] = {
    //  S_AGPIO0 ~ S_AGPIO19.
    {
        .start_pin_id = S_AGPIO0,
        .pin_num = GPIO_CHANNEL_0_GROUP_0_PIN_NUM,
        .start_callback_id = GPIO_CHANNEL_0_GROUP_0_CB_START_ID,
    },
};
#endif

/**
 * @brief  GPIO channel info table.
 *         Developer should adapt GPIO info here.
 * @note   This is const information and does not change during code running.
 */
#if (CORE == APPS)
hal_gpio_channel_info_t const g_gpio_channel_info_list[GPIO_CHANNEL_MAX_NUM] = {
    // memhs channel0, SS_EGPIO0 ~ S_EGPIO11, S_HGPIO0 ~ S_HGPIO15.
    {
        .start_pin_id = S_EGPIO0,
        .pin_num = GPIO_CHANNEL_0_GROUP_0_PIN_NUM,
        .group_num = GPIO_CHANNEL_0_GROUP_NUM,
        .irq_num = MEM_HS_GPIO_INT, //
        .base_addr = (uintptr_t)GPIO0_BASE_ADDR,
        .group_list = (hal_gpio_group_info_t *)g_gpio_channel_0_group_info,
    },
    // mgpio channel1, S_MGPIO0 ~ S_MGPIO26.
    {
        .start_pin_id = S_MGPIO0,
        .pin_num = GPIO_CHANNEL_1_GROUP_0_PIN_NUM,
        .group_num = GPIO_CHANNEL_1_GROUP_NUM,
        .irq_num = M_GPIO_IRQN,
        .base_addr = (uintptr_t)GPIO1_BASE_ADDR,
        .group_list = (hal_gpio_group_info_t *)g_gpio_channel_1_group_info,
    },
    //  agpio0 channel, S_AGPIO0 ~ S_AGPIO31.
    {
        .start_pin_id = S_AGPIO0,
        .pin_num = GPIO_CHANNEL_2_GROUP_0_PIN_NUM,
        .group_num = GPIO_CHANNEL_2_GROUP_NUM,
        .irq_num = GPIO_0_IRQN,
        .base_addr = (uintptr_t)GPIO2_BASE_ADDR,
        .group_list = (hal_gpio_group_info_t *)g_gpio_channel_2_group_info,
    },
    // agpio1 channel, S_AGPIO32 ~ S_AGPIO34.
    {
        .start_pin_id = S_AGPIO32,
        .pin_num = GPIO_CHANNEL_3_GROUP_0_PIN_NUM,
        .group_num = GPIO_CHANNEL_3_GROUP_NUM,
        .irq_num = GPIO_1_IRQN,
        .base_addr = (uintptr_t)GPIO3_BASE_ADDR,
        .group_list = (hal_gpio_group_info_t *)g_gpio_channel_3_group_info,
    },
    // aupio1 channe, S_UGPIO0 ~ S_UGPIO1.
    {
        .start_pin_id = S_UGPIO0,
        .pin_num = GPIO_CHANNEL_4_GROUP_0_PIN_NUM,
        .group_num = GPIO_CHANNEL_4_GROUP_NUM,
        .irq_num = ULP2MCU_INT,
        .base_addr = (uintptr_t)ULP_GPIO_BASE_ADDR,
        .group_list = (hal_gpio_group_info_t *)g_gpio_channel_4_group_info,
    },

};
#elif (CORE == BT)
static hal_gpio_channel_info_t const g_gpio_channel_info_list[GPIO_CHANNEL_MAX_NUM] = {
    // channel0,S_AGPIO0 ~ S_AGPIO19.
    {
        .start_pin_id = S_AGPIO0,
        .pin_num = GPIO_CHANNEL_0_GROUP_0_PIN_NUM,
        .group_num = GPIO_CHANNEL_0_GROUP_NUM,
        .irq_num = GPIO_BCPU_0_IRQN,
        .base_addr = (uintptr_t)GPIO0_BASE_ADDR,
        .group_list = (hal_gpio_group_info_t *)g_gpio_channel_0_group_info,
    },
};
#else
static hal_gpio_channel_info_t const g_gpio_channel_info_list[GPIO_CHANNEL_MAX_NUM] = {};
#endif

/**
 * @brief  GPIO context info table.
 *         Developer should adapt channel number, and group number of each channel here.
 */
static hal_gpio_group_context_t g_gpio_channel_0_group_context[GPIO_CHANNEL_0_GROUP_NUM] = {0};
#if (GPIO_CHANNELS_NUM > 1)
static hal_gpio_group_context_t g_gpio_channel_1_group_context[GPIO_CHANNEL_1_GROUP_NUM] = {0};
#endif
#if (GPIO_CHANNELS_NUM > 2)
static hal_gpio_group_context_t g_gpio_channel_2_group_context[GPIO_CHANNEL_2_GROUP_NUM] = {0};
#endif
#if (GPIO_CHANNELS_NUM > 3)
static hal_gpio_group_context_t g_gpio_channel_3_group_context[GPIO_CHANNEL_3_GROUP_NUM] = {0};
#endif
#if (GPIO_CHANNELS_NUM > 4)
static hal_gpio_group_context_t g_gpio_channel_4_group_context[GPIO_CHANNEL_4_GROUP_NUM] = {0};
#endif
static hal_gpio_group_context_t const *g_gpio_channel_context_list[GPIO_CHANNEL_MAX_NUM] = {
    g_gpio_channel_0_group_context,
#if (GPIO_CHANNELS_NUM > 1)
    g_gpio_channel_1_group_context,
#endif
#if (GPIO_CHANNELS_NUM > 2)
    g_gpio_channel_2_group_context,
#endif
#if (GPIO_CHANNELS_NUM > 3)
    g_gpio_channel_3_group_context,
#endif
#if (GPIO_CHANNELS_NUM > 4)
    g_gpio_channel_4_group_context,
#endif
};

hal_gpio_channel_info_t *gpio_porting_channel_info_get(uint32_t channel)
{
    return (hal_gpio_channel_info_t *)&g_gpio_channel_info_list[channel];
}

hal_gpio_group_context_t *gpio_porting_group_context_get(uint32_t channel, uint32_t group)
{
    hal_gpio_group_context_t *channel_info = (hal_gpio_group_context_t *)g_gpio_channel_context_list[channel];
    return &channel_info[group];
}

void gpio_porting_channel_context_clean(uint32_t channel, uint32_t group_num)
{
    memset_s((void *)(g_gpio_channel_context_list[channel]), sizeof(hal_gpio_group_context_t) * group_num, 0,
        sizeof(gpio_callback_t) * group_num);
}

uintptr_t gpio_porting_base_addr_get(gpio_channel_t channel)
{
    return g_gpio_base_addrs[channel];
}

uint8_t gpio_index_trans_by_pin(uint8_t pin_index)
{
    for (uint8_t i = 0; i < sizeof(g_pin2gpio_idx_map) / sizeof(pin_to_gpio_t); i++) {
        if (g_pin2gpio_idx_map[i].pin_index == pin_index) {
            return g_pin2gpio_idx_map[i].gpio_index;
        }
    }
    return pin_index;
}

uint8_t gpio_porting_index_trans_by_gpio(uint8_t gpio_index)
{
    for (uint8_t i = 0; i < sizeof(g_pin2gpio_idx_map) / sizeof(pin_to_gpio_t); i++) {
        if (g_pin2gpio_idx_map[i].gpio_index == gpio_index) {
            return g_pin2gpio_idx_map[i].pin_index;
        }
    }
    return gpio_index;
}

#if defined(CONFIG_GPIO_SELECT_CORE)
static uint32_t gpio_porting_select_core_get_addr(pin_t pin, cores_t core)
{
    if (((pin >= S_AGPIO0) && (pin <= S_AGPIO19))) {
        if ((core == CORES_APPS_CORE) || (core == CORES_BT_CORE)) {
            return GPIO_BT_CORE_SEL_ADDR;
        }
    }

    return GPIO_INVALID_CORE_SEL_ADDR;
}


void gpio_select_core(pin_t pin, cores_t core)
{
#if CORE == APPS
    /* 校验pin管脚和core类型是否合法并获取对应寄存器地址 */
    uint32_t core_sel_addr = gpio_porting_select_core_get_addr(pin, core);
    if (core_sel_addr == GPIO_INVALID_CORE_SEL_ADDR) {
        return;
    }

    for (uint8_t i = 0; i < sizeof(g_pin2core_select_map) / sizeof(pin_to_core_sel_t); i++) {
        if (g_pin2core_select_map[i].pin_index == pin) {
            if (core == CORES_APPS_CORE) {
                reg16_clrbit(core_sel_addr, g_pin2core_select_map[i].core_sel);
            } else {
                reg16_setbit(core_sel_addr, g_pin2core_select_map[i].core_sel);
            }
            return;
        }
    }
#else
    unused(pin);
    unused(core);
#endif
}
#endif

static void gpio_port_service_core_set_dir(uint32_t reg_addr, uint8_t dir, uint8_t pos)
{
    if (dir == GPIO_DIRECTION_INPUT) {
        reg32_setbit(reg_addr + GPIO_DIRECTION_REG_OFFSET, pos);
    } else {
        reg32_clrbit(reg_addr + GPIO_DIRECTION_REG_OFFSET, pos);
    }
}

static void gpio_port_service_core_set_val(uint32_t reg_addr, uint8_t val, uint8_t pos)
{
    if (val == GPIO_LEVEL_HIGH) {
        reg32_setbit(reg_addr + GPIO_HIGN_LEVEL_REG_OFFSET, pos);
    } else {
        reg32_setbit(reg_addr + GPIO_LOW_LEVEL_REG_OFFSET, pos);
    }
}

void gpio_porting_service_core_handle(pin_t pin, srv_core_gpio_optype_t op, uint8_t val)
{
    uint8_t pos, chn;

    if (pin >= PIN_NONE || op >= OP_BUTT) {
        return;
    }

    for (pos = 0; pos < GROUP_MAX_PIN_NUM; pos++) {
        for (chn = 0; chn < SRV_CHN_MAX; chn++) {
            if (pin == g_pins_of_srvcore[pos][chn]) {
                break;
            }
        }
        if (chn != SRV_CHN_MAX) {
            break;
        }
    }

    if (pos == GROUP_MAX_PIN_NUM && chn == SRV_CHN_MAX) {
        return;
    }

    switch (op) {
        case OP_SET_DIR:
            gpio_port_service_core_set_dir(g_srvcore_base_addr[chn], val, pos);
            return;
        case OP_SET_LEVEL:
            gpio_port_service_core_set_val(g_srvcore_base_addr[chn], val, pos);
            return;
        default:
            return;
    }
}

void gpio_ulp_int_en(bool on)
{
    UNUSED(on);
}

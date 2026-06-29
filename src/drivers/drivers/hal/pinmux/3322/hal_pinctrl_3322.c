/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides HAL pinctrl \n
 *
 * History: \n
 * 2022-08-29, Create file. \n
 */

#include <stdint.h>
#include "common_def.h"
#include "errcode.h"
#include "pinctrl_porting.h"
#include "hal_pinctrl_3322.h"

#define HAL_PIN_PINMUX_PINMUX_CTL_ADDR 0x57009000

/* PINMUX_CTL */
#define HAL_PAD_S_AGPIO_CFG_BASE_ADDR HAL_PIN_PINMUX_PINMUX_CTL_ADDR
#define HAL_PAD_S_EGPIO_CFG_BASE_ADDR (HAL_PIN_PINMUX_PINMUX_CTL_ADDR + 0x400)
#define HAL_PAD_S_HGPIO_CFG_BASE_ADDR (HAL_PIN_PINMUX_PINMUX_CTL_ADDR + 0x800)
#define HAL_PAD_S_MGPIO_CFG_BASE_ADDR (HAL_PIN_PINMUX_PINMUX_CTL_ADDR + 0xC00)
#define HAL_PAD_S_UGPIO_CFG_BASE_ADDR (0x5701A110)

/* MEM_PINMUX */
#define HAL_S_AGPIO_MODE_BASE_ADDR 0x5700A400

/* MEM_PINMUX */
#define HAL_S_EGPIO_MODE_BASE_ADDR 0x520F0000

/* S_HGPIO */
#define HAL_S_HGPIO_MODE_BASE_ADDR 0x52064000

/* S_MGPIO */
#define HAL_S_MGPIO_MODE_BASE_ADDR 0x52008000

/* AON_CTL */
#define HAL_ULP_GPIO_MODE_BASE_ADDR 0x5701A100

#define HAL_PIN_PINMUX_ULP_GPIO_4_POS 0
#define HAL_PIN_PINMUX_ULP_GPIO_5_POS 1
#define HAL_PIN_PINMUX_ULP_GPIO_6_POS 2

#define HAL_PIN_3322_MODE_CONTROL_CONFIG_BITS 4U
#define HAL_PIN_3322_DS_CFG_BITS 2
#define HAL_PIN_3322_IE_CFG_BIT 2

#define PIN_PULL_UP_VALUE 1
#define PIN_PULL_DOWN_VALUE 2
#define PIN_PULL_STATE_BITS 2

/**
 * @brief  PIN config dirver strength addr map.
 */
typedef struct {
    pin_t begin;      /* !< The first pin of each group. */
    pin_t end;        /* !< The last pin of each group. */
    uint32_t addr;    /* !< Address of the group. */
    uint32_t reg_num; /* !< Configure the number of registers of the pin. */
    uint32_t reg_bit; /* !< The first bit to config. */
} hal_pin_ds_map_t;

/**
 * @brief  PIN config mode addr map.
 */
typedef struct {
    pin_t begin;       /* !< The first pin of each group. */
    pin_t end;         /* !< The last pin of each group. */
    uint32_t reg_addr; /* !< Address of the group. */
    uint32_t per_num;  /* !< Number of pins that can be configured by one register. */
} hal_pin_mode_map_t;

/**
 * @brief  PIN config pull addr map.
 */
typedef struct {
    pin_t begin;     /* !< The first pin of each group. */
    pin_t end;       /* !< The last pin of each group. */
    uint32_t addr;   /* !< Address of group. */
    uint32_t pu_bit; /* !< The pu to config. */
    uint32_t pd_bit; /* !< The pd bit to config. */
} hal_pin_pull_map_t;

typedef struct {
    pin_t begin;     /* !< The first pin of each group. */
    pin_t end;       /* !< The last pin of each group. */
    uint32_t addr;   /* !< Address of group. */
} hal_pin_map_t;

static hal_pin_mode_map_t const g_pin_mode_map[] = {
    { S_EGPIO0,  S_EGPIO31,    HAL_S_EGPIO_MODE_BASE_ADDR, 1 },
    { S_MGPIO0,  S_MGPIO26,    HAL_S_MGPIO_MODE_BASE_ADDR, 1 },
    { S_HGPIO0,  S_HGPIO15,    HAL_S_HGPIO_MODE_BASE_ADDR, 1 },
    { S_AGPIO0,  S_AGPIO34,   HAL_S_AGPIO_MODE_BASE_ADDR, 1 },
    { S_UGPIO0,  S_UGPIO1,   HAL_ULP_GPIO_MODE_BASE_ADDR, 1 },
};

static hal_pin_pull_map_t const g_pin_pull_map[] = {
    { S_EGPIO0,  S_EGPIO31,  HAL_PAD_S_EGPIO_CFG_BASE_ADDR, 0, 1},
    { S_MGPIO0,  S_MGPIO26,  HAL_PAD_S_MGPIO_CFG_BASE_ADDR, 0, 1},
    { S_HGPIO0,  S_HGPIO15,  HAL_PAD_S_HGPIO_CFG_BASE_ADDR, 0, 1},
    { S_AGPIO0,  S_AGPIO34,  HAL_PAD_S_AGPIO_CFG_BASE_ADDR, 0, 1},
    { S_UGPIO0,  S_UGPIO1,  HAL_PAD_S_UGPIO_CFG_BASE_ADDR, 0, 1},
};

static hal_pin_ds_map_t const g_pin_ds_map[] = {
    { S_EGPIO0,  S_EGPIO31,  HAL_PAD_S_EGPIO_CFG_BASE_ADDR,  1, 4 },
    { S_MGPIO0,  S_MGPIO26,  HAL_PAD_S_MGPIO_CFG_BASE_ADDR,  1, 4 },
    { S_HGPIO0,  S_HGPIO15,  HAL_PAD_S_HGPIO_CFG_BASE_ADDR,  1, 4 },
    { S_AGPIO0,  S_AGPIO34,  HAL_PAD_S_AGPIO_CFG_BASE_ADDR,  1, 4 },
    { S_UGPIO0,  S_UGPIO1,   HAL_PAD_S_UGPIO_CFG_BASE_ADDR,  1, 4 },
};

#if defined(CONFIG_PINCTRL_SUPPORT_SLP_IE)
static hal_pin_map_t const g_pin_ie_slp_val_map[] = {
    { S_EGPIO0,  S_EGPIO15,    GLB_CTL_M_RB_CFG_S_EGPIO0_CTRL_IE_0_REG },
    { S_EGPIO16, S_EGPIO31,    GLB_CTL_M_RB_CFG_S_EGPIO0_CTRL_IE_1_REG },
    { S_MGPIO0,  S_MGPIO15,    GLB_CTL_M_RB_CFG_S_MGPIO0_CTRL_IE_0_REG },
    { S_MGPIO16, S_MGPIO26,    GLB_CTL_M_RB_CFG_S_MGPIO0_CTRL_IE_1_REG },
    { S_HGPIO0,  S_HGPIO15,    GLB_CTL_M_RB_CFG_S_HGPIO0_CTRL_IE_0_REG },
    { S_AGPIO0,  S_AGPIO15,    GLB_CTL_M_RB_CFG_S_AGPIO0_CTRL_IE_0_REG },
    { S_AGPIO16, S_AGPIO31,    GLB_CTL_M_RB_CFG_S_AGPIO0_CTRL_IE_1_REG },
    { S_AGPIO32, S_AGPIO34,    GLB_CTL_M_RB_CFG_S_AGPIO0_CTRL_IE_2_REG },
};

static hal_pin_map_t const g_pin_oen_slp_val_map[] = {
    { S_EGPIO0,  S_EGPIO15,    GLB_CTL_D_RB_EGPIO_OE_VAL_0_REG },
    { S_EGPIO16, S_EGPIO31,    GLB_CTL_D_RB_EGPIO_OE_SEL_1_REG },
    { S_MGPIO0,  S_MGPIO15,    GLB_CTL_D_RB_MGPIO_OE_VAL_0_REG },
    { S_MGPIO16, S_MGPIO26,    GLB_CTL_D_RB_MGPIO_OE_VAL_1_REG },
    { S_HGPIO0,  S_HGPIO15,    GLB_CTL_D_RB_HGPIO_OE_VAL_0_REG },
    { S_AGPIO0,  S_AGPIO15,    GLB_CTL_D_RB_AON_OE_VAL_0_REG },
    { S_AGPIO16, S_AGPIO31,    GLB_CTL_D_RB_AON_OE_VAL_1_REG },
    { S_AGPIO32, S_AGPIO34,    GLB_CTL_D_RB_AON_OE_VAL_2_REG },
};
#endif

static void hal_pin_get_mode_addr(hal_pin_mode_map_t **pin_map, uint32_t *map_size)
{
    *map_size = (uint32_t)array_size(g_pin_mode_map);
    *pin_map = (hal_pin_mode_map_t *)g_pin_mode_map;
}

static void hal_pin_get_pull_addr(hal_pin_pull_map_t **pin_map, uint32_t *map_size)
{
    *map_size = (uint32_t)array_size(g_pin_pull_map);
    *pin_map = (hal_pin_pull_map_t *)g_pin_pull_map;
}

static void hal_pin_get_ds_addr(hal_pin_ds_map_t **pin_map, uint32_t *map_size)
{
    *map_size = (uint32_t)array_size(g_pin_ds_map);
    *pin_map = (hal_pin_ds_map_t *)g_pin_ds_map;
}

#if defined(CONFIG_PINCTRL_SUPPORT_SLP_IE)
static void hal_pin_get_ie_slp_val_addr(hal_pin_map_t **pin_map, uint32_t *map_size)
{
    *map_size = (uint32_t)array_size(g_pin_ie_slp_val_map);
    *pin_map = (hal_pin_map_t *)g_pin_ie_slp_val_map;
}

static hal_pin_map_t *hal_pin_3322_get_ie_slp_val_addr(pin_t pin)
{
    hal_pin_map_t *pin_map = NULL;
    uint32_t map_size = 0;

    hal_pin_get_ie_slp_val_addr(&pin_map, &map_size);
    for (uint32_t i = 0; i < map_size; i++) {
        if ((pin >= pin_map[i].begin) && (pin <= pin_map[i].end)) {
            return (pin_map + i);
        }
    }
    return NULL;
}

static void hal_pin_get_oen_slp_val_addr(hal_pin_map_t **pin_map, uint32_t *map_size)
{
    *map_size = (uint32_t)array_size(g_pin_oen_slp_val_map);
    *pin_map = (hal_pin_map_t *)g_pin_oen_slp_val_map;
}

static hal_pin_map_t *hal_pin_3322_get_oen_slp_val_addr(pin_t pin)
{
    hal_pin_map_t *pin_map = NULL;
    uint32_t map_size = 0;

    hal_pin_get_oen_slp_val_addr(&pin_map, &map_size);
    for (uint32_t i = 0; i < map_size; i++) {
        if ((pin >= pin_map[i].begin) && (pin <= pin_map[i].end)) {
            return (pin_map + i);
        }
    }
    return NULL;
}
#endif

static hal_pin_mode_map_t *hal_pin_3322_get_aon_mode_addr(pin_t pin)
{
    hal_pin_mode_map_t *pin_map = NULL;
    uint32_t map_size = 0;

    hal_pin_get_mode_addr(&pin_map, &map_size);
    for (uint32_t i = 0; i < map_size; i++) {
        if ((pin >= pin_map[i].begin) && (pin <= pin_map[i].end)) {
            return (pin_map + i);
        }
    }
    return NULL;
}

static errcode_t hal_pin_3322_set_aon_mode(pin_t pin, pin_mode_t mode)
{
    uint32_t reg_addr, config_pos;
    hal_pin_mode_map_t *pin_map = NULL;

    pin_map = hal_pin_3322_get_aon_mode_addr(pin);
    if (pin_map == NULL) {
        return ERRCODE_FAIL;
    }
    reg_addr =
        (uintptr_t)pin_map->reg_addr + ((uint32_t)(pin - pin_map->begin) / pin_map->per_num) * (sizeof(uint32_t));
    config_pos = (((uint32_t)pin % pin_map->per_num) * HAL_PIN_3322_MODE_CONTROL_CONFIG_BITS);
    uapi_reg_setbits16(reg_addr, config_pos, HAL_PIN_3322_MODE_CONTROL_CONFIG_BITS, mode);
    return ERRCODE_SUCC;
}

static pin_mode_t hal_pin_3322_get_aon_mode(pin_t pin)
{
    uint32_t reg_addr, config_pos;
    hal_pin_mode_map_t *pin_map = NULL;

    pin_map = hal_pin_3322_get_aon_mode_addr(pin);
    if (pin_map == NULL) {
        return PIN_MODE_MAX;
    }
    reg_addr =
        (uintptr_t)pin_map->reg_addr + ((uint32_t)(pin - pin_map->begin) / pin_map->per_num) * (sizeof(uint32_t));
    config_pos = (((uint32_t)pin % pin_map->per_num) * HAL_PIN_3322_MODE_CONTROL_CONFIG_BITS);
    return (pin_mode_t)uapi_reg_getbits16(reg_addr, config_pos, HAL_PIN_3322_MODE_CONTROL_CONFIG_BITS);
}

static hal_pin_ds_map_t *hal_pin_3322_get_aon_ds_addr(pin_t pin)
{
    hal_pin_ds_map_t *pin_map = NULL;
    uint32_t map_size = 0;

    hal_pin_get_ds_addr(&pin_map, &map_size);
    for (uint32_t i = 0; i < map_size; i++) {
        if ((pin >= pin_map[i].begin) && (pin <= pin_map[i].end)) {
            return (pin_map + i);
        }
    }
    return NULL;
}

static errcode_t hal_pin_3322_set_aon_ds(pin_t pin, pin_drive_strength_t ds)
{
    uint32_t config_pos = 0;
    uint32_t reg_addr;
    hal_pin_ds_map_t *pin_map = NULL;

    pin_map = hal_pin_3322_get_aon_ds_addr(pin);
    if (pin_map == NULL) {
        return ERRCODE_FAIL;
    }
    reg_addr = (uintptr_t)pin_map->addr + ((uint32_t)pin - (uint32_t)(pin_map->begin)) * (sizeof(uint32_t));
    config_pos = pin_map->reg_bit;
    uapi_reg_setbits16(reg_addr, config_pos, HAL_PIN_3322_DS_CFG_BITS, ds);
    return ERRCODE_SUCC;
}

static pin_drive_strength_t hal_pin_3322_get_aon_ds(pin_t pin)
{
    uint32_t config_pos = 0;
    uint32_t reg_addr;
    hal_pin_ds_map_t *pin_map = NULL;

    pin_map = hal_pin_3322_get_aon_ds_addr(pin);
    if (pin_map == NULL) {
        return PIN_DS_MAX;
    }
    reg_addr = (uintptr_t)pin_map->addr + ((uint32_t)pin - (uint32_t)(pin_map->begin)) * (sizeof(uint32_t));
    config_pos = pin_map->reg_bit;

    return (pin_drive_strength_t)uapi_reg_getbits16(reg_addr, config_pos, HAL_PIN_3322_DS_CFG_BITS);
}

static hal_pin_pull_map_t *hal_pin_3322_get_pull_addr(pin_t pin)
{
    hal_pin_pull_map_t *pin_map = NULL;
    uint32_t map_size = 0;

    hal_pin_get_pull_addr(&pin_map, &map_size);
    for (uint32_t i = 0; i < map_size; i++) {
        if ((pin >= pin_map[i].begin) && (pin <= pin_map[i].end)) {
            return (pin_map + i);
        }
    }
    return NULL;
}

static errcode_t hal_pin_3322_set_aon_pull(pin_t pin, pin_pull_t pull_type)
{
    hal_pin_pull_map_t *pin_map = NULL;
    uint32_t pu_bit = 0;
    uint32_t reg_addr = 0;
    uint32_t pd_bit = 0;

    pin_map = hal_pin_3322_get_pull_addr(pin);
    if (pin_map == NULL) {
        return ERRCODE_FAIL;
    }
    reg_addr = (uintptr_t)pin_map->addr + (uint32_t)(pin - pin_map->begin) * (sizeof(uint32_t));
    pu_bit = pin_map->pu_bit;
    pd_bit = pin_map->pd_bit;

    switch (pull_type) {
        case PIN_PULL_NONE:
            uapi_reg_clrbit16(reg_addr, pu_bit);
            uapi_reg_clrbit16(reg_addr, pd_bit);
            break;
        case PIN_PULL_DOWN:
            uapi_reg_setbit16(reg_addr, pd_bit);
            uapi_reg_clrbit16(reg_addr, pu_bit);
            break;
        case PIN_PULL_UP:
            uapi_reg_setbit16(reg_addr, pu_bit);
            uapi_reg_clrbit16(reg_addr, pd_bit);
            break;
        default:
            return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static pin_pull_t hal_pin_3322_get_aon_pull(pin_t pin)
{
    hal_pin_pull_map_t *pin_map = NULL;
    uint32_t pu_bit = 0;
    uint32_t reg_addr = 0;

    pin_map = hal_pin_3322_get_pull_addr(pin);
    if (pin_map == NULL) {
        return PIN_PULL_MAX;
    }
    reg_addr = (uintptr_t)pin_map->addr + (uint32_t)(pin - pin_map->begin) * (sizeof(uint32_t));
    pu_bit = pin_map->pu_bit;

    if (uapi_reg_getbits16(reg_addr, pu_bit, PIN_PULL_STATE_BITS) == PIN_PULL_UP_VALUE) {
        return PIN_PULL_UP;
    } else if (uapi_reg_getbits16(reg_addr, pu_bit, PIN_PULL_STATE_BITS) == PIN_PULL_DOWN_VALUE) {
        return PIN_PULL_DOWN;
    } else {
        return PIN_PULL_NONE;
    }
}

static errcode_t hal_pin_3322_set_mode(pin_t pin, pin_mode_t mode)
{
    return hal_pin_3322_set_aon_mode(pin, mode);
}

static pin_mode_t hal_pin_3322_get_mode(pin_t pin)
{
    pin_mode_t mode;

    mode = hal_pin_3322_get_aon_mode(pin);
    return mode;
}

static errcode_t hal_pin_3322_set_ds(pin_t pin, pin_drive_strength_t ds)
{
    return hal_pin_3322_set_aon_ds(pin, ds);
}

static pin_drive_strength_t hal_pin_3322_get_ds(pin_t pin)
{
    return hal_pin_3322_get_aon_ds(pin);
}

static errcode_t hal_pin_3322_set_pull(pin_t pin, pin_pull_t pull_type)
{
    return hal_pin_3322_set_aon_pull(pin, pull_type);
}

static pin_pull_t hal_pin_3322_get_pull(pin_t pin)
{
    return hal_pin_3322_get_aon_pull(pin);
}

#if defined(CONFIG_PINCTRL_SUPPORT_IE)
STATIC errcode_t hal_pin_3322_set_ie(pin_t pin, pin_input_enable_t ie)
{
    hal_pin_pull_map_t *pin_map = NULL;
    uint32_t reg_addr = 0;

    pin_map = hal_pin_3322_get_pull_addr(pin);
    if (pin_map == NULL) {
        return ERRCODE_FAIL;
    }
    reg_addr = (uintptr_t)pin_map->addr + (uint32_t)(pin - pin_map->begin) * (sizeof(uint32_t));
    if (ie == PIN_IE_DISABLE) {
        reg16_clrbit(reg_addr, HAL_PIN_3322_IE_CFG_BIT);
    } else if (ie == PIN_IE_ENABLE) {
        reg16_setbit(reg_addr, HAL_PIN_3322_IE_CFG_BIT);
    }
    return ERRCODE_SUCC;
}

STATIC pin_input_enable_t hal_pin_3322_get_ie(pin_t pin)
{
    hal_pin_pull_map_t *pin_map = NULL;
    uint32_t reg_addr = 0;
    uint16_t ie_enable_bit = 0;

    pin_map = hal_pin_3322_get_pull_addr(pin);
    if (pin_map == NULL) {
        return PIN_IE_MAX;
    }
    reg_addr = (uintptr_t)pin_map->addr + (uint32_t)(pin - pin_map->begin) * (sizeof(uint32_t));
    ie_enable_bit = reg32_getbits(reg_addr, HAL_PIN_3322_IE_CFG_BIT, 1); // ie--bit1
    if (ie_enable_bit == 0) {
        return PIN_IE_DISABLE;
    } else if (ie_enable_bit == 1) {
        return PIN_IE_ENABLE;
    } else {
        return PIN_IE_MAX;
    }
}
#endif /* CONFIG_PINCTRL_SUPPORT_IE */

#if defined(CONFIG_PINCTRL_SUPPORT_SLP_IE)
STATIC errcode_t hal_pin_3322_set_slp_ie(pin_t pin, pin_input_enable_t ie)
{
    hal_pin_map_t *ie_pin_map = hal_pin_3322_get_ie_slp_val_addr(pin);
    hal_pin_map_t *oen_pin_map = hal_pin_3322_get_oen_slp_val_addr(pin);
    if ((ie_pin_map == NULL) || (oen_pin_map == NULL)) {
        return ERRCODE_FAIL;
    }

    if (ie == PIN_IE_DISABLE) {
        reg16_clrbit((uint32_t)ie_pin_map->addr, (uint32_t)(pin - ie_pin_map->begin));
        reg16_clrbit((uint32_t)oen_pin_map->addr, (uint32_t)(pin - oen_pin_map->begin));
    } else if (ie == PIN_IE_ENABLE) {
        reg16_setbit((uint32_t)ie_pin_map->addr, (uint32_t)(pin - ie_pin_map->begin));
        reg16_setbit((uint32_t)oen_pin_map->addr, (uint32_t)(pin - oen_pin_map->begin));
    }
    return ERRCODE_SUCC;
}

STATIC pin_input_enable_t hal_pin_3322_get_slp_ie(pin_t pin)
{
    hal_pin_map_t *ie_pin_map = hal_pin_3322_get_ie_slp_val_addr(pin);
    hal_pin_map_t *oen_pin_map = hal_pin_3322_get_oen_slp_val_addr(pin);
    if ((ie_pin_map == NULL) || (oen_pin_map == NULL)) {
        return PIN_IE_MAX;
    }

    uint16_t ie_slp_val = reg16_getbit((uint32_t)ie_pin_map->addr, (uint32_t)(pin - ie_pin_map->begin));
    uint16_t oen_slp_val = reg16_getbit((uint32_t)oen_pin_map->addr, (uint32_t)(pin - oen_pin_map->begin));
    if (ie_slp_val && oen_slp_val) {
        return PIN_IE_ENABLE;
    } else {
        return PIN_IE_DISABLE;
    }
}
#endif

static hal_pin_funcs_t g_hal_pin_3322_funcs = {
    .set_mode = hal_pin_3322_set_mode,
    .get_mode = hal_pin_3322_get_mode,
    .set_ds = hal_pin_3322_set_ds,
    .get_ds = hal_pin_3322_get_ds,
    .set_pull = hal_pin_3322_set_pull,
    .get_pull = hal_pin_3322_get_pull,
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    .set_ie = hal_pin_3322_set_ie,
    .get_ie = hal_pin_3322_get_ie,
#endif /* CONFIG_PINCTRL_SUPPORT_IE */
#if defined(CONFIG_PINCTRL_SUPPORT_SLP_IE)
    .set_slp_ie = hal_pin_3322_set_slp_ie,
    .get_slp_ie = hal_pin_3322_get_slp_ie,
#endif /* CONFIG_PINCTRL_SUPPORT_SLP_IE */
};

hal_pin_funcs_t *hal_pin_3322_funcs_get(void)
{
    return &g_hal_pin_3322_funcs;
}

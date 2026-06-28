/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides mmc UT \n
 *
 * History: \n
 * 2022-08-18， Create file. \n
 */

#include "common_def.h"
#include "errcode.h"
#include "chip_io.h"
#include "soc_reg.h"
#include "tcxo.h"
#include "mmc_porting.h"

#define TCXO_DELAY_400 400
#define TCXO_DELAY_10 10
#define TCXO_DELAY_50 50
#define TCXO_DELAY_240UL 240UL

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

#define HAL_PIN_PINMUX_PINMUX_CTL_ADDR 0x57009000

/* PINMUX_CTL */
#define HAL_PAD_S_AGPIO_CFG_BASE_ADDR HAL_PIN_PINMUX_PINMUX_CTL_ADDR
#define HAL_PAD_S_EGPIO_CFG_BASE_ADDR (HAL_PIN_PINMUX_PINMUX_CTL_ADDR + 0x400)
#define HAL_PAD_S_HGPIO_CFG_BASE_ADDR (HAL_PIN_PINMUX_PINMUX_CTL_ADDR + 0x800)
#define HAL_PAD_S_MGPIO_CFG_BASE_ADDR (HAL_PIN_PINMUX_PINMUX_CTL_ADDR + 0xC00)
#define HAL_PAD_S_UGPIO_CFG_BASE_ADDR (0x5701A110)

/**
 * @brief  Definition of mode-multiplexing.
 */
typedef enum { PIN_MODE_0 = 0, PIN_MODE_1 = 1, PIN_MODE_2 = 2, PIN_MODE_3 = 3, PIN_MODE_MAX } pin_mode_t;

/**
 * @brief  Definition of drive-strength.
 */
typedef enum {
    PIN_DS_0 = 0,
    PIN_DS_1 = 1,
    PIN_DS_2 = 2,
    PIN_DS_3 = 3,
    PIN_DS_4 = 3,
    PIN_DS_5 = 3,
    PIN_DS_6 = 3,
    PIN_DS_7 = 3,
    PIN_DS_8 = 3,
    PIN_DS_9 = 3,
    PIN_DS_10 = 3,
    PIN_DS_11 = 3,
    PIN_DS_12 = 3,
    PIN_DS_13 = 3,
    PIN_DS_14 = 3,
    PIN_DS_15 = 3,
    PIN_DS_MAX
} pin_drive_strength_t;

typedef enum {
    /* pinmux mode 0 funciton */
    HAL_PIO_FUNC_GPIO = 0,  // Default NON ULP GPIO
    HAL_PIO_FUNC_OPI = 0,

    /* pinmux mode 1 funciton */
    HAL_PIO_FUNC_QSPI0 = 1,
    HAL_PIO_FUNC_QSPI2 = 1,
    HAL_PIO_FUNC_EMMC = 1,
    HAL_PIO_FUNC_SDIO = 1,
    HAL_PIO_FUNC_BT_WIFI_SW = 1,
    HAL_PIO_FUNC_BT_ACT = 1,
    HAL_PIO_FUNC_BTS_SAMPLE = 1,
    HAL_PIO_FUNC_BT_FREQ = 1,
    HAL_PIO_FUNC_BT_STATUS = 1,
    HAL_PIO_FUNC_BT_LNA_EN = 1,
    HAL_PIO_FUNC_WLAN_ACT = 1,
    HAL_PIO_FUNC_UART3_M1 = 1,
    HAL_PIO_FUNC_CLKOUT0_M1 = 1,
    HAL_PIO_FUNC_CLKOUT1_M1 = 1,
    HAL_PIO_FUNC_UART4_M1 = 1,
    HAL_PIO_FUNC_PWM_M1 = 1,
    HAL_PIO_FUNC_GLP_SYNC_P = 1,
    HAL_PIO_FUNC_USB_VBUS = 1,
    HAL_PIO_FUNC_DSI_TE = 1,
    HAL_PIO_FUNC_CLKIN_M1 = 1,
    HAL_PIO_FUNC_I2S0 = 1,
    HAL_PIO_FUNC_I2S1 = 1,
    HAL_PIO_FUNC_PDM = 1,
    HAL_PIO_FUNC_CAN = 1,
    HAL_PIO_FUNC_UART0_M1 = 1,
    HAL_PIO_FUNC_I2C0_M1 = 1,
    HAL_PIO_FUNC_I2C1_M1 = 1,
    HAL_PIO_FUNC_I2C2_M1 = 1,
    HAL_PIO_FUNC_I2C3_M1 = 1,
    HAL_PIO_FUNC_SPI0_M1 = 1,
    HAL_PIO_FUNC_UART2_M1 = 1,
    HAL_PIO_FUNC_UART1_M1 = 1,
    HAL_PIO_FUNC_SPI1_M1 = 1,
    HAL_PIO_FUNC_SWD = 1,

    /* pinmux mode 2 funciton */
    HAL_PIO_FUNC_QSPI1_M2 = 2,
    HAL_PIO_FUNC_I2C0_M2 = 2,
    HAL_PIO_FUNC_I2C1_M2 = 2,
    HAL_PIO_FUNC_I2C2_M2 = 2,
    HAL_PIO_FUNC_I2C3_M2 = 2,
    HAL_PIO_FUNC_CLKIN_M2 = 1,
    HAL_PIO_FUNC_UART3_TXD_M2 = 1,
    HAL_PIO_FUNC_UART3_RXD_M2 = 1,
    HAL_PIO_FUNC_PWM_M2 = 1,
    HAL_PIO_FUNC_CLKOUT0_M2 = 1,
    HAL_PIO_FUNC_CLKOUT1_M2 = 1,
    HAL_PIO_FUNC_FEM_CTRL = 1,
    HAL_PIO_FUNC_UART1_M2 = 1,
    HAL_PIO_FUNC_SPI1_M2 = 1,
    HAL_PIO_FUNC_UART0_M2 = 1,

    /* pinmux mode 3 funciton */
    HAL_PIO_FUNC_DIAG = 3,  // DIAG[0] ~ DIAG[15]
    HAL_PIO_FUNC_USB_OVR_CUR = 3,
    HAL_PIO_FUNC_PWM_M3 = 3,
    HAL_PIO_FUNC_UART1_M3 = 3,
    HAL_PIO_FUNC_VICAP = 3,

    HAL_PIO_FUNC_MAX = 4,
    // input high resistance. Need config as HAL_PIO_FUNC_BT_GPIO and GPIO input mode
    HAL_PIO_FUNC_DEFAULT_HIGH_Z = 0xf,
} hal_pio_func_t;

typedef enum {
    HAL_PIO_PULL_NONE,  //!< No pull down or pull up enabled.
    HAL_PIO_PULL_DOWN,  //!< Pull down enabled for this pin.
    HAL_PIO_PULL_UP,    //!< Pull up enabled for this pin.
    HAL_PIO_PULL_MAX,
    HAL_PIO_PULL_DEFAULT = HAL_PIO_PULL_MAX,
} hal_pio_pull_t;

typedef enum {
    HAL_PIO_DRIVE_0 = 0,  //!< lowest pio current dirve strength.
    HAL_PIO_DRIVE_1 = 1,
    HAL_PIO_DRIVE_2 = 2,
    HAL_PIO_DRIVE_3 = 3,
    HAL_PIO_DRIVE_MAX,
    HAL_PIO_DRIVE_DEFAULT = HAL_PIO_DRIVE_MAX,
} hal_pio_drive_t;

typedef enum {
    HAL_PIO_IE_DISABLE = 0,
    HAL_PIO_IE_ENABLE = 1,
    HAL_PIO_IE_MAX,
    HAL_PIO_IE_DEFAULT = HAL_PIO_IE_MAX,
} hal_pio_ie_t;

/**
 * @brief  Definition of pull-up/pull-down.
 */
typedef enum { PIN_PULL_NONE = 0, PIN_PULL_DOWN = 1, PIN_PULL_UP = 2, PIN_PULL_MAX } pin_pull_t;

/**
 * @brief  Definition of input enable.
 */
typedef enum { PIN_IE_DISABLE = 0, PIN_IE_ENABLE = 1, PIN_IE_MAX } pin_input_enable_t;

typedef struct {
    hal_pio_func_t func;
    hal_pio_drive_t drive;
    hal_pio_pull_t pull;
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    hal_pio_ie_t ie;
#endif
} hal_pio_config_t;

/**
 * @addtogroup PRODUCT
 * @{
 */
/* *
 * @brief Cross Core Definition of pin.
 */
typedef enum {
    S_EGPIO0 = 0,    // QSPI0_CLK
    S_EGPIO1 = 1,    // QSPI0_CS
    S_EGPIO2 = 2,    // QSPI0_D0
    S_EGPIO3 = 3,    // QSPI0_D1
    S_EGPIO4 = 4,    // QSPI0_D2
    S_EGPIO5 = 5,    // QSPI0_D3
    S_EGPIO6 = 6,    // QSPI3_CLK
    S_EGPIO7 = 7,    // QSPI3_CS
    S_EGPIO8 = 8,    // QSPI3_D0
    S_EGPIO9 = 9,    // QSPI3_D1
    S_EGPIO10 = 10,  // QSPI3_D2
    S_EGPIO11 = 11,  // QSPI3_D3

    S_EGPIO12 = 12,  //
    S_EGPIO13 = 13,  //
    S_EGPIO14 = 14,  //
    S_EGPIO15 = 15,  //
    S_EGPIO16 = 16,  //
    S_EGPIO17 = 17,  //
    S_EGPIO18 = 18,  //
    S_EGPIO19 = 19,  //
    S_EGPIO20 = 20,  //
    S_EGPIO21 = 21,  //
    S_EGPIO22 = 22,  //
    S_EGPIO23 = 23,  //
    S_EGPIO24 = 24,  //
    S_EGPIO25 = 25,  //
    S_EGPIO26 = 26,  //
    S_EGPIO27 = 27,  //
    S_EGPIO28 = 28,  //
    S_EGPIO29 = 29,  //
    S_EGPIO30 = 30,  //
    S_EGPIO31 = 31,  //

    S_MGPIO0 = 32,   // UART0_TXD
    S_MGPIO1 = 33,   // UART0_RXD
    S_MGPIO2 = 34,   // UART0_CTS
    S_MGPIO3 = 35,   // UART0_RTS
    S_MGPIO4 = 36,   // I2C0_SCL
    S_MGPIO5 = 37,   // I2C0_SDA
    S_MGPIO6 = 38,   // I2C1_SCL
    S_MGPIO7 = 39,   // I2C1_SDA
    S_MGPIO8 = 40,   // I2C2_SCL
    S_MGPIO9 = 41,   // I2C2_SDA
    S_MGPIO10 = 42,  // I2C3_SCL
    S_MGPIO11 = 43,  // I2C3_SDA
    S_MGPIO12 = 44,  // SPI0_DI
    S_MGPIO13 = 45,  // SPI0_DO
    S_MGPIO14 = 46,  // SPI0_CLK
    S_MGPIO15 = 47,  // SPI0_CS
    S_MGPIO16 = 48,  // SPI0_CS1
    S_MGPIO17 = 49,  // UART2_TXD
    S_MGPIO18 = 50,  // UART2_RXD
    S_MGPIO19 = 51,  // UART1_TXD
    S_MGPIO20 = 52,  // UART1_RXD
    S_MGPIO21 = 53,  // UART1_CTS
    S_MGPIO22 = 54,  // UART1_RTS
    S_MGPIO23 = 55,  // SPI1_DI
    S_MGPIO24 = 56,  // SPI1_DO
    S_MGPIO25 = 57,  // SPI1_CLK
    S_MGPIO26 = 58,  // SPI1_CS

    S_HGPIO0 = 59,   // EMMC_D0
    S_HGPIO1 = 60,   // EMMC_D1
    S_HGPIO2 = 61,   // EMMC_D2
    S_HGPIO3 = 62,   // EMMC_D3
    S_HGPIO4 = 63,   // EMMC_D4
    S_HGPIO5 = 64,   // EMMC_D5
    S_HGPIO6 = 65,   // EMMC_D6
    S_HGPIO7 = 66,   // EMMC_D7
    S_HGPIO8 = 67,   // EMMC_CMD
    S_HGPIO9 = 68,   // EMMC_CLK
    S_HGPIO10 = 69,  // SDIO_D0
    S_HGPIO11 = 70,  // SDIO_D1
    S_HGPIO12 = 71,  // SDIO_D2
    S_HGPIO13 = 72,  // SDIO_D3
    S_HGPIO14 = 73,  // SDIO_CLK
    S_HGPIO15 = 74,  // SDIO_CMD

    S_AGPIO0 = 75,    //
    S_AGPIO1 = 76,    //
    S_AGPIO2 = 77,    //
    S_AGPIO3 = 78,    //
    S_AGPIO4 = 79,    //
    S_AGPIO5 = 80,    //
    S_AGPIO6 = 81,    //
    S_AGPIO7 = 82,    //
    S_AGPIO8 = 83,    //
    S_AGPIO9 = 84,    //
    S_AGPIO10 = 85,   //
    S_AGPIO11 = 86,   //
    S_AGPIO12 = 87,   //
    S_AGPIO13 = 88,   //
    S_AGPIO14 = 89,   //
    S_AGPIO15 = 90,   //
    S_AGPIO16 = 91,   //
    S_AGPIO17 = 92,   //
    S_AGPIO18 = 93,   //
    S_AGPIO19 = 94,   //
    S_AGPIO20 = 95,   //
    S_AGPIO21 = 96,   //
    S_AGPIO22 = 97,   //
    S_AGPIO23 = 98,   //
    S_AGPIO24 = 99,   //
    S_AGPIO25 = 100,  //
    S_AGPIO26 = 101,  //
    S_AGPIO27 = 102,  //
    S_AGPIO28 = 103,  //
    S_AGPIO29 = 104,  //
    S_AGPIO30 = 105,  //
    S_AGPIO31 = 106,  //
    S_AGPIO32 = 107,  //
    S_AGPIO33 = 108,  //
    S_AGPIO34 = 109,  //

    S_UGPIO0 = 110,  //
    S_UGPIO1 = 111,  //

    PIN_NONE,
} pin_t;

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

static hal_pin_mode_map_t const g_pin_mode_map[] = {
    {S_EGPIO0, S_EGPIO31, HAL_S_EGPIO_MODE_BASE_ADDR, 1}, {S_MGPIO0, S_MGPIO26, HAL_S_MGPIO_MODE_BASE_ADDR, 1},
    {S_HGPIO0, S_HGPIO15, HAL_S_HGPIO_MODE_BASE_ADDR, 1}, {S_AGPIO0, S_AGPIO34, HAL_S_AGPIO_MODE_BASE_ADDR, 1},
    {S_UGPIO0, S_UGPIO1, HAL_ULP_GPIO_MODE_BASE_ADDR, 1},
};

static hal_pin_pull_map_t const g_pin_pull_map[] = {
    {S_EGPIO0, S_EGPIO31, HAL_PAD_S_EGPIO_CFG_BASE_ADDR, 0, 1},
    {S_MGPIO0, S_MGPIO26, HAL_PAD_S_MGPIO_CFG_BASE_ADDR, 0, 1},
    {S_HGPIO0, S_HGPIO15, HAL_PAD_S_HGPIO_CFG_BASE_ADDR, 0, 1},
    {S_AGPIO0, S_AGPIO34, HAL_PAD_S_AGPIO_CFG_BASE_ADDR, 0, 1},
    {S_UGPIO0, S_UGPIO1, HAL_PAD_S_UGPIO_CFG_BASE_ADDR, 0, 1},
};

static hal_pin_ds_map_t const g_pin_ds_map[] = {
    {S_EGPIO0, S_EGPIO31, HAL_PAD_S_EGPIO_CFG_BASE_ADDR, 1, 4},
    {S_MGPIO0, S_MGPIO26, HAL_PAD_S_MGPIO_CFG_BASE_ADDR, 1, 4},
    {S_HGPIO0, S_HGPIO15, HAL_PAD_S_HGPIO_CFG_BASE_ADDR, 1, 4},
    {S_AGPIO0, S_AGPIO34, HAL_PAD_S_AGPIO_CFG_BASE_ADDR, 1, 4},
    {S_UGPIO0, S_UGPIO1, HAL_PAD_S_UGPIO_CFG_BASE_ADDR, 1, 4},
};

const hal_pio_config_t g_pio_function_config_mmc[] = {
    {HAL_PIO_FUNC_EMMC, HAL_PIO_DRIVE_2, HAL_PIO_PULL_UP, HAL_PIO_IE_ENABLE},   // S_HGPIO0
    {HAL_PIO_FUNC_EMMC, HAL_PIO_DRIVE_2, HAL_PIO_PULL_UP, HAL_PIO_IE_ENABLE},   // S_HGPIO1
    {HAL_PIO_FUNC_EMMC, HAL_PIO_DRIVE_2, HAL_PIO_PULL_UP, HAL_PIO_IE_ENABLE},   // S_HGPIO2
    {HAL_PIO_FUNC_EMMC, HAL_PIO_DRIVE_2, HAL_PIO_PULL_UP, HAL_PIO_IE_ENABLE},   // S_HGPIO3
    {HAL_PIO_FUNC_EMMC, HAL_PIO_DRIVE_2, HAL_PIO_PULL_UP, HAL_PIO_IE_ENABLE},   // S_HGPIO4
    {HAL_PIO_FUNC_EMMC, HAL_PIO_DRIVE_2, HAL_PIO_PULL_UP, HAL_PIO_IE_ENABLE},   // S_HGPIO5
    {HAL_PIO_FUNC_EMMC, HAL_PIO_DRIVE_2, HAL_PIO_PULL_UP, HAL_PIO_IE_ENABLE},   // S_HGPIO6
    {HAL_PIO_FUNC_EMMC, HAL_PIO_DRIVE_2, HAL_PIO_PULL_UP, HAL_PIO_IE_ENABLE},   // S_HGPIO7
    {HAL_PIO_FUNC_EMMC, HAL_PIO_DRIVE_2, HAL_PIO_PULL_UP, HAL_PIO_IE_ENABLE},   // S_HGPIO8 cmd
    {HAL_PIO_FUNC_EMMC, HAL_PIO_DRIVE_2, HAL_PIO_PULL_MAX, HAL_PIO_IE_ENABLE},  // S_HGPIO9 clk
};

static void hal_pin_get_ds_addr(hal_pin_ds_map_t **pin_map, uint32_t *map_size)
{
    *map_size = (uint32_t)array_size(g_pin_ds_map);
    *pin_map = (hal_pin_ds_map_t *)g_pin_ds_map;
}

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

    uapi_reg_write16(reg_addr, 1);
    return ERRCODE_SUCC;
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

static void emmc_pio_func_init(void)
{
    const hal_pio_config_t *pio_func = g_pio_function_config_mmc;
    for (pin_t i = S_HGPIO0; i <= S_HGPIO9; i++) {
        hal_pin_3322_set_aon_mode(i, (pin_mode_t)pio_func[i].func);
        hal_pin_3322_set_aon_ds(i, (pin_drive_strength_t)pio_func[i].drive);
        hal_pin_3322_set_aon_pull(i, (pin_pull_t)pio_func[i].pull);
        hal_pin_3322_set_ie(i, (pin_input_enable_t)pio_func[i].ie);
    }
    writel(HAL_PAD_S_HGPIO_CFG_BASE_ADDR + 0x0, 0x35);
    writel(HAL_PAD_S_HGPIO_CFG_BASE_ADDR + 0x4, 0x35);
    writel(HAL_PAD_S_HGPIO_CFG_BASE_ADDR + 0x8, 0x35);
    writel(HAL_PAD_S_HGPIO_CFG_BASE_ADDR + 0xc, 0x35);
    writel(HAL_PAD_S_HGPIO_CFG_BASE_ADDR + 0x10, 0x35);
    writel(HAL_PAD_S_HGPIO_CFG_BASE_ADDR + 0x14, 0x35);
    writel(HAL_PAD_S_HGPIO_CFG_BASE_ADDR + 0x18, 0x35);
    writel(HAL_PAD_S_HGPIO_CFG_BASE_ADDR + 0x1C, 0x35);
    writel(HAL_PAD_S_HGPIO_CFG_BASE_ADDR + 0x20, 0x35);
    writel(HAL_PAD_S_HGPIO_CFG_BASE_ADDR + 0x24, 0x34);
}

void emmc_pmu_reg_config(void)
{
    reg16_setbit(HS_CTL_RB_HS_CLK_EN_REG, HS_CTL_RB_CFG_HS_EMMC_CLKEN_OFFSET);
}

void uapi_pmu_mem_hs_0p9_set(void)
{
    if (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_2_REG, LP_CTL_MCU_RB_MCU_HS_PWR_EN_MAN_OFFSET)) {
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_H_REG, 0x0B);
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_MH_REG, 0x03);
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_ML_REG, 0x0B);
        writew(HS_CTL_RB_USB_CTRL_SP_TMOD_L_REG, 0x0B);

        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG0_REG, 0x0B);
        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG1_REG, 0x0B);
        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG2_REG, 0x03);
        writew(HS_CTL_RB_HS_PERP_TCM_RAM_TMOD_CFG3_REG, 0x0B);
        writew(HS_CTL_RB_HS_PERP_TCM_TP_TMOD_CFG2_REG, 0x24);
        writew(HS_CTL_RB_HS_PERP_TCM_TP_TMOD_CFG3_REG, 0x43);
    }
}

void emmc_porting_config(void)
{
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_2_REG, LP_CTL_MCU_RB_MCU_HS_PWR_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_2_REG, LP_CTL_MCU_RB_MCU_HS_PWR_EN_MAN_SEL_OFFSET);
    while (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_PWR_ACK_REG, LP_CTL_MCU_RB_MCU_HS_PWR_ACK_OFFSET) != 1) {
    }
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_2_REG, LP_CTL_MCU_RB_MCU_HS_ISO_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_2_REG, LP_CTL_MCU_RB_MCU_HS_ISO_EN_MAN_SEL_OFFSET);
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ZERO_REG, LP_CTL_MCU_RB_RST_SYS_MCU_HS_CRG_N_DBG_FRC_ZERO_OFFSET);
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ZERO_REG, LP_CTL_MCU_RB_RST_SYS_MCU_HS_LGC_N_DBG_FRC_ZERO_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ONE_REG, LP_CTL_MCU_RB_RST_SYS_MCU_HS_CRG_N_DBG_FRC_ONE_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_FRC_ONE_REG, LP_CTL_MCU_RB_RST_SYS_MCU_HS_LGC_N_DBG_FRC_ONE_OFFSET);
    if (reg32_getbits(GLB_CTL_M_RB_SOFT_RST_6_REG, 0, 0x2) != 0x3) { // len 2
        reg16_setbit(GLB_CTL_M_RB_SOFT_RST_6_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_SDIO_CRG_N_OFFSET);
        reg16_setbit(GLB_CTL_M_RB_SOFT_RST_6_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_SDIO_LGC_N_OFFSET);
    }
    // auto cg cfg
    writew(HS_CTL_RB_PWM_CFG_REG, 0);
    writew(HS_CTL_RB_HS_AXI_BRG_LP_CFG0_REG, 0);
    writew(HS_CTL_RB_HS_AXI_BRG_LP_CFG1_REG, 0);
    writew(HS_CTL_RB_HS_AXI_BRG_LP_CFG2_REG, 0);
    writew(HS_CTL_RB_HS_AXI_BRG_LP_CFG3_REG, 0);
    writew(HS_CTL_RB_HS_AXI_BRG_LP_CFG4_REG, 0);

    uapi_pmu_mem_hs_0p9_set();

    // fmc router
    reg16_clrbit(COM_CTL_RB_C_CRG_CLKEN_REG, COM_CTL_RB_CFG_COM_EMMC_CLKEN_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_10);
    writew(COM_CTL_RB_EMMC_CR_REG, 1);
    uapi_tcxo_delay_us(TCXO_DELAY_10);
    reg16_setbit(COM_CTL_RB_C_CRG_CLKEN_REG, COM_CTL_RB_CFG_COM_EMMC_CLKEN_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_10);
    // LDO1-SW0使能, 如果emmc使用板级1p8, 该处使能不用打开。
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_72_REG, LP_CTL_GLB_RB_D_PMU_SW1P8_0_EN_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_240UL);
    emmc_pio_func_init();
    emmc_pmu_reg_config();
}
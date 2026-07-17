/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 */
/* *
 * @file pins_arduino.h
 * @brief Pin mapping for Arduino compatibility on 3322 (HiDiTingV100) — chip porting layer
 *
 * REAL pin mapping (was a compile-time placeholder subset).
 *
 * Sources:
 *   - HiDiTingV100 硬件用户指南, "数字管脚" table (~L868) + "AUX ADC接口" (~L608) +
 *     "UART接口" (~L2133) + "PWM接口" (~L2187).
 *   - pin_t enum: drivers/boards/3322_evb/product/product_pin.h
 *     (S_AGPIO0=75 .. S_AGPIO34=109; S_MGPIO0=32 .. S_MGPIO26=58).
 *   - GPIO channel layout: drivers/chips/3322/include/shared/gpio_porting.h +
 *     porting/gpio/gpio_porting.c. hal_gpio_v150_pin_info_get() resolves a pin_t to a
 *     channel/group/bit by raw range match — so an Arduino pin must return a pin_t that
 *     falls inside a real GPIO channel.
 *
 * Why the S_AGPIO bank: on 3322 the other GPIO-capable banks are largely committed —
 *   S_EGPIO0..11 carry QSPI0/QSPI3 flash; S_HGPIO* (pin_t 59..74) carry eMMC/SDIO and
 *   fall in a channel gap (no GPIO channel covers 59..74); S_MGPIO* carry the fixed
 *   UART0/I2C0-3/SPI0/UART1-2/SPI1 buses. S_AGPIO* (GPIO channel 2, pin_t 75..106) is
 *   the general-purpose "application GPIO" bank, so Arduino D0..D16 are mapped there.
 *
 * Voltage domain: every S_AGPIO pin is IOLDO1 (1.8V). Level shifting is a board concern.
 *
 * NOTE: wiring_analog.cpp / wiring_pulse.cpp / Servo.cpp ARE compiled on 3322
 * (the chip-agnostic CMakeLists lists them; build verified). The ADC/PWM tables
 * below are filled from the hardware guide, but analog/tone paths have not yet
 * been validated on-board — exercise them only after verifying the SDK ADC/PWM
 * APIs behave as the ws63-origin code assumes.
 */

#ifndef PINS_ARDUINO_H
#define PINS_ARDUINO_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * 3322 pin_t bases (from product_pin.h). Kept as macros so this header stays
 * self-contained — it does not require product_pin.h on the include path. The
 * chip-agnostic layer casts these to pin_t at the call site ((pin_t)hw_pin).
 * ============================================================================ */
#define PIN_S_MGPIO_BASE 32   /* S_MGPIO0 = 32 (product_pin.h) */
#define PIN_S_MGPIO(n)   (PIN_S_MGPIO_BASE + (n))
#define PIN_S_AGPIO_BASE 75   /* S_AGPIO0 = 75 (product_pin.h) */
#define PIN_S_AGPIO(n)   (PIN_S_AGPIO_BASE + (n))

/* ============================================================================
 * Pinctrl pull-type name aliases (PORTING CONTRACT — see porting_contract.md)
 *
 * The chip-agnostic wiring_digital.cpp references PIN_PULL_TYPE_{UP,DOWN,
 * DISABLE} (the ws63 pinctrl_porting.h enum names). 3322's enum is instead
 * PIN_PULL_{UP,DOWN,NONE}. Each chip porting layer MUST provide these aliases
 * so the chip-agnostic source compiles unchanged. Macro expansion is deferred
 * to the point of use, so the target tokens resolve once pinctrl.h is included.
 *
 * NOTE: the enum *values* differ too (ws63 UP=3, 3322 UP=2), but the alias
 * forwards to the chip's own constant, so the value is always correct.
 * ============================================================================ */
#define PIN_PULL_TYPE_UP      PIN_PULL_UP
#define PIN_PULL_TYPE_DOWN    PIN_PULL_DOWN
#define PIN_PULL_TYPE_DISABLE PIN_PULL_NONE

/* ============================================================================
 * Total Pin Count
 * ============================================================================ */

/* D0..D16 → S_AGPIO0..S_AGPIO16 (pin_t 75..91). 17 pins so that D16 (S_AGPIO16,
 * the 4th ADC-muxed pin, AUX_ADC_CH_7) is reachable as A3. */
#define NUM_DIGITAL_PINS  17
#define NUM_ANALOG_INPUTS 4
#define NUM_ANALOG_PINS   NUM_ANALOG_INPUTS

/* 3322 HiDiTingV100 AUX ADC: 12-bit, 8 channels. CH_4..CH_7 are muxed with
 * S_AGPIO pins (per 硬件用户指南 AUX ADC接口); CH_0..CH_3 are dedicated analog
 * pads (not GPIO). */
#define NUM_ADC_CHANNELS  8

#ifndef NOT_A_PIN
#define NOT_A_PIN 0xFF
#endif

// Invalid timer marker (0xFF; distinct from valid timer index 0).
// digitalPinToPWMChannel() returns this for non-PWM pins.
#ifndef NOT_ON_TIMER
#define NOT_ON_TIMER 0xFF
#endif

/* ============================================================================
 * Arduino logical pin numbers
 * ============================================================================ */

/* Digital D0..D16 — indices into g_arduino_pin_to_pint[] below. */
#define D0  0
#define D1  1
#define D2  2
#define D3  3
#define D4  4
#define D5  5
#define D6  6
#define D7  7
#define D8  8
#define D9  9
#define D10 10
#define D11 11
#define D12 12
#define D13 13
#define D14 14
#define D15 15
#define D16 16

/* Analog A0..A3 — aliased onto the digital indices whose S_AGPIO pin is
 * ADC-muxed (硬件用户指南: CH_4..CH_7 share with digital pins). analogRead()
 * requires the pin to be < NUM_DIGITAL_PINS, hence the aliasing. */
#define A0  D8   /* S_AGPIO8  — AUX_ADC_CH_6 (also UART3_RXD / I2C3_SDA) */
#define A1  D11  /* S_AGPIO11 — AUX_ADC_CH_4 (also UART4_TXD / APWM1_P)  */
#define A2  D15  /* S_AGPIO15 — AUX_ADC_CH_5 (also APWM0_P  / FEM_CTRL3) */
#define A3  D16  /* S_AGPIO16 — AUX_ADC_CH_7 (also APWM0_N  / FEM_CTRL2) */

/* ============================================================================
 * Arduino pin index → 3322 pin_t lookup table
 * D<n> → S_AGPIO<n> (pin_t 75+n). Alternate-function notes from 硬件用户指南
 * 数字管脚 table; "-" means no listed digital mux (free GPIO in MODE_0).
 * Pins flagged BT/WLAN/USB are safe as GPIO only when that function is unused
 * on the board — reassign on boards where the RF/USB function is active.
 * ============================================================================ */
static const uint8_t g_arduino_pin_to_pint[NUM_DIGITAL_PINS] = {
    PIN_S_AGPIO(0),  /* D0  : BT_WIFI_SW  / I2C0_SCL  (BT — reassign if BT active) */
    PIN_S_AGPIO(1),  /* D1  : RF_TEST_IP  / BT_ACT / I2C0_SDA (BT/RF — reassign)   */
    PIN_S_AGPIO(2),  /* D2  : BTS_SAMPLE  / I2C1_SCL  (BT — reassign if BT active) */
    PIN_S_AGPIO(3),  /* D3  : - (free GPIO)                                       */
    PIN_S_AGPIO(4),  /* D4  : - (free GPIO)                                       */
    PIN_S_AGPIO(5),  /* D5  : - (free GPIO)                                       */
    PIN_S_AGPIO(6),  /* D6  : WLAN_ACT / CLK_IN (WLAN — reassign if WLAN active)  */
    PIN_S_AGPIO(7),  /* D7  : UART3_TXD / I2C3_SCL  (default Serial2 = UART3 TX)  */
    PIN_S_AGPIO(8),  /* D8  : UART3_RXD / I2C3_SDA + AUX_ADC_CH_6  (= A0)         */
    PIN_S_AGPIO(9),  /* D9  : - (free GPIO)                                       */
    PIN_S_AGPIO(10), /* D10 : CLKOUT1  / UART3_RXD                                */
    PIN_S_AGPIO(11), /* D11 : UART4_TXD / APWM1_P + AUX_ADC_CH_4 (= A1, PWM)      */
    PIN_S_AGPIO(12), /* D12 : UART4_RXD / APWM1_N                       (PWM)     */
    PIN_S_AGPIO(13), /* D13 : UART4_CTS / CLKOUT0                       (LED)     */
    PIN_S_AGPIO(14), /* D14 : UART4_RTS / CLKOUT1                                */
    PIN_S_AGPIO(15), /* D15 : APWM0_P / FEM_CTRL3 + AUX_ADC_CH_5 (= A2, PWM)      */
    PIN_S_AGPIO(16), /* D16 : APWM0_N / FEM_CTRL2 + AUX_ADC_CH_7 (= A3, PWM)      */
};

/* Arduino analog pin → AUX ADC channel (adc_channel_t ADC_CHANNEL_0..7).
 * Only CH_4..CH_7 are reachable via S_AGPIO pins; order matches A0..A3 above. */
static const uint8_t g_arduino_adc_channel[NUM_ANALOG_INPUTS] = {
    6,  /* A0 = S_AGPIO8  → AUX_ADC_CH_6 */
    4,  /* A1 = S_AGPIO11 → AUX_ADC_CH_4 */
    5,  /* A2 = S_AGPIO15 → AUX_ADC_CH_5 */
    7,  /* A3 = S_AGPIO16 → AUX_ADC_CH_7 */
};

/* ============================================================================
 * Default peripheral pins
 *
 * HardwareSerial (utils/arduino/src/HardwareSerial.cpp) does NOT consume these
 * macros — it passes tx_pin/rx_pin = (pin_t)-1 and lets the SDK's
 * uart_port_config_pinmux() pick the pins per UART bus. These macros document
 * the physical UART2/UART3 pins (硬件用户指南 UART接口; uart_porting.c) so sketches
 * and board docs agree on which bus/pins a Serial object uses:
 *   Serial  = UART_BUS_0 (S_MGPIO0/1)   — debug/log UART, not for peripherals
 *   Serial2 = UART_BUS_2 = UART2        — S_MGPIO17(TX)/S_MGPIO18(RX)  ← default
 *   UART3   = UART_BUS_3                — S_AGPIO7(TX)/S_AGPIO8(RX)
 * (UART4 / UART_BUS_1 cannot be used for peripherals per the hardware guide.)
 * ============================================================================ */

/* Default peripheral serial = UART2 (recommended for external devices). */
#define PIN_SERIAL_TX  PIN_S_MGPIO(17)   /* S_MGPIO17 = UART2_TXD */
#define PIN_SERIAL_RX  PIN_S_MGPIO(18)   /* S_MGPIO18 = UART2_RXD */
#define PIN_SERIAL1_TX PIN_S_AGPIO(7)    /* S_AGPIO7  = UART3_TXD */
#define PIN_SERIAL1_RX PIN_S_AGPIO(8)    /* S_AGPIO8  = UART3_RXD */

/* I2C / SPI placeholders — left pointing at D-pins; refine from the hardware
 * guide I2C/SPI interface sections when Wire/SPI are ported to 3322. */
#define PIN_WIRE_SDA A0
#define PIN_WIRE_SCL D7
#define PIN_SPI_SS   D10
#define PIN_SPI_MOSI D11
#define PIN_SPI_MISO D12
#define PIN_SPI_SCK  D13

/* On the EVB no LED is wired to a named GPIO; D13 (S_AGPIO13) is a free GPIO
 * in MODE_0, so the smoke-test blink toggles it harmlessly. */
#define LED_BUILTIN D13

/* ============================================================================
 * Pin mapping helper functions
 * ============================================================================ */

/* Arduino pin -> 3322 pin_t (for the uapi_gpio_xxx / uapi_pin_xxx calls). */
static inline uint8_t digitalPinToPin(uint8_t arduino_pin)
{
    if (arduino_pin >= NUM_DIGITAL_PINS) {
        return NOT_A_PIN;
    }
    return g_arduino_pin_to_pint[arduino_pin];
}

/* Arduino analog pin → AUX ADC channel (ADC_CHANNEL_0..7), else NOT_A_PIN. */
static inline uint8_t analogPinToChannel(uint8_t arduino_pin)
{
    switch (arduino_pin) {
        case A0: return g_arduino_adc_channel[0];
        case A1: return g_arduino_adc_channel[1];
        case A2: return g_arduino_adc_channel[2];
        case A3: return g_arduino_adc_channel[3];
        default: return NOT_A_PIN;
    }
}

/* Arduino pin → PWM channel (pwm_channel_t PWM_0..PWM_5), else NOT_ON_TIMER.
 * 3322 PWM channels (PWM接口 + pwm_porting.h, PWM_0..PWM_5) map to the 6 PWM
 * signals in APWM0(P/N), APWM1(P/N), MPWM0(P/N) order:
 *   PWM_0 = APWM0_P (S_AGPIO15 = D15)
 *   PWM_1 = APWM0_N (S_AGPIO16 = D16)
 *   PWM_2 = APWM1_P (S_AGPIO11 = D11)
 *   PWM_3 = APWM1_N (S_AGPIO12 = D12)
 *   PWM_4 = MPWM0_P (S_MGPIO16 — not in D0..D16; shares SPI0_CS1)
 *   PWM_5 = MPWM0_N (S_MGPIO18 — not in D0..D16; shares UART2_RXD)
 * NOTE: the PWM_x↔signal ordering is the conventional one; confirm on-board
 * when analogWrite/tone are enabled on 3322. */
static inline uint8_t digitalPinToPWMChannel(uint8_t arduino_pin)
{
    switch (arduino_pin) {
        case D15: return 0;  /* APWM0_P */
        case D16: return 1;  /* APWM0_N */
        case D11: return 2;  /* APWM1_P */
        case D12: return 3;  /* APWM1_N */
        default: return NOT_ON_TIMER;
    }
}

static inline bool pinSupportsPWM(uint8_t arduino_pin)
{
    if (arduino_pin >= NUM_DIGITAL_PINS) {
        return false;
    }
    return digitalPinToPWMChannel(arduino_pin) != NOT_ON_TIMER;
}

/* True if the Arduino pin is one of A0..A3 (ADC-muxed). */
static inline bool pinIsAnalog(uint8_t arduino_pin)
{
    return arduino_pin == A0 || arduino_pin == A1 ||
           arduino_pin == A2 || arduino_pin == A3;
}
#define IS_ANALOG_PIN(pin) pinIsAnalog((uint8_t)(pin))

/* True if the Arduino pin supports PWM output. */
#define HAS_PWM(pin) (digitalPinToPWMChannel((uint8_t)(pin)) != NOT_ON_TIMER)

#ifdef __cplusplus
}
#endif

#endif /* PINS_ARDUINO_H */

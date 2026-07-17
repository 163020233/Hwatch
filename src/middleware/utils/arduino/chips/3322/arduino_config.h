/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 */
/* *
 * @file arduino_config.h
 * @brief 3322 (HiDiTingV100) chip configuration for the Arduino layer (chip porting layer).
 *
 * Decides which Arduino features the 3322 build enables. Arduino.h includes this
 * (via middleware/chips/${CHIP}/arduino/ on the include path) instead of deriving
 * the config inline, so the chip-agnostic Arduino.h stays portable.
 *
 * Values sourced from:
 *   - src/build/config/target_config/3322/menuconfig/acore/diting_community.config
 *   - src/build/config/target_config/3322/target_config.py / config.py (diting-community)
 *
 * I2S is NOT supported on 3322 (no SIO driver — sio_porting.h / hal_sio.h are
 * porting stubs); CONFIG_I2S_SUPPORT stays undefined so I2SClass.cpp compiles to
 * its stub path.
 */

#ifndef ARDUINO_CONFIG_H
#define ARDUINO_CONFIG_H

/* ============================================================================
 * Hardware serial ports. 3322 exposes UART2/UART3 for peripherals (UART4 cannot
 * be used for peripherals per the hardware guide). HAVE_HWSERIAL1/2 expand when
 * multi-serial is wired.
 * ============================================================================ */
#define HAVE_HWSERIAL0
/* #define HAVE_HWSERIAL1 */   /* not wired yet */
/* #define HAVE_HWSERIAL2 */   /* not wired yet */

/* ============================================================================
 * GPIO digital IO — always on.
 * ============================================================================ */
#if !defined(CONFIG_GPIO_SUPPORT)
#define CONFIG_GPIO_SUPPORT 1
#endif

/* ============================================================================
 * PWM support — 3322 menuconfig: CONFIG_PWM_USING_V151=y (same IP version as
 * ws63). Derive CONFIG_PWM_SUPPORT so wiring_analog/wiring_pulse/Servo compile
 * their hardware paths.
 * ============================================================================ */
#if !defined(CONFIG_PWM_SUPPORT) && defined(CONFIG_PWM_USING_V151)
#define CONFIG_PWM_SUPPORT 1
#endif

/* ============================================================================
 * PWM period register width (wiring_analog.cpp / wiring_pulse.cpp). 3322 uses
 * the V151 PWM IP: the period register (high_time + low_time) is 16-bit.
 * Caps analogWrite()/tone() carrier cycle counts.
 * ============================================================================ */
#if !defined(ARDUINO_PWM_PERIOD_MAX)
#define ARDUINO_PWM_PERIOD_MAX 65535U
#endif

/* ============================================================================
 * ADC support — 3322 menuconfig: CONFIG_ADC_USING_V156=y (ws63 uses V154).
 * Derive CONFIG_ADC_SUPPORT from the 3322 ADC version.
 * ============================================================================ */
#if !defined(CONFIG_ADC_SUPPORT) && defined(CONFIG_ADC_USING_V156)
#define CONFIG_ADC_SUPPORT 1
#endif

/* ============================================================================
 * ADC hardware resolution (wiring_analog.cpp). 3322 HiDiTingV100 AUX ADC is
 * 12-bit (硬件用户指南 AUX ADC接口). Drives the analogRead() range scaling.
 * ============================================================================ */
#if !defined(ARDUINO_ADC_HW_BITS)
#define ARDUINO_ADC_HW_BITS 12
#endif

/* ============================================================================
 * Interrupt-capable pins — placeholder 16 (refine to 3322's actual count).
 * ============================================================================ */
#define MAX_INTERRUPT_PINS 16

/* ============================================================================
 * UART (HardwareSerial) TX/RX support. Always on.
 * ============================================================================ */
#if !defined(CONFIG_UART_SUPPORT_TX)
#define CONFIG_UART_SUPPORT_TX 1
#endif
#if !defined(CONFIG_UART_SUPPORT_RX)
#define CONFIG_UART_SUPPORT_RX 1
#endif

/* ============================================================================
 * I2C master support (Wire). 3322 menuconfig: CONFIG_I2C_SUPPORT_MASTER=y.
 * ============================================================================ */
#if !defined(CONFIG_I2C_SUPPORT_MASTER)
#define CONFIG_I2C_SUPPORT_MASTER 1
#endif

/* 3322 i2c driver has no uapi_i2c_slave_deinit (slave cleanup uses the shared
 * bus deinit uapi_i2c_deinit). Alias so Wire.cpp's slave path compiles. */
#define uapi_i2c_slave_deinit uapi_i2c_deinit

/* ============================================================================
 * SPI master pin assignment (SPI.cpp). 3322 menuconfig: CONFIG_SPI_SUPPORT_MASTER=y.
 * Pin numbers below are PLACEHOLDERS — replace with real 3322 GPIO/pinmux from
 * the HiDiTingV100 hardware guide (SPI接口, ~L2120) before on-board use.
 * Compile does not depend on correctness.
 * ============================================================================ */
#if !defined(CONFIG_SPI_DI_MASTER_PIN)
#define CONFIG_SPI_DI_MASTER_PIN 11
#endif
#if !defined(CONFIG_SPI_DO_MASTER_PIN)
#define CONFIG_SPI_DO_MASTER_PIN 9
#endif
#if !defined(CONFIG_SPI_CLK_MASTER_PIN)
#define CONFIG_SPI_CLK_MASTER_PIN 7
#endif
#if !defined(CONFIG_SPI_CS_MASTER_PIN)
#define CONFIG_SPI_CS_MASTER_PIN 10
#endif
#if !defined(CONFIG_SPI_MASTER_PIN_MODE)
#define CONFIG_SPI_MASTER_PIN_MODE 3
#endif

/* ============================================================================
 * SPI master bus clock (SPI.cpp). 3322 SSI source clock is 64MHz
 * (flashboot_3322/tee_test.c: TEE_SPI_BUS_CLK = 64000000). SDK uses bus_clk as
 * the baud-rate divider base: SCK = bus_clk / clk_div. ws63 uses 32MHz.
 * ============================================================================ */
#if !defined(ARDUINO_SPI_BUS_CLK_HZ)
#define ARDUINO_SPI_BUS_CLK_HZ 64000000UL
#endif
#if !defined(ARDUINO_SPI_MAX_CLOCK_HZ)
#define ARDUINO_SPI_MAX_CLOCK_HZ 64000000UL
#endif

/* ============================================================================
 * Timer (TimerClass). 3322 timer_porting.h defines TIMER_INDEX_0.
 * ARDUINO_TIMER_RESERVED expands to TIMER_INDEX_0 (resolved at point of use,
 * after timer_porting.h is included via TimerClass.h).
 * ============================================================================ */
#define ARDUINO_TIMER_IRQ_PRIORITY 1
#if !defined(ARDUINO_TIMER_RESERVED)
#define ARDUINO_TIMER_RESERVED TIMER_INDEX_0
#endif

// Timer instance mapping: Arduino timer number -> chip timer_index_t enum
// (defined in timer_porting.h). 3322 has TIMER_INDEX_0/1/2/3 (4 timers).
// Each TIMER_INSTANCE_N is consumed by the gated TimerN object in TimerClass.cpp.
#define TIMER_INSTANCE_0 ((uint8_t)TIMER_INDEX_0)
#define TIMER_INSTANCE_1 ((uint8_t)TIMER_INDEX_1)
#define TIMER_INSTANCE_2 ((uint8_t)TIMER_INDEX_2)
#define TIMER_INSTANCE_3 ((uint8_t)TIMER_INDEX_3)

// Default timer for the C-style API (timerInit/timerStart/timerStop). Picks a
// TimerN object the system/RTOS does NOT use. 3322: the RTOS tick runs on an
// independent HQ timer, Timer1 is reserved by chip_init, so Timer2 is free.
// Macro expansion is deferred to the point of use (inside TimerClass.cpp).
#define ARDUINO_DEFAULT_TIMER (&Timer2)

/* ============================================================================
 * I2S/SIO support — NOT available on 3322 (no SIO driver). CONFIG_I2S_SUPPORT
 * stays undefined; I2SClass.cpp compiles to its stub path (sio_porting.h /
 * hal_sio.h are porting stubs). Non-functional I2S until a real porting is added.
 * The ARDUINO_I2S_*_PIN values below are unused (I2S disabled) but provided so
 * the chip-agnostic I2SClass.h macros resolve consistently across chips.
 * ============================================================================ */
#if !defined(ARDUINO_I2S_SCK_PIN)
#define ARDUINO_I2S_SCK_PIN 0
#endif
#if !defined(ARDUINO_I2S_WS_PIN)
#define ARDUINO_I2S_WS_PIN 1
#endif
#if !defined(ARDUINO_I2S_SD_PIN)
#define ARDUINO_I2S_SD_PIN 2
#endif
#if !defined(ARDUINO_I2S_SD_IN_PIN)
#define ARDUINO_I2S_SD_IN_PIN 3
#endif

// USB macros (not applicable on this target, but kept for compatibility)
#define SERIAL_PORT_MONITOR Serial
#define SERIAL_PORT_HARDWARE Serial
#define SERIAL_PORT_HARDWARE1 Serial1
#define SERIAL_PORT_HARDWARE2 Serial2

#endif /* ARDUINO_CONFIG_H */

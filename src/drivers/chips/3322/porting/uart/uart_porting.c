/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides uart port template \n
 *
 * History: \n
 * 2022-06-06， Create file. \n
 */
#include "uart_porting.h"
#include "hal_uart_v151.h"
#include "chip_io.h"
#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "uart.h"
#include "platform_core.h"
#include "securec.h"
#include "osal_interrupt.h"
#include "debug_print.h"
#if defined(CONFIG_UART_SUPPORT_DMA)
#include "dma_porting.h"
#endif

#ifdef CONFIG_UART_SUPPORT_PORTING_IRQ
#endif
#ifdef LOG_SUPPORT
#include "log_oam_msg.h"
#endif
#ifdef HSO_SUPPORT
#include "dfx_channel.h"
#endif

#ifdef CFG_DRIVERS_USB_AS_SERIAL
#include "usb_porting.h"
#endif
#ifdef CONFIG_UART_SUPPORT_LPC
#include "pm_clocks.h"
#endif

#define UART_BUS_0_BASE_ADDR (UART0_BASE)
#if UART_BUS_MAX_NUMBER > 1
#define UART_BUS_1_BASE_ADDR (UART1_BASE)
#endif
#if UART_BUS_MAX_NUMBER > 2
#define UART_BUS_2_BASE_ADDR (UART2_BASE)
#endif
#if UART_BUS_MAX_NUMBER > 3
#define UART_BUS_3_BASE_ADDR (UART3_BASE)
#endif

#define UART_TRANS_LEN_MAX   512

const uintptr_t g_uart_base_addrs[UART_BUS_MAX_NUM] = {
    (uintptr_t)UART_BUS_0_BASE_ADDR,
#if UART_BUS_MAX_NUMBER > 1
    (uintptr_t)UART_BUS_1_BASE_ADDR,
#endif
#if UART_BUS_MAX_NUMBER > 2
    (uintptr_t)UART_BUS_2_BASE_ADDR,
#endif
#if UART_BUS_MAX_NUMBER > 3
    (uintptr_t)UART_BUS_3_BASE_ADDR,
#endif
};

#if CORE == APPS
#ifndef BUILD_ATE_IMAGE
#define HOST_BURN_UART_RX_BUFFER_SIZE  1
static bool g_host_burn_uart_enabled = false;
static uint8_t g_host_uart_rx_buffer[HOST_BURN_UART_RX_BUFFER_SIZE];
#endif
#endif

uintptr_t uart_porting_base_addr_get(uart_bus_t bus)
{
    return g_uart_base_addrs[bus];
}

uint32_t uart_porting_div_value_get(uart_bus_t bus)
{
    return hal_uart_v151_div_value_get(bus);
}

void uart_porting_div_value_set(uart_bus_t bus, uint32_t value)
{
    hal_uart_v151_div_value_set(bus, value);
}

typedef struct uart_interrupt {
    core_irq_t irq_num;
    osal_irq_handler irq_func;
}
hal_uart_interrupt_t;

#ifdef CONFIG_UART_SUPPORT_PORTING_IRQ
static const hal_uart_interrupt_t g_uart_interrupt_lines[UART_BUS_MAX_NUMBER] = {
#if UART_BUS_MAX_NUMBER > 0
    { UART_0_IRQN, (osal_irq_handler)irq_uart0_handler },
#endif
#if UART_BUS_MAX_NUMBER > 1
    { UART_1_IRQN, (osal_irq_handler)irq_uart1_handler },
#endif
#if UART_BUS_MAX_NUMBER > 2
    { UART_2_IRQN, (osal_irq_handler)irq_uart2_handler },
#endif
#if UART_BUS_MAX_NUMBER > 3
    { COM_UART_INT_IRQN, (osal_irq_handler)irq_uart3_handler },
#endif
};
#endif

#ifdef FT_SINGLE_UART
void log_factory_at_callback(const void *buffer, uint16_t length, bool remaining)
{
    log_uart_rx_callback(buffer, length, remaining);
    if (zdiag_is_enable() == false) {
#ifdef AT_DEBUG
        uint32_t ret = uapi_at_channel_data_recv(0, (uint8_t *)buffer, (uint32_t)length);
        if (ret != ERRCODE_SUCC) {
            PRINT("at callback exec fail [%x]\r\n", ret);
        }
#else
        uapi_at_channel_data_recv(0, (uint8_t *)buffer, (uint32_t)length);
#endif
        hal_uart_clear_pending(LOG_UART_BUS);
    }
}
#endif

void uart_port_register_hal_funcs(uart_bus_t bus)
{
    hal_uart_register_funcs(bus, hal_uart_v151_funcs_get());
}

static uint32_t g_uart_clock_value = UART_CLOCK_FRQ;

void uart_port_set_clock_value(uart_bus_t bus, uint32_t clock)
{
    unused(bus);
    g_uart_clock_value = clock;
}

uint32_t uart_port_get_clock_value(uart_bus_t bus)
{
    unused(bus);
    return g_uart_clock_value;
}

void uart_port_config_pinmux(uart_bus_t bus)
{
    /* 231219: 当前FPGA仅支持H0，L0, L1 */
    switch (bus) {
#if UART_BUS_MAX_NUMBER > 0
        case UART_BUS_0:
#if CORE == APPS
            uapi_pin_set_mode(S_MGPIO0, PIN_MODE_1);
            uapi_pin_set_mode(S_MGPIO1, PIN_MODE_1);
#elif CORE == BT
            uapi_pin_set_mode(S_AGPIO11, PIN_MODE_1);
            uapi_pin_set_mode(S_AGPIO12, PIN_MODE_1);
#endif
            break;
#endif
#if UART_BUS_MAX_NUMBER > 1
        case UART_BUS_1:
#if CORE == APPS
            uapi_pin_set_mode(S_MGPIO19, PIN_MODE_1);
            uapi_pin_set_mode(S_MGPIO20, PIN_MODE_1);
#elif CORE == BT
            uapi_pin_set_mode(S_AGPIO7, PIN_MODE_1);
            uapi_pin_set_mode(S_AGPIO8, PIN_MODE_1);
#endif
            break;
#endif
#if UART_BUS_MAX_NUMBER > 2
        case UART_BUS_2:
            uapi_pin_set_mode(S_MGPIO17, PIN_MODE_1);
            uapi_pin_set_mode(S_MGPIO18, PIN_MODE_1);
            break;
#endif
#if UART_BUS_MAX_NUMBER > 3
        case UART_BUS_3:
            uapi_pin_set_mode(S_AGPIO7, PIN_MODE_1);
            uapi_pin_set_mode(S_AGPIO8, PIN_MODE_1);
#endif
            break;
        default:
            break;
    }
    return;
}

void uart_port_release_pinmux(uart_bus_t bus)
{
    UNUSED(bus);
}

#ifdef CONFIG_UART_SUPPORT_PORTING_IRQ
void uart_port_register_irq(uart_bus_t bus)
{
    if (g_uart_interrupt_lines[bus].irq_num < BUTT_IRQN) {
#if CORE == APPS
#ifndef BUILD_ATE_IMAGE
        // 烧写uart1使用的轮询读, 不使用中断
        if (g_uart_interrupt_lines[bus].irq_num == UART_1_IRQN && g_host_burn_uart_enabled) {
            return;
        }
#endif
#endif
        osal_irq_request(g_uart_interrupt_lines[bus].irq_num, g_uart_interrupt_lines[bus].irq_func, NULL, NULL, NULL);
        osal_irq_set_priority(g_uart_interrupt_lines[bus].irq_num, irq_prio(g_uart_interrupt_lines[bus].irq_num));
        osal_irq_enable(g_uart_interrupt_lines[bus].irq_num);
    }
}

void irq_uart0_handler(void)
{
    hal_uart_irq_handler(UART_BUS_0);
    osal_irq_clear(UART_0_IRQN);
}

#if UART_BUS_MAX_NUMBER > 1
void irq_uart1_handler(void)
{
    hal_uart_irq_handler(UART_BUS_1);
    osal_irq_clear(UART_1_IRQN);
}
#endif

#if UART_BUS_MAX_NUMBER > 2
void irq_uart2_handler(void)
{
    hal_uart_irq_handler(UART_BUS_2);
    osal_irq_clear(UART_2_IRQN);
}
#endif

#if UART_BUS_MAX_NUMBER > 3
void irq_uart3_handler(void)
{
    hal_uart_irq_handler(UART_BUS_3);
    osal_irq_clear(COM_UART_INT_IRQN);
}
#endif

void uart_port_unregister_irq(uart_bus_t bus)
{
    osal_irq_disable(g_uart_interrupt_lines[bus].irq_num);
    osal_irq_free(g_uart_interrupt_lines[bus].irq_num, NULL);
}

void uart_port_set_pending_irq(uart_bus_t uart)
{
    int_set_pendind_irq(g_uart_interrupt_lines[uart].irq_num);
}
#else
void uart_port_register_irq(uart_bus_t bus)
{
    unused(bus);
}

void uart_port_unregister_irq(uart_bus_t bus)
{
    unused(bus);
}

void uart_port_set_pending_irq(uart_bus_t uart)
{
    unused(uart);
}
#endif

#ifdef SW_UART_DEBUG
#define DEBUG_UART_RX_BUFFER_SIZE 1
STATIC uart_bus_t g_sw_debug_uart;
static bool g_sw_debug_uart_enabled = false;
STATIC uint8_t g_uart_rx_buffer[DEBUG_UART_RX_BUFFER_SIZE];

static void uart_rx_callback(const void *buf, uint16_t buf_len, bool remaining)
{
    UNUSED(remaining);
    if (!g_sw_debug_uart_enabled) {
        return;
    }
#if defined(HSO_SUPPORT) && CORE == APPS
    diag_uart_rx_proc((uint8_t*)buf, buf_len);
#else
#ifdef CFG_DRIVERS_USB_AS_SERIAL
    uapi_usb_serial_write(USB_SERIAL_0, (const uint8_t *)buf, buf_len);
#else
    uapi_uart_write(SW_DEBUG_UART_BUS, (const void *)buf, buf_len, 0);
#endif
#endif
}

#if CORE == APPS
#ifndef BUILD_ATE_IMAGE
static void host_burn_uart_init(uint32_t baud_rate)
{
    uapi_pin_set_mode(HOST_BURN_UART_TX_PIN, PIN_MODE_1);
    uapi_pin_set_ds(HOST_BURN_UART_TX_PIN, PIN_DS_2);
    uapi_pin_set_pull(HOST_BURN_UART_TX_PIN, PIN_PULL_UP);
    uapi_pin_set_ie(HOST_BURN_UART_TX_PIN, PIN_IE_DISABLE);
    uapi_pin_set_mode(HOST_BURN_UART_RX_PIN, PIN_MODE_1);
    uapi_pin_set_ds(HOST_BURN_UART_RX_PIN, PIN_DS_2);
    uapi_pin_set_pull(HOST_BURN_UART_RX_PIN, PIN_PULL_UP);
    uapi_pin_set_ie(HOST_BURN_UART_RX_PIN, PIN_IE_ENABLE);
    uart_pin_config_t uart_pins;
    uart_attr_t uart_line_config;
    uart_buffer_config_t uart_buffer_config;
    uart_pins.rts_pin = PIN_NONE;
    uart_pins.cts_pin = PIN_NONE;
    uart_pins.tx_pin = HOST_BURN_UART_TX_PIN;
    uart_pins.rx_pin = HOST_BURN_UART_RX_PIN;
    uart_line_config.baud_rate = baud_rate;
    uart_line_config.data_bits = UART_DATA_BIT_8;
    uart_line_config.parity = UART_PARITY_NONE;
    uart_line_config.stop_bits = UART_STOP_BIT_1;
    uart_buffer_config.rx_buffer_size = HOST_BURN_UART_RX_BUFFER_SIZE;
    uart_buffer_config.rx_buffer = g_host_uart_rx_buffer;
    g_host_burn_uart_enabled = true;
    (void)uapi_uart_init(HOST_BURN_UART_BUS, &uart_pins, &uart_line_config, NULL, &uart_buffer_config);
}

void host_burn_uart_deinit(void)
{
    uapi_uart_deinit(HOST_BURN_UART_BUS);
    // 握手后切换Uart波特率不成功, 重新复位解复位后 OK，当前是Uart1的配置
    reg16_clrbit(M_CTL_RB_MTOP1_L_REG, M_CTL_RB_UART_H1_RST_SOFT_N_OFFSET);
    reg16_setbit(M_CTL_RB_MTOP1_L_REG, M_CTL_RB_UART_H1_RST_SOFT_N_OFFSET);
    g_host_burn_uart_enabled = false;
    uapi_pin_set_mode(HOST_BURN_UART_TX_PIN, PIN_MODE_0);
    uapi_pin_set_ds(HOST_BURN_UART_TX_PIN, PIN_DS_2);
    uapi_pin_set_pull(HOST_BURN_UART_TX_PIN, PIN_PULL_MAX);
    uapi_pin_set_ie(HOST_BURN_UART_TX_PIN, PIN_IE_DISABLE);
    uapi_pin_set_mode(HOST_BURN_UART_RX_PIN, PIN_MODE_0);
    uapi_pin_set_ds(HOST_BURN_UART_RX_PIN, PIN_DS_2);
    uapi_pin_set_pull(HOST_BURN_UART_RX_PIN, PIN_PULL_MAX);
    uapi_pin_set_ie(HOST_BURN_UART_RX_PIN, PIN_IE_ENABLE);
}

void host_burn_uart_reinit(uint32_t baud)
{
    host_burn_uart_deinit();
    host_burn_uart_init(baud);
}
#endif
#endif

void sw_debug_uart_init(uint32_t baud_rate)
{
    uart_pin_config_t uart_pins;
    uart_attr_t uart_line_config;
    uart_buffer_config_t uart_buffer_config;

    g_sw_debug_uart = SW_DEBUG_UART_BUS;
    // TX configuration
    uart_pins.tx_pin = CHIP_FIXED_TX_PIN;
    uart_pins.rts_pin = PIN_NONE;

    // RX configuration
    uart_pins.rx_pin = CHIP_FIXED_RX_PIN;
    uart_pins.cts_pin = PIN_NONE;

    uart_line_config.baud_rate = baud_rate;
    uart_line_config.data_bits = UART_DATA_BIT_8;
    uart_line_config.parity = UART_PARITY_NONE;
    uart_line_config.stop_bits = UART_STOP_BIT_1;

    uart_buffer_config.rx_buffer_size = DEBUG_UART_RX_BUFFER_SIZE;
    uart_buffer_config.rx_buffer = g_uart_rx_buffer;

    (void)uapi_uart_init(SW_DEBUG_UART_BUS, &uart_pins, &uart_line_config, NULL, &uart_buffer_config);

    uapi_uart_register_rx_callback(SW_DEBUG_UART_BUS, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                   DEBUG_UART_RX_BUFFER_SIZE, uart_rx_callback);

    g_sw_debug_uart_enabled = true;
}

static print_str_hook g_print_hook_func = NULL;

void uart_print_str_reg_hook(print_str_hook func)
{
    g_print_hook_func = func;
}

void print_str(const char *str, ...)
{
    static uint8_t s[UART_TRANS_LEN_MAX];  // This needs to be large enough to store the string TODO Change magic number
    va_list args;
    int32_t str_len;

    if ((str == NULL) || (strlen(str) == 0)) {
        return;
    }
#ifndef TEST_SUITE
#ifndef CFG_DRIVERS_USB_AS_SERIAL
    if (!g_sw_debug_uart_enabled) {
        return;
    }
#endif
#endif

    va_start(args, str);
    str_len = vsprintf_s((char *)s, sizeof(s), str, args);
    va_end(args);

    if (str_len < 0) {
        return;
    }

    if (g_print_hook_func != NULL && g_print_hook_func(s, (uint32_t)str_len) == ERRCODE_SUCC) {
        return;
    }
#ifdef CFG_DRIVERS_USB_AS_SERIAL
    uapi_usb_serial_write(USB_SERIAL_0, (const uint8_t *)s, str_len);
#else
    uapi_uart_write_nolock(SW_DEBUG_UART_BUS, (const void *)s, str_len, 0);
#endif
}
#endif

#ifdef LOG_SUPPORT
/** UART Settings. Define these in the C file to avoid pulling in the UART header in the header file. */
#ifdef FT_SINGLE_UART
#define LOG_UART_BAUD_RATE 750000
#else
#define LOG_UART_BAUD_RATE 921600
#endif
static uint8_t g_uart_log_rx_buffer[LOG_UART_RX_BUFFER_SIZE];
static uint32_t g_uart_log_baud = LOG_UART_BAUD_RATE;

void log_uart_port_init(void)
{
#ifdef FT_SINGLE_UART
    uapi_uart_deinit(LOG_UART_BUS);
#endif
    uart_pin_config_t log_uart_pins = {
        .tx_pin = LOG_UART_TX_PIN,
        .rx_pin = LOG_UART_RX_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };

    uart_attr_t uart_line_config = {
        .baud_rate = g_uart_log_baud,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };
    uart_buffer_config_t uart_buffer_config;

    uart_buffer_config.rx_buffer_size = LOG_UART_RX_BUFFER_SIZE;
    uart_buffer_config.rx_buffer = g_uart_log_rx_buffer;

    (void)uapi_uart_init(LOG_UART_BUS, &log_uart_pins, &uart_line_config, NULL, &uart_buffer_config);

#ifdef FT_SINGLE_UART
    uapi_uart_register_rx_callback(LOG_UART_BUS, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                   LOG_UART_RX_BUFFER_SIZE, log_factory_at_callback);
#else
#if SYS_DEBUG_MODE_ENABLE == YES
    uapi_uart_register_rx_callback(LOG_UART_BUS, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                   LOG_UART_RX_BUFFER_SIZE, log_uart_rx_callback);
#endif /* SYS_DEBUG_MODE_ENABLE */
#endif /* FT_SINGLE_UART */
}

void log_uart_port_reinit(uint32_t baud)
{
    uapi_uart_deinit(LOG_UART_BUS);
    g_uart_log_baud = baud;
    log_uart_port_init();
}
#endif

void hal_uart_clear_pending(uart_bus_t uart)
{
    switch (uart) {
#if UART_BUS_MAX_NUMBER > 0
        case UART_BUS_0:
            osal_irq_clear(UART_0_IRQN);
            break;
#endif
#if UART_BUS_MAX_NUMBER > 1
        case UART_BUS_1:
            osal_irq_clear(UART_1_IRQN);
            break;
#endif
#if UART_BUS_MAX_NUMBER > 2
        case UART_BUS_2:
            osal_irq_clear(UART_2_IRQN);
            break;
#endif
#if UART_BUS_MAX_NUMBER > 3
        case UART_BUS_3:
            osal_irq_clear(COM_UART_INT_IRQN);
            break;
#endif
        default:
            break;
    }
}

#if defined(CONFIG_UART_SUPPORT_DMA)
#if defined(CONFIG_UART_SUPPORT_INT_TRIGGER_DMA)
void uart_porting_int_enable(uart_bus_t bus)
{
    if (bus >= UART_BUS_MAX_NUM) {
        return;
    }

    osal_irq_enable(g_uart_interrupt_lines[bus].irq_num);
}

void uart_porting_int_disable(uart_bus_t bus)
{
    if (bus >= UART_BUS_MAX_NUM) {
        return;
    }

    osal_irq_disable(g_uart_interrupt_lines[bus].irq_num);
}
#endif  /* CONFIG_UART_SUPPORT_INT_TRIGGER_DMA */

uint8_t uart_port_get_dma_trans_dest_handshaking(uart_bus_t bus)
{
    switch (bus) {
        case UART_BUS_0:
            return (uint8_t)DMA_HANDSHAKE_UART_BUS_0_TX;
#if UART_BUS_MAX_NUMBER > 1
        case UART_BUS_1:
            return (uint8_t)DMA_HANDSHAKE_UART_BUS_1_TX;
#endif
#if UART_BUS_MAX_NUMBER > 2
        case UART_BUS_2:
            return (uint8_t)DMA_HANDSHAKE_UART_BUS_2_TX;
#endif
        default:
            return (uint8_t)HAL_DMA_HANDSHAKING_MAX_NUM;
    }
    return 0;
}

uint8_t uart_port_get_dma_trans_src_handshaking(uart_bus_t bus)
{
    switch (bus) {
        case UART_BUS_0:
            return (uint8_t)DMA_HANDSHAKE_UART_BUS_0_RX;
#if UART_BUS_MAX_NUMBER > 1
        case UART_BUS_1:
            return (uint8_t)DMA_HANDSHAKE_UART_BUS_1_RX;
#endif
#if UART_BUS_MAX_NUMBER > 2
        case UART_BUS_2:
            return (uint8_t)DMA_HANDSHAKE_UART_BUS_2_RX;
#endif
        default:
            return (uint8_t)HAL_DMA_HANDSHAKING_MAX_NUM;
    }
    return 0;
}
#endif  /* CONFIG_UART_SUPPORT_DMA */

uint32_t uart_porting_lock(uart_bus_t bus)
{
    unused(bus);
    return osal_irq_lock();
}

void uart_porting_unlock(uart_bus_t bus, uint32_t irq_sts)
{
    unused(bus);
    osal_irq_restore(irq_sts);
}

void uart_porting_ip_reset(uart_bus_t bus)
{
    unused(bus);
#if CORE == APPS
    switch (bus) {
        case UART_BUS_0:
            regw_clrbit(M_CTL_RB_MTOP3_L_REG, 0, M_CTL_RB_UART_H0_RST_SOFT_N_OFFSET);
            regw_setbit(M_CTL_RB_MTOP3_L_REG, 0, M_CTL_RB_UART_H0_RST_SOFT_N_OFFSET);
            break;
#if UART_BUS_MAX_NUMBER > 1
        case UART_BUS_1:
            regw_clrbit(M_CTL_RB_MTOP1_L_REG, 0, M_CTL_RB_UART_H1_RST_SOFT_N_OFFSET);
            regw_setbit(M_CTL_RB_MTOP1_L_REG, 0, M_CTL_RB_UART_H1_RST_SOFT_N_OFFSET);
            break;
#endif
#if UART_BUS_MAX_NUMBER > 2
        case UART_BUS_2:
            regw_clrbit(M_CTL_RB_MTOP3_L_REG, 0, M_CTL_RB_UART_L0_RST_SOFT_N_OFFSET);
            regw_setbit(M_CTL_RB_MTOP3_L_REG, 0, M_CTL_RB_UART_L0_RST_SOFT_N_OFFSET);
            break;
#endif
#if UART_BUS_MAX_NUMBER > 3
        case UART_BUS_3:
            regw_clrbit(COM_CTL_RB_COM_UART_REG, 0, COM_CTL_RB_COM_UART_SOFT_RST_N_OFFSET);
            regw_setbit(COM_CTL_RB_COM_UART_REG, 0, COM_CTL_RB_COM_UART_SOFT_RST_N_OFFSET);
            break;
#endif
        default:
            break;
    }
#endif
}

#ifdef CONFIG_UART_SUPPORT_LPC
void uart_port_clock_enable(uart_bus_t bus, bool en)
{
    if (en == false) {
        hal_uart_unregister_funcs(bus);
    }
    switch (bus) {
        case UART_BUS_0:
            uapi_clocks_switch(CLOCKS_ID_M_UART_H0, (switch_type_t)en);
            break;
        case UART_BUS_1:
            uapi_clocks_switch(CLOCKS_ID_M_UART_H1, (switch_type_t)en);
            break;
        default:
            break;
    }
}
#endif

#if defined(CONFIG_UART_SUPPORT_RECV_RAW_DATA)
#define PORT_DMA_CH_CONFIG_OFFSET               0x104
#define PORT_DMA_RX_CHANNEL_OFFSET              0x20
uint32_t uart_port_get_dma_rx_dest_addr(dma_channel_t channel)
{
    dma_channel_t real_ch = channel - B_DMA_CHANNEL_MAX_NUM;
    return uapi_reg_read_val32(SDMA_BASE_ADDR + PORT_DMA_CH_CONFIG_OFFSET + (PORT_DMA_RX_CHANNEL_OFFSET * real_ch));
}
#endif
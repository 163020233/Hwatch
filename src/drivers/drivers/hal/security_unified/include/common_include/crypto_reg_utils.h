/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: Provides register access utils for crypto hardware.
 *
 */
#ifndef CRYPTO_REG_UTILS_H
#define CRYPTO_REG_UTILS_H

#include <stdint.h>

/* Register Read&Write Macros. */
#define crypto_reg_read(reg_addr)               (*(volatile uint32_t *)(uintptr_t)(reg_addr))
#define crypto_reg_write(reg_addr, value)       (*(volatile uint32_t *)(uintptr_t)(reg_addr) = (value))
#define crypto_reg_read_u16(reg_addr)           (*(volatile uint16_t *)(uintptr_t)(reg_addr))

/* Register write with fence instruction. */
#ifndef crypto_ex_reg_write
#define crypto_ex_reg_write(addr, val) do {                   \
    (*(volatile uint32_t *)(uintptr_t)(addr) = (val)); \
    __asm__ __volatile__("fence");                   \
} while (0)
#endif

#ifndef crypto_ex_reg_write_u16
#define crypto_ex_reg_write_u16(addr, val) do {                   \
    (*(volatile uint16_t *)(uintptr_t)(addr) = (val)); \
    __asm__ __volatile__("fence");                   \
} while (0)
#endif

/* SPACC Register Access */
#ifndef spacc_reg_read
#define spacc_reg_read(offset)          crypto_reg_read(SPACC_REG_BASE_ADDR + (offset))
#define spacc_reg_write(offset, value)  crypto_ex_reg_write(SPACC_REG_BASE_ADDR + (offset), value)
#endif

/* PKE Register Access */
#ifndef pke_reg_read
#define pke_reg_read(offset)            crypto_reg_read(PKE_REG_BASE_ADDR + (offset))
#define pke_reg_write(offset, value)    crypto_ex_reg_write(PKE_REG_BASE_ADDR + (offset), value)
#endif

/* KM Register Access */
#ifndef km_reg_read
#define km_reg_read(offset)             crypto_reg_read(KM_REG_BASE_ADDR + (offset))
#define km_reg_write(offset, value)     crypto_ex_reg_write(KM_REG_BASE_ADDR + (offset), value)
#endif

/* Key Control Register Access */
#ifndef key_ctrl_reg_read
#define key_ctrl_reg_read(offset)             crypto_reg_read(KEY_CTRL_REG_BASE_ADDR + (offset))
#define key_ctrl_reg_write(offset, value)     crypto_ex_reg_write(KEY_CTRL_REG_BASE_ADDR + (offset), value)
#endif

/* TRNG Register Access */
#ifndef trng_reg_read
#define trng_reg_read(offset)           crypto_reg_read(TRNG_REG_BASE_ADDR + (offset))
#define trng_reg_write(offset, value)   crypto_ex_reg_write(TRNG_REG_BASE_ADDR + (offset), value)
#endif

/* CA Control Register Access */
#ifndef ca_ctrl_reg_read
#define ca_ctrl_reg_read(offset)           crypto_reg_read(CA_CTRL_REG_BASE_ADDR + (offset))
#define ca_ctrl_reg_write(offset, value)   crypto_ex_reg_write(CA_CTRL_REG_BASE_ADDR + (offset), value)
#endif

/* FAPC Register Access */
#ifndef fapc_reg_read
#define fapc_reg_read(offset)           crypto_reg_read(FAPC_REG_BASE_ADDR + (offset))
#define fapc_reg_write(offset, value)   crypto_ex_reg_write(FAPC_REG_BASE_ADDR + (offset), value)
#endif

#endif /* CRYPTO_REG_UTILS_H */
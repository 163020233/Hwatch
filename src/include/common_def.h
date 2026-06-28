/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Common define \n
 *
 */

#ifndef COMMON_DEF_H
#define COMMON_DEF_H
#ifndef __KERNEL__
#include <stdint.h>
#endif

#ifdef CONFIG_REG_FENCE_MODE
#include "arch_barrier.h"
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#ifndef NULL
#define NULL 0L
#endif

#ifndef array_size
#define array_size(x)   (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef min
#define min(x, y)       (((x) < (y)) ? (x) : (y))
#endif

#ifndef max
#define max(x, y)       (((x) > (y)) ? (x) : (y))
#endif

#ifdef __GNUC__
#ifndef likely
#define likely(x)       __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x)     __builtin_expect(!!(x), 0)
#endif
#else
#ifndef likely
#define likely(x)       (x)
#endif
#ifndef unlikely
#define unlikely(x)     (x)
#endif
#endif

#ifndef unused
#define unused(var)     (void)(var)
#endif


#ifndef STATIC
#if (defined(CONFIG_ROM_COMPILE) || defined(UT_TEST))
#define STATIC
#else
#define STATIC static
#endif
#endif

#ifndef INLINE
#if (defined(CONFIG_ROM_COMPILE) || defined(UT_TEST))
#define INLINE  static inline
#else
#define INLINE  inline
#endif
#endif


#ifndef bit
#define bit(x)              (1UL << (uint32_t)(x))
#endif


#define makeu16(a, b)       ((uint16_t)(((uint8_t)(a)) | ((uint16_t)((uint8_t)(b))) << 8))
#define makeu32(a, b)       ((uint32_t)(((uint16_t)(a)) | ((uint32_t)((uint16_t)(b))) << 16))
#define makeu64(a, b)       ((uint64_t)(((uint32_t)(a)) | ((uint64_t)((uint32_t)(b))) << 32))
#define joinu32(a, b, c, d) makeu32(makeu16((a), (b)), makeu16((c), (d)))

#define hiu32(x)            ((uint32_t)(((uint64_t)(x) >> 32) & 0xFFFFFFFF))
#define lou32(x)            ((uint32_t)(x))

#define hiu16(x)            ((uint16_t)(((uint32_t)(x) >> 16) & 0xFFFF))
#define lou16(x)            ((uint16_t)(x))

#define hiu8(x)             ((uint8_t)(((uint16_t)(x) >> 8) & 0xFF))
#define lou8(x)             ((uint8_t)(x))

#define uapi_max(a, b)      (((a) > (b)) ? (a) : (b))
#define uapi_min(a, b)      (((a) < (b)) ? (a) : (b))

#ifdef CONFIG_REG_FENCE_MODE
#define reg_dsb()  dsb()
#else
#define reg_dsb()
#endif

#define uapi_reg_write(addr, val)              ({ *(volatile unsigned int *)(uintptr_t)(addr) = (val); reg_dsb(); })
#define uapi_reg_read(addr, val)               ({ reg_dsb(); (val) = *(volatile unsigned int *)(uintptr_t)(addr); })
#define uapi_reg_write32(addr, val)            ({ *(volatile unsigned int *)(uintptr_t)(addr) = (val); reg_dsb(); })
#define uapi_reg_read32(addr, val)             ({ reg_dsb(); (val) = *(volatile unsigned int *)(uintptr_t)(addr); })
#define uapi_reg_read_val32(addr)              ({ reg_dsb(); unsigned int read_val = *(volatile unsigned int *)(uintptr_t)(addr); read_val; })
#define uapi_reg_setbitmsk(addr, msk)          ({ *(volatile unsigned int *)(uintptr_t)(addr) |= (msk); reg_dsb(); })
#define uapi_reg_clrbitmsk(addr, msk)          ({ *(volatile unsigned int *)(uintptr_t)(addr) &= ~(msk); reg_dsb(); })
#define uapi_reg_clrbit(addr, pos)             ({ *(volatile unsigned int *)(uintptr_t)(addr) &= ~((unsigned int)(1) << (pos)); reg_dsb(); })
#define uapi_reg_setbit(addr, pos)             ({ *(volatile unsigned int *)(uintptr_t)(addr) |= ((unsigned int)(1) << (pos)); reg_dsb(); })
#define uapi_reg_clrbits(addr, pos, bits)      ({ *(volatile unsigned int *)(uintptr_t)(addr) &= ~((((unsigned int)1 << (bits)) - 1) << (pos)); reg_dsb(); })
#define uapi_reg_setbits(addr, pos, bits, val) ({ *(volatile unsigned int *)(uintptr_t)(addr) =           \
    (*(volatile unsigned int *)(uintptr_t)(addr) & (~((((unsigned int)1 << (bits)) - 1) << (pos)))) | \
    ((unsigned int)((val) & (((unsigned int)1 << (bits)) - 1)) << (pos)); reg_dsb(); })
#define uapi_reg_getbits(addr, pos, bits)      ({ reg_dsb(); (*(volatile unsigned int *)(uintptr_t)(addr) >> (pos)) & (((unsigned int)1 << (bits)) - 1); })

#define uapi_reg_write16(addr, val)        ({ *(volatile unsigned short *)(uintptr_t)(addr) = (val); reg_dsb(); })
#define uapi_reg_read16(addr, val)         ({ reg_dsb(); (val) = *(volatile unsigned short *)(uintptr_t)(addr); })
#define uapi_reg_read_val16(addr)          ({ reg_dsb(); unsigned short read_val = *(volatile unsigned short *)(uintptr_t)(addr); read_val; })
#define uapi_reg_clrbit16(addr, pos)       ({ *(volatile unsigned short *)(uintptr_t)(addr) &= ~((unsigned short)(1) << (pos)); reg_dsb(); })
#define uapi_reg_setbit16(addr, pos)       ({ *(volatile unsigned short *)(uintptr_t)(addr) |= ((unsigned short)(1) << (pos)); reg_dsb(); })
#define uapi_reg_clrbits16(addr, pos, bits) ({ *(volatile unsigned short *)(uintptr_t)(addr) &= \
    ~((((unsigned short)1 << (bits)) - 1) << (pos)); reg_dsb(); })
#define uapi_reg_setbits16(addr, pos, bits, val) ({ *(volatile unsigned short *)(uintptr_t)(addr) =           \
    (*(volatile unsigned short *)(uintptr_t)(addr) & (~((((unsigned short)1 << (bits)) - 1) << (pos)))) | \
    ((unsigned short)((val) & (((unsigned short)1 << (bits)) - 1)) << (pos)); reg_dsb(); })
#define uapi_reg_getbits16(addr, pos, bits) ({ reg_dsb(); (*(volatile unsigned short *)(uintptr_t)(addr) >> (pos)) & (((unsigned short)1 << (bits)) - 1); })

#define uapi_reg_write8(addr, val)        ({ *(volatile unsigned char *)(uintptr_t)(addr) = (val); reg_dsb(); })
#define uapi_reg_read8(addr, val)         ({ reg_dsb(); (val) = *(volatile unsigned char *)(uintptr_t)(addr); })
#define uapi_reg_read_val8(addr)          ({ reg_dsb(); unsigned char read_val = *(volatile unsigned char *)(uintptr_t)(addr); read_val; })
#define uapi_reg_clrbit8(addr, pos)       ({ *(volatile unsigned char *)(uintptr_t)(addr) &= ~((unsigned char)(1) << (pos)); reg_dsb(); })
#define uapi_reg_setbit8(addr, pos)       ({ *(volatile unsigned char *)(uintptr_t)(addr) |= ((unsigned char)(1) << (pos)); reg_dsb(); })
#define uapi_reg_clrbits8(addr, pos, bits) ({ *(volatile unsigned char *)(uintptr_t)(addr) &= ~((((unsigned char)1 << (bits)) - 1) << (pos)); reg_dsb(); })
#define uapi_reg_setbits8(addr, pos, bits, val) ({ *(volatile unsigned char *)(uintptr_t)(addr) =           \
    (*(volatile unsigned char *)(uintptr_t)(addr) & (~((((unsigned char)1 << (bits)) - 1) << (pos)))) | \
    ((unsigned char)((val) & (((unsigned char)1 << (bits)) - 1)) << (pos)); reg_dsb(); })
#define uapi_reg_getbits8(addr, pos, bits) ({ reg_dsb(); (*(volatile unsigned char *)(uintptr_t)(addr) >> (pos)) & (((unsigned char)1 << (bits)) - 1); })

/**
 * @if Eng
 * @brief   The offset of member in the structrue.
 * @else
 * @brief   结构体成员偏移
 * @endif
 */
#define fieldoffset(s, m)   ((uint32_t) &(((s *)0)->m))


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

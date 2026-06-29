/*-
 * Copyright (c) 2008 Alexander Motin <mav@FreeBSD.org>
 * All rights reserved.
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DEV_MMC_HCI_H
#define DEV_MMC_HCI_H

#include "emmc_type.h"
#include "chip_io.h"
#include "debug_print.h"
#include "tcxo.h"

#define SDHCI_CLOCK_CONTROL 0x2C
#define SDHCI_DIVIDER_MASK 0xff
#define SDHCI_DIVIDER_MASK_LEN 8
#define SDHCI_DIVIDER_SHIFT 8
#define SDHCI_DIVIDER_HI_MASK 3
#define SDHCI_DIVIDER_HI_SHIFT 6
#define SDHCI_CLOCK_PLL_EN 0x0008
#define SDHCI_CLOCK_CARD_EN 0x0004
#define SDHCI_CLOCK_INT_STABLE 0x0002
#define SDHCI_CLOCK_INT_EN 0x0001

#define SDIO_EMMC_CCLK_EN BIT(0)
#define SDIO_EMMC_ORI_DIV_EN BIT(1)

#define SDIO_DIV_NUM_MASK 0xF8 /* bit3---bit7 */
#define EMMC_DIV_NUM_MASK 0x70 /* bit4---bit6 */

#define SDIO_DIV_NUM_POST_SHIFT 3
#define SDIO_EMMC_DIV_NUM_POST_SHIFT 4

#define EMMC_CCLK_LOAD_DIV_EN BIT(1)
#define EMMC_EMMC_CCLK_EN BIT(0)

#define SDIOM_SRC_CLK_LOW_FREQ_VAL 32000000     /* 32MHz */
#define SDIOM_SRC_CLK_FREQ_LVL_64M_VAL 64000000 /* 64MHz */
#define SDIOM_SRC_CLK_FREQ_LVL_49M_VAL 49000000
#define SDIOM_SRC_CLK_FREQ_LVL_98M_VAL 98000000   /* 98MHz */
#define SDIOM_SRC_CLK_HIGH_FREQ_LVL_VAL 294000000 /* 294MHz */

#define DWC_MSHC_PTR_VENDOR1 0x500
#define SDHCI_AXI_MBIIU_CTRL (DWC_MSHC_PTR_VENDOR1 + 0x10)

#define SDHCI_AXI_MBIIU_BURST_INCR16_EN 8

#define EMMC_REG_BASE_ADDESS 0x52061000
#define HOST_CTRL2 (EMMC_REG_BASE_ADDESS + 0x3E)
#define SAMPLE_CLK_SEL_BIT 0x7
#define AT_STAT (EMMC_REG_BASE_ADDESS + DWC_MSHC_PTR_VENDOR1 + 0x44)

#define SET_CMD_TIMEOUT_START_POS 24
#define SET_CMD_TIMEOUT_LEN 3

#define SDIO_HOST_REG_BASE_ADDESS 0x52060000
#define SDIO_HOST_REG_END_ADDRESS 0x52060FFF

#define SDHCI_EMMC_CTRL 0x52c
#define SDHCI_EMMC_CTRL_EMMC 0x1
#define SDHCI_EMMC_CTRL_ENH_STROBE_EN (1U << 8)

#define CONFIG_MMC0_CCLK_MIN 400000 /* 400KHz */
#define CONFIG_MMC1_CCLK_MIN 100000 /* 100KHz */

#define HCI_SLOTTYPE_REMOVABLE 0x00000000 /* used for SD slot */
#define HCI_SLOTTYPE_EMBEDDED 0x40000000  /* used for EMMC slot */
#define HCI_SLOTTYPE_SDIO 0x80000000

#define HCI_CAN_DO_8BITBUS 0x00040000

/* Controller doesn't honor resets unless we touch the clock register */
#define SDHCI_QUIRK_CLOCK_BEFORE_RESET (1 << 0)
/* Controller really supports DMA */
#define SDHCI_QUIRK_FORCE_DMA (1 << 1)
/* Controller has unusable DMA engine */
#define SDHCI_QUIRK_BROKEN_DMA (1 << 2)
/* Controller doesn't like to be reset when there is no card inserted. */
#define SDHCI_QUIRK_NO_CARD_NO_RESET (1 << 3)
/* Controller has flaky internal state so reset it on each ios change */
#define SDHCI_QUIRK_RESET_ON_IOS (1 << 4)
/* Controller can only DMA chunk sizes that are a multiple of 32 bits */
#define SDHCI_QUIRK_32BIT_DMA_SIZE (1 << 5)
/* Controller needs to be reset after each request to stay stable */
#define SDHCI_QUIRK_RESET_AFTER_REQUEST (1 << 6)
/* Controller has an off-by-one issue with timeout value */
#define SDHCI_QUIRK_INCR_TIMEOUT_CONTROL (1 << 7)
/* Controller has broken read timings */
#define SDHCI_QUIRK_BROKEN_TIMINGS (1 << 8)
/* Controller needs lowered frequency */
#define SDHCI_QUIRK_LOWER_FREQUENCY (1 << 9)
/* Data timeout is invalid, should use SD clock */
#define SDHCI_QUIRK_DATA_TIMEOUT_USES_SDCLK (1 << 10)
/* Timeout value is invalid, should be overriden */
#define SDHCI_QUIRK_BROKEN_TIMEOUT_VAL (1 << 11)
/* SDHCI_CAPABILITIES is invalid */
#define SDHCI_QUIRK_MISSING_CAPS (1 << 12)
/* Hardware shifts the 136-bit response, don't do it in software. */
#define SDHCI_QUIRK_DONT_SHIFT_RESPONSE (1 << 13)
/* Wait to see reset bit asserted before waiting for de-asserted  */
#define SDHCI_QUIRK_WAITFOR_RESET_ASSERTED (1 << 14)
/* Leave controller in standard mode when putting card in HS mode. */
#define SDHCI_QUIRK_DONT_SET_HISPD_BIT (1 << 15)
/* Alternate clock source is required when supplying a 400 KHz clock. */
#define SDHCI_QUIRK_BCM577XX_400KHZ_CLKSRC (1 << 16)
/* Card insert/remove interrupts don't work, polling required. */
#define SDHCI_QUIRK_POLL_CARD_PRESENT (1 << 17)
/* All controller slots are non-removable. */
#define SDHCI_QUIRK_ALL_SLOTS_NON_REMOVABLE (1 << 18)
/* Issue some customers only need 1-width data bus. */
#define SDHCI_QUIRK_ONLY_1_BIT_DATA (1 << 19)
/* Data timeout is invalid, use 1 MHz clock instead. */
#define SDHCI_QUIRK_DATA_TIMEOUT_1MHZ (1 << 20)
/* Controller support 3.3v only. */
#define SDHCI_QUIRK_SIG_3V3_ONLY (1 << 21)
/* Controller waits for busy responses. */
#define SDHCI_QUIRK_WAIT_WHILE_BUSY (1 << 22)
/* Controller supports eMMC DDR52 mode. */
#define SDHCI_QUIRK_MMC_DDR52 (1 << 23)
/* Controller support for UHS DDR50 mode is broken. */
#define SDHCI_QUIRK_BROKEN_UHS_DDR50 (1 << 24)
/* Controller support for eMMC HS200 mode is broken. */
#define SDHCI_QUIRK_BROKEN_MMC_HS200 (1 << 25)
/* Controller reports support for eMMC HS400 mode as SDHCI_CAN_MMC_HS400. */
#define SDHCI_QUIRK_CAPS_BIT63_FOR_MMC_HS400 (1 << 26)
/* Controller support for SDHCI_CTRL2_PRESET_VALUE is broken. */
#define SDHCI_QUIRK_PRESET_VALUE_BROKEN (1 << 27)
/* Controller does not support or the support for ACMD12 is broken. */
#define SDHCI_QUIRK_BROKEN_AUTO_STOP (1 << 28)
/* Controller may support both 1v8, 1v2, 3v3 but over write it cus hw-limit. */
#define SDHCI_QUIRK_SIG_1V8_ONLY (1 << 29)
/* Controller read-only high-level validity. */
#define SDHCI_QUIRK_WRITE_PROTECT (1 << 30)
/* Controller does not support or the support for ACMD23 is broken. */
#define SDHCI_QUIRK_BROKEN_ACMD23 (1 << 31)

#define HCI_CAN_DO_ADMA2_64BIT 0x10000000
#define HCI_CAN_ASYNC_INTR 0x20000000
#define HCI_SLOTTYPE_MASK 0xC0000000

#define HCI_CAPABILITIES 0x40
#define HCI_TIMEOUT_CLK_MASK 0x0000003F
#define HCI_TIMEOUT_CLK_SHIFT 0
#define HCI_TIMEOUT_CLK_UNIT 0x00000080
#define HCI_CLOCK_BASE_MASK 0x00003F00
#define HCI_CLOCK_V3_BASE_MASK 0x0000FF00
#define HCI_CLOCK_BASE_SHIFT 8
#define HCI_MAX_BLOCK_MASK 0x00030000
#define HCI_MAX_BLOCK_SHIFT 16
#define HCI_CAN_DO_8BITBUS 0x00040000
#define HCI_CAN_DO_ADMA2 0x00080000
#define HCI_CAN_DO_HISPD 0x00200000
#define HCI_CAN_DO_SDMA 0x00400000
#define HCI_CAN_DO_SUSPEND 0x00800000
#define HCI_CAN_VDD_330 0x01000000
#define HCI_CAN_VDD_300 0x02000000
#define HCI_CAN_VDD_180 0x04000000

#define HCI_CAPABILITIES2 0x44
#define HCI_CAN_SDR50 0x00000001
#define HCI_CAN_SDR104 0x00000002
#define HCI_CAN_HS200 HCI_CAN_SDR104
#define HCI_CAN_DDR50 0x00000004
#define HCI_CAN_DRIVE_TYPE_A 0x00000010
#define HCI_CAN_DRIVE_TYPE_C 0x00000020
#define HCI_CAN_DRIVE_TYPE_D 0x00000040
#define HCI_RETUNE_CNT_MASK 0x00000F00
#define HCI_RETUNE_CNT_SHIFT 8
#define HCI_TUNE_SDR50 0x00002000
#define HCI_RETUNE_MODES_MASK 0x0000C000
#define HCI_RETUNE_MODES_SHIFT 14
#define HCI_CLOCK_MULT_MASK 0x00FF0000
#define HCI_CLOCK_MULT_SHIFT 16
#define MCI_CAN_DO_IDMAC 0x01000000       /* non-standard */
#define MCI_CAN_DO_IDMAC_64BIT 0x02000000 /* non-standard */
#define HCI_CAN_MMC_HS400 0x80000000      /* non-standard */
#define HCI_CAN_MMC_HS400ES 0x40000000    /* non-standard */

#define SDHCI_CAPABILITIES HCI_CAPABILITIES
#define SDHCI_TIMEOUT_CLK_MASK HCI_TIMEOUT_CLK_MASK
#define SDHCI_TIMEOUT_CLK_SHIFT HCI_TIMEOUT_CLK_SHIFT
#define SDHCI_TIMEOUT_CLK_UNIT HCI_TIMEOUT_CLK_UNIT
#define SDHCI_CLOCK_BASE_MASK HCI_CLOCK_BASE_MASK
#define SDHCI_CLOCK_V3_BASE_MASK HCI_CLOCK_V3_BASE_MASK
#define SDHCI_CLOCK_BASE_SHIFT HCI_CLOCK_BASE_SHIFT
#define SDHCI_MAX_BLOCK_MASK HCI_MAX_BLOCK_MASK
#define SDHCI_MAX_BLOCK_SHIFT HCI_MAX_BLOCK_SHIFT
#define SDHCI_CAN_DO_8BITBUS HCI_CAN_DO_8BITBUS
#define SDHCI_CAN_DO_ADMA2 HCI_CAN_DO_ADMA2
#define SDHCI_CAN_DO_HISPD HCI_CAN_DO_HISPD
#define SDHCI_CAN_DO_SDMA HCI_CAN_DO_SDMA
#define SDHCI_CAN_DO_SUSPEND HCI_CAN_DO_SUSPEND
#define SDHCI_CAN_VDD_330 HCI_CAN_VDD_330
#define SDHCI_CAN_VDD_300 HCI_CAN_VDD_300
#define SDHCI_CAN_VDD_180 HCI_CAN_VDD_180
#define SDHCI_CAN_DO_ADMA2_64BIT HCI_CAN_DO_ADMA2_64BIT

#define SDHCI_CAN_ASYNC_INTR HCI_CAN_ASYNC_INTR

#define SDHCI_SLOTTYPE_MASK HCI_SLOTTYPE_MASK

#define SDHCI_SLOTTYPE_REMOVABLE HCI_SLOTTYPE_REMOVABLE /* used for SD slot */
#define SDHCI_SLOTTYPE_EMBEDDED HCI_SLOTTYPE_EMBEDDED   /* used for EMMC slot */
#define SDHCI_SLOTTYPE_SDIO HCI_SLOTTYPE_SDIO

#define SDHCI_CAPABILITIES2 HCI_CAPABILITIES2
#define SDHCI_CAN_SDR50 HCI_CAN_SDR50
#define SDHCI_CAN_SDR104 HCI_CAN_SDR104
#define SDHCI_CAN_DDR50 HCI_CAN_DDR50
#define SDHCI_CAN_DRIVE_TYPE_A HCI_CAN_DRIVE_TYPE_A
#define SDHCI_CAN_DRIVE_TYPE_C HCI_CAN_DRIVE_TYPE_C
#define SDHCI_CAN_DRIVE_TYPE_D HCI_CAN_DRIVE_TYPE_D
#define SDHCI_RETUNE_CNT_MASK HCI_RETUNE_CNT_MASK
#define SDHCI_RETUNE_CNT_SHIFT HCI_RETUNE_CNT_SHIFT
#define SDHCI_TUNE_SDR50 HCI_TUNE_SDR50
#define SDHCI_RETUNE_MODES_MASK HCI_RETUNE_MODES_MASK
#define SDHCI_RETUNE_MODES_SHIFT HCI_RETUNE_MODES_SHIFT
#define SDHCI_CLOCK_MULT_MASK HCI_CLOCK_MULT_MASK
#define SDHCI_CLOCK_MULT_SHIFT HCI_CLOCK_MULT_SHIFT
#define SDHCI_CAN_MMC_HS400 HCI_CAN_MMC_HS400
#define SDHCI_CAN_MMC_HS400ES HCI_CAN_MMC_HS400ES

#define SDHCI_EMMC_HW_RESET 0x534
#define SDHCI_AUTO_TUNING_CTRL 0x540
#define SDHCI_SW_TUNING_EN 0x00000010
#define SDHCI_AUTO_TUNING_STATUS 0x544

#define SDHCI_CENTER_PH_CODE_MASK 0x000000ff
#define SDHCI_PHASE_SEL_MASK 0xff

#define SDHCI_POWER_CONTROL 0x29
#define SDHCI_POWER_ON 0x01
#define SDHCI_POWER_180 0x0A
#define SDHCI_POWER_300 0x0C
#define SDHCI_POWER_330 0x0E

#define SDHCI_HOST_CONTROL2 0x3E
#define SDHCI_CTRL2_PRESET_VALUE 0x8000
#define SDHCI_CTRL2_ASYNC_INTR 0x4000
#define SDHCI_CTRL2_64BIT_ENABLE 0x2000
#define SDHCI_CTRL2_HOST_V4_ENABLE 0x1000
#define SDHCI_CTRL2_CMD23_ENABLE 0x0800
#define SDHCI_CTRL2_ADMA2_LENGTH_MODE 0x0400
#define SDHCI_CTRL2_UHS2_IFACE_ENABLE 0x0100
#define SDHCI_CTRL2_SAMPLING_CLOCK 0x0080
#define SDHCI_CTRL2_EXEC_TUNING 0x0040
#define SDHCI_CTRL2_DRIVER_TYPE_MASK 0x0030
#define SDHCI_CTRL2_DRIVER_TYPE_B 0x0000
#define SDHCI_CTRL2_DRIVER_TYPE_A 0x0010
#define SDHCI_CTRL2_DRIVER_TYPE_C 0x0020
#define SDHCI_CTRL2_DRIVER_TYPE_D 0x0030
#define SDHCI_CTRL2_S18_ENABLE 0x0008
#define SDHCI_CTRL2_UHS_MASK 0x0007
#define SDHCI_CTRL2_UHS_SDR12 0x0000
#define SDHCI_CTRL2_UHS_SDR25 0x0001
#define SDHCI_CTRL2_UHS_SDR50 0x0002
#define SDHCI_CTRL2_UHS_SDR104 0x0003
#define SDHCI_CTRL2_UHS_DDR50 0x0004
#define SDHCI_CTRL2_MMC_HS400 0x0007   /* non-standard */
#define SDHCI_CTRL2_MMC_HS400ES 0x0007 /* HISI USED only */

#ifndef CACHE_ALIGNED_SIZE
#define CACHE_ALIGNED_SIZE 32
#endif

#define ICMAINT 0x7C2
#define DCMAINT 0x7C3
#define ICINCVA 0x7C4
#define DCINCVA 0x7C5

#define VA 0x1
#define ICIV (0x1U << 2)
#define DCIV (0x1U << 2)
#define DCC (0x1U << 3)

#define ICACHE_BY_ALL ICIV
#define ICACHE_BY_VA (ICIV | VA)

#define DCACHE_INV_BY_VA (DCIV | VA)
#define DCACHE_INV_BY_ALL DCIV
#define DCACHE_CLEAN_ALL DCC
#define DCACHE_CLEAN_BY_VA (DCC | VA)
#define DCACHE_FLUSH_BY_VA (DCC | DCIV | VA)
#define DCACHE_FLUSH_ALL (DCC | DCIV)

#define APREFI 0x7C6
#define APREFD 0x7C7
#define IAPEN 0x1
#define SAPEN (0x1U << 4)

#define CACHE_LINE_SIZE 64U

#define HCI_EVENT_PLUGGED (1U << 0)
#define HCI_EVENT_UNPLUGGED (1U << 1)

#ifndef LITE_OS_SEC_TEXT
#define LITE_OS_SEC_TEXT /* __attribute__((section(".text.sram"))) */
#endif

#define WRITE_CUSTOM_CSR_VAL(csrRegAddr, csrVal)       \
    do {                                               \
        if (__builtin_constant_p(csrVal)) {            \
            asm volatile("li t0,"                      \
                         "%0"                          \
                         :                             \
                         : "i"(csrVal));               \
        } else {                                       \
            asm volatile("mv t0,"                      \
                         "%0"                          \
                         :                             \
                         : "r"(csrVal));               \
        }                                              \
        asm volatile("csrw %0, t0" ::"i"(csrRegAddr)); \
    } while (0)

#define DCCTL 0x7C1
#define DCCTL_ENABLE 0x1

typedef enum {
    CACHE_4KB = 0,
    CACHE_8KB = 2,
    CACHE_16KB = 4,
    CACHE_32KB = 6,
    CACHE_64KB = 8,
} CacheSize;

#define MB() __asm__ __volatile__("fence" ::: "memory")

#define DSB() MB()

#ifndef mb
#define mb MB
#endif

#ifndef dsb
#define dsb DSB
#endif

#ifndef ArchWrite8
#define ArchWrite8(addr, value) ArchWrite8(addr, value)
static inline void ArchWrite8(uintptr_t addr, uint8_t value)
{
    *(volatile uint8_t *)addr = value;
}
#endif

#ifndef ArchWrite16
#define ArchWrite16(addr, value) ArchWrite16(addr, value)
static inline void ArchWrite16(uintptr_t addr, uint16_t value)
{
    *(volatile uint16_t *)addr = value;
}
#endif

#ifndef ArchWrite32
#define ArchWrite32(addr, value) ArchWrite32(addr, value)
static inline void ArchWrite32(uintptr_t addr, uint32_t value)
{
    *(volatile uint32_t *)addr = value;
}
#endif

#ifndef ArchWrite64
#define ArchWrite64(addr, value) ArchWrite64(addr, value)
static inline void ArchWrite64(uintptr_t addr, uint64_t value)
{
    *(volatile uint64_t *)addr = value;
}
#endif

#ifndef ArchRead8
#define ArchRead8(addr) ArchRead8(addr)
static inline uint8_t ArchRead8(uintptr_t addr)
{
    return *(volatile uint8_t *)addr;
}
#endif

#ifndef ArchRead16
#define ArchRead16(addr) ArchRead16(addr)
static inline uint16_t ArchRead16(uintptr_t addr)
{
    return *(volatile uint16_t *)addr;
}
#endif

#ifndef ArchRead32
#define ArchRead32(addr) ArchRead32(addr)
static inline uint32_t ArchRead32(uintptr_t addr)
{
    return *(volatile uint32_t *)addr;
}
#endif

#ifndef ArchRead64
#define ArchRead64(addr) ArchRead64(addr)
static inline uint64_t ArchRead64(uintptr_t addr)
{
    return *(volatile uint64_t *)addr;
}
#endif

#define WRITE8(addr, value) ArchWrite8((uintptr_t)(addr), value)

#define WRITE16(addr, value) ArchWrite16((uintptr_t)(addr), value)

#define WRITE32(addr, value) ArchWrite32((uintptr_t)(addr), value)

#define WRITE64(addr, value) ArchWrite64((uintptr_t)(addr), value)

#define READ8_MB(addr)                            \
    ({                                            \
        uint8_t r = ArchRead8((uintptr_t)(addr)); \
        dsb();                                    \
        r;                                        \
    })

#define READ16_MB(addr)                             \
    ({                                              \
        uint16_t r = ArchRead16((uintptr_t)(addr)); \
        dsb();                                      \
        r;                                          \
    })

#define READ32_MB(addr)                             \
    ({                                              \
        uint32_t r = ArchRead32((uintptr_t)(addr)); \
        dsb();                                      \
        r;                                          \
    })

#define READ64_MB(addr)                             \
    ({                                              \
        uint64_t r = ArchRead64((uintptr_t)(addr)); \
        dsb();                                      \
        r;                                          \
    })

#define WRITE8_MB(addr, value)            \
    {                                     \
        dsb();                            \
        WRITE8((uintptr_t)(addr), value); \
    }

#define WRITE16_MB(addr, value)            \
    {                                      \
        dsb();                             \
        WRITE16((uintptr_t)(addr), value); \
    }

#define WRITE32_MB(addr, value)            \
    {                                      \
        dsb();                             \
        WRITE32((uintptr_t)(addr), value); \
    }

#define WRITE64_MB(addr, value)            \
    {                                      \
        dsb();                             \
        WRITE64((uintptr_t)(addr), value); \
    }

/**
 * Write a uint8_t value to addr.
 */
#define WRITE_UINT8(value, addr) WRITE8_MB(addr, value)

/**
 * Write a uint16_t value to addr.
 */
#define WRITE_UINT16(value, addr) WRITE16_MB(addr, value)

/**
 * Write a uint32_t value to addr.
 */
#define WRITE_UINT32(value, addr) WRITE32_MB(addr, value)

/**
 * Write a uint64_t addr to addr.
 */
#define WRITE_UINT64(value, addr) WRITE64_MB(addr, value)

/**
 * Get a uint8_t value from addr.
 */
#define GET_UINT8(addr) READ8_MB(addr)

/**
 * Get a uint16_t value from addr.
 */
#define GET_UINT16(addr) READ16_MB(addr)

/**
 * Get a uint32_t value from addr.
 */
#define GET_UINT32(addr) READ32_MB(addr)

/**
 * Get a uint64_t value from addr.
 */
#define GET_UINT64(addr) READ64_MB(addr)

static __inline uint8_t hci_readb(const struct hci_softc *sc, uintptr_t off)
{
    return GET_UINT8(sc->base + off);
}

static __inline void hci_writeb(const struct hci_softc *sc, uintptr_t off, uint32_t v)
{
    WRITE_UINT8(v, sc->base + off);
}

static __inline uint16_t hci_readw(const struct hci_softc *sc, uintptr_t off)
{
    return GET_UINT16(sc->base + off);
}

static __inline void hci_writew(const struct hci_softc *sc, uintptr_t off, uint32_t v)
{
    WRITE_UINT16(v, sc->base + off);
}

static __inline uint32_t hci_readl(const struct hci_softc *sc, uintptr_t off)
{
    return GET_UINT32(sc->base + off);
}

static __inline void hci_writel(const struct hci_softc *sc, uintptr_t off, uint32_t v)
{
    WRITE_UINT32(v, sc->base + off);
}

static __inline void hci_setl(const struct hci_softc *sc, uint32_t off, uint32_t bits)
{
    hci_writel(sc, off, hci_readl(sc, off) | bits);
}

static __inline void hci_clearl(const struct hci_softc *sc, uint32_t off, uint32_t bits)
{
    hci_writel(sc, off, hci_readl(sc, off) & (~bits));
}

static inline void device_set_nameunit(device_t dev, const char *name)
{
    if (dev == NULL)
        return;
    dev->name = name;
}

static const char unknown_string[] = {"unknown"};
static inline const char *device_get_nameunit(device_t dev)
{
    if (dev && dev->name) {
        return (dev->name);
    }
    return (unknown_string);
}

/* DMA memory */
#define MMC_DMA_VADDR_TO_PADDR(x) x
#define MMC_DMA_CACHE 0
#define mmc_dma_mem_alloc(pool, size, boundary, type) memalign(boundary, size)
#define mmc_dma_mem_free(addr) free(addr)
#define mmc_dcache_flush_range(start, end) dma_cache_clean1(start, end)
#define mmc_dcache_inv_range(start, end) dma_cache_inv1(start, end)

#define device_printf(dev, fmt, ...) PRINT("%s: " fmt, device_get_nameunit(dev), ##__VA_ARGS__)

#define slot_printf(slot, fmt, args...) PRINT("%s-slot%u: " fmt, device_get_nameunit(slot->bus), slot->num, ##args)

#define dev_loge(dev, fmt, args...) PRINT("[%s|ERROR] " fmt, device_get_nameunit(dev), ##args)
#define dev_logw(dev, fmt, args...) PRINT("[%s|WARN] " fmt, device_get_nameunit(dev), ##args)
#define mmc_printf(fmt, args...) PRINT(fmt, ##args)
#define mmc_trace(fmt, args...)                   \
    do {                                          \
        PRINT("%s:%d: ", __FUNCTION__, __LINE__); \
        PRINT(fmt, ##args);                       \
    } while (0)
#define mmc_err(fmt, args...)                             \
    do {                                                  \
        PRINT("[ERROR] %s:%d: ", __FUNCTION__, __LINE__); \
        PRINT(fmt, ##args);                               \
    } while (0)

#ifdef MMC_TRACE_LEVEL
#define dev_logi(dev, fmt, args...) PRINT("[%s|INFO] " fmt, device_get_nameunit(dev), ##args)
#define dev_logd(dev, fmt, args...) \
    PRINT("[%s|%s@%s,%d] " fmt, device_get_nameunit(dev), __FUNCTION__, __FILE__, __LINE__, ##args)
#define dev_logt(dev, fmt, args...) \
    PRINT("[%s|%s@%s,%d] " fmt, device_get_nameunit(dev), __FUNCTION__, __FILE__, __LINE__, ##args)
#else
#define dev_logi(dev, fmt, args...)
#define dev_logd(dev, fmt, args...)
#define dev_logt(dev, fmt, args...)
#endif

#define LOS_ASSERT(judge)                                                          \
    do {                                                                           \
        if ((uintptr_t)(judge) == 0) {                                             \
            PRINT("ASSERT ERROR! %s, %d, %s\n", __FILE__, __LINE__, __FUNCTION__); \
        }                                                                          \
    } while (0)

#define PRINT_ERR PRINT

#define mmc_err(fmt, args...)                             \
    do {                                                  \
        PRINT("[ERROR] %s:%d: ", __FUNCTION__, __LINE__); \
        PRINT(fmt, ##args);                               \
    } while (0)

void init_slot_methods(int slot, hci_slot_ops *slot_ops);
void hci_dma_inv_cache(void *addr, uint32_t size);
void hci_dma_clean_cache(void *addr, uint32_t size);
LITE_OS_SEC_TEXT uint32_t ArchDCacheEnable(CacheSize dcclSize);
#endif
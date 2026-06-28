/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides dma port \n
 *
 * History: \n
 * 2022-10-16， Create file. \n
 */

#include "common_def.h"
#include "interrupt/osal_interrupt.h"
#include "platform_core.h"
#include "chip_core_irq.h"
#include "hal_dmac_v151.h"
#include "dma_porting.h"
#include "pm_veto.h"

uintptr_t g_dma_base_addr = (uintptr_t)DMA_BASE_ADDR;
#if defined(CONFIG_DMA_SUPPORT_SMDMA)
uintptr_t g_sdma_base_addr = (uintptr_t)SDMA_BASE_ADDR;
#endif
uint8_t chn_sreq_info[DMA_CHANNEL_MAX_NUM];
uint8_t chn_dreq_info[DMA_CHANNEL_MAX_NUM];

static void irq_dma_handler(void)
{
    hal_dma_v151_irq_handler();
}

#if defined(CONFIG_DMA_SUPPORT_SMDMA)
static void irq_sdma_handler(void)
{
    hal_sdma_v151_irq_handler();
}
#endif

void dma_port_register_irq(void)
{
    osal_irq_request(DMA_IRQN, (osal_irq_handler)irq_dma_handler, NULL, NULL, NULL);
    osal_irq_enable(DMA_IRQN);
#if defined(CONFIG_DMA_SUPPORT_SMDMA)
    osal_irq_request(M_SDMA_IRQN, (osal_irq_handler)irq_sdma_handler, NULL, NULL, NULL);
    osal_irq_enable(M_SDMA_IRQN);
#endif
}

void dma_port_unregister_irq(void)
{
    osal_irq_disable(DMA_IRQN);
    osal_irq_free(DMA_IRQN, NULL);
#if defined(CONFIG_DMA_SUPPORT_SMDMA)
    osal_irq_disable(M_SDMA_IRQN);
    osal_irq_free(M_SDMA_IRQN, NULL);
#endif
}

/* 记录每个soure的状态, 0表示空闲， 1表示busy */
void dma_port_set_handshaking_channel_status(hal_dma_handshaking_source_t channel, bool on)
{
    unused(channel);
    unused(on);
}

void dma_port_release_handshaking_source(dma_channel_t ch)
{
    unused(ch);
}

errcode_t dma_port_set_mux_channel(dma_channel_t ch, hal_dma_transfer_peri_config_t *per_cfg)
{
    unused(ch);
    unused(per_cfg);
    return ERRCODE_SUCC;
}

#define DMA_MAIN_AHB_BUS_START_ADDR 0x20000000
#define DMA_MAIN_AHB_BUS_END_ADDR 0x28000FFF

uint32_t dma_porting_convert_address(uint32_t addr)
{
    return addr;
}

hal_dma_master_select_t dma_port_get_master_select(dma_channel_t ch, uint32_t addr)
{
    /* The M1 port of the mcu MDMA can access all spaces except the l2m, psram, and flash memory. */
#if (CORE == APPS)
    if (ch < DMA_CHANNEL_8) {
        if (addr < DMA_MAIN_AHB_BUS_START_ADDR || addr > DMA_MAIN_AHB_BUS_END_ADDR) {
            return HAL_DMA_MASTER_SELECT_1;
        }
    }
#else
    unused(ch);
    unused(addr);
#endif

    return HAL_DMA_MASTER_SELECT_0;
}

uint32_t dma_porting_adapt_lli_cfg(dma_channel_t ch, uint32_t item)
{
    if (ch < B_DMA_CHANNEL_MAX_NUM) {
        return item + 1;
    } else {
        return item;
    }
}

void dma_port_add_sleep_veto(void)
{
#ifdef BUILD_APPLICATION_STANDARD
    uapi_pm_add_sleep_veto_with_symmetry(PM_ID_DMA);
#endif
    return;
}

void dma_port_remove_sleep_veto(void)
{
#ifdef BUILD_APPLICATION_STANDARD
    uapi_pm_remove_sleep_veto_with_symmetry(PM_ID_DMA);
#endif
    return;
}

#if defined(CONFIG_DMA_SUPPORT_SMDMA)
dma_channel_t dma_port_get_sdma_start_ch(uint8_t burst_length)
{
    unused(burst_length);
    return B_DMA_CHANNEL_MAX_NUM;
}
#endif
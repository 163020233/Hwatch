/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LiteOS Board.
 *
 * Create: 2021-09-03
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#include <stdbool.h>
#include "kal_base.h"
#include "chip_mem.h"
#include "debug_print.h"
#include "sdhci.h"
#include "mmc.h"
#include "mmc_porting.h"
#include "boot_serial.h"

#define CLOCK_DIV_NONE 0
#define CLOCK_DIV_0 0
#define CLOCK_DIV_1 1
#define CLOCK_DIV_2 2
#define CLOCK_DIV_3 3
#define CLOCK_DIV_4 4
#define CLOCK_DIV_5 5
#define CLOCK_DIV_6 6
#define CLOCK_DIV_7 7
#define CLOCK_DIV_8 8
#define CLOCK_DIV_9 9
#define CLOCK_DIV_10 10
#define CLOCK_DIV_11 11
#define CLOCK_DIV_12 12
#define CLOCK_DIV_13 13
#define CLOCK_DIV_14 14
#define CLOCK_DIV_15 15
#define CLOCK_DIV_16 16
#define CLOCK_DIV_17 17
#define CLOCK_DIV_18 18
#define CLOCK_DIV_19 19
#define CLOCK_DIV_20 20
#define CLOCK_DIV_32 32

/* GLB_CTL_M_RB Base address of Module's Register */
#define GLB_CTL_M_RB_BASE (0x57000000)

#define SDIO_HOST_REG_BASE_ADDESS 0x52060000
#define SDIO_HOST_REG_END_ADDRESS 0x52060FFF
#define SDIO_INTERRUPT_IRQ 76
#define EMMC_REG_BASE_ADDESS 0x52061000
#define EMMC_REG_END_ADDRESS 0x52061FFF
#define EMMC_INTERRUPT_IRQ 78

#define CKG_CTRL_AND_STS 0x124

#define PERI_EMMC_CRG_RST_CTL (GLB_CTL_M_RB_BASE + 0xc8)

#define PLATDEV_GET_DEV(dev) (&(dev)->dev)
#define PLATDRV_GET_DRV(drv) (&(drv)->driver)

#define DEV_GET_LOSDEVICE(dev) (&(dev)->losdevice)
#define DRV_GET_LOSDRIVER(drv) (&(drv)->losdriver)

#define LOS_ERRNO_DRIVER_INPUT_INVALID LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x10)
#define LOS_ERRNO_DRIVER_DEVICE_REGISTERED LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x32)

typedef struct dpal_dl_list {
    struct dpal_dl_list *prev; /**< Current node's pointer to the previous node */
    struct dpal_dl_list *next; /**< Current node's pointer to the next node */
} dpal_dl_list_t;

struct dpal_device_config {
    uintptr_t irq_num;             /**< Interrupt number of device */
    uint32_t num_regs;             /**< Number of device registers */
    struct dpal_device_regs *regs; /**< Device registers */
};

struct dpal_device {
    const char *name;              /**< Device name */
    int32_t id;                    /**< Device id, generally like 0,1... */
    dpal_dl_list_t driver_node;    /**< Node for linking to the attached driver */
    dpal_dl_list_t device_item;    /**< Node for linking to the global device list */
    dpal_dl_list_t pm_node;        /**< Node for linking to the global power manager list */
    uint32_t mutex;                /**< Mutex */
    bool is_registered;            /**< If device registered to the system */
    struct dpal_driver *drv;       /**< Attached driver */
    struct dpal_device_config cfg; /**< Standard device configurations */
    void *data;                    /**< Device private data */
};

struct dpal_driver_ops {
    int32_t (*probe)(struct dpal_device *dev);  /**< Probe function of driver ops */
    int32_t (*remove)(struct dpal_device *dev); /**< Remove function of driver ops */
    void (*shutdown)(struct dpal_device *dev);  /**< Shutdown function of driver ops */
};

struct dpal_power_mgr_ops {
    int32_t (*suspend)(struct dpal_device *dev);  /**< Suspend function of power manager ops */
    int32_t (*resume)(struct dpal_device *dev);   /**< Resume function of power manager ops */
    int32_t (*prepare)(struct dpal_device *dev);  /**< Prepare function of power manager ops */
    int32_t (*complete)(struct dpal_device *dev); /**< Complete function of power manager ops */
};

struct dpal_driver {
    const char *name;                 /**< Driver name */
    dpal_dl_list_t device_list;       /**< List header for putting attached devices */
    dpal_dl_list_t driver_item;       /**< Node for linking to the global driver list */
    uint32_t mutex;                   /**< Mutex */
    bool is_registered;               /**< If driver registered to the system */
    struct dpal_driver_ops ops;       /**< Driver operations */
    struct dpal_power_mgr_ops pm_ops; /**< Driver power manager operations */
};

typedef struct dpal_pm_message {
    int32_t event;
} dpal_dev_pm_message_t;

struct dpal_dev_pm_info {
    dpal_dev_pm_message_t power_state;
    dpal_dl_list_t entry;
};

struct device_dpal {
    struct dpal_device dpal_dev;
    const char *name;
    void *driver_data;
    void *platform_data; /**< Platform specific data, device core doesn't touch it */
    struct dpal_dev_pm_info power;
};

typedef struct dpal_resource {
    uint32_t start;
    uint32_t end;
    const char *name;
    unsigned long flags;
    unsigned long desc;
    struct dpal_resource *parent, *sibling, *child;
} dpal_resource_t;

struct dpal_platform_device {
    const char *name;
    struct device_dpal dev;
    int32_t id;
    uint32_t num_resources;
    dpal_resource_t *resource;
};

static bool g_emmc_inited = false;

uint64_t g_emmc_capcity_size = 0;

void set_emmc_capcity_size(uint64_t emmc_capcity_size)
{
    g_emmc_capcity_size = emmc_capcity_size;
}

uint64_t get_emmc_capcity_size(void)
{
    if (!g_emmc_inited) {
        if (emmc_drv_context_init() != 0) {
            boot_msg0("emmc init fail");
            return 0;
        }
        g_emmc_inited = true;
    }
    return g_emmc_capcity_size;
}

static struct dpal_resource sdmmc0_resources[] = {
    {
        .start = EMMC_REG_BASE_ADDESS,
        .end = EMMC_REG_END_ADDRESS,
        .flags = IORESOURCE_MEM,
    },
    {
        .start = EMMC_INTERRUPT_IRQ,
        .end = EMMC_INTERRUPT_IRQ,
        .flags = IORESOURCE_IRQ,
    },
    {
        .start = PERI_EMMC_CRG_RST_CTL,
        .end = PERI_EMMC_CRG_RST_CTL,
        .flags = IORESOURCE_REG,
    },
};

static struct dpal_resource sdmmc1_resources[] = {
    {
        .start = SDIO_HOST_REG_BASE_ADDESS,
        .end = SDIO_HOST_REG_END_ADDRESS,
        .flags = IORESOURCE_MEM,
    },
    {
        .start = SDIO_INTERRUPT_IRQ,
        .end = SDIO_INTERRUPT_IRQ,
        .flags = IORESOURCE_IRQ,
    },
    {
        .start = PERI_EMMC_CRG_RST_CTL,
        .end = PERI_EMMC_CRG_RST_CTL,
        .flags = IORESOURCE_REG,
    },
};

struct dpal_platform_device device_sdmmc[] = {
    {
        .name = "sdhci0",
        .id = 0,
        .resource = sdmmc0_resources,
        .num_resources = sizeof(sdmmc0_resources) / sizeof(sdmmc0_resources[0]),
    },
    {
        .name = "sdhci1",
        .id = 1,
        .resource = sdmmc1_resources,
        .num_resources = sizeof(sdmmc1_resources) / sizeof(sdmmc1_resources[0]),
    }};

static bool g_mmc_inited = false;

static const clocks_clk_cfg_t g_system_emmc_freq_config[CLOCKS_EMMC_FREQ_MAX] = {
    { CLOCKS_SRC_XO, CLOCK_DIV_1 },      // 32M
    { CLOCKS_SRC_DLL, CLOCK_DIV_1 },     // 64M
    { CLOCKS_SRC_PLL, CLOCK_DIV_2 },     // 100M
};

clocks_clk_cfg_t clocks_system_emmc_freq_get(system_emmc_freq_t clk_level)
{
    return g_system_emmc_freq_config[clk_level];
}

struct platform_device *get_mmc_device(int id)
{
    return (struct platform_device *)&device_sdmmc[id];
}

uint32_t LOS_DeviceRegister(struct LosDevice *dev)
{
    if ((dev == NULL) || (dev->name == NULL)) {
        return LOS_ERRNO_DRIVER_INPUT_INVALID;
    }

    if (dev->isRegistered) {
        return LOS_ERRNO_DRIVER_DEVICE_REGISTERED;
    }
    dev->driver = NULL;
    dev->isRegistered = true;

    return LOS_OK;
}

int platform_device_register(struct platform_device *dev)
{
    uint32_t ret = 0;
    struct device *device = NULL;
    struct LosDevice *losDevice = NULL;

    if ((dev == NULL) || (dev->name == NULL)) {
        return -EINVAL;
    }

    device = PLATDEV_GET_DEV(dev);
    losDevice = DEV_GET_LOSDEVICE(device);

    /* set losDevice name to make LiteOS device work */
    losDevice->name = dev->name;
    device->name = dev->name;

    ret = LOS_DeviceRegister(losDevice);
    if (ret != LOS_OK) {
        PRINT("LOS_DeviceRegister error\n");
        return ret;
    }

    return 0;
}

errcode_t clocks_set_emmc_freq(system_emmc_freq_t clk_level)
{
    if (clk_level >= CLOCKS_EMMC_FREQ_MAX) {
        return ERRCODE_FAIL;
    }
    clocks_clk_cfg_t emmc_freq = clocks_system_emmc_freq_get(clk_level);
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_EMMC, emmc_freq.clk_src, emmc_freq.clk_div);
    return ERRCODE_SUCC;
}

static void switch_emmc_router(uint32_t freq_level)
{
    clocks_set_emmc_freq(freq_level);
}

int emmc_drv_init(void)
{
    nonos_disk_init();
    (void)ArchDCacheEnable(CACHE_32KB);
    emmc_couter_config_register(switch_emmc_router);
    (void)platform_device_register((struct platform_device *)&device_sdmmc[0]);
    int ret = MMC_HostInitById(device_sdmmc[0].id);
    if (ret != 0) {
        return -EIO;
    }
    return ENOERR;
}

int emmc_drv_context_init(void)
{
    emmc_porting_config();
    int ret;
    ret = emmc_drv_init();
    if (ret != ENOERR) {
        PRINT("[%s:%d] ret[%d]\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    g_mmc_inited = true;
    /* 卡时钟门控配置，器件初始化后才能配置 */
    writel(EMMC_BASE_ADDR + CKG_CTRL_AND_STS, 0x2000000);

    return ret;
}
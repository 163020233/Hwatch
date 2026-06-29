/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __HIFMC100_H__
#define __HIFMC100_H__

#include "host_common.h"
#include <stdbool.h>

#define HIFMC100_REG_BASE  0x52062000
#define HIFMC100_MEM_BASE  0x5c000000

#define IORESOURCE_MEM      0x00000200

#define _2K         (2048)


#define HIFMC100_NAND_CS_NUM        0x1

#define HIFMC_TIME_OUT   10000
#define HIFMC_DMA        0
#define HIFMC_REG        1
#define HIFMC_ID_LEN     2


#define STATUS_P_FAIL_MASK                  (1 << 3)
#define STATUS_E_FAIL_MASK                  (1 << 2)
#define STATUS_WEL_MASK                     (1 << 1)
#define STATUS_OIP_MASK                     (1 << 0)

#define MAX_SPI_OP          8

/* SPI general operation parameter */
struct spi_op {
    unsigned char iftype;
    unsigned char cmd;
    unsigned char dummy;
    unsigned int size;
    unsigned int clock;
};

struct spi;
/* SPI interface special operation function hook */
struct spi_drv {
    int (*wait_ready)(struct spi *spi);
    int (*write_enable)(struct spi *spi);
    int (*qe_enable)(struct spi *spi);
    int (*bus_prepare)(struct spi *spi, int op);
    int (*entry_4addr)(struct spi *spi, int en);
};

/* SPI interface all operation */
struct spi {
    char *name;
    unsigned int cs;
    unsigned long long chipsize;
    unsigned long long  erasesize;
    unsigned int addrcycle;

    struct spi_op read[MAX_SPI_OP];
    struct spi_op write[MAX_SPI_OP];
    struct spi_op erase[MAX_SPI_OP];

    void *host;

    struct spi_drv *driver;
};

#ifndef DPAL_STRUCT
#define DPAL_STRUCT
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

struct dpal_dev_pm_op {
    int32_t (*suspend)(struct device_dpal *dev);
    int32_t (*resume)(struct device_dpal *dev);
    int32_t (*prepare)(struct device_dpal *dev);
    int32_t (*complete)(struct device_dpal *dev);
};

struct dpal_device_driver {
    struct dpal_driver dpal_drv;
    const char *name;
    const struct dpal_dev_pm_op *pm;
};

struct dpal_platform_driver {
    int32_t (*probe)(struct dpal_platform_device *);
    int32_t (*remove)(struct dpal_platform_device *);
    void (*shutdown)(struct dpal_platform_device *);
    int32_t (*suspend)(struct dpal_platform_device *);
    int32_t (*resume)(struct dpal_platform_device *);
    struct dpal_device_driver drv;
};
#endif

struct hifmc_host {
    struct nand_host *host;
    struct spi spi[1];
    void (*set_system_clock)(unsigned clk, int clk_en);
};


static inline void dpal_mb(void)
{
    __asm__ __volatile__("fence":::"memory");
}

#define DPAL_WRITE_UINT8(value, addr) ({ dpal_mb(); *((volatile uint8_t *)((uintptr_t)(addr))) = (value); })
#define DPAL_WRITE_UINT16(value, addr) ({ dpal_mb(); *((volatile uint16_t *)((uintptr_t)(addr))) = (value); })
#define DPAL_WRITE_UINT32(value, addr) ({ dpal_mb(); *((volatile uint32_t *)((uintptr_t)(addr))) = (value); })
#define DPAL_GET_UINT8(addr) ({ uint8_t r = *((volatile uint8_t *)((uintptr_t)(addr))); dpal_mb(); r; })
#define DPAL_GET_UINT16(addr) ({ uint16_t r = *((volatile uint16_t *)((uintptr_t)(addr))); dpal_mb(); r; })
#define DPAL_GET_UINT32(addr) ({ uint32_t r = *((volatile uint32_t *)((uintptr_t)(addr))); dpal_mb(); r; })
#define DPAL_READ_UINT32(value, addr) ({ (value) = *((volatile uint32_t *)((uintptr_t)(addr))); dpal_mb(); })

#define dpal_writeb(value, address)  DPAL_WRITE_UINT8(value, address)
#define dpal_writew(value, address)  DPAL_WRITE_UINT16(value, address)
#define dpal_writel(value, address)  DPAL_WRITE_UINT32(value, address)

#define dpal_readb(address)          DPAL_GET_UINT8(address)
#define dpal_readw(address)          DPAL_GET_UINT16(address)
#define dpal_readl(address)          DPAL_GET_UINT32(address)

#define FEATURE_QE_ENABLE                   (1 << 0)

#ifndef CACHE_ALIGNED_SIZE
#define CACHE_ALIGNED_SIZE        32
#endif

int32_t nand_fmc100_init(void);
int spinand_general_qe_enable(struct spi *spi);
int spinand_qe_not_enable(struct spi *spi);
int spinand_buf_enable(struct spi *spi);

#endif /* End of __HIFMC100_H__ */
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef EMMC_TYPES_H
#define EMMC_TYPES_H

#define __NEED_uint32_t
#define __NEED_uint64_t
#define __NEED_uintptr_t
#define __NEED_struct_timeval
#define __NEED_time_t
#define __NEED_suseconds_t
#define __NEED_ssize_t
#define __NEED_int16_t
#define __NEED_off_t
#define __NEED_ino_t
#define __NEED_blkcnt_t
#define __NEED_blksize_t

#include "bits/alltypes.h"
#include <stdbool.h>
#include "kal_mem.h"
#include "clocks_switch.h"

#define realloc irrealloc

typedef volatile int Atomic;
typedef volatile long Atomic64;

typedef Atomic atomic_t;
typedef Atomic64 atomic64_t;

struct dev_pm_op;
struct LosDriver;

/**
 * @ingroup los_list
 * Structure of a node in a doubly linked list.
 */
typedef struct LOS_DL_LIST {
    struct LOS_DL_LIST *pstPrev; /**< Current node's pointer to the previous node */
    struct LOS_DL_LIST *pstNext; /**< Current node's pointer to the next node */
} LOS_DL_LIST;

/**
 * @ingroup los_driverbase
 * LosDevice register config structure.
 *
 */
struct LosDeviceRegs {
    uintptr_t base; /**< Register base address of device */
    uintptr_t size; /**< Register size of device */
};

/**
 * @ingroup los_driverbase
 * LosDevice config structure.
 *
 */
struct LosDeviceConfig {
    uintptr_t irqNum;           /**< Interrupt number of device */
    uint32_t numRegs;           /**< Number of device registers */
    struct LosDeviceRegs *regs; /**< Device registers */
};

/**
 * @ingroup los_driverbase
 * LosDevice information structure.
 *
 */
struct LosDevice {
    const char *name;           /**< Device name */
    int id;                     /**< Device id, generally like 0,1... */
    LOS_DL_LIST driverNode;     /**< Node for linking to the attached driver */
    LOS_DL_LIST deviceItem;     /**< Node for linking to the global device list */
    LOS_DL_LIST pmNode;         /**< Node for linking to the global power manager list */
    uint32_t mutex;             /**< Mutex */
    bool isRegistered;          /**< If device registered to the system */
    struct LosDriver *driver;   /**< Attached driver */
    struct LosDeviceConfig cfg; /**< Standard device configurations */
    void *data;                 /**< Device private data */
};

/**
 * @ingroup los_driverbase
 * LosDevice power manager operation structure.
 *
 */
struct LosPowerMgrOps {
    int (*suspend)(struct LosDevice *device);  /**< Suspend function of power manager ops */
    int (*resume)(struct LosDevice *device);   /**< Resume function of power manager ops */
    int (*prepare)(struct LosDevice *device);  /**< Prepare function of power manager ops */
    int (*complete)(struct LosDevice *device); /**< Complete function of power manager ops */
};

/**
 * @ingroup los_driverbase
 * LosDriver operation structure.
 *
 */
struct LosDriverOps {
    int (*probe)(struct LosDevice *device);     /**< Probe function of driver ops */
    int (*remove)(struct LosDevice *device);    /**< Remove function of driver ops */
    void (*shutdown)(struct LosDevice *device); /**< Shutdown function of driver ops */
};

/**
 * @ingroup los_driverbase
 * LosDriver information structure.
 *
 */
struct LosDriver {
    const char *name;            /**< Driver name */
    LOS_DL_LIST deviceList;      /**< List header for putting attached devices */
    LOS_DL_LIST driverItem;      /**< Node for linking to the global driver list */
    uint32_t mutex;              /**< Mutex */
    bool isRegistered;           /**< If driver registered to the system */
    struct LosDriverOps ops;     /**< Driver operations */
    struct LosPowerMgrOps pmOps; /**< Driver power manager operations */
};

typedef struct pm_message {
    int event;
} dev_pm_message_t;

struct dev_pm_info {
    dev_pm_message_t power_state;
    LOS_DL_LIST entry;
};

/* device define */
/**
 * @ingroup driver
 * Define the structure of the parameters used for device creation.
 */
struct device {
    struct LosDevice losdevice;
    const char *name;
    void *driver_data;
    void *platform_data; /**< Platform specific data, device core doesn't touch it */
    struct dev_pm_info power;
};

/* driver define */
/**
 * @ingroup driver
 * Define the structure of the parameters used for driver creation.
 */
struct device_driver {
    struct LosDriver losdriver;
    const char *name;
    const struct dev_pm_op *pm;
};

struct dev_pm_op {
    int (*suspend)(struct device *ptr);
    int (*resume)(struct device *ptr);
    int (*prepare)(struct device *dev);
    int (*complete)(struct device *dev);
};

typedef uintptr_t resource_size_t;

struct resource {
    resource_size_t start;
    resource_size_t end;
    const char *name;
    unsigned long flags;
    unsigned long desc;
    struct resource *parent, *sibling, *child;
};

/**
 * @ingroup driver
 * Define the structure of the parameters used for platform device creation.
 */
struct platform_device {
    const char *name;          /**< device name. */
    struct device dev;         /**< device. */
    int id;                    /**< device id. */
    uint32_t num_resources;    /**< Number of resources used by devices. */
    struct resource *resource; /**< Resources used by the device. */
};

/**
 * @ingroup driver
 * Define the structure of the parameters used for platform driver creation.
 */
struct platform_driver {
    int (*probe)(struct platform_device *);     /**< Called to query the existence of a specific device. */
    int (*remove)(struct platform_device *);    /**< Remove the device, and unbind this device from driver. */
    void (*shutdown)(struct platform_device *); /**< Shut-down the device. */
    int (*suspend)(struct platform_device *);   /**< Put the device to sleep mode. */
    int (*resume)(struct platform_device *);    /**< Resume the device from sleep mode. */
    struct device_driver driver;                /**< device driver. */
};

/**
 * @ingroup los_event
 * Event control structure
 */
typedef struct tagEvent {
    uint32_t uwEventID;      /**< Event mask in the event control block,
                                  indicating the event that has been logically processed. */
    LOS_DL_LIST stEventList; /**< Event control block linked list */
} EVENT_CB_S, *PEVENT_CB_S;

typedef struct device *device_t;
typedef void (*sdio_irq_cb)(void *arg);

typedef struct {
    void *ptr;
    uint32_t buf_size_max;
    uint32_t buf_size;
    uint32_t unaligned_flag;
} aligned_buf_s;

typedef struct {
    unsigned char *desc; /* Memory of ADMA2 Descriptor Line */
    uint32_t length;     /* Length of ADMA2 memory */
    uint32_t num_slices; /* Number of slice */
    uint32_t slice;      /* Size of slice */
    uint32_t attr;       /* Attribution of the ADMA2 Descriptor memory */
    aligned_buf_s aligned_buf_start;
    aligned_buf_s aligned_buf_end;
} adma2_s;

/*
 * Hci: host controller interface
 * capacities of host controller
 * that all host will have
 */
typedef struct {
    uint32_t caps;
    uint32_t caps2;
    uint32_t min_clk;
    uint32_t max_clk;
    uint32_t quirks;
} hci_caps;

/*
 * hci: host controller interface
 */
struct hci_softc {
    uint32_t num;
    device_t dev;
    void *peri_base;
    struct resource *irq_res;
    void *slot;
    uint32_t mmchs_clk_id;
    uint32_t peri_crg_reg;
    uintptr_t base;
    uint32_t baseclk_hz;
    uint32_t cmd_and_mode;
    uint32_t sdhci_clkdiv;
    bool disable_highspeed;
    bool force_card_present;
    hci_caps caps; /* The low-level interface between platform and the drivers. */
    uint32_t tuning_phase;
    /*
	 * Power control of cd-sw,
	 * when set, the host would not set the power again,
	 * that will result in miss-behave of power on/off
	 */
    uint32_t no_power;
    sdio_irq_cb sdio_cb;
    void *sdio_arg;
    bool sdio_rescan_allow;
};

/*
 * mmc_ios is a structure that is used to store the state of the mmc/sd
 * bus configuration.  This include the bus' clock speed, its voltage,
 * the bus mode for command output, the SPI chip select, some power
 * states and the bus width.
 */
enum mmc_vdd {
    vdd_150 = 0,
    vdd_155,
    vdd_160,
    vdd_165,
    vdd_170,
    vdd_180,
    vdd_190,
    vdd_200,
    vdd_210,
    vdd_220,
    vdd_230,
    vdd_240,
    vdd_250,
    vdd_260,
    vdd_270,
    vdd_280,
    vdd_290,
    vdd_300,
    vdd_310,
    vdd_320,
    vdd_330,
    vdd_340,
    vdd_350,
    vdd_360
};

enum mmc_vccq { vccq_120 = 0, vccq_180, vccq_330 };

enum mmc_power_mode { power_off = 0, power_up, power_on };

enum mmc_bus_mode { opendrain = 1, pushpull };

enum mmc_chip_select { cs_dontcare = 0, cs_high, cs_low };

enum mmc_bus_width { bus_width_1 = 0, bus_width_4 = 2, bus_width_8 = 3 };

enum mmc_drv_type { drv_type_b = 0, drv_type_a, drv_type_c, drv_type_d };

enum mmc_bus_timing {
    bus_timing_normal = 1,
    bus_timing_mmc_hs,
    bus_timing_sd_hs,
    bus_timing_uhs_sdr12,
    bus_timing_uhs_sdr25,
    bus_timing_uhs_sdr50,
    bus_timing_uhs_ddr50,
    bus_timing_uhs_sdr104,
    bus_timing_mmc_ddr52,
    bus_timing_mmc_hs200,
    bus_timing_mmc_hs400,
    bus_timing_mmc_hs400es,
    bus_timing_max = bus_timing_mmc_hs400es
};

struct mmc_ios {
    uint32_t clock;     /* Speed of the clock in Hz to move data */
    enum mmc_vdd vdd;   /* Voltage to apply to the power pins */
    enum mmc_vccq vccq; /* Voltage to use for signaling */
    enum mmc_bus_mode bus_mode;
    enum mmc_chip_select chip_select;
    enum mmc_bus_width bus_width;
    enum mmc_power_mode power_mode;
    enum mmc_bus_timing timing;
    enum mmc_drv_type drv_type;
};

enum mmc_card_mode { mode_mmc, mode_sd, mode_sdio, mode_combo };

struct mmc_host {
    int f_min;
    int f_max;
    uint32_t host_ocr;
    uint32_t ocr;
    uint32_t caps;
    enum mmc_card_mode mode;
    struct mmc_ios ios;     /* Current state of the host */
    uint32_t card_present;  /* The device is detected or not */
    uint32_t plugged_state; /* The device is plugged or unplugged */
};

typedef struct {
    uint32_t (*set_clock)(struct hci_softc *sc, uint32_t clock);
    void (*extra_init)(struct hci_softc *sc);
    void (*slot_init)(struct hci_softc *sc);
    void (*pre_clock)(struct hci_softc *sc);
    void (*post_clock)(struct hci_softc *sc);
    void (*set_phase)(struct hci_softc *sc, uint32_t timing);
    void (*enable_drv_sample)(struct hci_softc *sc, uint32_t en);
    void (*select_sample_phase)(struct hci_softc *sc, uint32_t phase);
    int (*switch_volt)(struct hci_softc *sc, uint32_t vccq);
    void (*soft_reset)(struct hci_softc *sc);
    uint32_t (*get_taps)(struct hci_softc *sc);
    void (*set_drv)(struct hci_softc *sc, uint32_t timing, uint32_t clk);
    void (*dump_regs)(struct hci_softc *sc);
#ifdef LOSCFG_DRIVERS_MMC_EXT_CLK_SWITCH
    void (*drv_ext_clk_switch)(struct hci_softc *sc, bool en);
#endif
} hci_slot_ops;

struct sdhci_slot {
    uint32_t mtx;      /* Slot mutex */
    uint32_t bus_mtx;  /* Bus mutex */
    uint32_t quirks;   /* Chip specific quirks */
    uint32_t caps;     /* Override SDHCI_CAPABILITIES */
    uint32_t caps2;    /* Override SDHCI_CAPABILITIES2 */
    device_t bus;      /* Bus device */
    device_t dev;      /* Slot device */
    unsigned char num; /* Slot number */
    unsigned char opt; /* Slot options */
    unsigned char version;
    int timeout;          /* Transfer timeout */
    uint32_t max_clk;     /* Max possible freq */
    uint32_t min_clk;     /* Min possible freq */
    uint32_t timeout_clk; /* Timeout freq */
    adma2_s adma2;

    EVENT_CB_S req_event; /* Request event*/

    uint16_t timeout_callout; /* Software timer for cmd/data response timeout */

    struct mmc_host host;       /* Host parameters */
    struct mmc_request *req;    /* Current request */
    struct mmc_command *curcmd; /* Current command of current request */

    struct mmc_request *tune_req; /* Tuning request */
    struct mmc_command *tune_cmd; /* Tuning command of tuning request */
    struct mmc_data *tune_data;   /* Tuning data of tuning command */
    uint32_t retune_ticks;        /* Re-tuning callout ticks [hz] */
    uint32_t intmask;             /* Current interrupt mask */
    uint32_t clock;               /* Current clock freq. */
    size_t offset;                /* Data buffer offset */
    uint8_t hostctrl;             /* Current host control register */
    uint8_t retune_count;         /* Controller re-tuning count [s] */
    uint8_t retune_mode;          /* Controller re-tuning mode */
    uint8_t retune_req;           /* Re-tuning request status */
    unsigned char power;          /* Current power */

    unsigned char cmd_done;  /* CMD command part done flag */
    unsigned char data_done; /* DAT command part done flag */
    unsigned char flags;     /* Request execution flags */
    unsigned char dma_flags; /* Request execution flags */
    hci_slot_ops *ops;
    uint32_t error_count;

#ifdef MMCCAM
    /* CAM stuff */
    union ccb *ccb;
    struct cam_devq *devq;
    struct cam_sim *sim;
    struct mtx sim_mtx;
    unsigned char card_present; /* XXX Maybe derive this from elsewhere? */
#endif
};

/*
 * hci: host controller interface
 */
typedef struct {
    int (*read_ivar)(device_t bus, device_t child, int which, uintptr_t *result);
    int (*write_ivar)(device_t bus, device_t child, int which, uintptr_t value);
    void (*update_ios)(device_t brdev, device_t reqdev);
    int (*request)(device_t brdev, device_t reqdev, struct mmc_request *req);
    void (*handle_error)(device_t reqdev, const struct mmc_request *req);
    int (*tune)(device_t brdev, device_t reqdev);
    int (*switch_vccq)(device_t brdev, device_t reqdev);
    uint32_t (*get_card_busy)(device_t brdev, device_t reqdev);
    int (*retune)(device_t brdev, device_t reqdev, bool reset);
    int (*get_ro)(device_t brdev, device_t reqdev);
    int (*acquire_host)(device_t brdev, device_t reqdev);
    int (*release_host)(device_t brdev, device_t reqdev);
    uint8_t (*readb)(device_t dev, uint32_t off);
    uint16_t (*readw)(device_t dev, uint32_t off);
    uint32_t (*readl)(device_t dev, uint32_t off);
    void (*writeb)(device_t dev, uint32_t off, uint8_t val);
    void (*writew)(device_t dev, uint32_t off, uint16_t val);
    void (*writel)(device_t dev, uint32_t off, uint32_t val);
    bool (*get_card_present)(device_t brdev);
    void (*set_card_present)(device_t brdev, uint32_t p);
    bool (*proc_get_card_present)(device_t brdev);
    bool (*proc_get_card_attach)(device_t brdev);
    void (*set_uhs_timing)(device_t brdev);
    void (*enable_sdio_irq)(device_t brdev);
    int (*registor_sdio_irq)(device_t brdev, sdio_irq_cb cb, void *arg);
    uint32_t (*get_host_id)(device_t reqdev);
#ifdef LOSCFG_DRIVERS_MMC_EXT_CLK_SWITCH
    bool (*set_ext_clk_switch)(device_t bus, bool en);
#endif
} hci_ops;

enum mmc_retune_req { retune_req_none = 0, retune_req_normal, retune_req_reset };

enum mmcbr_device_ivars {
    MMCBR_IVAR_BUS_MODE,
    MMCBR_IVAR_BUS_WIDTH,
    MMCBR_IVAR_CHIP_SELECT,
    MMCBR_IVAR_CLOCK,
    MMCBR_IVAR_F_MIN,
    MMCBR_IVAR_F_MAX,
    MMCBR_IVAR_HOST_OCR,
    MMCBR_IVAR_MODE,
    MMCBR_IVAR_OCR,
    MMCBR_IVAR_POWER_MODE,
    MMCBR_IVAR_RETUNE_REQ,
    MMCBR_IVAR_VDD,
    MMCBR_IVAR_VCCQ,
    MMCBR_IVAR_CAPS,
    MMCBR_IVAR_TIMING,
    MMCBR_IVAR_MAX_DATA,
    MMCBR_IVAR_MAX_BUSY_TIMEOUT,
    /*
     * Return the gap of the DMA which cannot be processed.
     * currently we have 0 for SDHCI and 4 for SDMCI,
     * while the system DMA asked for 64 or 32 alignment,
     * refer to the DMA spec for more details.
     */
    MMCBR_IVAR_DMA_ALIGNMENT,
    MMCBR_IVAR_CARD_ERROR_CNT,
    MMCBR_IVAR_ENABLE_SDIO_IRQ,
};

enum mmc_device_ivars {
    MMC_IVAR_SPEC_VERS,
    MMC_IVAR_DSR_IMP,
    MMC_IVAR_MEDIA_SIZE,
    MMC_IVAR_RCA,
    MMC_IVAR_SECTOR_SIZE,
    MMC_IVAR_TRAN_SPEED,
    MMC_IVAR_READ_ONLY,
    MMC_IVAR_HIGH_CAP,
    MMC_IVAR_CARD_TYPE,
    MMC_IVAR_BUS_WIDTH,
    MMC_IVAR_ERASE_SECTOR,
    MMC_IVAR_MAX_DATA,
    MMC_IVAR_CMD6_TIMEOUT,
    MMC_IVAR_QUIRKS,
    MMC_IVAR_CARD_ID_STRING,
    MMC_IVAR_CARD_SN_STRING,
    MMC_IVAR_AU_SIZE,
    MMC_IVAR_CARD_PRESENT,
    MMC_IVAR_AUTO_CMD23,
    MMC_IVAR_CCC_ERASE,
    MMC_IVAR_CCC_SC,    /* Speed_class of mmc_sd_status */
    MMC_IVAR_CCC_GRADE, /* Grade of mmc_sd_status */
    MMC_IVAR_CID,
    MMC_IVAR_CAPACITY,
    MMC_IVAR_CCCR_CAPS,
    MMC_IVAR_CIS_MANID,
    MMC_IVAR_CIS_PROID,
};

typedef enum irqreturn {
    IRQ_NONE = (0U << 0),       /* interrupt was not from this device. */
    IRQ_HANDLED = (1U << 0),    /* interrupt was handled by this device. */
    IRQ_WAKE_THREAD = (1U << 1) /* handler requests to wake the handler thread. */
} irqreturn_t;

typedef irqreturn_t (*irq_handler_t)(int, void *);

typedef struct {
    uint32_t start;
    uint32_t end;
    uint32_t left;
    uint32_t right;
    uint32_t phase;
    uint32_t error;
} tune_result;

typedef struct {
    void *softc; /* Current driver's variables */
    void *ivars; /* Instance variables */
    void *ops;   /* Driver's operators */
    void *child; /* Only used for the slot and the mmc bus layer */
} mmcdev_priv;

struct mmc_sg {
    size_t length;
    void *data;
};

struct mmc_data {
    size_t len; /* size of the data */
    size_t blk_num;
    size_t blk_size;
    void *data; /* data buffer */
    size_t sg_count;
    struct mmc_sg *sg;
    uint32_t flags;
};

struct mmc_command {
    uint32_t opcode;
    uint32_t arg;
    uint32_t resp[4];
    uint32_t flags; /* Expected responses */
    uint32_t retries;
    int32_t error;
    struct mmc_data *data; /* Data segment with cmd */
    uint32_t irq_status;
};

struct mmc_request {
    struct mmc_command *bc; /* Send cmd23 to set block count when multiple block r/w */
    struct mmc_command *cmd;
    struct mmc_command *stop;
    void (*done)(struct mmc_request *); /* Completion function */
    void *done_data;                    /* requestor set data */
    uint32_t flags;
};

struct mmc_softc {
    device_t dev;
    uint32_t sc_mtx;
    device_t owner;
    device_t *child_list;
    int child_count;
    uint16_t last_rca;
    uint16_t retune_paused;
    uint8_t retune_needed;
    uint8_t retune_ongoing;
    uint16_t squelched; /* suppress reporting of (expected) errors */
    int log_count;
    struct timeval log_time;
    EVENT_CB_S host_sync_event; /* Request event*/
    uint32_t card_init_status;
    bool recovery;
};

typedef struct {
    int (*read_ivar)(device_t bus, device_t child, int which, uintptr_t *result);
    int (*write_ivar)(device_t bus, device_t child, int which, uintptr_t value);
    int (*child_location_str)(device_t dev, device_t child, char *buf, size_t buflen);
    void (*retune_pause)(device_t busdev, device_t dev, bool retune);
    void (*retune_unpause)(device_t busdev, device_t dev);
    int (*wait_for_request)(device_t busdev, device_t dev, struct mmc_request *req);
    int (*acquire_bus)(device_t busdev, device_t dev);
    int (*release_bus)(device_t busdev, device_t dev);
} mmcbus_ops;

#define MMC_EXTCSD_SIZE 512

/* CSD -- decoded structure */
struct mmc_cid {
    uint32_t mid;
    char pnm[8];
    uint32_t psn;
    uint16_t oid;
    uint16_t mdt_year;
    uint8_t mdt_month;
    uint8_t prv;
    uint8_t crc;
};

struct mmc_csd {
    uint8_t csd_structure;
    uint8_t spec_vers;
    uint16_t ccc;
    uint16_t tacc;
    uint32_t nsac;
    uint32_t r2w_factor;
    uint32_t tran_speed;
    uint32_t read_bl_len;
    uint32_t write_bl_len;
    uint32_t vdd_r_curr_min;
    uint32_t vdd_r_curr_max;
    uint32_t vdd_w_curr_min;
    uint32_t vdd_w_curr_max;
    uint32_t wp_grp_size;
    uint32_t erase_sector;
    uint64_t capacity;
    uint32_t read_bl_partial : 1, read_blk_misalign : 1, write_bl_partial : 1, write_blk_misalign : 1, dsr_imp : 1,
        erase_blk_en : 1, wp_grp_enable : 1;
};

struct mmc_scr {
    unsigned char sda_vsn;
    unsigned char bus_widths;
    uint8_t sda_spec3;
    uint8_t cmds;
};

struct mmc_sd_status {
    uint8_t bus_width;
    uint8_t secured_mode;
    uint16_t card_type;
    uint16_t prot_area;
    uint8_t speed_class;
    uint8_t perf_move;
    uint8_t au_size;
    uint16_t erase_size;
    uint8_t erase_timeout;
    uint8_t erase_offset;
    uint8_t grade;
};

struct sdio_card_cis {
    uint16_t man_id;
    uint16_t prod_id;
    uint16_t blk_size;
};

/*
 * Per-card data
 */
struct mmc_ivars {
    uint32_t raw_cid[4];                  /* Raw bits of the CID */
    uint32_t raw_csd[4];                  /* Raw bits of the CSD */
    uint32_t raw_scr[2];                  /* Raw bits of the SCR */
    uint8_t raw_ext_csd[MMC_EXTCSD_SIZE]; /* Raw bits of the EXT_CSD */
    uint32_t raw_sd_status[16];           /* Raw bits of the SD_STATUS */
    uint16_t rca;
    unsigned char read_only; /* True when the device is read-only */
    unsigned char high_cap;  /* High Capacity device (block addressed) */
    enum mmc_card_mode mode;
    enum mmc_bus_width bus_width;   /* Bus width to use */
    struct mmc_cid cid;             /* cid decoded */
    struct mmc_csd csd;             /* csd decoded */
    struct mmc_scr scr;             /* scr decoded */
    struct mmc_sd_status sd_status; /* SD_STATUS decoded */
    uint32_t sec_count;             /* Card capacity in 512byte blocks */
    uint32_t timings;               /* Mask of bus timings supported */
    uint32_t vccq_120;              /* Mask of bus timings at VCCQ of 1.2 V */
    uint32_t vccq_180;              /* Mask of bus timings at VCCQ of 1.8 V */
    uint32_t tran_speed;            /* Max speed in normal mode */
    uint32_t hs_tran_speed;         /* Max speed in high speed mode */
    uint32_t erase_sector;          /* Card native erase sector size */
    uint32_t cmd6_time;             /* Generic switch timeout [ms] */
    uint32_t quirks;                /* Quirks as per mmc_quirk->quirks */
    char card_id_string[64];        /* Formatted CID info (serial, MFG, etc) */
    char card_sn_string[16];        /* Formatted serial # for disk->d_ident */
    unsigned char max_current_limit;
    unsigned char sdio_rev;
    unsigned char sdio_cccr_caps;
    struct sdio_card_cis cis;
};

#define MMC_PART_GP_MAX 4
#define MMC_PART_MAX 8

struct mmcsd_softc {
    device_t dev;
    device_t mmcbus;
    struct mmcsd_part *part[MMC_PART_MAX];
    enum mmc_card_mode mode;
    unsigned int max_data;            /* Maximum data size [blocks] */
    uint8_t ext_csd[MMC_EXTCSD_SIZE]; /* Make sure that the start addr is aligned by 4-byte */
    unsigned int erase_sector;        /* Device native erase sector size [blocks] */
    uint8_t high_cap;                 /* High Capacity device (block addressed) */
    uint8_t part_curr;                /* Partition currently switched to */
    uint16_t rca;
    uint32_t flags;
    uint32_t cmd6_time; /* Generic switch timeout [ms] */
    uint32_t part_time; /* Partition switch timeout [ms] */
    int log_count;
    struct timeval log_time;
};

typedef enum _disk_status_ {
    STAT_UNUSED,
    STAT_INUSED,
    STAT_UNREADY,
    STAT_ONGOING /* Not a state machine, represents an intermediate state when init and deinit */
} disk_status_e;

#define DISK_NAME 255

#define MMCSD_PART_NAMELEN 17

typedef struct _los_disk_ {
    uint32_t disk_id : 8;     /* physics disk number */
    uint32_t disk_status : 2; /* status of disk */
    uint32_t part_count : 8;  /* current partition count */
    uint32_t reserved : 14;
    struct inode *dev; /* device */
#ifdef LOSCFG_FS_FAT_CACHE
    void *bcache; /* cache of the disk, shared in all partitions */
#endif
    uint32_t sector_size;  /* disk sector size */
    uint64_t sector_start; /* disk start sector */
    uint64_t sector_count; /* disk sector number */
    uint8_t type;
    char disk_name[DISK_NAME + 1];
    LOS_DL_LIST head; /* link head of all the partitions */
    uint32_t disk_mutex;
} los_disk;

#define BIO_QUEUE_NORMAL 1
#define BIO_QUEUE_ABNORMAL 0

#define PART_NORMAL 1
#define PART_ABNORMAL 0

#define ALIGNED_BUFF_BLK_NUM 128

struct mmcsd_part {
    uint32_t disk_mtx;
    struct mmcsd_softc *sc;
    uint32_t pthread_id;
    int bio_queue_state;
    unsigned long eblock, eend; /* Range remaining after the last erase. */
    unsigned int cnt;
    unsigned int type;
    int thread_running;
    int suspend;
    int ioctl;
    bool ro;
    char name[MMCSD_PART_NAMELEN];
    unsigned int errot_cnt;
    atomic_t atom_reqs; /* Current requests in block */
    int part_status;
    uint8_t *buffer; /* Buffer for non-aligned data transfer */
    uint32_t size;   /* Buffer size */
    uint32_t align;
    uint32_t card_capacity;
    uint32_t bio_queue_id;
};

struct sdio_func;
typedef void(sdio_irq_handler)(struct sdio_func *);

struct sdio_func {
    struct sdio_softc *sc;
    union {
        uint32_t data;
        struct {
            uint32_t present : 1;
            uint32_t reserved : 31;
        } bits;
    } state;

    uint32_t max_blk_size;
    uint16_t cur_blk_size;
    uint32_t func_num;
    uint8_t func_class;
    uint16_t manufacturer_id;
    uint16_t device_id;
    uint32_t en_timeout_ms;
    sdio_irq_handler *sdio_irq_handler;
    void *data;
};

struct mmc_quirk {
    uint32_t mid;
    uint16_t oid;
    const char *pnm;
    uint32_t quirks;
};

#define SDIO_MAX_FUNC 7
#define SDIO_DRV_NODE_NAME 17

struct sdio_softc {
    device_t dev;
    device_t mmcbus;
    uint32_t sdio_func_num;
    struct sdio_func *sdio_funcs[SDIO_MAX_FUNC];
    struct sdio_func *func;
    int sdio_irq_refs;
    uint32_t sdio_thread;
    EVENT_CB_S sdio_event;
    bool sdio_event_enabled;
    uint32_t align;
    uint32_t buf_size;
    uint8_t *buf;
    uint32_t drv_no;
    char drv_node_name[SDIO_DRV_NODE_NAME];
    unsigned int max_data; /* Maximum data size [blocks] */
    uint16_t rca;
};

#define REQUEST_EVENT_ANSWER (1U << 0)

struct bio {
    uint16_t bio_cmd;        /* I/O operation. */
    uint16_t bio_flags;      /* General flags. */
    off_t bio_offset;        /* Offset into file. */
    unsigned char *bio_data; /* Memory, superblocks, indirect etc. */
    int bio_error;           /* Errno for BIO_ERROR. */
    long bio_resid;          /* Remaining I/O in bytes. */
    void (*bio_done)(struct bio *);
    off_t bio_length; /* Like bio_bcount */
#ifdef LOSCFG_MMCSD_SG_SUPPORT
    struct mmc_sg *sgs;
    uint32_t sg_count;
    bool sg_mode;
#endif
    /* XXX: these go away when bio chaining is introduced */
    unsigned long sector_start;
    uint32_t sectorsize;
    uint32_t numsectors;
    EVENT_CB_S wait_event;
    bool in_exc;
};

typedef void (*emmc_card_wake_up)(void);
typedef int32_t (*emmc_partition_ruler)(uint64_t);

#ifndef FAR
#define FAR
#endif

union inode_ops_u {
#ifdef CONFIG_VFS_OPERATION_EXTEND
    FAR const struct file_operations_vfs *i_ops; /* Driver operations for inode */
#else
    FAR const struct file_operations *i_ops; /* Driver operations for inode */
#endif
#ifndef CONFIG_DISABLE_MOUNTPOINT
    FAR const struct block_operations *i_bops; /* Block driver operations */
#ifndef CONFIG_VFS_MTD_DISABLE
    FAR struct mtd_dev_s *i_mtd; /* MTD device driver */
#endif
    FAR const struct mountpt_operations *i_mops; /* Operations on a mountpoint */
#endif
#ifdef CONFIG_FS_NAMED_SEMAPHORES
    FAR struct nsem_inode_s *i_nsem; /* Named semaphore */
#endif
#ifdef CONFIG_PSEUDOFS_SOFTLINKS
    FAR char *i_link; /* Full path to link target */
#endif
};

struct inode {
    FAR struct inode *i_parent; /* Link to parent level inode */
    FAR struct inode *i_peer;   /* Link to same level inode */
    FAR struct inode *i_child;  /* Link to lower level inode */
    int16_t i_crefs;            /* References to inode */
    uint16_t i_flags;           /* Flags for inode */
    union inode_ops_u u;        /* Inode operations */
    ino_t i_ino;                /* Inode serial number */
#ifdef CONFIG_PSEUDOFS_ATTRIBUTES
    mode_t i_mode;           /* Access mode flags */
    uid_t i_owner;           /* Owner */
    gid_t i_group;           /* Group */
    struct timespec i_atime; /* Time of last access */
    struct timespec i_mtime; /* Time of last modification */
    struct timespec i_ctime; /* Time of last status change */
#endif
    FAR void *i_private; /* Per inode driver private data */
#ifdef CONFIG_VFS_MOUNT_STATUS_ENABLE
    unsigned long mountflags; /* Flags for mount */
    int32_t e_status;         /* Status for mount */
#endif
    char i_name[1]; /* Name of inode (variable) */
};

#ifndef NAME_MAX
#define NAME_MAX 255
#endif

#define CONFIG_VFS_64BIT_ACCESS

struct geometry {
    bool geo_available;    /* true: The device is available */
    bool geo_mediachanged; /* true: The media has changed since last query */
    bool geo_writeenabled; /* true: It is okay to write to this device */
#ifdef CONFIG_VFS_64BIT_ACCESS
    unsigned long long geo_nsectors; /* Number of sectors on the device */
#else
    blkcnt_t geo_nsectors;                   /* Number of sectors on the device */
#endif
    blksize_t geo_sectorsize; /* Size of one sector */

    /* NULL-terminated string representing the device model */

    char geo_model[NAME_MAX + 1];
};

#define CODE

struct block_operations {
    CODE int (*open)(FAR struct inode *inode);
    CODE int (*close)(FAR struct inode *inode);
#ifdef CONFIG_VFS_64BIT_ACCESS
    ssize_t (*read)(FAR struct inode *inode, FAR unsigned char *buffer, unsigned long long start_sector,
                    unsigned int nsectors);
    ssize_t (*write)(FAR struct inode *inode, FAR const unsigned char *buffer, unsigned long long start_sector,
                     unsigned int nsectors);
#else
    CODE ssize_t (*read)(FAR struct inode *inode, FAR unsigned char *buffer, blkcnt_t start_sector,
                         unsigned int nsectors);
    CODE ssize_t (*write)(FAR struct inode *inode, FAR const unsigned char *buffer, blkcnt_t start_sector,
                          unsigned int nsectors);
#endif
#ifdef CONFIG_VFS_WRITEV_SUPPORT
    ssize_t (*writev)(FAR struct inode *inode, FAR const struct iovec *iov, /* iov_len indicates the nsectors */
                      unsigned int cnt, unsigned long long start_sector);
#endif
    CODE int (*geometry)(FAR struct inode *inode, FAR struct geometry *geometry);
    CODE int (*ioctl)(FAR struct inode *inode, int cmd, unsigned long arg);
#ifndef CONFIG_DISABLE_PSEUDOFS_OPERATIONS
    CODE int (*unlink)(FAR struct inode *inode);
#endif
};

#ifndef MAX_DIVIDE_PART_PER_DISK
#define MAX_DIVIDE_PART_PER_DISK 16
#endif

#define MAX_PRIMARY_PART_PER_DISK 4

struct partition_info {
    uint8_t type;
    uint64_t sector_start;
    uint64_t sector_count;
};

struct disk_divide_info {
    uint64_t sector_count;
    uint32_t sector_size;
    uint32_t part_count;
    /*
     * The primary partition place should be reversed and set to 0 in case all the partitions are
     * logical partition (maximum 16 currently). So the maximum part number should be 4 + 16.
     */
    struct partition_info part[MAX_DIVIDE_PART_PER_DISK + MAX_PRIMARY_PART_PER_DISK];
};

typedef struct {
    emmc_partition_ruler ruler;
    emmc_card_wake_up card_wakeup;
} mmc_block_ops;

struct inode_search_s {
    FAR const char *path;     /* Path of inode to find */
    FAR struct inode *node;   /* Pointer to the inode found */
    FAR struct inode *peer;   /* Node to the "left" for the found inode */
    FAR struct inode *parent; /* Node "above" the found inode */
    FAR const char *relpath;  /* Relative path into the mountpoint */
    FAR char *buffer;         /* Path expansion buffer */
    bool nofollow;            /* true: Don't follow terminal soft link */
};

typedef void (*config_emmc_router)(uint32_t freq_level);

uint64_t get_emmc_capcity_size(void);
#endif
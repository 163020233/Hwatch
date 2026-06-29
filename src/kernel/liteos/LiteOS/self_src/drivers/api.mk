# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.

ifeq ($(LOSCFG_DRIVERS_BASE), y)
    LITEOS_DBASE_INCLUDE   += -I $(LITEOSTOPDIR)/drivers/base/include
endif

ifeq ($(LOSCFG_DRIVERS_MMC), y)
    LITEOS_MMC_INCLUDE += -I $(LITEOSTOPDIR)/drivers/mmc/include
    LITEOS_MMC_INCLUDE += -I $(LITEOS_OPENSOURCE_PATHNAME)/freebsd/liteos/drivers/mmc/include
endif

ifeq ($(LOSCFG_DRIVERS_RANDOM), y)
    LITEOS_RANDOM_INCLUDE += -I $(LITEOSTOPDIR)/drivers/random/include
endif

ifeq ($(LOSCFG_DRIVERS_UART_LITEOS), y)
    LITEOS_UART_INCLUDE += -I $(LITEOSTOPDIR)/drivers/uart/include
endif

ifeq ($(LOSCFG_DRIVERS_IOMMU), y)
    LITEOS_IOMMU_INCLUDE += -I $(LITEOSTOPDIR)/drivers/iommu/include
endif

ifeq ($(LOSCFG_DRIVERS_USB), y)
    LITEOS_USB_INCLUDE += -I $(LITEOSTOPDIR)/drivers/usb/include
ifeq ($(LOSCFG_USB_DEBUG), y)
    LITEOS_CMACRO   += -DLOSCFG_USB_DEBUG
endif
endif

ifeq ($(LOSCFG_DRIVERS_VIDEO), y)
    LITEOS_VIDEO_INCLUDE += -I $(LITEOS_OPENSOURCE_PATHNAME)/incubator-nuttx/liteos/drivers/video/include
endif

ifeq ($(LOSCFG_DRIVERS_CIPHER), y)
    LITEOS_CIPHER_INCLUDE += -I $(LITEOSTOPDIR)/drivers/cipher/cipher/include
    LITEOS_CIPHER_INCLUDE += -I $(LITEOSTOPDIR)/drivers/cipher/cipher/src/drv/cipher_v1.0/osal/include
    LITEOS_CIPHER_INCLUDE += -I $(LITEOSTOPDIR)/drivers/cipher/cipher/src/drv/cipher_v1.0/drivers/core/include
    LITEOS_CIPHER_INCLUDE += -I $(LITEOSTOPDIR)/drivers/cipher/cipher/src/drv/cipher_v1.0/drivers/crypto/include
    LITEOS_CIPHER_INCLUDE += -I $(LITEOSTOPDIR)/drivers/cipher/osal/include
    LITEOS_CIPHER_INCLUDE += -I $(LITEOSTOPDIR)/drivers/cipher/sample
    LITEOS_CIPHER_INCLUDE += -I $(LITEOSTOPDIR)/drivers/cipher/hi_include

ifeq ($(LOSCFG_PLATFORM_HI3556V200), y)
    LITEOS_CMACRO += -DCHIP_TYPE_hi3516cv500 -D__HuaweiLite__
endif
ifeq ($(LOSCFG_PLATFORM_HI3516EV200), y)
    LITEOS_CMACRO += -DCHIP_TYPE_hi3516ev200 -D__HuaweiLite__
endif
endif

LITEOS_DRIVERS_INCLUDE     := -I $(LITEOSTOPDIR)/drivers/interrupt/include \
                              -I $(LITEOSTOPDIR)/drivers/timer/include

LITEOS_DRIVERS_INCLUDE     += $(LITEOS_CELLWISE_INCLUDE)   $(LITEOS_GPIO_INCLUDE) \
                              $(LITEOS_LCD_INCLUDE)        $(LITEOS_MMC_INCLUDE) \
                              $(LITEOS_RANDOM_INCLUDE)     $(LITEOS_USB_INCLUDE) \
                              $(LITEOS_WTDG_INCLUDE)       $(LITEOS_DBASE_INCLUDE) \
                              $(LITEOS_VIDEO_INCLUDE)      $(LITEOS_CIPHER_INCLUDE) \
                              $(LITEOS_IOMMU_INCLUDE)      $(LITEOS_MTD_INCLUDE)

ifeq ($(LOSCFG_DRIVERS_USE_VENDOR_HISI_CAMERA), y)
LOSCFG_DRIVERS_VENDOR_PATH_STRIP := $(subst $\",,$(LOSCFG_DRIVERS_VENDOR_PATH))
LOSCFG_DRIVERS_VENDOR_PATH = $(LOSCFG_DRIVERS_VENDOR_PATH_STRIP)
-include $(LITEOSTOPDIR)/$(LOSCFG_DRIVERS_VENDOR_PATH)/api.mk
endif

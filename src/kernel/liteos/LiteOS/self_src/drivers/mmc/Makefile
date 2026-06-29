include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_FLAGS := -DLOSCFG_MMCSD_SG_SUPPORT
OS_FREEBSD_SRC := $(LITEOS_OPENSOURCE_PATHNAME)/freebsd
OS_FREEBSD_INCLUDE_PATH := $(LITEOS_OPENSOURCE_PATHNAME)/freebsd
MMC_INCLUDE_PATH := $(LITEOSTOPDIR)/drivers/mmc

MMC_FREEBSD_SRC := $(OS_FREEBSD_SRC)/liteos/drivers/mmc
MMC_FREEBSD_INCLUDE_PATH := $(OS_FREEBSD_INCLUDE_PATH)/liteos/drivers/mmc

ifeq ($(LOSCFG_PLATFORM_SUQIN_APP), y)
    MMC_HOST_DIR := sdhci
    LOCAL_SRCS += $(wildcard host/$(MMC_HOST_DIR)/hi3321v100/*.c)
    LOCAL_SRCS += host/$(MMC_HOST_DIR)/tune/sdhci_tune.c
endif

ifeq ($(LOSCFG_PLATFORM_HI3516DV300), y)
    MMC_HOST_DIR := sdmci
    LOCAL_SRCS += $(wildcard host/$(MMC_HOST_DIR)/hi3556v200_series/*.c)
endif

ifeq ($(LOSCFG_PLATFORM_HI3556V200), y)
    MMC_HOST_DIR := sdmci
    LOCAL_SRCS += $(wildcard host/$(MMC_HOST_DIR)/hi3556v200_series/*.c)
endif

ifeq ($(LOSCFG_PLATFORM_HI3559V200), y)
    MMC_HOST_DIR := sdmci
    LOCAL_SRCS += $(wildcard host/$(MMC_HOST_DIR)/hi3556v200_series/*.c)
endif

ifeq ($(LOSCFG_PLATFORM_HI3556V200_DCG), y)
    MMC_HOST_DIR := sdmci
    LOCAL_SRCS += $(wildcard host/$(MMC_HOST_DIR)/hi3556v200_series/*.c)
endif

ifeq ($(LOSCFG_PLATFORM_HI3556AV100), y)
    MMC_HOST_DIR := sdmci
    LOCAL_SRCS += $(wildcard host/$(MMC_HOST_DIR)/hi3556av100/*.c)
endif

ifeq ($(LOSCFG_PLATFORM_HI3516EV200), y)
    MMC_HOST_DIR := sdhci
    LOCAL_SRCS += host/$(MMC_HOST_DIR)/tune/sdhci_tune_edge.c
    LOCAL_SRCS += $(wildcard host/$(MMC_HOST_DIR)/hi3518ev20x/*.c)
endif

ifeq ($(LOSCFG_PLATFORM_HI3516EV300), y)
    MMC_HOST_DIR := sdhci
    LOCAL_SRCS += host/$(MMC_HOST_DIR)/tune/sdhci_tune_edge.c
    LOCAL_SRCS += $(wildcard host/$(MMC_HOST_DIR)/hi3518ev20x/*.c)
endif

ifeq ($(LOSCFG_PLATFORM_HI3518EV300), y)
    MMC_HOST_DIR := sdhci
    LOCAL_SRCS += host/$(MMC_HOST_DIR)/tune/sdhci_tune_edge.c
    LOCAL_SRCS += $(wildcard host/$(MMC_HOST_DIR)/hi3518ev20x/*.c)
endif

ifeq ($(LOSCFG_PLATFORM_HI3559AV100), y)
    MMC_HOST_DIR := sdhci
    LOCAL_SRCS += host/$(MMC_HOST_DIR)/tune/sdhci_tune.c
    LOCAL_SRCS += $(wildcard host/$(MMC_HOST_DIR)/hi3559av100/*.c)
endif

ifeq ($(LOSCFG_PLATFORM_HI3892WV100ES_APP), y)
    MMC_HOST_DIR := sdhci
    LOCAL_SRCS += host/$(MMC_HOST_DIR)/tune/sdhci_tune_edge.c
    LOCAL_SRCS += $(wildcard host/$(MMC_HOST_DIR)/hi3892v100/*.c)
endif

ifeq ($(LOSCFG_PLATFORM_SW21_APP), y)
    MMC_HOST_DIR := sdhci
    LOCAL_SRCS += host/$(MMC_HOST_DIR)/tune/sdhci_tune.c
    LOCAL_SRCS += $(wildcard host/$(MMC_HOST_DIR)/sw21app/*.c)
endif

ifeq ($(MMC_HOST_DIR), sdmci)
LOCAL_SRCS += $(wildcard host/$(MMC_HOST_DIR)/tune/*.c)

MMC_INCLUDE += -I $(MMC_INCLUDE_PATH)/host/$(MMC_HOST_DIR)/tune \
               -I $(MMC_FREEBSD_INCLUDE_PATH)/host/$(MMC_HOST_DIR)/tune
endif

MMC_INCLUDE += -I $(MMC_INCLUDE_PATH)/host/$(MMC_HOST_DIR) \
               -I $(MMC_FREEBSD_INCLUDE_PATH)/host/$(MMC_HOST_DIR) \
               -I $(LITEOSTOPDIR)/arch_lite/riscv/include/arch

LOCAL_SRCS += $(wildcard host/$(MMC_HOST_DIR)/*.c)
LOCAL_SRCS += $(wildcard $(MMC_FREEBSD_SRC)/host/$(MMC_HOST_DIR)/*.c)

LOCAL_SRCS += $(wildcard core/*.c host/*.c)
LOCAL_SRCS += $(wildcard $(MMC_FREEBSD_SRC)/core/*.c $(MMC_FREEBSD_SRC)/host/*.c)

ifeq ($(LOSCFG_FS_PROC), y)
LOCAL_SRCS += $(wildcard proc/*.c)
endif

MMC_INCLUDE += -I $(MMC_INCLUDE_PATH)/proc \
               -I $(MMC_INCLUDE_PATH)/core \
               -I $(MMC_INCLUDE_PATH)/host \
               -I $(MMC_INCLUDE_PATH)/adapt_liteos \
               -I $(MMC_FREEBSD_INCLUDE_PATH)/core \
               -I $(MMC_FREEBSD_INCLUDE_PATH)/host


LOCAL_FLAGS += $(MMC_INCLUDE) $(LITEOS_GCOV_OPTS)

LOCAL_SRCS_NEED_SORT := y

include $(MODULE)


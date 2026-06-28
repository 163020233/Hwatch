# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.

LIB_SUBDIRS         += self_src/targets

LITEOS_PLATFORM     := $(subst $\",,$(LOSCFG_PLATFORM))
ifneq ($(LOSCFG_PLATFORM_EXTENDED_FEATURE),)
LITEOS_PLATFORM = $(subst $\",,$(LOSCFG_PLATFORM))/$(subst ",,$(LOSCFG_PLATFORM_EXTENDED_FEATURE))
endif

PLATFORM_INCLUDE    := -I $(LITEOSTOPDIR)/targets \
                       -I $(LITEOSTOPDIR)/targets/include \
                       -I $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/include \
                       -I $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/vendor/include

ifeq ($(LOSCFG_LIB_LIBC), y)
PLATFORM_INCLUDE    += -I $(LITEOSTOPDIR)/lib/liteos_libc/time
endif

ifeq ($(LOSCFG_SAFETY_KERNEL)$(LOSCFG_BOX), yy)
    BOX_CFG_SRC := $(LITEOS_PLATFORM)/box/boxes_cfg
    PLATFORM_INCLUDE += -I $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/box/boxes_cfg
    PLATFORM_INCLUDE += -I $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/box/usrdrvlib
ifeq ($(LOSCFG_TEST), y)
    include $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/box/boxes_cfg/test/boxes.mk
else
    include $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/box/boxes_cfg/boxes.mk
endif
endif

LITEOS_PLATFORM_INCLUDE += $(PLATFORM_INCLUDE)
LITEOS_CXXINCLUDE       += $(PLATFORM_INCLUDE)

LITEOS_BASELIB          += -ltargets

ifeq ($(LOSCFG_KERNEL_MIN), y)
LITEOS_BASELIB          += -linit
endif
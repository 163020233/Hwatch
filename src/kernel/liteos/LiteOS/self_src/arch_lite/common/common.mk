#===============================================================================
# @brief    compiler file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
#===============================================================================

LITEOS_BASELIB          += -larchcommon

LIB_SUBDIRS             += self_src/arch_lite/common/

ifeq ($(LOSCFG_ARCH_MMU_ENABLE), y)
ARCH_INCLUDE            := -I $(LITEOSTOPDIR)/arch_lite/common/mmu/include
endif

LITEOS_PLATFORM_INCLUDE += $(ARCH_INCLUDE)
LITEOS_CXXINCLUDE       += $(ARCH_INCLUDE)

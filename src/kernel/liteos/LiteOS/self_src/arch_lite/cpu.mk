#===============================================================================
# @brief    compiler file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
#===============================================================================

-include $(LITEOSTOPDIR)/arch_lite/common/common.mk
ifeq ($(LOSCFG_ARCH_ARM_AARCH32), y)
-include $(LITEOSTOPDIR)/arch_lite/arm/cpu.mk
endif

ifeq ($(LOSCFG_ARCH_ARM_AARCH64), y)
-include $(LITEOSTOPDIR)/arch_lite/arm64/cpu.mk
endif

ifeq ($(LOSCFG_ARCH_XTENSA), y)
-include $(LITEOSTOPDIR)/arch_lite/xtensa/cpu.mk
endif

ifeq ($(LOSCFG_ARCH_RISCV32), y)
-include $(LITEOSTOPDIR)/arch_lite/riscv/cpu.mk
endif

ifeq ($(LOSCFG_ARCH_LINGLONG), y)
-include $(LITEOSTOPDIR)/arch_lite/linglong/cpu.mk
endif


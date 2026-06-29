#===============================================================================
# @brief    compiler file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
#===============================================================================

ifeq ($(LOSCFG_ARCH_ARM_CORTEX_M), y)
-include $(LITEOSTOPDIR)/arch_lite/arm/cortex_m/cpu.mk
else
-include $(LITEOSTOPDIR)/arch_lite/arm/cortex_a_r/cpu.mk
endif

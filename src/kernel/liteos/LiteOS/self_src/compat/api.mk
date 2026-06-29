#===============================================================================
# @brief    compiler file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
#===============================================================================

ifeq ($(LOSCFG_COMPAT_CMSIS), y)
    LITEOS_CMSIS_INCLUDE += \
                         -I $(LITEOSTOPDIR)/compat/cmsis \
                         -I $(LITEOS_CMSIS_PATH)/CMSIS/RTOS2/Include \
                         -I $(LITEOS_CMSIS_PATH)/CMSIS/Core/Include
endif

ifeq ($(LOSCFG_COMPAT_LINUX), y)
    LITEOS_LINUX_INCLUDE   += -I $(LITEOSTOPDIR)/compat/linux/include
    LITEOS_LINUX_INCLUDE   += -I $(LITEOS_OPENSOURCE_PATHNAME)/freebsd/liteos/compat/linux/include
endif

LITEOS_NUTTX_INCLUDE := -I $(LITEOSTOPDIR)/compat/nuttx/include

ifeq ($(LOSCFG_COMPAT_RT_THREAD), y)
LITEOS_RT_INCLUDE := -I $(LITEOSTOPDIR)/compat/rt-thread/include
endif

LITEOS_COMPAT_INCLUDE      := $(LITEOS_CMSIS_INCLUDE) \
                              $(LITEOS_LINUX_INCLUDE) \
                              $(LITEOS_NUTTX_INCLUDE) \
                              $(LITEOS_RT_INCLUDE)

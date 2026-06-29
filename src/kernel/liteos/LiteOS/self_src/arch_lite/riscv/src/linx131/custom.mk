#===============================================================================
# @brief    compiler file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
#===============================================================================

LOCAL_SRCS_y += src/$(LITEOS_ARCH_RISCV)/runstop.S

LOCAL_SRCS_y := $(filter-out src/trap.S src/runstop.S, $(LOCAL_SRCS_y))
LOCAL_SRCS_y += src/$(LITEOS_ARCH_RISCV)/trap.S
#===============================================================================
# @brief    compiler file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
#===============================================================================

# CPU compile options
LOSCFG_ARCH_CPU         := linglong

LIB_SUBDIRS             += self_src/arch_lite/$(LOSCFG_ARCH_CPU)/

LITEOS_CPU_OPTS         := --target=$(LOSCFG_ARCH_CPU)

# FPU compile options
ifeq ($(LOSCFG_ARCH_FPU_HARD), y)
	LITEOS_FLOAT_OPTS   := -mfloat-abi=hard
endif

ARCH_C_OPT              += -mllvm -no-stack-coloring=false      # optimize stack size
ARCH_C_OPT              += -fno-short-wchar                     # wchar_t compatible for musl
ARCH_CXX_OPT            += $(LITEOS_CPU_OPTS) $(ARCH_C_OPT)

LITEOS_CORE_COPTS        = $(LITEOS_CPU_OPTS)
LITEOS_NODEBUG          += $(LITEOS_CORE_COPTS)
LITEOS_ASOPTS           += $(LITEOS_CPU_OPTS) $(ARCH_ASM_OPT)
LITEOS_COPTS_BASE       += $(ARCH_C_OPT) $(LITEOS_FLOAT_OPTS)
LITEOS_CXXOPTS_BASE     += $(ARCH_CXX_OPT) $(LITEOS_FLOAT_OPTS)

# strip quotation mark in configuration
ARCH_INCLUDE            := -I $(LITEOSTOPDIR)/arch_lite/$(LOSCFG_ARCH_CPU)/include \

LITEOS_PLATFORM_INCLUDE += $(ARCH_INCLUDE)
LITEOS_CXXINCLUDE       += $(ARCH_INCLUDE)

# extra definition for other module
LITEOS_CPU_TYPE          = $(LOSCFG_ARCH_CPU)

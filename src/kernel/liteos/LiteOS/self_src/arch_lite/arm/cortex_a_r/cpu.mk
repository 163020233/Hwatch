#===============================================================================
# @brief    compiler file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
#===============================================================================

# strip quotation mark in configuration

LOSCFG_ARCH_CPU_STRIP   := $(subst $\",,$(LOSCFG_ARCH_CPU))
LOSCFG_ARCH_CPU          = $(LOSCFG_ARCH_CPU_STRIP)

LOSCFG_ARCH_FPU_STRIP   := $(subst $\",,$(LOSCFG_ARCH_FPU))
LOSCFG_ARCH_FPU          = $(LOSCFG_ARCH_FPU_STRIP)

LITEOS_BASELIB          += -l$(LOSCFG_ARCH_CPU)
LIB_SUBDIRS             += self_src/arch_lite/arm/cortex_a_r

# FPU compile options
ifeq ($(LOSCFG_ARCH_FPU_ENABLE), y)
    ifeq ($(LOSCFG_ARCH_FPU_SOFTFP), y)
        LITEOS_FLOAT_OPTS       := -mfloat-abi=softfp
    else
        LITEOS_FLOAT_OPTS       := -mfloat-abi=hard
    endif
    LITEOS_FPU_OPTS         := -mfpu=$(LOSCFG_ARCH_FPU)
else
LITEOS_FLOAT_OPTS       := -mfloat-abi=soft
endif

# CPU compile options
LITEOS_CPU_OPTS         := -mcpu=$(LOSCFG_ARCH_CPU)
ifeq ($(LOSCFG_ARCH_ARM_CORTEX_R)$(LOSCFG_COMPILER_CLANG), yy)
    LITEOS_CPU_OPTS += target=arm-none-eabi
endif

# gcc libc folder style is combine with core and fpu
ifeq ($(LOSCFG_ARCH_ARM_V5), y)
LITEOS_GCCLIB           := arm/v5te/softfp
else
# for example, cortex-a7 with softfp abi and neon vfp4 is: a7_softfp_neon-vfp4
LITEOS_GCCLIB           := $(subst cortex-,,$(LOSCFG_ARCH_CPU))_softfp_$(LOSCFG_ARCH_FPU)
endif
ifeq ($(wildcard $(GCC_GCCLIB_PATH)/$(LITEOS_GCCLIB)),)
LITEOS_GCCLIB           :=
endif

ifeq ($(MAKE_COMPILER_LIBS),)
LITEOS_CORE_COPTS        = $(LITEOS_CPU_OPTS) $(LITEOS_FLOAT_OPTS) $(LITEOS_FPU_OPTS)
else
LITEOS_CORE_COPTS        =
endif
LITEOS_NODEBUG          += $(LITEOS_CORE_COPTS)
LITEOS_ASOPTS           += $(LITEOS_CPU_OPTS)
LITEOS_CXXOPTS_BASE     += $(LITEOS_CORE_COPTS)

LITEOS_COPTS_BASE       += -mno-unaligned-access
ifneq ($(LOSCFG_COMPILER_CLANG), y)
    LITEOS_COPTS_BASE += -mthumb-interwork
endif

ARCH_INCLUDE            := -I $(LITEOSTOPDIR)/arch_lite/arm/cortex_a_r/include \
                           -I $(LITEOSTOPDIR)/arch_lite/arm/cortex_a_r/include/arch \

LITEOS_PLATFORM_INCLUDE += $(ARCH_INCLUDE)
LITEOS_CXXINCLUDE       += $(ARCH_INCLUDE)

# extra definition for other module
LITEOS_CPU_TYPE          = $(LOSCFG_ARCH_CPU)
LITEOS_ARM_ARCH         := -march=$(subst $\",,$(LOSCFG_ARCH_ARM_VER))

# linux style macros
LINUX_ARCH_$(LOSCFG_ARCH_ARM_V5)  = -D__LINUX_ARM_ARCH__=5
LINUX_ARCH_$(LOSCFG_ARCH_ARM_V7A) = -D__LINUX_ARM_ARCH__=7
LINUX_ARCH_$(LOSCFG_ARCH_ARM_V7R) = -D__LINUX_ARM_ARCH__=7
LINUX_ARCH_$(LOSCFG_ARCH_ARM_V8A) = -D__LINUX_ARM_ARCH__=8
LINUX_ARCH_$(LOSCFG_ARCH_ARM_V8R) = -D__LINUX_ARM_ARCH__=8

AS_OBJS_LIBC_FLAGS      += $(LINUX_ARCH_y)

ifeq ($(LOSCFG_ARCH_ARM_WITH_THUMB_INSTRUCTION), y)
    LITEOS_COPTS_BASE += -mthumb
endif

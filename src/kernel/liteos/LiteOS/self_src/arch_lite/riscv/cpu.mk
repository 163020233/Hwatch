#===============================================================================
# @brief    compiler file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
#===============================================================================

# CPU compile options
LOSCFG_ARCH_CPU    := riscv

ifeq ($(LOSCFG_COMPILER_CLANG), y)
LITEOS_CPU_OPTS := --target=riscv32
endif

ifeq ($(LOSCFG_COMPILER_RISCV_CLANG_UNKNOWN), y)
ARCH_C_OPT += -mllvm -disable-loop-ivopt=true
endif

ifeq  ($(LOSCFG_COMPILER_GCC), y)
LITEOS_CPU_OPTS := -msmall-data-limit=0
ARCH_C_OPT   += -falign-functions=2 -fno-optimize-strlen -freorder-blocks-algorithm=simple -fno-schedule-insns
ARCH_CXX_OPT += -falign-functions=2 -fno-optimize-strlen -freorder-blocks-algorithm=simple -fno-schedule-insns
endif

RISCV_MARCH_y :=
RISCV_ABI_y :=
LCMP_MARCH_EXTENSIONS_y :=
CONNECTOR :=_
EMPTY :=
SPACE :=$(EMPTY) $(EMPTY)

# march string setting
RISCV_MARCH_$(LOSCFG_ARCH_RISCV_RV32IMC) := rv32im
RISCV_MARCH_$(LOSCFG_ARCH_FPU_ENABLE) := $(RISCV_MARCH_y)f
RISCV_MARCH_$(LOSCFG_ARCH_FPU_DOUBLE) := $(RISCV_MARCH_y)d
RISCV_MARCH_$(LOSCFG_ARCH_RISCV_RV32IMC) := $(RISCV_MARCH_y)c

# clang version greater 15.0.0 has two sets of "B" Standard Extension 2 for Bit Manipulation.
TWO_SET_B_EXTENCSION_INSTRUCTION := $(shell echo $(VERSION_NUM) | awk -F. '{if ($$1 >= 15 ) print 1; else print 0}')
ifeq ($(TWO_SET_B_EXTENCSION_INSTRUCTION), 1)
    ifeq ($(LOSCFG_ARCH_RISCV_ISA_B0p92), y)
        RISCV_MARCH_y := $(RISCV_MARCH_y)b0p92
    else ifeq ($(LOSCFG_ARCH_RISCV_ISA_B1p0), y)
        RISCV_MARCH_y := $(RISCV_MARCH_y)b
    endif
else
    # clang version less than 15.0.0 use 'b' options which indicate 0.92 instruction set.
    RISCV_MARCH_$(LOSCFG_ARCH_RISCV_ISA_B0p92) := $(RISCV_MARCH_y)b
endif

RISCV_MARCH_$(LOSCFG_ARCH_VECTOR_ENABLE) := $(RISCV_MARCH_y)v

LCMP_MARCH_EXTENSIONS_$(LOSCFG_LCMP_CUSTOM_INST16_EXTENSIONS) += xlinxma
LCMP_MARCH_EXTENSIONS_$(LOSCFG_LCMP_CUSTOM_INST16_SUB_EXTENSIONS) += xlinxmb
LCMP_MARCH_EXTENSIONS_$(LOSCFG_LCMP_CUSTOM_INST32_EXTENSIONS) += xlinxmc
LCMP_MARCH_EXTENSIONS_$(LOSCFG_LCMP_CUSTOM_INST48_EXTENSIONS) += xlinxmd
LCMP_MARCH_EXTENSIONS_$(LOSCFG_ARCH_RISCV_TES) += xlinxme
LCMP_MARCH_EXTENSIONS_y := $(foreach n,$(LCMP_MARCH_EXTENSIONS_y),$(n))
LCMP_MARCH_EXTENSIONS_y := $(subst  $(SPACE),$(CONNECTOR),$(LCMP_MARCH_EXTENSIONS_y))

ifeq ($(LOSCFG_COMPILER_RISCV_CLANG_MUSL)$(LOSCFG_ARCH_LINXCORE_131), yy)
    LITEOS_CPU_OPTS += -march=$(RISCV_MARCH_y)_xhimideer
else
    LITEOS_CPU_OPTS += -march=$(RISCV_MARCH_y)$(LCMP_MARCH_EXTENSIONS_y)
endif

# ISA string setting
RISCV_ABI_$(LOSCFG_ARCH_RISCV_RV32IMC) := ilp32
RISCV_ABI_$(LOSCFG_ARCH_FPU_ENABLE) := $(RISCV_ABI_y)f
RISCV_ABI_$(LOSCFG_ARCH_FPU_DOUBLE) := $(subst f,d,$(RISCV_ABI_y))
ARCH_C_OPT += -mabi=$(RISCV_ABI_y)
ARCH_CXX_OPT += -mabi=$(RISCV_ABI_y)

ifeq ($(LOSCFG_RISCV_COMPILER_OPTIONS_EMIT_LLI), y)
ifeq ($(LOSCFG_COMPILER_RISCV_CLANG_MUSL), y)
    LITEOS_CPU_OPTS += -mllvm --enable-lli=true
else
    LITEOS_CPU_OPTS += -femit-lli
endif
endif

ARCH_ASM_OPT += -x assembler-with-cpp

ifeq ($(LOSCFG_RISCV_COMPILER_OPTIONS_LDM_STM), y)
ifeq ($(LOSCFG_COMPILER_RISCV_CLANG_MUSL), y)
ifeq ($(LOSCFG_ARCH_LINXCORE_131), y)
    ARCH_C_OPT += -mllvm -fldm-stm-optimize
    ARCH_CXX_OPT += -mllvm -fldm-stm-optimize
endif
else
    ARCH_C_OPT += -fldm-stm-optimize
    ARCH_CXX_OPT += -fldm-stm-optimize
endif
    ARCH_ASM_OPT += -DLOS_COMPILE_LDM
endif

LITEOS_BASELIB          += -l$(LOSCFG_ARCH_CPU)

LITEOS_CORE_COPTS        = $(LITEOS_CPU_OPTS)
LITEOS_NODEBUG          += $(LITEOS_CORE_COPTS)
LITEOS_ASOPTS           += $(LITEOS_CPU_OPTS) $(ARCH_ASM_OPT)
LITEOS_COPTS_BASE       += $(ARCH_C_OPT)
LITEOS_CXXOPTS_BASE     += $(ARCH_CXX_OPT)

ifeq ($(LOSCFG_COMPILER_RISCV_CLANG_MUSL)$(LOSCFG_ARCH_LINXCORE_131), yy)
ifeq ($(LOSCFG_RISCV_COMPILER_OPTIONS_LBU_SB), y)
    LITEOS_COPTS_BASE += -Wa,-enable-c-lbu-sb
endif
ifeq ($(LOSCFG_RISCV_COMPILER_OPTIONS_LHU_SH), y)
    LITEOS_COPTS_BASE += -Wa,-enable-c-lhu-sh
endif
endif

ifeq ($(LOSCFG_COMPILER_RISCV_GCC_UNKNOWN), y)
LOSCFG_RISCV_COMPILER_OPTIONS_USER_DEFINED := $(subst $\",,$(LOSCFG_RISCV_COMPILER_OPTIONS_USER_DEFINED))
LITEOS_COPTS_BASE += $(LOSCFG_RISCV_COMPILER_OPTIONS_USER_DEFINED)
LITEOS_COPTS_BASE += -mtune=size -fno-short-enums

ifeq ($(LOSCFG_RISCV_COMPILER_OPTIONS_PUSH_POP), y)
    LITEOS_COPTS_BASE += -mpush-pop  # failed to work when backtrace enabled
endif
ifeq ($(LOSCFG_RISCV_COMPILER_OPTIONS_SAVE_RESTORE), y)
    LITEOS_COPTS_BASE += -msave-restore  # failed to work when backtrace enabled
endif
ifeq ($(LOSCFG_RISCV_COMPILER_OPTIONS_NO_INLINE), y)
    # these options are used to easily patch for ROM codes
    LITEOS_COPTS_BASE += -fno-inline-small-functions -fno-inline-functions-called-once
endif
ifeq ($(LOSCFG_RISCV_COMPILER_OPTIONS_LBU_SB), y)
    LITEOS_COPTS_BASE += -Wa,-enable-c-lbu-sb
endif
ifeq ($(LOSCFG_RISCV_COMPILER_OPTIONS_LDM_STM), y)
LITEOS_CMACRO += -DLOS_COMPILE_LDM
LITEOS_ASOPTS += -DLOS_COMPILE_LDM
endif
endif

# strip quotation mark in configuration
LIB_SUBDIRS             += self_src/arch_lite/riscv

ARCH_INCLUDE            := -I $(LITEOSTOPDIR)/arch_lite/riscv/include \

LITEOS_PLATFORM_INCLUDE += $(ARCH_INCLUDE)
LITEOS_CXXINCLUDE       += $(ARCH_INCLUDE)

# extra definition for other module
LITEOS_CPU_TYPE          = $(LOSCFG_ARCH_CPU)

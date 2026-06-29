# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.

## platform relative ##
LITEOS_ARM_ARCH :=
LITEOS_CPU_TYPE :=
## c as cxx ld options ##
LITEOS_ASOPTS :=
LITEOS_COPTS_BASE :=
LITEOS_COPTS_EXTRA :=
LITEOS_COPTS_DEBUG :=
LITEOS_COPTS_NODEBUG :=
LITEOS_CXXOPTS :=
LITEOS_CXXOPTS_BASE :=
LITEOS_LD_OPTS :=
LITEOS_GCOV_OPTS :=
## dynload ld options ##
LITEOS_DYNLOADOPTS :=
## macro define ##
LITEOS_CMACRO :=
LITEOS_CMACRO_TEST :=
LITEOS_CXXMACRO :=
## head file path and ld path ##
LITEOS_PLATFORM_INCLUDE :=
LITEOS_CXXINCLUDE :=
LITEOS_LD_PATH :=
LITEOS_LD_SCRIPT :=
LITEOS_MK_PATH :=
## c as cxx ld flags ##
LITEOS_ASFLAGS :=
LITEOS_U_INCLUDES :=
LITEOS_K_INCLUDES :=
LITEOS_CFLAGS :=
LITEOS_LDFLAGS :=
LITEOS_CXXFLAGS :=
LIBC_FILTER_OPTS :=
## depended lib ##
LITEOS_BASELIB :=
LITEOS_LIBDEP :=
## directory ##
LIB_BIGODIR :=
LIB_SUBDIRS :=
##compiler##
LITEOS_COMPILER_CXXLIB_PATH :=
LITEOS_COMPILER_GCCLIB_PATH :=
LITEOS_COMPILER_GCC_INCLUDE :=

ifeq ($(filter y,$(LOSCFG_PLATFORM_MP18_GNSS) $(LOSCFG_PLATFORM_MP18_BGLE)) $(LOSCFG_PLATFORM_TG8_BGLE)),y)
HUAWEI_SECURE_C_DIR := $(RTOSLITETOPDIR)/platform/huawei_secure_c
else
HUAWEI_SECURE_C_DIR := $(RTOSLITETOPDIR)/../../../middleware/utils/hwsec_c
endif
export HUAWEI_SECURE_C_DIR

## opensource path ##
LITEOS_OPENSOURCE_PATHNAME := $(OPENSOURCEDIR)
LITEOS_ZLIB_PATH := $(LITEOS_OPENSOURCE_PATHNAME)/zlib
LITEOS_LIBC_PATH := $(LITEOS_OPENSOURCE_PATHNAME)/musl
LITEOS_CMSIS_PATH := $(LITEOS_OPENSOURCE_PATHNAME)/CMSIS_6

-include $(RTOSLITETOPDIR)/.config

## LiteOS self-developed code compiler options ##
## please do not arbitrarily add -Wno-xxx suppression here ##
LITEOS_NEEDED_COMPILE_OPTS := -Wextra -Wno-unused-parameter -Wno-missing-field-initializers
LITEOS_SUGGESTED_COMPILE_OPTS := -Wunused -Wswitch-default -fvisibility=hidden
ifneq ($(LOSCFG_COMPILER_XTENSA_32), y)
    LITEOS_SUGGESTED_COMPILE_OPTS += -Wvla -Wdate-time -Wshadow
endif

LITEOS_NON_SECURE_LOCAL_OPTS := $(LITEOS_NEEDED_COMPILE_OPTS) $(LITEOS_SUGGESTED_COMPILE_OPTS)

ifeq ($(LOSCFG_SAFETY_KERNEL), y)
KERNEL_PATH = kernel_s
ARCH_PATH   = arch_s
TEST_PATH   = test_s
export KCONFIG_KERNEL_PATH=kernel_s
export KCONFIG_ARCH_PATH=arch_s
export KCONFIG_TEST_PATH=test_s
export KCONFIG_OUTPUT_MENUCONFIG_PATH=$(LITEOS_PLATFORM_MENUCONFIG_H)
MULTI_BINARIES_SCRIPT_PATH = $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)
else
KERNEL_PATH = kernel_lite
ARCH_PATH   = arch_lite
TEST_PATH   = test
export KCONFIG_KERNEL_PATH=kernel_lite
export KCONFIG_ARCH_PATH=arch_lite
export KCONFIG_TEST_PATH=test
export KCONFIG_OUTPUT_MENUCONFIG_PATH=$(LITEOS_PLATFORM_MENUCONFIG_H)
MULTI_BINARIES_SCRIPT_PATH = $(SCRIPTS_PATH)/multi_binaries
endif
# Introduced compiler binaries which might different from gcc compilers.
# Those compilers need to implement like $(CC) $(GPP) like gcc does.
# Meanwhile Introduced compiler includes and lib paths
# Export : $(LITEOS_COMPILER_GCC_INCLUDE)
#          $(LITEOS_CXXINCLUDE)
#          $(LITEOS_COMPILER_GCCLIB_PATH)
#          $(LITEOS_COMPILER_CXXLIB_PATH)
ifeq ($(LOSCFG_COMPILER_GCC), y)
    # Supported GCC Compilers
    # Also support "make CROSS_COMPILE=" to use third party compilers.
    include $(RTOSLITETOPDIR)/build/make/compiler_gcc.mk
else ifeq ($(LOSCFG_COMPILER_XTENSA_32), y)
    include $(RTOSLITETOPDIR)/build/make/compiler_xtensa.mk
else ifeq ($(LOSCFG_COMPILER_CLANG), y)
    include $(RTOSLITETOPDIR)/build/make/compiler_clang.mk
endif

CUR_OS  := liteos

HIDE    := @
MKDIR    = mkdir
RM       = -rm -rf
ECHO     = echo
ARFLAGS := crD
PYTHON  := python -B
PYTHON3  := python3 -B

OS       ?= $(shell uname -s)
OBJ_MKDIR = if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi

# Generate file that contains link targets.
# Also support old style sub module make directly using "LITEOS_BASELIB +="
# which is not recommented.
include $(RTOSLITETOPDIR)/build/make/gen_link_targets.mk

## variable define ##
ifeq ($(LOSCFG_TRUSTZONE), y)
    ifeq ($(LOSCFG_SECURE_REGION), y)
        OUT = $(RTOSLITETOPDIR)/out/$(LITEOS_PLATFORM)/secure
    else
        OUT = $(RTOSLITETOPDIR)/out/$(LITEOS_PLATFORM)/nonSecure
    endif
else
    OUT  = $(RTOSLITETOPDIR)/out/$(LITEOS_PLATFORM)
endif

BUILD  = $(OUT)/obj
MK_PATH  = $(RTOSLITETOPDIR)/build/make
LITEOS_SCRIPTPATH  ?= $(RTOSLITETOPDIR)/build/scripts
LITEOS_LIB_BIGODIR  = $(OUT)/lib/obj
LOSCFG_ENTRY_SRC    = $(LITEOSTOPDIR)/$(KERNEL_PATH)/init/los_init.c

LITEOS_MENUCONFIG_H = $(LITEOSTOPDIR)/targets/menuconfig.h
LITEOS_PLATFORM_MENUCONFIG_H = $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/include/menuconfig.h
BOARD_LD_FILE = $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/board.ld
BOARD_LD_S_FILE = $(BOARD_LD_FILE).S

### include variable
MODULE = $(MK_PATH)/kmodule.mk
MODULE_LIBC = $(MK_PATH)/module_libc.mk
UMODULE = $(MK_PATH)/umodule.mk

ifeq ($(LOSCFG_COMPILER_HIMIX_32), y)
LITEOS_CMACRO      += -D__COMPILER_HUAWEILITEOS__
else ifeq ($(LOSCFG_COMPILER_HIMIX210_64), y)
LITEOS_CMACRO      += -D__COMPILER_HUAWEILITEOS__
else ifeq ($(LOSCFG_COMPILER_HCC_64), y)
LITEOS_CMACRO      += -D__COMPILER_HUAWEILITEOS__
endif

LITEOS_CMACRO      += -D__LITEOS__ -DSECUREC_IN_KERNEL=0 -D_ALL_SOURCE
ifneq ($(LOSCFG_DEBUG_VERSION), y)
LITEOS_CMACRO      += -DNDEBUG
endif

# workround : the linker may use the math functions in libstdc++. put the libm in the front
#             to make sure the math functions in the libm will be used.
ifeq ($(LOSCFG_LIB_LIBM), y)
LITEOS_BASELIB     += -lm
endif

ifeq ($(LOSCFG_COMPILER_CLANG), y)
    ifeq ($(LOSCFG_COMPILER_LINGLONG), y)
        LITEOS_BASELIB     += -lclang_rt.builtins
    else ifeq ($(LOSCFG_COMPILER_ARM_NONE_EABI_CLANG), y)
        LITEOS_BASELIB     += -lclang_rt.builtins-arm
    else ifneq ($(findstring $(LOSCFG_COMPILER_RISCV_CLANG_UNKNOWN)$(LOSCFG_COMPILER_RISCV_CLANG_MUSL), y y),)
        LITEOS_BASELIB     += -lclang_rt.builtins-riscv32
    endif
else ifeq ($(LOSCFG_USING_GCC_NOPIC_LIB), y)
LITEOS_BASELIB     += -lgcc-nopic
else
LITEOS_BASELIB     += -lgcc
endif

ifneq ($(LOSCFG_LIB_LIBC), y)
LITEOS_BASELIB     += -lc
endif

ifneq ($(LOSCFG_COMPILER_ARM_NONE_EABI)$(LOSCFG_COMPILER_AARCH64_NONE_ELF)$(LOSCFG_COMPILER_RISCV_GCC_UNKNOWN)${LOSCFG_COMPILER_CLANG}, y)
ifneq ($(LOSCFG_COMPILER_XTENSA_32), y)
LITEOS_BASELIB     += -lgcc_eh
endif
endif

AS_OBJS_LIBC_FLAGS  = -D__ASSEMBLY__
WARNING_AS_ERROR   := -Wall -Werror

ifeq ($(LOSCFG_COMPILER_RISCV_CLANG_UNKNOWN)$(LOSCFG_COMPILER_RISCV_CLANG_MUSL)$(LOSCFG_COMPILER_LINGLONG), y)
WARNING_AS_ERROR   += -Wno-tautological-constant-out-of-range-compare -Wno-parentheses-equality -Wno-string-plus-int
endif

ifeq ($(LOSCFG_KERNEL_CPPSUPPORT), y)
ifeq ($(LOSCFG_COMPILER_CLANG), y)
    LITEOS_BASELIB += -lc++ -lc++abi -lunwind
else
    LITEOS_BASELIB += -lsupc++ -lstdc++
endif
endif

include $(LITEOSTOPDIR)/$(ARCH_PATH)/cpu.mk
include $(LITEOSTOPDIR)/targets/bsp.mk

include $(LITEOSTOPDIR)/lib/api.mk

include $(LITEOSTOPDIR)/drivers/api.mk

-include $(LITEOSTOPDIR)/compat/api.mk
-include $(LITEOSTOPDIR)/fs/api.mk
-include $(LITEOSTOPDIR)/shell/api.mk
-include $(LITEOSTOPDIR)/net/api.mk

ifeq ($(LOSCFG_KERNEL_DYNLOAD), y)
-include $(OUT)/dynload_dir/build/dynload_ld.mk
endif

LIB_SUBDIRS += self_src/$(KERNEL_PATH) self_src/lib self_src/drivers self_src/litekey

ifneq ($(wildcard $(LITEOSTOPDIR)/shell),)
LIB_SUBDIRS += self_src/shell
endif

ifneq ($(wildcard $(LITEOSTOPDIR)/compat),)
LIB_SUBDIRS += self_src/compat
endif

ifneq ($(wildcard $(LITEOSTOPDIR)/../../dt),)
LIB_SUBDIRS += self_src/../../dt
endif

ifeq ($(LOSCFG_LZ4_MEMCPY), y)
    ifneq ($(wildcard $(OPENSOURCEDIR)/lz4),)
    LIB_SUBDIRS += $(OPENSOURCEDIR)/lz4
    endif
endif

ifneq ($(LOSCFG_SAFETY_KERNEL), y)
# Export APIs from these module.
ifneq ($(wildcard $(LITEOSTOPDIR)/fs),)
LIB_SUBDIRS += self_src/fs
endif
ifneq ($(wildcard $(LITEOSTOPDIR)/net),)
LIB_SUBDIRS += self_src/net
endif
endif

LITEOS_KERNEL_INCLUDE := -I $(LITEOSTOPDIR)/$(KERNEL_PATH)/include
LITEOS_EXTKERNEL_INCLUDE := -I $(LITEOSTOPDIR)/$(KERNEL_PATH)/extended/include

# auto added the header file LITEOS_PLATFORM_MENUCONFIG_H in all files
LITEOS_PLATFORM_INCLUDE += -include $(LITEOS_PLATFORM_MENUCONFIG_H)
LITEOS_CXXINCLUDE       += -include $(LITEOS_PLATFORM_MENUCONFIG_H)

############################# Tools && Debug Option Begin ##############################

ifeq ($(LOSCFG_3RDPARTY), y)
    -include $(LITEOSTOPDIR)/3rdParty/3rdParty.mk
endif

ifeq ($(LOSCFG_TEST), y)
    -include $(LITEOSTOPDIR)/$(TEST_PATH)/test.mk
endif

ifeq ($(LOSCFG_NULL_ADDRESS_PROTECT), y)
    LITEOS_CMACRO += -DLOSCFG_NULL_ADDRESS_PROTECT
endif

############################# Tools && Debug Option End #################################

## Optimize Options
ifeq ($(LOSCFG_COMPILER_OPTIMIZE_NONE), y)
    LITEOS_COPTS_OPTIMIZE = -O0
else ifeq ($(LOSCFG_COMPILER_OPTIMIZE_SPEED), y)
    LITEOS_COPTS_OPTIMIZE = -O2
else ifeq ($(LOSCFG_COMPILER_OPTIMIZE_SIZE), y)
    LITEOS_COPTS_OPTIMIZE = -Os
endif

ifeq ($(LOSCFG_RISCV_COMPILER_CODESIZE_OPTIMIZE), y)
LITEOS_COPTS_OPTIMIZE += -mllvm --jump-is-expensive=false \
-mllvm --min-jump-table-entries=1000000 \
-mllvm --enable-oz-inline-threshold=true \
-mllvm --enable-machine-outliner=always \
-mllvm -machine-outliner-funcbytes=4 \
-mllvm -enable-rodata-sections=true \
-mllvm -enable-sequence-abstract=true \
-mllvm -enable-branch-imm-no-zero=true \
-mllvm --riscv-enable-copyelim=true
ifeq ($(LOSCFG_ARCH_LINXCORE_131), y)
LITEOS_COPTS_OPTIMIZE += -mllvm -imm-compare \
-mllvm -emit-muliadd \
-mllvm -merge-immshf \
-mllvm -emit-uxtb-uxth \
-mllvm -himideer-push-pop
else ifeq ($(LOSCFG_ARCH_LINXCORE_132)$(LOSCFG_ARCH_LINXCORE_170), y)
LITEOS_COPTS_OPTIMIZE += -mllvm -enable-lwgp=true \
-mllvm -enable-ldst-multiple-opt=true \
-mllvm -allow-unalign-ldst=true
endif
endif

ifeq ($(LOSCFG_COMPILE_DEBUG), y)
    LITEOS_COPTS_OPTION  = -g -gdwarf-2
endif

LITEOS_COPTS_DEBUG  += $(LITEOS_COPTS_OPTION) $(LITEOS_COPTS_OPTIMIZE)
LITEOS_CXXOPTS_BASE += $(LITEOS_COPTS_OPTION) $(LITEOS_COPTS_OPTIMIZE)
LITEOS_ASOPTS       += $(LITEOS_COPTS_OPTION)

## Security Options
ifeq ($(LOSCFG_CC_STACKPROTECTOR), y)
    LITEOS_SSP = -fstack-protector --param ssp-buffer-size=4
else ifeq ($(LOSCFG_CC_STACKPROTECTOR_STRONG), y)
    LITEOS_SSP = -fstack-protector-strong
else ifeq ($(LOSCFG_CC_STACKPROTECTOR_ALL), y)
    LITEOS_SSP = -fstack-protector-all
endif

# ASLR Options
ifeq ($(LOSCFG_LASLR_GOT), y)
    LITEOS_ASLR_OPTS = -fpie
    LITEOS_ASLR_LD_OPTS = -pie -z now
else ifeq ($(LOSCFG_LASLR_GSPR), y)
    LITEOS_ASLR_LD_OPTS = --emit-relocs
endif

LIBC_FILTER_OPTS  += -Wfloat-equal

LITEOS_COMMON_OPTS :=
ifneq ($(LOSCFG_LASLR), y)
    LITEOS_COMMON_OPTS += -fno-pic
endif

LITEOS_COMMON_OPTS += -fno-builtin -funsigned-char \
                     -ffunction-sections -fdata-sections \
                     $(WARNING_AS_ERROR) $(LITEOS_SSP) $(LIBC_FILTER_OPTS) \
                     $(LITEOS_ASLR_OPTS)

ifeq ($(LOSCFG_ARCH_XTENSA_XEA3), y)
# xea2 don't support shadow due to libc in compiler
LITEOS_COMMON_OPTS += -Wshadow
endif

LITEOS_COMMON_OPTS += -Wformat=2
ifeq ($(LOSCFG_COMPILER_CLANG)$(LOSCFG_COMPILER_XTENSA_32),)
LITEOS_COMMON_OPTS += -freg-struct-return -Wtrampolines
endif

ifeq ($(LOSCFG_LIB_LIBC), y)
    LITEOS_COMMON_OPTS += -nostdinc -nostdlib
endif

LITEOS_COPTS_BASE += $(LITEOS_COMMON_OPTS)

# Anonymous structs and unions are supported on c11; while gcc supports those features as extension
# which is turn on for default. As for Clang, if choose c99, -std=gnu99 should be used.
ifeq ($(LOSCFG_COMPILER_CLANG), y)
ifeq ($(LOSCFG_COMPILER_LINGLONG), y)
LITEOS_COPTS_BASE += -std=c99
else
LITEOS_COPTS_BASE += -std=c11
endif
else ifeq ($(LOSCFG_COMPILER_XTENSA_32), y)
LITEOS_COPTS_BASE += -std=gnu99
else
LITEOS_COPTS_BASE += -std=c99
endif

# -Wpointer-arith will treat the size of a void or of a function as 1.
# -Wstrict-prototypes will warn if a function is defined without specifying the argument types.
# -pipe will use pipes, save compilation time
LITEOS_COPTS_BASE += -Wpointer-arith -Wstrict-prototypes -fno-exceptions -pipe -fno-common

ifeq ($(LOSCFG_COMPILER_GCC), y)
LITEOS_COPTS_BASE += -fno-aggressive-loop-optimizations
endif

# clang support -fno-omit-frame-pointer
ifeq ($(LOSCFG_BACKTRACE_FP), y)
ifeq ($(LOSCFG_ARCH_ARM_CORTEX_M)$(LOSCFG_ARCH_XTENSA),)
LITEOS_COPTS_BASE += -fno-omit-frame-pointer
LITEOS_CXXOPTS_BASE += -fno-omit-frame-pointer
endif
endif

ifneq ($(LOSCFG_COMPILER_XTENSA_32), y)
LITEOS_COPTS_BASE += -Winvalid-pch
endif

ifeq ($(LOSCFG_ARCH_RISCV32)$(LOSCFG_COMPILER_ARM_NONE_EABI)$(LOSCFG_COMPILER_AARCH64_NONE_ELF),)
LITEOS_COPTS_BASE += -fno-short-enums
endif

LITEOS_CXXOPTS_BASE += -std=c++11 -fexceptions -fpermissive -fno-use-cxa-atexit -frtti \
                       $(LITEOS_COMMON_OPTS)

ifeq ($(LOSCFG_COMPILER_LINGLONG),)
LITEOS_CXXOPTS_BASE += -nostdinc++
endif

ifeq ($(LOSCFG_COMPILER_ARM_NONE_EABI)$(LOSCFG_COMPILER_AARCH64_NONE_ELF), y)
LITEOS_CXXOPTS_BASE += -Wno-deprecated
endif

ifneq ($(LOSCFG_COMPILER_XTENSA_32), y)
LITEOS_CXXOPTS_BASE += -Winvalid-pch
endif

ifeq ($(LOSCFG_LLTREPORT), y)
# -fprofile-arcs may introduce false alarm on 'maybe-uninitialized'
LITEOS_GCOV_OPTS := -fprofile-arcs -ftest-coverage -Wno-maybe-uninitialized
LITEOS_BASELIB += -lgcov
endif

LITEOS_LD_OPTS += -static --gc-sections $(LITEOS_ASLR_LD_OPTS)
ifeq ($(LOSCFG_TRUSTZONE)$(LOSCFG_SECURE_REGION), yy)
    LITEOS_LD_OPTS += --cmse-implib --out-implib=$(OUT)/obj/CMSE_importLib.o
else ifeq ($(LOSCFG_TRUSTZONE)_$(LOSCFG_SECURE_REGION), y_)
    LITEOS_LD_OPTS += $(OUT)/../secure/obj/CMSE_importLib.o
endif

ifeq ($(LOSCFG_MEM_DFX_DOUBLE_FREE_CHECK), y)
LITEOS_MEMORY_OPTS := --wrap=LOS_MemFree
endif
ifeq ($(LOSCFG_MEM_DFX_SHOW_CALLER_RA), y)
LITEOS_MEMORY_OPTS := --wrap=LOS_MemAlloc --wrap=LOS_MemAllocAlign --wrap=LOS_MemRealloc
ifeq ($(LOSCFG_MEM_MUL_MODULE)$(LOSCFG_KERNEL_MEM_BESTFIT), yy)
LITEOS_MEMORY_OPTS += --wrap=LOS_MemMalloc --wrap=LOS_MemMallocAlign --wrap=LOS_MemMrealloc
endif
ifeq ($(LOSCFG_KERNEL_MEM_SLAB_EXTENTION), y)
LITEOS_MEMORY_OPTS += --wrap=OsSlabAllocatorNew
endif
ifneq ($(LOSCFG_KERNEL_CPPSUPPORT)$(LOSCFG_COMPAT_POSIX),)
LITEOS_MEMORY_OPTS += --wrap=malloc --wrap=realloc --wrap=calloc --wrap=zalloc
endif
ifneq ($(LOSCFG_KERNEL_MEMBOX_STATIC)$(LOSCFG_KERNEL_MEMBOX_DYNAMIC),)
LITEOS_MEMORY_OPTS += --wrap=LOS_MemboxAlloc
endif
ifeq ($(LOSCFG_COMPAT_CMSIS_VER_1), y)
LITEOS_MEMORY_OPTS += --wrap=osPoolAlloc --wrap=osPoolCAlloc
endif
endif
LITEOS_LD_OPTS += $(LITEOS_MEMORY_OPTS)

LITEOS_LD_OPTS += $(LITEOS_DYNLOADOPTS)
LITEOS_LD_PATH += -L$(LITEOS_SCRIPTPATH)/ld -L$(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM) -L$(OUT)/lib -L$(LITEOS_LIB_BIGODIR) -L$(LITEOSTOPDIR)/tools/build
LITEOS_LD_PATH += -L$(LITEOS_COMPILER_GCCLIB_PATH) -L$(LITEOS_COMPILER_CXXLIB_PATH)

ifeq ($(LOSCFG_VENDOR), y)
LITEOS_LD_PATH +=  -L$(OUT)/lib/rdk \
                   -L$(OUT)/lib/sdk \
                   -L$(OUT)/lib/main_server
endif

ifeq ($(LOSCFG_USING_BOARD_LD), y)
ifeq ($(LOSCFG_BOX), y)
LITEOS_LD_SCRIPT := -T$(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/box/liteos.ld
else
LITEOS_LD_SCRIPT := -T$(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/liteos.ld
endif
else
LITEOS_LD_SCRIPT := -T$(RTOSLITETOPDIR)/liteos.ld
endif

# temporary
LITEOS_PLATFORM_INCLUDE += \
        -I $(LITEOSTOPDIR)/$(KERNEL_PATH)/base/include
LITEOS_CXXINCLUDE += \
        $(LITEOS_COMPILER_GCC_INCLUDE) \
        $(LITEOS_NET_INCLUDE) \
        $(LITEOS_SECURITY_INCLUDE) \
        -I $(LITEOSTOPDIR)/$(KERNEL_PATH)/base/include \
        -I $(LITEOSTOPDIR)/compat/posix/include \
        -I $(LITEOS_LIBC_PATH)/include \
        -I $(LITEOSTOPDIR)/fs/include \
        -I $(LITEOS_OPENSOURCE_PATHNAME)/incubator-nuttx/liteos/fs/include \
        -I $(LITEOSTOPDIR)/$(KERNEL_PATH)/include \
        $(LITEOS_LIBC_INCLUDE) \
        $(LITEOS_DRIVERS_INCLUDE)

LOSCFG_TOOLS_DEBUG_INCLUDE := $(LITEOS_SHELL_INCLUDE) $(LITEOS_UART_INCLUDE)

LITEOS_COPTS_NODEBUG    := $(LITEOS_NODEBUG) $(LITEOS_COPTS_BASE) $(LITEOS_COPTS_EXTRA)
LITEOS_BASE_INCLUDE     := $(LITEOS_KERNEL_INCLUDE) $(LITEOS_PLATFORM_INCLUDE) \
                           $(LITEOS_LIB_INCLUDE) $(LITEOS_FS_INCLUDE) \
                           $(LITEOS_EXTKERNEL_INCLUDE) \
                           $(LITEOS_COMPAT_INCLUDE) $(LITEOS_DRIVERS_INCLUDE) \
                           $(LOSCFG_TOOLS_DEBUG_INCLUDE) $(LITEOS_NET_INCLUDE) \
                           $(LITEOS_SECURITY_INCLUDE)
LITEOS_CFLAGS_NODEBUG   := $(LITEOS_COPTS_NODEBUG) $(LITEOS_CMACRO) \
                           $(LITEOS_CMACRO_TEST)   $(LITEOS_IMAGE_MACRO) \
                           $(LITEOS_BASE_INCLUDE)

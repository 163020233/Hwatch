#===============================================================================
# @brief    compiler file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
#===============================================================================

# LMS options
ifeq ($(LOSCFG_COMPILER_RISCV_CLANG_MUSL), y)
	LMS_COPTS :=
	LMS_CMACRO := -D__SANITIZE_ADDRESS__
else
	LMS_COPTS := -fsanitize=kernel-address
	LMS_CMACRO :=
	ifeq ($(LOSCFG_COMPILER_CLANG), y)
		LMS_COPTS += \
		-mllvm -asan-instrumentation-with-call-threshold=0     \
		-mllvm -asan-globals=0                                 \
		-mllvm -asan-stack=0
		LMS_CMACRO += -D__SANITIZE_ADDRESS__
	endif
endif

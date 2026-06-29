#===============================================================================
# @brief   Makefile
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
#============================== INCLUDES PATH ==================================
INCLUDES += \
	-I$(IDP_TOP_DIR)/overlay
#================================ LSP PATH =====================================
LSP_HIFI := $(IDP_OUT_DIR)/$(XTENSA_LSP_FILE)
LSPSRCFILE :=

ifndef BUILD_LIB
CFLAGS_EXT += -Werror
SRCFILE += component.c
SRCFILE += core_overlay.c
vpath %.c $(IDP_TOP_DIR)/overlay
endif
#===============================================================================
include $(IDP_BUILD_DIR)/hifi_rules.mak
#===============================================================================

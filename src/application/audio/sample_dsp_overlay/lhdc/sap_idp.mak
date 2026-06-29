#===============================================================================
# @brief   Makefile
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
#===================================================================================
PROJECT_ROOT := $(realpath $(IDP_TOP_DIR)/../../../../)
IDP_BUILD_DIR := $(IDP_TOP_DIR)/build/xtensa
IDP_LSP_DIR := $(IDP_BUILD_DIR)/arch/lsp
IDP_OUT_DIR := $(IDP_TOP_DIR)/out
IDP_OBJ_DIR := $(IDP_OUT_DIR)/obj
TXT_DIR     := $(IDP_OUT_DIR)/txt
#===================================================================================
CFG_SAP_LHDC_LIB0_SUPPORT=y
ifeq ($(CFG_SAP_LHDC_LIB0_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_LHDC_LIB0_SUPPORT
endif

SAP_FEATURE_CONFIGS += -DSAP_IPC_LOG_SUPPORT
SAP_FEATURE_CONFIGS += -DSAP_IPC_LOG_USE_DIAG_FRAME

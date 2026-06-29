#===============================================================================
# @brief    compiler file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
#===============================================================================

DYNLOADTESTDIR := $(LITEOSTOPDIR)/test/sample/kernel/extend/dynload
export DYNLOADTESTDIR

##### command to test dynload #####
-include $(DYNLOADTESTDIR)/test_dynload.mk

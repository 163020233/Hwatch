#===============================================================================
# @brief    compiler file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
#===============================================================================

MKYAFFS2DIR := $(LITEOSTOPDIR)/vendor/mkyaffs2image
export MKYAFFS2DIR

##### command to mkyaffs2image #####
-include $(MKYAFFS2DIR)/mkyaffs2image.mk

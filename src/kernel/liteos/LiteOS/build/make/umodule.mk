#===============================================================================
# @brief    compiler file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
#===============================================================================

LITEOS_ASFLAGS   += $(LITEOS_U_INCLUDES)
LITEOS_CFLAGS    += $(LITEOS_U_INCLUDES)

-include $(MK_PATH)/module.mk

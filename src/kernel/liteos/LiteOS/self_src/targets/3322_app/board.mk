#===============================================================================
# @brief    compiler file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
#===============================================================================

# C sources files

ifeq ($(LOSCFG_SECURE_REGION), y)
    USER_SRC  = $(wildcard $(LOCAL_PATH)/secure/*.c)
else
    USER_SRC  = $(wildcard $(LOCAL_PATH)/non_secure/*.c)
endif

# C includes files
USER_INC :=

# C definitions
C_DEFS =

LOCAL_SRCS += $(USER_SRC)
LOCAL_INCLUDE += $(USER_INC)
LOCAL_FLAGS += $(C_DEFS) $(LOCAL_INCLUDE)


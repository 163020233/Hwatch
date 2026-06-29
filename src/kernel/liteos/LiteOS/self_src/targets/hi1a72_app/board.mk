#===============================================================================
# @brief    compiler file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
#===============================================================================

USER_SRC =  \
        $(LITEOS_PLATFORM)/vendor/src/startup.S \
        $(LITEOS_PLATFORM)/vendor/src/hal_uart.c \
        $(LITEOS_PLATFORM)/vendor/src/hal_uart_v151.c \
        $(LITEOS_PLATFORM)/vendor/src/osal_interrupt.c \
        $(LITEOS_PLATFORM)/vendor/src/uart_porting.c \
        $(LITEOS_PLATFORM)/vendor/src/uart.c \
        $(LITEOS_PLATFORM)/vendor/src/tick_timer.c \

USER_INC = \
    -I $(LITEOS_PLATFORM)/vendor/include \
    -I ${LITEOS_OPENSOURCE_PATHNAME}/freebsd/liteos/drivers/mmc/core

LOCAL_SRCS    += $(USER_SRC)
LOCAL_INCLUDE += $(USER_INC)
LOCAL_FLAGS += $(LOCAL_INCLUDE) $(LITEOS_NON_SECURE_LOCAL_OPTS)

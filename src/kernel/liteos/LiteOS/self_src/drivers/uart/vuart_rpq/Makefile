include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS_y += vuart_rpq_core.c
LOCAL_SRCS_$(LOSCFG_DRIVERS_UART) += vuart_rpq_driver.c

LOCAL_INCLUDE := -I $(LITEOSTOPDIR)/drivers/uart

LOCAL_SRCS_NEED_SORT := y
LOCAL_SRCS = $(LOCAL_SRCS_y)
LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)

include $(MODULE)

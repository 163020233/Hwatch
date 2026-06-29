include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS_y += uart_core.c
ifeq ($(LOSCFG_DRIVERS_UART), y)
    LOCAL_SRCS_y += uart_ops.c
    LOCAL_SRCS_$(LOSCFG_FS_VFS) += uart_vfs.c
endif

ifneq ($(LOSCFG_DRIVERS_UART_VENDOR), y)
    MODULE_$(LOSCFG_DRIVERS_UART_ARM_PL011)     += arm_pl011
    MODULE_$(LOSCFG_DRIVERS_UART_HQ_UART)       += hq_uart_v5xx
    MODULE_$(LOSCFG_DRIVERS_UART_VUART_RPQ)      += vuart_rpq
endif

LOCAL_SRCS_NEED_SORT := y
LOCAL_SRCS = $(LOCAL_SRCS_y)
LOCAL_FLAGS := $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)

include $(MODULE)

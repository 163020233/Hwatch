include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))

ifeq ($(LOSCFG_SHELL), y)
LOCAL_SRCS := $(wildcard src/base/*.c) \

ifeq ($(LOSCFG_SHELL_VENDOR),)
ifeq ($(LOSCFG_SHELL_UART), y)
    ifeq ($(LOSCFG_DRIVERS_UART_LINUX), y)
        LOCAL_SRCS += src/base/porting/port_uart_linux.c
    else ifeq ($(LOSCFG_DRIVERS_UART_LITEOS), y)
        LOCAL_SRCS += src/base/porting/port_uart.c
    endif
endif

ifeq ($(LOSCFG_SHELL_CONSOLE), y)
LOCAL_SRCS += src/base/porting/port_console.c
endif
endif
endif

ifeq ($(LOSCFG_ENABLE_MAGICKEY), y)
LOCAL_SRCS += $(wildcard src/magickey/*.c)
endif

ifeq ($(LOSCFG_SHELL_EXTENDED_CMDS), y)
LOCAL_SRCS += $(wildcard src/cmds/*.c)
endif

ifeq ($(LOSCFG_SHELL), y)
LOCAL_INCLUDE := \
	-I $(LITEOSTOPDIR)/fs/nfs \
	-I $(LITEOSTOPDIR)/fs/ramfs \
	-I $(LITEOSTOPDIR)/fs/fat \
	-I $(LITEOSTOPDIR)/kernel_lite/extended/include
else
LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/shell/include
endif

ifeq ($(LOSCFG_TEST), y)
LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/$(TEST_PATH)/include
endif

LOCAL_SRCS_NEED_SORT := y
LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)

ifeq ($(LOSCFG_SHELL), y)
ifeq ($(LOSCFG_DYNAMIC_BOX), y)
ifeq ($(LOSCFG_PLATFORM_STM32F769IDISCOVERY), y)
LOCAL_FLAGS += -ffixed-r9
endif
endif
endif

ifeq ($(LOSCFG_SHELL_USER_TEXT), y)
include $(UMODULE)
else
include $(MODULE)
endif

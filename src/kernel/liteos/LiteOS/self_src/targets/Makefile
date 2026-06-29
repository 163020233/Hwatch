include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_PATH ?= $(LITEOS_PLATFORM)

LOCAL_SRCS := $(wildcard $(LOCAL_PATH)/*.c) \
              $(wildcard $(LOCAL_PATH)/mmu/*.c) \
              $(wildcard $(LOCAL_PATH)/extcmd/*.c)

ALL_ASSRCS := $(wildcard $(LITEOS_PLATFORM)/*.S)
ASSRCS := $(subst $(LITEOS_PLATFORM)/board.ld.S,,$(ALL_ASSRCS))
ifneq ($(LOSCFG_LASLR), y)
ASSRCS := $(subst $(LITEOS_PLATFORM)/aslr_func.S,,$(ASSRCS))
endif
LOCAL_SRCS += $(ASSRCS)

LOCAL_SRCS_$(LOSCFG_PLATFORM_ADAPT) += os_adapt/os_adapt.c

LOCAL_SRCS += $(addprefix $(LITEOS_PLATFORM)/,$(LOCAL_SRCS_y))
LOCAL_SRCS_NEED_SORT := y

LOCAL_INCLUDE := -I $(LITEOSTOPDIR)/$(KERNEL_PATH)/extended/include \
                 -I $(LITEOSTOPDIR)/drivers/mmc/host/$(MMC_HOST_DIR) \
                 -I $(LITEOSTOPDIR)/drivers/hieth-sf/include \
                 -I $(LITEOSTOPDIR)/$(KERNEL_PATH)/extended/perf

LOCAL_SRCS += $(wildcard $(LITEOS_PLATFORM)/flash/*.c)

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS)

-include $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/board.mk
LOCAL_FLAGS += $(TARGET_INCLUDE)
LOCAL_FLAGS += $(TARGET_EFLAGS)

include $(MODULE)

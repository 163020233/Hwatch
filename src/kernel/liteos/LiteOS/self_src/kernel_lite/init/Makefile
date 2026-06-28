include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME     := $(notdir $(shell pwd))

LOCAL_SRCS_y    :=
LOCAL_SRCS_$(LOSCFG_KERNEL_MIN) += los_init.c

ifeq ($(LOSCFG_LASLR)_$(LOSCFG_LASLR_EXTERNAL_RELOC), y_)
LOCAL_SRCS_y += los_aslr.c
endif

LOCAL_SRCS    = $(LOCAL_SRCS_y)
LOCAL_SRCS_NEED_SORT := y

LOCAL_INCLUDE   := \
                -I $(LITEOSTOPDIR)/kernel_lite/extended/include \
                -I $(LITEOSTOPDIR)/kernel_lite/extended/include/lowpower \
                -I $(LITEOSTOPDIR)/kernel_lite/extended/dynload/include

ifeq ($(LOSCFG_KERNEL_MEM_BESTFIT_LITTLE), y)
LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/kernel/base/mem/bestfit_little
endif
ifeq ($(LOSCFG_KERNEL_MEM_BESTFIT), y)
LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/kernel/base/mem/bestfit
endif

ifeq ($(LOSCFG_TEST), y)
LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/test/include
endif

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)

include $(MODULE)

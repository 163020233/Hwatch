include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS :=  $(wildcard src/*.c)

LOCAL_INCLUDE := -I $(LITEOSTOPDIR)/drivers/mmc/core
LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/fs/vfs/include/utrim

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)
LOCAL_FLAGS += -fno-strict-aliasing

include $(MODULE)


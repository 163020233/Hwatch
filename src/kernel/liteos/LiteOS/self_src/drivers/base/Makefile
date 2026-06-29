include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := driverbase

LOCAL_SRCS :=  $(wildcard src/*.c power/*.c)
LOCAL_FLAGS := $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)
LOCAL_SRCS_NEED_SORT := y

include $(MODULE)

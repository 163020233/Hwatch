# A simple Makefile for lib(libxxx.a)
# By Late Lee(http://www.latelee.org)

include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))

PROC_FREEBSD_PATH := $(LITEOS_OPENSOURCE_PATHNAME)/freebsd/liteos/fs/proc

LOCAL_SRCS := $(wildcard os_adapt/*.c) $(wildcard src/*.c)

LOCAL_SRCS += $(wildcard $(PROC_FREEBSD_PATH)/src/*.c)

LOCAL_INCLUDE := -I $(LITEOSTOPDIR)/fs/proc/include \

LOCAL_FLAGS := $(LITEOS_GCOV_OPTS) $(LOCAL_INCLUDE)
LOCAL_SRCS_NEED_SORT := y

include $(MODULE)

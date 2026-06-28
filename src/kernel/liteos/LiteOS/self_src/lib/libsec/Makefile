include $(RTOSLITETOPDIR)/config.mk
include $(RTOSLITETOPDIR)/build/make/lms.mk

MODULE_NAME := sec
LIBSEC_RELATIVE_PATH := $(HUAWEI_SECURE_C_DIR)

# In the minimum LIBC scenario, only the required memX_s/strX_s family of functions is included,
# and wide characters and floating point operations are not supported, if need you can add files.
ifneq ($(LOSCFG_LIB_FORMAT), y)
LOCAL_SRCS :=   memcpy_s.c  \
                memmove_s.c \
                memset_s.c  \
                strcpy_s.c  \
                strncpy_s.c \
                strcat_s.c  \
                strncat_s.c \
                strtok_s.c  \
                snprintf_s.c \
                vsnprintf_s.c \
                secureprintoutput_a.c

LOCAL_SRCS := $(addprefix $(LIBSEC_RELATIVE_PATH)/src/, $(LOCAL_SRCS))

# Wide characters and floating point are not supported
LITEOS_CFLAGS += -DSECUREC_HAVE_WCTOMB=0 -DSECUREC_ENABLE_SPRINTF_FLOAT=0
else
# STDIO macro enabled with all features (wide character and floating point default enabled)
LOCAL_SRCS := $(wildcard $(LIBSEC_RELATIVE_PATH)/src/*.c)
endif

ifeq ($(LOSCFG_KERNEL_LMS), y)
LOCAL_CFLAGS := $(LMS_COPTS) $(LMS_CMACRO)
endif
LOCAL_FLAGS := $(LITEOS_NON_SECURE_LOCAL_OPTS)

LOCAL_SRCS_NEED_SORT := y

include $(MODULE)

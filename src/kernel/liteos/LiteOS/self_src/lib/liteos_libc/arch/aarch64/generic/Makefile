include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := csysdeps

LOCAL_INCLUDE :=

# cmake LOCAL_SRCS_y is can not be empty, keep strcmp.S
LOCAL_SRCS := strcmp.S

# memcpy.S: Alignment fault
# memcmp.S: Alignment fault
# memset.S: floating pointer unit is not supported
ifeq ($(LOSCFG_ARCH_FPU_ENABLE), y)
LOCAL_SRCS += memset.S memcpy.S memcmp.S
endif

LOCAL_ASFLAGS := -Wa, $(AS_OBJS_LIBC_FLAGS)

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_NON_SECURE_LOCAL_OPTS)

include $(MODULE_LIBC)

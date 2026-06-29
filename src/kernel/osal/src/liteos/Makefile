-include $(BUILD_ENV_MK)
TOP_DIR_FROM_ENV := $(TOP_DIR)
OS_DIR_FROM_ENV := $(OS_DIR)
CFLAGS_FROM_ENV := $(CFLAGS)
OSAL_INCLUDE_FROM_ENV := $(OSAL_INCLUDE)
BUILDOUT_DIR_FROM_ENV := $(DRV_BUILD_DIR)
LIBOUT_FROM_ENV := $(DRV_LIB_DIR)

-include $(OS_DIR_FROM_ENV)/config.mk
PLATFORM_FROM_ENV := $(LITEOS_PLATFORM)

ARFLAGS = cr
RM = -rm -rf

#===============================================================================
# file path define
#===============================================================================
TARGET   := osal

FILES = $(wildcard *.c)
ifeq ($(OSAL_PROC_EN), n)
FILES := $(filter-out osal_proc.c,$(FILES))
endif
ifeq ($(OSAL_DEVICE_EN), n)
FILES := $(filter-out osal_device.c,$(FILES))
endif
ifeq ($(OSAL_DRVBOX_EN), n)
FILES := $(filter-out osal_drvbox.c,$(FILES))
endif

OBJS  = $(patsubst %.c,$(BUILDOUT_DIR_FROM_ENV)/%.o, $(FILES))
#===============================================================================
# compile flag
#===============================================================================

ifeq ("$(wildcard $(LITEOSTOPDIR)/arch_lite)", "")
LITEOS_ARCH := $(LITEOSTOPDIR)/arch
else
LITEOS_ARCH := $(LITEOSTOPDIR)/arch_lite
endif
 
ifeq ("$(wildcard $(LITEOSTOPDIR)/kernel_lite)", "")
LITEOS_KERNEL := $(LITEOSTOPDIR)/kernel
else
LITEOS_KERNEL := $(LITEOSTOPDIR)/kernel_lite
endif

 
ifeq ("$(wildcard $(RTOSLITETOPDIR)/platform/huawei_secure_c)", "")
SECURE_INCLUDE := $(RTOSLITETOPDIR)/../../../middleware/utils/hwsec_c
else
SECURE_INCLUDE := $(RTOSLITETOPDIR)/platform/huawei_secure_c
endif

OSAL_CFLAGS := -I$(DRV_INCLUDE) \
               -I$(OSAL_INCLUDE_FROM_ENV) \
               -I$(LITEOS_ARCH)/riscv/include \
               -I$(LITEOSTOPDIR)/compat/linux/include \
               -I$(LITEOSTOPDIR)/drivers/base/include \
               -I$(LITEOSTOPDIR)/drivers/interrupt/include \
               -I$(LITEOSTOPDIR)/drivers/timer/include \
               -I$(LITEOS_KERNEL)/base/include \
               -I$(LITEOS_KERNEL)/include \
               -I$(LITEOSTOPDIR)/lib/liteos_libc/include \
               -I$(LITEOSTOPDIR)/fs/include \
               -I$(LITEOSTOPDIR)/targets/$(PLATFORM_FROM_ENV)/include \
               -I$(SECURE_INCLUDE)/include \
               -I$(RTOSLITETOPDIR)/open_source/musl/include \
               -I$(RTOSLITETOPDIR)/open_source/musl/arch/riscv32 \
               -I$(RTOSLITETOPDIR)/open_source/musl/arch/generic

OSAL_CFLAGS += -Werror -D__LITEOS__ -nostdinc -nostdlib -ffunction-sections -fdata-sections -fstack-protector-strong
OSAL_CFLAGS += -Wno-implicit-function-declaration

OSAL_CFLAGS  +=  $(CFLAGS_FROM_ENV) -fno-lto

LIBM = $(LIBOUT_FROM_ENV)/libhi_$(TARGET).a

#===============================================================================
#   rules
#===============================================================================
.PHONY: all clean
all: $(LIBM)

clean: 
	$(RM) $(LIBM) $(BUILDOUT_DIR_FROM_ENV)/* $(LIBOUT_FROM_ENV)

#===============================================================================
# useage: $(call make-ar,lib,source)
#===============================================================================
define make-ar
	@echo "[AR]  $1"
	@mkdir -p $(dir $1)
	@$(AR) $(ARFLAGS) $1 $2
endef

#===============================================================================
# useage: $(call make-depend-compile,source-file,object-file,depend-file,complie-flags)
#===============================================================================
define make-depend-compile
	@echo "[CC]  $1"
	@mkdir -p $(dir $2)
	@$(CC) -MM -MF $3 -MP -MT $2 $4 $1
	@$(CC) $4 -o $2 -c $1
endef

$(LIBM): $(OBJS)
	$(call make-ar,$@,$^)

$(BUILDOUT_DIR_FROM_ENV)/%.o: %.c
	$(call make-depend-compile,$<,$@,$(subst .o,.d,$@),$(OSAL_CFLAGS))
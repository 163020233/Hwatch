include $(RTOSLITETOPDIR)/config.mk

USB_ROOT_PATH := ./

ADAPT_OS := adapt_liteos
OPENSOURCE_PATH := $(OPENSOURCEDIR)

MODULE_NAME := usb_base

USB_FREEBSD_PATH := $(OPENSOURCE_PATH)/freebsd/liteos/drivers/usb
GADGET_SRC = gadget
FREEBSD_GADGET_SRC = $(USB_FREEBSD_PATH)/$(GADGET_SRC)
IMPLEMENTATION_SRC = implementation

ADAPT_OS_PATH := $(USB_ROOT_PATH)/$(ADAPT_OS)
ADAPT_KAL_PATH := $(ADAPT_OS_PATH)/kal/include

NUTTX_USB_PATH := $(OPENSOURCE_PATH)/incubator-nuttx/liteos/drivers/usb
NUTTX_USB_HEAD_PATH := $(LITEOSTOPDIR)/compat/nuttx/include

LOCAL_PATH := $(shell pwd)

LOCAL_INCLUDE := -I $(LOCAL_PATH)/$(USB_ROOT_PATH)                \
                 -I $(LOCAL_PATH)/$(ADAPT_OS_PATH)                \
                 -I $(LOCAL_PATH)/$(ADAPT_KAL_PATH)               \
                 -I $(LOCAL_PATH)/$(NUTTX_USB_PATH)               \
                 -I $(NUTTX_USB_HEAD_PATH)

ifeq ($(LOSCFG_FS_VFS), y)
    LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/fs/vfs/include/driver
endif

ifeq ($(LOSCFG_FS_FAT_CACHE), y)
    LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/fs/vfs/include/bcache
endif

ifneq ($(LOSCFG_DRIVERS_USB_DWC_DRIVER), )
LOCAL_SRCS := $(ADAPT_OS_PATH)/$(IMPLEMENTATION_SRC)/usb_init.c  \
              $(IMPLEMENTATION_SRC)/usb_version.c                \
              $(IMPLEMENTATION_SRC)/_intr_ref.c                  \
              $(IMPLEMENTATION_SRC)/usb_dma_cache.c
endif

ifeq ($(LOSCFG_DRIVERS_USB_COMPOSITE_GADGET), y)
LOCAL_SRCS += $(GADGET_SRC)/composite_open.c
endif

ifneq ($(LOSCFG_DRIVERS_USB_UVC_GADGET)$(LOSCFG_DRIVERS_USB_CAMERA_GADGET), )
LOCAL_SRCS += $(GADGET_SRC)/f_uvc_open.c \
              $(GADGET_SRC)/usbd_video_ioctl_open.c
endif

ifeq ($(LOSCFG_DRIVERS_USB_MASS_STORAGE_GADGET), y)
LOCAL_SRCS += $(FREEBSD_GADGET_SRC)/f_mass_storage_open.c \
              $(GADGET_SRC)/f_mass_storage_ram_open.c

ifeq ($(LOSCFG_DRIVERS_USB_MASS_STORAGE_EXT_SER_FUNC), y)
LOCAL_SRCS += $(GADGET_SRC)/f_mass_ser.c
endif

LOCAL_INCLUDE += -I $(LOCAL_PATH)/$(USB_FREEBSD_PATH)
endif

# Therefore, the Wno-switch-default compilation option needs to be added. #
LOCAL_FLAGS += $(LITEOS_GCOV_OPTS) $(LOCAL_INCLUDE) $(LITEOS_SUGGESTED_COMPILE_OPTS) -Wno-switch-default

LOCAL_SRCS_NEED_SORT := y

include $(MODULE)

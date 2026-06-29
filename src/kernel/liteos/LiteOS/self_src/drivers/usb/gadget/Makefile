include $(RTOSLITETOPDIR)/config.mk

USB_ROOT_PATH := ../

ADAPT_OS := adapt_liteos
OPENSOURCE_PATH := $(OPENSOURCEDIR)

MODULE_NAME := usb_device

NUTTX_USB_PATH := $(OPENSOURCE_PATH)/incubator-nuttx/liteos/drivers/usb
NUTTX_USB_GADGET_PATH := $(NUTTX_USB_PATH)/gadget
NUTTX_USB_HEAD_PATH := $(LITEOSTOPDIR)/compat/nuttx/include

ADAPT_OS_PATH := $(USB_ROOT_PATH)/$(ADAPT_OS)
ADAPT_KAL_PATH := $(ADAPT_OS_PATH)/kal/include

CONTROLLER_DEVICE_SRC = $(USB_ROOT_PATH)/controller/usb_device
ADAPT_OS_CONTROLLER_DEVICE_SRC = $(ADAPT_OS_PATH)/controller/usb_device
USB3_SELF_DEVELOP_CONTROLLER_PATH = $(USB_ROOT_PATH)/controller/usb_device/hiusbc_controller

LOCAL_PATH := $(shell pwd)

LOCAL_INCLUDE := -I $(LOCAL_PATH)/$(USB_ROOT_PATH)                \
                 -I $(LOCAL_PATH)/$(ADAPT_OS_PATH)                \
                 -I $(LOCAL_PATH)/$(ADAPT_KAL_PATH)               \
                 -I $(LOCAL_PATH)/$(NUTTX_USB_PATH)               \
                 -I $(LOCAL_PATH)/$(NUTTX_USB_GADGET_PATH)        \
                 -I $(NUTTX_USB_HEAD_PATH)

ifeq ($(LOSCFG_DRIVERS_USB2_DEVICE_CONTROLLER), y)
LOCAL_SRCS += $(CONTROLLER_DEVICE_SRC)/dwc_otg_pcd_intr.c  \
              $(CONTROLLER_DEVICE_SRC)/dwc_otg_pcd.c       \
              $(CONTROLLER_DEVICE_SRC)/udc.c
endif

ifeq ($(LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER), y)
LOCAL_SRCS += $(CONTROLLER_DEVICE_SRC)/cil_intr.c            \
              $(CONTROLLER_DEVICE_SRC)/cil.c                 \
              $(CONTROLLER_DEVICE_SRC)/ep0.c                 \
              $(CONTROLLER_DEVICE_SRC)/linux_gadget.c        \
              $(ADAPT_OS_CONTROLLER_DEVICE_SRC)/linux_plat.c \
              $(CONTROLLER_DEVICE_SRC)/pcd_intr.c            \
              $(CONTROLLER_DEVICE_SRC)/pcd.c
endif

ifeq ($(LOSCFG_DRIVERS_HiUSBC3X_CONTROLLER), y)
LOCAL_SRCS += $(USB3_SELF_DEVELOP_CONTROLLER_PATH)/hiusbc_adapt.c    \
              $(USB3_SELF_DEVELOP_CONTROLLER_PATH)/hiusbc_core.c     \
              $(USB3_SELF_DEVELOP_CONTROLLER_PATH)/hiusbc_transfer.c \
              $(USB3_SELF_DEVELOP_CONTROLLER_PATH)/hiusbc_event.c    \
              $(USB3_SELF_DEVELOP_CONTROLLER_PATH)/hiusbc_gadget.c   \
              $(USB3_SELF_DEVELOP_CONTROLLER_PATH)/hiusbc_setup.c

endif

ifeq ($(LOSCFG_DRIVERS_USB_COMPOSITE_GADGET), y)
LOCAL_SRCS += composite_adapter.c \
              $(NUTTX_USB_GADGET_PATH)/composite.c
endif

ifneq ($(LOSCFG_DRIVERS_USB_ETH_SER_GADGET)$(LOSCFG_DRIVERS_USB_ECM_SER_GADGET), )
LOCAL_SRCS += f_multi.c
endif

ifneq ($(LOSCFG_DRIVERS_USB_SERIAL_GADGET)$(LOSCFG_DRIVERS_USB_ETH_SER_GADGET)$(LOSCFG_DRIVERS_USB_ECM_SER_GADGET) \
       $(LOSCFG_DRIVERS_USB_UAC_HID_EXT_SER_FUNC)$(LOSCFG_DRIVERS_USB_MASS_STORAGE_EXT_SER_FUNC), )
LOCAL_SRCS += usbd_acm.c \
              f_acm.c
endif

ifneq ($(LOSCFG_DRIVERS_USB_ECM_GADGET)$(LOSCFG_DRIVERS_USB_ECM_SER_GADGET), )
LOCAL_SRCS += f_ecm.c \
              usbd_eth_common.c
endif

ifneq ($(LOSCFG_DRIVERS_USB_ETHERNET_GADGET)$(LOSCFG_DRIVERS_USB_ETH_SER_GADGET), )
LOCAL_SRCS += f_rndis.c \
              $(NUTTX_USB_GADGET_PATH)/rndis.c \
              usbd_eth_common.c
LOCAL_SRCS += $(wildcard $(NUTTX_USB_GADGET_PATH)/queue/*.c)
endif

ifneq ($(LOSCFG_DRIVERS_USB_HID_GADGET)$(LOSCFG_DRIVERS_USB_UAC_HID_GADGET), )
LOCAL_SRCS += f_hid.c \
              usbd_hid.c
endif

ifeq ($(LOSCFG_DRIVERS_USB_DFU_GADGET), y)
LITEOS_CFLAGS += -DLOSCFG_DRIVERS_DFU_GADGET
LOCAL_SRCS += f_dfu.c
endif

ifneq ($(LOSCFG_DRIVERS_USB_UVC_GADGET)$(LOSCFG_DRIVERS_USB_CAMERA_GADGET), )
LOCAL_SRCS += usbd_video.c \
              camera_control.c
endif

ifneq ($(LOSCFG_DRIVERS_USB_UAC_GADGET)$(LOSCFG_DRIVERS_USB_CAMERA_GADGET)$(LOSCFG_DRIVERS_USB_UAC_HID_GADGET), )
LOCAL_SRCS += f_uac.c \
              usbd_audio.c
endif

ifeq ($(LOSCFG_DRIVERS_USB_CAMERA_GADGET), y)
LOCAL_SRCS += f_camera.c
endif

ifeq ($(LOSCFG_DRIVERS_USB_UAC_HID_GADGET), y)
LOCAL_SRCS += f_uac_hid.c
endif

ifneq ($(findstring 0x,$(LOSCFG_USB_DMA_OFFSET)),)
LOCAL_CFLAGS += -DCFG_USB_DMA_OFFSET=$(LOSCFG_USB_DMA_OFFSET)
endif

LOCAL_FLAGS += $(LITEOS_GCOV_OPTS) $(LOCAL_INCLUDE) $(LITEOS_SUGGESTED_COMPILE_OPTS)

LOCAL_SRCS_NEED_SORT := y

include $(MODULE)

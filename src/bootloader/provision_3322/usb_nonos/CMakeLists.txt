#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "usb_nonos_3322")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cil_intr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cil.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/composite_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/composite_open.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/composite.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ep0.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/f_dfu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/linux_gadget.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/linux_plat.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pcd_intr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pcd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usb_phy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usb_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usb2.0_phy_3322.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/_intr_ref.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usb_dma_cache.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usb_version.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usb_porting.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usb_dfu_sample.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/testcase
    ${CMAKE_CURRENT_SOURCE_DIR}/include/controller
    ${CMAKE_CURRENT_SOURCE_DIR}/include/implementation
    ${CMAKE_CURRENT_SOURCE_DIR}/include/kal
    ${CMAKE_CURRENT_SOURCE_DIR}/include/phy
    ${CMAKE_CURRENT_SOURCE_DIR}/include/nuttx
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gadget
    ${CMAKE_CURRENT_SOURCE_DIR}/../../non_os/irmalloc/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../osal/include/schedule
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
    -DLOSCFG_DRIVERS_USB
    -DLOSCFG_DRIVERS_USB_DWC_DRIVER
    -DLOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
    -DLOSCFG_DRIVERS_USB3_DEVICE_USB_MODE_USB_3_0
    -DLOSCFG_DRIVERS_USB_DEVICE_SLAVE_CONFIG
    -DLOSCFG_USB_OUT_EP_NUM=3
    -DLOSCFG_USB_IN_EP_NUM=3
    -DLOSCFG_USB_GEVENT_BUF_NUM=0
    -DLOSCFG_USB_DEPCFG_INTR_NUM=0
    -DLOSCFG_USB_DMA_OFFSET="0x0"
    -DLOSCFG_DRIVERS_USB_GADGET
    -DLOSCFG_DRIVERS_USB_DFU_GADGET
    -DLOSCFG_DRIVERS_USB_DFU_DOWNLOAD_CALLBACK
    -DLOSCFG_DRIVERS_USB_DFU_DRIVE_FREE
    -DLOSCFG_DRIVERS_USB_COMPOSITE_GADGET
    -D__NONOS__
    # -DDWC3_CIL_INTR_DEBUG
    # -DDWC3_PCD_INTR_DEBUG
    # -DDWC3_CIL_DEBUG
    # -DDWC3_EP0_DEBUG
    # -DDWC3_LINUX_GADGET_DEBUG
    # -DDWC_USB3_LINUX_PLAT_DEBUG
    -DDWC3_PCD_DEBUG
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error=unused-parameter
    -Wno-parentheses-equality
    -Wno-unused-parameter
    -Wno-unused-function
    -Wno-typedef-redefinition
    -Wno-implicit-function-declaration
    -Wno-unused-variable
    -Wno-unused-but-set-variable
    -Wno-unused-label
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/include/ "*")
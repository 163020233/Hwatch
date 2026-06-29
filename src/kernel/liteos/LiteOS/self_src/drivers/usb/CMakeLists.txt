set(MODULE_NAME usb_base)

set(USB_ROOT_PATH ${LITEOSTOPDIR}/drivers/usb)
set(OPENSOURCE_PATH ${OPENSOURCEDIR})
set(USB_FREEBSD_PATH ${OPENSOURCE_PATH}/freebsd/liteos/drivers/usb)

set(CONTROLLER_HOST_SRC controller/usb_host)
set(FREEBSD_CONTROLLER_HOST_SRC ${USB_FREEBSD_PATH}/${CONTROLLER_HOST_SRC})

set(CORE_SRC core)
set(FREEBSD_CORE_SRC ${USB_FREEBSD_PATH}/${CORE_SRC})

set(GADGET_SRC gadget)
set(FREEBSD_GADGET_SRC ${USB_FREEBSD_PATH}/${GADGET_SRC})

set(IMPLEMENTATION_SRC implementation)
set(FREEBSD_IMPLEMENTATION_SRC ${USB_FREEBSD_PATH}/${IMPLEMENTATION_SRC})

set(QUIRK_SRC quirk)
set(FREEBSD_QUIRK_SRC ${USB_FREEBSD_PATH}/${QUIRK_SRC})

set(STORAGE_SRC storage)
set(FREEBSD_STORAGE_SRC ${USB_FREEBSD_PATH}/${STORAGE_SRC})

set(HID_SRC hid)
set(FREEBSD_HID_SRC ${USB_FREEBSD_PATH}/${HID_SRC})

set(UVC_SRC uvc)

set(UAC_SRC uac)
set(FREEBSD_UAC_SRC ${USB_FREEBSD_PATH}/${UAC_SRC})

set(Net_SRC net)
set(FREEBSD_Net_SRC ${USB_FREEBSD_PATH}/${Net_SRC})

set(ADAPT_OS adapt_liteos)
set(ADAPT_OS_PATH ${USB_ROOT_PATH}/${ADAPT_OS})
set(ADAPT_KAL_PATH ${ADAPT_OS_PATH}/kal/include)
set(ADAPT_OS_FREEBSD_USB_PATH ${USB_FREEBSD_PATH}/${ADAPT_OS})
set(ADAPT_OS_CONTROLLER_HOST_SRC ${ADAPT_OS_PATH}/${CONTROLLER_HOST_SRC})

set(NUTTX_USB_PATH ${OPENSOURCE_PATH}/incubator-nuttx/liteos/drivers/usb)
set(NUTTX_USB_HEAD_PATH ${LITEOSTOPDIR}/compat/nuttx/include)

# add global include, compile macro and options to public target #
set(MODULE_INCLUDE_PUB)
set(MODULE_CXXINCLUDE_PUB)
set(MODULE_COPTS_PUB)
set(MODULE_ASOPTS_PUB)
set(MODULE_CXXOPTS_PUB)
set(MODULE_CMACRO_PUB)
set(MODULE_ASMACRO_PUB)
set(MODULE_CXXMACRO_PUB)

if(LOSCFG_USB_DEBUG)
list(APPEND MODULE_CMACRO_PUB LOSCFG_USB_DEBUG)
endif()

# add local include, compile macro and options to private target #
set(MODULE_INCLUDE_PRI
    ${USB_ROOT_PATH}
    ${ADAPT_OS_PATH}
    ${ADAPT_KAL_PATH}
    ${NUTTX_USB_PATH}
    ${NUTTX_USB_HEAD_PATH}
    ${IMPLEMENTATION_SRC}
)

if(LOSCFG_DRIVERS_USB_HOST_DRIVER)
    list(APPEND MODULE_INCLUDE_PRI
        ${USB_FREEBSD_PATH}
        ${ADAPT_OS_FREEBSD_USB_PATH}
	${NUTTX_USB_PATH}/${STORAGE_SRC}
    )
endif()

# Therefore, the Wno-switch-default compilation option needs to be added. #
set(MODULE_COPTS_PRI ${LITEOS_SUGGESTED_COMPILE_OPTS} -Wno-switch-default)
set(MODULE_ASOPTS_PRI)
set(MODULE_CXXOPTS_PRI)
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

# add srcs to private target #
if(LOSCFG_DRIVERS_USB_DWC_DRIVER OR LOSCFG_DRIVERS_USB_HOST_DRIVER)
    set(LOCAL_SRCS_y
        ${ADAPT_OS_PATH}/${IMPLEMENTATION_SRC}/usb_init.c
        ${IMPLEMENTATION_SRC}/usb_version.c
        ${IMPLEMENTATION_SRC}/_intr_ref.c
        ${IMPLEMENTATION_SRC}/usb_dma_cache.c
    )
endif()

list(APPEND LOCAL_SRCS_${LOSCFG_DRIVERS_USB_HOST_DRIVER}
    ${IMPLEMENTATION_SRC}/usb_btree.c
    ${IMPLEMENTATION_SRC}/usb_alloc.c
    ${FREEBSD_IMPLEMENTATION_SRC}/_condvar_ref.c
    ${FREEBSD_IMPLEMENTATION_SRC}/_timer_ref.c
    ${FREEBSD_IMPLEMENTATION_SRC}/bsd_busspace.c
    ${FREEBSD_IMPLEMENTATION_SRC}/usb_busdma_loader.c
    ${FREEBSD_IMPLEMENTATION_SRC}/_sx_ref.c
    ${ADAPT_OS_FREEBSD_USB_PATH}/${IMPLEMENTATION_SRC}/bsd_kernel.c
    ${FREEBSD_CORE_SRC}/usb_debug.c
    ${FREEBSD_CORE_SRC}/usb_dynamic.c
    ${FREEBSD_CORE_SRC}/usb_parse.c
    ${FREEBSD_CORE_SRC}/usb_error.c
    ${FREEBSD_CORE_SRC}/usb_handle_request.c
    ${FREEBSD_CORE_SRC}/usb_util.c
    ${FREEBSD_CORE_SRC}/usb_lookup.c
    ${FREEBSD_CORE_SRC}/usb_device.c
    ${FREEBSD_CORE_SRC}/usb_process.c
    ${FREEBSD_CORE_SRC}/usb_hub.c
    ${FREEBSD_CORE_SRC}/usb_request.c
    ${FREEBSD_CORE_SRC}/usb_transfer.c
    ${FREEBSD_CORE_SRC}/usb_mbuf.c
    ${FREEBSD_CONTROLLER_HOST_SRC}/usb_controller.c
    ${FREEBSD_QUIRK_SRC}/usb_quirk.c
)

list(APPEND LOCAL_SRCS_${LOSCFG_DRIVERS_USB_COMPOSITE_GADGET}
    ${GADGET_SRC}/composite_open.c
)

if(LOSCFG_DRIVERS_USB_UVC_GADGET OR LOSCFG_DRIVERS_USB_CAMERA_GADGET)
    list(APPEND LOCAL_SRCS_y
        ${GADGET_SRC}/f_uvc_open.c
        ${GADGET_SRC}/usbd_video_ioctl_open.c
    )
endif()

if(LOSCFG_DRIVERS_USB_MASS_STORAGE_GADGET)
    list(APPEND LOCAL_SRCS_y
        ${FREEBSD_GADGET_SRC}/f_mass_storage_open.c
        ${GADGET_SRC}/f_mass_storage_ram_open.c
    )

    if(LOSCFG_DRIVERS_USB_MASS_STORAGE_EXT_SER_FUNC)
        list(APPEND LOCAL_SRCS_y ${GADGET_SRC}/f_mass_ser.c)
    endif()

    list(APPEND MODULE_INCLUDE_PRI
        ${USB_FREEBSD_PATH}
    )
endif()

list(APPEND LOCAL_SRCS_${LOSCFG_DRIVERS_USB_MASS_STORAGE_GADGET}
    ${FREEBSD_GADGET_SRC}/f_mass_storage_open.c
    ${GADGET_SRC}/f_mass_storage_ram_open.c
)

if(LOSCFG_DRIVERS_USB_HOST_UVC_CLASS)
    list(APPEND LOCAL_SRCS_y
        ${UVC_SRC}/host_uvc.c
    )
endif()

list(APPEND LOCAL_SRCS_${LOSCFG_DRIVERS_USB_FIFO}
    ${FREEBSD_CORE_SRC}/usb_dev.c
)

list(APPEND LOCAL_SRCS_${LOSCFG_DRIVERS_USB_HOST_HID}
    ${FREEBSD_HID_SRC}/hid.c
    ${FREEBSD_HID_SRC}/uhid.c
    ${FREEBSD_HID_SRC}/usb_hid.c
)

if(LOSCFG_DRIVERS_USB_HOST_XHCI)
    list(APPEND LOCAL_SRCS_y
        ${FREEBSD_CONTROLLER_HOST_SRC}/xhci.c
        ${ADAPT_OS_CONTROLLER_HOST_SRC}/xhci_hcd.c
    )
endif()

if(LOSCFG_DRIVERS_USB_MASS_STORAGE)
    list(APPEND LOCAL_SRCS_y
        ${FREEBSD_STORAGE_SRC}/umass.c
    )
endif()

if(LOSCFG_DRIVERS_USB_HOST_UAC_CLASS)
    list(APPEND LOCAL_SRCS_y
        ${FREEBSD_UAC_SRC}/uaudio_pcm.c
        ${FREEBSD_UAC_SRC}/uaudio.c
    )
    list(APPEND MODULE_INCLUDE_PRI
        ${OPENSOURCE_PATH}/freebsd
        ${FREEBSD_HID_SRC}
    )
endif()

list(APPEND LOCAL_SRCS_${LOSCFG_DRIVERS_USB_NET_ASIX}
    ${FREEBSD_Net_SRC}/if_axe.c
)

if(LOSCFG_DRIVERS_USB_HOST_NET_CLASS)
    list(APPEND LOCAL_SRCS_y
        ${FREEBSD_Net_SRC}/usb_ethernet.c
        ${Net_SRC}/usb_eth_drv.c
    )
    list(APPEND MODULE_INCLUDE_PRI
        ${ROOT_DIR}/ohos/third_party/lwip/lwip_adapter/liteos/src/include
        ${ROOT_DIR}/ohos/third_party/lwip/lwip_v2.1.3/src/include/
    )
endif()

FOREACH(CUR_DIR ${LOCAL_DIRS_y})
    aux_source_directory(${CUR_DIR} LOCAL_SRCS_y)
ENDFOREACH(CUR_DIR)

set(LOCAL_SRCS ${LOCAL_SRCS_y})  # module.cmake will sort LOCAL_SRCS files
include(${MODULE})

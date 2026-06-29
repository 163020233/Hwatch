#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(BASE_PLATFORM_PATH wear)

if (DEFINES MATCHES "HI3322_CHIP_V100" OR DEFINES MATCHES "HI3322_PRODUCT_FPGA" )
	set(CHIP_NAME 3322)
	set(VERSION_HAL_IP v3)
	set(PLATFORM_PATH litos)
		
	set(
		viu_drv_list
			${CMAKE_CURRENT_SOURCE_DIR}/intf/${PLATFORM_PATH}/drv_viu_intf.c
			${CMAKE_CURRENT_SOURCE_DIR}/intf/${PLATFORM_PATH}/drv_viu_ioctl.c
			
			${CMAKE_CURRENT_SOURCE_DIR}/mkp/videv/drv_videv.c
			${CMAKE_CURRENT_SOURCE_DIR}/mkp/videv/viu_dev.c
			
			${CMAKE_CURRENT_SOURCE_DIR}/mkp/vicap/drv_vicap.c
			${CMAKE_CURRENT_SOURCE_DIR}/mkp/vicap/viu_pipe.c

			${CMAKE_CURRENT_SOURCE_DIR}/common/viu_drv_comm_vicap.c
			${CMAKE_CURRENT_SOURCE_DIR}/common/viu_public.c
			${CMAKE_CURRENT_SOURCE_DIR}/common/viu_common_vb.c
			${CMAKE_CURRENT_SOURCE_DIR}/common/viu_vicap.c
			
			${CMAKE_CURRENT_SOURCE_DIR}/extern/chip/${CHIP_NAME}/viu_extern_mem.c
			${CMAKE_CURRENT_SOURCE_DIR}/extern/chip/${CHIP_NAME}/viu_extern_venc.c
			
			${CMAKE_CURRENT_SOURCE_DIR}/mkp/ctrl/viu_pic_queue.c
			${CMAKE_CURRENT_SOURCE_DIR}/mkp/ctrl/viu_userframe.c
			
			${CMAKE_CURRENT_SOURCE_DIR}/mkp/context/wdr_drv.c
			${CMAKE_CURRENT_SOURCE_DIR}/mkp/context/viu_drv_vicap.c
			
			${CMAKE_CURRENT_SOURCE_DIR}/hal/chip/${CHIP_NAME}/hal_videv_intf.c
			${CMAKE_CURRENT_SOURCE_DIR}/hal/chip/${CHIP_NAME}/hal_vicap_intf.c
			${CMAKE_CURRENT_SOURCE_DIR}/hal/chip/${CHIP_NAME}/hal_viproc_intf.c
			${CMAKE_CURRENT_SOURCE_DIR}/hal/chip/${CHIP_NAME}/hal_capability_intf.c
			${CMAKE_CURRENT_SOURCE_DIR}/hal/chip/${CHIP_NAME}/policy/hal_adapt_comm.c
			${CMAKE_CURRENT_SOURCE_DIR}/hal/chip/${CHIP_NAME}/policy/viu_hal.c
					
			${CMAKE_CURRENT_SOURCE_DIR}/hal/ip/${VERSION_HAL_IP}/chn/hal_vicap_reg_chn.c
			${CMAKE_CURRENT_SOURCE_DIR}/hal/ip/${VERSION_HAL_IP}/crg/hal_vicap_reg_crg.c
			${CMAKE_CURRENT_SOURCE_DIR}/hal/ip/${VERSION_HAL_IP}/global/hal_vicap_reg_global.c
			${CMAKE_CURRENT_SOURCE_DIR}/hal/ip/${VERSION_HAL_IP}/pt/hal_videv_reg_pt.c

			${CMAKE_CURRENT_SOURCE_DIR}/mkp/dfx/viu_proc.c
	)

	set(
		viu_drv_header_list
			${CMAKE_CURRENT_SOURCE_DIR}/../include/
			
			${CMAKE_CURRENT_SOURCE_DIR}/mkp/videv
			${CMAKE_CURRENT_SOURCE_DIR}/mkp/vicap
			${CMAKE_CURRENT_SOURCE_DIR}/mkp/viproc
			${CMAKE_CURRENT_SOURCE_DIR}/mkp/ctrl
			${CMAKE_CURRENT_SOURCE_DIR}/mkp/dfx
			${CMAKE_CURRENT_SOURCE_DIR}/mkp/context
			
			${CMAKE_CURRENT_SOURCE_DIR}/intf/${PLATFORM_PATH}
			${CMAKE_CURRENT_SOURCE_DIR}/extern
			${CMAKE_CURRENT_SOURCE_DIR}/extern/chip
			${CMAKE_CURRENT_SOURCE_DIR}/extern/chip/${VIU_CHIP_NAME}
			
			${CMAKE_CURRENT_SOURCE_DIR}/common
			
			${CMAKE_CURRENT_SOURCE_DIR}/hal
			${CMAKE_CURRENT_SOURCE_DIR}/hal/chip/${CHIP_NAME}
			${CMAKE_CURRENT_SOURCE_DIR}/hal/chip/${CHIP_NAME}/policy

			${CMAKE_CURRENT_SOURCE_DIR}/hal/ip/${VERSION_HAL_IP}
			${CMAKE_CURRENT_SOURCE_DIR}/hal/ip/${VERSION_HAL_IP}/chn
			${CMAKE_CURRENT_SOURCE_DIR}/hal/ip/${VERSION_HAL_IP}/crg
			${CMAKE_CURRENT_SOURCE_DIR}/hal/ip/${VERSION_HAL_IP}/global
			${CMAKE_CURRENT_SOURCE_DIR}/hal/ip/${VERSION_HAL_IP}/pt
			${CMAKE_CURRENT_SOURCE_DIR}/hal/ip/${VERSION_HAL_IP}/reg
	)
else()
    
endif()

set(MCP_SRC_LIST "${MCP_SRC_LIST}" "${viu_drv_list}" CACHE INTERNAL "platform src obj list" FORCE)


set(api_header_include_list
	${ROOT_DIR}/middleware/utils/common_headers/native
	${ROOT_DIR}/middleware/utils/common_headers/media
	${ROOT_DIR}/middleware/utils/common_headers/mcp/internal/base
	${ROOT_DIR}/middleware/utils/common_headers/mcp/internal/base/hal/${VIU_CHIP_NAME}
	${ROOT_DIR}/middleware/utils/common_headers/mcp/internal/dis
	${ROOT_DIR}/middleware/utils/common_headers/mcp/internal/gdc
	${ROOT_DIR}/middleware/utils/common_headers/mcp/internal/isp
	${ROOT_DIR}/middleware/utils/common_headers/mcp/internal/viu
	${ROOT_DIR}/middleware/utils/common_headers/mcp/external/viu
	${ROOT_DIR}/middleware/utils/common_headers/mcp/external/base
)

set(MCP_INC_LIST "${MCP_INC_LIST}" "${viu_drv_header_list}" "${api_header_include_list}" CACHE INTERNAL "platform src obj list" FORCE)


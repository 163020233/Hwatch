#[[
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
# Description: CMake entries of sub-directories.
# Create: 2025-03-04
# ]]

if (DEFINES MATCHES "HI3322_CHIP_V100" OR DEFINES MATCHES "HI3322_PRODUCT_FPGA" )
    set(MCP_CHIP_NAME "3322")
	add_subdirectory(viu)
	add_subdirectory(base)

	set(MCP_SRC_LIST "${MCP_SRC_LIST}" CACHE INTERNAL "platform src obj list" FORCE)

	set(
		mcp_drv_header_list
			${CMAKE_CURRENT_SOURCE_DIR}/include
			${CMAKE_CURRENT_SOURCE_DIR}/include/chip/${MCP_CHIP_NAME}
	)
	set(MCP_INC_LIST "${MCP_INC_LIST}" "${mcp_drv_header_list}" CACHE INTERNAL "platform src obj list" FORCE)	
endif()

if (DEFINES MATCHES "SW22_CHIP_V100" OR DEFINES MATCHES "SW22_PRODUCT_FPGA")
    set(MCP_CHIP_NAME "1A22")

	if (((${MCP_WORK_DEPLOYING} STREQUAL "ccpu") AND (${CORE} STREQUAL "acore")) OR ((${MCP_WORK_DEPLOYING} STREQUAL "mcpu") AND (${CORE} STREQUAL "ccore")))
		## compile nothing
	endif()

	if (((${MCP_WORK_DEPLOYING} STREQUAL "ccpu") AND (${CORE} STREQUAL "ccore")) OR ((${MCP_WORK_DEPLOYING} STREQUAL "mcpu") AND (${CORE} STREQUAL "acore")))
		add_subdirectory(isp)
		add_subdirectory(mipi_rx)
		add_subdirectory(ppu)
		add_subdirectory(vgs)
		add_subdirectory(viu)
		add_subdirectory(base)
		add_subdirectory(fsd)		
		add_subdirectory(dis/gmedis)
		add_subdirectory(dis/gyrodis)
		
		set(
			mcp_drv_list
				${CMAKE_CURRENT_SOURCE_DIR}/drv_mcp_intf.c
		)
		set(MCP_SRC_LIST "${MCP_SRC_LIST}" "${mcp_drv_list}" CACHE INTERNAL "platform src obj list" FORCE)

		set(
			mcp_drv_header_list
				${CMAKE_CURRENT_SOURCE_DIR}/include
				${CMAKE_CURRENT_SOURCE_DIR}/include/chip/${MCP_CHIP_NAME}
		)
		set(MCP_INC_LIST "${MCP_INC_LIST}" "${mcp_drv_header_list}" CACHE INTERNAL "platform src obj list" FORCE)			
	endif()	
endif()

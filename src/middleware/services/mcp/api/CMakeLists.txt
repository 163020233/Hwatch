#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================

if (DEFINES MATCHES "HI3322_CHIP_V100" OR DEFINES MATCHES "HI3322_PRODUCT_FPGA" )
    add_subdirectory(viu)
	add_subdirectory(cis)
endif()

if (DEFINES MATCHES "SW22_CHIP_V100" OR DEFINES MATCHES "SW22_PRODUCT_FPGA")
	if ((${MCP_WORK_DEPLOYING} STREQUAL "ccpu") AND (${CORE} STREQUAL "acore"))
		add_subdirectory(ipc_adapt)
		add_subdirectory(pipeline)
	endif()

	if ((${MCP_WORK_DEPLOYING} STREQUAL "ccpu") AND (${CORE} STREQUAL "ccore"))
		add_subdirectory(ipc_adapt)
		add_subdirectory(pipeline)
		add_subdirectory(viu)
		add_subdirectory(base)
		add_subdirectory(cis)
		#add_subdirectory(gdc)  # 1A22 not support single gdc
		add_subdirectory(isp)
		add_subdirectory(mipi_rx)
		add_subdirectory(ppu)
		add_subdirectory(fsd)		
	endif()	

	if ((${MCP_WORK_DEPLOYING} STREQUAL "mcpu") AND (${CORE} STREQUAL "acore"))
		add_subdirectory(viu)
		add_subdirectory(base)
		add_subdirectory(cis)
		#add_subdirectory(gdc)  # 1A22 not support single gdc
		add_subdirectory(isp)
		add_subdirectory(mipi_rx)
		add_subdirectory(ppu)
		add_subdirectory(fsd)
		add_subdirectory(pipeline)
	endif()
	
	if ((${MCP_WORK_DEPLOYING} STREQUAL "mcpu") AND (${CORE} STREQUAL "ccore"))
		add_subdirectory(ipc_adapt)
		add_subdirectory(pipeline)
	endif()	
endif()

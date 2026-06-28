#[[
Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2022-2022. All rights reserved.
Description: CMake construct system.
Author: Media Software Group
Create: 2021-05-18
]]

if("${OS_TYPE}" STREQUAL "Linux")
	set(TARGET_OS Linux)
	set(CMAKE_C_COMPILER /usr/bin/gcc)
	set(CMAKE_CXX_COMPILER /usr/bin/g++)
	if("${TEST_TYPE}" STREQUAL "ut_test")
		set(SECUREC_INCLUDE ${PROJECT_ROOT_DIR}/test/common/libs_ut/libc_sec/include)
	else()
		set(SECUREC_INCLUDE ${PROJECT_ROOT_DIR}/test/common/libs_fuzz/libc_sec/include)
	endif()
endif()

# HAL_RELEASE_PATH
set(HAL_RELEASE_PATH hal_release/${OS})

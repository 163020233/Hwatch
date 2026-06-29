#[[
Copyright (c) @HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
Description: CMake entries of sub-directories.
Author: Media Software Group
Create: 2022-05-09
]]
if (DEFINES MATCHES "__LITEOS__")
    set(CDEFINES ${DEFINES} "__LITEOS_M__")
    set(CCDEFINES ${DEFINES} "__LITEOS_M__")
endif()

if(${CHIP} MATCHES "brandy|3322")
    add_subdirectory(utils)
    add_subdirectory(audiomanager manager)
    add_subdirectory(player player)
    add_subdirectory(audio_capturer capturer)
    add_subdirectory(service)
elseif(${CHIP} MATCHES "socmn1|socmn2")
    set(CDEFINES ${DEFINES} "ONLY_SUPPORT_HSO")
    set(CCDEFINES ${DEFINES} "ONLY_SUPPORT_HSO")
    add_subdirectory(utils)
    add_subdirectory(audiomanager manager)
    add_subdirectory_if_exist(audio_recorder rec)
    add_subdirectory_if_exist(tone_player)
    add_subdirectory_if_exist(lite_player)
if("media_at_test" IN_LIST RAM_COMPONENT)
    add_subdirectory(test)
endif()
endif()
if(NOT ${CHIP} MATCHES "socmn1|socmn2")
    add_subdirectory(sample)
endif()
if ((NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/vau/src/vau_rm_api.c) AND EXISTS ${ROOT_DIR}/build/config/target_config/${CHIP}/cmake/gpu.cmake)
    include(${ROOT_DIR}/build/config/target_config/${CHIP}/cmake/gpu.cmake)
else()

set(COMPONENT_NAME "gfx_rm")

if (PUBLIC_DEFINES MATCHES "GPU_SRE_SUPPORT")
    add_subdirectory_if_exist(sre)
endif()

add_subdirectory_if_exist(vau)

set(SOURCES
   ${GFX_RM_SRC_LIST}
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
   ${GFX_RM_INC_LIST}
)

list(APPEND PRIVATE_DEFINES "CONFIG_VG_RUN_LINUX")

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(LIBRARY_OUTPUT_PATH ${BIN_DIR}/${CHIP}/libs/gpu/${TARGET_COMMAND})
build_component()

endif()
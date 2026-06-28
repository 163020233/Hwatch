STRING( REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} )
set(MODULE_NAME ${CURRENT_FOLDER})

if (CONFIG_OS_OPTION_FS_LITEVFS)
    set(LITEOS_LITTLEFS_PATH  ${OPENSOURCEDIR}/littlefs)
else()
    set(LITEOS_LITTLEFS_PATH  ${OPENSOURCEDIR}/littlefs)
endif()
# add global include, compile macro and options to public target #
set(MODULE_INCLUDE_PUB)
set(MODULE_CXXINCLUDE_PUB)
set(MODULE_COPTS_PUB)
set(MODULE_ASOPTS_PUB)
set(MODULE_CXXOPTS_PUB)
set(MODULE_CMACRO_PUB)
set(MODULE_ASMACRO_PUB)
set(MODULE_CXXMACRO_PUB)

# add local include, compile macro and options to private target #
set(MODULE_INCLUDE_PRI
    ${LITEOSTOPDIR}/fs/littlefs/include
    ${LITEOS_LITTLEFS_PATH}
)

set(MODULE_COPTS_PRI)
set(MODULE_ASOPTS_PRI)
set(MODULE_CXXOPTS_PRI)
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

# The open-source code of littlefs contains self-assignment statements. #
if(LOSCFG_COMPILER_RISCV_CLANG_MUSL)
list(APPEND MODULE_COPTS_PRI -Wno-self-assign)
endif()
list(APPEND MODULE_CMACRO_PRI LFS_NO_ASSERT)
list(APPEND MODULE_CMACRO_PRI LFS_THREADSAFE)

# add srcs to private target #
set(LOCAL_SRCS_y)
set(LOCAL_DIRS_y os_adapt
   ${LITEOS_LITTLEFS_PATH}
)
FOREACH(CUR_DIR ${LOCAL_DIRS_y})
    aux_source_directory(${CUR_DIR} LOCAL_SRCS_y)
ENDFOREACH(CUR_DIR)

if (CONFIG_OS_OPTION_FS_LITEVFS)
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ../../../self_src/fs/litevfs/include
        ../../../self_src/osal/kal/include
        ${OPENSOURCEDIR}/littlefs
        ../../../self_src/include/uapi/posix)
    list(REMOVE_ITEM LOCAL_SRCS_y os_adapt/vfs_littlefs.c)
    list(APPEND MODULE_COPTS_PRI -Wno-cast-qual -Wno-shadow)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-cast-qual -Wno-shadow") # ck complie option
else()
    list(REMOVE_ITEM LOCAL_SRCS_y os_adapt/litevfs_littlefs.c)
endif()

set(LOCAL_SRCS ${LOCAL_SRCS_y})  # module.cmake will sort LOCAL_SRCS files
include(${MODULE})

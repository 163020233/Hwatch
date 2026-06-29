STRING( REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} )
set(MODULE_NAME ${CURRENT_FOLDER})

set(LITEOS_FATFS_PATH ${LITEOS_OPENSOURCE_PATHNAME}/FatFs)

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
    ${LITEOSTOPDIR}/fs/fat/virpart/include
    ${LITEOSTOPDIR}/fs/fat/dcache/include
    ${LITEOSTOPDIR}/fs/vfs/include/disk
    ${LITEOSTOPDIR}/fs/vfs/include/utrim
    ${LITEOS_FATFS_PATH}/source
)

set(MODULE_COPTS_PRI)
set(MODULE_ASOPTS_PRI)
set(MODULE_CXXOPTS_PRI)
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

list(APPEND MODULE_COPTS_PRI -fno-strict-aliasing)
list(APPEND MODULE_CXXOPTS_PRI -fno-strict-aliasing)

list(APPEND MODULE_CMACRO_PRI FATFS_EXTEND_CONFIG CONFIG_PRIVATE_DIRENT_STRUCT)

# add srcs to private target #
set(LOCAL_SRCS_y)

list(APPEND LOCAL_SRCS_y ${LITEOS_FATFS_PATH}/source/ff.c)
list(APPEND LOCAL_SRCS_y ${LITEOS_FATFS_PATH}/source/ffunicode.c)

set(LOCAL_DIRS_y os_adapt src)

list(APPEND LOCAL_DIRS_${LOSCFG_FS_FAT_DCACHE} dcache/src)

FOREACH(CUR_DIR ${LOCAL_DIRS_y})
    aux_source_directory(${CUR_DIR} LOCAL_SRCS_y)
ENDFOREACH(CUR_DIR)

set(LOCAL_SRCS ${LOCAL_SRCS_y})  # module.cmake will sort LOCAL_SRCS files
include(${MODULE})

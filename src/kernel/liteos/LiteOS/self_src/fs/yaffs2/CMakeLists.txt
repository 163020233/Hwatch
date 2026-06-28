# Derive module name from current folder
STRING( REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} )
set(MODULE_NAME ${CURRENT_FOLDER})

# Public include, compile macro and options (empty by default)
set(MODULE_INCLUDE_PUB)
set(MODULE_CXXINCLUDE_PUB)
set(MODULE_COPTS_PUB)
set(MODULE_ASOPTS_PUB)
set(MODULE_CXXOPTS_PUB)
set(MODULE_CMACRO_PUB)
set(MODULE_ASMACRO_PUB)
set(MODULE_CXXMACRO_PUB)

# Private include paths: match Open Center repo layout under LiteOS fs/yaffs2
set(MODULE_INCLUDE_PRI
    ${LITEOSTOPDIR}/fs/yaffs2
    ${LITEOSTOPDIR}/fs/yaffs2/direct
    ${LITEOSTOPDIR}/fs/yaffs2/os_adapt/adapt_liteos
)

set(MODULE_COPTS_PRI)
set(MODULE_ASOPTS_PRI)
set(MODULE_CXXOPTS_PRI)
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

# Toolchain-specific warning suppressions (keep consistent with LiteOS)
if(LOSCFG_COMPILER_RISCV_CLANG_UNKNOWN OR LOSCFG_COMPILER_RISCV_CLANG_MUSL)
    list(APPEND MODULE_COPTS_PRI -Wno-unused-variable -Wno-unused-but-set-variable -Wno-null-dereference -Wno-self-assign -Wno-strncat-size)
endif()

# Only compile the LiteOS-selected files from the Open Center repo

# os_adapt (LiteOS VFS glue and NAND cfg)
set(OS_ADAPT_SRCS
    os_adapt/mtd_nandcfg.c
    os_adapt/vfs_yaffs2.c
    os_adapt/yaffs_nandcfg.c
    os_adapt/yaffs_osglue.c
)

# YAFFS core (guts and helpers) - at repository root
set(YAFFS_CORE_SRCS
    yaffs_allocator.c
    yaffs_bitmap.c
    yaffs_cache.c
    yaffs_checkptrw.c
    yaffs_ecc.c
    yaffs_endian.c
    yaffs_guts.c
    yaffs_nameval.c
    yaffs_nand.c
    yaffs_packedtags1.c
    yaffs_packedtags2.c
    yaffs_summary.c
    yaffs_tagscompat.c
    yaffs_tagsmarshall.c
    yaffs_verify.c
    yaffs_yaffs1.c
    yaffs_yaffs2.c
)

# direct layer used by LiteOS - upstream paths
set(YAFFS_DIRECT_SRCS
    direct/yaffs_attribs.c
    direct/yaffsfs.c
    direct/yaffs_hweight.c
    direct/yaffs_nand_drv.c
)

# Final source list for the module
set(LOCAL_SRCS
    ${OS_ADAPT_SRCS}
    ${YAFFS_CORE_SRCS}
    ${YAFFS_DIRECT_SRCS}
)

# module.cmake will sort LOCAL_SRCS and generate targets
include(${MODULE})

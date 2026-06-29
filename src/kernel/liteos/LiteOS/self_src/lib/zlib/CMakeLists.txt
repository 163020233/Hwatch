set(MODULE_NAME z)

set(ZLIB_PATH ${LITEOS_OPENSOURCE_PATHNAME}/zlib)

# add global include, compile macro and options to public target #
set(MODULE_INCLUDE_PUB)
set(MODULE_CXXINCLUDE_PUB)
set(MODULE_COPTS_PUB)
set(MODULE_ASOPTS_PUB)
set(MODULE_CXXOPTS_PUB)
set(MODULE_CMACRO_PUB Z_HAVE_UNISTD_H)
set(MODULE_ASMACRO_PUB)
set(MODULE_CXXMACRO_PUB)

# add local include, compile macro and options to private target #
set(MODULE_INCLUDE_PRI)
set(MODULE_COPTS_PRI)
set(MODULE_ASOPTS_PRI)
set(MODULE_CXXOPTS_PRI)
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

set(LOCAL_SRCS_y
    adler32.c compress.c crc32.c deflate.c gzclose.c gzlib.c
    gzread.c gzwrite.c infback.c inffast.c inflate.c inftrees.c
    trees.c uncompr.c zutil.c
)

list(TRANSFORM LOCAL_SRCS_y PREPEND ${ZLIB_PATH}/)

set(LOCAL_SRCS ${LOCAL_SRCS_y})  # module.cmake will sort LOCAL_SRCS files
include(${MODULE})

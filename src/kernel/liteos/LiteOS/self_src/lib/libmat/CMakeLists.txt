set(MODULE_NAME mat)


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
# set(MODULE_INCLUDE_PRI
#     .
# )


set(MODULE_COPTS_PRI)
set(MODULE_ASOPTS_PRI)
set(MODULE_CXXOPTS_PRI)
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

set(LOCAL_SRCS_y
    matrix_inv.c
    matrix_mul.c
)

set(LOCAL_SRCS ${LOCAL_SRCS_y})

include(${MODULE})

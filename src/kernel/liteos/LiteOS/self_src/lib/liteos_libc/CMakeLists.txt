if (COMPILE_CK_POSIX)
set(LOCAL_DIRS_RTOS_LIBC
    pthread
    pthread/nonisolated
    pthread/wrap_rtos
    stdlib
    time/nonisolated
)
set(LOCAL_SRCS_LIBC
    pthread/pthread_attr.c
    pthread/nonisolated/pthread_mutex.c
    pthread/nonisolated/pthread_cond.c
    pthread/nonisolated/semaphore.c
    pthread/wrap_rtos/pthread_key.c
    pthread/wrap_rtos/pthread_exit.c
    pthread/wrap_rtos/pthread_cancel.c
    pthread/wrap_rtos/pthread_create.c
    pthread/wrap_rtos/pthread_detach.c
    pthread/wrap_rtos/pthread_join.c
    pthread/wrap_rtos/pthread_rwlock.c
    stdlib/cstdlib.c
    time/nonisolated/time.c
)

foreach(FILE_NAME ${LOCAL_SRCS_LIBC})
    add_library_ex(${FILE_NAME})
endforeach()
else() # LITEOS_POSIX
include(../libc/libc_src.cmake)

set(MODULE_NAME posix)

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
    ${LITEOSTOPDIR}/compat
    ${LITEOSTOPDIR}/lib/liteos_libc/pthread
    ${LITEOSTOPDIR}/lib/liteos_libc/mq
    ${LITEOSTOPDIR}/lib/liteos_libc/errno
    ${LITEOSTOPDIR}/lib/liteos_libc/tzdst
    ${LITEOSTOPDIR}/lib/liteos_libc/time
    ${LITEOSTOPDIR}/lib/liteos_libc/lock
    ${LITEOSTOPDIR}/lib/liteos_libc/init
    ${LITEOS_LIBC_PATH}/src/include
    ${LITEOS_LIBC_PATH}/src/internal
    ${LITEOS_LIBC_PATH}/src/time
)
set(MODULE_COPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_ASOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CXXOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

list(REMOVE_ITEM MODULE_COPTS_PRI -Wshadow)

# add srcs to private target #
set(LOCAL_SRCS_y)
set(LOCAL_DIRS_y)

# liteos_libc path
set(LOCAL_DIRS_LITEOS_LIBC sched misc env)
list(APPEND LOCAL_DIRS_LITEOS_LIBC mq/nonisolated ldso pthread/nonisolated)

# musl_libc path
set(LOCAL_SRCS_LIBC
    ${LIBC_PRNG_SRC} ${LIBC_LINUX_SRC} ${LIBC_THREAD_SRC}
)

# posix configured module path
list(APPEND LOCAL_DIRS_${LOSCFG_COMPAT_POSIX} ${LOCAL_DIRS_LITEOS_LIBC})
list(APPEND LOCAL_SRCS_${LOSCFG_COMPAT_POSIX} time/timer.c pthread/pthread_attr.c)
list(APPEND LOCAL_SRCS_${LOSCFG_COMPAT_POSIX} ${LOCAL_SRCS_LIBC})

FOREACH(CUR_DIR ${LOCAL_DIRS_y})
    aux_source_directory(${CUR_DIR} LOCAL_SRCS_y)
ENDFOREACH(CUR_DIR)

list(REMOVE_ITEM LOCAL_SRCS_y misc/getentropy.c)

list(APPEND MODULE_COPTS_PRI -Wno-parentheses -Wno-strict-prototypes)
if (LOSCFG_COMPILER_GCC)
    if (${VERSION_NUM} VERSION_GREATER_EQUAL 13.0.0)
        list(APPEND MODULE_COPTS_PRI -Wno-array-parameter)
    endif()
endif()

if (LOSCFG_KERNEL_CPPSUPPORT AND NOT LOSCFG_COMPAT_POSIX)
list(APPEND LOCAL_SRCS_y misc/stub.c)
endif()

set(LOCAL_SRCS ${LOCAL_SRCS_y})  # module.cmake will sort LOCAL_SRCS files
include(${MODULE_LIBC})
endif()

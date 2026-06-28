# LMS options
if(LOSCFG_COMPILER_RISCV_CLANG_MUSL)
    set(LMS_COPTS)
    set(LMS_CMACRO __SANITIZE_ADDRESS__)
else()
    set(LMS_COPTS -fsanitize=kernel-address)
    set(LMS_CMACRO)
    if(LOSCFG_COMPILER_CLANG)
        list(APPEND LMS_COPTS
            -mllvm -asan-instrumentation-with-call-threshold=0      # no inline check
            -mllvm -asan-globals=0                                  # no check global variables
            -mllvm -asan-stack=0                                    # no check stack
        )
        list(APPEND LMS_CMACRO __SANITIZE_ADDRESS__)
    endif()
endif()

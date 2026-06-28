if (COMMAND build_component)
# build component
    if (HOST)
        list(APPEND PUBLIC_DEFINES __GNUC_PYTHON__)
    endif()

    if (CONFIGTABLE)
        list(APPEND PUBLIC_DEFINES ARM_DSP_CONFIG_TABLES)
    endif()

    if (LOOPUNROLL)
        list(APPEND PRIVATE_DEFINES ARM_MATH_LOOPUNROLL)
    endif()

    if (ROUNDING)
        list(APPEND PRIVATE_DEFINES ARM_MATH_ROUNDING)
    endif()

    if (MATRIXCHECK)
        list(APPEND PRIVATE_DEFINES ARM_MATH_MATRIX_CHECK)
    endif()

    if (AUTOVECTORIZE)
        list(APPEND PRIVATE_DEFINES ARM_MATH_AUTOVECTORIZE)
    endif()

    if (NEON OR NEONEXPERIMENTAL)
        # Used in arm_vec_math.h - include directories would be handled at higher level
    endif()

    if (MVEFLOAT16)
        list(APPEND PRIVATE_DEFINES ARM_MATH_MVE_FLOAT16)
    endif()

    if (HELIUM OR MVEF OR SUPPORT)
        # Include directories would be handled at higher level
    endif()

    if (DISABLEFLOAT16)
        list(APPEND PRIVATE_DEFINES DISABLEFLOAT16)
    endif()

    if (RISCVV)
        list(APPEND PRIVATE_DEFINES RISCV_MATH_VECTOR_EXT)
    endif()
else()
function(configDsp project root)
# build lib
    if (HOST)
        target_compile_definitions(${project} PUBLIC __GNUC_PYTHON__)
    endif()

    if (CONFIGTABLE)
        # Public because initialization for FFT may be defined in client code 
        # and needs access to the table.
        target_compile_definitions(${project} PUBLIC ARM_DSP_CONFIG_TABLES)
    endif()

    if (LOOPUNROLL)
        target_compile_definitions(${project} PRIVATE ARM_MATH_LOOPUNROLL)
    endif()

    if (ROUNDING)
        target_compile_definitions(${project} PRIVATE ARM_MATH_ROUNDING)
    endif()

    if (MATRIXCHECK)
        target_compile_definitions(${project} PRIVATE ARM_MATH_MATRIX_CHECK)
    endif()

    if (AUTOVECTORIZE)
        target_compile_definitions(${project} PRIVATE ARM_MATH_AUTOVECTORIZE) 
    endif()

    if (NEON OR NEONEXPERIMENTAL)
        # Used in arm_vec_math.h
        target_include_directories(${project} PUBLIC "${root}/ComputeLibrary/Include")
    endif()

    if (MVEFLOAT16)
        target_compile_definitions(${project} PRIVATE ARM_MATH_MVE_FLOAT16) 
    endif()

    if (HELIUM OR MVEF OR SUPPORT)
        target_include_directories(${project} PRIVATE "${root}/PrivateInclude")
    endif()

    if (DISABLEFLOAT16)
        target_compile_definitions(${project} PRIVATE DISABLEFLOAT16) 
    endif()

    if (RISCVV)
        # RISC-V Vector Extensions Supported
        target_compile_definitions(${project} PRIVATE RISCV_MATH_VECTOR_EXT) 
    endif()
endfunction()
endif()
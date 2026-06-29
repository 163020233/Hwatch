set(SRCCOMMON 
${DSP}/Source/CommonTables/arm_common_tables.c
${DSP}/Source/CommonTables/arm_common_tables_f16.c
${SOURCES_RVV_PATH}/CommonTables/arm_const_structs.c
${DSP}/Source/CommonTables/arm_const_structs_f16.c
)

set(SRCCLTABLES 
${DSP}/ComputeLibrary/Source/arm_cl_tables.c
)

set(SRCMVETABLES 
${SOURCES_RVV_PATH}/CommonTables/arm_mve_tables.c
${DSP}/Source/CommonTables/arm_mve_tables_f16.c
)

# build component
list(APPEND SOURCES ${SRCCOMMON})

if (NEON OR NEONEXPERIMENTAL)
    list(APPEND SOURCES ${SRCCLTABLES})
endif()

if (HELIUM OR MVEF OR RISCVV)
    list(APPEND SOURCES ${SRCMVETABLES})
endif()

if (RISCVV)
list(APPEND SOURCES 
    ${SOURCES_RVV_PATH}/CommonTables/rvv_const_data.c
    ${SOURCES_RVV_PATH}/CommonTables/rearranged_twiddle.c
)
endif()

if (CONFIGTABLE AND ALLFFT)
    list(APPEND PUBLIC_DEFINES ARM_ALL_FFT_TABLES)
endif()

if (CONFIGTABLE AND ALLFAST)
    list(APPEND PUBLIC_DEFINES ARM_ALL_FAST_TABLES)
endif()

if (WRAPPER)
    list(APPEND PUBLIC_DEFINES
        ARM_TABLE_BITREV_1024
        ARM_TABLE_TWIDDLECOEF_F32_4096
        ARM_TABLE_TWIDDLECOEF_Q31_4096
        ARM_TABLE_TWIDDLECOEF_Q15_4096
    )
    if ((NOT ARMAC5) AND (NOT DISABLEFLOAT16))
        list(APPEND PUBLIC_DEFINES ARM_TABLE_TWIDDLECOEF_F16_4096)
    endif()
endif()

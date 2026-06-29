set(SRCCOMPLEXF64 
${DSP}/Source/ComplexMathFunctions/arm_cmplx_mag_f64.c
${DSP}/Source/ComplexMathFunctions/arm_cmplx_mag_squared_f64.c
${DSP}/Source/ComplexMathFunctions/arm_cmplx_mult_cmplx_f64.c
)

set(SRCCOMPLEXF32 
${SOURCES_RVV_PATH}/ComplexMathFunctions/arm_cmplx_conj_f32.c
${SOURCES_RVV_PATH}/ComplexMathFunctions/arm_cmplx_dot_prod_f32.c
${SOURCES_RVV_PATH}/ComplexMathFunctions/arm_cmplx_mag_f32.c
${SOURCES_RVV_PATH}/ComplexMathFunctions/arm_cmplx_mag_squared_f32.c
${SOURCES_RVV_PATH}/ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.c
${SOURCES_RVV_PATH}/ComplexMathFunctions/arm_cmplx_mult_real_f32.c
)

set(SRCCOMPLEXQ31 
${SOURCES_RVV_PATH}/ComplexMathFunctions/arm_cmplx_conj_q31.c
${SOURCES_RVV_PATH}/ComplexMathFunctions/arm_cmplx_dot_prod_q31.c
${SOURCES_RVV_PATH}/ComplexMathFunctions/arm_cmplx_mag_squared_q31.c
${DSP}/Source/ComplexMathFunctions/arm_cmplx_mult_cmplx_q31.c
${SOURCES_RVV_PATH}/ComplexMathFunctions/arm_cmplx_mult_real_q31.c
)

set(SRCCOMPLEXQ15 
${DSP}/Source/ComplexMathFunctions/arm_cmplx_mag_fast_q15.c
${SOURCES_RVV_PATH}/ComplexMathFunctions/arm_cmplx_conj_q15.c
${SOURCES_RVV_PATH}/ComplexMathFunctions/arm_cmplx_dot_prod_q15.c
${SOURCES_RVV_PATH}/ComplexMathFunctions/arm_cmplx_mag_squared_q15.c
${DSP}/Source/ComplexMathFunctions/arm_cmplx_mult_cmplx_q15.c
${SOURCES_RVV_PATH}/ComplexMathFunctions/arm_cmplx_mult_real_q15.c
)

set(SRCCOMPLEXF16 
${DSP}/Source/ComplexMathFunctions/arm_cmplx_conj_f16.c
${DSP}/Source/ComplexMathFunctions/arm_cmplx_dot_prod_f16.c
${DSP}/Source/ComplexMathFunctions/arm_cmplx_mag_f16.c
${DSP}/Source/ComplexMathFunctions/arm_cmplx_mag_squared_f16.c
${DSP}/Source/ComplexMathFunctions/arm_cmplx_mult_cmplx_f16.c
${DSP}/Source/ComplexMathFunctions/arm_cmplx_mult_real_f16.c
)

# build component
if (NOT CONFIGTABLE OR ALLFAST OR ARM_CMPLX_MAG_Q31 OR (NOT HELIUM AND NOT MVEI))
    list(APPEND SOURCES ${SOURCES_RVV_PATH}/ComplexMathFunctions/arm_cmplx_mag_q31.c)
endif()

if (NOT CONFIGTABLE OR ALLFAST OR ARM_CMPLX_MAG_Q15 OR (NOT HELIUM AND NOT MVEI))
    list(APPEND SOURCES ${SOURCES_RVV_PATH}/ComplexMathFunctions/arm_cmplx_mag_q15.c)
endif()

list(APPEND SOURCES ${SRCCOMPLEXF64})
list(APPEND SOURCES ${SRCCOMPLEXF32})
list(APPEND SOURCES ${SRCCOMPLEXQ31})
list(APPEND SOURCES ${SRCCOMPLEXQ15})

if ((NOT ARMAC5) AND (NOT DISABLEFLOAT16))
    list(APPEND SOURCES ${SRCCOMPLEXF16})
endif()

if (CONFIGTABLE AND ALLFAST)
    list(APPEND PUBLIC_DEFINES ARM_ALL_FAST_TABLES)
endif()

if (CONFIGTABLE AND (HELIUM OR MVEF OR MVEI))
    list(APPEND PUBLIC_DEFINES ARM_FAST_ALLOW_TABLES)
endif()
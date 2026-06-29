set(SRC_FASTMATH_F64
${DSP}/Source/FastMathFunctions/arm_vlog_f64.c
${DSP}/Source/FastMathFunctions/arm_vexp_f64.c
)

set(SRC_FASTMATH_F32 
${DSP}/Source/FastMathFunctions/arm_cos_f32.c
${DSP}/Source/FastMathFunctions/arm_sin_f32.c
${DSP}/Source/FastMathFunctions/arm_vlog_f32.c
${DSP}/Source/FastMathFunctions/arm_vexp_f32.c
${DSP}/Source/FastMathFunctions/arm_atan2_f32.c
)

set(SRC_FASTMATH_Q15 
${DSP}/Source/FastMathFunctions/arm_cos_q15.c
${DSP}/Source/FastMathFunctions/arm_sin_q15.c
${DSP}/Source/FastMathFunctions/arm_sqrt_q15.c
${DSP}/Source/FastMathFunctions/arm_vlog_q15.c
${DSP}/Source/FastMathFunctions/arm_divide_q15.c
${DSP}/Source/FastMathFunctions/arm_atan2_q15.c
)

set(SRC_FASTMATH_Q31 
${DSP}/Source/FastMathFunctions/arm_cos_q31.c
${DSP}/Source/FastMathFunctions/arm_sin_q31.c
${DSP}/Source/FastMathFunctions/arm_sqrt_q31.c
${DSP}/Source/FastMathFunctions/arm_vlog_q31.c
${DSP}/Source/FastMathFunctions/arm_divide_q31.c
${DSP}/Source/FastMathFunctions/arm_atan2_q31.c
)

set(SRC_FASTMATH_F16 
${DSP}/Source/FastMathFunctions/arm_vlog_f16.c
${DSP}/Source/FastMathFunctions/arm_vexp_f16.c
${DSP}/Source/FastMathFunctions/arm_vinverse_f16.c
${DSP}/Source/FastMathFunctions/arm_atan2_f16.c
)

# build component
    list(APPEND SOURCES ${SRC_FASTMATH_F64})
if (NOT CONFIGTABLE OR ALLFAST OR ARM_COS_F32 OR ARM_SIN_F32)
    list(APPEND SOURCES ${SRC_FASTMATH_F32})
endif()

if (NOT CONFIGTABLE OR ALLFAST OR ARM_COS_Q15 OR ARM_SIN_Q15)
    list(APPEND SOURCES ${SRC_FASTMATH_Q15})
endif()

if (NOT CONFIGTABLE OR ALLFAST OR ARM_COS_Q31 OR ARM_SIN_Q31)
    list(APPEND SOURCES ${SRC_FASTMATH_Q31})
endif()

if ((NOT ARMAC5) AND (NOT DISABLEFLOAT16))
    list(APPEND SOURCES ${SRC_FASTMATH_F16})
endif()

if (CONFIGTABLE AND ALLFAST)
    list(APPEND PUBLIC_DEFINES ARM_ALL_FAST_TABLES)
endif()
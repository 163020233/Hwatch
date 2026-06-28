set(SRC_INTERPOLATION_F32 
${DSP}/Source/InterpolationFunctions/arm_bilinear_interp_f32.c
${DSP}/Source/InterpolationFunctions/arm_linear_interp_f32.c
${SOURCES_RVV_PATH}/InterpolationFunctions/arm_spline_interp_f32.c
${DSP}/Source/InterpolationFunctions/arm_spline_interp_init_f32.c
)

set(SRC_INTERPOLATION_Q31 
${DSP}/Source/InterpolationFunctions/arm_bilinear_interp_q31.c
${DSP}/Source/InterpolationFunctions/arm_linear_interp_q31.c
)

set(SRC_INTERPOLATION_Q15 
${DSP}/Source/InterpolationFunctions/arm_bilinear_interp_q15.c
${DSP}/Source/InterpolationFunctions/arm_linear_interp_q15.c
)

set(SRC_INTERPOLATION_Q7 
${DSP}/Source/InterpolationFunctions/arm_bilinear_interp_q7.c
${DSP}/Source/InterpolationFunctions/arm_linear_interp_q7.c
)

set(SRC_INTERPOLATION_F16 
${DSP}/Source/InterpolationFunctions/arm_bilinear_interp_f16.c
${DSP}/Source/InterpolationFunctions/arm_linear_interp_f16.c
)

# build component
list(APPEND SOURCES ${SRC_INTERPOLATION_F32})
list(APPEND SOURCES ${SRC_INTERPOLATION_Q31})
list(APPEND SOURCES ${SRC_INTERPOLATION_Q15})
list(APPEND SOURCES ${SRC_INTERPOLATION_Q7})

if ((NOT ARMAC5) AND (NOT DISABLEFLOAT16))
list(APPEND SOURCES ${SRC_INTERPOLATION_F16})
endif()
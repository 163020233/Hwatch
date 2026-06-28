set(SRCF64 
${DSP}/Source/BasicMathFunctions/arm_abs_f64.c
${DSP}/Source/BasicMathFunctions/arm_add_f64.c
${DSP}/Source/BasicMathFunctions/arm_dot_prod_f64.c
${DSP}/Source/BasicMathFunctions/arm_mult_f64.c
${DSP}/Source/BasicMathFunctions/arm_negate_f64.c
${DSP}/Source/BasicMathFunctions/arm_offset_f64.c
${DSP}/Source/BasicMathFunctions/arm_scale_f64.c
${DSP}/Source/BasicMathFunctions/arm_sub_f64.c
)

set(SRCF32 
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_abs_f32.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_add_f32.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_clip_f32.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_dot_prod_f32.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_mult_f32.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_negate_f32.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_offset_f32.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_scale_f32.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_sub_f32.c
)

set(SRCF16 
${DSP}/Source/BasicMathFunctions/arm_abs_f16.c
${DSP}/Source/BasicMathFunctions/arm_add_f16.c
${DSP}/Source/BasicMathFunctions/arm_clip_f16.c
${DSP}/Source/BasicMathFunctions/arm_dot_prod_f16.c
${DSP}/Source/BasicMathFunctions/arm_mult_f16.c
${DSP}/Source/BasicMathFunctions/arm_negate_f16.c
${DSP}/Source/BasicMathFunctions/arm_offset_f16.c
${DSP}/Source/BasicMathFunctions/arm_scale_f16.c
${DSP}/Source/BasicMathFunctions/arm_sub_f16.c
)

set(SRCQ31 
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_abs_q31.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_add_q31.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_clip_q31.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_dot_prod_q31.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_mult_q31.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_negate_q31.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_offset_q31.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_scale_q31.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_shift_q31.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_sub_q31.c
)

set(SRCQ15 
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_abs_q15.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_add_q15.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_clip_q15.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_dot_prod_q15.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_mult_q15.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_negate_q15.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_offset_q15.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_scale_q15.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_shift_q15.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_sub_q15.c
)

set(SRCQ7 
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_abs_q7.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_add_q7.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_clip_q7.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_dot_prod_q7.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_mult_q7.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_negate_q7.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_offset_q7.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_scale_q7.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_shift_q7.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_sub_q7.c
)

set(SRCU32 
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_and_u32.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_not_u32.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_or_u32.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_xor_u32.c
)

set(SRCU16 
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_and_u16.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_not_u16.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_or_u16.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_xor_u16.c
)

set(SRCU8  
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_and_u8.c  
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_or_u8.c
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_not_u8.c  
${SOURCES_RVV_PATH}/BasicMathFunctions/arm_xor_u8.c)

list(APPEND SOURCES ${SRCF64})
list(APPEND SOURCES ${SRCF32})

if ((NOT ARMAC5) AND (NOT DISABLEFLOAT16))
list(APPEND SOURCES ${SRCF16})
endif()

list(APPEND SOURCES ${SRCQ31})
list(APPEND SOURCES ${SRCQ15})
list(APPEND SOURCES ${SRCQ7})
list(APPEND SOURCES ${SRCU32})
list(APPEND SOURCES ${SRCU16})
list(APPEND SOURCES ${SRCU8})





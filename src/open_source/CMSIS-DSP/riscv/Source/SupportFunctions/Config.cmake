set(SRC_SUPPORT_F64 
${DSP}/Source/SupportFunctions/arm_copy_f64.c
${DSP}/Source/SupportFunctions/arm_fill_f64.c
${DSP}/Source/SupportFunctions/arm_f64_to_float.c
${DSP}/Source/SupportFunctions/arm_f64_to_q31.c
${DSP}/Source/SupportFunctions/arm_f64_to_q15.c
${DSP}/Source/SupportFunctions/arm_f64_to_q7.c
${DSP}/Source/SupportFunctions/arm_float_to_f64.c
${DSP}/Source/SupportFunctions/arm_q15_to_f64.c
${DSP}/Source/SupportFunctions/arm_q31_to_f64.c
${DSP}/Source/SupportFunctions/arm_q7_to_f64.c
)

set(SRC_SUPPORT_F32 
${SOURCES_RVV_PATH}/SupportFunctions/arm_copy_f32.c
${SOURCES_RVV_PATH}/SupportFunctions/arm_fill_f32.c
${SOURCES_RVV_PATH}/SupportFunctions/arm_weighted_average_f32.c
${SOURCES_RVV_PATH}/SupportFunctions/arm_barycenter_f32.c
${DSP}/Source/SupportFunctions/arm_bitonic_sort_f32.c
${DSP}/Source/SupportFunctions/arm_bubble_sort_f32.c
${DSP}/Source/SupportFunctions/arm_heap_sort_f32.c
${DSP}/Source/SupportFunctions/arm_insertion_sort_f32.c
${DSP}/Source/SupportFunctions/arm_merge_sort_f32.c
${DSP}/Source/SupportFunctions/arm_merge_sort_init_f32.c
${DSP}/Source/SupportFunctions/arm_quick_sort_f32.c
${DSP}/Source/SupportFunctions/arm_selection_sort_f32.c
${DSP}/Source/SupportFunctions/arm_sort_f32.c
${DSP}/Source/SupportFunctions/arm_sort_init_f32.c
${DSP}/Source/SupportFunctions/arm_f64_to_float.c
${DSP}/Source/SupportFunctions/arm_float_to_f64.c
)

set(SRC_SUPPORT_Q31 
${SOURCES_RVV_PATH}/SupportFunctions/arm_copy_q31.c
${SOURCES_RVV_PATH}/SupportFunctions/arm_fill_q31.c
${SOURCES_RVV_PATH}/SupportFunctions/arm_q31_to_float.c
${SOURCES_RVV_PATH}/SupportFunctions/arm_q31_to_q15.c
${SOURCES_RVV_PATH}/SupportFunctions/arm_q31_to_q7.c
${SOURCES_RVV_PATH}/SupportFunctions/arm_float_to_q31.c
)

set(SRC_SUPPORT_Q15 
${SOURCES_RVV_PATH}/SupportFunctions/arm_copy_q15.c
${SOURCES_RVV_PATH}/SupportFunctions/arm_fill_q15.c
${DSP}/Source/SupportFunctions/arm_q15_to_float.c
${SOURCES_RVV_PATH}/SupportFunctions/arm_q15_to_q31.c
${SOURCES_RVV_PATH}/SupportFunctions/arm_q15_to_q7.c
${SOURCES_RVV_PATH}/SupportFunctions/arm_float_to_q15.c
)

set(SRC_SUPPORT_Q7 
${SOURCES_RVV_PATH}/SupportFunctions/arm_copy_q7.c
${SOURCES_RVV_PATH}/SupportFunctions/arm_fill_q7.c
${DSP}/Source/SupportFunctions/arm_q7_to_float.c
${SOURCES_RVV_PATH}/SupportFunctions/arm_q7_to_q15.c
${SOURCES_RVV_PATH}/SupportFunctions/arm_q7_to_q31.c
${SOURCES_RVV_PATH}/SupportFunctions/arm_float_to_q7.c
)

set(SRC_SUPPORT_F16 
${DSP}/Source/SupportFunctions/arm_copy_f16.c
${DSP}/Source/SupportFunctions/arm_fill_f16.c
${DSP}/Source/SupportFunctions/arm_f16_to_q15.c
${DSP}/Source/SupportFunctions/arm_q15_to_f16.c
${DSP}/Source/SupportFunctions/arm_float_to_f16.c
${DSP}/Source/SupportFunctions/arm_f16_to_float.c
${DSP}/Source/SupportFunctions/arm_weighted_average_f16.c
${DSP}/Source/SupportFunctions/arm_barycenter_f16.c
${DSP}/Source/SupportFunctions/arm_f16_to_f64.c
${DSP}/Source/SupportFunctions/arm_f64_to_f16.c
)

# build component
list(APPEND SOURCES ${SRC_SUPPORT_F64})
list(APPEND SOURCES ${SRC_SUPPORT_F32})
list(APPEND SOURCES ${SRC_SUPPORT_Q31})
list(APPEND SOURCES ${SRC_SUPPORT_Q15})
list(APPEND SOURCES ${SRC_SUPPORT_Q7})

if ((NOT ARMAC5) AND (NOT DISABLEFLOAT16))
list(APPEND SOURCES ${SRC_SUPPORT_F16})
endif()
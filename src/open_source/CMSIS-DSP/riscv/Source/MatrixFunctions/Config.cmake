set(SRCF64 
${DSP}/Source/MatrixFunctions/arm_mat_inverse_f64.c
${DSP}/Source/MatrixFunctions/arm_mat_mult_f64.c
${DSP}/Source/MatrixFunctions/arm_mat_sub_f64.c
${DSP}/Source/MatrixFunctions/arm_mat_trans_f64.c
${DSP}/Source/MatrixFunctions/arm_mat_qr_f64.c
${DSP}/Source/MatrixFunctions/arm_mat_init_f64.c
${DSP}/Source/MatrixFunctions/arm_mat_solve_upper_triangular_f64.c
${DSP}/Source/MatrixFunctions/arm_mat_cholesky_f64.c
${DSP}/Source/MatrixFunctions/arm_mat_solve_lower_triangular_f64.c
${DSP}/Source/MatrixFunctions/arm_mat_ldlt_f64.c
${DSP}/Source/MatrixFunctions/arm_householder_f64.c
${DSP}/Source/MatrixFunctions/arm_mat_ldlt_f64.c
)

set(SRCF32 
${DSP}/Source/MatrixFunctions/arm_mat_inverse_f32.c
${DSP}/Source/MatrixFunctions/arm_mat_init_f32.c
${DSP}/Source/MatrixFunctions/arm_householder_f32.c
${DSP}/Source/MatrixFunctions/arm_mat_cmplx_trans_f32.c
${DSP}/Source/MatrixFunctions/arm_mat_qr_f32.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_mult_f32.c
${DSP}/Source/MatrixFunctions/arm_mat_ldlt_f32.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_vec_mult_f32.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_cholesky_f32.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_solve_lower_triangular_f32.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_solve_upper_triangular_f32.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_add_f32.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_scale_f32.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_sub_f32.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_trans_f32.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_cmplx_mult_f32.c
)

set(SRCQ31 
${DSP}/Source/MatrixFunctions/arm_mat_mult_opt_q31.c
${DSP}/Source/MatrixFunctions/arm_mat_init_q31.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_add_q31.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_cmplx_mult_q31.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_cmplx_trans_q31.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_mult_fast_q31.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_mult_q31.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_scale_q31.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_sub_q31.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_trans_q31.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_vec_mult_q31.c
)

set(SRCQ15 
${DSP}/Source/MatrixFunctions/arm_mat_init_q15.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_add_q15.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_cmplx_mult_q15.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_cmplx_trans_q15.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_mult_fast_q15.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_mult_q15.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_scale_q15.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_sub_q15.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_trans_q15.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_vec_mult_q15.c
)

set(SRCQ7 
${DSP}/Source/MatrixFunctions/arm_mat_init_q7.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_mult_q7.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_trans_q7.c
${SOURCES_RVV_PATH}/MatrixFunctions/arm_mat_vec_mult_q7.c
)

set(SRCF16 
${DSP}/Source/MatrixFunctions/arm_mat_add_f16.c
${DSP}/Source/MatrixFunctions/arm_mat_cholesky_f16.c
${DSP}/Source/MatrixFunctions/arm_mat_cmplx_mult_f16.c
${DSP}/Source/MatrixFunctions/arm_mat_cmplx_trans_f16.c
${DSP}/Source/MatrixFunctions/arm_mat_init_f16.c
${DSP}/Source/MatrixFunctions/arm_mat_inverse_f16.c
${DSP}/Source/MatrixFunctions/arm_mat_mult_f16.c
${DSP}/Source/MatrixFunctions/arm_mat_scale_f16.c
${DSP}/Source/MatrixFunctions/arm_mat_solve_lower_triangular_f16.c
${DSP}/Source/MatrixFunctions/arm_mat_solve_upper_triangular_f16.c
${DSP}/Source/MatrixFunctions/arm_mat_sub_f16.c
${DSP}/Source/MatrixFunctions/arm_mat_trans_f16.c
${DSP}/Source/MatrixFunctions/arm_mat_vec_mult_f16.c
${DSP}/Source/MatrixFunctions/arm_mat_qr_f16.c
${DSP}/Source/MatrixFunctions/arm_householder_f16.c
)

# build component
list(APPEND SOURCES ${SRCF64})
list(APPEND SOURCES ${SRCF32})
list(APPEND SOURCES ${SRCQ31})
list(APPEND SOURCES ${SRCQ15})
list(APPEND SOURCES ${SRCQ7})

if ((NOT ARMAC5) AND (NOT DISABLEFLOAT16))
list(APPEND SOURCES ${SRCF16})
endif()

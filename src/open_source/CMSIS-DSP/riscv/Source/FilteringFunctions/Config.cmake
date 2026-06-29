set(SRC_FILTERING_F64 
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df2T_f64.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df2T_init_f64.c
${DSP}/Source/FilteringFunctions/arm_correlate_f64.c
${DSP}/Source/FilteringFunctions/arm_fir_decimate_f64.c
${DSP}/Source/FilteringFunctions/arm_fir_decimate_init_f64.c
${DSP}/Source/FilteringFunctions/arm_fir_f64.c
${DSP}/Source/FilteringFunctions/arm_fir_init_f64.c
)

set(SRC_FILTERING_F32 
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df1_f32.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df1_init_f32.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df2T_f32.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df2T_init_f32.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_stereo_df2T_f32.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_stereo_df2T_init_f32.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_conv_f32.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_conv_partial_f32.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_correlate_f32.c
${DSP}/Source/FilteringFunctions/arm_fir_decimate_f32.c
${DSP}/Source/FilteringFunctions/arm_fir_decimate_init_f32.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_fir_f32.c
${DSP}/Source/FilteringFunctions/arm_fir_init_f32.c
${DSP}/Source/FilteringFunctions/arm_fir_interpolate_f32.c
${DSP}/Source/FilteringFunctions/arm_fir_interpolate_init_f32.c
${DSP}/Source/FilteringFunctions/arm_fir_lattice_f32.c
${DSP}/Source/FilteringFunctions/arm_fir_lattice_init_f32.c
${DSP}/Source/FilteringFunctions/arm_fir_sparse_f32.c
${DSP}/Source/FilteringFunctions/arm_fir_sparse_init_f32.c
${DSP}/Source/FilteringFunctions/arm_iir_lattice_f32.c
${DSP}/Source/FilteringFunctions/arm_iir_lattice_init_f32.c
${DSP}/Source/FilteringFunctions/arm_lms_f32.c
${DSP}/Source/FilteringFunctions/arm_lms_init_f32.c
${DSP}/Source/FilteringFunctions/arm_lms_norm_f32.c
${DSP}/Source/FilteringFunctions/arm_lms_norm_init_f32.c
${DSP}/Source/FilteringFunctions/arm_levinson_durbin_f32.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df2T_f32.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df2T_init_f32.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_stereo_df2T_f32.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_stereo_df2T_init_f32.c
)

set(SRC_FILTERING_Q31 
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df1_32x64_init_q31.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df1_32x64_q31.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df1_fast_q31.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df1_init_q31.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df1_q31.c
${DSP}/Source/FilteringFunctions/arm_conv_fast_q31.c
${DSP}/Source/FilteringFunctions/arm_conv_partial_fast_q31.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_conv_partial_q31.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_conv_q31.c
${DSP}/Source/FilteringFunctions/arm_correlate_fast_q31.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_correlate_q31.c
${DSP}/Source/FilteringFunctions/arm_fir_decimate_fast_q31.c
${DSP}/Source/FilteringFunctions/arm_fir_decimate_init_q31.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_fir_decimate_q31.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_fir_fast_q31.c
${DSP}/Source/FilteringFunctions/arm_fir_init_q31.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_fir_q31.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_fir_interpolate_q31.c
${DSP}/Source/FilteringFunctions/arm_fir_interpolate_init_q31.c
${DSP}/Source/FilteringFunctions/arm_fir_lattice_init_q31.c
${DSP}/Source/FilteringFunctions/arm_fir_lattice_q31.c
${DSP}/Source/FilteringFunctions/arm_fir_sparse_init_q31.c
${DSP}/Source/FilteringFunctions/arm_fir_sparse_q31.c
${DSP}/Source/FilteringFunctions/arm_iir_lattice_init_q31.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_iir_lattice_q31.c
${DSP}/Source/FilteringFunctions/arm_lms_init_q31.c
${DSP}/Source/FilteringFunctions/arm_lms_norm_q31.c
${DSP}/Source/FilteringFunctions/arm_lms_q31.c
${DSP}/Source/FilteringFunctions/arm_levinson_durbin_q31.c
)

set(SRC_FILTERING_Q15 
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df1_fast_q15.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df1_init_q15.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df1_q15.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_conv_fast_opt_q15.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_conv_fast_q15.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_conv_opt_q15.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_conv_partial_fast_opt_q15.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_conv_partial_fast_q15.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_conv_partial_opt_q15.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_conv_partial_q15.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_conv_q15.c
${DSP}/Source/FilteringFunctions/arm_correlate_fast_opt_q15.c
${DSP}/Source/FilteringFunctions/arm_correlate_fast_q15.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_correlate_opt_q15.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_correlate_q15.c
${DSP}/Source/FilteringFunctions/arm_fir_decimate_fast_q15.c
${DSP}/Source/FilteringFunctions/arm_fir_decimate_init_q15.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_fir_decimate_q15.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_fir_fast_q15.c
${DSP}/Source/FilteringFunctions/arm_fir_init_q15.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_fir_q15.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_fir_interpolate_q15.c
${DSP}/Source/FilteringFunctions/arm_fir_interpolate_init_q15.c
${DSP}/Source/FilteringFunctions/arm_fir_lattice_init_q15.c
${DSP}/Source/FilteringFunctions/arm_fir_lattice_q15.c
${DSP}/Source/FilteringFunctions/arm_fir_sparse_init_q15.c
${DSP}/Source/FilteringFunctions/arm_fir_sparse_q15.c
${DSP}/Source/FilteringFunctions/arm_iir_lattice_init_q15.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_iir_lattice_q15.c
${DSP}/Source/FilteringFunctions/arm_lms_init_q15.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_lms_norm_q15.c
${DSP}/Source/FilteringFunctions/arm_lms_q15.c
)

set(SRC_FILTERING_Q7 
${SOURCES_RVV_PATH}/FilteringFunctions/arm_conv_opt_q7.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_conv_partial_opt_q7.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_conv_partial_q7.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_conv_q7.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_correlate_opt_q7.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_correlate_q7.c
${DSP}/Source/FilteringFunctions/arm_fir_init_q7.c
${SOURCES_RVV_PATH}/FilteringFunctions/arm_fir_q7.c
${DSP}/Source/FilteringFunctions/arm_fir_sparse_init_q7.c
${DSP}/Source/FilteringFunctions/arm_fir_sparse_q7.c
)
set(SRC_FILTERING_F16 
${DSP}/Source/FilteringFunctions/arm_fir_f16.c
${DSP}/Source/FilteringFunctions/arm_fir_init_f16.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df1_f16.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df1_init_f16.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df2T_f16.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_df2T_init_f16.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_stereo_df2T_f16.c
${DSP}/Source/FilteringFunctions/arm_biquad_cascade_stereo_df2T_init_f16.c
${DSP}/Source/FilteringFunctions/arm_correlate_f16.c
${DSP}/Source/FilteringFunctions/arm_levinson_durbin_f16.c
)


# build component
list(APPEND SOURCES ${SRC_FILTERING_F32})
list(APPEND SOURCES ${SRC_FILTERING_F64})
list(APPEND SOURCES ${SRC_FILTERING_Q31})
list(APPEND SOURCES ${SRC_FILTERING_Q15})
list(APPEND SOURCES ${SRC_FILTERING_Q7})

if (NOT CONFIGTABLE OR ALLFAST OR ARM_LMS_NORM_Q31)
    list(APPEND SOURCES ${SOURCES_RVV_PATH}/FilteringFunctions/arm_lms_norm_init_q31.c)
endif()

if (NOT CONFIGTABLE OR ALLFAST OR ARM_LMS_NORM_Q15)
    list(APPEND SOURCES ${DSP}/Source/FilteringFunctions/arm_lms_norm_init_q15.c)
endif()

if ((NOT ARMAC5) AND (NOT DISABLEFLOAT16))
    list(APPEND SOURCES ${SRC_FILTERING_F16})
endif()

if (CONFIGTABLE AND ALLFAST)
    list(APPEND PUBLIC_DEFINES ARM_ALL_FAST_TABLES)
endif()

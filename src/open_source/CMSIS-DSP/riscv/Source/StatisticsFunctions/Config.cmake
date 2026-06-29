set(SRC_STATISTICS_F64 
${DSP}/Source/StatisticsFunctions/arm_entropy_f64.c
${DSP}/Source/StatisticsFunctions/arm_kullback_leibler_f64.c
${DSP}/Source/StatisticsFunctions/arm_absmax_f64.c
${DSP}/Source/StatisticsFunctions/arm_absmax_no_idx_f64.c
${DSP}/Source/StatisticsFunctions/arm_absmin_f64.c
${DSP}/Source/StatisticsFunctions/arm_absmin_no_idx_f64.c
${DSP}/Source/StatisticsFunctions/arm_accumulate_f64.c
${DSP}/Source/StatisticsFunctions/arm_max_f64.c
${DSP}/Source/StatisticsFunctions/arm_max_no_idx_f64.c
${DSP}/Source/StatisticsFunctions/arm_mean_f64.c
${DSP}/Source/StatisticsFunctions/arm_min_f64.c
${DSP}/Source/StatisticsFunctions/arm_min_no_idx_f64.c
${DSP}/Source/StatisticsFunctions/arm_mse_f64.c
${DSP}/Source/StatisticsFunctions/arm_power_f64.c
${DSP}/Source/StatisticsFunctions/arm_std_f64.c
${DSP}/Source/StatisticsFunctions/arm_var_f64.c
)

set(SRC_STATISTICS_F32 
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_entropy_f32.c
${DSP}/Source/StatisticsFunctions/arm_kullback_leibler_f32.c
${DSP}/Source/StatisticsFunctions/arm_logsumexp_dot_prod_f32.c
${DSP}/Source/StatisticsFunctions/arm_logsumexp_f32.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_max_f32.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_max_no_idx_f32.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_min_no_idx_f32.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_mean_f32.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_min_f32.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_power_f32.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_rms_f32.c
${DSP}/Source/StatisticsFunctions/arm_std_f32.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_var_f32.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_absmax_f32.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_absmax_no_idx_f32.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_absmin_f32.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_absmin_no_idx_f32.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_mse_f32.c
${DSP}/Source/StatisticsFunctions/arm_accumulate_f32.c
)

set(SRC_STATISTICS_Q31 
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_max_q31.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_max_no_idx_q31.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_min_no_idx_q31.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_mean_q31.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_min_q31.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_power_q31.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_rms_q31.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_std_q31.c
${DSP}/Source/StatisticsFunctions/arm_var_q31.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_absmax_q31.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_absmax_no_idx_q31.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_absmin_q31.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_absmin_no_idx_q31.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_mse_q31.c
)

set(SRC_STATISTICS_Q15 
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_max_q15.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_max_no_idx_q15.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_min_no_idx_q15.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_mean_q15.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_min_q15.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_power_q15.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_rms_q15.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_std_q15.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_var_q15.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_absmax_q15.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_absmax_no_idx_q15.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_absmin_q15.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_absmin_no_idx_q15.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_mse_q15.c
)

set(SRC_STATISTICS_Q7 
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_max_q7.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_max_no_idx_q7.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_min_no_idx_q7.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_mean_q7.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_min_q7.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_power_q7.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_absmax_q7.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_absmax_no_idx_q7.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_absmin_q7.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_absmin_no_idx_q7.c
${SOURCES_RVV_PATH}/StatisticsFunctions/arm_mse_q7.c
)

set(SRC_STATISTICS_F16 
${DSP}/Source/StatisticsFunctions/arm_max_f16.c
${DSP}/Source/StatisticsFunctions/arm_min_f16.c
${DSP}/Source/StatisticsFunctions/arm_mean_f16.c
${DSP}/Source/StatisticsFunctions/arm_power_f16.c
${DSP}/Source/StatisticsFunctions/arm_rms_f16.c
${DSP}/Source/StatisticsFunctions/arm_std_f16.c
${DSP}/Source/StatisticsFunctions/arm_var_f16.c
${DSP}/Source/StatisticsFunctions/arm_entropy_f16.c
${DSP}/Source/StatisticsFunctions/arm_kullback_leibler_f16.c
${DSP}/Source/StatisticsFunctions/arm_logsumexp_dot_prod_f16.c
${DSP}/Source/StatisticsFunctions/arm_logsumexp_f16.c
${DSP}/Source/StatisticsFunctions/arm_max_no_idx_f16.c
${DSP}/Source/StatisticsFunctions/arm_min_no_idx_f16.c
${DSP}/Source/StatisticsFunctions/arm_absmax_f16.c
${DSP}/Source/StatisticsFunctions/arm_absmin_f16.c
${DSP}/Source/StatisticsFunctions/arm_absmax_no_idx_f16.c
${DSP}/Source/StatisticsFunctions/arm_absmin_no_idx_f16.c
${DSP}/Source/StatisticsFunctions/arm_mse_f16.c
${DSP}/Source/StatisticsFunctions/arm_accumulate_f16.c
)

list(APPEND SOURCES ${SRC_STATISTICS_F64})
list(APPEND SOURCES ${SRC_STATISTICS_F32})
list(APPEND SOURCES ${SRC_STATISTICS_Q31})
list(APPEND SOURCES ${SRC_STATISTICS_Q15})
list(APPEND SOURCES ${SRC_STATISTICS_Q7})

if ((NOT ARMAC5) AND (NOT DISABLEFLOAT16))
list(APPEND SOURCES ${SRC_STATISTICS_F16})
endif()
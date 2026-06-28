set(SRC_DISTANCE_F64 
${DSP}/Source/DistanceFunctions/arm_chebyshev_distance_f64.c
${DSP}/Source/DistanceFunctions/arm_cityblock_distance_f64.c
${DSP}/Source/DistanceFunctions/arm_cosine_distance_f64.c
${DSP}/Source/DistanceFunctions/arm_euclidean_distance_f64.c
)
set(SRC_DISTANCE_F32 
${DSP}/Source/DistanceFunctions/arm_boolean_distance.c
${SOURCES_RVV_PATH}/DistanceFunctions/arm_braycurtis_distance_f32.c
${SOURCES_RVV_PATH}/DistanceFunctions/arm_canberra_distance_f32.c
${SOURCES_RVV_PATH}/DistanceFunctions/arm_chebyshev_distance_f32.c
${SOURCES_RVV_PATH}/DistanceFunctions/arm_cityblock_distance_f32.c
${DSP}/Source/DistanceFunctions/arm_correlation_distance_f32.c
${DSP}/Source/DistanceFunctions/arm_cosine_distance_f32.c
${DSP}/Source/DistanceFunctions/arm_dice_distance.c
${SOURCES_RVV_PATH}/DistanceFunctions/arm_euclidean_distance_f32.c
${DSP}/Source/DistanceFunctions/arm_hamming_distance.c
${DSP}/Source/DistanceFunctions/arm_jaccard_distance.c
${DSP}/Source/DistanceFunctions/arm_dtw_distance_f32.c
${DSP}/Source/DistanceFunctions/arm_dtw_path_f32.c
${DSP}/Source/DistanceFunctions/arm_jensenshannon_distance_f32.c
${DSP}/Source/DistanceFunctions/arm_kulsinski_distance.c
${DSP}/Source/DistanceFunctions/arm_minkowski_distance_f32.c
${DSP}/Source/DistanceFunctions/arm_rogerstanimoto_distance.c
${DSP}/Source/DistanceFunctions/arm_russellrao_distance.c
${DSP}/Source/DistanceFunctions/arm_sokalmichener_distance.c
${DSP}/Source/DistanceFunctions/arm_sokalsneath_distance.c
${DSP}/Source/DistanceFunctions/arm_yule_distance.c
${DSP}/Source/DistanceFunctions/arm_dtw_init_window_q7.c
)

set(SRC_DISTANCE_F16 
${DSP}/Source/DistanceFunctions/arm_braycurtis_distance_f16.c
${DSP}/Source/DistanceFunctions/arm_canberra_distance_f16.c
${DSP}/Source/DistanceFunctions/arm_chebyshev_distance_f16.c
${DSP}/Source/DistanceFunctions/arm_cityblock_distance_f16.c
${DSP}/Source/DistanceFunctions/arm_correlation_distance_f16.c
${DSP}/Source/DistanceFunctions/arm_cosine_distance_f16.c
${DSP}/Source/DistanceFunctions/arm_euclidean_distance_f16.c
${DSP}/Source/DistanceFunctions/arm_jensenshannon_distance_f16.c
${DSP}/Source/DistanceFunctions/arm_minkowski_distance_f16.c
)

# build component
list(APPEND SOURCES ${SRC_DISTANCE_F64})
list(APPEND SOURCES ${SRC_DISTANCE_F32})

if ((NOT ARMAC5) AND (NOT DISABLEFLOAT16))
list(APPEND SOURCES ${SRC_DISTANCE_F16})
endif()
set(SRC_QUATERNION_F32 
${SOURCES_RVV_PATH}/QuaternionMathFunctions/arm_quaternion_norm_f32.c
${SOURCES_RVV_PATH}/QuaternionMathFunctions/arm_quaternion_inverse_f32.c
${SOURCES_RVV_PATH}/QuaternionMathFunctions/arm_quaternion_conjugate_f32.c
${SOURCES_RVV_PATH}/QuaternionMathFunctions/arm_quaternion_normalize_f32.c
${SOURCES_RVV_PATH}/QuaternionMathFunctions/arm_quaternion_product_single_f32.c
${SOURCES_RVV_PATH}/QuaternionMathFunctions/arm_quaternion_product_f32.c
${SOURCES_RVV_PATH}/QuaternionMathFunctions/arm_quaternion2rotation_f32.c
${SOURCES_RVV_PATH}/QuaternionMathFunctions/arm_rotation2quaternion_f32.c
)

set(SRC_QUATERNION_F16
)

# build component
list(APPEND SOURCES ${SRC_QUATERNION_F32})

if ((NOT ARMAC5) AND (NOT DISABLEFLOAT16))
list(APPEND SOURCES ${SRC_QUATERNION_F16})
endif()
set(SRC_SVM_F32 
${DSP}/Source/SVMFunctions/arm_svm_linear_init_f32.c
${DSP}/Source/SVMFunctions/arm_svm_rbf_init_f32.c
${DSP}/Source/SVMFunctions/arm_svm_linear_predict_f32.c
${DSP}/Source/SVMFunctions/arm_svm_rbf_predict_f32.c
${DSP}/Source/SVMFunctions/arm_svm_polynomial_init_f32.c
${DSP}/Source/SVMFunctions/arm_svm_sigmoid_init_f32.c
${DSP}/Source/SVMFunctions/arm_svm_polynomial_predict_f32.c
${DSP}/Source/SVMFunctions/arm_svm_sigmoid_predict_f32.c
)

set(SRC_SVM_F16 
${DSP}/Source/SVMFunctions/arm_svm_linear_init_f16.c
${DSP}/Source/SVMFunctions/arm_svm_rbf_init_f16.c
${DSP}/Source/SVMFunctions/arm_svm_linear_predict_f16.c
${DSP}/Source/SVMFunctions/arm_svm_rbf_predict_f16.c
${DSP}/Source/SVMFunctions/arm_svm_polynomial_init_f16.c
${DSP}/Source/SVMFunctions/arm_svm_sigmoid_init_f16.c
${DSP}/Source/SVMFunctions/arm_svm_polynomial_predict_f16.c
${DSP}/Source/SVMFunctions/arm_svm_sigmoid_predict_f16.c
)

# build component
list(APPEND SOURCES ${SRC_SVM_F32})

if ((NOT ARMAC5) AND (NOT DISABLEFLOAT16))
list(APPEND SOURCES ${SRC_SVM_F16})
endif()
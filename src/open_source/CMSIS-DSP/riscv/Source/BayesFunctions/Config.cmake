set(SRCBAYESF32 
${DSP}/Source/BayesFunctions/arm_gaussian_naive_bayes_predict_f32.c
)

set(SRCBAYESF16 
${DSP}/Source/BayesFunctions/arm_gaussian_naive_bayes_predict_f16.c
)

list(APPEND SOURCES ${SRCBAYESF32})

if ((NOT ARMAC5) AND (NOT DISABLEFLOAT16))
list(APPEND SOURCES ${SRCBAYESF16})
endif()
set(SRC_CONTROLLER_PID 
${DSP}/Source/ControllerFunctions/arm_pid_init_f32.c
${DSP}/Source/ControllerFunctions/arm_pid_init_q15.c
${DSP}/Source/ControllerFunctions/arm_pid_init_q31.c
${DSP}/Source/ControllerFunctions/arm_pid_reset_f32.c
${DSP}/Source/ControllerFunctions/arm_pid_reset_q15.c
${DSP}/Source/ControllerFunctions/arm_pid_reset_q31.c
)

set(SRC_CONTROLLER_F32 
${DSP}/Source/ControllerFunctions/arm_sin_cos_f32.c
)

set(SRC_CONTROLLER_Q31 
${DSP}/Source/ControllerFunctions/arm_sin_cos_q31.c
)

list(APPEND SOURCES ${SRC_CONTROLLER_PID})

if (NOT CONFIGTABLE OR ALLFAST OR ARM_SIN_COS_F32)
    list(APPEND SOURCES ${SRC_CONTROLLER_F32})
endif()

if (NOT CONFIGTABLE OR ALLFAST OR ARM_SIN_COS_Q31)
    list(APPEND SOURCES ${SRC_CONTROLLER_Q31})
endif()

if (CONFIGTABLE AND ALLFAST)
    list(APPEND PUBLIC_DEFINES ARM_ALL_FAST_TABLES)
endif()
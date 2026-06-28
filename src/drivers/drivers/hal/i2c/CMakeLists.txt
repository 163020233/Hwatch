#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_i2c")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_i2c.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(DEFINED CONFIG_I2C_USING_V100)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_i2c_v100_master.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_i2c_v100_slave.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_i2c_v100.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_i2c_v100_comm.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_i2c_v100_regs_op.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v100")
endif()

if(DEFINED CONFIG_I2C_USING_V150)
	list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v150/hal_i2c_v150_master.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v150/hal_i2c_v150.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v150/hal_i2c_v150_comm.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v150/hal_i2c_v150_regs_op.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v150")
endif()

# V151和V151_PRO除了寄存器相对位置不同,寄存器的每个bit位含义是一样的,驱动逻辑也是一样的
# V151_V1 修改了个别寄存器的bit含义和功能, 因此在代码逻辑上面有变动3点:
# 1:ic_v151_con_data_t.ic_10bitaddr_slave主动设置为false
# 2:ic_v151_enable_data_t.tx_cmd_block主动设置为false
# 3: MIN_CSL_LCNT,MIN_SCL_HCNT也是单独定义的
# V151_V2融合了V151,V151_PRO的寄存器相对位置,V151的寄存器bit的含义, 驱动逻辑和V151保持一致.
if(DEFINED CONFIG_I2C_USING_V151 OR CONFIG_I2C_USING_V151_V2)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v151/hal_i2c_v151_master.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v151/hal_i2c_v151_slave.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v151/hal_i2c_v151.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v151/hal_i2c_v151_comm.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v151/hal_i2c_v151_regs_op.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v151")
endif()

if(NOT DEFINED CONFIG_I2C_USING_V100 AND NOT DEFINED CONFIG_I2C_USING_V150 AND NOT DEFINED CONFIG_I2C_USING_V151 AND NOT DEFINED CONFIG_I2C_USING_V151_V2)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_i2c_v100_master.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_i2c_v100_slave.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_i2c_v100.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_i2c_v100_comm.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_i2c_v100_regs_op.c")
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v100")
endif()

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)
build_component()

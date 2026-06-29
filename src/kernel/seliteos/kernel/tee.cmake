#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "seliteos_kernel")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/base/communication/tesvec_records.c
    ${CMAKE_CURRENT_SOURCE_DIR}/base/communication/tesvec_ree_tee_comm.S
    ${CMAKE_CURRENT_SOURCE_DIR}/base/communication/tesvec/dispatch/tesvec_dispatch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/base/communication/tesvec/ioctl/tesvec_ioctl.c

    ${CMAKE_CURRENT_SOURCE_DIR}/../common/mem/los_kernel_mem.c

    ${CMAKE_CURRENT_SOURCE_DIR}/base/remote_call/remote_call.S
    ${CMAKE_CURRENT_SOURCE_DIR}/base/remote_call/los_remote_call.c
    ${CMAKE_CURRENT_SOURCE_DIR}/base/remote_call/remote_call.c

    ${CMAKE_CURRENT_SOURCE_DIR}/base/isolation/pmp/pmp_mgr.c

    ${CMAKE_CURRENT_SOURCE_DIR}/../arch/riscv/src/tes_csr_op.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../arch/riscv/src/exception.S
    ${CMAKE_CURRENT_SOURCE_DIR}/../arch/riscv/src/interrupt.S
    ${CMAKE_CURRENT_SOURCE_DIR}/../arch/riscv/src/trap.S

)

set(TA_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../arch/riscv/src/dispatch.S

    ${CMAKE_CURRENT_SOURCE_DIR}/service/gp/communication/tee_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service/gp/communication/ta_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service/gp/communication/tee_session.c

    ${CMAKE_CURRENT_SOURCE_DIR}/base/isolation/pmp/los_drivermgr.c

    ${CMAKE_CURRENT_SOURCE_DIR}/base/task/los_task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/base/task/task_init.c

    ${CMAKE_CURRENT_SOURCE_DIR}/base/syscalls/src/los_hw_syscalls.S
    ${CMAKE_CURRENT_SOURCE_DIR}/base/syscalls/src/syscalls_riscv32.S
    ${CMAKE_CURRENT_SOURCE_DIR}/base/syscalls/src/los_syscalls.c
)

if(${CONFIG_SUPPORT_TA} STREQUAL "y")
    list(APPEND SOURCES ${TA_SOURCES})
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../arch/riscv/include

    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/base/include
    ${CMAKE_CURRENT_SOURCE_DIR}/base/syscalls/include
    ${CMAKE_CURRENT_SOURCE_DIR}/base/syscalls/include/kernel
    ${CMAKE_CURRENT_SOURCE_DIR}/base/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/base/communication/tesvec/include
    ${CMAKE_CURRENT_SOURCE_DIR}/base/isolation/pmp
    ${CMAKE_CURRENT_SOURCE_DIR}/base/remote_call
    ${CMAKE_CURRENT_SOURCE_DIR}/base/task
    ${CMAKE_CURRENT_SOURCE_DIR}/service/gp/communication

    ${CMAKE_CURRENT_SOURCE_DIR}/../common/usrdrvlib/uart
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/usrsyslib/ta_communication
)

install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/base/include "*.h")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/../arch/riscv/include "*.h")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/base/isolation/pmp "*.h")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/base/remote_call "*.h")

if(${CONFIG_SUPPORT_TA} STREQUAL "y")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/base/syscalls/include "*.h")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/base/syscalls/include/kernel "*.h")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/base/task "*.h")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/service/gp/communication "*.h")
endif()

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/mem/bestfit_little
    ${CMAKE_CURRENT_SOURCE_DIR}/../kernel/service/psa/include
    ${CMAKE_CURRENT_SOURCE_DIR}/service/psa/crypto/common
)

set(TA_PRIVATE_DEFINES
    -DLOSCFG_BOX    # support syscall
)

set(PRIVATE_DEFINES
    -DLOSCFG_ARCH_LINX_M
    -DLOSCFG_ARCH_RISCV_TES
)

if(${CONFIG_SUPPORT_TA} STREQUAL "y")
    list(APPEND PRIVATE_DEFINES ${TA_PRIVATE_DEFINES})
endif()

set(PUBLIC_DEFINES
    # support untrusted interrupt preemption tee
    -DLOSCFG_REE_INT_PREEMPTION
    # support TEE exception delegate to REE.
    -DLOSCFG_TEE_EXECPTION_DELEGATE_ENABLE
    # support CUSTOM_INST16_EXTENSIONS
    -DLOSCFG_LCMP_CUSTOM_INST16_EXTENSIONS
    # support TMESCR EME
    -DTMESCR_EME_ENABLE
)

set(TA_PUBLIC_DEFINES
    -DLOSCFG_SUPPORT_TA     # support TA
)

if(${CONFIG_SUPPORT_TA} STREQUAL "y")
    list(APPEND PUBLIC_DEFINES ${TA_PUBLIC_DEFINES})
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
    -march=rv32gcxlinxma_xlinxmc_xlinxme
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    true
)

build_component()

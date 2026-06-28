# Copyright HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.

set(cfbb_root ${TOP_DIR}/../../../..)
 
set(LITEOS_DIR ${cfbb_root}/kernel/liteos/LiteOS)
set(AUDIODSP_DIR ${cfbb_root}/drivers/drivers/driver/audio/src/sdk/mu01xx/mu0103/dsp/vendor/hisi/audiodsp)
set(NPU_DIR ${cfbb_root}/protocol/npu/npu_nano)

set(NANO_LITEOS_HEADER_FILES_PATH
    ${LITEOS_DIR}/self_src/kernel_lite/include
    ${LITEOS_DIR}/self_src/targets/3322_app/include
    ${LITEOS_DIR}/self_src/drivers/interrupt/include
    ${LITEOS_DIR}/self_src/drivers/timer/include
    ${LITEOS_DIR}/self_src/arch_lite/riscv/include
    ${LITEOS_DIR}/self_src/lib/liteos_libc/include

    ${cfbb_root}/open_source/musl/include
    ${LITEOS_DIR}/lib/liteos_libc/include

    ${NPU_DIR}/lib/file_sys
    ${NPU_DIR}/inc
    ${NPU_DIR}/osal/liteos/arch
    ${NPU_DIR}/proj
    ${cfbb_root}/middleware/utils/hwsec_c/include
)

set(NANO_LITEOS_HEADERS_PATH
    ${NANO_LITEOS_HEADER_FILES_PATH}
)

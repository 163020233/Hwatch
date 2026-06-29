#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

if(${CHIP} MATCHES "sw21")
include(vfmw_v7.0/product/hi1a21/HD_FULL/CMakeLists.txt)
elseif(${CHIP} MATCHES "sw22")
include(vfmw_v7.0/product/hi1a22/HD_FULL/CMakeLists.txt)
elseif(${CHIP} MATCHES "3322")
include(vfmw_v7.0/product/hi3322/HD_FULL/CMakeLists.txt)
endif()

set(VFMW_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vfmw_v7.0)
set(CORE_DIR ${VFMW_SRC_DIR}/core)
set(COM_DIR  ${CORE_DIR}/common)
set(DPS_DIR  ${CORE_DIR}/dps)
set(STM_DIR  ${CORE_DIR}/stream)
set(SYN_DIR  ${CORE_DIR}/syntax)
set(DEC_DIR  ${CORE_DIR}/decode)
set(STD_DIR  ${CORE_DIR}/syntax/protocol)
set(MST_DIR  ${VFMW_SRC_DIR}/master)
set(OSL_DIR  ${VFMW_SRC_DIR}/osal)
set(TEST_DIR ${ROOT_DIR}/test/platform/testsuite/test_vdec/test/crc/testcode)

set(
    vfmw_drv_list
		${OSL_DIR}/vfmw_osal.c
		${OSL_DIR}/liteos/liteos_osal.c
		${OSL_DIR}/liteos/liteos_mem.c
		${COM_DIR}/dbg.c
		${COM_DIR}/event.c
		${COM_DIR}/fsp.c
		${COM_DIR}/vctrl.c
		${COM_DIR}/voque.c
		${STM_DIR}/stm_intf.c
		${STM_DIR}/stm_ctrl.c
		${STM_DIR}/stm_dev.c
		${STM_DIR}/stm_thread.c
		${STM_DIR}/stm_chan.c
		${STM_DIR}/stm_raw.c
		${STM_DIR}/stm_seg.c
		${SYN_DIR}/syn_intf.c
		${SYN_DIR}/syn_ctrl.c
		${SYN_DIR}/syn_parse.c
		${SYN_DIR}/syn_ops.c
		${STD_DIR}/syn_bs.c
		${DEC_DIR}/dec_intf.c
		${DEC_DIR}/dec_ctrl.c
		${DEC_DIR}/dec_thread.c
		${DEC_DIR}/dec_chn.c
		${DEC_DIR}/dec_dev.c
		${DEC_DIR}/dec_ref.c
		${DEC_DIR}/dec_post.c
		${DPS_DIR}/dps.c
		${DPS_DIR}/dps_ops.c
		${MST_DIR}/vfmw_intf.c
		${MST_DIR}/vfmw_adpt.c
		${MST_DIR}/vfmw_img.c
		${MST_DIR}/local/local.c
		${MST_DIR}/module/stream/stream.c
		${MST_DIR}/module/vpp/vpp.c
		${MST_DIR}/module/pts/pts.c
		${MST_DIR}/module/pts/frd.c
		${MST_DIR}/module/userdec/userdec.c
		${OSL_DIR}/liteos/liteos_proc.c
)

set(
    vfmw_drv_proc_list
		${STM_DIR}/stm_log.c
		${SYN_DIR}/syn_log.c
		${DEC_DIR}/dec_log.c
		${DPS_DIR}/dps_log.c
		${COM_DIR}/voque_log.c
		${COM_DIR}/vctrl_log.c
		${COM_DIR}/fsp_log.c
		${MST_DIR}/local/local_proc.c
		${MST_DIR}/module/stream/stream_log.c
		${MST_DIR}/module/vpp/vpp_log.c
		${MST_DIR}/vfmw_adpt_proc.c
)

if(${vdec_proc_support} STREQUAL "yes")
set(vfmw_drv_list "${vfmw_drv_list}" "${vfmw_drv_proc_list}"  CACHE INTERNAL "platform src obj list" FORCE)
endif()
set(VDEC_SRC_LIST "${VDEC_SRC_LIST}" "${vfmw_drv_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(
    vfmw_drv_header_list
		${VFMW_SRC_DIR}
		${VFMW_SRC_DIR}/include
		${VFMW_SRC_DIR}/master
		${VFMW_SRC_DIR}/master/local
		${VFMW_SRC_DIR}/master/module/pts
		${VFMW_SRC_DIR}/master/module/stream
		${VFMW_SRC_DIR}/master/module/vpp
		${VFMW_SRC_DIR}/master/module/crc
		${VFMW_SRC_DIR}/master/module/userdec
		${VFMW_SRC_DIR}/osal
		${VFMW_SRC_DIR}/osal/liteos
		${VFMW_SRC_DIR}/core/dps
		${VFMW_SRC_DIR}/core/stream
		${VFMW_SRC_DIR}/core/syntax
		${VFMW_SRC_DIR}/core/decode
		${VFMW_SRC_DIR}/core/syntax/protocol
		${VFMW_SRC_DIR}/core/common
		${VFMW_SRC_DIR}/product
)

if(${vfmw_ipt_support} STREQUAL "yes")
set(
    vfmw_ipt_header
        ${TEST_DIR}/ipt
)
endif()

set(VDEC_INC_LIST "${VDEC_INC_LIST}" "${vfmw_drv_header_list}" "${vfmw_ipt_header}" CACHE INTERNAL "platform src obj list" FORCE)

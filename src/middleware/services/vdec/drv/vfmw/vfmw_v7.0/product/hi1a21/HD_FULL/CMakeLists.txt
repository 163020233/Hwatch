#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(CHIP_VER hi1a21)
set(SCD_VER v3r3)
set(VDH_VER v2r4)

set(VFMW_SRC_DIR ${ROOT_DIR}/middleware/services/vdec/drv/vfmw/vfmw_v7.0)
set(CORE_DIR ${VFMW_SRC_DIR}/core)
set(DPS_DIR  ${CORE_DIR}/dps)
set(STD_DIR  ${CORE_DIR}/syntax/protocol)
set(SCD_DIR  ${CORE_DIR}/stream/hal/${SCD_VER})
set(VDH_DIR  ${CORE_DIR}/decode/hal/${VDH_VER})
set(PDT_DIR  ${VFMW_SRC_DIR}/product/${CHIP_VER})

set(vfmw_product_src_list
		${SCD_DIR}/scd_hal.c
		${VDH_DIR}/dec_hal.c
		${VDH_DIR}/vdh_h264.c
		${STD_DIR}/h264.c
		${DPS_DIR}/dps_h264.c
		${PDT_DIR}/pdt_master.c
		${PDT_DIR}/pdt_com.c
)
set(VDEC_SRC_LIST "${VDEC_SRC_LIST}" "${vfmw_product_src_list}" CACHE INTERNAL "platform src obj list" FORCE)

set(vfmw_product_inc_list
		${VFMW_SRC_DIR}/product/${CHIP_VER}
		${VFMW_SRC_DIR}/product/${CHIP_VER}/HD_FULL
		${VFMW_SRC_DIR}/core/stream/hal/${SCD_VER}
		${VFMW_SRC_DIR}/core/decode/hal/${VDH_VER}
)
set(VDEC_INC_LIST "${VDEC_INC_LIST}" "${vfmw_product_inc_list}" CACHE INTERNAL "platform src obj list" FORCE)

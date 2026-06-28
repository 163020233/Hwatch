/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_extern.h
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef VIU_EXTERN_H
#define VIU_EXTERN_H

#include "drv_mcp_dependency.h"

#if defined(CONFIG_CHIP_3322)
#include "chip/3322/viu_extern_vb.h"
#include "chip/3322/viu_extern_venc.h"
#elif defined(CONFIG_CHIP_SHAOLINSPEAR)
#include "chip/shaolinspear/viu_extern_vb.h"
#include "chip/viu_extern_isp.h"
#include "chip/viu_extern_ppu.h"
#include "chip/viu_export_dis.h"
#include "chip/viu_export_gdc.h"
#include "chip/viu_export_vgs.h"
#include "chip/viu_export_viu.h"
#else // 610/1A22
#include "chip/1A22/viu_extern_vb.h"
#include "chip/1A22/viu_extern_isp.h"
#include "chip/1A22/viu_extern_ppu.h"
#include "chip/viu_export_dis.h"
#include "chip/viu_export_gdc.h"
#include "chip/viu_export_vgs.h"
#include "chip/viu_export_viu.h"
#endif

#endif


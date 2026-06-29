/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  common logging producer interface - need to change name of log.h in all the protocol core files
 * Author: @CompanyNameTag
 * Create:
 */

#ifndef LOG_DEF_H
#define LOG_DEF_H

#ifdef CONFIG_BT_STATIC_LIB_NOT_SUPPORT
#ifdef BTC_SYS_PART
#include "log_def_btc.h"
#endif

#else
#ifdef CONFIG_BT_CORE
#include "log_def_btc.h"
#endif

#endif

#include "log_def_gpu.h"
#include "log_def_gui.h"
#include "log_def_pf.h"
#include "log_def_dsp.h"
#include "log_def_app.h"
#include "log_def_media.h"
#include "log_def_ohos.h"

#endif

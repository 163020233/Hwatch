/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SYS_TYPEDEFS_H
 * Author: czc
 * Create: 2025-1-5
 */

#ifndef _SYS_TYPEDEFS_H_
#define _SYS_TYPEDEFS_H_

#include <float.h>

#ifdef _WIN32
#if _MSC_VER == 1200  // vc6
typedef _int8 int8_t;
typedef _int16 int16_t;
typedef _int32 int32_t;
typedef _int64 int64_t;
typedef unsigned _int8 uint8_t;
typedef unsigned _int16 uint16_t;
typedef unsigned _int32 uint32_t;
typedef unsigned _int64 uint64_t;
#else
#include <stdint.h>
#endif // vc6
#else
#include <stdint.h>
#endif

#define CC_NUM_CEP_COEFFS 13
#define CC_ONLINE_DELAY 6
#define CC_FFT_SHIFT  29

#if defined QUALCOMM_4010
  #define LLONG_MIN  (~0x7fffffffffffffffLL)
#else
  #include <limits.h>
#endif  // QUALCOMM_4010

#if defined QUALCOMM_4010
    typedef long long int64_t;
    typedef unsigned long long uint64_t;
#endif  // QUALCOMM_4010

typedef void* HANDLE;

#ifndef NULL
#define NULL 0
#endif  // NULL

#ifdef BIN_MDL
  #define ATTR_CONST
#else
  #define ATTR_CONST const
#endif

#endif

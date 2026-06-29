/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: for TI、LSI、ARM、HiFi interface use
 * Author:audio
 * Create:2019
 * Update:2019-12-17
 */

#ifndef AUDIO_VQE_TYPEDEF_H
#define AUDIO_VQE_TYPEDEF_H

#ifdef __cplusplus
extern "C" {
#endif

/* use uniDSP Macro definition prevents compilation errors caused by repeated definitions */
#ifndef UNI_TYPE_DEF
#define UNI_TYPE_DEF

#define VAL_0  0
#define VAL_1  1
#define VAL_2  2
#define VAL_3  3
#define VAL_4  4
#define VAL_5  5
#define VAL_6  6
#define VAL_7  7
#define VAL_8  8
#define VAL_9  9
#define VAL_15  15
#define VAL_19  19
#define VAL_64  64
#define VAL_255  255

/* 8bit */
typedef unsigned char imedia_uint8;
typedef signed char imedia_int8;
typedef char imedia_char; /* 8bit, and uniDSP header file compatible */

/* 16bit */
typedef unsigned short imedia_uint16;
typedef signed short imedia_int16;

/* 32bit */
typedef unsigned int imedia_uint32;
typedef signed int imedia_int32;
typedef unsigned int imedia_bool; /* 32bit, and uniDSP header file compatible */

/* 32bit,  and uniDSP header file compatible */
typedef unsigned int imedia_uintptr;
typedef signed int imedia_intptr;

/* float point type redefine:  and uniDSP header file compatible */
typedef float imedia_float;
typedef double imedia_double;

/* float point type redefine:  and VTC、VQEalg with same definetion */
typedef float imedia_float32;
typedef double imedia_float64;

/* VOID redefine， and uniDSP header file compatible */
typedef void imedia_void;

typedef unsigned long long imedia_uint64;
typedef long long imedia_int64;

/* linux 64bit */
#ifdef __linux__
typedef unsigned long long imedia_uint64;
typedef long long imedia_int64;
#endif

/* ARM 64bit */
#ifdef __arm__
typedef unsigned long long imedia_uint64;
typedef long long imedia_int64;
#endif

#ifdef __aarch64__
typedef unsigned long long imedia_uint64;
typedef long long imedia_int64;
#endif

/* MSVC 64bit */
#if defined(_MSC_VER)
typedef __int64 imedia_int64;
#endif

#if defined(_MSC_VER)
typedef unsigned __int64 imedia_uint64;
#endif

/* TI 40 or 64bit */
#ifdef __COMPILER_VERSION__
/*
 * 40bit
 * COFF  long bitwidth 40bit, ELF
 */
#ifdef __TI_40BIT_LONG__
/* long bitwidth 32bit */
typedef unsigned long imedia_uint40;
typedef long imedia_int40;
#else /* ELF 40bit define */
typedef unsigned __int40_t imedia_uint40;
typedef __int40_t imedia_int40;
#endif

#if (__COMPILER_VERSION__ < 5000000)
/* CBSC TI low version of complier(4.32,< v5.0) unsupport long long type */
#else
typedef unsigned long long imedia_uint64;
typedef long long imedia_int64;
#endif
#endif

/* HIFI 64bit */
#ifdef __XTENSA__
typedef unsigned long long imedia_uint64;
typedef long long imedia_int64;
#endif

#endif /* end of #ifndef UNI_TYPE_DEF */

#ifndef IMEDIA_NULL
#define IMEDIA_NULL ((imedia_void *)0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* end of #ifndef AUDIO_VQE_TYPEDEF_H */

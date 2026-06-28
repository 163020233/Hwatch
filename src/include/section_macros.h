/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: section define. \n
 *
 */

#ifndef SECTION_MACROS_H
#define SECTION_MACROS_H

#ifdef ENABLE_LTO_OPTIMIZE

#define STRINGIFY_SECTION(x) #x
// Match global variables or functions
#define AUTO_SECTION_FUNC(func) __attribute__((section(".text."STRINGIFY_SECTION(func))))
#define AUTO_SECTION_RODATA(val) __attribute__((section(".rodata."STRINGIFY_SECTION(val))))
#define AUTO_SECTION_DATA(val) __attribute__((section(".data."STRINGIFY_SECTION(val))))
#define AUTO_SECTION_BSS(val) __attribute__((section(".bss."STRINGIFY_SECTION(val))))

// Match the global variables or functions that require KEEP.
#define KEEP_SECTION_FUNC(func) __attribute__((used, section(".text."STRINGIFY_SECTION(func))))
#define KEEP_SECTION_RODATA(val) __attribute__((used, section(".rodata."STRINGIFY_SECTION(val))))
#define KEEP_SECTION_DATA(val) __attribute__((used, section(".data."STRINGIFY_SECTION(val))))
#define KEEP_SECTION_BSS(val) __attribute__((used, section(".bss."STRINGIFY_SECTION(val))))

#define SECTION_FILE_PRAGMA_TEXT(file_name) _Pragma(STRINGIFY_SECTION(clang section text=".text."file_name))
#define SECTION_FILE_PRAGMA_RODATA(file_name) _Pragma(STRINGIFY_SECTION(clang section rodata=".rodata."file_name))
#define SECTION_FILE_PRAGMA_DATA(file_name) _Pragma(STRINGIFY_SECTION(clang section data=".data."file_name))
#define SECTION_FILE_PRAGMA_BSS(file_name) _Pragma(STRINGIFY_SECTION(clang section bss=".bss."file_name))

// Match the source code file
#define SECTION_PRAGMA_TEXT SECTION_FILE_PRAGMA_TEXT(THIS_FILE_NAME)
#define SECTION_PRAGMA_RODATA SECTION_FILE_PRAGMA_RODATA(THIS_FILE_NAME)
#define SECTION_PRAGMA_DATA SECTION_FILE_PRAGMA_DATA(THIS_FILE_NAME)
#define SECTION_PRAGMA_BSS SECTION_FILE_PRAGMA_BSS(THIS_FILE_NAME)

#else

#define AUTO_SECTION_FUNC(func)
#define AUTO_SECTION_RODATA(val)
#define AUTO_SECTION_DATA(val)
#define AUTO_SECTION_BSS(val)

#define KEEP_SECTION_FUNC(func)
#define KEEP_SECTION_RODATA(val)
#define KEEP_SECTION_DATA(val)
#define KEEP_SECTION_BSS(val)

#define SECTION_PRAGMA_TEXT
#define SECTION_PRAGMA_RODATA
#define SECTION_PRAGMA_DATA
#define SECTION_PRAGMA_BSS

#endif

#endif // SECTION_MACROS_H
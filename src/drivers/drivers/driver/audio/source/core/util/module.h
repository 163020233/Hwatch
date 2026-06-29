/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: hifi module init call
 * Author: audio
 */

#ifndef __MODULE_H__
#define __MODULE_H__

typedef void (*initcall_t)(void);
typedef void (*exitcall_t)(void);

#ifdef CONFIG_SAP_RUN_UT
#define module_initcall(fn) \
    static initcall_t __initcall_##fn \
    __attribute__((__used__)) \
    __attribute__((__section__(".init_array"))) = fn;

#define module_exitcall(fn) \
    static exitcall_t __exitcall_##fn \
    __attribute__((__used__)) \
    __attribute__((__section__(".fini_array"))) = fn;

#else
#define module_initcall(fn) \
    static initcall_t __initcall_##fn \
    __attribute__((__used__)) \
    __attribute__((__section__(".initcall"))) = fn;

#define module_exitcall(fn) \
    static exitcall_t __exitcall_##fn \
    __attribute__((__used__)) \
    __attribute__((__section__(".exitcall"))) = fn;
#endif

#endif

/*
  * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
  * Description: frontend fmea module
  * Author: AuthorNameMagicTag
  * Created: 2025-05-24
  */

#ifndef UX_MODULE_INIT_H
#define UX_MODULE_INIT_H
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
file:melody_app.prelds
    __initcall_app_start = ABSOLUTE(.);
    KEEP(*(SORT (.initcall.app.init)))
    __initcall_app_end = ABSOLUTE(.);
*/

#if (defined(__GNUC__) || defined(__clang__))
typedef void (*InitCall)(void);

#define USED_ATTR __attribute__((used))

#define LAYER_INITCALL(func, layer, clayer)            \
    static const InitCall USED_ATTR __initcall_##layer##_##func \
        __attribute__((section(".initcall." clayer ".init"))) = func

#define LAYER_INITCALL_DEF(func, layer, clayer) \
    LAYER_INITCALL(func, layer, clayer)


#define MODULE_BEGIN(name, step)                          \
    ({        extern InitCall __initcall_##name##_start;       \
        InitCall *initCall = &__initcall_##name##_start; \
        (initCall);                                       \
    })
#define MODULE_END(name, step)                          \
    ({        extern InitCall __initcall_##name##_end;       \
        InitCall *initCall = &__initcall_##name##_end; \
        (initCall);                                     \
    })


#define MODULE_CALL(name, step)                                      \
    do {                                                             \
        InitCall *initcall = (InitCall *)(MODULE_BEGIN(name, step)); \
        InitCall *initend = (InitCall *)(MODULE_END(name, step));    \
        for (; initcall < initend; initcall++) {                     \
            (*initcall)();                                           \
        }                                                            \
    } while (0)


#define UX_MODULE_REGISTER(func) LAYER_INITCALL_DEF(func, app, "app")
#define UX_MODULE_INIT()     \
    do {                      \
        MODULE_CALL(app, 0); \
    } while (0)

#define UX_FEATURE_ADD(func) LAYER_INITCALL_DEF(func, ux, "ux")
#define UX_FEATURE_LOAD()     \
    do {                      \
        MODULE_CALL(ux, 0); \
    } while (0)

#else
#error Not support current compiler!
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // UX_MODULE_INIT_H
/** @} */

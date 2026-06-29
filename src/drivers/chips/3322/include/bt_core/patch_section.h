/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: patch section header.
 *
 * Create: 2023-04-13
 */
#ifndef __PATCH_SECTION_H__
#define __PATCH_SECTION_H__

#define PATCH_T __attribute__((section(".patch.text")))
#define PATCH_R __attribute__((section(".patch.rodata")))
#define PATCH_D __attribute__((section(".patch.data")))
#define PATCH_B __attribute__((section(".patch.bss")))

#define EXPORT_HARD_PATCH(old_sym, new_sym) \
    void* old_sym##$patch$##new_sym##_hard __attribute__((section("patch.hard"))) __attribute__((__used__)) = new_sym

#define EXPORT_SOFT_PATCH(sym) \
    void* sym##_soft __attribute__((section("patch.soft"))) __attribute__((__used__)) = sym

#endif

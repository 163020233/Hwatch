//===============================================================================
// @brief    source file
// Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
//===============================================================================

.arch armv8-a

#include "asm/memmap_config.h"
#include "asm/register_config.h"

#define ttbr    x0
#define paddr   x1
#define vaddr   x2
#define size    x3
#define count   x4
#define table   x5
#define index   x6
#define entry   x7
#define items   x8
#define imm3    x9
#define tmp     x10

#define MEM_SIZE_4K   0x1000

#ifdef LOSCFG_ARCH_ARM64_TEMP_PGTABLE_L3
#define MMU_ENTRY_BLOCK_WIDTH    0xFFFFFFFFF000
#define MMU_ENTRY_BLOCK_ATTRS    0x70B
#define MMU_ENTRY_BOLCK_SIZE     0x1000       /* 4k */
#elif LOSCFG_ARCH_ARM64_TEMP_PGTABLE_L2
#define MMU_ENTRY_BLOCK_WIDTH    0xFFFFFFE00000
#define MMU_ENTRY_BLOCK_ATTRS    0x709
#define MMU_ENTRY_BOLCK_SIZE     0x200000     /* 2M */
#else /* LOSCFG_ARCH_ARM64_TEMP_PGTABLE_L1 OR LOSCFG_ARCH_MMU_KERNEL_SPACE */
#define MMU_ENTRY_BLOCK_WIDTH    0xFFFFC0000000
#define MMU_ENTRY_BLOCK_ATTRS    0x709
#define MMU_ENTRY_BOLCK_SIZE     0x40000000   /* 1G */
#endif

#ifndef ARCH_VECTOR_PGT_SIZE
#define ARCH_VECTOR_PGT_SIZE      0
#endif

.align 5
.text
.section ".vectors","ax"

.global create_temp_pgt
.type   create_temp_pgt,function
.global enable_temp_pgt
.type   enable_temp_pgt,function

create_entry:
    mov     imm3, #3
.foreach_mmu:
    lsr     index, vaddr, #30             /* level1, 1G */
    and     index, index, #0x1FF          /* 4kb granule, 9bit per level */
    add     entry, ttbr, index, lsl #3    /* a entry 8bit */

#if defined(LOSCFG_ARCH_ARM64_TEMP_PGTABLE_L2) || defined(LOSCFG_ARCH_ARM64_TEMP_PGTABLE_L3)
    /* Check table */
    ldr     items, [entry]
    bics    table, items, imm3
    cbnz    table, .add_2M

    add     table, ttbr, count, lsl #12   /* a table 4k */
    add     table, table, #0x1000         /* next table */
    add     items, table, #0x3            /* level1 table */
    str     items,[entry]
    add     count, count, #1

.add_2M:
    lsr     index, vaddr, #21             /* level2, 2M */
    and     index, index, #0x1FF          /* 4kb granule, 9bit per level */
    add     entry, table, index, lsl #3   /* a entry 8bit */

#ifdef LOSCFG_ARCH_ARM64_TEMP_PGTABLE_L3
    /* Check table */
    ldr     items, [entry]
    bics    table, items, imm3
    cbnz    table, .add_4KB

    add     table, ttbr, count, lsl #12   /* a table 4k */
    add     table, table, #0x1000         /* next table */
    add     items, table, #0x3            /* level2 table */
    str     items,[entry]
    add     count, count, #1

.add_4KB:
    lsr     index, vaddr, #12             /* level3, 4KB */
    and     index, index, #0x1FF          /* 4kb granule, 9bit per level */
    add     entry, table, index, lsl #3   /* a entry 8bit */
#endif
#endif

    and     items, paddr, #MMU_ENTRY_BLOCK_WIDTH
    add     items, items, #MMU_ENTRY_BLOCK_ATTRS
    str     items,[entry]

    mov     tmp, #MMU_ENTRY_BOLCK_SIZE
    add     vaddr, vaddr, tmp
    add     paddr, paddr, tmp

    subs    size, size, tmp
    b.gt    .foreach_mmu

    ret

clean_mmu:
    add     x2, x0, X1
.foreach_clean:
    str     xzr, [x0], #8
    cmp     x0, x2
    b.ne    .foreach_clean
    ret

create_temp_pgt:
    /* init */
    mov     x19, x30         /* save lr */
    adrp    x0, __page_table_start
    ldr     x1, =ARCH_VECTOR_PGT_SIZE
    bl      clean_mmu
    mov     count, #0
    adrp    ttbr, __ttbr0_temp_pgt

    /* vector, identify map*/
    adrp    paddr, __ram_vectors_lma
    mov     vaddr, paddr
    mov     size, #MEM_SIZE_4K
    bl      create_entry

#ifdef LOSCFG_ARCH_MMU_KERNEL_SPACE
    mov     count, #0

#ifdef LOSCFG_ARCH_ARM64_VA_BITS_48
    adrp    entry, __ttbr1_temp_pgt
    add     ttbr, entry, #0x1000     /* level0, 512G */
    add     items, ttbr, 0x3         /* table */
    str     items,[entry]
#else /*LOSCFG_ARCH_ARM64_VA_BITS_39 */
    adrp    ttbr, __ttbr1_temp_pgt
#endif
#endif
    /* base, swapper */
    adrp    paddr, __ram_vectors_lma
    ldr     vaddr, =reset_vector
    ldr     size, =SYS_MEM_SIZE_DEFAULT
    bl      create_entry

#ifdef LOSCFG_DEBUG_VERSION
    /* uart */
    ldr     paddr, =UART0_REG_BASE
#ifdef LOSCFG_ARCH_MMU_KERNEL_SPACE
    and     paddr, paddr, #((1ULL << LOSCFG_ARCH_ARM64_VA_BITS) - 1)
#endif
    mov     vaddr, paddr
    mov     size, #MEM_SIZE_4K
    bl      create_entry
#endif
    mov     x30, x19          /* restore lr */
    ret

enable_temp_pgt:
    /* Invalidate TLB */
    tlbi    vmalle1is
    isb
    dsb     sy

    /* Initialize Memory Attribute Indirection Register */
    ldr     x9, =0x44FF0400
    msr     mair_el1, x9

    /* Initialize TCR_EL1 */
    /* set cacheable attributes on translation walk */
    /* (SMP extensions) non-shareable, inner write-back write-allocate */
    mov     x9, #0
    orr     x9, x9, #(1 << 38)    /* [38]select low address calculation */
    orr     x9, x9, #(0x1 << 36)  /* [36]asid = 16bits */
    orr     x9, x9, #(0x2 << 32)  /* [34:32]physical address size 1TB */
#ifdef LOSCFG_ARCH_MMU_KERNEL_SPACE
    orr     x9, x9, #(0x1 << 31)  /* [31:30]granule be set as 4K */
    orr     x9, x9, #(0x3 << 28)  /* [29:28]ttbr1:inner shareable */
    orr     x9, x9, #(0x1 << 26)  /* [27:26]ttbr1:outer cacheability: write-back write-allocate */
    orr     x9, x9, #(0x1 << 24)  /* [25:24]ttbr1:inner cacheability: write-back write-allocate */
    mov     tmp, #64              /* [21:16]ttbr1:the size offset of memory region set to 2^(64-VA_BITS) */
    sub     tmp, tmp, #LOSCFG_ARCH_ARM64_VA_BITS
    lsl     tmp, tmp, #16
    orr     x9, x9, tmp
#endif
    orr     x9, x9, #(0x3 << 12)  /* [13:12]ttbr0:inner shareable */
    orr     x9, x9, #(0x1 << 10)  /* [11:10]ttbr0:outer cacheability: write-back write-allocate */
    orr     x9, x9, #(0x1 << 8)   /* [9:8]ttbr0:inner cacheability: write-back write-allocate */
    orr     x9, x9, #(0x10)       /* [5:0]T0SZ the size offset of memory region set to 2^(64-0x19)=512GB */
    orr     x9, x9, #(0x8)        /* [5:0]T0SZ the size offset of memory region set to 2^(64-0x19)=512GB */
    orr     x9, x9, #(0x1)        /* [5:0]T0SZ the size offset of memory region set to 2^(64-0x19)=512GB */

    msr     tcr_el1, x9

    isb

    adrp    x9, __ttbr0_temp_pgt
    msr     ttbr0_el1, x9
#ifdef LOSCFG_ARCH_MMU_KERNEL_SPACE
    adrp    x9, __ttbr1_temp_pgt
    msr     ttbr1_el1, x9
#endif
    isb

    /* Read SCTLR */
    mrs     x9, sctlr_el1

    /* Turn on the MMU */
    orr     x9, x9, #0x1    /* enable mmu */
    /* Write back SCTLR */
    msr     sctlr_el1, x9
    isb

    /* Jump to virtual code address */
    /* Invalidate TLB */
    tlbi    vmalle1
    isb

    ret
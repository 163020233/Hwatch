/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  Basic chip definitions
 * Author:
 * Create:  2018-10-15
 */
#ifndef LIB_COMMON_HEADERS_CHIP_DEFINITIONS_H
#define LIB_COMMON_HEADERS_CHIP_DEFINITIONS_H

/** @defgroup CHIP_Base CHIP Base Definitions
 * CHIP Base Definitions
 * @ingroup CHIP_Base
 * */
/** @defgroup CHIP_ARMCORE CHIP ARM Core Specifics
 * CHIP ARM Core Application Core Specifics
 * @ingroup CHIP_Base
 * @{
 * */
#define BT       0
#define PROTOCOL 1
#define APPS     2
#define GNSS     3
#define SECURITY 4
#define HIFI     PROTOCOL
#define WIFI     5
#define CONTROL_CORE 6
#define SENSOR   7

#define CM3      0
#define CM7      1
#define RISCV31  2
#define RISCV70  3
#define RISCV32  4

#ifndef CHIP_1A71E
#define CHIP_1A71E          1
#endif

#define MASTER_BY_ALL   APPS
#define CORE_NUMS 6 // Total cores in chip
#define CPU_NUM  3    // Total cpus in chip

#define OTHER_CPU_NUM (CPU_NUM - 1) // Other cpus number, exclude current cpu.

#define MASTER_BY_1A71E_ONLY       (CHIP_1A71E && (CORE == APPS))


#define MASTER_ONLY                 (MASTER_BY_1A71E_ONLY)

#define MCU_ONLY                    (MASTER_BY_1A71E_ONLY)


#define SLAVE_BY_1A71E_ONLY       (CHIP_1A71E && (CORE == BT))

#define CHIP_LIBRA_FPGA            (CHIP_LIBRA && (LIBRA_CHIP_FPGA))
#define CHIP_LIBRA_CS              (CHIP_LIBRA && (LIBRA_CHIP_CS))

#ifdef PRE_FPGA
    #define FPGA 1
    #define ASIC 0
    #define CHIP_1A71E_FPGA      (CHIP_1A71E)
    #define CHIP_FPGA             (CHIP_1A71E_FPGA)
#endif

#ifdef PRE_ASIC
    #define FPGA 0
    #define ASIC 1
    #define CHIP_1A71E_ASIC      (CHIP_1A71E)
    #define CHIP_ASIC             (CHIP_1A71E_ASIC)
#endif

/** @} end of group CHIP_ARMCORE */
#endif

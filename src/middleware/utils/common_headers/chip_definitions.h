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
#define CCORE    SECURITY // cooperate core

#define CM3      0
#define CM7      1
#define RISCV31  2
#define RISCV70  3
#define RISCV32  4
#define HIFI3Z   5

#define CHIP_LIBRA           (TARGET_CHIP_LIBRA)
#define CHIP_SOCMN1          (TARGET_CHIP_SOCMN1)
#define CHIP_BS25            (TARGET_CHIP_BS25)
#define CHIP_BRANDY          (TARGET_CHIP_BRANDY)
#define CHIP_WUXING          (TARGET_CHIP_WUXING)
#define CHIP_SW39            (TARGET_CHIP_SW39)
#define CHIP_SW21            (TARGET_CHIP_SW21)
#define CHIP_SOCMN2          (TARGET_CHIP_SOCMN2)
#define CHIP_3322            (TARGET_CHIP_3322)
#define CHIP_SW22            (TARGET_CHIP_SW22)
#define CHIP_1A72            (TARGET_CHIP_1A72)
#define CHIP_1A71E           (TARGET_CHIP_1A71E)


#if (defined(CHIP_LIBRA) && (CHIP_LIBRA == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 5 // Total cores in chip
#define CPU_NUM  3    // Total cpus in chip
#elif (defined(CHIP_SOCMN1) && (CHIP_SOCMN1 == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 4 // Total cores in chip
#define CPU_NUM  3    // Total cpus in chip
#elif (defined(CHIP_BS25) && (CHIP_BS25 == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 2 // Total cores in chip
#define CPU_NUM  2    // Total cpus in chip
#elif (defined(CHIP_BS27A) && (CHIP_BS27A == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 2 // Total cores in chip
#define CPU_NUM  2    // Total cpus in chip
#elif ((defined(CHIP_BS20) && (CHIP_BS20 == 1)) || (defined(CHIP_BS21) && (CHIP_BS21  == 1)) || (defined(CHIP_BS21A) && (CHIP_BS21A  == 1)) || \
       (defined(CHIP_BS21E) && (CHIP_BS21E == 1)) || (defined(CHIP_BS22) && (CHIP_BS22 == 1)) || (defined(CHIP_BS26) && (CHIP_BS26 == 1)))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 1 // Total cores in chip
#define CPU_NUM  1    // Total cpus in chip
#elif (defined(CHIP_WS53) && (CHIP_WS53 == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 2 // Total cores in chip
#define CPU_NUM  2    // Total cpus in chip
#elif defined(CONFIG_CHIP_WS75) && (CONFIG_CHIP_WS75 == 1)
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 2
#define CPU_NUM  2
#elif (defined(CHIP_WS63) && (CHIP_WS63 == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 1 // Total cores in chip
#define CPU_NUM  1    // Total cpus in chip
#elif (defined(CHIP_BRANDY) && (CHIP_BRANDY == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 3 // Total cores in chip
#define CPU_NUM  3    // Total cpus in chip
#elif (defined(CHIP_WUXING) && (CHIP_WUXING == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 1 // Total cores in chip
#define CPU_NUM  1    // Total cpus in chip
#elif (defined(CHIP_SW39) && (CHIP_SW39 == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 6 // Total cores in chip
#define CPU_NUM  3    // Total cpus in chip
#elif (defined(CHIP_1A71E) && (CHIP_1A71E == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 6 // Total cores in chip
#define CPU_NUM  3    // Total cpus in chip
#elif (defined(CHIP_CAT1) && (CHIP_CAT1 == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 2 // Total cores in chip
#define CPU_NUM  2    // Total cpus in chip
#elif (defined(CHIP_SW21) && (CHIP_SW21 == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 5 // Total cores in chip
#define CPU_NUM  5    // Total cpus in chip
#elif (defined(CHIP_SOCMN2) && (CHIP_SOCMN2 == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 3 // Total cores in chip
#define CPU_NUM  3    // Total cpus in chip
#elif (defined(CHIP_3322) && (CHIP_3322 == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 3 // Total cores in chip
#define CPU_NUM  3    // Total cpus in chip
#elif (defined(CHIP_FHL2) && (CHIP_FHL2 == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 1 // Total cores in chip
#define CPU_NUM  1    // Total cpus in chip
#elif (defined(CHIP_SW22) && (CHIP_SW22 == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 7 // Total cores in chip
#define CPU_NUM  7    // Total cpus in chip
#elif (defined(CHIP_USW) && (CHIP_USW == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 1 // Total cores in chip
#define CPU_NUM  1    // Total cpus in chip
#elif (defined(CHIP_1A72) && (CHIP_1A72 == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 6 // Total cores in chip
#define CPU_NUM  3    // Total cpus in chip
#elif (defined(CHIP_2132) && (CHIP_2132 == 1))
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 2 // Total cores in chip
#define CPU_NUM  1    // Total cpus in chip
#else
#error Please define master control core
#endif

#define OTHER_CPU_NUM (CPU_NUM - 1) // Other cpus number, exclude current cpu.

#define MASTER_BY_LIBRA_ONLY        (CHIP_LIBRA && (CORE == APPS))
#define MASTER_BY_SOCMN1_ONLY       (CHIP_SOCMN1 && (CORE == APPS))
#define MASTER_BY_BS25_ONLY         (CHIP_BS25 && (CORE == APPS))
#define MASTER_BY_BS27A_ONLY        (CHIP_BS27A && (CORE == APPS))
#define MASTER_BY_BS20_ONLY         (CHIP_BS20 && (CORE == APPS))
#define MASTER_BY_BS21_ONLY         (CHIP_BS21 && (CORE == APPS))
#define MASTER_BY_BS21E_ONLY        (CHIP_BS21E && (CORE == APPS))
#define MASTER_BY_BS21A_ONLY        (CHIP_BS21A && (CORE == APPS))
#define MASTER_BY_BS22_ONLY         (CHIP_BS22 && (CORE == APPS))
#define MASTER_BY_BS26_ONLY         (CHIP_BS26 && (CORE == APPS))
#define MASTER_BY_BRANDY_ONLY       (CHIP_BRANDY && (CORE == APPS))
#define MASTER_BY_WS53_ONLY         (CHIP_WS53 && (CORE == APPS))
#define MASTER_BY_SW39_ONLY         (CHIP_SW39 && (CORE == APPS))
#define MASTER_BY_SW21_ONLY         (CHIP_SW21 && (CORE == APPS))
#define MASTER_BY_3322_ONLY         (CHIP_3322 && (CORE == APPS))
#define MASTER_BY_SOCMN2_ONLY       (CHIP_SOCMN2 && (CORE == APPS))
#define MASTER_BY_SW22_ONLY         (CHIP_SW22 && (CORE == APPS))
#define MASTER_BY_1A72_ONLY         (CHIP_1A72 && (CORE == APPS))
#define MASTER_BY_1A71E_ONLY        (CHIP_1A71E && (CORE == APPS))

#define MASTER_ONLY                 (MASTER_BY_LIBRA_ONLY || MASTER_BY_SOCMN2_ONLY || \
                                     MASTER_BY_SOCMN1_ONLY || MASTER_BY_BS25_ONLY || \
                                     MASTER_BY_BRANDY_ONLY || MASTER_BY_BS20_ONLY || \
                                     MASTER_BY_BS21_ONLY || MASTER_BY_BS21A_ONLY || \
                                     MASTER_BY_BS22_ONLY || MASTER_BY_BS26_ONLY || \
                                     MASTER_BY_WS53_ONLY || MASTER_BY_SW39_ONLY || \
                                     MASTER_BY_SW21_ONLY || MASTER_BY_BS21E_ONLY || \
                                     MASTER_BY_3322_ONLY || MASTER_BY_SW22_ONLY || \
                                     MASTER_BY_1A72_ONLY || MASTER_BY_1A71E_ONLY)

#define MCU_ONLY                    (MASTER_BY_LIBRA_ONLY || MASTER_BY_SOCMN1_ONLY || \
                                     MASTER_BY_BS25_ONLY || MASTER_BY_BRANDY_ONLY || MASTER_BY_BS27A_ONLY || \
                                     MASTER_BY_WS53_ONLY || CHIP_WS63 || CHIP_BS20 || CHIP_BS21 || CHIP_BS21E || \
                                     CHIP_BS21A || CHIP_BS22 || CHIP_BS26 || MASTER_BY_SW39_ONLY || \
                                     MASTER_BY_SW21_ONLY || MASTER_BY_SOCMN2_ONLY || MASTER_BY_3322_ONLY || \
                                     MASTER_BY_SW22_ONLY || MASTER_BY_1A72_ONLY || MASTER_BY_1A71E_ONLY)


#define SLAVE_BY_LIBRA_BT          (CHIP_LIBRA && (CORE == BT))
#define SLAVE_BY_LIBRA_GNSS        (CHIP_LIBRA && (CORE == GNSS))
#define SLAVE_BY_LIBRA_SECURITY    (CHIP_LIBRA && (CORE == SECURITY))
#define SLAVE_BY_LIBRA_ONLY        (SLAVE_BY_LIBRA_BT || SLAVE_BY_LIBRA_GNSS || SLAVE_BY_LIBRA_SECURITY)
#define SLAVE_BY_SOCMN1_ONLY       (CHIP_SOCMN1 && (CORE == BT))
#define SLAVE_BY_SW39_BT           (CHIP_SW39 && (CORE == BT))
#define SLAVE_BY_SOCMN2_ONLY       (CHIP_SOCMN2 && (CORE == BT))
#define SLAVE_BY_BS25_ONLY         (CHIP_BS25 && (CORE == BT))
#define SLAVE_BY_BRANDY_BT         (CHIP_BRANDY && (CORE == BT))
#define SLAVE_BY_BRANDY_DSP        (CHIP_BRANDY && (CORE == DSP))
#define SLAVE_BY_BRANDY_ONLY       (SLAVE_BY_BRANDY_BT || SLAVE_BY_BRANDY_DSP)
#define SLAVE_BY_WS53_ONLY         (CHIP_WS53 && (CORE == CONTROL_CORE))
#define SLAVE_BY_1A72_BT           (CHIP_1A72 && (CORE == BT))
#define SLAVE_BY_1A71E_BT          (CHIP_1A71E && (CORE == BT))

#define CHIP_LIBRA_FPGA            (CHIP_LIBRA && (LIBRA_CHIP_FPGA))
#define CHIP_LIBRA_CS              (CHIP_LIBRA && (LIBRA_CHIP_CS))

#define CHIP_SOCMN1_FPGA             (CHIP_SOCMN1 && (SOCMN1_CHIP_FPGA))
#define CHIP_SOCMN1_V100             (CHIP_SOCMN1 && (SOCMN1_CHIP_V100))
#define CHIP_SOCMN1_V200             (CHIP_SOCMN1 && (SOCMN1_CHIP_V200))

#define CHIP_BS25_FPGA              (CHIP_BS25 && (BS25_CHIP_FPGA))
#define CHIP_BS25_V100              (CHIP_BS25 && (BS25_CHIP_V100))

#ifdef PRE_FPGA
    #define FPGA 1
    #define ASIC 0
#endif

#ifdef PRE_ASIC
    #define FPGA 0
    #define ASIC 1
#endif

#define CHIP_BS20_FPGA          (CHIP_BS20 && (FPGA))
#define CHIP_BS20_V100          (CHIP_BS20 && (ASIC))

#define CHIP_BS21_FPGA          (CHIP_BS21 && (FPGA))
#define CHIP_BS21_V100          (CHIP_BS21 && (ASIC))

#define CHIP_BS21E_FPGA          (CHIP_BS21E && (FPGA))
#define CHIP_BS21E_V100          (CHIP_BS21E && (ASIC))

#define CHIP_BS21A_FPGA          (CHIP_BS21A && (FPGA))
#define CHIP_BS21A_V100          (CHIP_BS21A && (ASIC))

#define CHIP_BS22_FPGA          (CHIP_BS22 && (FPGA))
#define CHIP_BS22_V100          (CHIP_BS22 && (ASIC))

#define CHIP_BS26_FPGA          (CHIP_BS26 && (FPGA))
#define CHIP_BS26_V100          (CHIP_BS26 && (ASIC))

#define CHIP_BS27A_FPGA          (CHIP_BS27A && (FPGA))
#define CHIP_BS27A_V100          (CHIP_BS27A && (ASIC))

#define CHIP_WS63_FPGA          (CHIP_WS63 && (FPGA))
#define CHIP_WS63_V100          (CHIP_WS63 && (ASIC))

#define CHIP_BRANDY_FPGA       (CHIP_BRANDY && (BRANDY_CHIP_FPGA))
#define CHIP_BRANDY_V100       (CHIP_BRANDY && (BRANDY_CHIP_V100))

#define CHIP_SW39_FPGA         (CHIP_SW39 && (FPGA))
#define CHIP_SW39_V100         (CHIP_SW39 && (ASIC))

#define CHIP_SOCMN2_FPGA       (CHIP_SOCMN2 && (FPGA))
#define CHIP_SOCMN2_ASIC       (CHIP_SOCMN2 && (ASIC))

#define CHIP_SW21_FPGA         (CHIP_SW21 && (SW21_CHIP_FPGA))
#define CHIP_SW21_V100         (CHIP_SW21 && (SW21_CHIP_V100))

#define CHIP_3322_FPGA         (CHIP_3322 && (HI3322_CHIP_FPGA))
#define CHIP_3322_V100         (CHIP_3322 && (HI3322_CHIP_V100))

#define CHIP_WS53_FPGA         (CHIP_WS53 && (FPGA))
#define CHIP_WS53_V100          (CHIP_WS53 && (ASIC))

#define CHIP_SW22_FPGA         (CHIP_SW22 && (SW22_CHIP_FPGA))
#define CHIP_SW22_V100         (CHIP_SW22 && (SW22_CHIP_V100))

#define CHIP_1A72_FPGA         (CHIP_1A72 && (FPGA))
#define CHIP_1A72_V100         (CHIP_1A72 && (ASIC))

#define CHIP_1A71E_FPGA        (CHIP_1A71E && (FPGA))
#define CHIP_1A71E_V100        (CHIP_1A71E && (ASIC))

#define CHIP_FPGA             (CHIP_LIBRA_FPGA || CHIP_SOCMN1_FPGA || CHIP_BS25_FPGA || CHIP_SW39_FPGA || \
                               CHIP_BRANDY_FPGA || CHIP_BS20_FPGA || CHIP_BS21_FPGA || CHIP_BS21A_FPGA || \
                               CHIP_BS27A_FPGA || \
                               CHIP_BS22_FPGA || CHIP_BS26_FPGA || CHIP_WS63_FPGA || CHIP_WS53_FPGA || \
                               CHIP_SW21_FPGA || CHIP_BS21E_FPGA || CHIP_SOCMN2_FPGA || CHIP_3322_FPGA || \
                               CHIP_SW22_FPGA || CHIP_1A72_FPGA || CHIP_1A71E_FPGA)

#define CHIP_SOCMN1_ASIC       (CHIP_SOCMN1_V100 || CHIP_SOCMN1_V200)
#define CHIP_BS25_ASIC        (CHIP_BS25_V100)
#define CHIP_BS20_ASIC        (CHIP_BS20_V100)
#define CHIP_BS21_ASIC        (CHIP_BS21_V100)
#define CHIP_BS21E_ASIC        (CHIP_BS21E_V100)
#define CHIP_BS21A_ASIC        (CHIP_BS21A_V100)
#define CHIP_BS22_ASIC        (CHIP_BS22_V100)
#define CHIP_BS26_ASIC        (CHIP_BS26_V100)
#define CHIP_BS27A_ASIC        (CHIP_BS27A_V100)
#define CHIP_BRANDY_ASIC      (CHIP_BRANDY_V100)
#define CHIP_WS63_ASIC        (CHIP_WS63_V100)
#define CHIP_WS53_ASIC        (CHIP_WS53_V100)
#define CHIP_SW21_ASIC        (CHIP_SW21_V100)
#define CHIP_SW39_ASIC        (CHIP_SW39_V100)
#define CHIP_3322_ASIC        (CHIP_3322_V100)
#define CHIP_SW22_ASIC        (CHIP_SW22_V100)
#define CHIP_1A72_ASIC        (CHIP_1A72_V100)
#define CHIP_1A71E_ASIC       (CHIP_1A71E_V100)

#define CHIP_ASIC             (CHIP_LIBRA_CS || CHIP_SOCMN1_ASIC || CHIP_SOCMN2_ASIC || \
                               CHIP_BS25_ASIC || CHIP_BRANDY_ASIC || \
                               CHIP_BS20_ASIC || CHIP_BS21_ASIC || \
                               CHIP_BS21A_ASIC || CHIP_BS22_ASIC || \
                               CHIP_BS26_ASIC || CHIP_WS63_ASIC || \
                               CHIP_WS53_ASIC || CHIP_SW21_ASIC || \
                               CHIP_BS21E_ASIC || CHIP_SW39_ASIC || \
                               CHIP_3322_ASIC || CHIP_BS27A_ASIC || \
                               CHIP_SW22_ASIC || CHIP_1A72_ASIC || \
                               CHIP_1A71E_ASIC)

/** @} end of group CHIP_ARMCORE */
#endif

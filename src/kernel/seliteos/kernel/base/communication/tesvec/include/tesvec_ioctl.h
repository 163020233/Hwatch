/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: tesvec ioctl header for SELiteOS BOX.
 */

#ifndef TESVEC_IOCTL_H
#define TESVEC_IOCTL_H

#include <errcode.h>

#define IOC_TYPE_TESVEC 't'
#define IOC_TYPE_CHIPS_PORTING 'a'
#define IOC_TYPE_KERNEL_TEST 's'
#define IOC_TYPE_PSA_SERVICE 'p'
#define IOC_TYPE_PSA_SERVICE_ENHANCE 'e'

typedef enum {
    TESVEC_GP_COMM_NR,
    TESVEC_IOC_MAX_NR,
} tesvec_ioc_nr_t;

typedef struct gp_comm_arg {
    void *arg;
    uint32_t param_nums;
} gp_comm_arg_t;

#define TESVEC_IOC_NA 0U
#define TESVEC_IOC_W  1U
#define TESVEC_IOC_R  2U
#define TESVEC_IOC_RW 3U

/* Ioctl definitions */
/*
 * dir(direction):ioctl 命令访问模式（数据传输方向），占据 2 bit，可以为 TESVEC_IOC_NA、TESVEC_IOC_R、TESVEC_IOC_W、TESVEC_IOC_RW
 * 分别指示了四种访问模式：无数据、读数据、写数据、读写数据;
 * size:涉及到 ioctl 函数第三个参数 arg ，占据14bit，指定了 arg 的数据类型及长度;
 * type(device type):设备类型，占据 8 bit，可以为任意 char 型字符，例如‘a’、’b’、’c’ 等等，其主要作用是使 ioctl 命令有唯一的设备标识;
 * nr(number):命令编号/序数，占据 8 bit，可以为任意 unsigned char 型数据，取值范围 0~255，如果定义了多个 ioctl 命令，通常从 0 开始编号递增;
 */
#define TESVEC_IOC(dir, type, nr, size) (((dir) << 30) | ((size) << 16) | ((type) << 8) | ((nr) << 0))

#define TESVEC_IO(nr)                TESVEC_IOC(TESVEC_IOC_NA, IOC_TYPE_TESVEC, (nr), 0)
#define TESVEC_IOWR(nr, struct_name) TESVEC_IOC(TESVEC_IOC_RW, IOC_TYPE_TESVEC, (nr), sizeof(struct_name))
#define TESVEC_IOW(nr, struct_name)  TESVEC_IOC(TESVEC_IOC_W, IOC_TYPE_TESVEC, (nr), sizeof(struct_name))

#define TESVEC_IOC_DIR(cmd)  (((cmd) >> 30) & 0x03)
#define TESVEC_IOC_TYPE(cmd) (((cmd) >> 8) & 0xFF)
#define TESVEC_IOC_NR(cmd)   (((cmd) >> 0) & 0xFF)
#define TESVEC_IOC_SIZE(cmd) (((cmd) >> 16) & 0x3FFF)

#define CMD_GP_COMM                 TESVEC_IOWR(TESVEC_GP_COMM_NR, gp_comm_arg_t)

// kernel_test entry
#define KERNEL_TEST_IOWR(nr, struct_name) TESVEC_IOC(TESVEC_IOC_RW, IOC_TYPE_KERNEL_TEST, (nr), sizeof(struct_name))

errcode_t tesvec_ioctl(uint32_t cmd, void *arg);

#endif

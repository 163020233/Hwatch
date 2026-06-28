/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2024. All rights reserved.
 * Description: USB dfu main
 * Author: HiSilicon
 * Create:  2025
 */

#ifndef USB_DFU_H
#define USB_DFU_H

#include "stdint.h"
#include "errcode.h"

#define EXT_ERR_SUCCESS 0          /* !< succes */
#define EXT_ERR_FAILURE 0xFFFFFFFF /* !< failure */
// frame type, ref cmd_loop.h.
enum {
    FRAME_TYPE_PROVISION = 0xE1,
    FRAME_TYPE_FLASH = 0xD2,
    FRAME_TYPE_EFUSE = 0xC3,
    FRAME_TYPE_CMD_RESET = 0x87,
    FRAME_TYPE_KV = 0x4B,
    FRAME_TYPE_CMD_EXIT_BURNING = 0x1E,
};

enum boot_usb_event_e {
    BOOT_USB_FINISHED = 0,
    BOOT_USB_RUNING = 1,
    BOOT_USB_ERROR,
    BOOT_USB_TIMEOUT,
    BOOT_USB_PARSE_HEADER_ERROR,
    BOOT_USB_MEM_CPY_ERROR,
    BOOT_USB_FILE_LEN_ERROR,
    BOOT_USB_INIT,
    BOOT_USB_CLR_TIMEOUT,
    BOOT_USB_DELAY_FINISHED, // 多烧写场景需要板子延时重启
};

struct flash_erase_info {
    uint32_t addr;
    uint32_t size;
};

/**
 * @if Eng
 * @brief  General structure of the header of a DFU download file.
 * @else
 * @brief  DFU下载文件文件头通用结构。
 * @endif
 */
struct dfu_file_header_common_info {
    uint32_t start_flag;        /* !< @if Eng DFU File transfer start flag
                                 * @else   DFU文件传输开始标识位
                                 * @endif */
    uint16_t packet_size;       /* !< @if Eng DFU file size
                                 * @else   DFU文件大小
                                 * @endif */
    uint8_t frame_type;         /* !< @if Eng DFU Download File Type
                                 * - 0xE1: Downloading provision
                                 * - 0xD2: Downloading the Flash Image
                                 * - 0xC3: Downloading the Efuse Image
                                 * - 0x87: Reset Command
                                 * @else   DFU下载文件类型
                                 * - 0xE1: 下载provision
                                 * - 0xD2: 下载flash镜像
                                 * - 0xC3: 下载efuse镜像
                                 * - 0x87: 复位命令
                                 * @endif */
    uint8_t frame_type_reverse; /* !< @if Eng frame_type reverse val
                                 * @else   frame_type 翻转值
                                 * @endif */
} __attribute__((packed));

/**
 * @if Eng
 * @brief  Header structure of the efuse image downloaded by the DFU.
 * @else
 * @brief  DFU下载efuse文件文件头结构。
 * @endif
 */
struct dfu_file_header_efuse_info {
    struct dfu_file_header_common_info common_info; /* !< @if Eng Common header of a DFU file
                                                     * @else   DFU文件通用文件头
                                                     * @endif */
    uint32_t file_len;                              /* !< @if Eng Flash image size
                                                     * @else   镜像大小
                                                     * @endif */
    uint16_t check_sum;                             /* !< @if Eng Checksum
                                                     * @else   文件校验和
                                                     * @endif */
} __attribute__((packed));

/**
 * @if Eng
 * @brief  Header structure of the provision image downloaded by the DFU.
 * @else
 * @brief  DFU下载provision文件文件头结构。
 * @endif
 */
struct dfu_file_header_provision_info {
    struct dfu_file_header_common_info common_info; /* !< @if Eng Common header of a DFU file
                                                     * @else   DFU文件通用文件头
                                                     * @endif */
    uint32_t file_len;                              /* !< @if Eng Flash image size
                                                     * @else   镜像大小
                                                     * @endif */
    uint16_t check_sum;                             /* !< @if Eng Checksum
                                                     * @else   文件校验和
                                                     * @endif */
} __attribute__((packed));

/**
 * @if Eng
 * @brief  Header structure of the flash image downloaded by the DFU.
 * @else
 * @brief  DFU下载Flash镜像文件头结构。
 * @endif
 */
struct dfu_file_header_flash_info {
    struct dfu_file_header_common_info common_info; /* !< @if Eng Common header of a DFU file
                                                     * @else   DFU文件通用文件头
                                                     * @endif */
    uint32_t file_addr;                             /* !< @if Eng Physical address for burning the flash image
                                                     * @else   flash镜像烧录的物理地址
                                                     * @endif */
    uint32_t file_len;                              /* !< @if Eng Flash image size
                                                     * @else   flash镜像大小
                                                     * @endif */
    uint32_t erase_size;                            /* !< @if Eng Size of the area to be erased from the flash memory
                                                     * @else   flash擦除区域大小
                                                     * @endif */
    uint8_t formal;                                 /* !< @if Eng Flash image type
                                                     * @else   flash镜像类型
                                                     * @endif */
    uint8_t formal_reveers;                         /* !< @if Eng formal reveers value
                                                     * @else   formal翻转值
                                                     * @endif */
    uint16_t check_sum;                             /* !< @if Eng Checksum
                                                     * @else   文件校验和
                                                     * @endif */
} __attribute__((packed));

/**
 * @if Eng
 * @brief  Header structure of the efuse image downloaded by the DFU.
 * @else
 * @brief  DFU下载kv文件文件头结构。
 * @endif
 */
struct dfu_file_header_kv_info {
    struct dfu_file_header_common_info common_info; /* !< @if Eng Common header of a DFU file
                                                     * @else   DFU文件通用文件头
                                                     * @endif */
    uint32_t file_addr;                             /* !< @if Eng Physical address for burning the kv image
                                                     * @else   镜像烧录的物理地址
                                                     * @endif */
    uint32_t file_len;                              /* !< @if Eng Kv image size
                                                     * @else   镜像大小
                                                     * @endif */
    uint32_t erase_size;                            /* !< @if Eng Size of the area to be erased from the memory
                                                     * @else   擦除区域大小
                                                     * @endif */
    uint16_t enc_item_cnt;                          /* !< @if Eng Encryted item count
                                                     * @else   加密项目个数
                                                     * @endif */
    uint16_t flag;                                  /* !< @if Eng Burn mode flag
                                                     * @else   烧写模式标志
                                                     * @endif */
    uint16_t check_sum;                             /* !< @if Eng Checksum
                                                     * @else   文件校验和
                                                     * @endif */
} __attribute__((packed));

/**
 * @if Eng
 * @brief  Header structure of the provision image downloaded by the DFU.
 * @else
 * @brief  DFU下载Command文件文件头结构。
 * @endif
 */
struct dfu_file_header_command_info {
    struct dfu_file_header_common_info common_info; /* !< @if Eng Common header of a DFU file
                                                     * @else   DFU文件通用文件头
                                                     * @endif */
    uint16_t param;                                 /* !< @if Eng command parameter
                                                     * @else   命令参数
                                                     * @endif */
    uint16_t check_sum;                             /* !< @if Eng Checksum
                                                     * @else   文件校验和
                                                     * @endif */
} __attribute__((packed));

/**
 * @if Eng
 * @brief  DFU status indication structure.
 * @else
 * @brief  DFU状态指示结构。
 * @endif
 */
typedef struct boot_usb_dfu_entity_s {
    uint32_t trans_complete; /* !< @if Eng DFU transmission status indication
                              * - 0x0: Transmission not completed
                              * - 0x1: Transfer Complete
                              * @else   DFU传输状态指示
                              * - 0x0: 未传输完成
                              * - 0x1: 传输完成
                              * @endif */
} boot_usb_dfu_entity_t;

int bootrom_usb_plug(int plug);
void usb_dfu_init(void);
void usb_dfu_deinit(void);
uint16_t crc16_ccitt(uint16_t crc_start, uint8_t *buf, uint32_t len);
uint8_t get_provision_usb_dfu_frame_type(void);
uint8_t boot_usb_controllor_get_status(void);
void boot_usb_controllor_set_status(enum boot_usb_event_e status);
void boot_reset(void);
void set_dfu_trans_flag(uint8_t status);
// ROM里边已经初始化phy & usbc, 只有uart+usb场景才需要调用这两个函数
void usbc_init(void);
void usb_phy_init(void);
#endif
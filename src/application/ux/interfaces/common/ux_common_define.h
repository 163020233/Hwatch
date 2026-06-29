/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux common define \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#ifndef UX_COMMON_DEFINE_H
#define UX_COMMON_DEFINE_H

#define UX_UINT64_BYTE_NUM 8                              /**< 64位整数字节数 */
 
#define UX_BIT_INDEX_56 56                                /**< 第56位索引 */
#define UX_BIT_INDEX_48 48                                /**< 第48位索引 */
#define UX_BIT_INDEX_40 40                                /**< 第40位索引 */
#define UX_BIT_INDEX_32 32                                /**< 第32位索引 */
#define UX_BIT_INDEX_24 24                                /**< 第24位索引 */
#define UX_BIT_INDEX_16 16                                /**< 第16位索引 */
#define UX_BIT_INDEX_8   8                                /**< 第8位索引 */
 
#define UX_BYTE_INDEX_0 0                                 /**< 字节索引0 */
#define UX_BYTE_INDEX_1 1                                 /**< 字节索引1 */
#define UX_BYTE_INDEX_2 2                                 /**< 字节索引2 */
#define UX_BYTE_INDEX_3 3                                 /**< 字节索引3 */
#define UX_BYTE_INDEX_4 4                                 /**< 字节索引4 */
#define UX_BYTE_INDEX_5 5                                 /**< 字节索引5 */
#define UX_BYTE_INDEX_6 6                                 /**< 字节索引6 */
#define UX_BYTE_INDEX_7 7                                 /**< 字节索引7 */
#define UX_BYTE_INDEX_8 8                                 /**< 字节索引8 */

#define UX_BG_ADDR_LEN 6                                  /**< 后台地址长度 */
#define UX_ADDR_IDX_0 0                                   /**< 地址索引0 */
#define UX_ADDR_IDX_1 1                                   /**< 地址索引1 */
#define UX_ADDR_IDX_2 2                                   /**< 地址索引2 */
#define UX_ADDR_IDX_3 3                                   /**< 地址索引3 */
#define UX_ADDR_IDX_4 4                                   /**< 地址索引4 */
#define UX_ADDR_IDX_5 5                                   /**< 地址索引5 */
#define UX_CHAR_NUM 2                                     /**< 字符数量 */
#define UX_BASE16 16                                      /**< 十六进制基数 */
#define UX_DEVICE_NAME_LEN 64                             /**< 设备名称最大长度 */
#define UX_BS_ADDR_STR_LEN 12                             /**< 星闪地址字符串长度 */
#define UX_PEER_PAIR_CODE_LEN 4                           /**< 对耳配对码长度 */
#define UX_BIT_INDEX_0 0                                  /**< 第0位索引 */
#define UX_UINT8_BYTE_NUM 1                               /**< 8位整数字节数 */
#define UX_UINT16_BYTE_NUM 2                              /**< 16位整数字节数 */
#define UX_UINT32_BYTE_NUM 4                              /**< 32位整数字节数 */
#define UX_POINTER_SIZE 4                                 /**< 指针大小 */
#define UX_BS_ADDR_SIZE 6                                 /**< 星闪地址大小 */
#define UX_ANNOUNCE_DATA_RESERVED_LEN 8                   /**< 广播数据保留长度 */
#define UX_VENDOR_INFO_MAX_LEN 64                         /**< 厂商信息最大长度 */
#define UX_ANNOUNCE_VENDOR_INFO_MAX_LEN 66                /**< 广播厂商信息最大长度 */
#define UX_BS_SLE_ANNOUNCE_DATA_MAX_LEN 68                /**< 星闪广播数据最大长度 */
#define UX_BS_SLE_ANNOUNCE_TLV_DATA_MIN_LEN 6             /**< 星闪广播TLV数据最小长度 */
#define UX_BS_SLE_ANNOUNCE_TLV_DATA_MAX_LEN 78            /**< 星闪广播TLV数据最大长度 */
#define UX_BS_SLE_ANNOUNCE_FILTER_MAX_LEN 24              /**< 星闪广播过滤器最大长度 */
#define UX_BYTE_MAX_VALUE 0xFF                            /**< 字节最大值 */
#define UX_DEFAULT_COMPANY_ID 0x01                        /**< 默认公司ID */
#define DEV_CONN_MIN_TIME 30000                           /**< 设备连接最小时间 */
#define DEV_CONN_MAX_TIME 1800000                         /**< 设备连接最大时间 */
#define DEV_RECON_MIN_TIME 30000                          /**< 设备重连最小时间 */
#define DEV_RECON_MAX_TIME 1800000                        /**< 设备重连最大时间 */
#define PEER_RECON_MIN_TIME 10000                         /**< 对耳重连最小时间 */
#define PEER_RECON_MAX_TIME 1800000                       /**< 对耳重连最大时间 */
#define UX_DOWNLINK_VOLUME_LEVEL_MAX 31                   /**< 下行音量等级最大值 */
#define UX_HEX_NUM 16                                     /**< 十六进制数字数量 */
#define UX_MS_IN_ONE_SECOND 1000                          /**< 每秒毫秒数 */
#define PD_NUM_LEN_MAX 156                                /**< 配对设备列表最大长度 */
#define UX_INDEX_0 0                                      /**< 索引0 */
#define UX_INDEX_1 1                                      /**< 索引1 */
#define UX_INDEX_2 2                                      /**< 索引2 */
#define UX_INDEX_3 3                                      /**< 索引3 */
#define UX_INDEX_4 4                                      /**< 索引4 */
#define UX_INDEX_5 5                                      /**< 索引5 */
#define UX_INDEX_14 14                                    /**< 索引14 */
#define UX_INDEX_15 15                                    /**< 索引15 */
#define UX_ONE_BYTE_LEN 1                                 /**< 单字节长度 */
#define UX_UINT16_MAX 0xffff                              /**< 16位无符号整数最大值 */
#define UX_BS_SLE_CDS_KEY_INFO_LEN 32
#define UX_STATUS_READY 1                                 /**< 状态就绪 */
#define UX_STATUS_NOT_READY 0                             /**< 状态未就绪 */
#define UX_RESERVED_LEN_THR 3                             /**< 保留3个字节长度 */
#define UX_CONNECTED_DEV_NUM_0 0                          /**< 已连接0个设备 */
#define UX_CONNECTED_DEV_NUM_1 1                          /**< 已连接1个设备 */
#define UX_CONNECTED_DEV_NUM_2 2                          /**< 已连接2个设备 */
#define UX_STATUS_NOT_READY 0                             /**< 状态未就绪 */
#define UX_INVALID_DEVICE_ID 0xff

#endif

/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Function headers of different chips in the UPG module.
 */

#ifndef UPG_ADAPTION_H
#define UPG_ADAPTION_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "partition.h"
#include "upg_definitions.h"
#include "upg.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* Adaption function for UPG common */
/* 获取镜像ID和分区ID的map表
 * map: 镜像ID和分区ID的映射表
 * 返回map表中映射数量
 */
uint32_t upg_get_ids_map(upg_image_partition_ids_map_t **map);
/* 获取当前程序支持的升级镜像 */
upg_image_collections_t *uapi_upg_get_image_id_collection(void);
/* 获取升级包路径 */
char *upg_get_pkg_file_path(void);
/* 获取升级包所在目录 */
char *upg_get_pkg_file_dir(void);
/* 获取资源索引路径 */
char *upg_get_res_file_index_path(void);

/*
 * 获取FOTA升级标记区的Flash起始地址，该地址为在flash上的相对地址，是相对flash基地址的偏移
 * start_address 返回升级标记区的起始地址
 */
errcode_t upg_get_upgrade_flag_flash_start_addr(uint32_t *start_address);

/*
 * 获取2131标记区的Flash起始地址，该地址为在flash上的相对地址，是相对flash基地址的偏移
 * start_address 返回升级标记区的起始地址
 */
errcode_t upg_get_slave_chip_upgrade_flag_flash_start_addr(uint32_t *start_address);
/*
 * 获取FOTA升级进度恢复标记区的Flash起始地址，该地址为在flash上的相对地址，是相对flash基地址的偏移
 * start_address 返回标记区的起始地址
 */
errcode_t upg_get_progress_status_start_addr(uint32_t *start_address, uint32_t *size);

 /*
 * 如果升级包保存在flash上，FOTA分区为如下结构：
 * ---------------------
 *    升级包存储区
 * --------------------
 *    缓存区（4K）（可选）
 * --------------------
 *    写入状态区（4K）（可选）
 * --------------------
 *    升级标记区（4K）
 * --------------------
 *
 * 如果升级包保存在EMMC上，FOTA分区为如下结构：
 * --------------------
 *    缓存区（4K）
 * --------------------
 *    写入状态区（4K）
 * --------------------
 *    升级标记区（4K）
 * --------------------
 */
/*
 * 获取在Flash上预留的FOTA分区的地址和长度，该地址为在flash上的相对地址，是相对flash基地址的偏移
 * start_address 返回FOTA分区的起始地址
 * size          返回FOTA分区大小（包含升级标记区和缓存区、状态区）
 */
errcode_t upg_get_fota_partiton_area_addr(uint32_t *start_address, uint32_t *size);

/*
 * 获取校验用的root_public_key
 */
uint8_t *upg_get_root_public_key(void);

/*
 * 获取Flash基地址
 */
uint32_t upg_get_flash_base_addr(void);

/*
 * 获取Flash大小
 */
uint32_t upg_get_flash_size(void);

/*
 * 重启
 */
void upg_reboot(void);

/*
 * 防止看门狗超时，踢狗
 */
void upg_watchdog_kick(void);

/*
 * 检查FOTA升级包中的信息与板端是否匹配，检查数据包括msid_ext, hardware_id, user_defined等，根据需要选择检查内容
 * pkg_header 升级包包头指针
 * 检查成功，返回 ERRCODE_SUCC
 */
errcode_t upg_check_fota_information(const upg_package_header_t *pkg_header);

/*
 * 获取otp/efuse中的防回滚版本号
 * image_id 镜像的ID
 * anti_rollback_ver 防回滚版本号地址
 */
errcode_t upg_get_board_rollback_version(uint32_t image_id, uint32_t *anti_rollback_ver);

/*
 * 设置otp/efuse中的防回滚版本号
 * image_id 镜像的ID
 * anti_rollback_ver 防回滚版本号地址
 */
errcode_t upg_set_board_rollback_version(uint32_t image_id, uint32_t *anti_rollback_ver);

/*
 * 获取板端防回滚版本号Mask值
 * key_mask KEY区防回滚版本号Mask
 * code_mask Code区防回滚版本号Mask
 */
errcode_t upg_get_board_version_mask(uint32_t image_id, uint32_t *key_mask, uint32_t *code_mask);

/*
 * 获取板端镜像的版本号
 * key_ver KEY区版本号
 * code_ver Code区版本号
 */
errcode_t upg_get_board_version(uint32_t image_id, uint32_t *key_ver, uint32_t *code_ver);

/*
 * flash读数据接口
 * flash_offset 待读取数据的flash偏移地址
 * size 期望读取的数据长度
 * ram_data 出参，数据存放的内存地址
 */
errcode_t upg_flash_read(const uint32_t flash_offset, const uint32_t size, uint8_t *ram_data);

/*
 * flash写数据接口
 * flash_offset 待写入数据的flash偏移地址
 * size 期望写入的数据长度
 * ram_data 入参，待写入数据存放的内存地址
 * do_erase 写前擦除标记，true：写前执行擦除；false：直接写入
 */
errcode_t upg_flash_write(const uint32_t flash_offset, uint32_t size, const uint8_t *ram_data, bool do_erase);

/*
 * flash擦除数据接口
 * flash_offset 待擦除数据的flash偏移地址
 * size 期望擦除的数据长度
 */
errcode_t upg_flash_erase(const uint32_t flash_offset, const uint32_t size);

/*
 * 启动HASH计算
 * key_alg 使用的校验算法，参见upg_key_alg_t
 * handle 返回的句柄
 */
errcode_t upg_start_hash(uint32_t key_alg, uint32_t *handle);

/*
 * 更新HASH
 * handle 句柄
 * src 要计算HASH的数据指针
 * length 要计算HASH的数据长度
 */
errcode_t upg_update_hash(uint32_t handle, const uint8_t *src, uint32_t length);

/*
 * 完成HASH计算并返回HASH值
 * handle 句柄
 * hash 返回的HASH值
 * hash_len HASH值长度
 */
errcode_t upg_complete_hash(uint32_t handle, uint8_t *hash, uint32_t hash_len);

/*
 * 直接计算数据的HASH值
 * key_alg 使用的校验算法，参见upg_key_alg_t
 * src 要计算HASH的数据指针
 * length 要计算HASH的数据长度
 * hash 返回的HASH值
 * hash_len HASH值长度
 * 说明：该函数默认已实现，如果具体芯片需要特殊实现，可以在具体芯片的upg_common_porting.c中重新定义
 */
errcode_t upg_calc_hash_directly(uint32_t key_alg, const uint8_t *src, uint32_t length,
    uint8_t *hash, uint32_t hash_len);

/*
 * 安全校验函数
 * key 校验用的公钥
 * key_alg 使用的校验算法，参见upg_key_alg_t
 * verify_data 要校验数据的存放结构
 * sign_data 签名数据的存放结构
 * 说明：该函数默认已实现，如果具体芯片需要特殊实现，可以在具体芯片的upg_common_porting.c中重新定义
 */
errcode_t upg_secure_authenticate(const uint8_t *key, uint32_t key_alg,
    const upg_auth_data_t *verify_data, const upg_auth_data_t *sign_data);

/*
 * 获取指定的校验算法
 * 说明：算法值参见upg_key_alg_t。
 *       如返回特定算法，升级包的校验时，则强制使用该算法校验；如返回UPG_KEY_ALG_INVAILD，则根据升级包打包时使用的算法校验。
 */
upg_key_alg_t upg_get_specific_verify_key_alg(void);

/*
 * 获取SSB SELITEOS 升级标记区的Flash起始地址，该地址为在flash上的相对地址，是相对flash基地址的偏移
 * start_address 返回升级标记区的起始地址
 */
errcode_t upg_get_store_flag_flash_start_addr(uint32_t *start_address);

errcode_t upg_detect_upgrade_image(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* UPG_ADAPTION_H */

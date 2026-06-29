/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: provide pairinfo manager header for livemic.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2026-03-23, Create file. \n
 */
#ifndef UX_BS_SLE_LIVEMIC_PAIRING_SERVICE
#define UX_BS_SLE_LIVEMIC_PAIRING_SERVICE

#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX)

#include "stddef.h"
#include "stdbool.h"
#include "errcode.h"
#include "bc_api.h"
#include "ux_bg_config.h"
#include "ux_config.h"
#include "ux_bs_sle_db_livemic.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* 上下文管理：每个槽位对应一个NV ID，且保存该NV ID值 */
typedef struct {
    bool valid;                              /* 槽位是否有效 */
    uint16_t nv_id;                          /* 该槽位对应的NV ID */
    ux_sle_db_livemic_pair_info_t pair_info; /* 配对信息 */
    ux_conn_state_t conn_state;              /* 连接状态（pre_pair_check时记录） */
} ux_bs_sle_livemic_slot_t;

typedef struct {
    uint8_t count;                                              /* 当前有效配对数量 */
    uint16_t max_index;                                         /* 当前最大index，用于生成新index */
    ux_device_role_t local_role;                                /* 用于记录本端设备的角色 */
    ux_device_role_t remote_role;                               /* 用于记录对端设备的角色 */
    ux_bs_sle_livemic_slot_t slots[CONFIG_MAX_PAIR_DEVICE_NUM]; /* 配对信息槽位 */
} ux_bs_sle_livemic_pairing_ctx_t;

/**
 * @brief 初始化模块，从NV中加载所有配对信息到上下文。
 * @param[in] role 本机设备的工作角色。
 * @return 0表示成功，非0表示失败。
 */
errcode_t ux_bs_sle_livemic_pairing_service_init(ux_device_role_t role);

/**
 * @brief 去初始化模块。
 * @return 0表示成功，非0表示失败。
 */
errcode_t ux_bs_sle_livemic_pairing_service_deinit(void);

/**
 * @brief 获取模块内部上下文。
 * @return 指向模块上下文的指针。
 */
ux_bs_sle_livemic_pairing_ctx_t *ux_bs_sle_livemic_get_pairing_ctx(void);

/**
 * @brief 在开启配对模式之前调用，查询所有已配对设备的连接状态并保存到上下文。
 * @return 0表示成功，非0表示失败。
 */
errcode_t ux_bs_sle_livemic_pre_pair_check(void);

/**
 * @brief 向指定设备配对，根据配对信息筛选是配对信息新增或者配对信息更新。
 * @param[in] info 设备的配对信息。
 * @return 0表示成功，非0表示失败（如设备不存在）。
 */
errcode_t ux_bs_sle_livemic_pair_to_device(const bs_2_4_g_paired_device_info_t *pair_info);

/**
 * @brief 删除指定地址的配对信息。
 * @param[in] addr 设备地址指针。
 * @param[in] addr_len 地址长度（应为6）。
 * @return 0表示成功，非0表示失败（如设备不存在）。
 */
errcode_t ux_bs_sle_livemic_delete_pair_info(uint8_t *addr, uint16_t addr_len);

/**
 * @brief 更新指定地址设备的 device_id。
 * @param[in] device_id 新的设备 ID。
 * @param[in] addr 设备地址指针。
 * @param[in] addr_len 地址长度（应为6）。
 * @return 0表示成功，非0表示失败（如设备不存在或地址长度无效）。
 */
errcode_t ux_bs_sle_livemic_update_device_id(uint16_t device_id, uint8_t *addr, uint16_t addr_len);

/**
 * @brief 获取当前已配对设备数量。
 * @return 数量。
 */
uint8_t ux_bs_sle_livemic_get_pair_info_num(void);

/**
 * @brief 查询所有已配对设备的信息（从上下文读取，不从NV读取）。
 * @param[out] pair_list 用于存储配对信息的数组（调用者分配）。
 * @param[in] pair_list_size pair_list的元素个数。
 * @return 0表示成功，非0表示失败（如设备不存在）。
 */
errcode_t ux_bs_sle_livemic_get_pair_info(ux_sle_db_livemic_pair_info_t *pair_list, uint8_t pair_list_size);

/**
 * @brief 加载所有的配对信息并下发给星闪协议栈
 * @return 0表示成功，非0表示失败（如设备不存在）。
 */
errcode_t ux_bs_sle_livemic_restore_pair_info(void);

/**
 * @brief 打印当前所有配对信息（调试用）。
 */
void ux_bs_sle_livemic_dump(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

#endif /* UX_BS_bs_sle_LIVEMIC_H */
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag data store
 * This file should be changed only infrequently and with great care.
 */

#include "dfx_adapt_layer.h"
#include "diag_data_store.h"

// 全局静态指针，指向已注册的驱动
static diag_data_store_driver_t *g_critical_driver = NULL;
static diag_data_store_driver_t *g_non_critical_driver = NULL;

// 注册非关键diag数据存储驱动
errcode_t diag_data_store_register_non_critical_driver(diag_data_store_driver_t *driver)
{
    if (driver == NULL || driver->write == NULL || driver->read == NULL || driver->release == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    g_non_critical_driver = driver;
    return ERRCODE_SUCC;
}

// 注册关键diag数据存储驱动
errcode_t diag_data_store_register_critical_driver(diag_data_store_driver_t *driver)
{
    if (driver == NULL || driver->write == NULL || driver->read == NULL || driver->release == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    g_critical_driver = driver;
    return ERRCODE_SUCC;
}

// 初始化diag数据存储模块
errcode_t diag_data_store_init(void)
{
    errcode_t ret = ERRCODE_SUCC;
    if (g_critical_driver && g_critical_driver->init) {
        if (g_critical_driver->init(NULL) != 0) {
            dfx_log_err("[DIAG_REPORT]Critical driver init failed.\n");
            ret = ERRCODE_FAIL;
        }
    }
    if (g_non_critical_driver && g_non_critical_driver->init) {
        if (g_non_critical_driver->init(NULL) != 0) {
            dfx_log_err("[DIAG_REPORT]Non-critical driver init failed.\n");
            ret = ERRCODE_FAIL;
        }
    }
    return ret;
}

// 去初始化diag数据存储模块
void diag_data_store_deinit(void)
{
    if (g_critical_driver && g_critical_driver->deinit) {
        g_critical_driver->deinit();
    }
    if (g_non_critical_driver && g_non_critical_driver->deinit) {
        g_non_critical_driver->deinit();
    }
}

// 非关键diag数据存储写入函数
errcode_t diag_data_store_non_critical_write(uint16_t cmd_id, void *data, uint32_t len,
                                             const diag_data_store_ctx_t *ctx)
{
    if (g_non_critical_driver == NULL || g_non_critical_driver->write == NULL) {
        return ERRCODE_FAIL;
    }
    if (data == NULL || len == 0 || ctx == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    return g_non_critical_driver->write(cmd_id, data, len, ctx);
}

// 非关键diag数据存储读取函数
errcode_t diag_data_store_non_critical_read(diag_data_item_t **item, uint32_t timeout)
{
    if (g_non_critical_driver == NULL || g_non_critical_driver->read == NULL) {
        return ERRCODE_FAIL;
    }
    return g_non_critical_driver->read(item, timeout);
}

// 非关键diag数据存储释放函数
errcode_t diag_data_store_non_critical_release(diag_data_item_t *item)
{
    if (g_non_critical_driver == NULL || g_non_critical_driver->release == NULL) {
        return ERRCODE_FAIL;
    }
    return g_non_critical_driver->release(item);
}

// 关键diag数据存储写入函数
errcode_t diag_data_store_critical_write(uint16_t cmd_id, void *data, uint32_t len, const diag_data_store_ctx_t *ctx)
{
    if (g_critical_driver == NULL || g_critical_driver->write == NULL) {
        return ERRCODE_FAIL;
    }
    if (data == NULL || len == 0 || ctx == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    return g_critical_driver->write(cmd_id, data, len, ctx);
}

// 关键diag数据存储读取函数
errcode_t diag_data_store_critical_read(diag_data_item_t **item, uint32_t timeout)
{
    if (g_critical_driver == NULL || g_critical_driver->read == NULL) {
        return ERRCODE_FAIL;
    }
    return g_critical_driver->read(item, timeout);
}

// 关键diag数据存储释放函数
errcode_t diag_data_store_critical_release(diag_data_item_t *item)
{
    if (g_critical_driver == NULL || g_critical_driver->release == NULL) {
        return ERRCODE_FAIL;
    }
    return g_critical_driver->release(item);
}

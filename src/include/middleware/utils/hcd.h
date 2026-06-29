/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Header file for diag cloud report api \n
 *
 * History: \n
 * 2025-10-23， Create file. \n
 */

#ifndef HCD_H
#define HCD_H

#include "errcode.h"
#include "dfx_resource_id.h"
#include "soc_diag_log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @if Eng
 * @brief  Report diag events.
 * @param  [in]  id log ID (only 0 is supported now)
 * @param  [in]  fmt the format information of the event. '@' separates the event tag and the event content.
 * @param  [in]  args the parameter list of event (variable number, up to 10)
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @else
 * @brief  diag事件打印
 * @param  [in]  id 日志ID（当前只支持0）
 * @param  [in]  fmt 打印输出的格式字符串，'@'分割事件标签和事件内容
 * @param  [in]  args 参数列表（最多支持10个参数）
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @endif
 */
#define uapi_diag_report_event(id, fmt, args...)         uapi_diag_eventx(id, fmt, ##args)

/**
 * @if Eng
 * @brief  Data getter callback function for a registered metric.
 * @else
 * @brief  metric注册时的数据获取回调函数。
 * @endif
 */
typedef errcode_t (*diag_metrics_getter_t)(void *data_ptr, uint32_t *len_ptr, void *ctx);

/**
 * @if Eng
 * @brief  Register a new metric for periodic collection.
 * @par Description: This function is used to register a new metric for periodic collection. You can
 *                    run uapi_diag_metrics_period_report to register new metrics.
 * @param  [in]  cmd_id Command Id.
 * @param  [in]  getter The callback function to get the metric's value.
 * @param  [in]  interval The collection interval in seconds for this metric.
 *                        If 0, the metric is registered but not collected periodically.
 * @param  [in]  data_len The maximum possible size of the data for this metric.
 *                         Used for internal buffer allocation.
 * @param  [in]  ctx A user-defined context pointer to be passed to the getter.
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li hcd.h
 * @else
 * @brief  注册一个新的metric用于周期性采集.
 * @par 说明: 调用此接口注册新的metric用于周期性采集。用户可以调用uapi_diag_metrics_period_report注册新的metric。
 * @param  [in]  cmd_id 命令id.
 * @param  [in]  getter 获取metric数据的回调函数。
 * @param  [in]  interval 此metric的采集间隔，单位为秒。如果为0，则该metric将被注册，但不会被周期性采集。
 * @param  [in]  data_len 此metric的数据的最大可能大小。用于内部缓冲区分配。
 * @param  [in]  ctx 用户定义的上下文指针，将被传递给回调函数。
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li hcd.h
 * @endif
 */
errcode_t uapi_diag_metrics_period_report(uint16_t cmd_id, diag_metrics_getter_t getter, uint32_t interval,
                                          uint32_t data_len, void *ctx);

/**
 * @if Eng
 * @brief  Stop periodic report of a metric
 * @par Description: This function is used to stop periodic collection for a certain metric. You can
 *                    run uapi_diag_stop_report to stop metrics.
 * @param  [in]  cmd_id DIAG command request ID of the metric to stop.
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li hcd.h
 * @else
 * @brief  停止metric的周期性上报。
 * @par 说明: 调用此接口停止某个metric的周期性采集。用户可以调用uapi_diag_stop_report停止metric的上报。
 * @param  [in]  cmd_id 要停止的metric的命令id。
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li hcd.h
 * @endif
 */
errcode_t uapi_diag_metrics_stop_report(uint16_t cmd_id);

/**
 * @if Eng
 * @brief  Manually report a single metric value immediately.
 * This is for event-driven metrics. The data is passed directly.
 * @par Description: This function is used to manually report a single metric value immediately.
 * @param  [in]  cmd_id DIAG command request ID of the metric.
 * @param  [in]  data Pointer to the metric data.
 * @param  [in]  len Length of the metric data.
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li hcd.h
 * @else
 * @brief  手动立即上报单个metric值。
 * @par 说明: 此接口用于手动立即上报单个metric值。
 * @param  [in]  cmd_id 要上报的metric的命令id。
 * @param  [in]  data 指向metric数据的指针。
 * @param  [in]  len metric数据的长度。
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li hcd.h
 * @endif
 */
errcode_t uapi_diag_metrics_report_once(uint16_t cmd_id, const void *data, uint32_t len);

 /**
 * @if Eng
 * @brief  Update the collection interval for a registered metric.
 * @par Description: This function is used to update the collection interval for a registered metric.
 * @param  [in]  cmd_id DIAG command request ID of the metric.
 * @param  [in]  new_interval The new collection interval in seconds.
 *                            If 0, periodic collection for this metric will be paused.
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li hcd.h
 * @else
 * @brief  更新已注册metric的采集间隔。
 * @par 说明: 调用此接口更新已注册metric的采集间隔。
 * @param  [in]  cmd_id 要更新的metric的命令id。
 * @param  [in]  new_interval 新的采集间隔，单位为秒。如果为0，则将暂停对此metric的周期性采集。
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li hcd.h
 * @endif
 */
errcode_t uapi_diag_metrics_update_interval(uint16_t cmd_id, uint32_t new_interval);

/**
 * @}
 */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* HCD_H */
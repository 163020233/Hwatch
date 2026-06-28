/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: SLE Adv Filter Manager module.
 */
 
/**
 * @defgroup sle_adv_filter_manager adv filter manager API
 * @ingroup  SLE
 * @{
 */
 
#ifndef SLE_ADV_FILTER
#define SLE_ADV_FILTER
 
#include <stdint.h>
#include "errcode.h"
#include "sle_common.h"
 
#ifdef __cplusplus
extern "C" {
#endif
 
#define SLE_ADV_FILTER_DATA_LEN_MAX 24
#define FILTER_BY_HARDWARE_DATA_LEN_MAX 16
 
/* 使用方法：先调用初始化接口，再调用添加/删除/总开关接口，若删除了所有过滤器，再最后调用去初始化接口 */
 
/**
 * @if Eng
 * @brief Enum of filter switch type.
 * @else
 * @brief 过滤器总开关类型。
 * @endif
 */
typedef enum {
    SLE_DISABLE_ALL_FILTERS = 0,            /*!< @if Eng disable all filters
                                                 @else   去激活所有过滤器 @endif */
    SLE_ENABLE_ALL_FILTERS,                 /*!< @if Eng enable all filters
                                                 @else   激活所有过滤器 @endif */
    SLE_DELETE_ALL_FILTERS,                 /*!< @if Eng delete all filters
                                                 @else   删除所有过滤器 @endif */
} sle_adv_filter_switch_type_t;
 
/**
 * @if Eng
 * @brief  Enum of filter type.
 * @else
 * @brief  过滤器类型。类型0代表过滤器总开关，已单独提供接口表征；类型1代表广播数据传输方式，只支持一种传输方式，所以不体现。
 * @endif
 */
typedef enum {
    SLE_RSSI_FILTER = 2,                           /*!< @if Eng rssi filter.
                                                        @else   rssi过滤器 @endif */
    SLE_ADDR_FILTER,                               /*!< @if Eng addr filter,parameters.
                                                        @else   地址过滤器 @endif */
    SLE_NAME_FILTER,                               /*!< @if Eng name filter.
                                                        @else   名称过滤器 @endif */
    SLE_SERVICE_DATA_FILTER,                       /*!< @if Eng service data filter.
                                                        @else   服务数据过滤器 @endif */
    SLE_MANUFACTURER_DATA_FILTER,                  /*!< @if Eng manufacturer data filter.
                                                        @else   厂商数据过滤器 @endif */
    SLE_ADV_TYPE_AND_SERVICE_DATA_FILTER,          /*!< @if Eng adv type and service data filter.
                                                        @else   广播类型和服务数据过滤器 @endif */
    SLE_ADV_TYPE_AND_SERVICE_DATA_MASK_FILTER,     /*!< @if Eng adv type and service data mask filter
                                                        @else   广播类型和服务数据掩码过滤 @endif */
    SLE_ADV_DATA_FILTER_BY_HARDWARE,               /*!< @if Eng hardware filter.
                                                        @else   硬件过滤器 @endif */
} sle_adv_filter_type_t;
 
/**
 * @if Eng
 * @brief  Rssi filter.
 * @else
 * @brief  rssi过滤器
 * @endif
 */
typedef struct {
    int8_t rssi;                            /*!< @if Eng rssi
                                                 @else   收到的包的rssi大于等于此值 @endif */
} sle_adv_rssi_filter_t;
 
/**
 * @if Eng
 * @brief  Addr filter.
 * @else
 * @brief  地址过滤器
 * @endif
 */
typedef struct {
    uint8_t addr_type;                      /*!< @if Eng addr type.
                                                 @else   地址类型 @endif */
    uint8_t addr[SLE_ADDR_LEN];             /*!< @if Eng addr.
                                                 @else   地址 @endif */
} sle_adv_addr_filter_t;
 
/**
 * @if Eng
 * @brief  Name filter.
 * @else
 * @brief  名称过滤器
 * @endif
 */
typedef struct {
    uint8_t name_len;                       /*!< @if Eng name len
                                                 @else   名称长度，大于24字节，只取前24字节 @endif */
    uint8_t *name;                          /*!< @if Eng name
                                                 @else   名称，广播数据长度小于此长度代表不匹配 @endif */
} sle_adv_name_filter_t;
 
/**
 * @if Eng
 * @brief  Service data filter.
 * @else
 * @brief  服务数据过滤器
 * @endif
 */
typedef struct {
    uint8_t data_len;                       /*!< @if Eng data len
                                                 @else   服务数据长度/服务数据掩码长度，大于24字节，只取前24字节 @endif */
    uint8_t *service_data;                  /*!< @if Eng service data
                                                 @else   服务数据，广播数据长度小于此长度代表不匹配。 @endif */
    uint8_t *service_data_mask;             /*!< @if Eng service data mask
                                                 @else   服务数据掩码，广播数据和服务数据分别与服务数据掩码按位&，
                                                         比较两者结果是否一致。@endif */
} sle_adv_service_data_filter_t;
 
/**
 * @if Eng
 * @brief  Manufacturer data filter.
 * @else
 * @brief  厂商数据过滤器
 * @endif
 */
typedef struct {
    uint8_t data_len;                       /*!< @if Eng data len
                                                 @else   厂商数据长度/厂商数据掩码长度，大于24字节，只取前24字节 @endif */
    uint8_t *manufacturer_data;             /*!< @if Eng manufacturer data
                                                 @else   厂商数据，广播数据长度小于此长度代表不匹配。@endif */
    uint8_t *manufacturer_data_mask;        /*!< @if Eng manufacturer data mask
                                                 @else   厂商数据掩码，广播数据和厂商数据分别与厂商数据掩码按位&，
                                                         比较两者结果是否一致 @endif */
} sle_adv_manufacturer_data_filter_t;
 
/**
 * @if Eng
 * @brief  Adv type and data filter.
 * @else
 * @brief  广播类型和服务数据过滤器
 * @endif
 */
typedef struct {
    uint8_t adv_type;                       /*!< @if Eng adv type.
                                                 @else   广播类型。@endif */
    uint8_t data_len;                       /*!< @if Eng data len
                                                 @else   服务数据长度，大于24字节，只取前24字节 @endif */
    uint8_t *service_data;                  /*!< @if Eng service data
                                                 @else   服务数据，广播数据长度小于此长度代表不匹配。 @endif */
} sle_adv_type_and_data_filter_t;
 
/**
 * @if Eng
 * @brief  Adv type and data mask filter.
 * @else
 * @brief  广播类型和服务数据掩码过滤器
 * @endif
 */
typedef struct {
    uint8_t adv_type;                       /*!< @if Eng adv type.
                                                 @else   广播类型。@endif */
    uint8_t data_len;                       /*!< @if Eng data len
                                                 @else   服务数据长度/服务数据掩码长度，大于24字节，只取前24字节 @endif */
    uint8_t *service_data;                  /*!< @if Eng service data
                                                 @else   服务数据，广播数据长度小于此长度代表不匹配。 @endif */
    uint8_t *service_data_mask;             /*!< @if Eng service data mask
                                                 @else   服务数据掩码，广播数据和服务数据分别与服务数据掩码按位&，
                                                         比较两者结果是否一致。@endif */
} sle_adv_type_and_data_mask_filter_t;

/**
 * @if Eng
 * @brief  filter by hardware.
 * @else
 * @brief  硬件过滤器
 * @endif
 */
typedef struct {
    uint8_t data_len;                              /*!< @if Eng data len
                                                         @else   用户数据长度, 最大长度为16字节 @endif */
    uint8_t data_offset;                           /*!< @if Eng data offset
                                                         @else   广播数据偏移 @endif */
    uint16_t data_mask;                            /*!< @if Eng data mask
                                                         @else   广播数据掩码, 按字节掩码 @endif */
    uint8_t data[FILTER_BY_HARDWARE_DATA_LEN_MAX]; /*!< @if Eng data
                                                         @else   广播数据匹配字段 @endif */
} sle_adv_data_filter_by_hardware_t;
 
/**
 * @if Eng
 * @brief  Sle adv filter.
 * @else
 * @brief  星闪广播过滤器
 * @endif
 */
typedef union {
    sle_adv_rssi_filter_t rssi_filter;                              /*!< @if Eng rssi filter
                                                                         @else   rssi过滤器。 @endif */
    sle_adv_addr_filter_t addr_filter;                              /*!< @if Eng addr filter
                                                                         @else   地址过滤器。 @endif */
    sle_adv_name_filter_t name_filter;                              /*!< @if Eng name filter
                                                                         @else   名称过滤器。 @endif */
    sle_adv_service_data_filter_t service_data_filter;              /*!< @if Eng service data filter
                                                                         @else   服务数据过滤器。 @endif */
    sle_adv_manufacturer_data_filter_t manufacturer_data_filter;    /*!< @if Eng manufacturer data filter
                                                                         @else   厂商数据过滤器。 @endif */
    sle_adv_type_and_data_filter_t adv_type_and_data_filter;        /*!< @if Eng adv type and service data filter
                                                                         @else   广播类型和服务数据过滤器。 @endif */
    sle_adv_type_and_data_mask_filter_t adv_type_and_data_mask_filter;    /*!< @if Eng adv type and service data mask
                                                                                       filter.
                                                                               @else   广播类型和服务数据掩码过滤器。
                                                                               @endif */
    sle_adv_data_filter_by_hardware_t filter_by_hardware;           /*!< @if Eng filter by hardware
                                                                         @else   硬件过滤器 @endif */
} sle_adv_filter_t;
 
/**
 * @if Eng
 * @brief Struct of add adv filter.
 * @else
 * @brief 添加过滤器参数结构体。
 * @endif
 */
typedef struct {
    uint8_t filter_type;                    /*!< @if Eng filter type, {@ref sle_adv_filter_type_t}.
                                                 @else   过滤器类型，{@ref sle_adv_filter_type_t} @endif */
    sle_adv_filter_t filter;                /*!< @if Eng filter param.
                                                 @else   添加过滤器所需参数 @endif */
} sle_add_adv_filter_param_t;
 
/**
 * @if Eng
 * @brief  Struct of config filter result.
 * @else
 * @brief  配置过滤器结果结构体。
 * @endif
 */
typedef struct {
    uint8_t filter_index;           /*!< @if Eng filter index.
                                         @else   过滤器序号 @endif */
    uint8_t filter_type;            /*!< @if Eng filter type，{ @ref sle_adv_filter_type_t }.
                                         @else   过滤器类型, { @ref sle_adv_filter_type_t } @endif */
    uint8_t num_available;          /*!< @if Eng num available.
                                         @else   剩余可用过滤器数量 @endif */
} sle_adv_filter_config_result_t;
 
/**
 * @if Eng
 * @brief  Struct of switch filter result.
 * @else
 * @brief  总开关控制过滤器结果结构体。
 * @endif
 */
typedef struct {
    uint8_t switch_type;                /*!< @if Eng filter switch，{ @ref sle_adv_filter_switch_type_t }.
                                             @else   控制过滤器总开关, { @ref sle_adv_filter_switch_type_t } @endif */
    uint8_t num_available;              /*!< @if Eng num available.
                                             @else   剩余可用过滤器数量 @endif */
} sle_adv_filter_switch_result_t;
 
/**
 * @if Eng
 * @brief Callback invoked when add adv filter.
 * @par Callback invoked when add adv filter.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] result add filter result.
 * @param [in] status error code.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @else
 * @brief  添加过滤器回调函数。
 * @par    添加过滤器回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] result 添加过滤器结果。
 * @param [in] status 返回错误码。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @endif
 */
typedef void (*sle_adv_filter_add_callback)(sle_adv_filter_config_result_t *result, errcode_t status);
 
/**
 * @if Eng
 * @brief Callback invoked when delete adv filter.
 * @par Callback invoked when delete adv filter.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] result delete filter result.
 * @param [in] status error code.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @else
 * @brief  删除过滤器回调函数。
 * @par    删除过滤器回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] result 删除过滤器结果。
 * @param [in] status 返回错误码。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @endif
 */
typedef void (*sle_adv_filter_delete_callback)(sle_adv_filter_config_result_t *result, errcode_t status);
 
/**
 * @if Eng
 * @brief Callback invoked when switch adv filter.
 * @par Callback invoked when switch adv filter.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] result switch filter result.
 * @param [in] status error code.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @else
 * @brief  总开关控制过滤器回调函数。
 * @par    总开关控制过滤器回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] result 总开关控制过滤器结果。
 * @param [in] status 返回错误码。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @endif
 */
typedef void (*sle_adv_filter_switch_callback)(sle_adv_filter_switch_result_t *result, errcode_t status);

/**
 * @if Eng
 * @brief Struct of SLE adv filter callback function.
 * @else
 * @brief SLE 广播过滤器回调函数接口定义。
 * @endif
 */
typedef struct {
    sle_adv_filter_add_callback adv_filter_add_cb;          /*!< @if Eng add adv filter callback.
                                                                 @else   添加过滤器回调函数。 @endif */
    sle_adv_filter_delete_callback adv_filter_del_cb;       /*!< @if Eng delete adv filter callback.
                                                                 @else   删除过滤器回调函数。 @endif */
    sle_adv_filter_switch_callback adv_filter_switch_cb;    /*!< @if Eng switch adv filter callback.
                                                                 @else   总开关控制过滤器回调函数。 @endif */
} sle_adv_filter_callbacks_t;
 
/**
 * @if Eng
 * @brief  Init sle adv filter.
 * @par Description: Init sle adv filter.
 * @param  [in]  func  adv filter cbks.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  初始化星闪广播过滤器
 * @par Description: 初始化星闪广播过滤器
 * @param  [in]  func 过滤器回调函数。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_init_adv_filter(sle_adv_filter_callbacks_t *func);
 
/**
 * @if Eng
 * @brief  Add sle adv filter.
 * @par Description: Add sle adv filter，This function is asynchronous,
 *   and the execution result is reported through a callback.
 * @param  [in]  param add filter param.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  添加星闪广播过滤器, 该函数是异步函数，执行结果通过回调上报
 * @par Description: 添加星闪广播过滤器
 * @param  [in]  param 添加星闪广播过滤器参数。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_add_adv_filter(sle_add_adv_filter_param_t *param);
 
/**
 * @if Eng
 * @brief  Delete sle adv filter.
 * @par Description: Delete sle adv filter，This function is asynchronous,
 *   and the execution result is reported through a callback.
 * @param  [in]  filter_index  adv filter index.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  删除星闪广播过滤器，该函数是异步函数，执行结果通过回调上报
 * @par Description: 删除星闪广播过滤器
 * @param  [in]  filter_index 过滤器序号。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_delete_adv_filter(uint8_t filter_index);
 
/**
 * @if Eng
 * @brief  Switch sle adv filter.
 * @par Description: Switch sle adv filter，This function is asynchronous,
 *   and the execution result is reported through a callback.
 * @param  [in]  filter_switch  adv filter switch, { @ref sle_adv_filter_switch_type_t } .
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  总开关控制星闪广播过滤器，该函数是异步函数，执行结果通过回调上报
 * @par Description: 总开关控制星闪广播过滤器
 * @param  [in]  filter_switch 过滤器总开关, { @ref sle_adv_filter_switch_type_t } 。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_switch_adv_filter(uint8_t filter_switch);
 
/**
 * @if Eng
 * @brief  Deinit sle adv filter.
 * @par Description: Deinit sle adv_filter.
 * @retval error code.
 * @else
 * @brief  去初始化SLE广播过滤器。
 * @par Description: 去初始化SLE广播过滤器。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t sle_deinit_adv_filter(void);
 
#ifdef __cplusplus
}
#endif
#endif /* SLE_ADV_FILTER */
/**
 * @}
 */
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Provides capsense driver api \n
 *
 * History: \n
 * 2025-04-06， Create file. \n
 */

#ifndef CAPSENSE_H
#define CAPSENSE_H

#include "stdbool.h"
#include "errcode.h"
#include "capsense_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_capsense CAPSENSE
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of CAPSENSE oversample rate.
 * @else
 * @brief  电容传感器过采样率定义。
 * @endif
 */
typedef enum capsense_osr {
    CAPSENSE_OSR_256,              /*!< @if Eng 256 oversample rate.
                                         @else   256过采样率。 @endif */
    CAPSENSE_OSR_128,              /*!< @if Eng 128 oversample rate.
                                         @else   128过采样率。 @endif */
    CAPSENSE_OSR_64,               /*!< @if Eng 64 oversample rate.
                                         @else   64过采样率。 @endif */
    CAPSENSE_OSR_32                /*!< @if Eng 32 oversample rate.
                                         @else   32过采样率。 @endif */
} capsense_osr_t;

/**
 * @if Eng
 * @brief  Definition of CAPSENSE range.
 * @else
 * @brief  电容传感器测量范围定义。
 * @endif
 */
typedef enum capsense_range {
    CAPSENSE_RANGE_1P25PF,         /*!< @if Eng Range ±1.25pF.
                                         @else   测量范围±1.25pF。 @endif */
    CAPSENSE_RANGE_2P5PF,          /*!< @if Eng Range ±2.5pF.
                                         @else   测量范围±2.5pF。 @endif */
    CAPSENSE_RANGE_MAX
} capsense_range_t;

/**
 * @if Eng
 * @brief  Definition of CAPSENSE mode.
 * @else
 * @brief  电容传感器模式定义。
 * @endif
 */
typedef enum capsense_mode {
    CAPSENSE_MODE_POLL,            /*!< @if Eng Sample mode poll.
                                         @else   轮询。 @endif */
    CAPSENSE_MODE_DATA_READY_IRQ,  /*!< @if Eng IRQ when data ready.
                                         @else   数据就绪中断。 @endif */
    CAPSENSE_MODE_THRESHOLD_IRQ    /*!< @if Eng IRQ when threshold.
                                         @else   阈值中断。 @endif */
} capsense_mode_t;

/**
 * @if Eng
 * @brief  Definition of CAPSENSE baseline configuration.
 * @else
 * @brief  电容传感器基准值配置定义。
 * @endif
 */
typedef struct capsense_baseline {
    int16_t (*buffer)[CAPSENSE_CHANNEL_MAX];  /*!< @if Eng Baseline data buffer.
                                                            Format: buffer[frame][channel].
                                                            Size: request_length * CAPSENSE_CHANNEL_MAX.
                                                    @else   基准数据缓冲区。
                                                            格式：buffer[帧][通道]。
                                                            大小：request_length * CAPSENSE_CHANNEL_MAX。 @endif */
    uint32_t request_length;                   /*!< @if Eng Request baseline frames count.
                                                            Range: 0 ~ maximum buffer frames CAPSENSE_BUFFER_LENGTH.
                                                            0 means skip baseline capture.
                                                    @else   请求的基准帧数。
                                                            范围：0 ~ 缓冲区最大帧数 CAPSENSE_BUFFER_LENGTH。
                                                            0 表示跳过基准捕获。 @endif */
    uint32_t captured_length;                  /*!< @if Eng Actual captured baseline frames count.
                                                    @else   实际捕获的基准帧数。 @endif */
} capsense_baseline_t;

/**
 * @if Eng
 * @brief  Definition of CAPSENSE attributes.
 * @else
 * @brief  电容传感器配置参数定义。
 * @endif
 */
typedef struct capsense_attr {
    uint32_t period;                /*!< @if Eng Interval between samples, count in CAPSENSOR working clock.
                                                 If working clock is Tus,
                                                 Real period = Sample time + period * Tus. Refer to the CAPSENSE_WOKR_CYCLE
                                                 macro definition for the working clock period.
                                         @else   两次采样的间隔时间，按照CAPSENSE工作时钟周期计数。如果工作时钟周期为Tus，
                                                 实际周期 = 采样时间 + period * Tus。 工作时钟周期参考 CAPSENSE_WOKR_CYCLE
                                                 宏定义 @endif */
    uint8_t osr;                    /*!< @if Eng Oversample rate.
                                             For details, see @ref capsense_osr_t
                                         @else   过采样率。参考 @ref capsense_osr_t @endif */
    uint8_t range;                  /*!< @if Eng Range.
                                             For details, see @ref capsense_range_t
                                         @else   测量范围。参考 @ref capsense_range_t @endif */
    uint8_t buffer_length;          /*!< @if Eng Buffer length, no bigger than CAPSENSE_BUFFER_LENGTH. In polling mode
                                                 and data-ready mode, data exceeding the buffer length will be overwritten.
                                                 In data-ready mode, buffer_length serves as the interrupt trigger threshold.
                                         @else   缓存长度，不超过CAPSENSE_BUFFER_LENGTH。轮询模式和阈值中断模式下，超过缓存长度
                                                 的数据将被覆盖，数据就绪中断模式下，buffer_length是触发中断的阈值。 @endif */
    bool cali_manual;               /*!< @if Eng Manual calibration value configuration.
                                         @else   手动配置校准值。 @endif */
    uint16_t cali_val[CAPSENSE_CHANNEL_MAX];   /*!< @if Eng Calibration value. When cali_manual = 1, manual
                                                         calibration enabled, cali_val is written to the driver.
                                                         When cali_manual = 0, auto calibration is enabled, power-on
                                                         calibration result is stored in cali_val.
                                                         Eng Calibration value.
                                                             - cali_manual = 0: Auto mode. Stores power-on calibration
                                                             result in 'cali_val'.
                                                             - cali_manual = 1: Manual mode. Value in 'cali_val' is
                                                             written to the driver.
                                                    @else   校准值。
                                                            - cali_manual = 0: 自动模式。上电自校准结果保存到'cali_val'。
                                                            - cali_manual = 1: 手动模式。'cali_val'中的值被写入驱动中。
                                                    @endif */
#ifdef CONFIG_CAPSENSE_SUPPORT_THRESHOLD_MODE
    int16_t thre_h[CAPSENSE_CHANNEL_MAX];          /*!< @if Eng High threshold.
                                                        @else   上限阈值。 @endif */
    int16_t thre_l[CAPSENSE_CHANNEL_MAX];          /*!< @if Eng Low threshold.
                                                        @else   下限阈值。 @endif */
#endif
#ifdef CONFIG_CAPSENSE_SUPPORT_CHANNEL_MAPPING
    uint8_t channel_mapping[CAPSENSE_CHANNEL_MAX]; /*!< @if Eng Logical-to-physical channel mapping table.
                                                                When channel mapping is enabled, all
                                                                channel parameters in the interface refer to logical channels
                                                                rather than physical hardware channels. This includes:
                                                                1. The data order in uapi_capsense_read output.
                                                                2. Channel index parameters used in other APIs, such as
                                                                   uapi_capsense_calibration.
                                                                3. Channel-related configurations in configuration interfaces,
                                                                   such as capsense_attr_t.cali_val.
                                                                For example, when channel mapping is not used, the read data is
                                                                [data0, data1, data2, data3, data4]. After configuring channel mapping
                                                                as [1, 2, 3, 4, 0], the read data becomes [data1, data2, data3,
                                                                data4, data0].
                                                        @else   逻辑通道到物理通道的映射关系表。
                                                                启用通道映射后，所有接口中的通道参数
                                                                均指逻辑通道而非物理硬件通道。包括：
                                                                1. uapi_capsense_read 输出的数据顺序。
                                                                2. 其他API中使用的通道索引参数，例如uapi_capsense_calibration。
                                                                3. 配置接口中的通道相关配置，例如capsense_attr_t.cali_val。
                                                                例如，不使用通道映射时，读出的数据为[data0, data1, data2, data3,
                                                                data4]，通道映射配置为[1, 2, 3, 4, 0]后，读出的数据为[data1, data2,
                                                                data3, data4, data0]。 @endif */
#endif
    capsense_baseline_t baseline;                  /*!< @if Eng Baseline capture.
                                                        @else   基线获取。 @endif */
} capsense_attr_t;

/**
 * @if Eng
 * @brief  CAPSENSE event callback to be called.
           @ref uapi_capsense_set_mode is invoked.
 * @note   This callback is invoked in an interrupt context.
 * @param  type The IRQ type. For details, see @ref capsense_mode_t.
 * @param  param Callback parameter.
 * @else
 * @brief  电容传感器中断事件回调函数，通过 @ref uapi_capsense_set_mode 注册到驱动中。
 * @note   这个函数是在中断上下文中执行的。
 * @param  type 中断类型。参考 @ref capsense_mode_t。
 * @param  param 回调参数。
 * @endif
 */
typedef void (*capsense_callback_t)(uint8_t type, uint32_t param);

/**
 * @if Eng
 * @brief  Initialize the CAPSENSE.
 * @param  [in]      attr Basic configuration of CAPSENSE. see @ref capsense_attr_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief 初始化电容传感器。
 * @param  [in,out]  attr CAPSENSE的配置参数，参考 @ref capsense_attr_t。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_capsense_init(capsense_attr_t *attr);

/**
 * @if Eng
 * @brief  Set threshold values for CAPSENSE interrupt detection.
 * @param  [in] thre_h High threshold array for each channel.
 *                     When capacitance exceeds this value, high-threshold interrupt triggers.
 * @param  [in] thre_l Low threshold array for each channel.
 *                     When capacitance falls below this value, low-threshold interrupt triggers.
 * @param  [in] channel_num Number of channels to configure thresholds for.
 *                          Range: 1 to CAPSENSE_CHANNEL_MAX.
 *                          If less than CAPSENSE_CHANNEL_MAX, only the first channel_num
 *                          channels are configured; others remain unchanged.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @note   Logical channel mapping applies:
 *          - thre_h[logical_ch] and thre_l[logical_ch] correspond to logical channels.
 *          - Physical channel mapping is automatically handled internally.
 * @else
 * @brief  设置电容传感器的中断检测阈值。
 * @param  [in] thre_h 各通道的高阈值数组，电容值超过此值时触发阈值中断。
 * @param  [in] thre_l 各通道的低阈值数组，电容值低于此值时触发阈值中断。
 * @param  [in] channel_num 要配置阈值的通道数量。
 *                          范围：1 到 CAPSENSE_CHANNEL_MAX。
 *                          如果小于 CAPSENSE_CHANNEL_MAX，只配置前 channel_num
 *                          个通道的阈值；其他通道保持原值。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t。
 * @note   如果应用逻辑通道映射：
 *          - thre_h[逻辑通道] 和 thre_l[逻辑通道] 对应逻辑通道。
 *          - 物理通道映射由内部自动处理。
 * @endif
 */
errcode_t uapi_capsense_set_threshold(int16_t *thre_h,
                                      int16_t *thre_l,
                                      uint8_t channel_num);

/**
 * @if Eng
 * @brief  Deinitialize the CAPSENSE.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去初始化电容传感器。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_capsense_deinit(void);

/**
 * @if Eng
 * @brief  Open a CAPSENSE channel.
 * @param  [in]  ch CAPSENSE channel. For details, see @ref capsense_channel_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  打开一个电容传感器通道。
 * @param  [in]  ch 电容传感器通道。 参考 @ref capsense_channel_t。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_capsense_open(uint8_t ch);

/**
 * @if Eng
 * @brief  Close a CAPSENSE channel.
 * @param  [in]  ch CAPSENSE channel. For details, see @ref capsense_channel_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  关闭一个电容传感器通道。
 * @param  [in]  ch 电容传感器通道。参考 @ref capsense_channel_t。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_capsense_close(uint8_t ch);

/**
 * @if Eng
 * @brief  Get the current scan number from CAPSENSE, shared by all channels. This value
           increments each time a complete scan cycle finishes.
 * @retval Current scan number.
 * @else
 * @brief  获取电容传感器当前扫描数，所有通道共用同一扫描数。只有在所有启用通道都被扫描一轮后，
           扫描数才会增加。
 * @retval 当前扫描数。
 * @endif
 */
uint32_t uapi_capsense_get_scan_num(void);

/**
 * @if Eng
 * @brief  Read all channel data from CAPSENSE. Data is returned in a 2D array format where thre
           first dimension represents the sample index and the second dimension represents the
 *         channel number.
 * @param  [out]  data Output data buffer pointer.
 *                     Type: int16_t 2D array.
 *                     Size requirement: data[num][CAPSENSE_CHANNEL_NUMBER].
 *                     Storage format: data[sample_index][channel_number].
 *                     Example: data[0] represents the array storing the first frame of data.
                                data[0][3] represents channel 3 data of the 1st sample.
 * @param  [in]  num Amount of data to be received.
                     Note: This parameter specifies the number of sample frames, NOT the total
                     data points.
 * @return int32_t Actual number of samples successfully read.
 *         - positive value: Number of samples successfully read.
 *         - -1: Fail.
 * @else
 * @brief  从电容传感器中读取所有通道的采样数据。数据以二维数组形式返回，第一维表示采样索引，第二维表示通道编号。
 * @param  [out] data 输出数据缓冲区指针。
 *                    数据类型：int16_t 二维数组。
 *                    尺寸要求：data[num][CAPSENSE_CHANNEL_MAX]。
 *                    存储格式：data[采样索引][通道号]。
 *                    示例：data[0] 表示存放第一帧数据的数组。data[0][3]表示通道3的第一帧数据。
 * @param  [in]  num 要读取的采样数量。
 *                   注意：此参数表示采样帧数，而非数据点总数，必须小于等于 data 数组的第一维大小。
 * @return int32_t 实际成功读取的采样数。
           - 正值：成功读取的采样数量。
           - -1：读取失败。
 * @endif
 */
int32_t uapi_capsense_read(int16_t data[][CAPSENSE_CHANNEL_MAX], uint32_t num);

/**
 * @if Eng
 * @brief  Read sensor data in compact format (only read the first ch_num channels).
 * @param  [out] data Output data buffer in compact format.
 *                    Data arrangement: [sample0_ch0, sample0_ch1, ..., sample0_chN-1,
 *                                       sample1_ch0, sample1_ch1, ..., sample1_chN-1,
 *                                       ...]
 *                    Size requirement: ch_num * num elements.
 * @param  [in]  ch_num Number of channels to read.
 * @param  [in]  num Number of samples (frames) to read for each channel.
 * @return int32_t Number of samples successfully read.
           - Positive value: Number of samples successfully read.
           - -1: Read operation failed.
 * @note   If channel mapping is enabled, data is rearranged according to logical order.
 * @else
 * @brief  以紧凑格式读取传感器数据（仅读取前ch_num个通道）。
 * @param  [out] data 紧凑格式的输出数据缓冲区。
 *                    数据排列：[采样0通道0, 采样0通道1, ..., 采样0通道N-1,
 *                              采样1通道0, 采样1通道1, ..., 采样1通道N-1,
 *                              ...]
 *                    大小要求：ch_num * num 个元素。
 * @param  [in]  ch_num 要读取的通道数量。
 * @param  [in]  num 每个通道要读取的采样数（帧数）。
 * @return int32_t 实际成功读取的采样数。
           - 正值：成功读取的采样数量。
           - -1：读取失败。
 * @note  如果启用了通道映射， 数据按照逻辑顺序重排。
 * @endif
 */
int32_t uapi_capsense_read_compact(int16_t *data, uint32_t ch_num, uint32_t num);

/**
 * @if Eng
 * @brief  Set CAPSENSE operation mode and register callback function.
 * @param  [in]  mode Operation mode to set. For details, see @ref capsense_mode_t.
 * @param  [in]  callback Event callback. For details, see @ref capsense_callback_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置CAPSENSE操作模式并注册回调函数。
 * @param  [in]  mode ，要设置的操作模式，参考 @ref capsense_mode_t 。
 * @param  [in]  callback 事件回调。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_capsense_set_mode(uint8_t mode, capsense_callback_t callback);

/**
 * @if Eng
 * @brief  Calibrate specified CAPSENSE channel.
 * @param  [in]  ch CAPSENSE channel to calibrate (logical channel if mapping enabled).
                 For details, see @ref capsense_channel_t.
 * @param  [out] result Pointer to store calibration result.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @note   Use logical channel numbers when channel mapping is enabled.
 *         Convert result to capacitance with uapi_capsense_convert_calib_to_ff().
 * @else
 * @brief  校准指定的CAPSENSE通道。
 * @param  [in]  ch 要校准的电容传感器通道（启用映射时为逻辑通道）。参考 @ref capsense_channel_t。
 * @param  [out] result 存储校准结果的指针。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @note   启用通道映射时使用逻辑通道号。
 *         使用 uapi_capsense_convert_calib_to_ff() 可以将校准结果转换为电容值。
 * @endif
 */
errcode_t uapi_capsense_calibration(uint8_t ch, uint16_t *result);

/**
 * @if Eng
 * @brief  Convert calibration result to capacitance value in femtofarads (fF).
 * @param  [in]  cali_val Calibration result value to be converted.
 * @param  [out] cap_ff   Converted capacitance value in femtofarads (fF).
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @note   This function converts the raw calibration value obtained from
 *         uapi_capsense_calibration() to standard capacitance units.
 *         1 fF = 10⁻¹⁵ farad.
 * @else
 * @brief  将校准结果转换为飞法拉(fF)为单位的电容值。
 * @param  [in]  cali_val 待转换的校准结果。
 * @param  [out] cap_ff   转换后的电容值，单位为飞法拉(fF)。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t。
 * @note   此函数将 uapi_capsense_calibration() 获取的校准结果转换为标准电容单位。
 *         1 fF = 10⁻¹⁵ 法拉。
 * @endif
 */
errcode_t uapi_capsense_convert_calib_to_ff(uint16_t cali_val, uint32_t *cap_ff);

/**
 * @if Eng
 * @brief  Start CAPSENSE operation.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  启动电容传感器运行。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_capsense_start(void);

/**
 * @if Eng
 * @brief  Stop CAPSENSE operation.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  停止电容传感器运行。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_capsense_stop(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
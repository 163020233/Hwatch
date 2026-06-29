/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2025. All rights reserved.
 * Description: audio manager c service header
 * Author: Media Software Group
 * Create: 2023-08-11
 */

#ifndef AUDIO_MANAGER_SERVICE_H
#define AUDIO_MANAGER_SERVICE_H

#include "audio_base_type.h"
#include "audio_manager.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_CONNECT_DEVICE_TYPE 4

typedef struct {
    uint8_t protocol; /*!< @if Eng Connection protocol.
                                        @else   连接协议 @endif */
    uint8_t vendorId; /*!< @if Eng vendor Id.
                                        @else   供应商Id @endif */
    uint8_t versionId; /*!< @if Eng version Id.
                                        @else   版本Id @endif */
} AudioL2HCVersion;

typedef struct {
    AudioL2HCVersion version; /*!< @if Eng Audio L2HC version information{@link AudioL2HCVersion}.
                                        @else   音频L2HC版本信息{@link AudioL2HCVersion} @endif */
    uint8_t enable; /*!< @if Eng Aggregation frame enable.
                                        @else   聚合帧使能 @endif */
    uint16_t frameLen; /*!< @if Eng The length of frame.
                                        @else   帧长 @endif */
} AudioL2HCConfig;

typedef enum {
    AUDIO_STREAM_EVENT_UNKNOWN,        /*!< @if Eng Unknown event.
                                            @else 未知事件。 @endif */
    AUDIO_STREAM_EVENT_CHANGE_VOLUME,   /*!< @if Eng The volume has changed.
                                            @else 音量变化事件 @endif */
    AUDIO_STREAM_EVENT_ERROR_OCCUR = 0xFFFFFFFFu,  /*!< @if Eng An error occurs.
                                            @else 发生错误。 @endif */
} AudioStreamCallbackEventType;

typedef int32_t (*AudioStreamCallBack)(const void *caller, AudioStreamCallbackEventType eventType, const void *info);

typedef struct {
    AudioCodecFormat audioFormat; /*!< @if Eng streamType audio stream format {@link AudioCodecFormat}.
                                        @else   音频流格式{@link AudioCodecFormat} @endif */
    AudioStreamType streamType; /*!< @if Eng streamType audio stream type {@link AudioStreamType}.
                                        @else   音频流类型{@link AudioStreamType} @endif */
    uint32_t sampleRate; /*!< @if Eng streamType audio stream sample rate.
                                        @else   音频流码率 */
    uint32_t sessionId; /*!< @if Eng streamType audio stream session id.
                                        @else   音频流会话id */
    uint32_t channelCount; /*!< @if Eng streamType audio stream channel count.
                                        @else   音频声道数 */
    uint16_t bitWidth; /*!< @if Eng streamType audio stream bitWidth.
                                        @else   音频流位宽{@link AudioStreamType} @endif */
    AudioLinkDirection direction; /*!< @if Eng Audio Streaming Direction {@link AudioLinkDirection}.
                                        @else   音频流方向{@link AudioLinkDirection} @endif */
    AudioDeviceType connectedDeviceType[MAX_CONNECT_DEVICE_TYPE]; /*!< @if Eng streamType audio stream connected
                                        device type {@link AudioDeviceType}.
                                        @else   音频流连接设备类型{@link AudioDeviceType} @endif */
    uint32_t micType; /*!< @if Eng streamType audio stream mic type {@link CapturerMicType}.
                                        @else   音频流麦克风{@link CapturerMicType} @endif */
    int32_t deviceId; /*!< @if Eng The audio device id.
                                        @else   音频设备id @endif */
    bool isSeaClose; /*!< @if Eng streamType audio stream is sea close.
                                        @else   音频语音增强是否关闭@endif */
    uint32_t connectedId; /*!< @if Eng streamType audio stream connected id.
                                        @else   音频连接id@endif */
    AudioParamCmd cmd; /*!< @if Eng Audio Param cmd {@link AudioParamCmd}.
                                        @else   音频参数控制ID{@link AudioParamCmd} @endif */
    char *param; /*!< @if Eng Audio parameter control ID corresponding parameters  {@link AudioL2HCConfig}.
                                        @else   音频参数控制ID对应参数{@link AudioL2HCConfig} @endif */
    uint32_t paramLen; /*!< @if Eng Length of the parameter{@link AudioL2HCConfig}.
                                        @else   音频参数的长度{@link AudioL2HCConfig} @endif */
    AudioStreamCallBack callBack; /*!< @if Eng Callback function of the audio stream{@link AudioStreamCallBack}.
                                        @else   数据流的回调函数{@link AudioStreamCallBack} @endif */
    const void *caller; /*!< @if Eng Caller of the audio stream callback function{@link AudioStreamCallBack}.
                                        @else   数据流回调函数的调用者{@link AudioStreamCallBack} @endif */
    uint32_t callerSize; /*!< @if Eng Caller Size of the audio stream callback function{@link AudioStreamCallBack}.
                                        @else   数据流回调函数的调用者长度{@link AudioStreamCallBack} @endif */
    bool isSync;        /*!< @if Eng Whether the current stream is synchronized across multiple devices.
                                        @else   当前流是否是多设备同步的 @endif */
    uint32_t frameLength; /*!< @if Eng Audio frame length{@link AudioFrameLength}.
                                        @else   音频帧长{@link AudioFrameLength} @endif */
} AudioStreamConfig;

typedef enum {
    AUDIO_VOLUME_MODULE_INPUT = 0, /*!< @if Eng Audio volume input module.
                                        @else   音频音量输入模块 @endif */
    AUDIO_VOLUME_MODULE_ENCODE = 1, /*!< @if Eng Audio volume encode module.
                                        @else   音频音量编码模块 @endif */
    AUDIO_VOLUME_MODULE_OUTPUT = 2, /*!< @if Eng Audio volume output module.
                                        @else   音频音量输出模块 @endif */
    AUDIO_VOLUME_MODULE_DECODE = 3, /*!< @if Eng Mic Audio volume decode module.
                                        @else   音频音量解码模块 @endif */
    AUDIO_VOLUME_MODULE_TRACK = 4, /*!< @if Eng Mic Audio volume track module.
                                        @else   音频音量track模块 @endif */
    AUDIO_VOLUME_MODULE_MAX,
    AUDIO_VOLUME_MODULE_BUTT = 0x7FFFFFFFu,
} AudioVolumeModule;

typedef enum {
    AUDIO_VOLUME_TYPE_ANA_PGA = 0, /*!< @if Eng chip analog pga.
                                     @else 芯片模拟增益 @endif */
    AUDIO_VOLUME_TYPE_DIG_PGA = 1, /*!< @if Eng chip digital pga.
                                     @else 芯片数字增益 @endif */
    AUDIO_VOLUME_TYPE_SOFT_GAIN = 2, /*!< @if Eng software gain.
                                       @else 软件音量 @endif */
    AUDIO_VOLUME_TYPE_MAX,
    AUDIO_VOLUME_TYPE_BUTT = 0x7FFFFFFFu,
} AudioVolumeType;

typedef enum {
    AUDIO_CHANNEL_COUNT0 = 0,/*!< @if Eng channel count is 0.
                                     @else 声道数是0 @endif */
    AUDIO_CHANNEL_COUNT1 = 1,/*!< @if Eng channel count is 1.
                                     @else 声道数是1 @endif */
    AUDIO_CHANNEL_COUNT2 = 2,/*!< @if Eng channel count is 2.
                                     @else 声道数是2 @endif */
    AUDIO_CHANNEL_COUNT3 = 3,/*!< @if Eng channel count is 3.
                                     @else 声道数是3 @endif */
    AUDIO_CHANNEL_MAX,
    AUDIO_CHANNEL_COUNT_BUTT = 0x7FFFFFFFu,
} AudioChannelCount;

typedef enum {
    AUDIO_SND_OUT_PORT_DAC0 = 0x00, /*!< @if Eng audio sound port DAC0.
                                     @else 音频输出端口DAC0 @endif */
    AUDIO_SND_OUT_PORT_I2S0 = 0x10, /*!< @if Eng audio sound port I2S0.
                                     @else 音频输出端口I2S0 @endif */
    AUDIO_SND_OUT_PORT_CAST0 = 0x70, /*!< @if Eng audio sound port CAST0.
                                     @else 音频输出端口CAST0 @endif */
    AUDIO_SND_OUT_PORT_ALL = 0x7fff,
    AUDIO_SND_OUT_PORT_MAX = 0x7FFFFFFFu,
} AudioSoundPortType;

typedef enum {
    AUDIO_SND_IN_PORT_I2S0 = 0x00, /*!< @if Eng audio sound in port I2S0.
                                     @else 音频输入端口I2S0 @endif */
    AUDIO_SND_IN_PORT_ADC0 = 0x10, /*!< @if Eng audio sound in port ADC0.
                                     @else 音频输入端口ADC0 @endif */
    AUDIO_SND_IN_PORT_ALL = 0x7fff,
    AUDIO_SND_IN_PORT_MAX = 0x7FFFFFFFu,
} AudioSoundInPortType;

typedef struct {
    AudioStreamType streamType; /*!< @if Eng streamType audio stream type {@link AudioStreamType}.
                                        @else   音频流类型{@link AudioStreamType} @endif */
    AudioLinkDirection direction; /*!< @if Eng Audio Streaming Direction {@link AudioLinkDirection}.
                                        @else   音频流方向{@link AudioLinkDirection} @endif */
    int32_t dbVolume; /*!< @if Eng The volume DB value.
                                        @else   音量DB值 @endif */
    AudioChannelCount channelCount; /*!< @if Eng The volume channel count {@link AudioChannelCount}.
                                        @else   音量声道数{@link AudioChannelCount} @endif */
    AudioVolumeType volumeType; /*!< @if Eng The volume type {@link AudioVolumeType}.
                                        @else   音量类型{@link AudioVolumeType} @endif */
    AudioVolumeModule volumeModule; /*!< @if Eng The volume module {@link AudioVolumeModule}.
                                        @else   音量模式{@link AudioVolumeModule}@endif */
    uint32_t port; /*!< @if Eng The volume port {@link AudioSoundPortType#AudioSoundInPortType}.
                                        @else   音量端口{@link AudioSoundPortType} @endif */
    uint32_t deviceId; /*!< @if Eng The audio device id.
                                        @else   音频设备id @endif */
    bool isMixVolumeTriggered; /*!< @if Eng Is it triggered by the mixing volume?
                                        @else   是否是混音音量触发 @endif */
} AudioVolumeConfig;

typedef enum {
    AUDIO_MIC_MIX_MODE_MONO, /*!< @if Eng Mic mix mode mono.
                                        @else   麦克风混音模式单声道 @endif */
    AUDIO_MIC_MIX_MODE_STEREO, /*!< @if Eng Mic mix mode Stereo.
                                          @else   麦克风混音模式立体声 @endif */
    AUDIO_MIC_MIX_MODE_SAFETY_TRACK, /*!< @if Eng Mic mix mode safety track.
                                                 @else   麦克风混音模式安全音轨 @endif */
    AUDIO_MIC_MIX_MODE_MAX,
    AUDIO_MIC_MIX_MODE_BUTT = 0x7FFFFFFFu,
} AudioMixMode;

typedef enum {
    AUDIO_MIC_MODE_NULL, /*!< @if Eng Not Mic mode.
                                    @else   非麦克风模式 @endif */
    AUDIO_MIC_MODE_1T1R, /*!< @if Eng 1 transmitter and 1 receiver.
                                    @else   1个发射器搭配1个接收器 @endif */
    AUDIO_MIC_MODE_2T1R, /*!< @if Eng 2 transmitter and 1 receiver.
                                     @else   2个发射器搭配1个接收器 @endif */
    AUDIO_MIC_MODE_4T1R, /*!< @if Eng 4 transmitter and 1 receiver.
                                   @else   4个发射器搭配1个接收器 @endif */
    AUDIO_MIC_MODE_8T1R, /*!< @if Eng 8 transmitter and 1 receiver.
                                   @else   8个发射器搭配1个接收器 @endif */
    AUDIO_MIC_MODE_MAX,
    AUDIO_MIC_MODE_BUTT = 0x7FFFFFFFu,
} AudioMicMode;

typedef enum {
    AUDIO_AEF_TYPE_NULL,    /*!< @if Eng NULL audio effect.
                                @else   无效音效 @endif */
    AUDIO_AEF_TYPE_SWS,     /*!< @if Eng SWS audio effect.
                                @else   SWS音效 @endif */
    AUDIO_AEF_TYPE_SMARTPA, /*!< @if Eng Smart PA audio effect.
                                @else   Smart PA音效 @endif */
    AUDIO_AEF_TYPE_WDRC,    /*!< @if Eng WDRC audio effect for hearing aid.
                               @else    助听WDRC音效 @endif */
    AUDIO_AEF_TYPE_AVC,     /*!< @if Eng Auto volume control.
                                @else   自动音量控制 @endif */
    AUDIO_AEF_TYPE_GEQ,     /*!< @if Eng GEQ audio effect.
                                @else   GEQ音效 @endif */
    AUDIO_AEF_TYPE_DRC,     /*!< @if Eng Dynamic range control.
                                @else   音频动态范围控制 @endif */
    AUDIO_AEF_TYPE_PEQ,     /*!< @if Eng PEQ audio effect.
                                @else   PEQ音效 @endif */
    AUDIO_AEF_TYPE_AEF_PRE, /*!< @if Eng Pre-processing aef audio effect.
                                @else   AEF前处理音效 @endif */
    AUDIO_AEF_TYPE_HVS,     /*!< @if Eng hvs lite audio effect.
                                @else   HVS lite音效 @endif */
    AUDIO_AEF_TYPE_CUSTOMER = 0x80, /*!< @if Eng Customer audio effect.
                                        @else   客户自研音效 @endif */
    AUDIO_AEF_TYPE_MAX,
    AUDIO_AEF_TYPE_BUTT = 0xFFFFFFFu,
} AudioAefType;

typedef enum {
    AUDIO_SEA_AFE_NULL = 0x80,
    AUDIO_SEA_AFE_SEE,         /*!< @if Eng Speech Enhancement Engine
                                   @else   语音增强 @endif */
    AUDIO_SEA_AFE_VQE,         /*!< @if Eng Voice Quality Engine
                                   @else   语音通话 @endif */
    AUDIO_SEA_AFE_AHE,         /*!< @if Eng Augment Hearing Engine
                                   @else   人声增强 @endif */
    AUDIO_SEA_AFE_MAX
} AudioSeaAfeType;

#define AUDIO_AEF_CMD(AudioAefType, num) (uint32_t)(((AudioAefType) << 16) | (uint32_t)(num))
#define AUDIO_SEA_CMD(AudioSeaAfeType, num) (uint32_t)(((num) << 16) | (AudioSeaAfeType))

typedef enum {
    /*!< @if Eng Read and Write AFE SEE param command.
         @else   获取和设置语音增强算法参数命令 @endif */
    AUDIO_SEA_AFE_SEE_RW_PARAM_CMD = AUDIO_SEA_CMD(AUDIO_SEA_AFE_SEE, 0),

    /*!< @if Eng Read and Write AFE SEE Gain command.
        @else   获取和设置语音增强算法音量命令 @endif */
    AUDIO_SEA_AFE_SEE_RW_GAIN_CMD = AUDIO_SEA_CMD(AUDIO_SEA_AFE_SEE, 1),

    /*!< @if Eng Read and Write AFE SEE nr param command.
         @else   获取和设置语音增强算法降噪参数命令 @endif */
    AUDIO_SEA_AFE_SEE_RW_NR_PARAM_CMD = AUDIO_SEA_CMD(AUDIO_SEA_AFE_SEE, 2),

    /*!< @if Eng Read and Write AFE SEE usr param command.
         @else   获取和设置语音增强算法通用参数命令 @endif */
    AUDIO_SEA_AFE_SEE_RW_COMMON_PARAM_CMD = AUDIO_SEA_CMD(AUDIO_SEA_AFE_SEE, 3),

    /*!< @if Eng Read and Write AFE SEE agc param command.
         @else   获取和设置语音增强算法AGC参数命令 @endif */
    AUDIO_SEA_AFE_SEE_RW_AGC_PARAM_CMD = AUDIO_SEA_CMD(AUDIO_SEA_AFE_SEE, 4),

    /*!< @if Eng Read and Write AFE SEE bypass param command.
         @else   获取和设置语音增强算法bypass参数命令 @endif */
    AUDIO_SEA_AFE_SEE_RW_BYPASS_PARAM_CMD  = AUDIO_SEA_CMD(AUDIO_SEA_AFE_SEE, 5),

    AUDIO_SEA_AFE_RW_PARAM0_CMD = AUDIO_SEA_CMD(AUDIO_SEA_AFE_SEE, 0xe0),
    AUDIO_SEA_AFE_RW_PARAM1_CMD = AUDIO_SEA_CMD(AUDIO_SEA_AFE_SEE, 0xe1),
    AUDIO_SEA_AFE_RW_PARAM2_CMD = AUDIO_SEA_CMD(AUDIO_SEA_AFE_SEE, 0xe2),
    AUDIO_SEA_AFE_RW_PARAM3_CMD = AUDIO_SEA_CMD(AUDIO_SEA_AFE_SEE, 0xe3),
    AUDIO_SEA_AFE_RW_PARAM4_CMD = AUDIO_SEA_CMD(AUDIO_SEA_AFE_SEE, 0xe4),
    AUDIO_SEA_AFE_RW_PARAM5_CMD = AUDIO_SEA_CMD(AUDIO_SEA_AFE_SEE, 0xe5),

    /*!< @if Eng Read and Write AFE VQE param command.
         @else   获取和设置语音通话算法参数命令 @endif */
    AUDIO_SEA_AFE_VEQ_RW_PARAM_CMD = AUDIO_SEA_CMD(AUDIO_SEA_AFE_VQE, 0),

    /*!< @if Eng Read and Write AFE AHE param command.
         @else   获取和设置人声增强算法参数命令 @endif */
    AUDIO_SEA_AFE_AHE_RW_PARAM_CMD = AUDIO_SEA_CMD(AUDIO_SEA_AFE_AHE, 0),
    AUDIO_SEA_AFE_CMD_MAX,
    AUDIO_SEA_AFE_CMD_BUTT = 0x7FFFFFFFu,
} AudioSeaAfeCmdId;

typedef enum {
    /*!< @if Eng Read and Write PEQ param command.
         @else   获取和设置PEQ音效参数命令 @endif */
    AUDIO_PEQ_RW_PARAM_CMD = AUDIO_AEF_CMD(AUDIO_AEF_TYPE_PEQ, 0),

    /*!< @if Eng Read and Write COM EQ biquad filter coef command.
         @else   获取和设置通用EQ滤波器系数命令 @endif */
    AUDIO_PEQ_RW_COM_EQ_CH0_COEF_CMD = AUDIO_AEF_CMD(AUDIO_AEF_TYPE_PEQ, 1),

    /*!< @if Eng Read and Write COM EQ biquad filter coef command.
         @else   获取和设置通用EQ滤波器系数命令 @endif */
    AUDIO_PEQ_RW_COM_EQ_CH1_COEF_CMD = AUDIO_AEF_CMD(AUDIO_AEF_TYPE_PEQ, 2),

    /*!< @if Eng Read and Write calibration EQ biquad filter coef command.
         @else   获取和设置校准EQ滤波器系数命令 @endif */
    AUDIO_PEQ_RW_CALIB_EQ_CH0_COEF_CMD = AUDIO_AEF_CMD(AUDIO_AEF_TYPE_PEQ, 3),

    /*!< @if Eng Read and Write calibration EQ biquad filter coef command.
         @else   获取和设置校准EQ滤波器系数命令 @endif */
    AUDIO_PEQ_RW_CALIB_EQ_CH1_COEF_CMD = AUDIO_AEF_CMD(AUDIO_AEF_TYPE_PEQ, 4),

    /*!< @if Eng Read and Write DRC param command.
         @else   获取和设置DRC音效参数命令 @endif */
    AUDIO_DRC_RW_PARAM_CMD = AUDIO_AEF_CMD(AUDIO_AEF_TYPE_DRC, 0),
    /*!< @if Eng Read and Write HVS param command.
         @else   获取和设置HVS音效参数命令 @endif */
    AUDIO_HVS_RW_PARAM_CMD = AUDIO_AEF_CMD(AUDIO_AEF_TYPE_HVS, 0),
    AUDIO_PEQ_CMD_MAX,
    AUDIO_PEQ_CMD_BUTT = 0x7FFFFFFFu,
} AudioAefCmd;

typedef enum {
    AUDIO_CHANNEL_MONO = 0, /*!< @if Eng Mixing the left and the right audio channels.
                                      @else   左右声道混合 @endif */
    AUDIO_CHANNEL_LEFT = 1, /*!< @if Eng The left audio channel.
                                      @else   左声道 @endif */
    AUDIO_CHANNEL_RIGHT = 2, /*!< @if Eng The right audio channel.
                                      @else   右声道 @endif */
    AUDIO_CHANNEL_MODE_MAX,
    AUDIO_CHANNEL_MODE_BUTT = 0x7FFFFFFFu,
} AudioChannelType;

typedef struct {
    uint32_t id;
    AudioChannelType channelType;
} AudioChannelSelect;

typedef enum {
    AUDIO_ENCODE_PARAM_BIT_RATE, /*!< @if Eng Setting the bit rate type.
                                      @else   设置码率类型 @endif */
    AUDIO_ENCODE_PARAM_CHANNEL_MODE, /*!< @if Eng Setting the left and right audio channel types.
                                      @else   设置左右声道类型 @endif */
    AUDIO_ENCODE_PARAM_BGC_RATE_CFG, /*!< @if Eng Setting the BGC bit rate parameter type.
                                      @else   设置BGC码率参数类型 @endif */
    AUDIO_ENCODE_PARAM_DATA_SOURCE_CFG, /*!< @if Eng Setting the BGC bit rate parameter type.
                                      @else   设置数据参数类型 @endif */
    AUDIO_ENCODE_PARAM_MAX,
    AUDIO_ENCODE_PARAM_BUTT = 0x7FFFFFFFu,
} AudioEncodeParamType;

typedef struct {
    AudioStreamType streamType; /*!< @if Eng streamType audio stream type {@link AudioStreamType}.
                                        @else   音频流类型{@link AudioStreamType} @endif */
    uint32_t num; /*!< @if Eng Number of encoders.
                                        @else   编码器数量 @endif */
    const int8_t *config; /*!< @if Eng parameter configuration.
                                        @else   参数配置 @endif */
    uint32_t configLen; /*!< @if Eng Length of the parameter setting.
                                        @else   参数配置的长度 @endif */
    AudioEncodeParamType encodeParamType; /*!< @if Eng encode param type{@link AudioEncodeParamType}.
                                        @else   编码格式参数类型 {@link AudioEncodeParamType} @endif */
} AudioEncodeParamConfig;

/**
 * @if Eng
 * @brief Set the live TV microphone mode.
 *
 * @param [in] micMode Live wheat form mode  {@link AudioMicMode}.
 * @return Returns {@code ERRCODE_SUCC} if set the mic mode successfully;
 * Otherwise, the error code defined in {@link errcode.h} is returned.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置直播麦克风模式。
 *
 * @param [in] micMode 直播模式{@link AudioMicMode}。
 * @return 如果成功设置麦克风模式，则返回{@code ERRCODE_SUCC}；
 * 否则返回{@link errcode.h}中定义的错误代码
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LiveMicSetMicMode(int32_t micMode);

/**
 * @if Eng
 * @brief get the live TV microphone mode.
 *
 * @param [in] micMode to be obtained  {@link AudioMicMode}.
 * @return Returns {@code ERRCODE_SUCC} if get the mic mode successfully;
 * Otherwise, the error code defined in {@link errcode.h} is returned.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取直播麦克风模式。
 *
 * @param [in] micMode 待获取{@link AudioMicMode}。
 * @return 如果成功获取麦克风模式，则返回{@code ERRCODE_SUCC}；
 * 否则返回{@link errcode.h}中定义的错误代码
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LiveMicGetMicMode(int32_t *micMode);

/**
 * @if Eng
 * @brief Set the audio mixing mode.
 *
 * This function must be called after {@link AudioStreamCreate}.
 *
 * @param [in] mode Audio mixing mode to be set {@link AudioMixMode}.
 * @return Returns {@code ERRCODE_SUCC} if the audio mixing mode is set successfully;
 * Otherwise, the error code defined in {@link errcode.h} is returned.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置混音模式。
 *
 * 此函数必须在{@link AudioStreamCreate}之后调用。
 *
 * @param [in] mode 要设置的混音模式{@link AudioMixMode}。
 * @return 如果成功设置混音模式，则返回{@code ERRCODE_SUCC}；
 * 否则返回{@link errcode.h}中定义的错误代码
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LiveMicSetMixMode(int32_t mixMode);

/**
 * @if Eng
 * @brief Get the audio mixing mode.
 *
 * @param [out] mode Audio mixing mode to be obtained {@link AudioMixMode}.
 * @return Returns {@code ERRCODE_SUCC} if the audio mixing mode is obtained successfully;
 * Otherwise, the error code defined in {@link errcode.h} is returned.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取混音模式。
 *
 * @param [out] mode 获取混音模式{@link AudioMixMode}。
 * @return 如果成功获取混音模式，则返回{@code ERRCODE_SUCC}；
 * 否则返回{@link errcode.h}中定义的错误代码
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LiveMicGetMixMode(int32_t *mixMode);

/**
 * @if Eng
 * @brief Audio streaming service initialization.
 *
 * This service requests audio focus and generates AudioSession.
 *
 * @param [in] streamType audio stream type  {@link AudioStreamType}.
 * @param [in] interruptListener Audio focus policy processing callback event {@link InterruptListener}.
 * @param [out] sessionId Audio Sessions {@link AudioSession}.
 * @return Returns {@code ERRCODE_SUCC} if the audio streaming service is successfully initialized;
 * returns an error code defined.
 * in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 音频流服务初始化。
 *
 * 该服务请求音频焦点并生成AudioSession。
 *
 * @param [in] streamType 音频流类型{@link AudioStreamType}。
 * @param [in] interruptListener 音频焦点策略处理回调事件{@link InterruptListener}。
 * @param [out] sessionId 音频会话数{@link AudioSession}。
 * @return 如果成功初始化音频流服务，则返回{@code ERRCODE_SUCC}；
 * 否则返回{@link errcode.h}中定义的错误代码
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioManagerInitStreamService(AudioStreamType streamType, InterruptListener *interruptListener,
    AudioSession *sessionId);

/**
 * @if Eng
 * @brief Audio Stream Creation.
 *
 * This function must be called after {@link AudioManagerInitStreamService}.
 *
 * @param [in] config Audio stream configuration information  {@link AudioStreamConfig}.
 * @return Returns {@code ERRCODE_SUCC} If the stream is successfully created; returns an error code defined
 * in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
* @brief 音频流创建。
 *
 * 该函数必须在{@link AudioManagerInitStreamService}之后调用。
 *
 * @param [in] config 音频流配置信息{@link AudioStreamConfig}。
 * @return 如果成功创建流则返回{@code ERRCODE_SUCC}；
 * 否则返回{@link errcode.h}中定义的错误代码
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioStreamCreate(const AudioStreamConfig *config);

/**
 * @if Eng
 * @brief Audio Stream Destruction.
 *
 * This function must be called after {@link AudioManagerInitStreamService}.
 *
 * @param [in] config Audio stream configuration information  {@link AudioStreamConfig}.
 * @return Returns {@code ERRCODE_SUCC} if the stream is destroyed successfully; returns an error code defined
 * in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 音频流销毁。
 *
 * 该函数必须在{@link AudioManagerInitStreamService}之后调用。
 *
 * @param [in] config 音频流配置信息{@link AudioStreamConfig}。
 * @return 如果成功销毁流返回{@code ERRCODE_SUCC}；
 * 否则返回{@link errcode.h}中定义的错误代码
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioStreamDestroy(const AudioStreamConfig *config);

/**
 * @if Eng
 * @brief Enable the Aef algorithm.
 *
 * This function must be called after {@link AudioStreamCreate}.
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @param [in] aefType Aef algorithm type{@link AudioAefType}.
 * @param [in] port Aef algorithm port{@link AudioSoundPortType}.
 * @param [in] enable Aef algorithm Enable.
 * @return Returns {@code ERRCODE_SUCC} if the AEF algorithm is enabled successfully; returns an error code defined
 * in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 启用Aef算法。
 *
 * 此函数必须在{@link AudioStreamCreate}之后调用。
 *
 * @param [in] streamType 音频流类型{@link AudioStreamType}。
 * @param [in] aefType 音效算法类型{@link AudioAefType}。
 * @param [in] port 音效算法输出端口{@link AudioSoundPortType}。
 * @param [in] enable Aef算法使能。
 * @return 如果成功启用AEF算法，则返回{@code ERRCODE_SUCC}；
 * 否则返回{@link errcode.h}中定义的错误代码
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioStreamEnableAef(AudioStreamType streamType, AudioAefType aefType, AudioSoundPortType port, bool enable);

/**
 * @if Eng
 * @brief Setting AEF Parameters for Audio Streams.
 *
 * This function must be called after {@link AudioStreamCreate}.
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @param [in] cmd AEF algorithm type {@link AudioAefCmd}.
 * @param [in] port Aef algorithm port{@link AudioSoundPortType}.
 * @param [in] param AEF Algorithm parameters to be set.
 * @param [in] len Length of the AEF algorithm parameter to be set.
 * @return Returns {@code ERRCODE_SUCC} if the aef algorithm is successfully set; returns an error code defined
 * in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置音频流的AEF参数。
 *
 * 此函数必须在{@link AudioStreamCreate}之后调用。
 *
 * @param [in] streamType 音频流类型{@link AudioStreamType}。
 * @param [in] cmd AEF算法类型 {@link AudioAefCmd}。
 * @param [in] port 音效算法输出端口{@link AudioSoundPortType}。
 * @param [in] param 要设置的AEF算法参数。
 * @param [in] len 要设置的AEF算法参数的长度。
 * @return 如果成功设置aef算法，则返回{@code ERRCODE_SUCC}；
 * 否则返回{@link errcode.h}中定义的错误代码
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioStreamSetAefParam(AudioStreamType streamType, AudioAefCmd cmd, AudioSoundPortType port, const char *param, uint32_t len);

/**
 * @if Eng
 * @brief Set the encoding bitrate for the audio stream.
 *
 * This function must be called after {@link AudioStreamCreate}.
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @param [in] rate audio stream bit rate.
 * @param [in] direction stream direction.
 * @return Returns {@code ERRCODE_SUCC} if the rate is successfully set; returns an error code defined
 * in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置音频流的编码码率。
 *
 * 此函数必须在{@link AudioStreamCreate}之后调用。
 *
 * @param [in] streamType 音频流类型{@link AudioStreamType}。
 * @param [in] rate 码率。
 * @param [in] direction 流方向。
 * @return 如果成功设置码率，则返回{@code ERRCODE_SUCC}；
 * 否则返回{@link errcode.h}中定义的错误代码
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioStreamSetBitRate(AudioStreamType streamType, uint32_t rate, AudioLinkDirection direction);

/**
 * @if Eng
 * @brief Set the audio channels for left and right ears.
 *
 * This function must be called after {@link AudioStreamCreate}.
 *
 * @param [in] encodeConfig Audio Encoding Configuration Parameters.{@link AudioEncodeParamConfig}.
 * @return Returns {@code ERRCODE_SUCC} if the audio channels for left and right ears is successfully set;
 * returns an error code defined in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置左右耳声道。
 *
 * 此函数必须在{@link AudioStreamCreate}之后调用。
 *
 * @param [in] encodeConfig 音频编码配置参数{@link AudioEncodeParamConfig}。
 * @return 如果成功设置左右耳声道，则返回{@code ERRCODE_SUCC}；
 * 否则返回{@link errcode.h}中定义的错误代码
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioStreamSetEncodeParam(AudioEncodeParamConfig *encodeConfig);

/**
 * @if Eng
 * @brief Getting AEF Parameters for Audio Streams.
 *
 * This function must be called after {@link AudioStreamCreate}.
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @param [in] cmd AEF algorithm type {@link AudioAefCmd}.
 * @param [in] port AEF algorithm port {@link AudioSoundPortType}.
 * @param [out] param AEF Algorithm parameters to be obtained.
 * @param [in] len Length of the AEF algorithm parameter to be obtained.
 * @return Returns {@code ERRCODE_SUCC} if the aef algorithm is obtained successfully;
 * returns an error code defined in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取音频流的AEF参数。
 *
 * 此函数必须在{@link AudioStreamCreate}之后调用。
 *
 * @param [in] streamType 音频流类型{@link AudioStreamType}。
 * @param [in] cmd AEF算法类型 {@link AudioAefCmd}。
 * @param [in] port AEF算法端口 {@link AudioSoundPortType}。
 * @param [out] param 需要获取的AEF算法参数。
 * @param [in] len 需要获取的AEF算法参数长度。
 * @return如果成功获取aef算法，则返回{@code ERRCODE_SUCC}；
 * 否则返回{@link errcode.h}中定义的错误代码
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioStreamGetAefParam(AudioStreamType streamType, AudioAefCmd cmd, AudioSoundPortType port, char *param, uint32_t len);

/**
 * @if Eng
 * @brief Setting SEA algorithm enable.
 *
 * This function must be called after {@link AudioStreamCreate}.
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @param [in] seaType SEA type {@link AudioSeaAfeType}.
 * @param [in] enable SEA algorithm enable.
 * @return Returns {@code ERRCODE_SUCC} If the sea enable status is obtained successfully;
 * returns an error code definedin {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置SEA算法使能。
 *
 * 此函数必须在{@link AudioStreamCreate}之后调用。
 *
 * @param [in] streamType 音频流类型{@link AudioStreamType}。
 * @param [in] seaType SEA类型 {@link AudioSeaAfeType}.
 * @param [in] enable SEA算法使能。
 * @return 返回{@code ERRCODE_SUCC}如果获取成功；
 * 否则返回{@link errcode.h}中定义的错误代码
 * @自1.0起
 * @版本1.0
 * @endif
 */

errcode_t AudioStreamEnableSea(AudioStreamType streamType, AudioSeaAfeType seaType, bool enable);

/**
 * @if Eng
 * @brief Setting Sea Parameters for Audio Streams.
 *
 * This function must be called after {@link AudioStreamCreate}.
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @param [in] cmd Audio afe sea cmd id {@link AudioSeaAfeCmdId}.
 * @param [in] param Sea Algorithm parameters to be set.
 * @param [in] len Length of the Sea algorithm parameter to be set.
 * @return Returns {@code ERRCODE_SUCC} if the sea algorithm is successfully set;
 * returns an error code defined in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置音频流的语音增强与智能参数。
 *
 * 此函数必须在{@link AudioStreamCreate}之后调用。
 *
 * @param [in] streamType 音频流类型{@link AudioStreamType}。
 * @param [in] cmd 音频sea cmd id {@link AudioSeaAfeCmdId}。
 * @param [in] param 要设置的参数。
 * @param [in] len 要设置的Sea算法参数的长度。
 * @return 如果设置算法参数成功返回{@code ERRCODE_SUCC}；
 * 否则返回{@link errcode.h}中定义的错误代码
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioStreamSetSeaParam(AudioStreamType streamType, AudioSeaAfeCmdId cmd, const char *param,
    uint32_t len);

/**
 * @if Eng
 * @brief Getting Sea Parameters for Audio Streams.
 *
 * This function must be called after {@link AudioStreamCreate}.
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @param [in] cmd Audio afe sea cmd id {@link AudioSeaAfeCmdId}.
 * @param [out] param Sea Algorithm parameters to be obtained.
 * @param [in] len Length of the Sea algorithm parameter to be obtained.
 * @return Returns {@code ERRCODE_SUCC} if the sea algorithm is obtained successfully;
 * returns an error code defined in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取音频流的语音增强与智能参数。
 *
 * 此函数必须在{@link AudioStreamCreate}之后调用。
 *
 * @param [in] streamType 音频流类型{@link AudioStreamType}。
 * @param [in] cmd 音频sea cmd id {@link AudioSeaAfeCmdId}。
 * @param [out] param 要获取的参数。
 * @param [in] len 要获取的Sea算法参数的长度。
 * @return 如果成功获取sea算法参数返回{@code ERRCODE_SUCC}；
 * 否则返回在{@link errcode.h}中定义的错误代码。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioStreamGetSeaParam(AudioStreamType streamType, AudioSeaAfeCmdId cmd, char *param,
    uint32_t len);

/**
 * @if Eng
 * @brief Setting the enable status of the low-cut mode.
 *
 * This function must be called after {@link AudioStreamCreate}.
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @param [in] enable Low-cut mode enable.
 * @return Returns {@code ERRCODE_SUCC} If the low cut is enabled successfully; returns an error code defined
 * in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置低切模式的使能状态。
 *
 * 此函数必须在{@link AudioStreamCreate}之后调用。
 *
 * @param [in] streamType 音频流类型{@link AudioStreamType}。
 * @param [in] enable 低切模式使能。
 * @return 如果成功使能低切，返回{@code ERRCODE_SUCC}；
 * 否则返回在{@link errcode.h}中定义的错误代码。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioStreamEnableLowCut(AudioStreamType streamType, bool enable);

/**
 * @if Eng
 * @brief Setting the noise reduction mode switch.
 *
 * This function must be called after {@link AudioStreamCreate}.
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @param [in] enable Noise reduction mode enable.
 * @return Returns {@code ERRCODE_SUCC} if the noise reduction function is enabled successfully;
 * returns an error code defined.
 * in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置降噪模式开关。
 *
 * 此函数必须在{@link AudioStreamCreate}之后调用。
 *
 * @param [in] streamType 音频流类型{@link AudioStreamType}。
 * @param [in] enable 降噪模式使能。
 * @return 如果降噪成功，则返回{@code ERRCODE_SUCC}；
 * 否则返回在{@link errcode.h}中定义的错误代码
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioStreamEnableDenoise(AudioStreamType streamType, bool enable);

/**
 * @if Eng
 * @brief Set high fidelity enabled.
 *
 * This function must be called after {@link AudioStreamCreate}.
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @param [in] enable high fidelity enable.
 * @return Returns {@code SUCCESS} if set the setting is successfully; returns an error code defined
 * in {@link audio_errors.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置高保真启用。
 *
 * 此函数必须在 {@link AudioStreamCreate} 之后调用。
 *
 * @param [in] streamType 音频流类型 {@link AudioStreamType}。
 * @param [in] enable 高保真启用。
 * @return 如果设置成功，返回 {@code SUCCESS}；否则返回在 {@link audio_errors.h} 中定义的错误代码。
 * @自1.0起
 * @版本1.0
 */
errcode_t AudioStreamEnableHiFi(AudioStreamType streamType, bool enable);

/**
 * @if Eng
 * @brief Set the synchronization clock switchover parameters of the downlink audio stream.
 *
 * @param [in] streamType Indicates stream type.
 * @param [in] type Indicates synchronous clock type.
 * @param [in] toggleConfig Indicates synchronous clock switchover parameters.
 * @return Returns <b>0</b> If successful; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置下行音频流同步时钟切换参数。
 *
 * @param [in] streamType 流类型。
 * @param [in] type 表示同步时钟类型。
 * @param [in] toggleConfig 同步时钟切换参数。
 * @return 如果成功，则返回<b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamSetDownLinkToggleConfig(AudioStreamType streamType,
    ToggleConfigType type, ToggleConfig toggleConfig);


 /**
 * @if Eng
 * @brief Enable voice enhancement.
 *
 * This function must be called after {@link AudioStreamCreate}.
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @param [in] enable Noise reduction mode enable.
 * @return Returns {@code SUCCESS} if set the setting is successfully; returns an error code defined
 * in {@link audio_errors.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 启用语音增强。
 *
 * 此函数必须在 {@link AudioStreamCreate} 之后调用。
 *
 * @param [in] streamType 音频流类型 {@link AudioStreamType}。
 * @param [in] enable 降噪模式启用。
 * @return 如果设置成功，返回 {@code SUCCESS}；否则返回在 {@link audio_errors.h} 中定义的错误代码。
 * @自1.0起
 * @版本1.0
 */
errcode_t AudioStreamEnableVoiceEnhance(AudioStreamType streamType, bool enable);

/**
 * @if Eng
 * @brief Obtains sound energy parameters.
 *
 * This function must be called after {@link AudioStreamCreate}.
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @param [out] deviceId Device ID parameter.
 * @param [in] energy Sound energy parameters.
 * @return Returns {@code ERRCODE_SUCC} if the energy parameters are obtained successfully;
 * returns an error code defined.
 * in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取音频流声音能量参数。
 *
 * 此函数必须在{@link AudioStreamCreate}之后调用。
 *
 * @param [in] streamType 音频流类型{@link AudioStreamType}。
 * @param [out] deviceId 设备ID参数。
 * @param [in] energy 声音能量参数。
 * @return 如果获取能量参数成功,则返回{@code ERRCODE_SUCC};
 * 否则返回在{@link errcode.h}中定义的错误代码
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioStreamGetEnergyParam(AudioStreamType streamType, int32_t deviceId, int32_t *energy);

/**
 * @if Eng
 * @brief Setting the DB Value Volume.
 *
 * This function must be called after {@link AudioStreamCreate}.
 *
 * @param [in] volumeConfig audio stream volume config {@link AudioVolumeConfig}.
 * @return Returns {@code ERRCODE_SUCC} if the db volume is set successfully; returns an error code defined
 * in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置分贝音量大小。
 *
 * 此函数必须在{@link AudioStreamCreate}之后调用。
 *
 * @param [in] volumeConfig 音频流音频配置{@link AudioVolumeConfig}。
 * @return 如果设置db音量值成功，则返回{@code ERRCODE_SUCC}；
 * 否则返回在{@link errcode.h}中定义的错误代码。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioManagerSetDbVolume(const AudioVolumeConfig *volumeConfig);

/**
 * @if Eng
 * @brief Getting the DB Value Volume.
 *
 * This function must be called after {@link AudioStreamCreate}.
 *
 * @param [in] volumeConfig audio stream volume config {@link AudioVolumeConfig}.
 * @return Returns {@code ERRCODE_SUCC} if the db volume is obtained successfully; returns an error code defined
 * in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取分贝音量大小。
 *
 * 此函数必须在{@link AudioStreamCreate}之后调用。
 *
 * @param [in] volumeConfig 音频流音频配置{@link AudioVolumeConfig}。
 * @return 如果成功获取db 音量值，则返回{@code ERRCODE_SUCC}；
 * 否则返回在{@link errcode.h}中定义的错误代码。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioManagerGetDbVolume(AudioVolumeConfig *volumeConfig);

/**
 * @if Eng
 * @brief Set the microphone type.
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @param [in] micType Indicates the type of the microphone. {@link CapturerMicType}.
 * @return Returns {@code ERRCODE_SUCC} if set the mic type successfully; returns an error code defined
 * in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置麦克风类型。
 *
 * @param [in] streamType 音频流类型{@link AudioStreamType}。
 * @param [in] micType 麦克风类型。{@link CapturerMicType}。
 * @return 如果设置麦克风类型成功，则返回{@code ERRCODE_SUCC}；
 * 否则返回在{@link errcode.h}中定义的错误代码。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioManagerSetMicType(AudioStreamType streamType, uint32_t micType);

/**
 * @if Eng
 * @brief Setting Speaker Calibration Parameters.
 *
 * This function must be called after {@link AudioStreamCreate}.
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @param [in] caliCoef Speaker Calibration Parameters.
 * @param [in] len Speaker Calibration Parameter Length.
 * @return Returns {@code ERRCODE_SUCC} if the speaker calibration parameters are set successfully;
 * returns an error code defined
 * in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置扬声器校准参数。
 *
 * 此函数必须在{@link AudioStreamCreate}之后调用。
 *
 * @param [in] streamType 音频流类型{@link AudioStreamType}。
 * @param [in] caliCoef 喇叭校准参数
 * @param [in] len 喇叭校准参数长度。
 * @return 如果设置喇叭校准参数成功，则返回{@code ERRCODE_SUCC}；
 * 否则返回在{@link errcode.h}中定义的错误代码。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioManagerSetSpkCaliCoef(AudioStreamType streamType, const char *caliCoef, uint16_t len);

/**
 * @if Eng
 * @brief Setting the calibration volume of the microphone.
 *
 * This function must be called after {@link AudioStreamCreate}.
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @param [in] micType Type of the microphone to be calibrated {@link AudioLinkDirection}.
 * @param [in] volInteger Integer part of the volume parameter.
 * @param [in] volDecimal Decimal part of the volume parameter.
 * @return Returns {@code ERRCODE_SUCC} if the microphone calibration parameters are set successfully;
 * returns an error code defined in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置麦克风的校准音量。
 *
 * 此函数必须在{@link AudioStreamCreate}之后调用。
 *
 * @param [in] streamType 音频流类型{@link AudioStreamType}。
 * @param [in] micType 待校准麦克风类型{@link AudioLinkDirection}。
 * @param [in] volInteger db音量的整数部分。
 * @param [in] volDecimal db音量的小数部分。
 * @return设置麦克风校准参数成功，则返回{@code ERRCODE_SUCC}；
 * 否则返回在{@link errcode.h}中定义的错误代码。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioManagerSetMicCaliVolume(AudioStreamType streamType, uint32_t micType,
    int32_t volInteger, int32_t volDecimal);

/**
 * @if Eng
 * @brief Whether the audio output port is enabled.
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @param [in] portType auido output port type {@link SoundPortType}.
 * @param [in] enable audio output port enable.
 * @return Returns {@code ERRCODE_SUCC} If the port enable is successful;
 * returns an error code defined in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 是否启用音频输出端口。
 *
 * @param [in] streamType 音频流类型 {@link AudioStreamType}。
 * @param [in] portType 音频输出端口类型 {@link SoundPortType}。
 * @param [in] enable 音频输出端口启用状态。
 * @return 如果端口启用成功，返回 {@code ERRCODE_SUCC}；
 * 否则返回在 {@link errcode.h} 中定义的错误代码。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioManagerEnableAudioOutputPort(AudioStreamType streamType, AudioSoundPortType portType, bool enable);

/**
 * @if Eng
 * @brief Deinitializes the audio stream service..
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @return Returns {@code ERRCODE_SUCC} If the audio stream service is successfully deinitialized;
 * returns an error code defined in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 去初始化音频流服务。
 *
 * @param [in] streamType 音频流类型{@link AudioStreamType}。
 * @return 如果音频流服务去初始化成功，则返回{@code ERRCODE_SUCC}；
 * 否则返回在{@link errcode.h}中定义的错误代码。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioManagerDeInitStreamService(AudioStreamType streamType);

/**
 * @if Eng
 * @brief Obtain audio stream data.
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @param [in] deviceId audio stream deviceId.
 * @param [out] buffer audio stream buffer.
 * @param [in] bufferSize audio stream bufferSize.
 * @return Returns {@code ERRCODE_SUCC} If the audio stream data is successfully obtained;
 * returns an error code defined in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取音频流数据。
 *
 * @param [in] streamType 音频流类型{@link AudioStreamType}。
 * @param [in] deviceId 音频流设备Id。
 * @param [out] buffer 音频流数据buffer。
 * @param [in] bufferSize 音频流数据大小。
 * @return 如果音频流数据成功获取，则返回{@code ERRCODE_SUCC}；
 * 否则返回在{@link errcode.h}中定义的错误代码。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioStreamReadData(AudioStreamType streamType, int32_t deviceId, uint8_t *buffer, uint32_t bufferSize);

/**
 * @if Eng
 * @brief Write audio stream data.
 *
 * @param [in] streamType audio stream type {@link AudioStreamType}.
 * @param [in] deviceId audio stream deviceId.
 * @param [in] buffer audio stream buffer.
 * @param [in] bufferSize audio stream bufferSize.
 * @return Returns {@code ERRCODE_SUCC} If the audio stream data is successfully writed;
 * returns an error code defined in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 写入音频流数据。
 *
 * @param [in] streamType 音频流类型{@link AudioStreamType}。
 * @param [in] deviceId 音频流设备Id。
 * @param [in] buffer 音频流数据buffer。
 * @param [in] bufferSize 音频流数据大小。
 * @return 如果音频流数据成功写入，则返回{@code ERRCODE_SUCC}；
 * 否则返回在{@link errcode.h}中定义的错误代码。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioStreamWriteData(AudioStreamType streamType, int32_t deviceId, const uint8_t *buffer,
    uint32_t bufferSize);

/**
 * @if Eng
 * @brief Setting the volume matrix.
 *
 * @param [in] port Volume Matrix Output Port {@link AudioSoundPortType}.
 * @param [in] config Volume matrix configuration.
 * @param [in] configLen Length of the volume matrix configuration.
 * @return Returns {@code ERRCODE_SUCC} If the volume matrix is successfully set;
 * returns an error code defined in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置音量矩阵。
 *
 * @param [in] port 音量矩阵输出端口 {@link AudioSoundPortType}。
 * @param [in] config 音量矩阵配置
 * @param [in] configLen 音量矩阵配置的长度。
 * @return 如果音量矩阵成功设置，则返回{@code ERRCODE_SUCC}；
 * 否则返回在{@link errcode.h}中定义的错误代码。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LiveMicSetVolumeMatrix(AudioSoundPortType port, const char *config, uint32_t configLen);

/**
 * @if Eng
 * @brief Getting the volume matrix.
 *
 * @param [in] port Volume Matrix Output Port {@link AudioSoundPortType}.
 * @param [out] config Volume matrix configuration.
 * @param [in] configLen Length of the volume matrix configuration.
 * @return Returns {@code ERRCODE_SUCC} If the volume matrix is successfully get;
 * returns an error code defined in {@link errcode.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取音量矩阵。
 *
 * @param [in] port 音量矩阵输出端口 {@link AudioSoundPortType}。
 * @param [out] config 音量矩阵配置
 * @param [in] configLen 音量矩阵配置的长度。
 * @return 如果音量矩阵成功获取，则返回{@code ERRCODE_SUCC}；
 * 否则返回在{@link errcode.h}中定义的错误代码。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LiveMicGetVolumeMatrix(AudioSoundPortType port, char *config, uint32_t configLen);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_MANAGER_C_WRAPPER_H */
/*
* Copyright (c) @HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
* Description: audio base type
* Author: Media Software Group
* Create: 2025-05-26
*/

#ifndef AUDIO_BASE_TYPE_H
#define AUDIO_BASE_TYPE_H

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t AudioSession;

enum {
    AUDIO_SESSION_ID_NONE    = 0,
};

typedef enum {
    OUTPUT_DEVICES_FLAG    = 0x1u,
    INPUT_DEVICES_FLAG     = 0x2u,
    DEVICES_FLAG_INVALID = 0xFFFFFFFF,
} DeviceFlag;

typedef enum {
    UNKNOWN = 0x0,
    IN_FLAG = 0x80000000,
    OUT_BUILTIN_SPEAKER = 0x1,
    OUT_BLUETOOTH_SCO = 0x2,
    OUT_NEARLINK_ISO_CALL = 0x4,
    OUT_NEARLINK_ISO_MUSIC = 0x8,
    OUT_BLUETOOTH_A2DP = 0x10,
    OUT_MODEM = 0x20,
    OUT_MODEM_HEADSET = 0x40,
    OUT_USB = 0x80,
    OUT_USB_DONGLE = 0x100,
    OUT_NEARLINK_DONGLE = 0x80000,
    IN_BUILTIN_MIC = IN_FLAG | 0x200,
    IN_BLUETOOTH_SCO = IN_FLAG | 0x400,
    IN_NEARLINK_ISO_CALL = IN_FLAG | 0x800,
    IN_NEARLINK_ISO_MUSIC = IN_FLAG | 0x1000,
    IN_BLUETOOTH_A2DP = IN_FLAG | 0x2000,
    IN_MODEM = IN_FLAG | 0x4000,
    IN_MODEM_HEADSET = IN_FLAG | 0x8000,
    IN_USB = IN_FLAG | 0x10000,
    IN_USB_DONGLE = IN_FLAG | 0x20000,
    IN_NEARLINK_DONGLE = IN_FLAG | 0x40000,
} AudioDeviceType;

typedef struct {
    DeviceFlag      flag;
    AudioDeviceType device;
    const char      *name;
} AudioDeviceInfo;

typedef enum {
    AUDIO_DEVICE_CONNECTED        = 0x0u,
    AUDIO_DEVICE_DISCONNECTED     = 0x1u,
    AUDIO_DEVICE_INVALID = 0xFFFFFFFF,
} AudioDeviceConnectState;

typedef enum {
    AUDIO_STREAM_CONFIG_CMD_DEFAULT, /*!< @if Eng Audio parameter default cmd.
                                        @else   音频参数默认cmd @endif */
    AUDIO_STREAM_CONFIG_CMD_L2HC, /*!< @if Eng Audio parameter L2HC cmd.
                                        @else   音频参数L2HCcmd @endif */
    AUDIO_STREAM_CONFIG_CMD_MAX,
    AUDIO_STREAM_CONFIG_CMD_BUTT = 0x7FFFFFFFu,
} AudioParamCmd;

/**
 * @if Eng
 * @brief Describes ringer modes.
 *
 * @since 1.0
 * @version 1.0
 * @else
 * @brief描述振铃器模式。
 *
 * @自1.0起
 * @版本1.0
 * @endif
 */
typedef enum {
    /**
     * @if Eng
     * Indicates that the ringer mode is restored to the previous setting.
     * A device will be audible if the previous mode was sound.
     * @else
     * 表示恢复振铃模式为之前的设置。
     * 如果之前的模式是声音，则设备会发出声音。
     * @endif
     */
    RINGER_MODE_NORMAL = 0,
    /**
     * @if Eng
     * Indicates that a device is silent and vibrates. For calls, the device will vibrate,
     * and for message notifications, the vibration can be enabled by users.
     * @else
     * 表示设备静默，振动。对于呼叫，设备将振动，
     * 对于消息通知，用户可以开启振动。
     * @endif
     */
    RINGER_MODE_VIBRATE,
    /**
     * @if Eng
     * Indicates that a device is silent and does not vibrate. This mode overrides the vibration mode.
     * @else
     * 表示设备静默，不振动。此模式将覆盖振动模式。
     * @endif
     */
    RINGER_MODE_SILENT,
    /**
     * @if Eng
     * Invalid value
     * @else
     * 无效值
     * @endif
     */
    RINGER_MODE_INVALID = 0xFFFFFFFF,
} AudioRingMode;

/**
 * @if Eng
 * @brief Enumerates audio source types.
 *
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 枚举音频源类型。
 *
 * @自1.0起
 * @版本1.0
 * @endif
 */
typedef enum {
    AUDIO_SOURCE_INVALID = 0xFFFFFFFF, /*!< @if Eng Invalid audio source.
                                        @else   无效的音频源 @endif */
    AUDIO_SOURCE_DEFAULT = 0, /*!< @if Eng Default audio source.
                                        @else   默认音频源 @endif */
    AUDIO_MIC = 1, /*!< @if Eng Microphone.
                             @else   麦克风 @endif */
    AUDIO_VOICE_UPLINK = 2, /*!< @if Eng Uplink voice.
                                        @else   上行语音 @endif */
    /** Downlink voice */
    AUDIO_VOICE_DOWNLINK = 3, /*!< @if Eng Downlink voice.
                                        @else   下行语音 @endif */
    /** Voice call */
    AUDIO_VOICE_CALL = 4, /*!< @if Eng Voice call.
                                        @else   语音通话 @endif */
    /** Camcorder */
    AUDIO_CAMCORDER = 5, /*!< @if Eng Camcorder.
                                        @else   摄录一体机 @endif */
    /** Voice recognition */
    AUDIO_VOICE_RECOGNITION = 6, /*!< @if Eng Voice recognition.
                                        @else   语音识别 @endif */
    /** Voice communication */
    AUDIO_VOICE_COMMUNICATION = 7, /*!< @if Eng Voice communication.
                                        @else   语音通信 @endif */
    /** Remote submix */
    AUDIO_REMOTE_SUBMIX = 8, /*!< @if Eng Remote submix.
                                        @else   远程分混 @endif */
    /** Unprocessed audio */
    AUDIO_UNPROCESSED = 9, /*!< @if Eng Unprocessed audio.
                                        @else   未处理的音频 @endif */
    /** Voice performance */
    AUDIO_VOICE_PERFORMANCE = 10, /*!< @if Eng Voice performance.
                                        @else   语音性能 @endif */
    /** Echo reference */
    AUDIO_ECHO_REFERENCE = 1997, /*!< @if Eng Echo reference.
                                        @else   回声参考 @endif */
    /** Radio tuner */
    AUDIO_RADIO_TUNER = 1998, /*!< @if Eng Radio tuner.
                                        @else   收音机调谐器 @endif */
    /** Hotword */
    AUDIO_HOTWORD = 1999, /*!< @if Eng Hotword.
                                        @else   热词 @endif */
    /** Extended remote submix */
    AUDIO_REMOTE_SUBMIX_EXTEND = 10007, /*!< @if Eng Extended remote submix.
                                        @else   扩展远程子混合 @endif */
} AudioSourceType;

/**
 * @if Eng
 * @brief Audio Frame Length Enumeration.
 *
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 音频帧长枚举。
 *
 * @自1.0起
 * @版本1.0
 * @endif
 */
typedef enum {
    AUDIO_FRAME_LENGTH_2_5_MS = 25,    /*!< @if Eng 2.5 ms frame length.
                                            @else 2.5ms 帧长。 @endif */
    AUDIO_FRAME_LENGTH_5_MS = 50,      /*!< @if Eng 5 ms frame length.
                                            @else 5ms 帧长。 @endif */
    AUDIO_FRAME_LENGTH_10_MS = 100,    /*!< @if Eng 10 ms frame length.
                                            @else 10ms 帧长。 @endif */
    AUDIO_FRAME_LENGTH_20_MS = 200,    /*!< @if Eng 20 ms frame length.
                                            @else 20ms 帧长。 @endif */
    AUDIO_FRAME_LENGTH_40_MS = 400,    /*!< @if Eng 40 ms frame length.
                                            @else 40ms 帧长。 @endif */
    AUDIO_FRAME_LENGTH_60_MS = 600,    /*!< @if Eng 60 ms frame length.
                                            @else 60ms 帧长。 @endif */
    AUDIO_FRAME_LENGTH_80_MS = 800,    /*!< @if Eng 80 ms frame length.
                                            @else 80ms 帧长。 @endif */
    AUDIO_FRAME_LENGTH_100_MS = 1000,  /*!< @if Eng 100 ms frame length.
                                            @else 100ms 帧长。 @endif */
    AUDIO_FRAME_LENGTH_120_MS = 1200,  /*!< @if Eng 120 ms frame length.
                                            @else 120ms 帧长。 @endif */
    AUDIO_FRAME_LENGTH_MAX,
    AUDIO_FRAME_LENGTH_BUTT = 0xFFFFFFFFu,
} AudioFrameLength;

/**
 * @if Eng
 * @brief Defines the audio Device Descriptor.
 *
 * @since 1.0
 * @version 1.0
 * @else
 * @brief定义音频设备描述符。
 *
 * @自1.0起
 * @版本1.0
 * @endif
 */
typedef struct {
    uint8_t *deviceName;
    AudioSourceType inputSourceType;
    uint32_t deviceId;
} AudioDeviceDesc;

/**
 * @if Eng
 * @brief Enumerates audio codec formats.
 *
 * @since 1.0
 * @version 1.0
 * @else
 * @brief枚举音频编解码格式。
 *
 * @自1.0起
 * @版本1.0
 * @endif
 */
typedef enum {
    AUDIO_DEFAULT    = 0,
    PCM              = 1,
    AAC_LC           = 2, /*!< @if Eng Advanced Audio Coding Low Complexity (AAC-LC).
                                        @else   高级音频编码低复杂度(AAC-LC) @endif */
    AAC_HE_V1        = 3, /*!< @if Eng High-Efficiency Advanced Audio Coding (AAC-HE),
                                        previously known as AAC+ or aacPlus v1.
                                        @else   高效高级音频编码（AAC-HE），以前称为AAC+或aacPlus v1 @endif */
    AAC_HE_V2        = 4, /*!< @if Eng AAC++ or aacPlus v2.
                                        @else   AAC++或aacPlus v2 @endif */
    AAC_LD           = 5, /*!< @if Eng Advanced Audio Coding Low Delay (AAC-LD).
                                        @else   高级音频编码低时延(AAC-LD)  @endif */
    AAC_ELD          = 6, /*!< @if Eng Advanced Audio Coding Enhanced Low Delay (AAC-ELD).
                                        @else   高级音频编码增强型低延迟(AAC-ELD)  @endif */
    G711A            = 7,
    G711U            = 8,
    G726             = 9,
    OPUS             = 10,
    FLAC             = 11,
    VORBIS           = 12,
    APE              = 13,
    MP3              = 14,
    mSBC             = 15,
    SILK             = 16,
    SBC              = 17,
    L2HC             = 18,
    AMR_WB           = 19,
    LHDC             = 20,
    LDAC             = 21,
    L2HC_VOICE       = 22,
    AUDIO_FORMAT_MAX,
    FORMAT_INVALID = 0xFFFFFFFF,
} AudioCodecFormat;

/**
 * @if Eng
 * @brief 音频流类型定义.
 * 0xFFFF FFFFu: The most significant eight bits indicate the basic type category,
 * and the least significant eight bits indicate the subcategory.
 * @since 1.0
 * @version 1.0
 * @else
 * @简要音频流类型定义。
 * 0xFFFF FFFFu高8位表示基础类型分类,低8位表示子类
 * @自1.0起
 * @版本1.0
 * @endif
 */
typedef enum {
    /*!< @if Eng Default audio stream type. @else  默认音频流类型 @endif */
    TYPE_DEFAULT = -1,
    TYPE_MEDIA = 0,
    TYPE_VOICE_COMMUNICATION = 1,
    /*!< @if Eng None  @else 无 @endif */
    AUDIO_STREAM_NONE                       = 0x0u,
    /*!< @if Eng Hearing aid  @else 助听类型 @endif */
    AUDIO_STREAM_HEAR_AID                   = 0x00001000u,
    /*!< @if Eng None  @else 报警类 */
    AUDIO_STREAM_ALARM                      = 0x00010000u,
    /*!< @if Eng Equipment alarm: watch overtemperature alarm,
     vehicle engine fault alarm, steering system alarm, brake system fault alarm, etc.
     Health alarm: high heart rate and ambient noise alarm.
    @else 设备报警: 手表温度过高报警、车机上发动机故障报警、转向系统报警、刹车系统故障报警等
    健康报警:心率过高、环境噪声报警 @endif */
    AUDIO_STREAM_ALARM_SYSTEM               = 0x00010001u,
    /*!< @if Eng Alarm clock  @else  闹钟 @endif */
    AUDIO_STREAM_ALARM_CLOCK                = 0x00010002u,
    /*!< @if Eng Ringtone for incoming calls  @else 来电铃声 @endif */
    AUDIO_STREAM_RING                       = 0x00020000u,
    /*!< @if Eng Voice call  @else 语音通话 @endif */
    AUDIO_STREAM_VOICE_CALL                 = 0x00040000u,
    /*!< @if Eng Voice call  @else 语音通话 @endif */
    AUDIO_STREAM_VOICE_CALL_VOIP            = 0x00040001u,
    /*!< @if Eng Bluetooth Calling  @else  蓝牙通话 @endif */
    AUDIO_STREAM_VOICE_CALL_BT_SCO          = 0x00040002u,
    /*!< @if Eng 4G VoLTE call  @else  4G VOLTE通话 @endif */
    AUDIO_STREAM_VOICE_CALL_VOLTE           = 0x00040003u,
    /*!< @if Eng Live Mai  @else  直播麦 @endif */
    AUDIO_STREAM_LIVE_MIC                   = 0x00040004u,
    /*!< @if Eng USB voice call  @else  USB语音通话 @endif */
    AUDIO_STREAM_VOICE_CALL_USB             = 0x00040005u,
    /*!< @if Eng Nearlink voice call  @else  星闪语音通话 @endif */
    AUDIO_STREAM_VOICE_CALL_NEARLINK        = 0x00040006u,
    /*!< @if Eng Nearlink voice call  @else  4G VOLTE通话SPI方式 @endif */
    AUDIO_STREAM_VOICE_CALL_VOLTE_SPI       = 0x00040008u,
    /*!< @if Eng Voice assistant  @else  语音助手 @endif */
    AUDIO_STREAM_VOICE_ASSISTANT            = 0x00080004u,
    /*!< @if Eng speech synthesis  @else  语音合成 @endif */
    AUDIO_STREAM_TTS                        = 0x00100000u,
    /*!< @if Eng Prompt tone  @else  提示音 @endif */
    AUDIO_STREAM_NOTIFICATION               = 0x00200000u,
    /*!< @if Eng Device alert tone: Bluetooth disconnection and connection alert tone Low power message alert tone
    @else  设备提示音: 蓝牙断开、连接提示音、低电量、消息提示音 @endif */
    AUDIO_STREAM_NOTIFICATION_SYSTEM        = 0x00200001u,
    /*!< @if Eng Health alert tone  @else  运动健康提示音 @endif */
    AUDIO_STREAM_NOTIFICATION_PROMPT        = 0x00200002,
    /*!< @if Eng Media music  @else  媒体音 @endif */
    AUDIO_STREAM_MUSIC                      = 0x00400000u,
    /*!< @if Eng Bluetooth Input Media music  @else  蓝牙输入媒体音 @endif */
    AUDIO_STREAM_A2DP_MUSIC                 = 0x00400001u,
    /*!< @if Eng USB Music  @else  USB音乐 @endif */
    AUDIO_STREAM_USB_MUSIC                  = 0x00400002u,
    /*!< @if Eng Nearlink Music  @else  星闪音乐 @endif */
    AUDIO_STREAM_NEARLINK_MUSIC             = 0x00400003u,
    /*!< @if Eng Fitness video instruction  @else  健身视频指导提示音 @endif */
    AUDIO_STREAM_FITNESS_VIDEO              = 0x00800000u,
    /*!< @if Eng Voice collection and recording  @else  语音采集录制 @endif */
    AUDIO_STREAM_VOICE_RECORD               = 0x01000000u,
    /*!< @if Eng speech recognition  @else  语音识别 @endif */
    AUDIO_STREAM_VOICE_RECOGNITION          = 0x02000000u,
    /*!< @if Eng pickup audio data   @else  拾音通路 @endif */
    AUDIO_STREAM_VOICE_PICKUP               = 0x02000001u,
    /*!< @if Eng Invalidation  @else 无效 @endif */
    AUDIO_STREAM_INVALID                    = 0x7FFFFFFFu
} AudioStreamType;

/**
 * @if Eng
 * Describes audio stream Link Direction.
 *
 * @since 1
 * @else
 * 描述音频流的链接方向。
 *
 * @从1开始
 * @endif
 */
typedef enum {
    AUDIO_DOWN_LINK,
    AUDIO_UP_LINK,
    AUDIO_UP_DOWN_LINK,
    AUDIO_LINK_NONE = 0xFFFFFFFF,
} AudioLinkDirection;

typedef enum {
    AUDIO_STREAM_FLAG_NONE = 0x0,
    AUDIO_STREAM_FLAG_LOW_LATENCY = 0x1,
    AUDIO_STREAM_FLAG_SECURE = 0x2,
    AUDIO_STREAM_FLAG_INVALID = 0xFFFFFFFF,
} AudioStreamFlag;

/**
 * @if Eng
 * @brief Enumerates the audio channel mode.
 *
 * A mask describes an audio channel position.
 * @else
 * @brief枚举音频通道模式。
 *
 * 掩码描述了音频通道的位置。
 * @endif
 */
typedef enum {
    /**
     * @if Eng
     * Indicates an invalid channel mask.
     * @else
     * 无效的通道掩码。
     * @endif
     */
    AUDIO_CHANNEL_MODE_NONE = 0,
    /**
     * @if Eng
     * Indicates the mask of the front left output channel.
     * @else
     * 左前输出通道掩码。
     * @endif
     */
    AUDIO_CHANNEL_MODE_FRONT_LEFT = 0x1u,
    /**
     * @if Eng
     * Indicates the mask of the front right output channel.
     * @else
     * 右前输出通道掩码。
     * @endif
     */
    AUDIO_CHANNEL_MODE_FRONT_RIGHT = 0x2u,
    /**
     * @if Eng
     * Indicates the single output channel mask.
     * @else
     * 单路输出通道掩码。
     * @endif
     */
    AUDIO_CHANNEL_MODE_MONO = 0x4u,
    /**
     * @if Eng
     * Indicates the dual output channel mask of
     * {@code AUDIO_CHANNEL_FRONT_LEFT | AUDIO_CHANNEL_RIGHT}.
     * @else
     * 双路输出通道掩码
     * {@code AUDIO_CHANNEL_FRONT_LEFT | AUDIO_CHANNEL_RIGHT}.
     * @endif
     */
    AUDIO_CHANNEL_MODE_STEREO = 0x8u,
    AUDIO_CHANNEL_MODE_INVALID = 0xFFFFFFFFu,
} AudioChannelMode;

/**
 * @if Eng
 * @brief Enumerates audio bit widths.
 *
 * @since 1.0
 * @version 1.0
 * @else
 * @brief枚举音频位宽。
 *
 * @自1.0起
 * @版本1.0
 * @endif
 */
typedef enum {
    BIT_WIDTH_8   = 8,
    BIT_WIDTH_16  = 16,
    BIT_WIDTH_24  = 24,
    BIT_WIDTH_32  = 32,
    BIT_WIDTH_32_FLOAT = 33,
    BIT_WIDTH_BUTT = 0xFFFFFFFFu,
} AudioBitWidth;

typedef struct {
    AudioStreamType streamType;
    uint32_t      samplerate;
    AudioCodecFormat format;
    uint32_t      channel;
    uint32_t      bitwidth;
} AudioOffloadInfo;

/**
 * @if Eng
 * @brief Audio policy definition.
 *
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 音频策略定义.
 *
 * @since 1.0
 * @version 1.0
 * @endif
 */
typedef enum {
    STRATEGY_NONE       = 0x0u, /*!< @if Eng NA traffic policy of the same level,
                                        or the traffic combination does not exist.
                                        @else NA 同级别流策略，或者不存在此种流组合 @endif */
    STRATEGY_EXCLUSIVE  = 0x1u, /*!< @if Eng Interrupt and exclusive. When this policy is returned,
                                            the INTERRUPT event needs to be called back to the user
                                            and the data of the previous flow stops being received.
                                        @else 打断、排他，当返回该策略时需要给使用者回调INTERRUPT 的事件并停止接收之前流的数据 @endif */
    STRATEGY_MIX        = 0x2u, /*!< @if Eng Audio mixing (The weight of the first stream is 100%.
                                        Each time the weight of the stream to be mixed is increased,
                                        the weight of the stream to be mixed is 50%.
                                        Each previous stream shares the remaining 50% in proportion.).
                                        @else 混音 （第一条流权重100%，每次增加需要混音的流权重 为50%，之前的每条流按比例共享剩下的50%） @endif */
    STRATEGY_DELAY      = 0x3u, /*!< @if Eng Delay. The audio focus is obtained after the current stream is delayed..
                                        @else 延时，当前流延时获取音频焦点 @endif */
    STRATEGY_REJECT     = 0x4u, /*!< @if Eng No playback. Currently, the audio focus cannot be obtained.
                                        @else 不播放，当前无法获取音频焦点 @endif */
    STRATEGY_INVALID = 0xFFFFFFFFu,
} AudioStrategy;

/**
 * @if Eng
 * Indicates that the interruption result.
 *
 * @since 0
 * @else
 * 表示中断结果。
 * @endif
 */
enum {
    /**
     * @if Eng
     * Indicates that failed to interrupt.
     *
     * @since 1
     * @else
     * 表示中断失败。
     *
     * @从1开始
     * @endif
     */
    INTERRUPT_FAILED = -1,

    /**
     * @if Eng
     * Indicates Interrupted successfully, include mix interrupt.
     *
     * @since 1
     * @else
     * 中断成功，包括混合中断。
     *
     * @自1
     * @endif
     */
    INTERRUPT_GRANTED = 0,

    /**
     * @if Eng
     * Indicates Delayed interrupt.
     *
     * @since 1
     * @else
     * 延迟中断。
     *
     * @从1开始
     * @endif
     */
    INTERRUPT_DELAYED,
};

/**
 * @if Eng
 * Indicates that the interruption types.
 *
 * @since 1
 * @else
 * 表示中断类型。
 *
 * @从1开始
 * @endif
 */
enum AudioInterruptType {
    /**
     * @if Eng
     * Indicates that the interruption starts.
     *
     * @since 1
     * @else
     * 表示中断开始。
     *
     * @从1开始
     * @endif
     */
    INTERRUPT_TYPE_BEGIN = 1,

    /**
     * @if Eng
     * Indicates that the interruption ends.
     *
     * @since 1
     * @else
     * 中断结束。
     *
     * @从1开始
     * @endif
     */
    INTERRUPT_TYPE_END
};

/**
 * @if Eng
 * Indicates that the interruption hint types.
 *
 * @since 1
 * @else
 * 中断提示类型。
 *
 * @从1开始
 * @endif
 */
enum AudioInterruptHintType {
    /**
     * @if Eng
     * Indicates a prompt for the application to pause audio playback.
     *
     * @since 1
     * @else
     * 提示应用程序暂停音频播放。
     *
     * @从1开始
     * @endif
     */
    INTERRUPT_HINT_PAUSE = 0,

    /**
     * @if Eng
     * Indicates a prompt for the application to resume audio playback.
     *
     * @since 1
     * @else
     * 提示应用程序恢复音频播放。
     *
     * @从1开始
     * @endif
     */
    INTERRUPT_HINT_RESUME,

    /**
     * @if Eng
     * Indicates a prompt for the application to stop audio playback.
     *
     * @since 1
     * @else
     * 提示应用程序停止音频播放。
     *
     * @从1开始
     * @endif
     */
    INTERRUPT_HINT_STOP
};

typedef enum {
    CAPTURE_MIC_AMIC_0 = 0x1,
    CAPTURE_MIC_AMIC_1 = 0x2,
    CAPTURE_MIC_AMIC_2 = 0x4,
    CAPTURE_MIC_AMIC_3 = 0x8,
    CAPTURE_MIC_DMIC_0 = 0x10,
    CAPTURE_MIC_DMIC_1 = 0x20,
    CAPTURE_MIC_DMIC_2 = 0x40,
    CAPTURE_MIC_DMIC_3 = 0x80,
    CAPTURE_MIC_TYPE_MAX,
    CAPTURE_MIC_TYPE_BUTT = 0x7FFFFFFF,
} CapturerMicType;

/**
 * @if Eng
 * Provides audio stream information.
 *
 * This class provides initialized parameters of {@code AudioCapturer} and {@code AudioRenderer},
 * including the stream encoding format, sampling rate, audio channel, usage type, session ID,
 * buffer size, clearing flag, audio source, and stream flags.
 *
 * @since 1
 * @else
 * 提供音频流信息。
 *
 * 该类提供{@code AudioCapturer}和{@code AudioRenderer}的初始化参数。
 * 包括码流编码格式、采样率、音频通道、使用类型、会话ID、
 * 缓冲区大小、清除标志、音频源和码流标志。
 *
 * @从1开始
 * @endif
 */
typedef struct {
    AudioStreamType streamType;
    AudioSession sessionID;
    AudioCodecFormat format;
    uint32_t sampleRate;
    uint32_t bitWidth;
    int32_t bitRate;
    uint32_t channelCount;
    uint32_t deviceId;
    uint32_t connectedId;
    AudioChannelMode channelMask;
    AudioStreamFlag audioStreamFlag;
    AudioLinkDirection linkDir;
    uint16_t sampleFmt;
    bool disableSEA;
    uint32_t micType;
    bool isAlwaysWake;
    bool isSync;
    uint32_t frameLength;
} AudioStreamInfo;

/**
 * @if Eng
 * Provides audio stream out config.
 *
 * <p>This class provides initialized parameters of {@code AudioCapturer} and {@code AudioRenderer},
 * including the stream encoding format, sampling rate, audio channel, usage type, session ID,
 * buffer size, clearing flag, audio source, and stream flags.
 *
 * @since 1
 * @else
 * 提供音频流输出配置。
 *
 * 该类提供{@code AudioCapturer}和{@code AudioRenderer}的初始化参数。
 * 包括码流编码格式、采样率、音频通道、使用类型、会话ID、
 * 缓冲区大小、清除标志、音频源和码流标志。
 *
 * @从1开始
 * @endif
 */
typedef struct {
    AudioCodecFormat audioFormat;
    AudioStreamType streamType;
    uint32_t sampleRate;
    uint32_t sessionID;
    uint32_t channelCount;
    uint32_t deviceId;
    uint32_t connectedId;
    AudioLinkDirection linkDir;
    uint16_t sampleFmt;
    AudioBitWidth bitWidth;
    bool isSync;
    uint32_t frameLength;
} AudioRendererConfig;

typedef struct {
    AudioCodecFormat audioFormat;
    AudioStreamType streamType;
    uint32_t sampleRate;
    uint32_t sessionID;
    uint32_t channelCount;
    AudioLinkDirection linkDir;
    bool disableSEA; /*!< @if Eng disable Speech Enhancement & AI.
                            @else 关闭语音增强算法。 @endif */
    uint32_t micType;
    AudioBitWidth bitWidth;
    bool isAlwaysWake;
    uint32_t frameLength;
} CapturerInputConfig;

typedef enum {
    AUDIO_EVENT_NONBLOCK_WRITE_COMPLETED, /*!< @if Eng The non-block write is complete.
                                            @else 非阻塞写操作完成。 @endif */
    AUDIO_EVENT_DRAIN_COMPLETED,          /*!< @if Eng The draining is complete.
                                            @else 排水完毕。 @endif */
    AUDIO_EVENT_FLUSH_COMPLETED,           /*!< @if Eng The flush is complete.
                                            @else 刷新完成。 @endif *//**< The flush is complete. */
    AUDIO_EVENT_RENDER_FULL,               /*!< @if Eng The render buffer is full.
                                            @else 渲染缓冲区已满。 @endif */
    AUDIO_EVENT_RENDER_EOS,                /*!< @if Eng Render to the end of the stream.
                                            @else 渲染到流的末尾。 @endif */
    AUDIO_EVENT_BUFFER_EMPTY,              /*!< @if Eng The underlying buffer data is empty.
                                            @else 基础缓冲区数据为空。 @endif */
    AUDIO_EVENT_OUTPUT_DEVICE_CHANGED,     /*!< @if Eng The output device has been changed.
                                            @else 输出设备已更改。 @endif */
    AUDIO_EVENT_VOICE_BEGIN,               /*!< @if Eng Command word recognition starts.
                                            @else 命令词识别开始。 @endif */
    AUDIO_EVENT_VOICE_END,                 /*!< @if Eng Command word recognition ended.
                                            @else 命令词识别结束。 @endif */
    AUDIO_EVENT_VOICE_COMMAND,             /*!< @if Eng Identified command words.
                                            @else 已经识别的命令词。 @endif */
    AUDIO_EVENT_TTS_START,                 /*!< @if Eng TTS start.
                                            @else 文本转语音播放开始。 @endif */
    AUDIO_EVENT_TTS_COMPLETED,             /*!< @if Eng The TTS playback is complete.
                                            @else 文本转语音播放完成。 @endif */
    AUDIO_EVENT_TTS_END,                   /*!< @if Eng The TTS playback ends.
                                            @else 文本转语音播放停止。 @endif */
    AUDIO_EVENT_CHANGE_VOLUME,              /*!< @if Eng The volume of the current stream has been changed.
                                            @else 当前流音量已经改变。 @endif */
    AUDIO_EVENT_ERROR_OCCUR = 0xFFFFFFFFu,  /*!< @if Eng An error occurs.
                                            @else 发生错误。 @endif */
} AudioCallbackEventType;

typedef int32_t (*RendererCallback)(const void *caller, AudioCallbackEventType event, const void *info);

typedef int32_t (*CapturerCallback)(const void *caller, AudioCallbackEventType event, const void *info);

typedef struct {
    uint32_t frameCount;
    uint32_t size;
    uint8_t *data;
} CapturerBuffer;

typedef enum {
    AUDIO_INVOKE_HAID_CREATE,
    AUDIO_INVOKE_HAID_DESTROY,
    AUDIO_INVOKE_HAID_SET_ENABLE,
    AUDIO_INVOKE_HAID_GET_ENABLE,
    AUDIO_INVOKE_HAID_SET_EVENT_CALL_BACK,
    AUDIO_INVOKE_HAID_SET_VOLUME,
    AUDIO_INVOKE_HAID_GET_VOLUME,
    AUDIO_INVOKE_HAID_SET_HARDWARE_CONFIG,
    AUDIO_INVOKE_HAID_GET_HARDWARE_CONFIG,
    AUDIO_INVOKE_HAID_START_CALIBRATION,
    AUDIO_INVOKE_HAID_STOP_CALIBRATION,
    AUDIO_INVOKE_HAID_SET_TEST_CONFIG,
    AUDIO_INVOKE_HAID_SET_IOCTL,
    AUDIO_INVOKE_HAID_SET_CONFIG,
    AUDIO_INVOKE_HAID_GET_CONFIG,
    AUDIO_INVOKE_HAID_SET_ALL_CONFIG,
    AUDIO_INVOKE_HAID_GET_ALL_CONFIG,
    AUDIO_INVOKE_HAID_TONE_PLAY,
    AUDIO_INVOKE_HAID_TONE_STOP,
    AUDIO_INVOKE_HAID_SET_SCENE,
    AUDIO_INVOKE_HAID_GET_SCENE,
    AUDIO_INVOKE_HAID_SET_SPK_CALI_COEF,
    AUDIO_INVOKE_HAID_SET_MIC_VOLUME,
    AUDIO_INVOKE_HAID_GET_ENV_NOISE,
    AUDIO_INVOKE_TWS_SET_MODE,
    AUDIO_INVOKE_TWS_SET_ROLE,
    AUDIO_INVOKE_TWS_SET_CHANNEL,
    AUDIO_INVOKE_TWS_SET_PRODUCT_FORM,
    AUDIO_INVOKE_ANC_INIT,
    AUDIO_INVOKE_ANC_DEINIT,
    AUDIO_INVOKE_ANC_OPEN,
    AUDIO_INVOKE_ANC_GET_ATTR,
    AUDIO_INVOKE_ANC_CLOSE,
    AUDIO_INVOKE_ANC_SET_PARAM,
    AUDIO_INVOKE_ANC_GET_PARAM,
    AUDIO_INVOKE_LIVEMIC_SET_MIXMODE,
    AUDIO_INVOKE_LIVEMIC_GET_MIXMODE,
    AUDIO_INVOKE_LIVEMIC_SET_MICMODE,
    AUDIO_INVOKE_LIVEMIC_GET_MICMODE,
    AUDIO_INVOKE_RECORD_DB_VOLUME,
    AUDIO_INVOKE_READ_DB_VOLUME,
    AUDIO_INVOKE_SET_A2DP_CODEC_ATTR,
    AUDIO_INVOKE_SET_CONFIG_PARAM,
    AUDIO_INVOKE_SET_GLOBAL_MUTE,
    AUDIO_INVOKE_SET_VOLUME_MATRIX,
    AUDIO_INVOKE_GET_VOLUME_MATRIX,
    AUDIO_INVOKE_SET_ENCODE_PARAM_EXT,
    AUDIO_INVOKE_SEA_SET_LIB_ID,
} AudioInvokeId;

typedef struct {
    uint16_t sampleRate;  /*!< @if Eng Sample frequency, @See bs_br_media_aac_sf_t.
                                    @else 采样频率，@参考 bs_br_media_aac_sf_t @endif */
    uint32_t bitRate;          /*!< @if Eng bit rate.
                                    @else 比特率。 @endif */
    uint8_t codecType;
    uint8_t payload;
    uint8_t channel;           /*!< @if Eng channels, @See bs_br_media_aac_channel_t.
                                    @else 通道，@参考 bs_br_media_aac_channel_t @endif */
    uint8_t channelMode;           /*!< @if Eng channelMode, @See bs_br_media_aac_channel_t.
                                    @else 通道模式，@参考 bs_br_media_aac_channel_t @endif */
    uint8_t blockLength;         /*!< @if Eng Block length, @See bs_br_media_sbc_block_t.
                                     @else   块长度, @参考 bs_br_media_sbc_block_t @endif */
    uint8_t subband;              /*!< @if Eng Num of subband, @See bs_br_media_sbc_subband_t.
                                     @else   子带个数, @参考 bs_br_media_sbc_subband_t @endif */
    uint8_t allocMethod;         /*!< @if Eng Allocation Method,@See bs_br_media_sbc_allocation_t.
                                     @else Allocation Method, @参考 bs_br_media_sbc_allocation_t @endif */
    uint8_t minBitpool;          /*!< @if Eng Min bitpool value 2~250.
                                     @else   最小位池，取值 2~250。 @endif */
    uint8_t maxBitpool;          /*!< @if Eng Max bitpool value 2~250.
                                     @else   最大位池，值 2~250。 @endif */
    uint8_t objectType;        /*!< @if Eng object type, @See bs_br_media_aac_object_type_t.
                                    @else 对象类型，@参考 bs_br_media_aac_object_type_t @endif */
    uint8_t rfa;                /*!< @if Eng Reserved for future use.
                                    @else 预留 @endif */
    uint8_t vbr;                /*!< @if Eng Variable Bit Rate supported, @See bs_br_media_aac_vbr_t.
                                    @else 支持动态比特率, @参考 bs_br_media_aac_vbr_t @endif */
} AudioA2dpCodecAttr;

typedef struct {
    bool toggleValid;  /*!< @if Eng the config will take effect immediately;
                                    true, the config will take effect in the toggle_count interrupt.
                                @else 配置将立即生效；如果为true，则配置将在switch_count中断中生效。 @endif */
    uint32_t toggleCount; /*!< @if Eng when toggle_valid is true,
                                       the config will take effect in the toggle_count interrupt.
                                      @else 当switch_valid为true时，则配置将在switch_count中断中生效。 @endif */
} ToggleConfig;

typedef enum {
    TOGGLE_COUNT_TYPE_VOLUME,
    TOGGLE_COUNT_TYPE_NOTIFY,
    TOGGLE_COUNT_TYPE_INVALID = 0xFFFFFFFF,
} ToggleConfigType;

typedef enum {
    AUDIO_SEA_LIB_NULL = 0x00, /*!< @if Eng Invalid Engine ID
                                   @else   无效的引擎库 @endif */
    AUDIO_SEA_LIB_SEE  = 0x01, /*!< @if Eng Speech Enhancement Engine @UAPI_SEA_LIB_SEE
                                   @else   语音增强引擎库 @endif */
    AUDIO_SEA_LIB_AHE  = 0x03, /*!< @if Eng Augment Hearing Engine @UAPI_SEA_LIB_AHE
                                   @else   人声增强引擎库 @endif */
    AUDIO_SEA_LIB_MAX = 0xffffffffu
} AudioSeaLibId;

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

    /*!< @if Eng Read and Write AFE VQE param command.
         @else   获取和设置语音通话算法参数命令 @endif */
    AUDIO_SEA_AFE_VEQ_RW_PARAM_CMD = AUDIO_SEA_CMD(AUDIO_SEA_AFE_VQE, 0),

    /*!< @if Eng Read and Write AFE AHE param command.
         @else   获取和设置人声增强算法参数命令 @endif */
    AUDIO_SEA_AFE_AHE_RW_PARAM_CMD = AUDIO_SEA_CMD(AUDIO_SEA_AFE_AHE, 0),
    AUDIO_SEA_AFE_CMD_MAX,
    AUDIO_SEA_AFE_CMD_BUTT = 0x7FFFFFFFu,
} AudioSeaAfeCmdId;

#endif  // AUDIO_BASE_TYPE_H

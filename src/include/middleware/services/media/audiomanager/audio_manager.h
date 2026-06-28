/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
* Description: audio manager define
* Author: Media Software Group
* Create: 2021-04-21
*/

/**
 * @if Eng
 * @file audio_manager.h
 *
 * @brief Declares the <b>AudioManager</b> class for Manages audio information,
 * controls audio devices, adjusts the volume, and manages the audio focus.
 *
 *
 * @从1.0开始
 * @version 1.0
 * @else
 * @文件 audio_manager.h
 *
 * @brief 声明用于管理音频信息的<b>AudioManager</b>类。
 * 控制音频设备，调节音量，管理音频焦点.
 *
 *
 * @从1.0开始
 * @版本 1.0
 * @endif
 */

#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <stddef.h>
#include <stdint.h>

#include "audio_base_type.h"
#include "audio_debug_info.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @if Eng
 * Called back to notify an application when its audio is interrupted by another application.
 *
 * @param type Indicates the interruption type, which can be {@link AudioInterruptType#INTERRUPT_TYPE_BEGIN}
 * or {@link AudioInterruptType#INTERRUPT_TYPE_END}.
 * @param hint Indicates the interruption prompt, which can be {@link AudioInterruptHintType#INTERRUPT_HINT_PAUSE},
 * {@link AudioInterruptHintType#INTERRUPT_HINT_RESUME}, or {@link AudioInterruptHintType#INTERRUPT_HINT_STOP}.
 * @since 1.0
 * @else
 * 当应用程序的音频被其他应用程序中断时，回调以通知应用程序。
 *
 * @param type中断类型{@link AudioInterruptType#INTERRUPT_TYPE_BEGIN}
 * 或者 {@link AudioInterruptType#INTERRUPT_TYPE_END}.
 * @param hint中断提示信息{@link AudioInterruptHintType#INTERRUPT_HINT_PAUSE}。
 * {@link AudioInterruptHintType#INTERRUPT_HINT_RESUME}或{@link AudioInterruptHintType#INTERRUPT_HINT_STOP}。
 * @从1.0开始
 * @endif
 */
typedef struct {
    void (*OnInterrupt)(int32_t type, int32_t hint, void *cookie);
    void *cookie;
    uint32_t cookieSize;
} InterruptListener;

/**
 * @if Eng
 * @brief An audio interruption management mechanism
 * used to resolve audio playback conflicts among multiple applications.
 *
 * @since 1.0
 * @version 1.0
 * @else
 * @brief一种音频中断管理机制
 * 用于解决多个应用之间的音频播放冲突。
 * @从1.0开始
 * @版本 1.0
 * @endif
 */
typedef struct {
    AudioStreamType streamType;
    AudioSession sessionID;
    InterruptListener *interruptListener;
} AudioInterrupt;

/**
 * @brief audio manager debug info.
 *
 * @since 1.0
 * @version 1.0
 */
typedef struct {
    InterruptDebugInfo *inputInterruptInfo; /*!< @if Eng All input focus type
                                            @else   所有输入焦点类型 @endif */
    uint32_t interruptInfoLength;
    InterruptDebugInfo *outputInterruptInfo; /*!< @if Eng All output focus types
                                            @else   所有输出焦点类型 @endif */
    uint32_t outputInterruptInfoLength;
    StreamDebugInfo *allstreamInfo; /*!< @if Information about all Streams
                                            @else   所有的流信息 @endif */
    uint32_t streamInfoLength;
    DeviceDebugInfo *allDeviceInfo; /*!< @if Eng Information about all devices
                                            @else   所有的设备信息 @endif */
    uint32_t deviceInfoLength;
} AudioManagerDebugInfo;

/**
 * @if Eng
 * @brief Audio management initialization.
 *
 * @return If the audio management is successfully initialized,
 * ERRCODE_SUCC is returned. Otherwise, other values are returned.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief音频管理初始化。
 *
 * 如果音频管理初始化成功，则返回ERRCODE_SUCC，否则返回其他值
 * @从1.0开始
 * @版本 1.0
 * @endif
 */
errcode_t AudioManagerInitialize(void);

/**
 * @if Eng
 * @brief Creates a session ID.
 *
 * @return Returns the session ID if created; returns {@code 0} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 创建一个会话ID。
 *
 * @return 如果创建则返回会话ID，否则返回{@code 0}。
 * @从1.0开始
 * @版本 1.0
 * @endif
 */
AudioSession AudioManagerMakeSessionId(void);

/**
 * @if Eng
 * @brief Sets the volume level of a specified type of streams.
 *
 * @param [in] streamType Indicates the type of streams for which the volume level is to set.
 * @param [in] volume     Indicates the volume level to set. You can obtain the maximum level value by calling
 *                   {@link #AudioManagerGetMaxVolume(AudioStreamType)}, and the minimum by
 *                   {@link #AudioManagerGetMinVolume(AudioStreamType)}.
 * @return Returns {@code true} if the setting is successful; returns {@code false} otherwise.
 * @see #AudioManagerGetMaxVolume(AudioStreamType)
 * @see #AudioManagerGetMinVolume(AudioStreamType)
 * @see #AudioManagerGetVolume(AudioStreamType)
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置指定类型码流的音量大小。
 *
 * @param [in] streamType 设置音量大小的码流类型。
 * @param [in] volume 指定要设置的音量级别。可以通过调用获取最大电平值
 *               {@link #AudioManagerGetMaxVolume(AudioStreamType)}，其中最小值为
 *               {@link #AudioManagerGetMinVolume(AudioStreamType)}.
 * @return 如果设置成功返回{@code true}，否则返回{@code false}。
 * @see #AudioManagerGetMaxVolume（音频流类型）
 * @see #AudioManagerGetMinVolume（音频流类型）
 * @see #AudioManagerGetVolume（音频流类型）
 * @自1.0起
 * @版本1.0
 * @endif
 */
bool AudioManagerSetVolume(AudioStreamType streamType, int32_t volume);

/**
 * @if Eng
 * @brief Obtains the volume level of a specified type of audio streams.
 *
 * @param [in] streamType Indicates the stream type.
 * @return Returns the volume level of the type of streams.
 * @see #AudioManagerGetMaxVolume(AudioStreamType)
 * @see #AudioManagerGetMinVolume(AudioStreamType)
 * @see #AudioManagerSetVolume(AudioStreamType, int32_t)
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取指定类型音频流的音量大小。
 *
 * @param [in] streamType 流类型。
 * @return 返回流类型的音量级别。
 * @see #AudioManagerGetMaxVolume（获取最大音量）
 * @see #AudioManagerGetMinVolume（获取最小音量）
 * @see #AudioManagerSetVolume(AudioStreamType, int32_t)
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioManagerGetVolume(AudioStreamType streamType);

/**
 * @if Eng
 * @brief Obtains the minimum volume level of a specified type of audio streams.
 *
 * @param [in] streamType Indicates the stream type.
 * @return Returns the minimum valid volume level for the type of streams.
 * @see #AudioManagerGetVolume(AudioStreamType)
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取指定类型音频流的最小音量。
 *
 * @param [in] streamType流类型。
 * @return 返回该类型流的最小有效音量。
 * @see # AudioManagerGetVolume（音频流类型）
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioManagerGetMinVolume(AudioStreamType streamType);

/**
 * @if Eng
 * @brief Obtains the maximum volume level of a specified type of audio streams.
 *
 * @param [in] streamType Indicates the stream type.
 * @return Returns the maximum valid volume level for the type of streams.
 * @see #AudioManagerGetVolume(AudioStreamType)
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取指定类型音频流的最大音量。
 *
 * @param [in] streamType 流类型。
 * @return 返回该类型流的最大有效音量。
 * @see #AudioManagerGetVolume（音频流类型）
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioManagerGetMaxVolume(AudioStreamType streamType);

/**
 * @if Eng
 * @brief Mutes a specified type of streams.
 *
 * @param [in] streamType Indicates the type of streams to mute.
 * @return Returns {@code true} if the streams are muted; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 静音指定类型的流。
 *
 * @param [in] streamType 需要静音的码流类型。
 * @return 如果流被静音，则返回{@code true}，否则返回{@code false}。
 * @自1.0起
 * @版本1.0
 * @endif
 */
bool AudioManagerMute(AudioStreamType streamType);

/**
 * @if Eng
 * @brief Unmutes a specified type of streams.
 *
 * @param [in] streamType Indicates the type of streams to unmute.
 * @return Returns {@code true} if the streams are unmuted; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 取消静音指定类型的码流。
 *
 * @param [in] streamType 需要取消静音的码流类型。
 * @return 如果流被取消静音，则返回{@code true}，否则返回{@code false}。
 * @自1.0起
 * @版本1.0
 * @endif
 */
bool AudioManagerUnMute(AudioStreamType streamType);

/**
 * @if Eng
 * @brief Checks whether a specified type of streams are muted.
 *
 * @param [in] streamType Indicates the stream type.
 * @return Returns {@code true} if the streams are muted; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 检查指定类型的码流是否静音。
 *
 * @param [in] streamType 流类型。
 * @return 如果流被静音，则返回{@code true}，否则返回{@code false}。
 * @自1.0起
 * @版本1.0
 * @endif
 */
bool AudioManagerIsMute(AudioStreamType streamType);

/**
 * @if Eng
 * @brief Mutes or unmutes global audio streams.
 *
 * @param [in] isMute Indicates whether to mute global audio streams. Value {@code true} means global audio streams
 *               will be muted, and {@code false} means the opposite.
 * @return Returns {@code true} if the setting is successful; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 静音/取消静音全局音频流。
 *
 * @param [in] isMute 是否静音全局音频流。{@code true}表示全局音频流
 * 将被静音，而{@code false}则表示相反。
 * @return 如果设置成功返回{@code true}，否则返回{@code false}。
 * @自1.0起
 * @版本1.0
 * @endif
 */
bool AudioManagerSetMasterMute(bool isMute);

/**
 * @if Eng
 * Checks whether audio streams are muted globally.
 *
 * @return Returns {@code true} if all audio streams are muted; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 检测音频流是否全局静音。
 *
 * @return 如果所有音频流都被静音，则返回{@code true}；否则返回{@code false}。
 * @自1.0起
 * @版本1.0
 * @endif
 */
bool AudioManagerIsMasterMute(void);

/**
 * @if Eng
 * @brief Mutes or unmutes a microphone.
 *
 * @param [in] isMute Indicates whether to mute a microphone. Value {@code true} means that the microphone
 *               will be muted, and {@code false} means the opposite.
 * @return Returns {@code true} if the setting is successful; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置麦克风是否静音。
 *
 * @param [in] isMute 是否静音麦克风。{@code true}表示麦克风
 * 将被静音，而{@code false}则表示相反。
 * @return 如果所有音频流都被静音，则返回{@code true}；否则返回{@code false}。
 * @自1.0起
 * @版本1.0
 * @endif
 */
bool AudioManagerSetMicrophoneMute(bool isMute);

/**
 * @if Eng
 * @brief Checks whether a microphone is muted.
 *
 * @return Returns {@code true} if the microphone is muted; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 检查麦克风是否静音。
 *
 * @return 如果麦克风被静音，则返回{@code true}，否则返回{@code false}。
 * @自1.0起
 * @版本1.0
 * @endif
 */
bool AudioManagerIsMicrophoneMute(void);

/**
 * @if Eng
 * @brief Sets the ringer mode.
 *
 * <p>In silent mode, a device is silent and does not vibrate. In vibration mode, a device is silent and vibrates.
 * In sound mode, a device is audible and may vibrate (depending on user settings).
 *
 * @param [in] mode Indicates the ringer mode, which can be {@link AudioRingMode#RINGER_MODE_NORMAL},
 *             {@link AudioRingMode#RINGER_MODE_SILENT}, or {@link AudioRingMode#RINGER_MODE_VIBRATE}.
 * @return Returns {@code true} if the setting is successful; returns {@code false} otherwise.
 * @see #AudioManagerGetRingerMode()
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置振铃模式。
 *
 * <p>静默模式下，设备处于静默状态，不会产生振动。在振动模式下，设备处于静音状态，设备会振动。
 * 在声音模式下，设备可以听到声音，并且可能会振动（取决于用户设置）。
 *
 * @param [in] mode 振铃模式{@link AudioRingMode#RingER_MODE_NORMAL}，
 * {@link AudioRingMode#RingER_MODE_SILENT}或{@link AudioRingMode#Ringer_MODE_VIBRATE}。
 * @return 如果设置成功返回{@code true}，否则返回{@code false}。
 * @see #AudioManager获取振铃模式()
 * @自1.0起
 * @版本1.0
 * @endif
 */
bool AudioManagerSetRingerMode(AudioRingMode mode);

/**
 * @if Eng
 * @brief Obtains the ringer mode.
 *
 * @return Returns the ringer mode, which can be {@link AudioRingMode#RINGER_MODE_NORMAL},
 * {@link AudioRingMode#RINGER_MODE_SILENT}, or {@link AudioRingMode#RINGER_MODE_VIBRATE}.
 * @see #AudioManagerSetRingerMode(AudioRingMode)
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取振铃模式。
 *
 * @return 返回振铃模式{@link AudioRingMode#RingER_MODE_NORMAL}，
 * {@link AudioRingMode#RingER_MODE_SILENT}或{@link AudioRingMode#Ringer_MODE_VIBRATE}。
 * @see #AudioManagerSetRingerMode（音频振铃模式）
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioManagerGetRingerMode(void);

/**
 * @if Eng
 * @brief Obtains information about audio input or output devices, or that about all.
 *
 * @param [in] flag Indicates input or output devices, or all audio devices.
 * @param [out] deviceInfo Indicates Audio device information.
 * @param [out] deviceCount Indicates Number of Audio Devices.
 * @return Returns an array with the same information structure as an {@code AudioDeviceInfo} instance if
 * the information is obtained; returns an array whose length is 0 otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取音频输入输出设备的信息，或所有的信息。
 *
 * @param [in] flag 表示输入或输出设备，或所有音频设备。
 * @param [out] deviceInfo 音频设备信息。
 * @param [out] deviceCount 音频设备个数。
 * @return 返回一个与{@code AudioDeviceInfo}实例具有相同信息结构的数组，如果
 * 获取信息，否则返回长度为0的数组。
 * @自1.0起
 * @版本1.0
 * @endif
 */

errcode_t AudioManagerGetDevices(DeviceFlag flag, AudioDeviceInfo *deviceInfo, uint32_t *deviceCount);

/**
 * @if Eng
 * @brief Query whether offload is supported based on audio offload information.
 *
 * @param [out] info Indicates audio offload information {@link AudioOffloadInfo}.
 * @return Returns {@code true} if the offload mode is support; returns {@code false} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 根据音频卸载信息查询是否支持卸载。
 *
 * @param [out] info 音频卸载信息{@link AudioOffloadInfo}。
 * @return 如果支持卸载模式，则返回{@code true}，否则返回{@code false}。
 * @自1.0起
 * @版本1.0
 * @endif
 */

bool AudioManagerIsSupportOffload(const AudioOffloadInfo *info);
/**
 * @if Eng
 * @brief Sets the device connection state.
 *
 * @param [in] device Indicates audio device {@link AudioDeviceType}.
 * @param [in] state Indicates audio device connection state {@link AudioDeviceConnectState}.
 * @return Returns {@code ERRCODE_SUCC} if set the device connection status successfully; returns an error code defined
 * in {@link audio_errors.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置设备连接状态。
 *
 * @param [in] device 音频设备{@link AudioDeviceType}。
 * @param [in] state 音频设备连接状态{@link AudioDeviceConnectState}。
 * @return 如果成功设置设备连接状态，则返回{{@code ERRCODE_SUCC}；否则返回{@link audio_errors.h}；
 * 里面定义的错误代码。
 * @自1.0起
 * @版本1.0
 * @endif
 */

errcode_t AudioManagerSetDeviceConnectionState(AudioDeviceType device, AudioDeviceConnectState state);
/**
 * @if Eng
 * @brief Activates audio interruption.
 *
 * <p>Before playing audio, an application sends an interruption request to other applications for
 * pausing audio playback or decreasing the volume, preventing audio conflicts.
 *
 * @param [in] interrupt Indicates the interruption data structure, including interruption information and the
 *                  interruption listener.
 * @return Returns {@code INTERRUPT_GRANTED} if audio interruption is activated; returns {@code INTERRUPT_DELAYED}
 * if should be delayed; returns {INTERRUPT_FAILED}
 * if be rejected, and other error code defined in {@link audio_errors.h}.
 * @see #AudioManagerDeactivateAudioInterrupt(AudioInterrupt)
 * @see AudioInterrupt
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 激活音频中断。
 *
 * <p>在播放音频之前，应用程序向其他应用程序发送中断请求，用于
 * 暂停音频播放或降低音量，防止音频冲突。
 *
 * @param [in] interrupt 中断数据结构，包括中断信息和
 * 中断监听器。
 * @return 如果音频中断被激活，则返回{@code INTERRUPT_GRIDED}；返回{@code INTERRUPT_DELAYED}
 * 如果应该延迟；返回{INTERRUPT_FAILED}
 * 如果被拒绝，其他错误码在{@link audio_errors.h}中定义。
 * @see #AudioManagerDeactivateAudioInterrupt（音频中断）
 * @see AudioInterrupt
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioManagerActivateAudioInterrupt(AudioInterrupt interrupt);

/**
 * @if Eng
 * @brief Deactivates audio interruption.
 *
 * <p>If an application does not need to receive interruption messages, call this method to
 * deactivate audio interruption for the application.
 *
 * @param [in] interrupt Indicates the interruption data structure, including interruption information and the
 *                  interruption listener.
 * @return Returns {@code ERRCODE_SUCC} if audio interruption is deactivated; returns an error code defined
 * in {@link audio_errors.h} otherwise.
 * @see #AudioManagerActivateAudioInterrupt(AudioInterrupt)
 * @see AudioInterrupt
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 去激活音频中断。
 *
 * <p>如果应用程序不需要接收中断消息，则调用此方法
 * 禁用应用程序的音频中断。
 *
 * @param [in] interrupt 中断数据结构，包括中断信息和
 * 中断监听器。
 * @return 如果音频中断被禁用，则返回{@code ERRCODE_SUCC}；否则返回{@link audio_errors.h}中定义的错误代码
 *
 * @see #AudioManagerActivateAudioInterrupt（音频中断）
 * @see AudioInterrupt
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioManagerDeactivateAudioInterrupt(AudioInterrupt interrupt);

/**
 * @if Eng
 * @brief Audio management obtains DumpInfo.
 *
 * @param [out] audioManagerInfo DumpInfo of the audio management module.
 * @return Returns {@code ERRCODE_SUCC} if The DumpInfo of the audio management module is obtained successfully;
 * returns an error code defined
 * in {@link audio_errors.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 音频管理获取DumpInfo。
 *
 * @param [out] audioManagerInfo 音频管理模块的DumpInfo。
 * @return 如果成功获取音频管理模块的DumpInfo，则返回{@code ERRCODE_SUCC}；否则返回
 * {@link audio_errors.h}里面定义的错误代码
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t AudioManagerDumpInfo(AudioManagerDebugInfo *audioManagerInfo);

/**
 * @if Eng
 * @brief When mixing multiple streams, set the volume ratio of the current stream.
 * If the current stream volume is 100, and the current stream volume ratio is set to 50%,
 * then when a new stream comes to mix with the current stream, the current stream volume will become 50.
 * If set to 100, it means the current stream volume will not change due to the new stream.
 * If set to 0, it means handling according to the default strategy: during mixing,
 * the current stream volume will become half of the new stream volume.
 * @param [in] streamType Indicates the stream type.
 * @param [in] mixWeight ratio of the current stream volume to the original volume during mixing, range [0,100].
 * @return Returns {@code ERRCODE_SUCC} if the setting is succeeded; returns an error code defined
 * in {@link audio_errors.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 当多个流混音的时候，设置当前流的音量比例。
 * 如果当前流音量有100，设置当前流音量比例为50%，则当新的流来跟当前流混音时，当前流的音量就变成50。
 * 如果设置100就表示当前流的音量不会因为新来的流而改变。
 * 如果设置0就表示按照默认策略处理：混音时，当前流的音量会变成新来的流音量的一半。
 * @param [in] streamType 流类型。
 * @param [in] mixWeight 混音的时候，当前流音量的占原始音量的比例, 范围 [0,100]。
 * @return 如果设置成功，则返回{@code ERRCODE_SUCC}；返回{@link audio_errors.h}中定义的错误代码
 *
 * @自1.0起
 * @版本1.0
 */
errcode_t AudioManagerSetVolumeMixWeight(AudioStreamType streamType, int32_t mixWeight);

/**
 * @if Eng
 * @brief Obtains the volume ratio of the current stream when multiple streams are mixed.
 * @param [in] streamType Indicates the stream type.
 * @return Returns If the operation is successful, the actual volume ratio is returned; otherwise, 0xFFFFFFFF is returned.
 *
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 当多个流混音的时候，获取当前流的音量比例。
 *
 * @param [in] streamType 流类型。
 * @return 如果获取成功，则返回真正的混音音量比例，否则返回0xFFFFFFFF。
 *
 * @自1.0起
 * @版本1.0
 */
errcode_t AudioManagerGetVolumeMixWeight(AudioStreamType streamType);

/**
 * @if Eng
 * @brief Audio management deinitialization.
 *
 * @return Returns {@code ERRCODE_SUCC} if Audio management deinitialization succeeded; returns an error code defined
 * in {@link audio_errors.h} otherwise.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 音频管理去初始化。
 *
 * @return 如果音频管理去初始化成功，则返回{@code ERRCODE_SUCC}；返回{@link audio_errors.h}中定义的错误代码
 *
 * @自1.0起
 * @版本1.0
 */
errcode_t AudioManagerDeinitialize(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

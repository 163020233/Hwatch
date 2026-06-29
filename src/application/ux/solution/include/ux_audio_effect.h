/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provide ux audio setting\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-01， Create file. \n
 */
#ifndef UX_AUDIO_EFFECT_H
#define UX_AUDIO_EFFECT_H

#include "errcode.h"
#include "ux_api_system_config.h"
#include "ux_api_audio_common.h"
#include "ux_api_audio_effect.h"
#include "feature_config.h"
#ifdef CONFIG_SUPPORT_EQ
#include "eq_param.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ANC功能支持的最大麦克风数量 */
#define ANC_MIC_NUM_MAX 3
/* ANC通道索引0，通常对应前馈麦克风0 */
#define ANC_INDEX0 0
/* ANC通道索引1，通常对应前馈麦克风1 */
#define ANC_INDEX1 1
/* ANC通道索引2，通常对应反馈麦克风 */
#define ANC_INDEX2 2
/* 音频处理命令数量 */
#define CMD_NUM 4
/* EQ参数最大数量，根据配置或默认80设置 */
#ifdef AUDIO_EQ_PARAM_COUNT
#define EQ_PARAM_MAX_NUM AUDIO_EQ_PARAM_COUNT
#else
#define EQ_PARAM_MAX_NUM 80
#endif
/* 通用EQ参数数据大小，单位字节 */
#define AUDIO_COM_EQ_PARAM_SIZE 120
/* ANC输出通路最大数量 */
#define ANC_OUT_NUM_MAX 2

/**
 * @brief ANC通道类型枚举
 * @details 定义ANC（主动降噪）模块的各信号通路，包括：
 *         - FF（前馈）通路：采集环境声音并反向播放实现降噪
 *         - FB（反馈）通路：采集扬声器输出声音进行闭环控制
 *         - SPE（speaker）：扬声器通路
 *         - POST_PROC0/1：后处理通路
 *         - WFIR：加权有限冲击响应滤波器通路
 */
typedef enum {
    ANC_CHANNEL_FF0 = 0, /*!< @if Eng ff0 channel in ANC
                                     @else   ANC模块的ff0通路（前馈麦克风0） @endif */

    ANC_CHANNEL_FF1 = 1, /*!< @if Eng ff1 channel in ANC
                                     @else   ANC模块的ff1通路（前馈麦克风1） @endif */

    ANC_CHANNEL_FB = 2, /*!< @if Eng fb channel in ANC
                                     @else   ANC模块的fb通路（反馈麦克风） @endif */

    ANC_CHANNEL_SPE = 3, /*!< @if Eng spe channel in ANC
                                     @else   ANC模块的spe通路（扬声器） @endif */

    ANC_CHANNEL_POST_PROC0 = 4, /*!< @if Eng post-proc0 channel in ANC
                                     @else   ANC模块的post-proc0通路（后处理0） @endif */

    ANC_CHANNEL_POST_PROC1 = 5, /*!< @if Eng post-proc1 channel in ANC
                                     @else   ANC模块的post-proc1通路（后处理1） @endif */

    ANC_CHANNEL_WFIR = 6,       /*!< @if Eng wfir channel in ANC
                                     @else   ANC模块的wfir通路（加权滤波器） @endif */

    ANC_CHANNEL_TYPE_MAX,                /*!< ANC通道类型最大值，用于边界检查 */
    ANC_CHANNEL_TYPE_BUTT = 0x7FFFFFFFu, /*!< 枚举最大值，确保枚举为32位 */
} ux_anc_channel_t;

/**
 * @brief ANC麦克风ID枚举
 * @details 定义ANC功能使用的模拟麦克风（AMIC）和数字麦克风（DMIC）ID
 *         AMIC编号范围0-3，DMIC编号从0x10开始以区分
 */
typedef enum {
    ANC_AMIC0 = 0, /*!< 模拟麦克风0 */
    ANC_AMIC1,     /*!< 模拟麦克风1 */
    ANC_AMIC2,     /*!< 模拟麦克风2 */
    ANC_AMIC3,     /*!< 模拟麦克风3 */

    ANC_DMIC0 = 0x10, /*!< 数字麦克风0 */
    ANC_DMIC1,        /*!< 数字麦克风1 */
    ANC_DMIC2,        /*!< 数字麦克风2 */
    ANC_DMIC3,        /*!< 数字麦克风3 */

    ANC_MIC_MAX,                /*!< 麦克风最大数量 */
    ANC_MIC_BUTT = 0x7FFFFFFFu, /*!< 枚举最大值 */
} ux_anc_mic_id;

/**
 * @brief ANC通道与麦克风选择关联结构体
 * @details 描述单个ANC通道与其关联的麦克风之间的映射关系
 * @note  通过配置mic_sel可以实现任意麦克风到任意ANC通道的灵活路由
 */
typedef struct {
    ux_anc_channel_t channel; /*!< ANC通道类型 */
    ux_anc_mic_id mic;        /*!< 麦克风ID（AMIC或DMIC） */
} anc_channel_mic_sel;

/**
 * @brief ANC配置结构体
 * @details 配置ANC模块的多个通道与麦克风的映射关系
 * @note  mic_sel数组大小为ANC_MIC_NUM_MAX（3），表示最多支持3组通道-麦克风映射
 */
typedef struct {
    anc_channel_mic_sel mic_sel[ANC_MIC_NUM_MAX]; /*!< 通道与麦克风选择数组 */
} ux_anc_config_t;

/**
 * @brief 通用EQ参数结构体
 * @details 存储均衡器（EQ）的参数配置，包含参数尺寸和数据
 * @note  data数组大小为AUDIO_COM_EQ_PARAM_SIZE（120），表示最多120个32位参数
 */
typedef struct {
    uint32_t size;                          /*!< EQ参数实际数据长度，单位字节 */
    uint32_t data[AUDIO_COM_EQ_PARAM_SIZE]; /*!< EQ参数数据缓冲区 */
} audio_com_eq_param_t;

/**
 * @brief 保存ANC配置到持久化存储
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 * @note  调用后会将当前ANC配置写入Flash或非易失存储
 */
errcode_t ux_audio_anc_save(void);

/**
 * @brief 从持久化存储恢复ANC配置
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 * @note  上电时调用，从存储介质读取之前保存的ANC参数
 */
errcode_t ux_audio_anc_restore(void);

/**
 * @brief 获取当前ANC功能开关状态
 * @return bool true表示ANC已开启，false表示关闭
 */
bool ux_audio_get_anc_state(void);

/**
 * @brief 获取当前ANC工作模式
 * @return ux_api_audio_anc_mode_t 当前ANC模式（如前馈、反馈、混合等）
 */
ux_api_audio_anc_mode_t ux_audio_get_anc_mode(void);

/**
 * @brief 切换ANC开关状态
 * @param anc_state true开启ANC，false关闭ANC
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t ux_audio_switch_anc_state(bool anc_state);

/**
 * @brief 切换ANC工作模式
 * @param anc_mode 目标ANC模式
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t ux_audio_switch_anc_mode(ux_api_audio_anc_mode_t anc_mode);

/**
 * @brief 设置ANC模式
 * @param anc_mode 目标ANC模式
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 * @note  与switch_anc_mode区别在于此函数直接设置，不进行状态检查
 */
errcode_t ux_audio_anc_set_mode(ux_api_audio_anc_mode_t anc_mode);

/**
 * @brief 禁用ANC功能
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t ux_audio_anc_disable(void);

/**
 * @brief 使能ANC功能
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t ux_audio_anc_enable(void);

/**
 * @brief 设置PGA（可编程增益放大器）增益值
 * @param pga_gain 目标增益值，单位根据实际硬件确定
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t ux_audio_set_pga_gain(int32_t pga_gain);

/**
 * @brief 获取当前PGA增益值
 * @return int32_t 当前PGA增益值
 */
int32_t ux_audio_get_pga_gain(void);

/**
 * @brief 获取指定ID的EQ参数
 * @param id EQ参数索引ID
 * @param eq_param 输出参数，指向存储EQ参数的结构体
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t ux_audio_get_eq_param(uint8_t id, audio_com_eq_param_t *eq_param);

/**
 * @brief 保存EQ参数到持久化存储
 * @param id EQ参数索引ID
 * @param eq_param 输入参数，指向要保存的EQ参数结构体
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t ux_audio_save_eq_param(uint8_t id, audio_com_eq_param_t *eq_param);

/**
 * @brief 设置当前使用的EQ参数
 * @param eq_param 输入参数，指向要设置的EQ参数结构体
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t ux_audio_set_eq_param(audio_com_eq_param_t *eq_param);

/**
 * @brief 获取当前EQ模式
 * @param mode 输出参数，指向存储EQ模式的变量
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t ux_audio_get_eq_mode(ux_api_audio_eq_mode_t *mode);

/**
 * @brief 设置降噪（NR）模式
 * @param mode 目标降噪模式
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t ux_audio_set_nr_mode(ux_api_audio_nr_mode_t mode);

/**
 * @brief 获取当前降噪（NR）模式
 * @param mode 输出参数，指向存储降噪模式的变量
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t ux_audio_get_nr_mode(ux_api_audio_nr_mode_t *mode);

/**
 * @brief 获取低切滤波器（Low Cut）开关状态
 * @param enable 输出参数，指向存储启用状态的变量
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 * @note  低切滤波器用于切除低频噪声（如风噪、电流声等）
 */
errcode_t ux_audio_get_low_cut(bool *enable);

/**
 * @brief 设置用户音频参数
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 * @note  根据用户偏好设置更新音频处理参数
 */
errcode_t ux_audio_set_user_param(void);

/**
 * @brief 更新指定音频流的EQ参数
 * @param audio_stream_type 音频流类型（播放、录音等）
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t ux_audio_update_stream_eq_param(ux_api_audio_stream_type_t audio_stream_type);

/**
 * @brief 恢复音频流EQ参数到默认状态
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t ux_audio_stream_eq_param_restore(void);

/**
 * @brief 设置传输模式
 * @param transmit_mode 目标传输模式（如星闪、蓝牙等）
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t ux_audio_set_transmit_mode(ux_api_sys_transmit_mode_t transmit_mode);

/**
 * @brief 设置降噪参数
 * @param info 降噪参数信息结构体指针
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t ux_audio_set_nr(const ux_api_audio_nr_info_t *info);

/**
 * @brief 获取指定音频流的降噪模式
 * @param stream 音频流类型
 * @return ux_api_audio_nr_mode_t 当前降噪模式
 */
ux_api_audio_nr_mode_t ux_audio_get_nr(ux_api_audio_stream_type_t stream);

/**
 * @brief 远端音效模块初始化
 * @note  初始化远端音频效果处理所需的资源和状态
 */
void ux_audio_remote_effect_init(void);

/**
 * @brief 设置指定输出端口的混音矩阵
 * @param port 音频输出端口
 * @param matrix 混音矩阵参数指针
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t ux_audio_set_mix_matrix(ux_api_audio_output_port_t port, ux_api_audio_mix_matrix_t *matrix);

/**
 * @brief 获取指定输出端口的混音矩阵
 * @param port 音频输出端口
 * @param matrix 输出参数，指向存储混音矩阵指针的变量
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t ux_audio_get_mix_matrix(ux_api_audio_output_port_t port, ux_api_audio_mix_matrix_t **matrix);

/**
 * @brief 恢复混音矩阵信息到默认状态
 * @return errcode_t 返回错误码，ERRCODE_SUCC表示成功
 */
errcode_t ux_audio_mix_matrix_info_restore(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

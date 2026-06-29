/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: AlipayHsc32I1Model
 * Create: 2025-06-06
 */

#ifndef ALIPAY_HSC32I1_MODEL_H
#define ALIPAY_HSC32I1_MODEL_H

#include "alipay_feature.h"
#include "cmsis_os.h"
#include "i2c.h"
#include "connection_manage.h"

namespace OHOS {
constexpr uint16_t ALIPAY_BIND_STATUS_TIMER_MS = 2000;
constexpr uint16_t ALIPAY_PAY_RESULT_TIMER_MS = 2000;
constexpr uint16_t ALIPAY_BIND_STR_MAX_LENGTH = 256;
constexpr uint16_t ALIPAY_PAY_STR_MAX_LENGTH = 20;
constexpr uint16_t ALIPAY_PAY_AMOUNT_STR_MAX_LENGTH = 32;
constexpr uint16_t ALIPAY_PAY_EXT_DATA_MAX_LENGTH = 128;
constexpr uint16_t ALIPAY_NICKNAME_MAX_LENGTH = 128;
constexpr uint16_t ALIPAY_ID_STR_MAX_LENGTH = 128;
constexpr uint16_t ALIPAY_AID_STR_MAX_LENGTH = 256;
enum nfc_status {
    DEVICE_IDLE,
    DEVICE_DISCOVERY,
    DEVICE_POLL_ACTIVE,
    DEVICE_OPEN,
};

enum pay_err_index {
    ERR_CLOSE = 0,
    ERR_UNBIND,
    ERR_FAILED,
    ERR_UNKNOWN,
    ERR_OPEN_NET_ERROR,
    ERR_BIND_TIMEOUT,
    ERR_TAP_PAY_ERROR,
    ERR_TAP_NET_ERROR,
    ERR_TAP_DEVICE_INVALIDATION,
    ERR_PAYING_NET_ERROR,
    ERR_TAP_TIMEOUT,
};

const uint8_t ALIPAY_NET_FAIL = 17;
const uint8_t ALIPAY_PAYMENT_STATUS_TIMEOUT = 18;

class AlipayHsc32I1Model {
public:
    AlipayHsc32I1Model();
    AlipayHsc32I1Model(const AlipayHsc32I1Model &) = delete;
    AlipayHsc32I1Model &operator=(const AlipayHsc32I1Model &) = delete;
    virtual ~AlipayHsc32I1Model();
    static AlipayHsc32I1Model *GetInstance(void);
    bool AlipayInitModel(void);
    void AlipayDeinitModel(void);
    bool AlipayGetBindstate(void);
    int32_t AlipayGetBindCode(uint8_t *bindStr);
    int32_t AlipayGetPayCode(uint8_t *payStr);
    int32_t AlipayGetNickName(uint8_t *nickName);
    int32_t AlipayGetIdName(uint8_t *idStr);
    int32_t AlipayGetAidCode(int8_t *aidStr);
    void AlipayUnbind(void);
    bool AlipayStartBind(void);
    bool AlipayFinishBind(void);
    void AlipayStopBindTimer(void);
    void AlipayEnterLpm();
    void AlipayExitLpm();
    bool AlipayGetBindingFlag(void);
    void AlipaySetBindingFlag(bool flag);
    void AlipayReset(void);
    bool AlipayStartQueryPayResult(void);
    void AlipayStopQueryPayResult(void);
    char *AlipayGetCurrentPayCode(void);
    char *AlipayGetPayAmount(void);
    char *AlipayGetPayExtData(void);
    void AlipaySetPayErrCodeIndex(pay_err_index index);
    pay_err_index AlipayGetPayErrCodeIndex(void);
    bool GetNetState();  // 获取网络状态
    std::string AlipayGetName();
#ifdef SUPPORT_ALIPAY_TRANS
    int32_t AlipayGetLastTranscode();
    int32_t AlipayGetLocalTranslist();
    int32_t AlipayGetOnlineTranslist();
    int32_t AlipayCheckTransCard(uint32_t index);
    int32_t AlipayGetTransCode(uint32_t index);
    bool AlipayTransStartGetListsTimer(void);
    void AlipayTransStopGetListsTimer(void);
    uint32_t AlipaySendMsg(uint32_t msgId);
    mc_alipay_trans_card_info_t *AlipayTransGetCardList(void);
    int32_t AlipayTransGetCardNum(void);
    uint8_t *AlipayGetTransCode(void);
    uint32_t AlipayGetTransCodeLen(void);
    uint32_t AlipayGetCardNum(void);
    void AlipaySetCardNum(uint32_t cardNum);
    uint32_t AlipayGetCardSum(void);
    void AlipaySetCardSum(uint32_t cardSum);
    char *AlipayGetCardTitle(uint32_t index);
    char *AlipayGetCardTitle(void);
#endif
#ifdef SUPPORT_ALIPAY_TAP
    void AlipayGetTapPayResult();                             // 获取碰码牌的支付结果
    alipay_union_code_order_info_t GetOrderInfo();            // 获取订单信息 包含支付金额
    void SetOrderInfo(alipay_union_code_order_info_t &info);  // 重置订单信息
    std::string AlipayGetTapInputAmount(void);                // 获取碰码牌输入支付金额
    void AlipaySetTapInputAmount(std::string amount);
    std::string AlipayGetTapSucessAmount(void);  // 获取碰机具实际支付金额
    void AlipaySetTapSucessAmount(std::string amount);
    bool AlipayGetNFCPayResult();  // 获取碰机具轮询开关
    void AlipaySetNFCPayResult(bool value);
    bool AlipayGetTapGuideFlag(void);
    void AlipaySetTapGuideFlag(bool flag);
    bool AlipayGetTapLightFlag(void);
    void AlipaySetTapLightFlag(bool flag);
#endif
    char payCode_[ALIPAY_PAY_STR_MAX_LENGTH] = {'\0'};
    char payAmount_[ALIPAY_PAY_AMOUNT_STR_MAX_LENGTH] = {'\0'};
    char payExtData_[ALIPAY_PAY_EXT_DATA_MAX_LENGTH] = {'\0'};

private:
    bool tapLightFlag_{true};
    uint32_t bindingFlag_ = 0;
    pay_err_index errIndex_ = ERR_CLOSE;
    osTimerId_t bindTimerId_ = nullptr;
    osTimerId_t payResultTimerId_ = nullptr;
    const alipay_svr_api_t *alipayApi_;
    std::string nameStr_;
#ifdef SUPPORT_ALIPAY_TRANS
    uint32_t cardNum_ = 0;
    uint32_t cardSum_ = 0;
    mc_alipay_trans_card_info_t *cardInfo_ = nullptr;
    uint8_t *transCode_ = nullptr;
    uint32_t transCodeLen_ = 0;
    char cardTitle_[ALIPAY_TRANS_CARD_MEMBER_LEN] = {'\0'};
#endif
#ifdef SUPPORT_ALIPAY_TAP
    alipay_union_code_order_info_t orderInfo_{};  // 订单信息，用于碰码牌
    alipay_union_code_pay_result_t payResult_{};  // 支付结果，用于碰码牌
    std::string tapPayAmount_;                  // 碰码牌支付金额
    std::string tapSuccessAmount_;              // 碰机具轮询到的支付成功金额
    bool nfcPayResult_ = false;                 // 轮询NFC碰机具开关
    bool tapGuideFlag_{true};
#endif
};
}

#endif
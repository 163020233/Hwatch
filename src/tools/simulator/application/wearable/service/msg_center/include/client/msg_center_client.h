/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: simulator mouse driver definition
 */

#ifndef MSG_CENTER_CLIENT_H
#define MSG_CENTER_CLIENT_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MSG_CENTER_SERVICE_NAME "MsgCenter"

int MsgCenterSendMsg(int msgId, int msgValue, void *data, int dataLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* MSG_CENTER_CLIENT_H */

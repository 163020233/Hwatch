/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: Standalone AT command for the voice_cloud_LLM sample. \n
 *
 * History: \n
 * 2026-06-27, Create file. \n
 */

/**
 * Usage: \n
 *   AT+VOICECLOUDLLM=START                       start the wakeup -> cloud dialogue \n
 *   AT+VOICECLOUDLLM=STOP                        stop and return to wakeup idle \n
 *   AT+VOICECLOUDLLM=CONFIG,<bot_id>,<token>     override cloud auth at runtime \n
 */
#include "at.h"
#include "ohos_init.h"
#include "errcode.h"
#include "voice_cloud_llm.h"
#include "voice_cloud_llm_config.h"

#include <stdbool.h>
#include <string.h>

/* cmd_id must be unique across all registered AT commands. Reserved for voice_cloud_LLM. */
#define VOICE_CLOUD_LLM_AT_CMD_ID     0x2C01
/* Per-parameter max length (covers a long auth token). */
#define VOICE_CLOUD_LLM_AT_PARA_MAX   256

/*
 * The AT framework splits the parameter list on commas, so each field of
 * AT+VOICECLOUDLLM=<action>[,<arg1>[,<arg2>]] lands in a separate paraN. All three are
 * OPTIONAL, so 1 param (START/STOP) and 3 params (CONFIG,<bot>,<token>) both pass the
 * framework's parameter-count check.
 *   para1 = action (START | STOP | CONFIG)
 *   para2 = bot_id   (CONFIG only)
 *   para3 = token    (CONFIG only)
 */
typedef struct {
    uint32_t para_map;
    char *para1;
    char *para2;
    char *para3;
} voice_cloud_llm_at_args_t;

#define VOICE_CLOUD_LLM_AT_STRING_PARA(field, is_last) \
    { \
        .type = AT_SYNTAX_TYPE_STRING, \
        .last = (is_last), \
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_FIX_CASE | AT_SYNTAX_ATTR_MAX_LENGTH, \
        .entry.string.max_length = VOICE_CLOUD_LLM_AT_PARA_MAX, \
        .offset = offsetof(voice_cloud_llm_at_args_t, field), \
    }

static const at_para_parse_syntax_t g_voice_cloud_llm_at_syntax[] = {
    VOICE_CLOUD_LLM_AT_STRING_PARA(para1, false),
    VOICE_CLOUD_LLM_AT_STRING_PARA(para2, false),
    VOICE_CLOUD_LLM_AT_STRING_PARA(para3, true),
};

static at_ret_t voice_cloud_llm_at_process(const void *arg)
{
    const voice_cloud_llm_at_args_t *args = (const voice_cloud_llm_at_args_t *)arg;
    if (args == NULL || args->para1 == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }

    if (strcmp(args->para1, "START") == 0) {
        return (VoiceCloudLlmStart() == ERRCODE_SUCC) ? AT_RET_OK : AT_RET_RUN_ERROR;
    }
    if (strcmp(args->para1, "STOP") == 0) {
        return (VoiceCloudLlmStop() == ERRCODE_SUCC) ? AT_RET_OK : AT_RET_RUN_ERROR;
    }
    if (strcmp(args->para1, "CONFIG") == 0) {
        /* CONFIG,<bot_id>,<token> — both extra args are required. */
        if (args->para2 == NULL || args->para2[0] == '\0' ||
            args->para3 == NULL || args->para3[0] == '\0') {
            return AT_RET_SYNTAX_ERROR;
        }
        errcode_t ret = VoiceCloudLlmSetAuth(args->para2, args->para3);
        return (ret == ERRCODE_SUCC) ? AT_RET_OK : AT_RET_RUN_ERROR;
    }

    return AT_RET_SYNTAX_ERROR;
}

static const at_cmd_entry_t g_voice_cloud_llm_at_cmd_table[] = {
    {
        .name = "VOICECLOUDLLM",
        .cmd_id = VOICE_CLOUD_LLM_AT_CMD_ID,
        .attribute = 0,
        .syntax = g_voice_cloud_llm_at_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)voice_cloud_llm_at_process,
        .read = NULL,
        .test = NULL,
    },
};

void voice_cloud_llm_at_register(void)
{
    uint32_t num = (uint32_t)(sizeof(g_voice_cloud_llm_at_cmd_table) /
                              sizeof(g_voice_cloud_llm_at_cmd_table[0]));
    /* Registration is best-effort: on failure the command simply is unavailable. */
    errcode_t ret = uapi_at_cmd_table_register(g_voice_cloud_llm_at_cmd_table, num,
                                               VOICE_CLOUD_LLM_AT_PARA_MAX);
    (void)ret;
}

/* Auto-register during the app_feature init phase (after the AT subsystem is up). */
APP_FEATURE_INIT(voice_cloud_llm_at_register);

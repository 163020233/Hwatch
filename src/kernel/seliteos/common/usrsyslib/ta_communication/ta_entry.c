/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: ta_entry source file.
 */

#include "ta_manager.h"
#include "los_printf.h"
#include "ta_entry.h"

static TEE_Result ta_open_session(struct ta_header *ta, TEE_Param *up, uint32_t param_type)
{
    TEE_Result ret = TEE_FAILURE;

    PRINT_INFO("%s!\n", __func__);
    if (ta->ops->create_entry_point != NULL) {
        ret = ta->ops->create_entry_point();
        if (ret != TEE_SUCCESS) {
            return ret;
        }
    }

    if (ta->ops->open_session_entry_point != NULL) {
        ret = ta->ops->open_session_entry_point(param_type, up, NULL);
        if ((ret != TEE_SUCCESS) && (ta->ops->create_entry_point != NULL) && (ta->ops->destroy_entry_point != NULL)) {
            ta->ops->destroy_entry_point();
        }
    }

    return ret;
}

static TEE_Result ta_close_session(struct ta_header *ta)
{
    PRINT_INFO("%s!\n", __func__);

    if (ta->ops->close_session_entry_point != NULL) {
        ta->ops->close_session_entry_point(NULL);
    }

    if (ta->ops->destroy_entry_point != NULL) {
        ta->ops->destroy_entry_point();
    }

    return TEE_SUCCESS;
}

static TEE_Result ta_invoke_command(struct ta_header *ta, TEE_Param *up,
                                    unsigned long cmd_id, uint32_t param_type)
{
    TEE_Result ret = TEE_FAILURE;

    PRINT_INFO("%s!\n", __func__);
    if (ta->ops->invoke_command_entry_point != NULL) {
        ret = ta->ops->invoke_command_entry_point(NULL, cmd_id, param_type, up);
    }

    return ret;
}

TEE_Result utee_entry(unsigned long func, unsigned long ta_info,
                      TEE_Param *up, unsigned long cmd_id, uint32_t param_type)
{
    TEE_Result res;
    struct ta_header *ta = (struct ta_header *)(uintptr_t)ta_info;

    if (ta == NULL || ta->ops == NULL) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    switch (func) {
        case UTEE_ENTRY_FUNC_OPEN_SESSION:
            res = ta_open_session(ta, up, param_type);
            break;
        case UTEE_ENTRY_FUNC_CLOSE_SESSION:
            res = ta_close_session(ta);
            break;
        case UTEE_ENTRY_FUNC_INVOKE_COMMAND:
            res = ta_invoke_command(ta, up, cmd_id, param_type);
            break;
        default:
            res = TEE_ERROR_NOT_IMPLEMENTED;
            break;
    }

    return res;
}

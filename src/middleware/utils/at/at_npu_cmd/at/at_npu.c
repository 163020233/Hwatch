/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: At npu func \n
 *
 */

#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "at_cmd.h"
#include "at_npu_cmd_register.h"
#include "at_npu_cmd_table.h"
#include "debug_print.h"
#include "ipc.h"
#include "ipc_new_shared.h"
#include "at_npu.h"

#if defined(CONFIG_NPU_LIB) && defined(CONFIG_NPU_DEBUG)
unsigned int shell_cmd_execute(char *param);
unsigned int shell_cmd_init(void);
#endif

#define AT_NPU_FUNC_NUM (sizeof(at_npu_cmd_parse_table) / sizeof(at_npu_cmd_parse_table[0]))
void los_at_npu_cmd_register(void)
{
    print_str("los_at_npu_cmd_register EXCUTE\r\n");
    uapi_at_npu_register_cmd(at_npu_cmd_parse_table, AT_NPU_FUNC_NUM);
}

static at_ret_t npu_handle_cmd_local(uint8_t *cmd_data)
{
#if defined(CONFIG_NPU_LIB) && defined(CONFIG_NPU_DEBUG)
    static uint32_t shell_init = 0;
    if (shell_init == 0) {
        shell_cmd_init();
        shell_init++;
    }
    (void)shell_cmd_execute((char *)cmd_data);
    return AT_RET_OK;
#else
    unused(cmd_data);
    PRINT("NPU AT not support on Core[%d]\n", CORE);
    return AT_RET_CMD_PARA_ERROR;
#endif
}

static at_ret_t npu_handle_cmd_ipc(uint32_t core_id, const uint8_t *cmd, uint32_t cmd_len)
{
#ifndef NPU_AT_USE_IPC
    unused(core_id);
    unused(cmd);
    unused(cmd_len);
#else
    errcode_t ret;
    ipc_msg_info_t msg_info;
    msg_info.dst_core = core_id;
    msg_info.priority = 1;
    msg_info.channel = 0;
    msg_info.msg_id = IPC_MSG_NPU_IPC_CMD;
    msg_info.buf_addr = (uint8_t *)cmd;
    msg_info.buf_len = cmd_len;
    ret = uapi_ipc_send_msg_async(&msg_info);
    if (ret != ERRCODE_SUCC) {
        PRINT("npu ipc cmd send failed, errcode:0x%x.", ret);
        return (at_ret_t)ret;
    }
#endif
    return AT_RET_OK;
}

at_ret_t npu_handle_cmd(const npucmd_args_t *args)
{
    at_ret_t ret;
    errno_t sec_ret;
    uint8_t *cmd_data;
    size_t cmd_len = strlen((const char *)args->para1);
    if (cmd_len == 0) {
        return AT_RET_SYNTAX_ERROR;
    }
    cmd_data = osal_kmalloc(cmd_len + 1, OSAL_GFP_KERNEL);
    if (cmd_data == NULL) {
        return AT_RET_MALLOC_ERROR;
    }
    sec_ret = memcpy_s(cmd_data, cmd_len, args->para1, cmd_len);
    if (sec_ret != EOK) {
        return AT_RET_MEM_API_ERROR;
    }
    cmd_data[cmd_len] = '\0';
#ifdef NPU_AT_USE_IPC
    ret = npu_handle_cmd_ipc(CORES_SEN_CORE, cmd_data, cmd_len + 1);
#else
    ret = npu_handle_cmd_local(cmd_data);
#endif
    osal_kfree(cmd_data);
    return ret;
}

at_ret_t npu_handle_cmd_help(void)
{
    at_ret_t ret;
    const char help_str[] = "help";
#ifdef NPU_AT_USE_IPC
    ret = npu_handle_cmd_ipc(CORES_SEN_CORE, (const uint8_t *)help_str, sizeof(help_str));
#else
    ret = npu_handle_cmd_local((uint8_t *)help_str);
#endif
    return ret;
}

at_ret_t npu_handle_cmd_v2(const npucmd_v2_args_t *args)
{
    at_ret_t ret;
    errno_t sec_ret;
    uint8_t *cmd_data;
    uint32_t core_id = args->core_id;
    size_t cmd_len = strlen((const char *)args->para1);
    if (cmd_len == 0) {
        return AT_RET_SYNTAX_ERROR;
    }
    cmd_data = osal_kmalloc(cmd_len + 1, OSAL_GFP_KERNEL);
    if (cmd_data == NULL) {
        return AT_RET_MALLOC_ERROR;
    }
    sec_ret = memcpy_s(cmd_data, cmd_len, args->para1, cmd_len);
    if (sec_ret != EOK) {
        osal_kfree(cmd_data);
        return AT_RET_MEM_API_ERROR;
    }
    cmd_data[cmd_len] = '\0';

    if (core_id == CORE) {
        ret = npu_handle_cmd_local(cmd_data);
    } else {
        ret = npu_handle_cmd_ipc(core_id, cmd_data, cmd_len + 1);
    }
    osal_kfree(cmd_data);
    return ret;
}

at_ret_t npu_handle_cmd_help_v2(void)
{
    PRINT("[NPU AT CMD V2]syntax: AT^NPU=core_id,cmd_string\n", CORE);
    return AT_RET_OK;
}

at_ret_t load2mem_cmd_help(void)
{
    PRINT("[LOAD2MEM]syntax: AT^LOADTOMEM=/user/path/to/file,0xdeadbeaf\n");
    return AT_RET_OK;
}

at_ret_t load2mem_cmd(const load2mem_args_t *args)
{
    int fd;
    struct stat st;
    const char *path = (const char *)args->path;
    const char *addr_str = (const char *)args->addr;
    char *endp;
    const char *valid_path;
    uintptr_t addr = osal_strtoul(addr_str, &endp, 0);
    if (endp == addr_str || *endp != '\0' || addr == 0) {
        PRINT("[LOAD2MEM]addr %s convert failed\n", addr_str);
        return AT_RET_CMD_PARA_ERROR;
    }
    valid_path = realpath(path, NULL);
    if (valid_path == NULL) {
        return AT_RET_CMD_PARA_ERROR;
    }
    fd = open(valid_path, O_RDONLY);
    if (fd < 0) {
        PRINT("[LOAD2MEM]path %s open failed\n", path);
        free((void *)valid_path);
        return AT_RET_RUN_ERROR;
    }
    free((void *)valid_path);

    fstat(fd, &st);
    if (read(fd, (void *)addr, st.st_size) < 0) {
        PRINT("[LOAD2MEM]path %s read failed\n", path);
        close(fd);
        return AT_RET_RUN_ERROR;
    }
    PRINT("[LOAD2MEM]file %s load to [0x%x] OK!\n", path, addr);
    close(fd);
    return AT_RET_OK;
}

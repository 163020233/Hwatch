/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: dfx manage
 */

#include "vau_dfx.h"
#include "platform_core.h"
#include "drv_gfx_port.h"
#include "vau_mem.h"
#include "soc_osal.h"
#include "vau_reg_define.h"
#include "gpu_mem_config.h"
#include "gpu_crg.h"

#ifdef CONFIG_GFX_PROC_SUPPORT
STATIC td_u32 g_vau_version = 0x20260415;
STATIC td_u32 g_vau_debug_mask = VAU_DFX_MASK_NODE_ALL;
vau_dfx_info g_vau_dfx_info = {0};

#ifdef GPU_DUMP_SUPPORT
td_u8 *g_dump_addr = (td_u8 *)GPU_DUMP_DEBUG_ADDR;
td_u32 g_dump_pool_size = GPU_DUMP_DEBUG_SIZE;
#endif

STATIC td_void vau_dfx_print_func_time_info(td_void)
{
    td_u32 list_pfcnt;
    uapi_reg_read(HAL_VAU_LIST_PFCNT, list_pfcnt);

    gpu_print("--------interface cost(us)--------\n");
    gpu_print("create_cost      :%u\n", (td_u32)(g_vau_dfx_info.func_info.time_info.create_cost));
    gpu_print("destroy_cost     :%u\n", (td_u32)(g_vau_dfx_info.func_info.time_info.destroy_cost));
    gpu_print("blit_cost        :%u\n", (td_u32)(g_vau_dfx_info.func_info.time_info.blit_cost));
    gpu_print("compose_cost     :%u\n", (td_u32)(g_vau_dfx_info.func_info.time_info.compose_cost));
    gpu_print("submit_cost      :%u\n", (td_u32)(g_vau_dfx_info.func_info.time_info.submit_cost));
    gpu_print("wait_done_cost   :%u\n", (td_u32)(g_vau_dfx_info.func_info.time_info.wait_for_done_cost));
    gpu_print("wait_all_cost    :%u\n", (td_u32)(g_vau_dfx_info.func_info.time_info.wait_all_done_cost));
    gpu_print("list pfcnt/time  :%u/%u\n", list_pfcnt, list_pfcnt / 200); // 200M AXI
    gpu_print("suspend/resume   :%u/%u\n", g_vau_dfx_info.suspend_cnt, g_vau_dfx_info.resume_cnt);
}

STATIC td_void vau_dfx_print_func_cnt_info(td_void)
{
    td_u32 cnt = g_vau_dfx_info.func_info.func_cnt.submit_cnt - g_vau_dfx_info.func_info.func_cnt.submit_usr_cnt;
    td_u32 hard_start_cnt, hard_list_finish_cnt, hard_int_cnt;
    uapi_reg_read(HAL_VAU_START_CNT, hard_start_cnt);
    uapi_reg_read(HAL_VAU_LIST_FINISH_CNT, hard_list_finish_cnt);
    uapi_reg_read(HAL_VAU_INT_CNT, hard_int_cnt);

    gpu_print("--------call count info-----------\n");
    gpu_print("create_cnt       :%u\n",  g_vau_dfx_info.func_info.func_cnt.create_cnt);
    gpu_print("cancel_cnt       :%u\n",  g_vau_dfx_info.func_info.func_cnt.cancel_cnt);
    gpu_print("destroy_cnt      :%u\n",  g_vau_dfx_info.func_info.func_cnt.destroy_cnt);
    gpu_print("blit_cnt         :%u\n",  g_vau_dfx_info.func_info.func_cnt.blit_cnt);
    gpu_print("compose_cnt      :%u\n",  g_vau_dfx_info.func_info.func_cnt.compose_cnt);
    gpu_print("submit_cnt       :%u ",   g_vau_dfx_info.func_info.func_cnt.submit_cnt);
    gpu_print("(usr:%u drv:%u",          g_vau_dfx_info.func_info.func_cnt.submit_usr_cnt, cnt);
    gpu_print(" submit from int :%u",    g_vau_dfx_info.func_info.func_cnt.submit_from_int);
    gpu_print(" submit from user:%u)\n", g_vau_dfx_info.func_info.func_cnt.submit_from_user);
    gpu_print("wait_done_cnt    :%u\n",  g_vau_dfx_info.func_info.func_cnt.wait_for_done_cnt);
    gpu_print("wait_all_cnt     :%u\n",  g_vau_dfx_info.func_info.func_cnt.wait_all_done_cnt);
    gpu_print("isr_cnt          :%u ",   g_vau_dfx_info.func_info.func_cnt.isr_cnt);
    gpu_print("(node_end:%u",            g_vau_dfx_info.func_info.isr_cnt.isr_node_end);
    gpu_print(" tunl_done:%u",           g_vau_dfx_info.func_info.isr_cnt.isr_tunl_done);
    gpu_print(" timeout:%u",             g_vau_dfx_info.func_info.isr_cnt.isr_timeout);
    gpu_print(" bus_err:%u",             g_vau_dfx_info.func_info.isr_cnt.isr_bus_err);
    gpu_print(" list_end:%u",            g_vau_dfx_info.func_info.isr_cnt.isr_list_end);
    gpu_print(" disp_done:%u",           g_vau_dfx_info.func_info.isr_cnt.isr_disp_done);
    gpu_print(" conflict:%u",            g_vau_dfx_info.func_info.isr_cnt.isr_conflict);
    gpu_print("\n                    ");
    gpu_print(" draw_err:%u",            g_vau_dfx_info.func_info.isr_cnt.isr_draw_err);
    gpu_print(" et_oversize:%u",         g_vau_dfx_info.func_info.isr_cnt.isr_et_oversize);
    gpu_print(" draw_oversize:%u",       g_vau_dfx_info.func_info.isr_cnt.isr_draw_oversize);
    gpu_print(" dcmp_cfg_err:%u",        g_vau_dfx_info.func_info.isr_cnt.isr_dcmp_cfg_err);
    gpu_print(" dcmp_bs_err:%u",         g_vau_dfx_info.func_info.isr_cnt.isr_dcmp_bs_err);
    gpu_print(" zero:%u)\n",             g_vau_dfx_info.func_info.isr_cnt.isr_zero);

    gpu_print("tasklet_enter_cnt:%u\n",  g_vau_dfx_info.func_info.func_cnt.tasklet_enter_cnt);
    gpu_print("tasklet_exit_cnt :%u\n",  g_vau_dfx_info.func_info.func_cnt.tasklet_exit_cnt);
    gpu_print("execute_node_cnt :%u\n",  g_vau_dfx_info.func_info.func_cnt.execute_node_cnt);
    // hard int count
    gpu_print("hard int cnt     :%u ", hard_int_cnt);
    gpu_print("(hard start cnt  :%u", hard_start_cnt);
    gpu_print(" hard list finish cnt:%u)\n", hard_list_finish_cnt);

    td_u32 *p_checksum = (td_u32 *)HAL_VAU_CHECKSUM;
    gpu_print("checksum         :list:%#x|s1:%#x|s2:%#x|s2c:%#x|model:%#x|dst:%#x\n",
        p_checksum[0], p_checksum[1], p_checksum[2], p_checksum[3], p_checksum[4], p_checksum[5]); // 2,3,4,5：index
#ifdef GPU_1A22
    vau_clk_info clk_info = {0};
    gpu_crg_get_vau_clk(0, &clk_info);
    gpu_print("vau_clk         :%u\n", clk_info.vau_clk);
    gpu_print("axi_clk         :%u\n", clk_info.axi_clk);
#endif
}

td_s32 uapi_vau_dfx_proc_read(int argc, const char *argv[])
{
    if (argc >= 1) {
        g_vau_debug_mask = strtoul(argv[0], TD_NULL, 10); // 10:index
        gpu_print("debug mask = %#x\n", g_vau_debug_mask);
        gpu_print("0:none 1:node_all 2:node_nozero 4:path 8:surface\n");
        return EXT_SUCCESS;
    }

    gpu_print("\n---------VAU PROC %x------\n", g_vau_version);
    vau_dfx_print_func_time_info();
    vau_dfx_print_func_cnt_info();
    gpu_print("\n---------VAU PROC END-----------\n");
    return EXT_SUCCESS;
}

td_bool vau_dfx_check_mask(td_u32 mask)
{
    return ((g_vau_debug_mask & mask) == 0) ? TD_FALSE : TD_TRUE;
}
#endif

STATIC td_void dfx_print_path(td_void)
{
    td_u8 *cmds;
    td_float *datas;
    td_u16 cmd_num, data_num;
    volatile td_u32 value;

    uapi_reg_read(HAL_VAU_DRAW_PATH_CMD_ADDR, value);
    cmds = (td_u8 *)(uintptr_t)value;

    uapi_reg_read(HAL_VAU_DRAW_PATH_DATA_ADDR, value);
    datas = (td_float *)(uintptr_t)value;

    uapi_reg_read(HAL_VAU_DRAW_PATH_NUM_ADDR, value);
    cmd_num  = value & 0xFFFF;
    data_num = (value & 0xFFFF0000) >> 16; // 16:index

    if ((cmds == TD_NULL) || (datas == TD_NULL) ||
        (cmd_num == 0) || (data_num == 0)) {
        return;
    }

    gpu_print("cmd[%u] = { ", cmd_num);
    for (td_u32 i = 0; i < cmd_num; i++) {
        gpu_print("%u, ", (td_u32)cmds[i]);
    }
    gpu_print("};\n");

    gpu_print("data[%u] = { ", data_num);
    for (td_u32 i = 0; i < data_num; i++) {
        gpu_print("%f, ", datas[i]);
    }
    gpu_print("};\n");

#ifdef GPU_DUMP_SUPPORT
    *((td_u32*)g_dump_addr) = cmd_num;
    g_dump_addr += 0x4;
    g_dump_pool_size -= 0x4;
    if (memcpy_s((void *)g_dump_addr, g_dump_pool_size, cmds, sizeof(td_u8) * cmd_num) != EOK) {
        gpu_err("memcpy cmds failure!");
        return;
    }
    g_dump_addr += uapi_byte_align(sizeof(td_u8) * cmd_num, 16) - 0x4; // 16byte align
    g_dump_pool_size -= uapi_byte_align(sizeof(td_u8) * cmd_num, 16) - 0x4; // 16byte align

    *((td_u32*)g_dump_addr) = data_num;
    g_dump_addr += 0x4;
    g_dump_pool_size -= 0x4;
    if (memcpy_s((void *)g_dump_addr, g_dump_pool_size, datas, sizeof(td_float) * data_num) != EOK) {
        gpu_err("memcpy datas failure!");
        return;
    }
    g_dump_addr += sizeof(td_float) * data_num;
    g_dump_pool_size -= sizeof(td_float) * data_num;
#endif
}

STATIC td_void dfx_print_register(td_u32 start, td_u32 size)
{
    if ((start == 0) || (start == 0xFFFFFFFF)) {
        gpu_print("dfx_print_register start is zero!");
        return;
    }
#ifdef GPU_DUMP_SUPPORT
    memcpy_s((void *)g_dump_addr, g_dump_pool_size, (void *)start, size * 0x4);
    g_dump_addr += size * 0x4;
    g_dump_pool_size -= size * 0x4;
#endif
    td_u32 *addr = (td_u32 *)(uintptr_t)start;
    td_bool print_nozero = vau_dfx_check_mask(VAU_DFX_MASK_NODE_NOZERO);

#ifdef GPU_SUPPORT_CHR
    gpu_record_chr_reg(start, size, MT_VAU_MANAGER, MT_VAU_REGISTER, MT_VAU_TIMEOUT_REGISTER);
#endif
    for (td_u32 i = 0; i < size / 0x4; i++) {
        if ((addr[i * 0x4 + 0x0] == 0) && (addr[i * 0x4 + 0x1] == 0) &&
            (addr[i * 0x4 + 0x2] == 0) && (addr[i * 0x4 + 0x3] == 0) &&
            (print_nozero == TD_TRUE)) {
            continue;
        }

        gpu_print("%04X = %08X %08X %08X %08X\n", ((td_u32)(uintptr_t)addr + i * 16),         /* 16:index */
                  addr[i * 4], addr[i * 4 + 1], addr[i * 4 + 2], addr[i * 4 + 3]); /* 2,3,4:index */
    }

    return;
}

td_void vau_dfx_print_reg(td_void)
{
    dfx_print_register(HAL_VAU_REG_BASEADDR + 0x0,    0x80);  // 128 base node
    dfx_print_register(HAL_VAU_REG_BASEADDR + 0x200,  0x20);  // 32 draw
    dfx_print_register(HAL_VAU_REG_BASEADDR + 0x300,  0x20);  // 32 mapping
    dfx_print_register(HAL_VAU_REG_BASEADDR + 0x380,  0x20);  // 32 SP/HSV
    dfx_print_register(HAL_VAU_REG_BASEADDR + 0x400,  0x34);  // 52 tile/trans
    dfx_print_register(HAL_VAU_REG_BASEADDR + 0x500,  0x28);  // 40 APB start/reset/int_mask
    dfx_print_register(HAL_VAU_REG_BASEADDR + 0x700,  0xF0);  // 240 CSC
    dfx_print_register(HAL_VAU_REG_BASEADDR + 0xF00,  0x14);  // 20 SDF node
    dfx_print_register(HAL_VAU_REG_BASEADDR + 0xF80,  0x4E);  // 78 SDF goloal cfg
    dfx_print_register(HAL_VAU_REG_BASEADDR + 0x4000, 0x34);  // 52 debug

    td_u32 current_node;
    uapi_reg_read(HAL_VAU_REG_BASEADDR + 0x4004, current_node);
    dfx_print_register(current_node, 0xA0); // timeout software list node

    td_u32 draw_ctrl;
    uapi_reg_read(HAL_VAU_REG_BASEADDR + 0x200, draw_ctrl);

    if ((draw_ctrl & 0x80000000) == 0x80000000) {
        dfx_print_path();
    }

    g_vau_debug_mask = 0;

    return;
}

PLT_SEC_ITCM_T vau_dfx_info *vau_dfx_get_info(td_void)
{
    return &g_vau_dfx_info;
}

/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: gpu chr source file
 */

#include "gpu_chr.h"
#include "gpu_dfx.h"

#define MAX_INFO_DATA_LEN 128
#define MIPI_REG_BASEADDR 0X520D0000
#define DPU_REG_BASEADDR  0x52090000

td_void gpu_record_chr_reg(td_u32 addr, td_u32 cnt, td_u32 event, td_u32 info1, td_u32 info2)
{
    td_char tmp[MAX_INFO_DATA_LEN] = {0};
    td_char *temp_addr = TD_NULL;
    td_u32 temp_buf_len = MAX_INFO_DATA_LEN;

    for (uint32_t i = 0; i < cnt; i++) {
        volatile uint32_t value;
        uapi_reg_read((addr + i * 4), value); /* 4 is bytes */
        if (i % 4 == 0) { // 4: 4byte
            if (i != 0) {
                temp_addr = tmp + strlen(tmp);
                temp_buf_len = (MAX_INFO_DATA_LEN - strlen(tmp)) > 0 ? (MAX_INFO_DATA_LEN - strlen(tmp)) : 0;
                sprintf_s(temp_addr, temp_buf_len, "\n");
                gpu_chr_log_long(event, info1, info2, (td_u8 *)tmp, sizeof(tmp));
                memset_s(tmp, sizeof(tmp), 0, sizeof(tmp));
            }
            temp_addr = tmp + strlen(tmp);
            temp_buf_len = (MAX_INFO_DATA_LEN - strlen(tmp)) > 0 ? (MAX_INFO_DATA_LEN - strlen(tmp)) : 0;
            if (sprintf_s(temp_addr, temp_buf_len, "%X = ", addr + i * 4) < 0) { /* 4: 4byte */
                return;
            }
        }
        temp_addr = tmp + strlen(tmp);
        temp_buf_len = (MAX_INFO_DATA_LEN - strlen(tmp)) > 0 ? (MAX_INFO_DATA_LEN - strlen(tmp)) : 0;
        if (sprintf_s(temp_addr, temp_buf_len, " %08X", value) < 0) {
            return;
        }
    }
}

td_void print_mipi_and_dpu_reg(td_void)
{
    /* mipi reg */
    gpu_record_chr_reg(MIPI_REG_BASEADDR, 0x40, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_MIPI_REGISTER_INFO);
    gpu_record_chr_reg(MIPI_REG_BASEADDR + 0x100, 0x4, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_MIPI_REGISTER_INFO);
    gpu_record_chr_reg(MIPI_REG_BASEADDR + 0x120, 0x4, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_MIPI_REGISTER_INFO);
    gpu_record_chr_reg(MIPI_REG_BASEADDR + 0x180, 0x74, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_MIPI_REGISTER_INFO);
    gpu_record_chr_reg(MIPI_REG_BASEADDR + 0x200, 0x10, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_MIPI_REGISTER_INFO);
    gpu_record_chr_reg(MIPI_REG_BASEADDR + 0x300, 0x40, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_MIPI_REGISTER_INFO);
    /* dpu reg */
    gpu_record_chr_reg(DPU_REG_BASEADDR, 0x24, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0x300, 0x4, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0x400, 0x8, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0xC00, 0x4, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0xE00, 0x4, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0xF00, 0x4, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0xF40, 0x4, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0x1000, 0x20, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0x1100, 0x8, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0x7000, 0x4, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0x7080, 0x28, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0x7200, 0x4, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0x7800, 0x4, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0x7880, 0x60, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0x7B00, 0x10, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0x7C00, 0x8, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0x7D00, 0x8, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0x7E00, 0x4, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0xA000, 0x4, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0xA100, 0x14, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0xA500, 0x50, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0xD000, 0x2C, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0xD170, 0x8, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0xD300, 0x4, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0xD3f0, 0x4, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0x10000, 0x34, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0x10E00, 0xC4, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0x11E80, 0x4, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0x11F80, 0x4, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
    gpu_record_chr_reg(DPU_REG_BASEADDR + 0x12600, 0x18, MT_DPU_MANAGER, MT_REGISTER_INFO, MT_DPU_REGISTER_INFO);
}

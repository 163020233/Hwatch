/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2023. All rights reserved.
 * Description: DIAG - cmd id.
 */

#ifndef SOC_DIAG_CMD_ID_H
#define SOC_DIAG_CMD_ID_H

/* Common command ID */
#include "diag_cmd_id.h"

/* Customized cmd id for the chip. [0xc0, 0xef] */

/* diag system metrics report cmd [0xC0, 0xCf] */
#define DIAG_CMD_REPORT_BOOT_INFO              diag_cmd_id_comb(0xC0)
#define DIAG_CMD_REPORT_MASSDATA_FILE          diag_cmd_id_comb(0xC1)
#define DIAG_CMD_REPORT_MASSDATA               diag_cmd_id_comb(0xC2)

#endif /* SOC_DIAG_CMD_ID_H */

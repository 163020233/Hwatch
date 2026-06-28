/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Main boot for Second Stage Bootloader (SSB).
 * Author: HiSilicon
 * Create: 2023-09-01
 */

#include <stdbool.h>
#include "hal_reboot.h"
#include "pinctrl_porting.h"
#include "watchdog.h"
#include "watchdog_porting.h"
#include "tcxo.h"
#include "vectors.h"
#include "non_os.h"
#include "chip_io.h"
#include "securec.h"
#include "gpio.h"
#include "cpu_utils.h"
#include "partition.h"
#include "uart_porting.h"
#include "sfc_v2.h"
#include "arch_barrier.h"
#include "arch_encoding.h"
#include "product.h"
#include "soc_reg.h"
#include "secure_verify_boot.h"
#include "main.h"
#include "hal_fapc.h"
#include "upgrade.h"
#include "psram_porting.h"
#include "psram_v2.h"

#if defined(CONFIG_SUPPORT_SELITEOS_FLASH_ONLINE_DECRYPT)
#include "security_km.h"
#endif
#include "clocks_ccrg.h"

#define TCXO_REBOOT_DELAY             500ULL
#define STARTUP_FIRST_INSTRUCTION     0x40006f /* TODO */
#define SHARE_ROMBOOT_INFO __attribute__((section("romboot_info")))

#define TEE_ITCM_ORIGIN_ALIGNED             0x40000
#define TEE_ITCM_LENGTH_ALIGNED             0x40000
#define PMP_E_ONLY_M_RW                     0x9B
#define PMP_E_ONLY_M_RX                     0x9D
#define PMPCFG_MAX_SUPPORT                  8
#define HS_SUB_RESET_CFG                    3

#define SELITEOS_FAPC_REGION_NUM    7
#define FAPC_TEE_MID                (1 << 0)
#define FAPC_REE_MID                (1 << 1)

static const uint32_t g_pmpcfg_addr[PMPCFG_MAX_SUPPORT] = {
    PMPCFG0, PMPCFG1, PMPCFG2, PMPCFG3, PMPCFG4, PMPCFG5, PMPCFG6, PMPCFG7
};

SHARE_ROMBOOT_INFO bootup_ctrl g_bootup_ctrl;
typedef void (*RESET_HANDLER)(void);

static void boot_psram_init(void)
{
#define PSRAM_WINBOND_TAG                   0x5a6d
#define PSRAM_MANUFACTURER_TAG_REG          LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_GP_REG2_REG
#define PSRAM_TYPE_NUM                      2
    errcode_t retcode = ERRCODE_FAIL;
    for (uint8_t i = 0; i < PSRAM_TYPE_NUM; i++) {
        retcode = uapi_psram_init(PSRAM_ID_0);
        if (retcode == ERRCODE_SUCC) {
            if (psram_port_winbond_flag_get()) {
                writel(PSRAM_MANUFACTURER_TAG_REG, PSRAM_WINBOND_TAG);
            }
            return;
        }
        /* psram reset */
        reg16_clrbit(XIP_CACHE_CTL_RB_MEM_SOFT_RST_N_REG, XIP_CACHE_CTL_RB_CFG_XIPCACHE_SOFT_RST_OPI0_XIP_N_OFFSET);
        /* psram dereset */
        reg16_setbit(XIP_CACHE_CTL_RB_MEM_SOFT_RST_N_REG, XIP_CACHE_CTL_RB_CFG_XIPCACHE_SOFT_RST_OPI0_XIP_N_OFFSET);
        /* set winbond config flag */
        psram_port_winbond_flag_set(true);
    }
    if (retcode != ERRCODE_SUCC) {
        writel(PSRAM_MANUFACTURER_TAG_REG, 0x0);
        hal_reboot_chip();
        PRINT("psram init RET:%#x\r\n", retcode);
    }
}

static void boot_flash_init(void)
{
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO6_MODE_REG, HAL_PIO_FUNC_QSPI0);  /* qspi0_d0 */
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO7_MODE_REG, HAL_PIO_FUNC_QSPI0);  /* qspi0_d1 */
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO8_MODE_REG, HAL_PIO_FUNC_QSPI0);  /* qspi0_d2 QSPI mode */
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO9_MODE_REG, HAL_PIO_FUNC_QSPI0);  /* qspi0_d3 QSPI mode */
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO10_MODE_REG, HAL_PIO_FUNC_QSPI0); /* qspi0_clk */
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO11_MODE_REG, HAL_PIO_FUNC_QSPI0); /* qspi0_cs */
    uapi_pin_set_pull(S_EGPIO8, PIN_PULL_UP);
    uapi_pin_set_pull(S_EGPIO9, PIN_PULL_UP);

    sfc_flash_config_t sfc_cfg = {
        .read_type = FAST_READ_QUAD_IO,
        .write_type = QUAD_INPUT_PAGE_PROGRAM,
        .mapping_addr = FLASH_START,
        .mapping_size = FLASH_LENGTH,
    };
    isb();
    /* Need first flash power on */
    errcode_t ret = uapi_sfc_init(SFC_ID_0, &sfc_cfg);
    if (ret != ERRCODE_SUCC) {
        PRINT("flash err = 0x%x \r\n", ret);
    }

    isb();
}

static bool code_is_valid(const uint32_t *buff)
{
    /* g_RamExceptionTable data fit this feature */
    /* The APP start address does not locate the interrupt vector table for linx170. */
    /* Adaptation is required for this */
    if (buff[0] == STARTUP_FIRST_INSTRUCTION) {
        return true;
    }
    return false;
}

#define PMPADDR_ALIGN_BIT       2
static void pmp_set_napot_addr(uint8_t index, uint32_t baseaddr, uint32_t size)
{
    uint32_t pmpaddr_val = (uint32_t)(((baseaddr) >> PMPADDR_ALIGN_BIT) + ((size) >> 3) - 1);
    write_custom_csr_val((PMPADDR0 + index), pmpaddr_val);
}

static void pmp_config_rwx_permission(uint8_t pmp_index, uint8_t pmp_permission_value)
{
#define UINT32_CUT_MASK                     0xFFFFFFFF
#define GET_PMPCFG_OFFFSET(pmp_index)       ((uint8_t)((pmp_index) % 4) << 3)
#define GET_PMPCFG_NUM(pmpIdx)              ((uint8_t)(pmpIdx) >> 2)

   /* Only the following permissions are supported in riscv enhanced pmp:
    * user   R	 W	 X  kernel	R	W	X	  mode:L      R	  W	  X	   rule:
    * 0      #	 #	 #          #	#	#	       0      0	  0	  0	    NA
    * 1      #	 #	 √          #	#	#	       0      0	  0	  1	    u-mode-only
    * 2      √	 #	 #          √	√	#	       0      0	  1	  0	    shared data region
    * 3      √	 √	 #          √	√	#	       0      0	  1	  1	    shared data region
    * 4      √	 #	 #          #	#	#	       0      1	  0	  0	    u-mode-only
    * 5      √	 #	 √          #	#	#	       0      1	  0	  1	    u-mode-only
    * 6      √	 √	 #          #	#	#	       0      1	  1	  0	    u-mode-only
    * 7      √	 √	 √          #	#	#	       0      1	  1	  1	    u-mode-only
    *
    * 8      #	 #	 #          #	#	#		   1      0	  0	  0	    NA
    * 9      #	 #	 #          #	#	√		   1      0	  0	  1	    m-mode-only
    * 10     #	 #	 √          #	#	√		   1      0	  1	  0	    share code region
    * 11     #	 #	 √          √	#	√		   1      0	  1	  1	    share code region
    * 12     #	 #	 #          √	#	#		   1      1	  0	  0	    m-mode-only
    * 13     #	 #	 #          √	#	√		   1      1	  0	  1	    m-mode-only
    * 14     #	 #	 #          √	√	#		   1      1	  1	  0	    m-mode-only
    * 15     √	 #	 #          √	#	#		   1      1	  1	  1	    shared data region
    */
    uint8_t pmpcfg_index = GET_PMPCFG_NUM(pmp_index);
    uint32_t pmpcfg_val = read_custom_csr(g_pmpcfg_addr[pmpcfg_index]);
    pmpcfg_val &= (~((0xFF << GET_PMPCFG_OFFFSET(pmp_index)) & UINT32_CUT_MASK));
    pmpcfg_val |= pmp_permission_value << GET_PMPCFG_OFFFSET(pmp_index);
    write_custom_csr_val(g_pmpcfg_addr[pmpcfg_index], pmpcfg_val);
}

errcode_t ssb_verify_app_image(uint32_t key_addr)
{
    return secure_verify_image(OEM_MCU_BOOT_TYPE, key_addr, SSB_FLASH_REGION_START);
}

errcode_t ssb_verify_recovery_image(void)
{
    return secure_verify_image(OEM_FLASH_RECOVERY_TYPE, RECOVERY_FLASH_REGION_START, SSB_FLASH_REGION_START);
}

errcode_t ssb_verify_seliteos_image(void)
{
    return secure_verify_image(OEM_TEE_FIRMWARE_TYPE, TEE_ITCM_ORIGIN, SSB_FLASH_REGION_START);
}

errcode_t ssb_verify_partition_image(void)
{
    return secure_verify_image(OEM_PARAMS_BOOT_TYPE, PARTITION_TABLE_REGION_START, SSB_FLASH_REGION_START);
}

#if defined(CONFIG_SUPPORT_SELITEOS_FLASH_ONLINE_DECRYPT)
#define ENC_FLAG_OFFSET     0x148
#define ENC_FLAG_FLASH_ADDR (SELITEOS_FLASH_REGION_START + ENC_FLAG_OFFSET)
#define NON_ENC_FLAG        0x3C7896E1
#define SALT_LEN            0x1C
#define RESERVED_LEN        0x4
#define KEY_IV_LEN          0x10

typedef struct {
    uint32_t code_enc_flag;    /* 0x3C7896E1: code area not enc, others: enc */
    uint8_t salt[SALT_LEN];
    uint8_t reserved[RESERVED_LEN];
    uint8_t iv[KEY_IV_LEN];
} fapc_image_info_t;

static void tee_fapc_send_key(uint8_t *salt, uint32_t salt_len)
{
    uint32_t keyslot_handle;
    errcode_t ret;

    ret = uapi_drv_keyslot_setup(&keyslot_handle, UAPI_DRV_KEYSLOT_TYPE_FLASH);
    if (ret != ERRCODE_SUCC) {
        PRINT("keyslot_setup failed, ret = 0x%x\r\n", ret);
        return;
    }

    ret = uapi_drv_keyslot_set_hard_key(keyslot_handle, UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_ODRK_TEE,
        UAPI_DRV_KLAD_ENGINE_AES, salt, salt_len, KEY_IV_LEN);
    if (ret != ERRCODE_SUCC) {
        PRINT("keyslot_set_hard_key failed, ret = 0x%x\r\n", ret);
    }

    (void)uapi_drv_keyslot_teardown(keyslot_handle);
}

void tee_flash_online_decryption_config(uint32_t start_addr, uint32_t end_addr)
{
    uint32_t region = SELITEOS_FAPC_REGION_NUM;
    uint32_t read_mid = FAPC_TEE_MID;
    uint32_t write_mid = 0;
    fapc_image_info_t image_info;
    uint8_t *src = (uint8_t *)(uintptr_t)ENC_FLAG_FLASH_ADDR;

    (void)memcpy_s(&image_info, sizeof(fapc_image_info_t), src, sizeof(fapc_image_info_t));

    if (image_info.code_enc_flag != NON_ENC_FLAG) {
        /* send decryption key */
        tee_fapc_send_key(image_info.salt, sizeof(image_info.salt));

        /* set fapc to enable flash online decryption */
        hal_fapc_set_region_addr(region, start_addr, end_addr);
        hal_fapc_set_region_permission(region, read_mid, write_mid);
        hal_fapc_set_region_mode(region, CRYPTO_FAPC_REGION_MODE_DEC);
        hal_fapc_set_region_iv(CRYPTO_FAPC_IV_TYPE_TEE, image_info.iv, sizeof(image_info.iv));
        hal_fapc_set_region_iv_start_addr(region, start_addr);
        hal_fapc_region_enable(region, 1);
        hal_fapc_region_lock(region);
        PRINT("seliteos flash online decryption enable!\n");
    }
}
#endif

static void ssb_cp_app_image(void)
{
    uint32_t image_start_addr = 0;
    partition_information_t image_info = { 0 };
    errcode_t ret = uapi_partition_get_info(PARTITION_ACPU_IMAGE, &image_info);
    if (ret != EOK) {
        PRINT("image=%x get addr=%x len=%x error ret=%d\n", PARTITION_ACPU_IMAGE,
              image_info.part_info.addr_info.addr, image_info.part_info.addr_info.size, ret);
        return;
    }
    image_start_addr = image_info.part_info.addr_info.addr + FLASH_START;
    ret = ssb_verify_app_image(image_start_addr);
    if (ret != ERRCODE_SUCC) {
        PRINT("app image=%x verify failed, ret = 0x%x\r\n", image_start_addr, ret);
        return;
    }

    // config flash as rx
    pmp_config_rwx_permission(PMP_INDEX_SFC_CODE_ADDR, PMP_E_ONLY_M_RX);

    // config itcm fisrt 2k as rw
    pmp_set_napot_addr(PMP_INDEX_ITCM_STARTUP_ADDR, APP_ITCM_ORIGIN, APP_VECTORS_LENGTH);
    pmp_config_rwx_permission(PMP_INDEX_ITCM_STARTUP_ADDR, PMP_E_ONLY_M_RW);

    image_start_addr += FLASH_SIGN_HEADER;
    errno_t sec_ret = memcpy_s((void *)(uintptr_t)APP_VECTORS_ORIGIN, APP_VECTORS_LENGTH,
                               (void *)(uintptr_t)image_start_addr, APP_VECTORS_LENGTH);
    if (sec_ret != EOK) {
        PRINT("Memory process is error, jump_addr 0x%x, sec_ret %d", image_start_addr, sec_ret);
        return;
    }

    // config itcm fisrt 2k as rx
    pmp_config_rwx_permission(PMP_INDEX_ITCM_STARTUP_ADDR, PMP_E_ONLY_M_RX);
}

static void ssb_enter_seliteos(void)
{
    uintptr_t seliteos_startup_addr = TEE_ITCM_ORIGIN + FLASH_SIGN_HEADER;
    PRINT("ssb enter seliteos...\n");

#if defined(CONFIG_SUPPORT_SELITEOS_FLASH_ONLINE_DECRYPT)
    tee_flash_online_decryption_config(SELITEOS_PROGRAM_ORIGIN,
                                       (SELITEOS_FLASH_REGION_START + SELITEOS_FLASH_REGION_LENGTH));
#endif

    // config flash as rw
    pmp_set_napot_addr(PMP_INDEX_SFC_CODE_ADDR, FLASH_START, FLASH_LENGTH);
    pmp_config_rwx_permission(PMP_INDEX_SFC_CODE_ADDR, PMP_E_ONLY_M_RW);

    // config seliteos itcm as rw
    // pmp start_addr must be aligned with size, set tee itcm range [0x40000, 0x80000), include [0x50000, 0x70000).
    pmp_set_napot_addr(PMP_INDEX_ITCM_SELITEOS_ADDR, TEE_ITCM_ORIGIN_ALIGNED, TEE_ITCM_LENGTH_ALIGNED);
    pmp_config_rwx_permission(PMP_INDEX_ITCM_SELITEOS_ADDR, PMP_E_ONLY_M_RW);

    errno_t sec_ret = memcpy_s((void *)(uintptr_t)TEE_ITCM_ORIGIN, TEE_ITCM_LENGTH,
                               (void *)(uintptr_t)SELITEOS_FLASH_REGION_START, TEE_ITCM_LENGTH);
    if (sec_ret != EOK) {
        PRINT("memcpy_s error, SELITEOS_FLASH_REGION_START = 0x%x, sec_ret = %d", SELITEOS_FLASH_REGION_START, sec_ret);
        return;
    }

    errcode_t ret = ssb_verify_seliteos_image();
    if (ret != ERRCODE_SUCC) {
        PRINT("seliteos image verify failed, ret = 0x%x\r\n", ret);
        return;
    }

    // config seliteos itcm as rx
    pmp_config_rwx_permission(PMP_INDEX_ITCM_SELITEOS_ADDR, PMP_E_ONLY_M_RX);

    // config seliteos dtcm as rw
    pmp_set_napot_addr(PMP_INDEX_DTCM_TEE_ADDR, TEE_DTCM_ORIGIN, TEE_DTCM_LENGTH);
    pmp_config_rwx_permission(PMP_INDEX_DTCM_TEE_ADDR, PMP_E_ONLY_M_RW);

    ssb_cp_app_image();

    if (code_is_valid((uint32_t *)seliteos_startup_addr)) {
        ((RESET_HANDLER)(seliteos_startup_addr))();
        return;
    }
    PRINT("faild addr:0x%x, val:0x%x\r\n", (uint32_t)(uintptr_t)seliteos_startup_addr, readl(seliteos_startup_addr));
    watchdog_turnoff_clk();
    while (1) {
        ;
    }
}

static void ssb_enter_recovery(void)
{
    errcode_t ret = ssb_verify_recovery_image();
    if (ret != ERRCODE_SUCC) {
        PRINT("recovery image verify failed, ret = 0x%x\r\n", ret);
        return;
    }

    pmp_set_napot_addr(PMP_INDEX_ITCM_STARTUP_ADDR, APP_ITCM_ORIGIN, APP_VECTORS_LENGTH);
    pmp_config_rwx_permission(PMP_INDEX_ITCM_STARTUP_ADDR, PMP_E_ONLY_M_RW);

    pmp_set_napot_addr(PMP_INDEX_DTCM_TEE_ADDR, TEE_DTCM_ORIGIN, UPG_TEE_DTCM_LENGTH);
    pmp_config_rwx_permission(PMP_INDEX_DTCM_TEE_ADDR, PMP_E_ONLY_M_RW);

    errno_t sec_ret = memcpy_s((void *)(uintptr_t)APP_VECTORS_ORIGIN, APP_VECTORS_LENGTH,
                               (void *)(uintptr_t)RECOVERY_VECTORS_LOAD_ORIGIN, APP_VECTORS_LENGTH);
    if (sec_ret != EOK) {
        PRINT("Memory process is error, jump_addr 0x%x, sec_ret %d", RECOVERY_VECTORS_LOAD_ORIGIN, sec_ret);
        return;
    }

    pmp_config_rwx_permission(PMP_INDEX_ITCM_STARTUP_ADDR, PMP_E_ONLY_M_RX);
    PRINT("jump_to_cimage recovery, addr:0x%x, value is 0x%x\r\n", APP_VECTORS_ORIGIN, readl(APP_VECTORS_ORIGIN));

    if (code_is_valid((uint32_t *)APP_VECTORS_ORIGIN)) {
        upg_set_efuse_protect(true);
        ((RESET_HANDLER)(APP_VECTORS_ORIGIN))();
    } else {
        watchdog_turnoff_clk();
        while (1) {
        }
    }
}

static void ssb_enter_app(void)
{
    core_images_e cimage = ssb_get_jump_image();
    if (cimage == CORE_IMAGES_RECOVERY) {
        ssb_enter_recovery();
    } else {
        ssb_enter_seliteos();
    }
}

void watchdog_init(void)
{
    watchdog_turnon_clk();
#ifdef NO_TIMEOUT
    uapi_watchdog_disable();
    watchdog_turnoff_clk();
#endif
}

static uint32_t g_flashboot_run_sys_tick = 0;
static void check_romboot_info(void)
{
    PRINT("reboot cnt:%d, flashboot cnt:%d"NEWLINE, boot_get_reboot_cnt(), boot_get_flashboot_up_cnt());
    PRINT("flash id:0x%x, flash backup:0x%x"NEWLINE, g_bootup_ctrl.flash_id, boot_get_flashboot_backup_flag());
    PRINT("nand boot config:%d, cost:%d ms"NEWLINE, g_bootup_ctrl.efuse.nand_bootup_flag,
        (uint32_t)((g_flashboot_run_sys_tick -  g_bootup_ctrl.start_systicks) >> 0x5));
    PRINT("uart wait timeout:%dms"NEWLINE, g_bootup_ctrl.uart_wait_timeout);
    PRINT("chip lifecycle:0x%x, sec boot:0x%x"NEWLINE, g_bootup_ctrl.chip_lifecycle, g_bootup_ctrl.secure_boot_enble);
    PRINT("start flashboot %s"NEWLINE, (boot_get_flashboot_backup_flag() == BOOT_BACKFLASH) ? "backup" : "");
    boot_set_flashboot_up_cnt(0);
    boot_set_reboot_cnt(0);
}

static void pmu_init(void)
{
    reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_72_REG, LP_CTL_GLB_RB_D_PMU_SW1P8_0_EN_OFFSET);
    uapi_tcxo_delay_ms(10UL);

    // nandflash and psram 1p8 power on
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_72_REG, LP_CTL_GLB_RB_D_PMU_SW1P8_0_EN_OFFSET);
    uapi_tcxo_delay_ms(10UL);
}

static void xo32m_power_off(void)
{
    // 先切RC 32M
    reg16_setbit(ULP_AON_FUSE_CTL_RB_FUSE_CTL_AUTO_RD_38_REG, ULP_AON_FUSE_CTL_RB_CFG_XO_RC_SEL_MAN_OFFSET);
    reg16_setbit(ULP_AON_FUSE_CTL_RB_FUSE_CTL_AUTO_RD_34_REG, ULP_AON_FUSE_CTL_RB_CFG_XO_RC_SEL_MAN_SEL_OFFSET);
    reg16_setbit(GLB_CTL_M_RB_CLK_32K_32M_EN_SEL_REG, GLB_CTL_M_RB_CFG_GLBM_XO_RC_SEL_OFFSET);

    // 下电xo32 m
    reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_3_REG, LP_CTL_GLB_RB_TCXO_CLK_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_SEL_3_REG, LP_CTL_GLB_RB_TCXO_CLK_EN_MAN_SEL_OFFSET);
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_REG, LP_CTL_GLB_RB_DCXO_XO_CKO_ISO_MAN_OFFSET);
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_SEL_REG, LP_CTL_GLB_RB_DCXO_XO_CKO_ISO_MAN_SEL_OFFSET);
    reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_REG, LP_CTL_GLB_RB_DCXO_XOCLKOUT3_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_SEL_REG, LP_CTL_GLB_RB_DCXO_XOCLKOUT3_EN_MAN_SEL_OFFSET);
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_REG, LP_CTL_GLB_RB_DCXO_XO_CORE_PD_MAN_OFFSET);
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_SEL_REG, LP_CTL_GLB_RB_DCXO_XO_CORE_PD_MAN_SEL_OFFSET);
}

static void xo32m_start_gear_config(uint32_t time)
{
    // 32k cnt
#define XO32M_LEVEL1_TIME_THRESHOLD 5
#define XO32M_LEVEL2_TIME_THRESHOLD 32
    if (time <= XO32M_LEVEL1_TIME_THRESHOLD) {
        writew(LP_CTL_BT_RB_CFG_LP_CTL_BT_GP_REG4_REG, 0x1);
    } else if (time < XO32M_LEVEL2_TIME_THRESHOLD) {
        writew(LP_CTL_BT_RB_CFG_LP_CTL_BT_GP_REG4_REG, 0x2);
    } else {
        writew(LP_CTL_BT_RB_CFG_LP_CTL_BT_GP_REG4_REG, 0x3);
    }
    return;
}

static void xo32m_gear_detect(void)
{
#define XO_32M_DET_THRESHOLD 10
    // 上电XO32M
    reg16_setbit(PMU2_CMU_CTL_RB_FAST_XO_INIT_CNT_CLKIN_CLR_REG, PMU2_CMU_CTL_RB_FAST_XO_INIT_CNT_CLR_OFFSET);
    reg16_setbit(PMU2_CMU_CTL_RB_FAST_XO_INIT_CNT_CLKIN_CLR_REG, PMU2_CMU_CTL_RB_FAST_XO_INIT_SUCCESS_CLR_OFFSET);
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_DCXO_REG, 0x0);
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_DCXO_1_REG, 0x0);
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_DCXO_2_REG, 0x0);
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_DCXO_3_REG, 0xF038);

    reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_REG, LP_CTL_GLB_RB_DCXO_XO_CORE_PD_MAN_OFFSET);
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_SEL_REG, LP_CTL_GLB_RB_DCXO_XO_CORE_PD_MAN_SEL_OFFSET);
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_REG, LP_CTL_GLB_RB_DCXO_XOCLKOUT3_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_SEL_REG, LP_CTL_GLB_RB_DCXO_XOCLKOUT3_EN_MAN_SEL_OFFSET);
    reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_REG, LP_CTL_GLB_RB_DCXO_XO_CKO_ISO_MAN_OFFSET);
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_SEL_REG, LP_CTL_GLB_RB_DCXO_XO_CKO_ISO_MAN_SEL_OFFSET);
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_3_REG, LP_CTL_GLB_RB_TCXO_CLK_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_SEL_3_REG, LP_CTL_GLB_RB_TCXO_CLK_EN_MAN_SEL_OFFSET);
    // 回读XO START STOP信号
    uint64_t detct_start = uapi_tcxo_get_ms();
    while (1) {
        if (reg16_getbit(PMU2_CMU_CTL_RB_PMU2_GP_REG0_REG, 0xF) == 1 &&
            reg16_getbit(PMU2_CMU_CTL_RB_PMU2_GP_REG0_REG, 0xE) == 1) {
            uapi_tcxo_delay_us(65UL);
            uint32_t fast_xo_cnt = readw(PMU2_CMU_CTL_RB_FAST_XO_INIT_CNT_CLKIN_L_REG);
            uint32_t fast_xo_cnt_hl = reg16_getbits(PMU2_CMU_CTL_RB_FAST_XO_INIT_CNT_CLKIN_H_REG,
                PMU2_CMU_CTL_RB_FAST_XO_INIT_CNT_H_OFFSET, PMU2_CMU_CTL_RB_FAST_XO_INIT_CNT_H_LEN);
            fast_xo_cnt = fast_xo_cnt | (fast_xo_cnt_hl << PMU2_CMU_CTL_RB_FAST_XO_INIT_CNT_L_LEN);
            // cnt 32k
            PRINT("xo 32m gear det:0x%x.\r\n", fast_xo_cnt);
            xo32m_start_gear_config(fast_xo_cnt);
            break;
        }
        if (uapi_tcxo_get_ms() > (detct_start + XO_32M_DET_THRESHOLD)) {
            PRINT("xo 32m gear det timeout.\r\n");
            writew(LP_CTL_BT_RB_CFG_LP_CTL_BT_GP_REG4_REG, 0x3);
            break;
        }
    }
    reg16_clrbit(GLB_CTL_M_RB_CLK_32K_32M_EN_SEL_REG, GLB_CTL_M_RB_CFG_GLBM_XO_RC_SEL_OFFSET);
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_DCXO_3_REG, 0); // 恢复，功耗优化
    reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_SEL_REG, LP_CTL_GLB_RB_DCXO_XO_CORE_PD_MAN_SEL_OFFSET);
    reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_SEL_REG, LP_CTL_GLB_RB_DCXO_XOCLKOUT3_EN_MAN_SEL_OFFSET);
    reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_SEL_REG, LP_CTL_GLB_RB_DCXO_XO_CKO_ISO_MAN_SEL_OFFSET);
    reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_SEL_3_REG, LP_CTL_GLB_RB_TCXO_CLK_EN_MAN_SEL_OFFSET);
}

static void clock_init(void)
{
#define XO_32K_DET_TIME_MAX 10
#define XO_32K_DET_PRE_THRESHOLD 10
#if CHIP_ASIC
    // 启动电压0.9v 默认不调压
    clocks_ccrg_init();
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_USB_BUS, CLOCKS_SRC_MAX, 0x1);  // close usbc
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_MCU_CORE, CLOCKS_SRC_XO, 0x1);  // close fnpll, change fnpll freq

    xo32m_power_off();
    // xo 32k power on
    reg16_setbit(LP_CTL_ULP_A_RB_XO_32K_CFG_REG_REG, LP_CTL_ULP_A_RB_D_PMU_XO_32K_LP_EN_OFFSET);
    uapi_tcxo_delay_ms(120UL);
    xo32m_gear_detect();

    // enable xo 32k det
    reg16_setbits(GLB_CTL_M_RB_32K_DET_CFG_REG, GLB_CTL_M_RB_XO32K_DET_SEL_OFFSET, GLB_CTL_M_RB_XO32K_DET_SEL_LEN, 0xf);
    reg16_setbit(GLB_CTL_M_RB_32K_DET_CFG_REG, GLB_CTL_M_RB_XO32K_DET_EN_OFFSET);
    uapi_tcxo_delay_ms(2UL);
    uint64_t detct_start = uapi_tcxo_get_ms();
    uint32_t xo_times = 0;
    while (xo_times < XO_32K_DET_PRE_THRESHOLD) {
        xo_times += reg16_getbit(GLB_CTL_M_RB_32K_DET_CFG_REG, GLB_CTL_M_RB_XO32K_DET_STS_ORG_OFFSET);
        if (uapi_tcxo_get_ms() > (detct_start + XO_32K_DET_TIME_MAX)) {
            break;
        }
    }
    if (xo_times >= XO_32K_DET_PRE_THRESHOLD) {
        PRINT("xo 32k, det:%x" NEWLINE, readw(GLB_CTL_M_RB_32K_DET_CFG_REG));
        // switch to xo 32k
        reg16_setbit(LP_CTL_ULP_A_RB_RC_XO_CLK_MUX_REG, LP_CTL_ULP_A_RB_XO_32K_EN_OFFSET);
        reg16_setbit(LP_CTL_ULP_A_RB_RC_XO_CLK_MUX_REG, LP_CTL_ULP_A_RB_RC_XO_SEL_OFFSET);
    } else {
        PRINT("rc 32k,det:%x,times:%x" NEWLINE, readw(GLB_CTL_M_RB_32K_DET_CFG_REG), xo_times);
        // xo 32k power off
        reg16_clrbit(LP_CTL_ULP_A_RB_XO_32K_CFG_REG_REG, LP_CTL_ULP_A_RB_D_PMU_XO_32K_LP_EN_OFFSET);
    }
    // disable xo 32k det
    reg16_clrbit(GLB_CTL_M_RB_32K_DET_CFG_REG, GLB_CTL_M_RB_XO32K_DET_EN_OFFSET);

    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_MCU_CORE, CLOCKS_SRC_PLL, 0x3); // 266MHz
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_XIP_OPI0, CLOCKS_SRC_PLL, 0x2); // 200MHz
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_XIP_SFC, CLOCKS_SRC_PLL, 0x4);  // 100MHz
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_EMMC, CLOCKS_SRC_PLL, 0x6);     // FMC IO 67MHz
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_SCR_SEC, CLOCKS_SRC_PLL, 0x4);  // 200MHz
#endif
}

static uint32_t systick_get_count(void)
{
    uint32_t val = 0;
    reg16_setbit(GLB_CTL_M_RB_SYS_TICK_CFG_M_REG, GLB_CTL_M_RB_SYS_TICK_FRZ_M_OFFSET);
    val = (uint32_t)(readw(GLB_CTL_M_RB_SYS_TICK_VALUE_M_1_REG)) << 16U;
    val |= (uint32_t)readw(GLB_CTL_M_RB_SYS_TICK_VALUE_M_0_REG);
    reg16_clrbit(GLB_CTL_M_RB_SYS_TICK_CFG_M_REG, GLB_CTL_M_RB_SYS_TICK_FRZ_M_OFFSET);
    return val;
}

typedef struct {
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t read_per;
    uint32_t write_per;
} fapc_config_t;

static fapc_config_t g_fapc_config[] = {
#ifdef JTAG_DEBUG_MODE
    {
        FLASH_START,
        FLASH_START + FLASH_LENGTH,
        FAPC_TEE_MID | FAPC_REE_MID,
        FAPC_TEE_MID | FAPC_REE_MID,
    },
#else
    {
        FLASH_START,
        FLASH_START + FLASH_LENGTH,
        FAPC_TEE_MID,
        FAPC_TEE_MID,
    },
#endif
    {
        FLASH_START,
        SSB_FLASH_REGION_START,
        FAPC_TEE_MID | FAPC_REE_MID,
        FAPC_TEE_MID | FAPC_REE_MID
    },
    {
        RECOVERY_FLASH_REGION_START,
        (FLASH_START + FLASH_LENGTH),
        FAPC_TEE_MID | FAPC_REE_MID,
        FAPC_TEE_MID | FAPC_REE_MID,
    },
    {
        SELITEOS_FLASH_REGION_START,
        (SELITEOS_FLASH_REGION_START + SELITEOS_FLASH_REGION_LENGTH),
        FAPC_TEE_MID,
        FAPC_TEE_MID,
    },
};

static void tee_fapc_init(void)
{
    for (uint32_t i = 0 ; i < sizeof(g_fapc_config) / sizeof(fapc_config_t); i++) {
        hal_fapc_set_region_addr(i, g_fapc_config[i].start_addr, g_fapc_config[i].end_addr);
        hal_fapc_set_region_permission(i, g_fapc_config[i].read_per, g_fapc_config[i].write_per);
        hal_fapc_set_region_mode(i, CRYPTO_FAPC_REGION_MODE_NORMAL);
        hal_fapc_region_enable(i, 1);
        hal_fapc_region_lock(i);
    }

    flash_spi_info_t *flash_info = sfc_get_flash_info(g_bootup_ctrl.flash_id);

    hal_fapc_set_cmd_lut0(0, FAPC_TEE_MID, flash_info->erase_cmds[0].cmd); // REE cannot do CHIP_ERASE operation
    hal_fapc_set_cmd_lut0_enable(0, true);

    uint32_t cmd_lut1_index = 0;
    for (uint32_t i = 1; i < flash_info->erase_cmd_num; i++) {
        hal_fapc_set_cmd_lut1(cmd_lut1_index, flash_info->erase_cmds[i].size - 1, flash_info->erase_cmds[i].cmd, true);
        hal_fapc_set_cmd_lut1_enable(cmd_lut1_index, true);
        cmd_lut1_index++;
    }
    hal_fapc_set_cmd_lut_lock();
}

static void jtag_norflash_erase_proc(void)
{
#define NORFLASH_ERASE_SATRT        0x3C
#define NORFLASH_ERASE_DONE         0x5A3C
#define NORFLASH_ERASE_WAIT_ADDR    LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_GP_REG2_REG

    /* 用于 JLINK 脚本擦除 norflash */
    if (readl(NORFLASH_ERASE_WAIT_ADDR) == NORFLASH_ERASE_SATRT) {
        while (readl(NORFLASH_ERASE_WAIT_ADDR) != NORFLASH_ERASE_DONE) {
            ;
        }
    }
}

int main(void)
{
    jtag_norflash_erase_proc();
    int_setup();
    uapi_tcxo_init();
    uapi_systick_init();
    g_flashboot_run_sys_tick = systick_get_count();
    // hs_sub解复位
    writew(GLB_CTL_M_RB_SOFT_RST_6_REG, HS_SUB_RESET_CFG);
    uapi_pin_init();

    if (g_bootup_ctrl.chip_lifecycle != 0) {
#ifdef JTAG_DEBUG_MODE
        reg16_setbit(TEE_CTL_RB_CFG_DEBUG_INTF_EN_REG, TEE_CTL_RB_SWD_EN_REG_OFFSET);
        reg16_setbit(TEE_CTL_RB_CFG_DEBUG_INTF_EN_REG, TEE_CTL_RB_SWD_EN_REG_LOCK_OFFSET);
#endif
    }

    // clear usb dfu software trigger flag
    if (readw(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_15_REG) == 0x5a5b) {
        writew(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_15_REG, 0x0);
    }

    reg16_setbit(LP_CTL_ULP_B_RB_HRESET_MODE_CTRL_REG, LP_CTL_ULP_B_RB_ULP_HRESET_MODE_SEL_OFFSET); // 默认复位选择热复位

    watchdog_init();
#ifdef SW_UART_DEBUG
    sw_debug_uart_init(UART_L_BAUDRATE);
#endif
    check_romboot_info();
    boot_set_flashboot_backup_flag(0);

    tee_fapc_init();

    boot_flash_init();
    pmu_init();
    clock_init();

    uapi_efuse_init();
    uapi_drv_cipher_env_init();
    secure_verify_init();
    boot_psram_init();
    errcode_t ret = ssb_verify_partition_image();
    if (ret != ERRCODE_SUCC) {
        PRINT("partition image verify failed, ret = 0x%x\r\n", ret);
        uapi_tcxo_delay_ms((uint64_t)TCXO_REBOOT_DELAY);
        hal_reboot_chip();
    }
    ret = uapi_partition_init();
    if (ret != ERRCODE_SUCC) {
        PRINT("partition init error, error code = 0x%x\r\n", ret);
    }

#ifdef TEE_IP_TEST
    tee_test_timer();
    tee_test_spi();
#endif
    boot_upg_handle();
    ssb_enter_app();

    PRINT("No jump, reset now.\r\n");
    uapi_tcxo_delay_ms((uint64_t)TCXO_REBOOT_DELAY);
    cpu_utils_set_system_status_by_cause(REBOOT_CAUSE_BT_SSB_EXCEPTION_REBOOT);
    /* This will reset the aliased regions, you will go to ROM */
    hal_reboot_chip();
    return 0;
}

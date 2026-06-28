/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: source file for module operation
 */
#include <stdlib.h>
#ifdef __LITEOS__
#include <los_task.h>
#endif
#include "cmsis_os.h"

#include "thread_os.h"
#include "securec.h"
#include "sample_audio_api.h"

#include "chip_type.h"
#include "audio_struct.h"
#include "soc_uapi_audio_sys.h"
#include "sample_audio.h"

#if (SAP_CHIP_TYPE == brandy)
#include "es8311_codec.h"
#include "pm_clocks.h"
#endif /* SAP_CHIP_TYPE */

#define TEST_SUITE_TASK_STACK_SIZE 0xc00
#define TEST_SUITE_NAME "audio"
#define TEST_SUITE_MAX_ARGV_LEN 128
#ifdef BOARD_CS
#define AUDIO_MEM_PROT_CFG   0x500006a0
#define AUDIO_MEM_PROT_LOCK  0x3
#endif

#define ADDRESS_OFFSET 4

#define HIFI_FREQ_176M 176947200
#define HIFI_FREQ_147M 147456000
#define HIFI_FREQ_64M 64000000

#define HIFI_INVALID_PROFILE 0xF

static td_u8 g_dsp_profile = HIFI_INVALID_PROFILE;

typedef struct {
    td_s32 argc;
    td_char argv[TEST_SUITE_MAX_ARGC][TEST_SUITE_MAX_ARGV_LEN];
} cmd_msg_queue;

typedef struct {
    td_char *name;
    td_s32 (*func)(td_s32 argc, td_char *argv[]);
    td_bool running;
} functions;

typedef struct {
    td_char *std_data_name;
    td_u8 std_data_size;
    td_char *td_data_name;
    td_u8 td_data_size;
} data_type_func;

typedef struct {
    athread_handle task;
    td_bool task_active;
} sample_audio_inst;

/* only one sample instance when set to true */
static td_bool g_sample_cfg_singleton = TD_TRUE;

typedef struct {
    td_char *sample;
    td_char *config;
    td_char *value;
} sample_set_config_arg;

typedef struct {
    td_char *sample;
    td_char *addr;
    td_char *value;
} sample_audio_wr_arg;

typedef struct {
    td_char *sample;
    td_char *handle;
    td_char *port;
    td_char *ch_id;
    td_char *type;
    td_char *integer;
    td_char *decimal;
} sample_set_vol_arg;

static td_void sample_set_dump_mask(td_u8 dump_mask)
{
    td_s32 ret;
    uapi_audio_debug_cfg cfg = {0};

    ret = uapi_audio_get_debug_cfg(&cfg);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_audio_get_debug_cfg, ret);
        return;
    }

    cfg.dump_mask = dump_mask;
    ret = uapi_audio_set_debug_cfg(&cfg);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_audio_set_debug_cfg, ret);
        return;
    }

    sap_alert_log_info("dump mask status");
    sap_alert_log_bool((dump_mask & UAPI_AUDIO_DUMP_MASK_AI) != 0);
    sap_alert_log_bool((dump_mask & UAPI_AUDIO_DUMP_MASK_SEA) != 0);
    sap_alert_log_bool((dump_mask & UAPI_AUDIO_DUMP_MASK_AENC) != 0);
    sap_alert_log_bool((dump_mask & UAPI_AUDIO_DUMP_MASK_ADEC) != 0);
    sap_alert_log_bool((dump_mask & UAPI_AUDIO_DUMP_MASK_TRACK) != 0);
    sap_alert_log_bool((dump_mask & UAPI_AUDIO_DUMP_MASK_AO) != 0);
}

static td_void sample_set_proc_mask(td_u8 proc_mask)
{
    td_s32 ret;
    uapi_audio_debug_cfg cfg = {0};

    ret = uapi_audio_get_debug_cfg(&cfg);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_audio_get_debug_cfg, ret);
        return;
    }

    cfg.proc_mask = proc_mask;
    ret = uapi_audio_set_debug_cfg(&cfg);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_audio_set_debug_cfg, ret);
        return;
    }

    sap_alert_log_info("proc mask status");
    sap_alert_log_bool((proc_mask & UAPI_AUDIO_PROC_MASK_UART) != 0);
    sap_alert_log_bool((proc_mask & UAPI_AUDIO_PROC_MASK_DIAG_SHELL) != 0);
}

#if defined(SAP_TWS_PRODUCT) || defined(SAP_SOUND_BOX_PRODUCT)
enum {
    TWS_ROLE,
    TWS_FORM,
    TWS_MONO,
    TWS_MAX,
};

typedef struct {
    char *name;
    td_u32 type;
    td_u32 value;
} tws_config;

static tws_config g_tws_config[] = {
    {"master", TWS_ROLE, AUDIO_TWS_ROLE_MASTER},
    {"slave", TWS_ROLE, AUDIO_TWS_ROLE_SLAVE},
    {"freeman", TWS_ROLE, AUDIO_TWS_ROLE_FREEMAN},

    {"left", TWS_FORM, UAPI_AUDIO_PRODUCT_FORM_TWS_LEFT},
    {"right", TWS_FORM, UAPI_AUDIO_PRODUCT_FORM_TWS_RIGHT},

    {"mono", TWS_MONO, 1},
    {"stereo", TWS_MONO, 0},
};

static td_void sample_set_tws_config(td_char *config)
{
    td_u32 i;
    td_u32 type = TWS_MAX;
    td_u32 val;

    for (i = 0; i < sizeof(g_tws_config) / sizeof(g_tws_config[0]); i++) {
        if ((strcmp(g_tws_config[i].name, config) == 0)) {
            type = g_tws_config[i].type;
            val = g_tws_config[i].value;
            break;
        }
    }

    switch (type) {
        case TWS_ROLE :
            uapi_audio_set_tws_role((uapi_audio_tws_role)val);
            break;
        case TWS_FORM :
            uapi_audio_set_product_form((uapi_audio_product_form)val);
            break;
        case TWS_MONO :
            uapi_audio_set_tws_mono_mode((td_bool)val);
            break;
        default :
            sap_alert_log_info("invalid tws config\n");
            return;
    }
}
#else
static td_void sample_set_tws_config(td_char *config)
{
    audio_unused(config);
}
#endif

static td_void sample_set_dsp_profile(td_u8 profile)
{
    g_dsp_profile = profile;
    sap_alert_log_u32(g_dsp_profile);
}

static td_void sample_audio_init(td_u8 arg)
{
    td_s32 ret;

    if (arg == TD_TRUE) {
        ret = uapi_audio_init();
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_audio_init, ret);
        }

        sap_alert_log_info("call uapi_audio_init\n");
    } else if (arg == TD_FALSE) {
        ret = uapi_audio_deinit();
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_audio_deinit, ret);
        }

        sap_alert_log_info("call uapi_audio_deinit\n");
    } else {
        sap_alert_log_info("nothing changed!");
    }
}

#ifndef MONO_PROFILE_CUT
static td_void sample_audio_set_global_mute(td_u8 arg)
{
    td_s32 ret;
    td_bool mute = !!arg;

    ret = uapi_audio_set_global_mute(mute);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_audio_set_global_mute, ret);
    }
}
#endif

static td_s32 sample_set_config(td_s32 argc, td_char *argv[])
{
    td_u8 tmp_mask;
    sample_set_config_arg *arg = (sample_set_config_arg *)argv;

    if (argc == 0x2 && (strcmp("q", argv[1]) == 0)) {
        return EXT_SUCCESS;
    }

    if (argc != (td_s32)sizeof(sample_set_config_arg) / (td_s32)sizeof(td_char *)) {
        sap_err_log_info("nothing changed!");
        return EXT_SUCCESS;
    }

    tmp_mask = (td_u8)strtoul(arg->value, TD_NULL, 0);
    if (strcmp(arg->config, "singleton") == 0) {
        g_sample_cfg_singleton = ((strtoul(arg->value, TD_NULL, 0)) == 0) ? TD_FALSE : TD_TRUE;
        sap_alert_log_u32(g_sample_cfg_singleton);
    } else if (strcmp(arg->config, "dump_mask") == 0) {
        sample_set_dump_mask(tmp_mask);
    } else if (strcmp(arg->config, "proc_mask") == 0) {
        sample_set_proc_mask(tmp_mask);
    } else if (strcmp(arg->config, "tws") == 0) {
        sample_set_tws_config(arg->value);
    } else if (strcmp(arg->config, "profile") == 0) {
        sample_set_dsp_profile(tmp_mask);
    } else if (strcmp(arg->config, "audio_init") == 0) {
        sample_audio_init(tmp_mask);
#ifndef MONO_PROFILE_CUT
    } else if (strcmp(arg->config, "global_mute") == 0) {
        sample_audio_set_global_mute(tmp_mask);
#endif
    } else {
        sap_err_log_info("nothing changed!");
    }

    return EXT_SUCCESS;
}

#ifdef SAP_REG_DFX_SAMPLE_SUPPORT
td_s32 sample_audio_w4(td_s32 argc, td_char *argv[])
{
    td_u32 val;
    td_u32 addr;
    td_u32 read_val;
    sample_audio_wr_arg *arg = (sample_audio_wr_arg *)argv;

    if (argc != (td_s32)sizeof(sample_audio_wr_arg) / (td_s32)sizeof(td_char *)) {
        sap_printf("cmd input is invalid!\n");
        return AUDIO_FAILURE;
    }

    addr = (td_u32)strtoul(arg->addr, TD_NULL, 16); /* 16 for hex */
    val = (td_u32)strtoul(arg->value, TD_NULL, 16); /* 16 for hex */

    if (is_4byte_aligned(addr) != TD_TRUE) {
        sap_printf("Invalid address\n");
        return AUDIO_FAILURE;
    }

    audio_reg_read(addr, read_val);
    audio_reg_write(addr, val);
    sap_printf("Writing addr 0x%08x: 0x%08x -> 0x%08x.\n", addr, read_val, val);

    return AUDIO_SUCCESS;
}

#define MEM32_MAX_NUM 1000

td_s32 sample_audio_mem32(td_s32 argc, td_char *argv[])
{
    td_u32 addr;
    td_u32 len;
    td_u32 val;
    td_u32 i;
    sample_audio_wr_arg *arg = (sample_audio_wr_arg *)argv;

    if (argc != (td_s32)sizeof(sample_audio_wr_arg) / (td_s32)sizeof(td_char *)) {
        sap_printf("cmd input is invalid!\n");
        return AUDIO_FAILURE;
    }

    addr = (td_u32)strtoul(arg->addr, TD_NULL, 16); /* 16 for hex */
    len = (td_u32)strtoul(arg->value, TD_NULL, 16); /* 16 for hex */
    if (len == 0 || len > MEM32_MAX_NUM) {
        sap_err_log_u32(len);
        return EXT_FAILURE;
    }

    if (is_4byte_aligned(addr) != TD_TRUE) {
        sap_printf("Invalid address\n");
        return AUDIO_FAILURE;
    }

    for (i = 0; i < len; i++) {
        /* 4 output value in a row */
        if (i % 4 == 0) {
            sap_printf("\n%08x =", addr);
        }
        audio_reg_read(addr, val);
        sap_printf(" %08x", val);
        addr = addr + ADDRESS_OFFSET;
    }
    sap_printf("\n");
    return AUDIO_SUCCESS;
}
#endif

static td_s32 sample_set_vol(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    td_u32 handle;
    uapi_audio_volume_cfg vol_cfg = {0, 0, 0, 0, 0};
    sample_set_vol_arg *arg = (sample_set_vol_arg *)argv;

    if (argc != (td_s32)sizeof(sample_set_vol_arg) / (td_s32)sizeof(td_char *)) {
        sap_printf("cmd input is invalid!\n");
        return AUDIO_FAILURE;
    }

    handle = (td_u32)strtoul(arg->handle, TD_NULL, 16); /* 16 for hex */
    vol_cfg.port = (td_u32)strtoul(arg->port, TD_NULL, 16); /* 16 for hex */
    vol_cfg.ch_id = (td_u32)strtoul(arg->ch_id, TD_NULL, 16); /* 16 for hex */
    vol_cfg.vol_type = (td_u32)strtoul(arg->type, TD_NULL, 16); /* 16 for hex */
    vol_cfg.integer = (td_s32)strtol(arg->integer, TD_NULL, 0);
    vol_cfg.decimal = (td_s32)strtol(arg->decimal, TD_NULL, 0);

    sap_printf("handle: 0x%08x |port: 0x%x |ch_id: 0x%d |type: 0x%d |integer: %d |decimal: %d\n",
        handle, vol_cfg.port, vol_cfg.ch_id, vol_cfg.vol_type, vol_cfg.integer, vol_cfg.decimal);

    ret = uapi_audio_set_volume(handle, &vol_cfg);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_audio_set_volume, ret);
        return ret;
    }

    return AUDIO_SUCCESS;
}

#define SAMPLE_ITEM(name) {#name, name, TD_FALSE}
#define SAMPLE_ITEM2(name, function) {#name, function, TD_FALSE}

static functions g_audio_func[] = {
#ifdef SAP_REG_DFX_SAMPLE_SUPPORT
    SAMPLE_ITEM2(w4, sample_audio_w4),
    SAMPLE_ITEM2(mem32, sample_audio_mem32),
#endif
    SAMPLE_ITEM(sample_set_vol),
    SAMPLE_ITEM(sample_set_config),
    SAMPLE_ITEM(sample_ai),
    SAMPLE_ITEM(sample_ao),
    SAMPLE_ITEM(sample_proc),
    SAMPLE_ITEM2(sample_bmic, sample_bmic),

#ifndef SAP_MINIMIZE_SAMPLE_RELEASE
    SAMPLE_ITEM(sample_asr),
    SAMPLE_ITEM(sample_vad_feed),
    SAMPLE_ITEM(sample_encode),
    SAMPLE_ITEM(sample_cast_aenc_play),
    SAMPLE_ITEM(sample_encode_play),
    SAMPLE_ITEM(sample_adec),
    SAMPLE_ITEM(sample_decode),
    SAMPLE_ITEM(sample_ai_aenc),
    SAMPLE_ITEM(sample_ai_sea_aenc),
    SAMPLE_ITEM(sample_ai_ao),
    SAMPLE_ITEM(sample_ai_sea_ao),
    SAMPLE_ITEM(sample_sea),
    SAMPLE_ITEM(sample_sea_only),
    SAMPLE_ITEM(sample_gui_aef),
    SAMPLE_ITEM(sample_lpwk),
    SAMPLE_ITEM(sample_dump),
    SAMPLE_ITEM(sample_rms),
    SAMPLE_ITEM2(sample_phone, sample_phone_application),
    SAMPLE_ITEM(sample_aef),
    SAMPLE_ITEM(sample_effect),
    SAMPLE_ITEM(sample_test_int),
    SAMPLE_ITEM(sample_haid),
    SAMPLE_ITEM2(sample_bmic_rx, sample_bmic),
    SAMPLE_ITEM2(sample_bmic_tx, sample_bmic),
    SAMPLE_ITEM2(sample_bmic_rec, sample_bmic),
    SAMPLE_ITEM(sample_dpm),
    SAMPLE_ITEM(sample_aha),
    SAMPLE_ITEM(sample_sle_music),
    SAMPLE_ITEM(sample_tts),
#endif
};

static td_void audio_auto_exit_function(td_s32 argc, td_char *argv[], td_u32 fun_id)
{
    td_u32 i;
    td_char *q = "q";
    td_char *cmd[] = {argv[0], q};

    if (!g_sample_cfg_singleton) {
        return;
    }

    /* do not need to quit when operating register */
    if ((strcmp(argv[0], "mem32") == 0) || (strcmp(argv[0], "w4") == 0)) {
        return;
    }

    /* audio func q */
    if (argc == 0x2 && (strcmp("q", argv[1]) == 0)) {
        g_audio_func[fun_id].running = TD_FALSE;
        return;
    }

    for (i = 0; i < (sizeof(g_audio_func) / sizeof(g_audio_func[0])); i++) {
        if (g_audio_func[i].running == TD_TRUE) {
            (td_void)g_audio_func[i].func(0x2, cmd);
            g_audio_func[i].running = TD_FALSE;
        }
    }

    g_audio_func[fun_id].running = TD_TRUE;
}

static td_void update_dsp_profile(td_void)
{
#if (SAP_CHIP_TYPE == brandy)
#ifndef CONFIG_PROT_CORE
    /* brandy默认进入低功耗模式，按最小频点工作，启动sample业务前需更新到所需频点 */
    pm_hifi_mode_t hifi_mode;
    if (SAP_DSP_CLK_HZ > HIFI_FREQ_176M) {
        hifi_mode = HIFI_LPM_CLK_HIGH_LEVEL;
    } else if (SAP_DSP_CLK_HZ > HIFI_FREQ_147M) {
        hifi_mode = HIFI_LPM_CLK_LEVEL2;
    }  else if (SAP_DSP_CLK_HZ > HIFI_FREQ_64M) {
        hifi_mode = HIFI_LPM_CLK_LEVEL1;
    } else {
        hifi_mode = HIFI_LPM_CLK_LOW_LEVEL;
    }

    if ((td_u32)g_dsp_profile < HIFI_LPM_CLK_MAX) {
        /* 对于需要指定DSP工作频点的场景，使用上层用户配置的频点 */
        hifi_mode = g_dsp_profile;
    }
    sap_printf("SAP_DSP_CLK_HZ = %d, hifi_mode = %d.\n", SAP_DSP_CLK_HZ, hifi_mode);

    uapi_set_hifi_mode(hifi_mode);
#endif
#endif
}

static td_u32 g_data_type_cnt = 0;
static data_type_func g_data_type_func[] = {
    /* unsigned */
    {"uint8_t", sizeof(uint8_t), "td_u8", sizeof(td_u8)},
    {"uint16_t", sizeof(uint16_t), "td_u16", sizeof(td_u16)},
    {"uint32_t", sizeof(uint32_t), "td_u32", sizeof(td_u32)},
    {"uint64_t", sizeof(uint64_t), "td_u64", sizeof(td_u64)},
    /* signed */
    {"int8_t", sizeof(int8_t), "td_s8", sizeof(td_s8)},
    {"int16_t", sizeof(int16_t), "td_s16", sizeof(td_s16)},
    {"int32_t", sizeof(int32_t), "td_s32", sizeof(td_s32)},
    {"int64_t", sizeof(int64_t), "td_s64", sizeof(td_s64)},
    /* other */
    {"uint8_t", sizeof(uint8_t), "td_bool", sizeof(td_bool)},
    {"uint32_t", sizeof(uint32_t), "td_handle", sizeof(td_handle)},
    {"char", sizeof(char), "td_char", sizeof(td_char)},
};

static td_void check_data_type(td_void)
{
    td_u32 i;
    td_s32 ret = EXT_SUCCESS;

    g_data_type_cnt++;

    for (i = 0; i < (sizeof(g_data_type_func) / sizeof(g_data_type_func[0])); i++) {
        if (g_data_type_func[i].std_data_size != g_data_type_func[i].td_data_size) {
            ret = EXT_FAILURE;
            sap_err_log_str(g_data_type_func[i].std_data_name);
            sap_err_log_str(g_data_type_func[i].td_data_name);
        }
    }

    if (ret != EXT_SUCCESS) {
        sap_err_log_info("check data type failed.");
    } else {
        sap_alert_log_info("check data type succeeded.");
    }
}

td_void audio_execute_function(td_s32 argc, td_char *argv[])
{
    td_u32 i;
    td_s32 ret;

    if (argc == 0) {
        return;
    }

    if (strcmp("help", *argv) == 0) {
        return;
    }

    if (g_data_type_cnt == 0) {
        check_data_type();
    }

    ret = uapi_audio_set_dsp_log_level(AUDIO_LOG_LEVEL_WARN);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_audio_set_dsp_log_level, ret);
    }

    for (i = 0; i < (sizeof(g_audio_func) / sizeof(g_audio_func[0])); i++) {
        if (strcmp(*argv, g_audio_func[i].name) != 0) {
            continue;
        }

        if (strcmp(*argv, "sample_proc") != 0) {
            /* do not need to quit when reading proc */
            update_dsp_profile();
            sample_audio_set_hifi_mode(UAPI_AUDIO_SCENE_UNKNOWN);
            audio_auto_exit_function(argc, argv, i);
        }

        ret = g_audio_func[i].func(argc, argv);
        sap_alert_log_str(g_audio_func[i].name);
        if (ret != EXT_SUCCESS) {
            sap_err_log_info("sample execute failed.");
        } else {
            sap_alert_log_info("sample execute succeeded.");
        }
        return;
    }

    sap_err_log_info("sample execute failed.");
}

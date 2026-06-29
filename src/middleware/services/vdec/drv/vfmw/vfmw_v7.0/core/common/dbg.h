/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __VFMW_DEBUG__
#define __VFMW_DEBUG__

#include "vfmw.h"
#include "vfmw_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VFMW_DBG_OK  0
#define VFMW_DBG_ERR (-1)

typedef enum {
    PRN_DEV_COM,
    PRN_DEV_LOG,
    PRN_DEV_MAX,
} print_dev;

extern td_u32 g_print_word;
extern print_dev g_print_out;


#ifdef VFMW_DPRINT_SUPPORT
#define dprint(type, fmt, arg...) dbg_print(type, fmt, ##arg)
#else
#define dprint(type, fmt, arg...)
#endif

#define dcheck(type) ((g_print_word & (1 << (type))) != 0)

typedef enum {
    PRN_FATAL = 0,  /* 0. fatal error */
    PRN_ERROR,      /* 1. normal error */
    PRN_WARN,       /* 2. warnning message */
    PRN_DBG,        /* 3. debug message */
    PRN_SCD_REGMSG, /* 4. scd register info */
    PRN_REF,        /* 5. reference pictures info */
    PRN_STM_BLK,    /* 6. stm module block info */
    PRN_SYN_BLK,    /* 7. syn module block info */
    PRN_DEC_BLK,    /* 8. dec module block info */
    PRN_PTS,        /* 9. pts info */
    PRN_PIX_SUM,    /* 10. pix sum info */
    PRN_CYCLE,      /* 11. cycle info */
    PRN_TIME,       /* 12. dec time info */
    PRN_REV_13,     /* 13. */
    PRN_REV_14,     /* 14. */
    PRN_REV_15,     /* 15. */
    PRN_REV_16,     /* 16. */
    PRN_REV_17,     /* 17. */
    PRN_REV_18,     /* 18. */
    PRN_REV_19,     /* 19. */
    PRN_REV_20,     /* 20. */
    PRN_REV_21,     /* 21. */
    PRN_REV_22,     /* 22. */
    PRN_REV_23,     /* 23. */
    PRN_REV_24,     /* 24. */
    PRN_REV_25,     /* 25. */
    PRN_REV_26,     /* 26. */
    PRN_REV_27,     /* 27. */
    PRN_REV_28,     /* 28. */
    PRN_REV_29,     /* 29. */
    PRN_REV_30,     /* 30. */
    PRN_REV_31,     /* 31. */
    PRN_ALWS = 32   /* 32. unlimit print level */
} print_type;

typedef enum {
    TPOS_QUE_RAW,
    TPOS_DEQ_RAW,
    TPOS_QUE_SEG,
    TPOS_DEQ_SEG,
    TPOS_QUE_DPS,
    TPOS_DEQ_DPS,
    TPOS_QUE_DEC,
    TPOS_DEQ_DEC,
    TPOS_MAX_NUM,
} time_pos;

#define MAX_TIME_NUM 1000
typedef struct {
    td_u32 frm_cnt;
    td_u32 frm_time[MAX_TIME_NUM][TPOS_MAX_NUM];
} time_info;

td_s32 str_to_val(const td_char *str, td_u32 *data);
td_s32 dbg_create_tracer(td_void);
td_void dbg_destroy_tracer(td_void);
td_void dbg_print_tracer(td_void);
td_void dbg_add_trace(const td_char *trace_string, td_s32 trace_line, td_s32 trace_data);
td_s32 dbg_save_yuv(td_s32 chan_id, vfmw_image *image, os_file *save_file);
td_s32 dbg_save_mtdt(td_s32 chan_id, const vfmw_image *image, os_file *save_file);
td_s32 dbg_set_print_word(td_u32 word);
td_u32 dbg_get_print_word(td_void);
print_dev dbg_get_print_dev(td_void);
td_void dbg_set_print_dev(print_dev out);
char *dbg_show_std(vfmw_vid_std std, const vfmw_std_ext *ext);
td_s32 dbg_print(td_u32 type, const td_char *format, ...);
td_void dbg_add_time(td_s32 chan_id, td_u32 fid, td_u32 pid, td_u32 time);
td_s32 dbg_show_time(td_void *buf);

td_void dbg_create_tracer_number(td_void);
td_void dbg_destroy_tracer_number(td_void);
td_void dbg_add_trace_number(td_u32 trace_data);
td_void dbg_read_reg(td_void);
td_s32 dbg_set_dbg_path(td_char *path, td_u32 path_len);
td_char *dbg_get_dbg_path(td_void);
td_s32 dbg_init(td_void);
td_void dbg_deinit(td_void);

#define pos()         dprint(PRN_ALWS, "pos: %s %d\r\n", __func__, __LINE__)
#define rec_pos(data) dbg_add_trace(__func__, __LINE__, (td_s32)(data))
#define rec_num(data) dbg_add_trace_number((td_s32)(data))

#define dprint_dbg(fmt...)
#define dprint_info(fmt...)
#define dprint_warn(fmt...)
#define dprint_err(fmt...)
#define dprint_fatal(fmt...)

#define print_s32(val) dprint(PRN_ALWS, "%s %d: %s = %d\n", __func__, __LINE__, #val, val)
#define print_u32(val) dprint(PRN_ALWS, "%s %d: %s = %u\n", __func__, __LINE__, #val, val)
#define print_s64(val) dprint(PRN_ALWS, "%s %d: %s = %lld\n", __func__, __LINE__, #val, val)
#define print_u64(val) dprint(PRN_ALWS, "%s %d: %s = %llu\n", __func__, __LINE__, #val, val)
#define print_h32(val) dprint(PRN_ALWS, "%s %d: %s = 0x%08X\n", __func__, __LINE__, #val, val)
#define print_h64(val) dprint(PRN_ALWS, "%s %d: %s = 0x%016llX\n", __func__, __LINE__, #val, val)
#define print_ptr(val) dprint(PRN_ALWS, "%s %d: %s = %p\n", __func__, __LINE__, #val, val)
#define print_flt(val) dprint(PRN_ALWS, "%s %d: %s = %f\n", __func__, __LINE__, #val, val)
#define print_func_err(func, err_code) dprint(PRN_ERROR, "Call %s return [0x%08X]\n", #func, err_code)

#define vfmw_check_no_print(cond) \
    do { \
        if (!(cond)) { \
            return; \
        } \
    } while (0)

#define vfmw_check_ret_no_print(cond, ret) \
    do { \
        if (!(cond)) { \
            return ret; \
        } \
    } while (0)

#define vfmw_check(cond) \
    do { \
        if (!(cond)) { \
            dprint(PRN_ERROR, "[%s %d] %s not match.\n", __func__, __LINE__, #cond); \
            return; \
        } \
    } while (0)

#define vfmw_check_print(cond, fmt, arg...) \
    do { \
        if (!(cond)) { \
            dprint(PRN_ERROR, fmt, ##arg); \
            return; \
        } \
    } while (0)

#define vfmw_check_goto_no_print(cond, tag) \
    do { \
        if (!(cond)) { \
            goto tag; \
        } \
    } while (0)

#define vfmw_check_goto(cond, tag) \
    do { \
        if (!(cond)) { \
            dprint(PRN_ERROR, "[%s %d] %s not match.\n", __func__, __LINE__, #cond); \
            goto tag; \
        } \
    } while (0)

#define vfmw_check_goto_print(cond, tag, fmt, arg...) \
    do { \
        if (!(cond)) { \
            dprint(PRN_ERROR, fmt, ##arg); \
            goto tag; \
        } \
    } while (0)

#define vfmw_check_ret(cond, ret) \
    do { \
        if (!(cond)) { \
            dprint(PRN_ERROR, "[%s %d] %s not match.\n", __func__, __LINE__, #cond); \
            return ret; \
        } \
    } while (0)

#define vfmw_check_ret_print(cond, ret, fmt, arg...) \
    do { \
        if (!(cond)) { \
            dprint(PRN_ERROR, fmt, ##arg); \
            return ret; \
        } \
    } while (0)

#define vfmw_check_break(cond) \
    do { \
        if (!(cond)) { \
            break; \
        } \
    } while (0)

#define vfmw_alert_ret(cond, ret) \
    do { \
        if (!(cond)) { \
            dprint(PRN_FATAL, "[%s %d] %s not match.\n", __func__, __LINE__, #cond); \
            return ret; \
        } \
    } while (0)

#define vfmw_alert(cond) \
    do { \
        if (!(cond)) { \
            dprint(PRN_FATAL, "[%s %d] %s not match.\n", __func__, __LINE__, #cond); \
            return; \
        } \
    } while (0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

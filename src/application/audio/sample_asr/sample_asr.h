/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: asr sample define
 * Author: audio
 * Create: 2025-10-13
 */

#ifndef SAMPLE_ASR_H
#define SAMPLE_ASR_H

#define FUZZ_SUCCESS              0
#define FUZZ_WORD_DONE            (td_s32)0x83001000
#define FUZZ_FRAME_LEN            160
#define CHAN_NUM_MAX              (2)

td_u32 sample_asr_get_inst_size(td_void);
td_u32 sample_asr_get_scratch_size(td_void);
td_void sample_asr_set_scratch(td_void *inst, td_char *buf);
td_void sample_asr_set_pps_data(td_void *inst, td_s16 pps_data[][FUZZ_FRAME_LEN]);
td_s32 asr_fuzz_init(td_void *inst, td_bool dfx);
td_s32 asr_fuzz_deinit(td_void *inst);
td_s32 asr_fuzz_proc(td_void *inst, td_s16 *stream_in, td_u32 readlen);

#endif

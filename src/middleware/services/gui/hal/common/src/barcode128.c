/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Description: barcode128
 * Author:
 * Create: 2023-12
 */

#include "barcode128.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "securec.h"

#define CODE128_QUIET_ZONE_LEN 0
#define CODE128_CODE_LEN       11
#define CODE128_STOP_CODE_LEN  13

#define CODE128_START_CODE_A 103
#define CODE128_START_CODE_B 104
#define CODE128_START_CODE_C 105

#define CODE128_FNC1 '\xf1'
#define CODE128_FNC2 '\xf2'
#define CODE128_FNC3 '\xf3'
#define CODE128_FNC4 '\xf4'

#define CODE128_MODE_A 'a'
#define CODE128_MODE_B 'b'
#define CODE128_MODE_C 'c'

// Since the FNCn characters are not ASCII, define versions here to simplify encoding strings that include them.
#define CODE128_FNC1 '\xf1'
#define CODE128_FNC2 '\xf2'
#define CODE128_FNC3 '\xf3'
#define CODE128_FNC4 '\xf4'

// quiet zone(begin/end) + start + checksum +stop
#define CODE128_MIN_ENCODE_LEN (CODE128_QUIET_ZONE_LEN * 2 + CODE128_CODE_LEN * 2 + CODE128_STOP_CODE_LEN)

static const int32_t MAX_STEPS = 512;
static const int32_t FUNC1_CODE = 102;
static const int32_t FUNC2_CODE = 97;
static const int32_t FUNC3_CODE = 96;
static const int32_t FUNC4_A_CODE = 101;
static const int32_t FUNC4_B_CODE = 100;
static const int32_t MODE_A = 101;
static const int32_t MODE_B = 100;
static const int32_t MODE_C = 99;

static const int32_t CODE128_PATTERN[] = {
    // value : code : pattern
    1740, //   0: 11011001100
    1644, //   1: 11001101100
    1638, //   2: 11001100110
    1176, //   3: 10010011000
    1164, //   4: 10010001100
    1100, //   5: 10001001100
    1224, //   6: 10011001000
    1220, //   7: 10011000100
    1124, //   8: 10001100100
    1608, //   9: 11001001000
    1604, //  10: 11001000100
    1572, //  11: 11000100100
    1436, //  12: 10110011100
    1244, //  13: 10011011100
    1230, //  14: 10011001110
    1484, //  15: 10111001100
    1260, //  16: 10011101100
    1254, //  17: 10011100110
    1650, //  18: 11001110010
    1628, //  19: 11001011100
    1614, //  20: 11001001110
    1764, //  21: 11011100100
    1652, //  22: 11001110100
    1902, //  23: 11101101110
    1868, //  24: 11101001100
    1836, //  25: 11100101100
    1830, //  26: 11100100110
    1892, //  27: 11101100100
    1844, //  28: 11100110100
    1842, //  29: 11100110010
    1752, //  30: 11011011000
    1734, //  31: 11011000110
    1590, //  32: 11000110110
    1304, //  33: 10100011000
    1112, //  34: 10001011000
    1094, //  35: 10001000110
    1416, //  36: 10110001000
    1128, //  37: 10001101000
    1122, //  38: 10001100010
    1672, //  39: 11010001000
    1576, //  40: 11000101000
    1570, //  41: 11000100010
    1464, //  42: 10110111000
    1422, //  43: 10110001110
    1134, //  44: 10001101110
    1496, //  45: 10111011000
    1478, //  46: 10111000110
    1142, //  47: 10001110110
    1910, //  48: 11101110110
    1678, //  49: 11010001110
    1582, //  50: 11000101110
    1768, //  51: 11011101000
    1762, //  52: 11011100010
    1774, //  53: 11011101110
    1880, //  54: 11101011000
    1862, //  55: 11101000110
    1814, //  56: 11100010110
    1896, //  57: 11101101000
    1890, //  58: 11101100010
    1818, //  59: 11100011010
    1914, //  60: 11101111010
    1602, //  61: 11001000010
    1930, //  62: 11110001010
    1328, //  63: 10100110000
    1292, //  64: 10100001100
    1200, //  65: 10010110000
    1158, //  66: 10010000110
    1068, //  67: 10000101100
    1062, //  68: 10000100110
    1424, //  69: 10110010000
    1412, //  70: 10110000100
    1232, //  71: 10011010000
    1218, //  72: 10011000010
    1076, //  73: 10000110100
    1074, //  74: 10000110010
    1554, //  75: 11000010010
    1616, //  76: 11001010000
    1978, //  77: 11110111010
    1556, //  78: 11000010100
    1146, //  79: 10001111010
    1340, //  80: 10100111100
    1212, //  81: 10010111100
    1182, //  82: 10010011110
    1508, //  83: 10111100100
    1268, //  84: 10011110100
    1266, //  85: 10011110010
    1956, //  86: 11110100100
    1940, //  87: 11110010100
    1938, //  88: 11110010010
    1758, //  89: 11011011110
    1782, //  90: 11011110110
    1974, //  91: 11110110110
    1400, //  92: 10101111000
    1310, //  93: 10100011110
    1118, //  94: 10001011110
    1512, //  95: 10111101000
    1506, //  96: 10111100010
    1960, //  97: 11110101000
    1954, //  98: 11110100010
    1502, //  99: 10111011110
    1518, // 100: 10111101110
    1886, // 101: 11101011110
    1966, // 102: 11110101110
    1668, // 103: 11010000100
    1680, // 104: 11010010000
    1692  // 105: 11010011100
};

static const int32_t CODE128_STOP_PATTERN = 6379; // 6379: 1100011101011, stop pattern.

typedef struct Code128Step {
    int32_t preIndex;      // Index of previous step, if any
    const char *nextInput; // Remaining input
    uint16_t len;          // The length of the pattern so far (includes this step)
    char mode;             // State for the current encoding
    char code;             // What code should be written for this step
} CodeStep;

typedef struct Code128State {
    CodeStep *steps;
    int32_t allocatedSteps;
    int32_t currentIndex;
    int32_t todoIndex;
    int32_t bestIndex;
    int32_t maxLength;
} CodeState;

int32_t Code128EstimateLen(const char *s)
{
    return CODE128_MIN_ENCODE_LEN + CODE128_CODE_LEN * (strlen(s) * 11 / 10); // 11,10: contents + 10% padding
}

static void Code128AppendPattern(int32_t pattern, int32_t patternLen, char *out)
{
    // All patterns have their first bit set by design
    for (int i = patternLen - 1; i >= 0; i--) {
        *out++ = (unsigned char)((((uint32_t)pattern & (1U << i)) != 0) ? 255 : 0); // 255: positive number
    }
}

static int32_t Code128AppendCode(int32_t code, char *out)
{
    Code128AppendPattern(CODE128_PATTERN[code], CODE128_CODE_LEN, out);
    return CODE128_CODE_LEN;
}

static int32_t Code128AppendStopCode(char *out)
{
    Code128AppendPattern(CODE128_STOP_PATTERN, CODE128_STOP_CODE_LEN, out);
    return CODE128_STOP_CODE_LEN;
}

static char Code128SwitchSode(char fromMode, char toMode)
{
    switch (fromMode) {
        case CODE128_MODE_A:
            switch (toMode) {
                case CODE128_MODE_B:
                    return MODE_B;

                case CODE128_MODE_C:
                    return MODE_C;

                default:
                    return -1;
            }

        case CODE128_MODE_B:
            switch (toMode) {
                case CODE128_MODE_A:
                    return MODE_A;

                case CODE128_MODE_C:
                    return MODE_C;

                default:
                    return -1;
            }

        case CODE128_MODE_C:
            switch (toMode) {
                case CODE128_MODE_B:
                    return MODE_B;

                case CODE128_MODE_A:
                    return MODE_A;

                default:
                    return -1;
            }

        default:
            return -1;
    }
}

static char Code128AAsciiToCode(char asciiVal)
{
    if (asciiVal >= ' ' && asciiVal <= '_') {
        return asciiVal - ' ';
    } else if (asciiVal > 0 && asciiVal < ' ') {
        return asciiVal + 64; // 64: mode A ascii to code swap offset
    } else if (asciiVal == CODE128_FNC1) {
        return FUNC1_CODE;
    } else if (asciiVal == CODE128_FNC2) {
        return FUNC2_CODE;
    } else if (asciiVal == CODE128_FNC3) {
        return FUNC3_CODE;
    } else if (asciiVal == CODE128_FNC4) {
        return FUNC4_A_CODE;
    } else {
        return -1;
    }
}

static char Code128BAsciiToCode(char asciiVal)
{
    if (asciiVal >= ' ') {
        return asciiVal - ' ';
    } else if (asciiVal == CODE128_FNC1) {
        return FUNC1_CODE;
    } else if (asciiVal == CODE128_FNC2) {
        return FUNC2_CODE;
    } else if (asciiVal == CODE128_FNC3) {
        return FUNC3_CODE;
    } else if (asciiVal == CODE128_FNC4) {
        return FUNC4_B_CODE;
    } else {
        return -1;
    }
}

static char Code128CAsciiToCode(const char *asciiVal)
{
    if (asciiVal[0] == CODE128_FNC1) {
        return FUNC1_CODE; // 102: code witch means FNC1.
    }

    if (asciiVal[0] >= '0' && asciiVal[0] <= '9' && asciiVal[1] >= '0' && asciiVal[1] <= '9') {
        char code = 10 * (asciiVal[0] - '0') + (asciiVal[1] - '0'); // 10: decimal number concatenation.
        return code;
    }
    return -1;
}

static bool Code128DoAStep(CodeStep *base, int32_t preIndex, int32_t index)
{
    CodeStep *previousStep = &base[preIndex];
    CodeStep *step = &base[index];

    char value = *previousStep->nextInput;
    // NOTE: Currently we can't encode NULL
    if (value == 0) {
        return false;
    }

    step->code = Code128AAsciiToCode(value);
    if (step->code < 0) {
        return false;
    }
    step->preIndex = preIndex;
    step->nextInput = previousStep->nextInput + 1;
    step->mode = CODE128_MODE_A;
    step->len = previousStep->len + CODE128_CODE_LEN;
    if (step->mode != previousStep->mode) {
        step->len += CODE128_CODE_LEN;
    }

    return true;
}

static bool Code128DoBStep(CodeStep *base, int32_t preIndex, int32_t index)
{
    CodeStep *previousStep = &base[preIndex];
    CodeStep *step = &base[index];

    char value = *previousStep->nextInput;
    // NOTE: Currently we can't encode NULL
    if (value == 0) {
        return false;
    }

    step->code = Code128BAsciiToCode(value);
    if (step->code < 0) {
        return false;
    }

    step->preIndex = preIndex;
    step->nextInput = previousStep->nextInput + 1;
    step->mode = CODE128_MODE_B;
    step->len = previousStep->len + CODE128_CODE_LEN;
    if (step->mode != previousStep->mode) {
        step->len += CODE128_CODE_LEN;
    } // Need to switch modes

    return true;
}

static bool Code128DoCStep(CodeStep *base, int32_t preIndex, int32_t index)
{
    CodeStep *previousStep = &base[preIndex];
    CodeStep *step = &base[index];

    char value = *previousStep->nextInput;
    // NOTE: Currently we can't encode NULL
    if (value == 0) {
        return false;
    }

    step->code = Code128CAsciiToCode(previousStep->nextInput);
    if (step->code < 0) {
        return false;
    }

    step->preIndex = preIndex;
    step->nextInput = previousStep->nextInput + 1;

    if (step->code < 100) { // 100: Mode C consumes 2 characters for codes 0-99
        step->nextInput++;
    }

    step->mode = CODE128_MODE_C;
    step->len = previousStep->len + CODE128_CODE_LEN;
    if (step->mode != previousStep->mode) {
        step->len += CODE128_CODE_LEN;
    }

    return true;
}

static CodeStep *Code128AllocStep(CodeState *state)
{
    if (state->todoIndex >= state->allocatedSteps) {
        state->todoIndex = state->allocatedSteps - 1;
    }

    CodeStep *step = &state->steps[state->todoIndex];
    (void)memset_s(step, sizeof(*step), 0, sizeof(*step));
    return step;
}

static void Code128DoStep(CodeState *state)
{
    CodeStep *step = &state->steps[state->currentIndex];
    if (*step->nextInput == 0) {
        // Done, so see if we have a new shortest encoding.
        if ((step->len < state->maxLength) || (state->bestIndex < 0 && step->len == state->maxLength)) {
            state->bestIndex = state->currentIndex;
            // Update maxLength to avoid considering anything longer
            state->maxLength = step->len;
        }
        return;
    }

    // Don't try if we're already at or beyond the max acceptable length;
    if (step->len >= state->maxLength) {
        return;
    }
    char mode = step->mode;
    Code128AllocStep(state);
    bool mode_c_worked = false;

    // Always try mode C
    if (Code128DoCStep(state->steps, state->currentIndex, state->todoIndex)) {
        state->todoIndex++;
        Code128AllocStep(state);
        mode_c_worked = true;
    }

    if (mode == CODE128_MODE_A) {
        // If A works, stick with A. There's no advantage to switching to B proactively if A still works.
        if (Code128DoAStep(state->steps, state->currentIndex, state->todoIndex) ||
            Code128DoBStep(state->steps, state->currentIndex, state->todoIndex)) {
            state->todoIndex++;
        }
    } else if (mode == CODE128_MODE_B) {
        // The same logic applies here. There's no advantage to switching proactively to A if B still works.
        if (Code128DoBStep(state->steps, state->currentIndex, state->todoIndex) ||
            Code128DoAStep(state->steps, state->currentIndex, state->todoIndex)) {
            state->todoIndex++;
        }
    } else if (!mode_c_worked) {
        // In mode C. If mode C worked and we're in mode C, trying anything else is pointless since the mode C encoding
        // will be shorter and there won't be any mode switches.

        // If we're leaving mode C, though, try both in case one ends up better than the other.
        if (Code128DoAStep(state->steps, state->currentIndex, state->todoIndex)) {
            state->todoIndex++;
            Code128AllocStep(state);
        }
        if (Code128DoBStep(state->steps, state->currentIndex, state->todoIndex)) {
            state->todoIndex++;
        }
    }
}

static void FillOutBufWithCodes(CodeState *state, char *out)
{
    // Determine the list of codes
    int32_t codesNum = state->maxLength / CODE128_CODE_LEN;
    char codes[codesNum];
    CodeStep *step = &state->steps[state->bestIndex];
    int32_t i;
    for (i = codesNum - 1; i > 0; --i) {
        CodeStep *prevStep = &state->steps[step->preIndex];
        codes[i] = step->code;
        if (step->mode != prevStep->mode) {
            --i;
            codes[i] = Code128SwitchSode(prevStep->mode, step->mode);
        }
        step = prevStep;
    }
    codes[0] = step->code;

    // Encode everything up to the checksum
    (void)memset_s(out, CODE128_QUIET_ZONE_LEN, 0, CODE128_QUIET_ZONE_LEN);
    out += CODE128_QUIET_ZONE_LEN;
    for (i = 0; i < codesNum; i++) {
        out += Code128AppendCode(codes[i], out);
    }

    // Compute the checksum
    int sum = codes[0];
    for (i = 1; i < codesNum; i++) {
        sum += (int)codes[i] * i;
    }
    out += Code128AppendCode(sum % 103, out); // 103: the divider for calculating crc.

    // Finalize the code.
    out += Code128AppendStopCode(out);
    (void)memset_s(out, CODE128_QUIET_ZONE_LEN, 0, CODE128_QUIET_ZONE_LEN);
}

static int32_t Code128EncodeRaw(const char *s, char *out, int32_t maxLength)
{
    CodeState state;
    const int32_t overhead = CODE128_QUIET_ZONE_LEN + CODE128_CODE_LEN + CODE128_STOP_CODE_LEN + CODE128_QUIET_ZONE_LEN;
    if (maxLength < overhead + CODE128_CODE_LEN + CODE128_CODE_LEN) {
        // Need space to encode the start character and one additional character.
        return 0;
    }

    state.allocatedSteps = MAX_STEPS;
    state.steps = (CodeStep *)malloc(state.allocatedSteps * sizeof(CodeStep));
    if (state.steps == NULL) {
        return 0;
    }

    state.currentIndex = 0;
    state.todoIndex = 3;  // 3: start todo index.
    state.maxLength = maxLength - overhead;
    state.bestIndex = -1;

    // Initialize the first 3 steps for the 3 encoding routes (A, B, C)
    state.steps[0].preIndex = -1;
    state.steps[0].nextInput = s;
    state.steps[0].len = CODE128_CODE_LEN;
    state.steps[0].mode = CODE128_MODE_C;
    state.steps[0].code = CODE128_START_CODE_C;

    state.steps[1].preIndex = -1;
    state.steps[1].nextInput = s;
    state.steps[1].len = CODE128_CODE_LEN;
    state.steps[1].mode = CODE128_MODE_A;
    state.steps[1].code = CODE128_START_CODE_A;

    state.steps[2].preIndex = -1;  // 2: array subscript
    state.steps[2].nextInput = s;  // 2: array subscript
    state.steps[2].len = CODE128_CODE_LEN;  // 2: array subscript
    state.steps[2].mode = CODE128_MODE_B;  // 2: array subscript
    state.steps[2].code = CODE128_START_CODE_B;  // 2: array subscript

    // Keep going until no more work
    do {
        Code128DoStep(&state);
        state.currentIndex++;
    } while (state.currentIndex != state.todoIndex && state.todoIndex < (MAX_STEPS - 1));

    // If no best_step, then fail.
    if (state.bestIndex < 0) {
        free(state.steps);
        state.steps = NULL;
        return 0;
    }

    FillOutBufWithCodes(&state, out);
    free(state.steps);
    return state.maxLength + overhead;
}

int32_t Code128Encode(const char *s, char *out, int32_t maxLength)
{
    if (s == NULL) {
        return 0;
    }

    char raw[strlen(s) + 1];
    char *p = raw;

    if (strncmp(s, "[FNC1]", 6) == 0) {  // 6: the string length of "[FNC1]".
        *p++ = CODE128_FNC1;
        s += 6; // 6: the string length of "[FNC1]".
    }

    for (; *s != '\0'; s++) {
        if (*s != ' ') {
            *p++ = *s;
        }
    }
    *p = '\0';

    return Code128EncodeRaw(raw, out, maxLength);
}

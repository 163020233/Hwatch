/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: PullUpDownModel
 * Created: 2026-01-08
 */

#ifndef PULL_UP_DOWN_MODEL_H
#define PULL_UP_DOWN_MODEL_H

#include <cstdio>

namespace OHOS {
#define NUM_1 1
#define NUM_2 2
#define NUM_3 3
#define NUM_4 4
#define NUM_5 5
#define NUM_7 7
#define NUM_16 16
#define NUM_99 99

class PullUpDownModel {
public:
    PullUpDownModel();
    virtual ~PullUpDownModel();
    static PullUpDownModel *GetInstance(void);
    int GetBriScrStatus(void);
    int GetDntDistStatus(void);
    void UpdateBriScrStatus(uint16_t status);
    void UpdateDntDistStatus(uint16_t status);
private:
    int briScreenStatus{0};
    int dntDisturbStatus{0};
};
}

#endif // PULL_UP_DOWN_MODEL_H

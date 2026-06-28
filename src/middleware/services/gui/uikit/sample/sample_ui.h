/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SAMPLE_UI_H
#define SAMPLE_UI_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes and starts the graphical test suite for wearable devices.
 * This function is used to initialize and launch the graphical test interface for wearable devices.
 * It serves as the entry point for the test suite, preparing the environment and executing the tests.
 * @return int 0: Success; Non-zero: Failure
 */
int SampleUi(void);

/*
 * @brief This function initializes and starts the graphical test suite for the socmn2 product,
 * including loading and executing test cases.
 */
void SampleUISocmn2(void);

/**
 * @brief Starting a Specified Test Case
 * @param testCaseId testcase name
 */
void StartCaseUi(const char* testCaseId);

#ifdef __cplusplus
}
#endif
#endif
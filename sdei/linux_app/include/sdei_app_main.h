/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

#ifndef __SDEI_APP_MAIN_H__
#define __SDEI_APP_MAIN_H__

#define SDEI_APP_VERSION_MAJOR  1
#define SDEI_APP_VERSION_MINOR  0

#define SDEI_NUM_TESTS 50

#define SDEI_PASS  0
#define SDEI_SKIP  1
#define SDEI_FAIL  2
#define SDEI_ERROR 3

typedef unsigned long test_flags;

typedef struct sdei_log_control {
    int32_t print_level;
    void *log_file_handle;
} sdei_log_control;

typedef struct sdei_test_control {
	/* which tests to run? */
    test_flags flags[2];
    unsigned int tests_skipped;
    unsigned int tests_passed;
    unsigned int tests_failed;
    unsigned int tests_aborted;
} sdei_test_control;

typedef struct sdei_control {
    sdei_log_control log_control;
    sdei_test_control tst_control;
} sdei_control_t;

typedef struct __SDEI_MSG__ {
    char string[100];
    unsigned long data;
} sdei_msg_parms_t;

#endif

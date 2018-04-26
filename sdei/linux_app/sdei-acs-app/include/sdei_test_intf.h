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


#ifndef __SDEI_TEST_INTF_H__
#define __SDEI_TEST_INTF_H__

#include "sdei_app_main.h"

#define ACS_LOG_KERNEL 0x6
#define SDEI_TEST_COMPLETE 0xFFFFFFFF
#define SDEI_TEST_CLEANUP    0xAAAAAAAA

/* Function Prototypes */
void testlib_enable_test(sdei_control_t *control, int test_id);
void testlib_disable_test(sdei_control_t *control, int test_id);
int init_test_env(sdei_control_t *control);
int testlib_execute_tests(sdei_control_t *control);
void testlib_cleanup(void);

void testlib_run_specific(sdei_control_t *control, int test_id, int count);
void read_msg_from_proc_sdei(void);
#endif

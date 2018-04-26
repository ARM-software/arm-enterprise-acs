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


#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <stdint.h>
#include "include/sdei_test_intf.h"

static inline void enable_test(sdei_test_control *control, int i) {
    control->flags[i/64] |= ((unsigned long)(1ULL << (i % 64)));
}

static inline void disable_test(sdei_test_control *control, int i) {
    control->flags[i/64] &= ~((unsigned long)(1ULL << (i % 64)));
}

void testlib_run_specific(sdei_control_t *control, int test_id, int count)
{
    if (count == 1) {
        control->tst_control.flags[0] = 0ULL;
        control->tst_control.flags[1] = 0ULL;
    }

    if (test_id <= SDEI_NUM_TESTS)
        enable_test(&control->tst_control, test_id);
}

void testlib_enable_test(sdei_control_t *control, int test_id) {

    if (test_id < SDEI_NUM_TESTS)
        enable_test(&control->tst_control, test_id);
}

void testlib_disable_test(sdei_control_t *control, int test_id) {

    if (test_id < SDEI_NUM_TESTS)
        disable_test(&control->tst_control, test_id);
}

int init_test_env(sdei_control_t *control)
{
    FILE *fd = NULL;

    fd = fopen("/proc/sdei", "rw+");
    if (fd == NULL) {
        printf("Unable to open /proc/sdei\n");
        return SDEI_FAIL;
    }

    // Initialize global counters
    control->tst_control.tests_passed = 0;
    control->tst_control.tests_failed = 0;
    control->tst_control.tests_skipped = 0;
    control->tst_control.tests_aborted = 0;
    control->tst_control.flags[0] = ~(0ULL);
    control->tst_control.flags[1] = ~(0ULL);
    control->log_control.log_file_handle = NULL;
    control->log_control.print_level = 3;

    fclose(fd);
    return 0;
}

void testlib_cleanup(void)
{
    FILE *fd = NULL;
    unsigned int flag = SDEI_TEST_CLEANUP;

    fd = fopen("/proc/sdei_msg", "w");
    if (fd == NULL) {
        printf("Unable to open /proc/sdei_msg\n");
        return;
    }

    fwrite(&flag, 1, sizeof(flag), fd);
    fclose(fd);
}

void read_msg_from_proc_sdei(void)
{
    FILE *fd = NULL;
    char buf_msg[sizeof(sdei_msg_parms_t)];
    sdei_msg_parms_t msg_params;

    fd = fopen("/proc/sdei_msg", "r");
    if (fd == NULL) {
        printf("Unable to open /proc/sdei_msg\n");
        return;
    }

    fseek(fd, 0, SEEK_SET);
    /* Print Until buffer is empty */
    while (fread(buf_msg,sizeof(buf_msg),1,fd)) {
        printf("%s", buf_msg);
    }
    fclose(fd);
}

int testlib_execute_tests(sdei_control_t *control)
{
    FILE *fd = NULL;
    unsigned int test_status = 0;

    fd = fopen("/proc/sdei", "rw+");
    if (fd == NULL) {
        printf("Unable to open /proc/sdei\n");
        return SDEI_FAIL;
    }

    /* Few tests targeted only for UEFI environment.
     * So skipping those tests in Linux
     */
    testlib_disable_test(control, 44);
    testlib_disable_test(control, 46);
    testlib_disable_test(control, 47);

    fwrite(control, 1, sizeof(struct sdei_control), fd);

    if (control->log_control.print_level != ACS_LOG_KERNEL) {
        while (test_status != SDEI_TEST_COMPLETE) {
            fread(&test_status, sizeof(test_status), 1, fd);
            read_msg_from_proc_sdei();
        }
    }

    fclose(fd);
    return 0;
}

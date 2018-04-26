/** @file
 *
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
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

#include "val_interface.h"

extern sdei_log_control g_log_control;
sdei_test_desc sdei_test[SDEI_NUM_TESTS];

static int is_test_passed(int test_id) {
    int i;
    for (i = 0; i < SDEI_NUM_TESTS && sdei_test[i].id != test_id; i++);
    if (i < SDEI_NUM_TESTS)
        return sdei_test[i].status == SDEI_TEST_PASS;
    return 0;
}

static int deps_resolved(sdei_test_deps *deps) {
    int i;
    for (i = 0; deps[i]!=0 && is_test_passed(deps[i]); i++);
    return (deps[i] == 0);
}

static inline int run_test(sdei_test_control *control, int i) {
    if (control->flags[i/64] & (1ULL << (i % 64)))
        return deps_resolved(sdei_test[i].deps);
    return 0;
}

static inline void enable_test(sdei_test_control *control, int i) {
    control->flags[i/64] |= ((unsigned long)(1ULL << (i % 64)));
}

static inline void disable_test(sdei_test_control *control, int i) {
    control->flags[i/64] &= ~((unsigned long)(1ULL << (i % 64)));
}

void val_test_run_specific(sdei_test_control *control, int test_id, int init) {
    int i;
    if (init) {
        control->flags[0] = 0ULL;
        control->flags[1] = 0ULL;
    }
    for (i = 0; i < SDEI_NUM_TESTS && sdei_test[i].id != test_id; i++);
    if (i < SDEI_NUM_TESTS)
        enable_test(control, i);
}

static void init_test(sdei_test_control *control, sdei_test_desc *test) {
    val_test_set_status(val_pe_get_num(), SDEI_TEST_PENDING);
    /* Always print test id and test description */
    val_print(ACS_LOG_ERR, "\n%d. ", test->id);
#ifdef TARGET_LINUX
    val_print(ACS_LOG_ERR, "%s", test->description);
#else
    val_print(ACS_LOG_ERR, test->description);
#endif
}

static void log_test_result(sdei_test_control *control, int result) {

    /* Always print test results */
    if (result == SDEI_TEST_PASS) {
        control->tests_passed += 1;
        val_print(ACS_LOG_ERR, " : PASS\n");
    }
    else if (result == SDEI_TEST_SKIP) {
        control->tests_skipped += 1;
        val_print(ACS_LOG_ERR, " : SKIP\n");
    }
    else if (result == SDEI_TEST_FAIL) {
        control->tests_failed += 1;
        val_print(ACS_LOG_ERR, " : FAIL\n");
    }
    else if (result == SDEI_TEST_ABORT) {
        control->tests_aborted += 1;
        val_print(ACS_LOG_ERR, " : ABORT\n");
    }
    else if (result == SDEI_TEST_TIMEOUT) {
        val_print(ACS_LOG_ERR, " : TIMEOUT\n");
    }
    else if (result == SDEI_TEST_PENDING) {
        val_print(ACS_LOG_ERR, " : PENDING\n");
    }
    else if (result == SDEI_TEST_ERROR) {
        val_print(ACS_LOG_ERR, " : ERROR\n");
    }
}

/**
 *  @brief   This function initializes the test infrastructure.
 *
 *  @return  none
 */
void val_test_init(sdei_test_control *control) {
    sdei_test[0] = test_001;
    sdei_test[1] = test_002;
    sdei_test[2] = test_003;
    sdei_test[3] = test_004;
    sdei_test[4] = test_005;
    sdei_test[5] = test_006;
    sdei_test[6] = test_007;
    sdei_test[7] = test_008;
    sdei_test[8] = test_009;
    sdei_test[9] = test_010;
    sdei_test[10] = test_011;
    sdei_test[11] = test_012;
    sdei_test[12] = test_013;
    sdei_test[13] = test_014;
    sdei_test[14] = test_015;
    sdei_test[15] = test_016;
    sdei_test[16] = test_017;
    sdei_test[17] = test_018;
    sdei_test[18] = test_019;
    sdei_test[19] = test_020;
    sdei_test[20] = test_021;
    sdei_test[21] = test_022;
    sdei_test[22] = test_023;
    sdei_test[23] = test_024;
    sdei_test[24] = test_025;
    sdei_test[25] = test_026;
    sdei_test[26] = test_027;
    sdei_test[27] = test_028;
    sdei_test[28] = test_029;
    sdei_test[29] = test_030;
    sdei_test[30] = test_031;
    sdei_test[31] = test_032;
    sdei_test[32] = test_033;
    sdei_test[33] = test_034;
    sdei_test[34] = test_035;
    sdei_test[35] = test_036;
    sdei_test[36] = test_037;
    sdei_test[37] = test_038;
    sdei_test[38] = test_039;
    sdei_test[39] = test_040;
    sdei_test[40] = test_041;
    sdei_test[41] = test_042;
    sdei_test[42] = test_043;
    sdei_test[43] = test_044;
    sdei_test[44] = test_045;
    sdei_test[45] = test_046;
    sdei_test[46] = test_047;
    sdei_test[47] = test_048;
    sdei_test[48] = test_049;
    control->flags[0] = ~(0ULL);
    control->flags[1] = ~(0ULL);
}

/**
 *  @brief   This function enables a test for a given test id.
 *  @param test_id   Test ID number
 *
 *  @return  none
 */
void val_test_enable(sdei_test_control *control, int test_id) {
    int i;
    for (i = 0; i < SDEI_NUM_TESTS && sdei_test[i].id != test_id; i++);
    if (i < SDEI_NUM_TESTS)
        enable_test(control, i);
}

/**
 *  @brief   This function disables a test for a given test id.
 *  @param test_id   Test ID number
 *
 *  @return  none
 */
void val_test_disable(sdei_test_control *control, int test_id) {
    int i;
    for (i = 0; i < SDEI_NUM_TESTS && sdei_test[i].id != test_id; i++);
    if (i < SDEI_NUM_TESTS)
        disable_test(control, i);
}

/**
 *  @brief   This function executes all test cases and log the results
 *
 *  @return  none
 */
void val_test_execute(sdei_test_control *control) {
    int i;
    uint32_t num_pe;

    for (i = 0; i < SDEI_NUM_TESTS; i++) {
        init_test(control, &sdei_test[i]);
        if (sdei_test[i].all_pe)
            num_pe = val_pe_get_num();
        else
            num_pe = 1;
        if (run_test(control, i)) {
            sdei_test[i].test_fn();
            sdei_test[i].status = val_test_get_status(num_pe, TEST_TIMEOUT);
        }
        else
            sdei_test[i].status = SDEI_TEST_SKIP;
        log_test_result(control, sdei_test[i].status);
    }
}

/**
 *  @brief   This function sets test status for a given PE.
 *  @param index   PE index
 *  @param status  test status
 *
 *  @return  none
 */
void val_test_pe_set_status(uint32_t index, uint32_t status)
{
    pe_shared_mem_t mem;
    mem.status = status;
    val_shared_mem_write(index, &mem);
}

/**
 *  @brief   This function sets test status for a given number of PEs.
 *  @param num_pe   number of PEs
 *  @param status   test status
 *
 *  @return  none
 */
void val_test_set_status(uint32_t num_pe, uint32_t status)
{
    uint32_t i;
    for (i = 0; i < num_pe; i++) {
        val_test_pe_set_status(i, status);
    }
}

/**
 *  @brief   This function gets test status for a given PE.
 *  @param index  PE index
 *
 *  @return  status for given PE
 */
uint32_t val_test_pe_get_status(uint32_t index)
{
    pe_shared_mem_t mem;
    val_shared_mem_read(index, &mem);
    return mem.status;
}

/**
 *  @brief   This function Gets test status for a given number of PEs.
 *           It keeps polling for a change of status from pending to a known termination status.
 *  @param num_pe   number of PEs
 *  @param timeout  Polling timeout value
 *
 *  @return  status
 */
uint32_t val_test_get_status(uint32_t num_pe, uint64_t timeout)
{
    uint32_t status, i;

    if (num_pe == 1) {
        /* On Linux, the payload can be scheduled on any PE by the Kernel.
           Hence check for status on all PEs  */
        num_pe = val_pe_get_num();
        while (timeout--) {
            status = SDEI_TEST_PASS;
            for (i=0; i < num_pe; i++) {
                status = val_test_pe_get_status(i);
                if (status != SDEI_TEST_PENDING)
                    return status;
            }
        }
    }
    else {
        while (timeout--) {
            status = SDEI_TEST_PASS;
            for (i = 0; i < num_pe; i++) {
                status = status | val_test_pe_get_status(i);
                if (status & SDEI_TEST_FAIL)
                    return status;
            }
            if (status == SDEI_TEST_PASS)
                return status;
        }
    }
    return SDEI_TEST_TIMEOUT;
}

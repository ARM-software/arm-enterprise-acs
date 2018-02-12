/** @file
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

#include <val_interface.h>

#define TEST_DESC "Verify that system has EL2, or EL3, or both     "

/* This function reads ID_AA64PFR0_EL1 system register on each PE and verify the 
 * EL3 or EL2 implementation
 */
static void payload(void* data)
{
    uint64_t cfg;
    int el3 = 0, el2 = 0;
    acs_status_t status = val_pe_reg_read(ID_AA64PFR0_EL1, &cfg);
    if (status == ACS_SUCCESS) {
        if (cfg & (0xf << 12))
            el3 = 1;
        if (cfg & (0xf << 8))
            el2 = 1;
    } else {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_ABORT);
        return;
    }
    if (el3 || el2)
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);
    else
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
}

static void test_entry(void)
{
    val_pe_execute_on_all((void*)payload, 0);
}

SDEI_SET_TEST_DEPS(test_001_deps, TEST_NONE_ID);
SDEI_PUBLISH_TEST(test_001, TEST_001_ID, TEST_DESC, test_001_deps, test_entry, TRUE);

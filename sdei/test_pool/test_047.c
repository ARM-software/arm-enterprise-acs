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
#include <val_sdei_interface.h>

#define TEST_DESC "Verify after PE reset, events will be masked   "

#define ALREADY_MASKED 0

static volatile uint32_t g_test_status = SDEI_TEST_PASS;
static uint32_t g_reset_pe_index;

uint32_t get_reset_pe_index(void)
{
    uint32_t my_index = val_pe_get_index();
    uint32_t num_pe = val_pe_get_num(), i;
    for (i = 0; i < num_pe; i++) {
        if (my_index != i)
            return i;
    }
    return -1;
}

static void payload(void *ignore)
{
    uint64_t result = 0;
    int32_t err;

    if (val_pe_get_index() == g_reset_pe_index) {
        err = val_sdei_mask(&result);
        if (err || (result != ALREADY_MASKED)) {
            g_test_status = SDEI_TEST_FAIL;
        }
    }
}

static void test_entry(void)
{

    g_reset_pe_index = get_reset_pe_index();
    val_pe_data_cache_clean_invalidate((uint64_t)&g_reset_pe_index);

    /* Reset PE */
    val_pe_poweroff(g_reset_pe_index);
    val_pe_poweron(g_reset_pe_index);

    val_pe_execute_on_all(payload, 0);

    if (g_test_status & SDEI_TEST_FAIL) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
    } else {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);
    }

}

SDEI_SET_TEST_DEPS(test_047_deps, TEST_001_ID, TEST_002_ID);
SDEI_PUBLISH_TEST(test_047, TEST_047_ID, TEST_DESC, test_047_deps, test_entry, FALSE);

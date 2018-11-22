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

#define TEST_DESC "Verify SDEI_PE_UNMASK Call                     "

static void payload(void *data)
{
    int32_t err;

    err = val_sdei_unmask();
    if (err) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);
}

static void test_entry(void)
{
    val_pe_execute_on_all(payload, 0);
}

SDEI_SET_TEST_DEPS(test_016_deps, TEST_001_ID, TEST_002_ID);
SDEI_PUBLISH_TEST(test_016, TEST_016_ID, TEST_DESC, test_016_deps, test_entry, TRUE);

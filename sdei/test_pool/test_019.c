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

#define TEST_DESC "Verify ERROR CODE-NOT_SUPPORTED                "

static void test_entry(void)
{
    int32_t err;
    uint64_t version;

    err = val_sdei_get_version(&version);
    if (!err) {
        val_print(ACS_LOG_WARN, "\n        SDEI dispatcher present, skipping test   ");
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_SKIP);
        return;
    }

    if (err == SDEI_STATUS_NOT_SUPPORTED) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);
        return;
    }

    val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
}

SDEI_SET_TEST_DEPS(test_019_deps, TEST_001_ID, TEST_002_ID);
SDEI_PUBLISH_TEST(test_019, TEST_019_ID, TEST_DESC, test_019_deps, test_entry, FALSE);

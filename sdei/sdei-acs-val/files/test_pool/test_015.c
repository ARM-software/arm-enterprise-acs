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

#define TEST_DESC "Verify SDEI_PE_MASK Call                       "

#define ALREADY_MASKED 0
#define MASKED 1

/* Verifying PE_MASK call on each PE */
static void payload(void *data)
{
    int32_t err;
    uint32_t status = SDEI_TEST_PASS;
    uint64_t result = 0;

    err = val_sdei_unmask();
    if (err) {
        status = SDEI_TEST_SKIP;
        goto set_status;
    }

    err = val_sdei_mask(&result);
    if (err || (result != MASKED)) {
        status = SDEI_TEST_FAIL;
        goto set_status;
    }

    err = val_sdei_mask(&result);
    if (err || (result != ALREADY_MASKED)) {
        status = SDEI_TEST_FAIL;
        goto set_status;
    }

set_status:
    val_test_pe_set_status(val_pe_get_index(), status);
}

static void test_entry(void)
{
    val_pe_execute_on_all(payload, 0);
}

SDEI_SET_TEST_DEPS(test_015_deps, TEST_001_ID, TEST_002_ID);
SDEI_PUBLISH_TEST(test_015, TEST_015_ID, TEST_DESC, test_015_deps, test_entry, TRUE);

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

#define TEST_DESC "Verify SDEI_FEATURES Call                      "
#define VALID_FEATURE 0
#define INVALID_FEATURE 0x10

static void payload(void)
{
    uint64_t num_slots, shared_slots, private_slots;
    int32_t err;

    /* Check SDEI_FEATURES valid call */
    err = val_sdei_features(VALID_FEATURE, &num_slots);

    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI_FEATURES failed with err %d", err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    } else {
        private_slots = __EXTRACT_BITS(num_slots, 0, 16);
        shared_slots = __EXTRACT_BITS(num_slots, 16, 16);

        if (__EXTRACT_BITS(num_slots, 32, 32) || (private_slots < 2) || (shared_slots < 2)) {
            val_print(ACS_LOG_ERR, "\n        SDEI_FEATURES expected value mismatch");
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        }
    }

    /* Check Invalid Parameters call */
    err = val_sdei_features(INVALID_FEATURE, &num_slots);
    if (err != SDEI_STATUS_INVALID) {
        val_print(ACS_LOG_ERR, "\n        SDEI_FEATURES failed with err %d", err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);
}

static void test_entry(void)
{
    payload();
}

SDEI_SET_TEST_DEPS(test_017_deps, TEST_NONE_ID);
SDEI_PUBLISH_TEST(test_017, TEST_017_ID, TEST_DESC, test_017_deps, test_entry, FALSE);

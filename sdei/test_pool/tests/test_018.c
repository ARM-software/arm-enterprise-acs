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

#define TEST_DESC "Verify SDEI_VERSION Call                       "

static void test_entry(void)
{
    uint64_t version, major_rev, minor_rev;
    int32_t err;

    err = val_sdei_get_version(&version);

    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI_VERSION failed with err %d", err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    } else {
        major_rev = __EXTRACT_BITS(version, 48, 15);
        minor_rev = __EXTRACT_BITS(version, 32, 16);
        val_print(ACS_LOG_INFO, "\n        SDEI minor revision = %llx", major_rev);
        val_print(ACS_LOG_INFO, "\n        SDEI major revision = %llx", minor_rev);

        if (major_rev != 1) {
            val_print(ACS_LOG_ERR, "\n        SDEI_VERSION major revision value mismatch"
                                            " expected = %d: found = %llx", 1, major_rev);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        }
        if (minor_rev != 0) {
            val_print(ACS_LOG_ERR, "\n        SDEI_VERSION minor revision value mismatch"
                                   "expected = %d: found = %llx", 0, minor_rev);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        }
        if (__EXTRACT_BITS(version, 63, 1)) { /* Bit[63] should be 0 */
            val_print(ACS_LOG_ERR, "\n        SDEI_VERSION expected value mismatch");
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        }
    }
    val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);
}

SDEI_SET_TEST_DEPS(test_018_deps, TEST_NONE_ID);
SDEI_PUBLISH_TEST(test_018, TEST_018_ID, TEST_DESC, test_018_deps, test_entry, FALSE);

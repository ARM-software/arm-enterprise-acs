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

#define TEST_DESC "Verify that system has GICv3                    "

/* This function verifies the GICv3 support implementation */
static void payload(void)
{
    uint32_t gic_version;
    acs_status_t status = val_gic_get_version(&gic_version);

    if (status == ACS_SUCCESS) {
        if (gic_version == GIC_INFO_VERSION) {
            val_print(ACS_LOG_INFO, "\n        GICv3 implemented");
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);
        } else {
            val_print(ACS_LOG_INFO, "\n        GICv3 not implemented");
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        }
    } else {
        val_print(ACS_LOG_ERR, "\n        val_gic_get_version failed");
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_ABORT);
    }
    return;
}

static void test_entry(void)
{
    payload();
}

SDEI_SET_TEST_DEPS(test_002_deps, TEST_NONE_ID);
SDEI_PUBLISH_TEST(test_002, TEST_002_ID, TEST_DESC, test_002_deps, test_entry, FALSE);

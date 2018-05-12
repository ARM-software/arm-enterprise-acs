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

#define TEST_DESC "Verify client execute in AArch64 State          "

static void payload(void)
{
    val_print(ACS_LOG_ERR, "\n        Not implemented                            ");
    val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_SKIP);
}

static void test_entry(void)
{
    payload();
}

SDEI_SET_TEST_DEPS(test_008_deps, TEST_NONE_ID);
SDEI_PUBLISH_TEST(test_008, TEST_008_ID, TEST_DESC, test_008_deps, test_entry, FALSE);

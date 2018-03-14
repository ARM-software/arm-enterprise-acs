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

#define TEST_DESC "Verify ERROR CODE-OUT_OF_RESOURCE              "
#define VALID_FEATURE 0

static void test_entry(void)
{
    int32_t err;
    uint64_t num_slots;
    uint32_t i, shared_slots, intr_num = SPI_INTR_NUM;
    uint32_t event_num[100];

    err = val_sdei_features(VALID_FEATURE, &num_slots);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI_FEATURES failed with err %d", err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    shared_slots = __EXTRACT_BITS(num_slots, 16, 16);

    /* Bind the interrupts for available shared slots */
    for (i=0; i < shared_slots; i++) {
        err = val_gic_disable_interrupt(intr_num+i);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        Interrupt %d disable failed\n", intr_num+i);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        }
        err = val_sdei_interrupt_bind(intr_num+i, event_num+i);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        Interrupt number %d bind failed with err %d",
                                                                        intr_num+i, err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        }
    }

    /* Bind event when no bind slots are available */
    err = val_gic_disable_interrupt(intr_num+i);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Interrupt %d disable failed", intr_num+i);
        return;
    }
    err = val_sdei_interrupt_bind(intr_num+i, event_num+i);
    if (err != SDEI_STATUS_OUT_OF_RESOURCE) {
        val_print(ACS_LOG_ERR, "\n        ERROR CODE-OUT_OF_RESOURCE check failed");
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
    } else {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);
    }

    /* Release all the bind interrupts */
    for (i=0; i<shared_slots; i++) {
        err = val_sdei_interrupt_release(event_num[i]);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        Event number %d intrrupt release failed with err %d",
                                                                            event_num[i], err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        }
    }
}

SDEI_SET_TEST_DEPS(test_023_deps, TEST_001_ID, TEST_002_ID);
SDEI_PUBLISH_TEST(test_023, TEST_023_ID, TEST_DESC, test_023_deps, test_entry, FALSE);

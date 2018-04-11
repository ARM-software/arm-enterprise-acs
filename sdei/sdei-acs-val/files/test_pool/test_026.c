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

#define TEST_DESC "Verify GHES fw discovery, events and notify    "

static void test_entry(void)
{
    uint32_t type;
    uint64_t result = 0;
    int32_t err;
    int32_t num_events;
    int32_t i;
    event_info_t *event_info;
    struct sdei_event event;

    /* GHES firmware discovery in ACPI */
    type = HEST_FOUND;
    err = val_event_get_hest_info(type, &result);
    if (result && !err) {
        val_print(ACS_LOG_INFO, "\n        Generic hardware error source found in ACPI");
    } else {
        val_print(ACS_LOG_ERR, "\n        Generic hardware error source not found    ");
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    /* GHES Notify structres check */
    type = NUM_GHES;
    err = val_event_get_hest_info(type, &result);
    if (result && !err) {
        val_print(ACS_LOG_INFO, "\n        Generic hardware error source notify structures found");
    } else {
        val_print(ACS_LOG_ERR, "\n        Generic hardware error source notify structures"
                                                                                " not found");
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    /* GHES SDEI Events check */
    type = NUM_EVENTS;
    err = val_event_get_hest_info(type, &result);
    if (result && !err) {
        val_print(ACS_LOG_INFO, "\n        Generic hardware error source SDEI events found: %lld",
                                                                                            result);
    } else {
        val_print(ACS_LOG_ERR, "\n        Generic hardware error source SDEI events not found");
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    type = HEST_NOTIFY;
    num_events = result;
    err = val_event_get_hest_info(type, &result);
    if (!result && !err) {
        val_print(ACS_LOG_ERR, "\n        HEST SDEI events info not found");
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }
    event_info = (event_info_t *)result;
    event.is_bound_irq = FALSE;
    for (i=0; i < num_events; i++)
    {
        event.event_num = event_info->number;
        err = val_sdei_event_register(event.event_num, (uint64_t)asm_event_handler, NULL, 0, 0);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event %d register failed with err %x",
                                                                event.event_num, err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        }
        err = val_sdei_event_unregister(event.event_num);
        if (err) {
            val_print(ACS_LOG_WARN, "\n        SDEI event %d unregister failed with err %x",
                                                                event.event_num, err);
        }
        event_info++;
    }

    val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);
}

SDEI_SET_TEST_DEPS(test_026_deps, TEST_NONE_ID);
SDEI_PUBLISH_TEST(test_026, TEST_026_ID, TEST_DESC, test_026_deps, test_entry, FALSE);

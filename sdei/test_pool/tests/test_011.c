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

#define TEST_DESC "Verify SDEI event status test                  "

#define SDEI_UNKNOWN_EVENT 0x10000

static void event_handler(void)
{
    return;
}

static void test_entry(void)
{
    int32_t err;
    uint64_t result;
    uint32_t event_num;

    event_num = val_event_get(SDEI_EVENT_TYPE_SHARED, SDEI_EVENT_PRIORITY_ANY);

    /* Verify the event status in event un-registered state */
    err = val_sdei_event_status(event_num, &result);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event status test failed with err %d", err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }
    if (result == 0) {
        val_print(ACS_LOG_INFO, "\n        SDEI event %d unregistered, disabled,"
                                           " event handler not running", event_num);
    }
    else {
        val_print(ACS_LOG_ERR, "\n        SDEI event status test failed");
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    /* Verify the event status in event register and enable state */
    err = val_sdei_event_register(event_num, (uint64_t)event_handler, NULL, 0, 0);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event %d register failed with err %x",
                                                                            event_num, err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }
    else {
        err = val_sdei_event_enable(event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event enable test failed with err %d", err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            goto event_unregister;
        }
        err = val_sdei_event_status(event_num, &result);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event status test failed with err %d", err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            goto event_unregister;
        }

        if (!(result & (EVENT_STATUS_REGISTER_BIT | EVENT_STATUS_ENABLE_BIT))) {
            val_print(ACS_LOG_ERR, "\n        SDEI event status test failed");
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            goto event_unregister;
        }

        err = val_sdei_event_unregister(event_num);
        if (err) {
            val_print(ACS_LOG_WARN, "\n        SDEI event %d unregister failed with err %x",
                                                                            event_num, err);
        }
    }

    /* Verify the event status for unknown event */
    event_num = SDEI_UNKNOWN_EVENT;
    err = val_sdei_event_status(event_num, &result);
    if (err != SDEI_STATUS_INVALID) {
        val_print(ACS_LOG_ERR, "\n        SDEI event status test failed with err %d", err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);
    return;

event_unregister:
    err = val_sdei_event_unregister(event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event %d unregister fail with err %x",
                                                                            event_num, err);
    }
}

SDEI_SET_TEST_DEPS(test_011_deps, TEST_001_ID, TEST_002_ID);
SDEI_PUBLISH_TEST(test_011, TEST_011_ID, TEST_DESC, test_011_deps, test_entry, FALSE);

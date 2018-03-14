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

#define TEST_DESC "Verify SDEI_EVENT_ROUTING_SET call              "
#define SDEI_UNKNOWN_EVENT 0x10000
#define INVALID_AFFINITY   0xFFFFFFFF

static void test_entry(void)
{
    int32_t err, evt_routing_err;
    int32_t cpu_id = 2;
    uint32_t event_num;

    event_num = val_event_get(SDEI_EVENT_TYPE_SHARED, SDEI_EVENT_PRIORITY_ANY);

    /* Routing Mode - RM_ANY testcase */
    err = val_sdei_event_register(event_num, (uint64_t)asm_event_handler, NULL, 0, 0);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event %d register failed with err %x",
                                                                                event_num, err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    } else {
        evt_routing_err = val_sdei_event_routing_set(event_num, FALSE, 0);
        if (evt_routing_err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event routing set failed with err %x",
                                                                                evt_routing_err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            goto event_unregister;
        }

        err = val_sdei_event_unregister(event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event %d unregister failed with err %x",
                                                                                   event_num, err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        }
    }

    event_num = val_event_get(SDEI_EVENT_TYPE_SHARED, SDEI_EVENT_PRIORITY_ANY);
    evt_routing_err = val_sdei_event_routing_set(event_num, FALSE, 0);
    if (evt_routing_err != SDEI_STATUS_DENIED) {
        val_print(ACS_LOG_ERR, "\n        SDEI event routing set failed with err %x",
                                                                            evt_routing_err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    /* Routing Mode - RM_PE testcase */
    err = val_sdei_event_register(event_num, (uint64_t)asm_event_handler, NULL, 0, 0);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event %d register failed with err %x",
                                                                                event_num, err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }
    else {
        evt_routing_err = val_sdei_event_routing_set(event_num, TRUE, cpu_id);
        if (evt_routing_err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event routing set failed with err %x",
                                                                                evt_routing_err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            goto event_unregister;
        }

        err = val_sdei_event_unregister(event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event %d unregister failed with err %x",
                                                                                event_num, err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        }
    }

    /* INVALID Parameters if Invalid Affinity */
    err = val_sdei_event_register(event_num, (uint64_t)asm_event_handler, NULL, 0, 0);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event %d register failed with err %x",
                                                                                event_num, err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    } else {
        evt_routing_err = val_sdei_event_routing_set(event_num, TRUE, INVALID_AFFINITY);
        if (evt_routing_err != SDEI_STATUS_INVALID) {
            val_print(ACS_LOG_ERR, "\n        SDEI event routing set failed with err %x",
                                                                                   evt_routing_err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            goto event_unregister;
        }

        err = val_sdei_event_unregister(event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event %d unregister failed with err %x",
                                                                                    event_num, err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        }
    }

    /* INVALID Parameters if Invalid Event Number */
    event_num = SDEI_UNKNOWN_EVENT;
    evt_routing_err = val_sdei_event_routing_set(event_num, FALSE, 0);

    if (evt_routing_err != SDEI_STATUS_INVALID) {
        val_print(ACS_LOG_ERR, "\n        SDEI event routing set failed with err %x",
                                                                            evt_routing_err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    /* INVALID Parameters if Not Shared Event */
    event_num = val_event_get(SDEI_EVENT_TYPE_PRIVATE, SDEI_EVENT_PRIORITY_ANY);
    if (event_num) {
        err = val_sdei_event_register(event_num, (uint64_t)asm_event_handler, NULL, 0, 0);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event %d register failed with err %x",
                                                                                event_num, err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        } else {
            evt_routing_err = val_sdei_event_routing_set(event_num, FALSE, 0);
            if (evt_routing_err != SDEI_STATUS_INVALID) {
                val_print(ACS_LOG_ERR, "\n        SDEI event routing set failed with err %x",
                                                                            evt_routing_err);
                val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
                goto event_unregister;
            }

            err = val_sdei_event_unregister(event_num);
            if (err) {
                val_print(ACS_LOG_ERR, "\n        SDEI event %d unregister failed with err %x",
                                                                                event_num, err);
                val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
                return;
            }
        }
    } else
        val_print(ACS_LOG_TEST, "\n        SDEI private event not found");

    val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);
    return;

event_unregister:
    err = val_sdei_event_unregister(event_num);
    if (err) {
        val_print(ACS_LOG_WARN, "\n        SDEI event %d unregister failed with err %x",
                                                                            event_num, err);
    }
}

SDEI_SET_TEST_DEPS(test_009_deps, TEST_001_ID, TEST_002_ID);
SDEI_PUBLISH_TEST(test_009, TEST_009_ID, TEST_DESC, test_009_deps, test_entry, FALSE);

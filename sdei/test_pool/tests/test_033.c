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

#define TEST_DESC "Verify SDEI_EVENT_SIGNAL test                  "

#define SDEI_UNKNOWN_EVENT 0x10000

static volatile int32_t g_handler_flag = 1;

static void event_handler(void)
{
    g_handler_flag = 0;
}

/* Registering event 0 on each PE and verifying sdei event signal to each PE */
static void payload(void *ignore)
{
    uint64_t timeout;
    uint64_t res = 0;
    int32_t err;
    void *entry_point;
    struct sdei_event event;

    g_handler_flag = 1;
    timeout = TIMEOUT_MEDIUM;
    entry_point = &asm_event_handler;
    event.is_bound_irq = FALSE;
    event.event_num = 0;

    err = val_sdei_event_register(event.event_num, (uint64_t)entry_point,
                                                                 (void *)event_handler, 0, 0);
    if (err) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    err = val_sdei_event_enable(event.event_num);
    if (err) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_unregister;
    }

    err = val_sdei_event_signal(event.event_num, val_pe_get_mpid_index(val_pe_get_index()));
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event signal failed with err %d", err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_unregister;
    }

    while (g_handler_flag && timeout--);
    if (g_handler_flag) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_unregister;
    }

    val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);
    timeout = TIMEOUT_MEDIUM;
    do {
        err = val_sdei_event_status(event.event_num, &res);
        if (!(res & EVENT_STATUS_RUNNING_BIT))
            break;
    } while (timeout--);

event_unregister:
    err = val_sdei_event_unregister(event.event_num);
}

static void test_entry(void) {
    uint32_t event_num;
    int32_t err;

    /* verifying sdei_event_signal for unknown event */
    event_num = SDEI_UNKNOWN_EVENT;
    err = val_sdei_event_signal(event_num, val_pe_get_mpid_index(0));
    if (err != SDEI_STATUS_INVALID) {
        val_print(ACS_LOG_ERR, "\n        SDEI event signal failed with err %d", err);
        val_test_set_status(val_pe_get_num(), SDEI_TEST_FAIL);
        return;
    }

    val_pe_execute_on_all((void*)payload, 0);
}

SDEI_SET_TEST_DEPS(test_033_deps, TEST_001_ID, TEST_002_ID);
SDEI_PUBLISH_TEST(test_033, TEST_033_ID, TEST_DESC, test_033_deps, test_entry, TRUE);

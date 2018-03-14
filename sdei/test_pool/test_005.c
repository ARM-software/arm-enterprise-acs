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

#define TEST_DESC "Verify SDEI event enable test                   "
#define SDEI_UNKNOWN_EVENT 0x10000
#define EVENT_ENABLED 0x2

/* This function verifies the shared and private events enable functionality.
 * it also checks event enable error checks.
 */
static uint32_t payload(void *data)
{
    uint64_t type = (uint64_t)data;
    uint32_t event_num, status = SDEI_TEST_PASS;
    int32_t err;
    uint64_t event_status;

    event_num = val_event_get(type, SDEI_EVENT_PRIORITY_ANY);
    if (!event_num) {
        status = SDEI_TEST_SKIP;
        goto return_status;
    }
    /*Try to to enable unregistered event */
    err = val_sdei_event_enable(event_num);
    if (err != SDEI_STATUS_DENIED) {
        status = SDEI_TEST_FAIL;
        goto return_status;
    }

    err = val_sdei_event_register(event_num, (uint64_t)asm_event_handler, NULL, 0, 0);
    if (err) {
        status = SDEI_TEST_FAIL;
        goto return_status;
    }

    err = val_sdei_event_enable(event_num);
    if (err) {
        status = SDEI_TEST_FAIL;
        goto event_unregister;
    }

    err = val_sdei_event_status(event_num, &event_status);
    if (err) {
        status = SDEI_TEST_ERROR;
        goto event_unregister;
    }

    if (!(event_status & EVENT_ENABLED)) {
        status = SDEI_TEST_FAIL;
        goto event_unregister;
    }

event_unregister:
    err = val_sdei_event_unregister(event_num);
    if (err)
        status = SDEI_TEST_ERROR;
return_status:
    val_test_pe_set_status(val_pe_get_index(), status);
    return status;
}

static void test_entry(void)
{
    uint32_t ret;

    ret = payload((void*)SDEI_EVENT_TYPE_SHARED);
    if (ret & SDEI_TEST_FAIL)
        return;

    /* Invalid event number enable testcase */
    ret = val_sdei_event_enable(SDEI_UNKNOWN_EVENT);
    if (ret != SDEI_STATUS_INVALID) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    val_pe_execute_on_all(payload, SDEI_EVENT_TYPE_PRIVATE);
}

SDEI_SET_TEST_DEPS(test_005_deps, TEST_001_ID, TEST_002_ID);
SDEI_PUBLISH_TEST(test_005, TEST_005_ID, TEST_DESC, test_005_deps, test_entry, TRUE);

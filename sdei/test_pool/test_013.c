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

#define TEST_DESC "Verify SDEI private reset test                 "

#define REGISTERED 0x1

/* Verifying Privet reset on each PE */
static uint32_t payload(void *data)
{
    uint32_t event_num, status = SDEI_TEST_PASS;
    int32_t err;
    uint64_t event_status = 0;

    event_num = val_event_get(SDEI_EVENT_TYPE_PRIVATE, SDEI_EVENT_PRIORITY_ANY);
    if (!event_num) {
        status = SDEI_TEST_SKIP;
        goto set_status;
    }

    err = val_sdei_event_register(event_num, (uint64_t)asm_event_handler, NULL, 0, 0);
    if (err) {
        status = SDEI_TEST_SKIP;
        goto set_status;
    }

    err = val_sdei_private_reset(NULL);
    if (err) {
        status = SDEI_TEST_FAIL;
        goto event_unregister;
    }

    err = val_sdei_event_status(event_num, &event_status);
    if (err) {
        status = SDEI_TEST_ERROR;
        goto event_unregister;
    }

    if (event_status & REGISTERED) {
        status = SDEI_TEST_FAIL;
        goto event_unregister;
    } else
        goto set_status;

event_unregister:
    err = val_sdei_event_unregister(event_num);
    if (err)
        status = SDEI_TEST_ERROR;
set_status:
    val_test_pe_set_status(val_pe_get_index(), status);
    return status;
}

static void test_entry(void)
{
    val_pe_execute_on_all(payload, 0);
}

SDEI_SET_TEST_DEPS(test_013_deps, TEST_NONE_ID);
SDEI_PUBLISH_TEST(test_013, TEST_013_ID, TEST_DESC, test_013_deps, test_entry, TRUE);

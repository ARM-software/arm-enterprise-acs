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

#define TEST_DESC "Verify Suspend, check wakeup mask and status   "

#define REGISTERED 0x1
#define ALREADY_MASKED 0
#define WD_TIMEOUT 0x1000
#define SUSPEND_POWERDOWN  1

static uint32_t g_wd_index;

static void isr(void)
{
    uint32_t wd_int_id;
    val_wd_set_ws0(0, g_wd_index, 0);
    wd_int_id = val_wd_get_info(g_wd_index, WD_INFO_GSIV);
    val_print(ACS_LOG_INFO, "\n        Watchdog interrupt %d recieved", wd_int_id);
    val_gic_end_of_interrupt(wd_int_id);
}

static void test_entry(void)
{
    uint32_t wd_int_id, event_num, wd_num;
    uint64_t result, event_status;
    int err;

    g_wd_index = -1;
    wd_num = val_wd_get_info(0, WD_INFO_COUNT);

    /* Get the first non-secure watchdog timer index */
    while (wd_num--) {
        if (!val_wd_get_info(wd_num, WD_INFO_ISSECURE)) {
            g_wd_index = wd_num;
            break;
        }
    }

    if (g_wd_index == -1) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_SKIP);
        return;
    }

    /* Get a shared event */
    event_num = val_event_get(SDEI_EVENT_TYPE_SHARED, SDEI_EVENT_PRIORITY_ANY);
    if (!event_num) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    /* Register the shared event*/
    err = val_sdei_event_register(event_num, (uint64_t)asm_event_handler, NULL, 0, 0);
    if (err) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    /* Unmask SDEI on this PE*/
    err = val_sdei_unmask();
    if (err) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_unregister;
    }

    /* Get the watchdog interrupt id */
    wd_int_id = val_wd_get_info(g_wd_index, WD_INFO_GSIV);

    /* Install interrupt handler for the watchdog interrupt */
    err = val_gic_install_isr(wd_int_id, isr);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Interrupt register %x failed with err %d\n",
                                                                        wd_int_id, err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_unregister;
    }

    /* Enable the watchdog with a small timeout value */
    val_wd_set_ws0(0, g_wd_index, WD_TIMEOUT);

    /* Put the PE to suspend with powerdown state */
    val_pe_suspend(SUSPEND_POWERDOWN);

    /* After wake up, check the event status */
    err = val_sdei_event_status(event_num, &event_status);
    if (err) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto free_interrupt;
    }

    /* The event status should be REGISTERED */
    if (!(event_status & REGISTERED)) {
        val_print(ACS_LOG_ERR, "\n        Event status changed after waking up 0x%x", event_status);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto free_interrupt;
    }

    /* Check the PE SDEI mask status. PE should be already masked. */
    err = val_sdei_mask(&result);
    if (err || (result != ALREADY_MASKED)) {
        val_print(ACS_LOG_ERR, "\n        PE unmasked after waking up 0x%x", result);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto free_interrupt;
    }

    val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);
free_interrupt:
    val_gic_free_interrupt(wd_int_id);
event_unregister:
    err = val_sdei_event_unregister(event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event %d unregister fail with err %x",
                                                            event_num, err);
    }
}

SDEI_SET_TEST_DEPS(test_045_deps, TEST_NONE_ID);
SDEI_PUBLISH_TEST(test_045, TEST_045_ID, TEST_DESC, test_045_deps, test_entry, FALSE);

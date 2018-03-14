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

#define TEST_DESC "If PE off, no event should not bring it online "

#define WD_TIMEOUT 0x1
#define TIMEOUT 0x1000

static int g_handler_called;
static uint32_t g_wd_index;

static void event_handler(void) {
    val_wd_set_ws0(0, g_wd_index, 0);
    g_handler_called = 1;
}
uint32_t get_secondary_pe_index(void)
{
    uint32_t my_index = val_pe_get_index();
    uint32_t num_pe = val_pe_get_num(), i;
    for (i = 0; i < num_pe; i++) {
        if (my_index != i)
            return i;
    }
    return -1;
}

static void test_entry(void)
{
    uint32_t wd_int_id, event_num, wd_num, timeout;
    uint64_t secondary_mpidr;
    uint32_t secondary_index;
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

    val_pe_data_cache_clean_invalidate((uint64_t)&g_wd_index);

    /* Read Watchdog interrupt from Watchdog info table */
    wd_int_id = val_wd_get_info(g_wd_index, WD_INFO_GSIV);
    /* Bind Watchdog interrupt to event */
    err = val_sdei_interrupt_bind(wd_int_id, &event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Interrupt (%d) bind failed", wd_int_id);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    secondary_index = get_secondary_pe_index();
    secondary_mpidr = val_pe_get_mpid_index(secondary_index);

    err = val_sdei_event_register(event_num, (uint64_t)asm_event_handler, (void*)event_handler,
                                  SDEI_EVENT_REGISTER_RM_PE, secondary_mpidr);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Event (%d) register failed", event_num);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto interrupt_release;
    }

    err = val_sdei_event_enable(event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Event (%d) enable failed", event_num);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_unregister;
    }

    val_pe_poweroff(secondary_index);

    val_wd_set_ws0(0, g_wd_index, WD_TIMEOUT);

    timeout = TIMEOUT;

    while (!timeout--) {
        val_pe_data_cache_invalidate((uint64_t)&g_handler_called);
        if (g_handler_called)
            break;
    }

    val_wd_set_ws0(0, g_wd_index, 0);

    if (g_handler_called) {
        val_print(ACS_LOG_ERR, "\n        Event turned on powered off core");
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
    } else
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);

event_unregister:
    err = val_sdei_event_unregister(event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Event (%d) unregister failed", event_num);
    }
    
interrupt_release:
    err = val_sdei_interrupt_release(event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Interrupt (%d) release failed", wd_int_id);
    }
}

SDEI_SET_TEST_DEPS(test_048_deps, TEST_NONE_ID);
SDEI_PUBLISH_TEST(test_048, TEST_048_ID, TEST_DESC, test_048_deps, test_entry, FALSE);

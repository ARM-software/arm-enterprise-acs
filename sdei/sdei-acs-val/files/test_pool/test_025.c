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

#define TEST_DESC "Verify SDEI_EVENT_CONTEXT and X0,X1,X2 args    "
#define INVALID_PARAM_ID 0x100

static int32_t g_wd_num;
static uint64_t g_int_id = 0;
static uint64_t g_x0_arg = 0;
static uint64_t g_x1_arg = 0;
static uint64_t *g_wd_addr = NULL;
static volatile int32_t g_handler_flag = 1;

static void event_handler(uint64_t x0, uint64_t x1, uint64_t x2)
{
    int32_t err;
    uint32_t query;
    uint64_t *result = NULL;

    val_wd_set_ws0(g_wd_addr, g_wd_num, 0);

    /* Query on X0 value using event context api */
    query = 0;
    err = val_sdei_event_context(query, result);

    /* Verifying x0 and x1 args after trigger the event */
    if (x0 != g_x0_arg) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        g_handler_flag = 0;
        return;
    }
    if (x1 != g_x1_arg) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        g_handler_flag = 0;
        return;
    }

    /* Invalid parameter check */
    query = INVALID_PARAM_ID;
    err = val_sdei_event_context(query, result);
     if (err != SDEI_STATUS_INVALID) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        g_handler_flag = 0;
        return;
    }

    val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);
    g_handler_flag = 0;
}

static void test_entry(void) {
    uint32_t ns_wdg = 0;
    uint64_t result = 0;
    uint64_t timer_expire_ticks = 1, timeout;
    uint64_t wd_ctrl_base;
    struct sdei_event event;
    int32_t err;

    g_handler_flag = 1;
    g_wd_num = val_wd_get_info(0, WD_INFO_COUNT);
    event.is_bound_irq = TRUE;

    do {
        /* array index starts from 0, so subtract 1 from count */
        g_wd_num--;

        /* Skip Secure watchdog */
        if (val_wd_get_info(g_wd_num, WD_INFO_ISSECURE))
            continue;

        ns_wdg++;
        timeout = WD_TIME_OUT;

        /* Read Watchdog interrupt from Watchdog info table */
        g_int_id = val_wd_get_info(g_wd_num, WD_INFO_GSIV);
        val_print(ACS_LOG_DEBUG, "\n        WS0 interrupt id: %lld", g_int_id);
        /* Read Watchdog base address from Watchdog info table */
        wd_ctrl_base = val_wd_get_info(g_wd_num, WD_INFO_CTRL_BASE);
        g_wd_addr = val_pa_to_va(wd_ctrl_base);

        err = val_gic_disable_interrupt(g_int_id);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        Interrupt %lld disable failed", g_int_id);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            goto unmap_va;
        }
        /* Bind Watchdog interrupt to event */
        err = val_sdei_interrupt_bind(g_int_id, &event.event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SPI interrupt number %lld bind failed with err %d",
                                    g_int_id, err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            goto unmap_va;
        }

        g_x0_arg = event.event_num;
        g_x1_arg = (uint64_t)event_handler;
        val_pe_data_cache_clean_invalidate((uint64_t)&g_x0_arg);
        val_pe_data_cache_clean_invalidate((uint64_t)&g_x1_arg);
        err = val_sdei_event_register(event.event_num, (uint64_t)asm_event_handler,
                                      (void *)event_handler, 0, 0);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event %d register failed with err %x",
                                    event.event_num, err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            goto interrupt_release;
        }

        err = val_sdei_event_enable(event.event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event enable test failed with err %d", err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            goto event_unregister;
        }

        /* Generate Watchdog interrupt */
        val_wd_set_ws0(g_wd_addr, g_wd_num, timer_expire_ticks);

        while (timeout--) {
            val_pe_data_cache_invalidate((uint64_t)&g_handler_flag);
            if (g_handler_flag == 0)
                break;
        }
        if (g_handler_flag) {
            val_print(ACS_LOG_ERR, "\n        Watchdog interrupt trigger failed");
            val_wd_set_ws0(g_wd_addr, g_wd_num, 0);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            goto event_unregister;
        }
    } while (0);

    if (!ns_wdg) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        val_print(ACS_LOG_ERR, "\n        No non-secure Watchdogs reported");
        return;
    }

event_unregister:
    timeout = TIMEOUT_MEDIUM;
    do {
        err = val_sdei_event_status(event.event_num, &result);
        if (err)
            val_print(ACS_LOG_ERR, "\n        SDEI event %d status failed err %x",
                                                                        event.event_num, err);
        if (!(result & EVENT_STATUS_RUNNING_BIT))
            break;
    } while (timeout--);

    err = val_sdei_event_unregister(event.event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event %d unregister fail with err %x",
                                                                            event.event_num, err);
    }
interrupt_release:
    err = val_sdei_interrupt_release(event.event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Event number %d release failed with err %x",
                                                                        event.event_num, err);
    }
unmap_va:
    val_va_free(g_wd_addr);
}

SDEI_SET_TEST_DEPS(test_025_deps, TEST_001_ID, TEST_002_ID);
SDEI_PUBLISH_TEST(test_025, TEST_025_ID, TEST_DESC, test_025_deps, test_entry, FALSE);

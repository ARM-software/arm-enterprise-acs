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

#define TEST_DESC "Verify always availability- SDEI_EVENT_SIGNAL  "

static int32_t g_wd_num;
static uint64_t g_test_status= SDEI_TEST_PASS;
static uint64_t *g_wd_addr = NULL;
static volatile int32_t g_handler_flag = 1;
static struct sdei_event g_event;
static uint32_t g_main_pe;

static uint32_t get_secondary_pe_index(void)
{
    uint32_t my_index = val_pe_get_index();
    uint32_t num_pe = val_pe_get_num(), i;

    for (i = 0; i < num_pe; i++) {
        if (my_index != i)
            return i;
    }
    return -1;
}

static void event_signal_handler(void)
{
    g_handler_flag = 0;
    val_pe_data_cache_clean_invalidate((uint64_t)&g_handler_flag);
}

/* Check the SDEI_EVENT_SIGNAL in below states
 *          1. Handler-Unregistered State
 *          2. Handler-registered State
 *          3. Handler-Enabled State
 *          4. Handler-Enabled & Running State
 *          5. Handler-Unregister Pending State
 */
static void event_handler(void)
{
    int32_t err;
    uint32_t event_num = 0;
    uint64_t res = 0;
    uint64_t timeout;

    val_wd_set_ws0(g_wd_addr, g_wd_num, 0);

    timeout = TIMEOUT_MEDIUM;
    /* Check 4 - Handler-Enabled & Running State */
    err = val_sdei_event_signal(event_num, val_pe_get_mpid_index(g_main_pe));
    if (err) {
        g_test_status = SDEI_TEST_FAIL;
        g_handler_flag = 0;
        return;
    }
    while (timeout--) {
        if (!g_handler_flag)
            break;
    }
    /* Check the event signal handler invoke */
    if (g_handler_flag) {
        g_test_status = SDEI_TEST_FAIL;
        g_handler_flag = 0;
        return;
    }
    timeout = TIMEOUT_MEDIUM;
    do {
        err = val_sdei_event_status(event_num, &res);
        if (err)
            break;
        if (!(res & EVENT_STATUS_RUNNING_BIT))
            break;
    } while (timeout--);

    err = val_sdei_event_unregister(g_event.event_num);
    if (err != SDEI_STATUS_PENDING) {
        g_test_status = SDEI_TEST_FAIL;
        g_handler_flag = 0;
        return;
    }

    timeout = TIMEOUT_MEDIUM;
    /* Check 5 - Handler-Unregister Pending State */
    err = val_sdei_event_signal(event_num, val_pe_get_mpid_index(val_pe_get_index()));
    if (err) {
        g_test_status = SDEI_TEST_FAIL;
        g_handler_flag = 0;
        return;
    }
    while (timeout--) {
        if (!g_handler_flag)
            break;
    }
    /* Check the event signal handler invoke */
    if (g_handler_flag) {
        g_test_status = SDEI_TEST_FAIL;
        g_handler_flag = 0;
        return;
    }
    timeout = TIMEOUT_MEDIUM;
    do {
        err = val_sdei_event_status(event_num, &res);
        if (err)
            break;
        if (!(res & EVENT_STATUS_RUNNING_BIT))
            break;
    } while (timeout--);

    g_handler_flag = 0;
}

static void event_signal_register(void *ignore)
{
    int32_t err;
    void *entry_point;
    uint32_t event_num;

    entry_point = &asm_event_handler;
    event_num = 0;
    /* Register the event zero */
    err = val_sdei_event_register(event_num, (uint64_t)entry_point,
                                                  (void *)event_signal_handler, 0, 0);
    if (err) {
        g_test_status = SDEI_TEST_FAIL;
        return;
    }

    err = val_sdei_event_enable(event_num);
    if (!err)
        return;

    g_test_status = SDEI_TEST_FAIL;
    val_sdei_event_unregister(event_num);
}

static void event_signal_unregister(void *ignore)
{
    uint32_t err;
    uint32_t event_num = 0;

    err = val_sdei_event_unregister(event_num);
    if (err) {
        g_test_status = SDEI_TEST_FAIL;
    }
}

static void test_entry(void) {
    uint32_t ns_wdg = 0;
    uint64_t timer_expire_ticks = 1, timeout;
    uint64_t wd_ctrl_base;
    void *entry_point;
    int32_t err;
    uint32_t event_num = 0;
    uint64_t res = 0;
    uint64_t int_id = 0;

    g_handler_flag = 1;
    g_wd_num = val_wd_get_info(0, WD_INFO_COUNT);
    entry_point = &asm_event_handler;
    g_event.is_bound_irq = TRUE;
    g_event.type = SDEI_EVENT_TYPE_SHARED;

    val_pe_execute_on_all((void*)event_signal_register, 0);
    if (g_test_status & SDEI_TEST_FAIL) {
        val_print(ACS_LOG_ERR, "\n        SDEI event signal register failed");
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    do {
        /*Array index starts from 0, so subtract 1 from count*/
        g_wd_num--;

        /* Skip secure Watchdog */
        if (val_wd_get_info(g_wd_num, WD_INFO_ISSECURE))
            continue;

        ns_wdg++;
        timeout = TIMEOUT_MEDIUM;

        int_id = val_wd_get_info(g_wd_num, WD_INFO_GSIV);
        val_print(ACS_LOG_DEBUG, "\n        WS0 interrupt id: %lld", int_id);
        wd_ctrl_base = val_wd_get_info(g_wd_num, WD_INFO_CTRL_BASE);
        g_wd_addr = val_pa_to_va(wd_ctrl_base);

        err = val_gic_disable_interrupt(int_id);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        Interrupt %lld disable failed", int_id);
            g_test_status = SDEI_TEST_FAIL;
            goto unmap_va;
        }
        err = val_sdei_interrupt_bind(int_id, &g_event.event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SPI intr number %lld bind failed with err %d",
                                                                                    int_id, err);
            g_test_status = SDEI_TEST_FAIL;
            goto unmap_va;
        }
        val_pe_data_cache_clean_invalidate((uint64_t)&g_event.event_num);
        /* Check 1 - Handler-Unregistered State */
        err = val_sdei_event_signal(event_num, val_pe_get_mpid_index(val_pe_get_index()));
        if (err) {
            val_print(ACS_LOG_ERR, "\n        Handler-Unregistered state check failed with err %d",
                                                                                               err);
            g_test_status = SDEI_TEST_FAIL;
            goto unmap_va;
        }
        while (timeout--) {
            if (!g_handler_flag)
                break;
        }
        if (g_handler_flag) {
            g_test_status = SDEI_TEST_FAIL;
            goto unmap_va;
        }
        timeout = TIMEOUT_MEDIUM;
        do {
            err = val_sdei_event_status(event_num, &res);
            if (err)
                val_print(ACS_LOG_ERR, "\n        SDEI event %d status failed err %x",
                                                                        event_num, err);
            if (!(res & EVENT_STATUS_RUNNING_BIT))
                break;
        } while (timeout--);

        g_handler_flag = 1;
        timeout = TIMEOUT_MEDIUM;
        err = val_sdei_event_register(g_event.event_num, (uint64_t)entry_point,
                                    (void *)event_handler, SDEI_EVENT_REGISTER_RM_PE,
                                    val_pe_get_mpid_index(get_secondary_pe_index()));
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SDEI evt %d register fail with err %x",
                                                                g_event.event_num, err);
            g_test_status = SDEI_TEST_FAIL;
            goto interrupt_release;
        }

        /* Check 2 - Handler-Registered State */
        err = val_sdei_event_signal(event_num, val_pe_get_mpid_index(val_pe_get_index()));
        if (err) {
            val_print(ACS_LOG_ERR, "\n        Handler-Registered state check failed with err %d",
                                                                                            err);
            g_test_status = SDEI_TEST_FAIL;
            goto event_unregister;
        }
        while (timeout--) {
            if (!g_handler_flag)
                break;
        }
        if (g_handler_flag) {
            g_test_status = SDEI_TEST_FAIL;
            goto event_unregister;
        }
        timeout = TIMEOUT_MEDIUM;
        do {
            err = val_sdei_event_status(event_num, &res);
            if (err)
                val_print(ACS_LOG_ERR, "\n        SDEI event %d status failed err %x",
                                                                            event_num, err);
            if (!(res & EVENT_STATUS_RUNNING_BIT))
                break;
        } while (timeout--);

        g_handler_flag = 1;
        timeout = TIMEOUT_MEDIUM;
        err = val_sdei_event_enable(g_event.event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event enable failed with err %d", err);
            g_test_status = SDEI_TEST_FAIL;
            goto event_unregister;
        }

        /* Check 3 - Handler-Enabled State */
        err = val_sdei_event_signal(event_num, val_pe_get_mpid_index(val_pe_get_index()));
        if (err) {
            val_print(ACS_LOG_ERR, "\n        Handler-Enabled state check failed with err %d", err);
            g_test_status = SDEI_TEST_FAIL;
            goto event_unregister;
        }
        while (timeout--) {
            val_pe_data_cache_invalidate((uint64_t)&g_handler_flag);
            if (!g_handler_flag)
                break;
        }
        if (g_handler_flag) {
            g_test_status = SDEI_TEST_FAIL;
            goto event_unregister;
        }
        timeout = TIMEOUT_MEDIUM;
        do {
            err = val_sdei_event_status(event_num, &res);
            if (err)
                val_print(ACS_LOG_ERR, "\n        SDEI event %d status failed err %x",
                                                                        event_num, err);
            if (!(res & EVENT_STATUS_RUNNING_BIT))
                break;
        } while (timeout--);

        g_handler_flag = 1;
        g_main_pe = val_pe_get_index();
        val_pe_data_cache_clean_invalidate((uint64_t)&g_main_pe);
        timeout = WD_TIME_OUT;
        val_wd_set_ws0(g_wd_addr, g_wd_num, timer_expire_ticks);
        while (timeout--) {
            if (g_handler_flag == 0)
                break;
        }
        if (g_handler_flag) {
            val_print(ACS_LOG_ERR, "\n        Watchdog interrupt trigger failed");
            val_wd_set_ws0(g_wd_addr, g_wd_num, 0);
            g_test_status = SDEI_TEST_FAIL;
            goto event_unregister;
        }
    } while (0);

    if (!ns_wdg) {
        g_test_status = SDEI_TEST_FAIL;
        val_print(ACS_LOG_ERR, "\n        No non-secure Watchdogs reported");
        return;
    }

    timeout = TIMEOUT_MEDIUM;
    do {
        err = val_sdei_event_status(g_event.event_num, &res);
        if (err)
            val_print(ACS_LOG_ERR, "\n        SDEI event %d status failed err %x",
                                                                g_event.event_num, err);
        if (!(res & EVENT_STATUS_RUNNING_BIT))
            break;
    } while (timeout--);

    if ((res & EVENT_STATUS_REGISTER_BIT)) {
        g_test_status = SDEI_TEST_ERROR;
        goto event_unregister;
    }
    else {
        err = val_sdei_interrupt_release(g_event.event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        Event num %d release failed : err %d",
                                                                g_event.event_num, err);
            g_test_status = SDEI_TEST_ERROR;
        }
        goto unmap_va;
    }

event_unregister:
    err = val_sdei_event_unregister(g_event.event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event %d unregister failed : err %x",
                                                                g_event.event_num, err);
    }
interrupt_release:
    err = val_sdei_interrupt_release(g_event.event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Event number %d release failed with err %x",
                                                                 g_event.event_num, err);
    }
unmap_va:
    val_va_free(g_wd_addr);
    val_pe_execute_on_all((void*)event_signal_unregister, 0);
    val_test_pe_set_status(val_pe_get_index(),
                           ((g_test_status == SDEI_TEST_PASS) ? SDEI_TEST_PASS : SDEI_TEST_FAIL)
                           );
}

SDEI_SET_TEST_DEPS(test_042_deps, TEST_001_ID, TEST_002_ID);
SDEI_PUBLISH_TEST(test_042, TEST_042_ID, TEST_DESC, test_042_deps, test_entry, FALSE);

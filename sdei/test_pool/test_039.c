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

#define TEST_DESC "Verify always availability- SDEI_PRIVATE_RESET "

#define PPI_NUM 31

static volatile int32_t g_handler_flag = 1;
static uint64_t g_test_status= SDEI_TEST_PASS;
static struct sdei_event g_event;

/* Check the SDEI_PRIVATE_RESET API in below states
 *          1. Handler-Unregistered State
 *          2. Handler-registered State
 *          3. Handler-Enabled State
 *          4. Handler-Enabled & Running State
 *          5. Handler-Unregister Pending State
 */
static void event_handler(void)
{
    uint32_t err;
    uint32_t res = 0;
    uint64_t current_el;

    err = val_pe_reg_read(CurrentEL, &current_el);
    current_el = __EXTRACT_BITS(current_el, 2, 2);

    if (current_el == 1)
        val_timer_set_phy_el1(0);
    else if (current_el == 2)
        val_timer_set_phy_el2(0);

    /* Check 4 - Handler-Enabled & Running State */
    err = val_sdei_private_reset(&res);
    if (err != SDEI_STATUS_DENIED) {
        g_test_status = SDEI_TEST_FAIL;
        g_handler_flag = 0;
        return;
    }

    err = val_sdei_event_unregister(g_event.event_num);
    if (err != SDEI_STATUS_PENDING) {
        g_test_status = SDEI_TEST_FAIL;
        g_handler_flag = 0;
        return;
    }

    /* Check 5 - Handler-Unregister Pending State */
    err = val_sdei_private_reset(&res);
    if (err != SDEI_STATUS_DENIED) {
        g_test_status = SDEI_TEST_FAIL;
    }

    g_handler_flag = 0;
}

static void payload(void *ignore)
{
    uint64_t timer_expire_ticks = 1;
    uint64_t timeout;
    uint64_t res = 0;
    uint32_t err;
    uint64_t current_el;
    void *entry_point;

    g_handler_flag = 1;
    g_test_status = SDEI_TEST_PASS;
    timeout = WD_TIME_OUT;
    entry_point = &asm_event_handler;

    /* Check 1 - Handler-Unregistered State */
    err = val_sdei_private_reset(&res);
    if (err) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    err = val_sdei_event_register(g_event.event_num, (uint64_t)entry_point,
                                                                (void *)event_handler, 0, 0);
    if (err) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    /* Check 2 - Handler-Registered State */
    err = val_sdei_private_reset(&res);
    if (err) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_unregister;
    }

    err = val_sdei_event_register(g_event.event_num, (uint64_t)entry_point,
                                                                    (void *)event_handler, 0, 0);
    if (err) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }
    err = val_sdei_event_enable(g_event.event_num);
    if (err) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_unregister;
    }

    /* Check 3 - Handler-Enabled State */
    err = val_sdei_private_reset(&res);
    if (err) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_unregister;
    }

    err = val_sdei_event_register(g_event.event_num, (uint64_t)entry_point,
                                                                    (void *)event_handler, 0, 0);
    if (err) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }
    err = val_sdei_event_enable(g_event.event_num);
    if (err) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_unregister;
    }

    err = val_pe_reg_read(CurrentEL, &current_el);
    current_el = __EXTRACT_BITS(current_el, 2, 2);
    /* Generate the timer interrupt based on EL state */
    if (current_el == 1)
        val_timer_set_phy_el1(timer_expire_ticks);
    else if (current_el == 2)
        val_timer_set_phy_el2(timer_expire_ticks);

    while (g_handler_flag && timeout--);
    if (g_handler_flag) {
        if (current_el == 1)
            val_timer_set_phy_el1(0);
        else if (current_el == 2)
            val_timer_set_phy_el2(0);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_unregister;
    }

    val_test_pe_set_status(val_pe_get_index(),
                           ((g_test_status == SDEI_TEST_PASS) ? SDEI_TEST_PASS : SDEI_TEST_FAIL));

    timeout = TIMEOUT_MEDIUM;
    do {
        err = val_sdei_event_status(g_event.event_num, &res);
        if (!(res & EVENT_STATUS_RUNNING_BIT) && !(res & EVENT_STATUS_REGISTER_BIT))
            return;
    } while (timeout--);

event_unregister:
    err = val_sdei_event_unregister(g_event.event_num);
}

static void test_entry(void) {
    int32_t err;
    uint64_t g_int_id = 0;
    uint64_t current_el;

    g_handler_flag = 1;
    g_event.is_bound_irq = TRUE;

    /* Read current EL state */
    err = val_pe_reg_read(CurrentEL, &current_el);
    current_el = __EXTRACT_BITS(current_el, 2, 2);

    /* Read the timer interrupt based on EL state */
    if (current_el == 1)
        g_int_id = val_timer_get_info(TIMER_INFO_PHY_EL1_INTID, 0);
    else if (current_el == 2)
        g_int_id = val_timer_get_info(TIMER_INFO_PHY_EL2_INTID, 0);

    if (g_int_id > PPI_NUM) {
        val_print(ACS_LOG_ERR, "\n        Incorrect PPI value %lld", g_int_id);
        return;
    }
    val_print(ACS_LOG_DEBUG, "\n        timer interrupt id: %lld", g_int_id);

    err = val_gic_disable_interrupt(g_int_id);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Interrupt %lld disable failed", g_int_id);
        val_test_set_status(val_pe_get_num(), SDEI_TEST_FAIL);
        return;
    }
    /* Bind the timer interrupt to event */
    err = val_sdei_interrupt_bind(g_int_id, &g_event.event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SPI intr number %lld bind failed with err %d",
                                                                            g_int_id, err);
        val_test_set_status(val_pe_get_num(), SDEI_TEST_FAIL);
        return;
    }

    val_pe_data_cache_clean_invalidate((uint64_t)&g_event.event_num);

    val_pe_execute_on_all((void*)payload, 0);

    /* Release the timer interrupt */
    err = val_sdei_interrupt_release(g_event.event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Event num %d release failed : err %d",
                                                            g_event.event_num, err);
    }
}

SDEI_SET_TEST_DEPS(test_039_deps, TEST_001_ID, TEST_002_ID);
SDEI_PUBLISH_TEST(test_039, TEST_039_ID, TEST_DESC, test_039_deps, test_entry, TRUE);

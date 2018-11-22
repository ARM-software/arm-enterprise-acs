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

#define TEST_DESC "Verify only 1 instance of shared event handled "

static int32_t g_wd_num;
static uint32_t g_index = 0;
static uint64_t *g_wd_va_addr = NULL;
static volatile uint64_t g_test_status = SDEI_TEST_PASS;
static volatile int32_t g_event_counter = 0;
static volatile int32_t g_handler_flag = 1;

static void wd_event_handler(void)
{
    uint64_t timeout = TIMEOUT_MEDIUM;

    val_wd_set_ws0(g_wd_va_addr, g_wd_num, 0);
    g_event_counter++;
    if (g_handler_flag) {
        while (timeout--) {
            if (g_event_counter == 2) {
                g_test_status = SDEI_TEST_FAIL;
                break;
            }
        }
    }
    g_handler_flag = 0;
    val_pe_data_cache_clean_invalidate((uint64_t)&g_handler_flag);
    val_pe_data_cache_clean_invalidate((uint64_t)&g_event_counter);
}

static void second_event_handler(void)
{
    uint64_t timeout = TIMEOUT_MEDIUM;

    val_disable_second_interrupt(g_index);
    g_event_counter++;
    if (g_handler_flag) {
        while (timeout--) {
            if (g_event_counter == 2) {
                g_test_status = SDEI_TEST_FAIL;
                break;
            }
        }
    }
    g_handler_flag = 0;
    val_pe_data_cache_clean_invalidate((uint64_t)&g_handler_flag);
    val_pe_data_cache_clean_invalidate((uint64_t)&g_event_counter);
}

static uint64_t wd_event_register(struct sdei_event *wd_event)
{
    uint32_t ns_wdg = 0;
    uint64_t wd_ctrl_base;
    uint64_t int_id = 0;
    void *entry_point;
    int32_t err;

    g_wd_num = val_wd_get_info(0, WD_INFO_COUNT);
    entry_point = &asm_event_handler;
    wd_event->is_bound_irq = TRUE;

    do {
        /* array index starts from 0, so subtract 1 from count */
        g_wd_num--;

        /* Skip Secure watchdog */
        if (val_wd_get_info(g_wd_num, WD_INFO_ISSECURE))
            continue;

        ns_wdg++;

        /* Read the Watchdog interrupt from info table */
        int_id = val_wd_get_info(g_wd_num, WD_INFO_GSIV);
        val_print(ACS_LOG_DEBUG, "\n        WS0 interrupt id: %lld", int_id);
        /* Read the Watchdog base address from info table */
        wd_ctrl_base = val_wd_get_info(g_wd_num, WD_INFO_CTRL_BASE);
        g_wd_va_addr = val_pa_to_va(wd_ctrl_base);

        err = val_gic_disable_interrupt(int_id);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        Interrupt %lld disable failed", int_id);
            g_test_status = SDEI_TEST_FAIL;
            goto unmap_va;
        }
        /* Bind Watchdog interrupt to event */
        err = val_sdei_interrupt_bind(int_id, &wd_event->event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SPI intr number %lld bind failed with err %d",
                                                                                int_id, err);
            g_test_status = SDEI_TEST_FAIL;
            goto unmap_va;
        }

        err = val_sdei_event_register(wd_event->event_num, (uint64_t)entry_point,
                                     (void *)wd_event_handler, SDEI_EVENT_REGISTER_RM_PE,
                                     val_pe_get_mpid_index(0));
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event %d register fail with err %x",
                                                                wd_event->event_num, err);
            g_test_status = SDEI_TEST_FAIL;
            goto interrupt_release;
        }

        err = val_sdei_event_enable(wd_event->event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event enable failed with err %d", err);
            g_test_status = SDEI_TEST_FAIL;
            goto event_unregister;
        }
    } while (0);

    if (!ns_wdg) {
        g_test_status = SDEI_TEST_FAIL;
        val_print(ACS_LOG_ERR, "\n        No non-secure Watchdogs reported");
        return g_test_status;
    }

    return g_test_status;

event_unregister:
    err = val_sdei_event_unregister(wd_event->event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event %d unregister fail with err %x",
                                                                wd_event->event_num, err);
    }
interrupt_release:
    err = val_sdei_interrupt_release(wd_event->event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Event number %d release failed with err %x",
                                                                 wd_event->event_num, err);
    }
unmap_va:
    val_va_free(g_wd_va_addr);
    return g_test_status;
}

static uint64_t second_event_register(struct sdei_event *second_event)
{
    uint64_t int_id;
    uint32_t err;
    void *entry_point;

    entry_point = &asm_event_handler;
    second_event->is_bound_irq = TRUE;

    /* To generate second event, here Platform timer interrupt is used in this routine */
    err = val_configure_second_interrupt(&g_index, &int_id);
    val_pe_data_cache_clean_invalidate((uint64_t)&g_index);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Second interrupt enable failed");
        return SDEI_TEST_FAIL;
    }
    val_print(ACS_LOG_DEBUG, "\n        Found second source intid %llx", int_id);

    err = val_gic_disable_interrupt(int_id);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Interrupt %lld disable failed", int_id);
        return SDEI_TEST_FAIL;
    }
    /* Bind second source interrupt to event */
    err = val_sdei_interrupt_bind(int_id, &second_event->event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SPI intr number %lld bind failed with err %d",
                                                                            int_id, err);
        return SDEI_TEST_FAIL;
    }

    err = val_sdei_event_register(second_event->event_num, (uint64_t)entry_point,
                                 (void *)second_event_handler, SDEI_EVENT_REGISTER_RM_PE,
                                 val_pe_get_mpid_index(0));
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event %d register fail with err %x",
                                                            second_event->event_num, err);
        goto interrupt_release;
    }

    err = val_sdei_event_enable(second_event->event_num);
    if (!err) {
        return g_test_status;
    }

    val_print(ACS_LOG_ERR, "\n        SDEI event enable test failed with err %d", err);
    err = val_sdei_event_unregister(second_event->event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event %d unregister fail with err %x",
                                                                second_event->event_num, err);
    }
interrupt_release:
    err = val_sdei_interrupt_release(second_event->event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Event number %d release failed with err %x",
                                                                 second_event->event_num, err);
    }
    return SDEI_TEST_FAIL;
}

static void test_entry(void) {
    uint64_t timer_expire_ticks = 100, timeout;
    uint32_t err;
    uint64_t result1, result2;
    struct sdei_event wd_event;
    struct sdei_event second_event;

    g_event_counter = 0;
    g_handler_flag = 1;
    timeout = WD_TIME_OUT;

    /* Register and Enable the Watchdog event */
    g_test_status = wd_event_register(&wd_event);
    if (g_test_status & SDEI_TEST_FAIL) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    /* Register and Enable the second event, here Platform timer used as a second event  */
    g_test_status = second_event_register(&second_event);
    if (g_test_status & SDEI_TEST_FAIL) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_unregister1;
    }

    /* Generate Watchdog event */
    val_wd_set_ws0(g_wd_va_addr, g_wd_num, timer_expire_ticks);
    /* Generate second event */
    val_generate_second_interrupt(g_index, timer_expire_ticks);

    while (timeout--) {
        val_pe_data_cache_invalidate((uint64_t)&g_event_counter);
        if (g_event_counter == 2)
            break;
    }
    if (g_event_counter != 2) {
        val_print(ACS_LOG_ERR, "\n        Event trigger failed");
        val_wd_set_ws0(g_wd_va_addr, g_wd_num, 0);
        val_disable_second_interrupt(g_index);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_unregister2;
    }

    /* Handler Running False Check */
    timeout = TIMEOUT_MEDIUM;
    do {
        err = val_sdei_event_status(wd_event.event_num, &result1);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event status failed with err %d", err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            goto event_unregister2;
        }
        err = val_sdei_event_status(second_event.event_num, &result2);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event status failed with err %d", err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            goto event_unregister2;
        }

        if (!(result1 & EVENT_STATUS_RUNNING_BIT) && !(result2 & EVENT_STATUS_RUNNING_BIT))
            break;
    } while (timeout--);

    if (g_test_status & SDEI_TEST_FAIL)
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
    else
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);

event_unregister2:
    err = val_sdei_event_unregister(second_event.event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event %d unregister fail with err %x",
                                                     second_event.event_num, err);
    }
    err = val_sdei_interrupt_release(second_event.event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Event number %d release failed with err %x",
                                                     second_event.event_num, err);
    }
event_unregister1:
    err = val_sdei_event_unregister(wd_event.event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event %d unregister fail with err %x",
                                                            wd_event.event_num, err);
    }
    err = val_sdei_interrupt_release(wd_event.event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Event number %d release failed with err %x",
                                                             wd_event.event_num, err);
    }
    val_va_free(g_wd_va_addr);
}

SDEI_SET_TEST_DEPS(test_043_deps, TEST_001_ID, TEST_002_ID);
SDEI_PUBLISH_TEST(test_043, TEST_043_ID, TEST_DESC, test_043_deps, test_entry, FALSE);

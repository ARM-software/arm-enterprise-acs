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

#define TEST_DESC "Verify clients intr cannot pre-empt handler    "
#define RUNNING_BIT     0x4

static int32_t g_wd_num;
static uint64_t *g_wd_addr = NULL;
static volatile int32_t g_handler_flag = 1;
static volatile int32_t g_handler_counter = 0;
static volatile uint64_t g_status = SDEI_TEST_PASS;

static int client_isr_handler(void)
{
    /* If interrupt is preempted event handler, assign status to TEST_FAIL */
    if (g_handler_flag)
        g_status = SDEI_TEST_FAIL;

    g_handler_counter++;
    val_pe_data_cache_clean_invalidate((uint64_t)&g_handler_counter);
    return 0;
}

/* Generate Interrupt in Event handler to check the preemption */
static void event_handler(void)
{
    uint32_t timeout = TIMEOUT_MEDIUM;

    val_wd_set_ws0(g_wd_addr, g_wd_num, 0);
    /* Generate the SPI interrupt */
    val_gic_generate_interrupt(SPI_INTR_NUM1);
    while (timeout--);
    g_handler_flag = 0;
    g_handler_counter++;
    val_pe_data_cache_clean_invalidate((uint64_t)&g_handler_flag);
    val_pe_data_cache_clean_invalidate((uint64_t)&g_handler_counter);
}

static void test_entry(void) {
    uint32_t ns_wdg = 0;
    uint64_t int_id = 0;
    uint64_t timer_expire_ticks = 1, timeout;
    uint64_t wd_ctrl_base;
    uint64_t res = 0;
    struct sdei_event event;
    int32_t err;

    g_handler_flag = 1;
    g_handler_counter = 0;
    g_wd_num = val_wd_get_info(0, WD_INFO_COUNT);
    event.is_bound_irq = TRUE;
    event.type = SDEI_EVENT_TYPE_SHARED;

    do {
        /* array index starts from 0, so subtract 1 from count */
        g_wd_num--;

        /* Skip Secure watchdog */
        if (val_wd_get_info(g_wd_num, WD_INFO_ISSECURE))
            continue;

        ns_wdg++;
        timeout = WD_TIME_OUT;

        /* Read Watchdog interrupt from Watchdog info table */
        int_id = val_wd_get_info(g_wd_num, WD_INFO_GSIV);
        val_print(ACS_LOG_DEBUG, "\n        WS0 interrupt id: %lld", int_id);
        /* Read Watchdog base address from Watchdog info table */
        wd_ctrl_base = val_wd_get_info(g_wd_num, WD_INFO_CTRL_BASE);
        g_wd_addr = val_pa_to_va(wd_ctrl_base);

        err = val_gic_disable_interrupt(int_id);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        Interrupt %lld disable failed", int_id);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            goto unmap_va;
        }
        /* Bind Watchdog interrupt to event */
        err = val_sdei_interrupt_bind(int_id, &event.event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SPI interrupt number %lld bind failed with err %d",
                                                                                int_id, err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            goto unmap_va;
        }

        err = val_sdei_event_register(event.event_num, (uint64_t)asm_event_handler,
                                     (void *)event_handler, SDEI_EVENT_REGISTER_RM_PE,
                                     val_pe_get_mpid_index(0));
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

        /* Register SPI Interrupt line */
        err = val_gic_install_isr(SPI_INTR_NUM1, client_isr_handler);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        Interrupt register %x failed with err %d",
                                                                            SPI_INTR_NUM1, err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            goto event_unregister;
        }

        /* Route SPI interrupt to PE zero */
        val_gic_route_interrupt_to_pe(SPI_INTR_NUM1, val_pe_get_mpid_index(0));

        /* Generate Watchdog interrupt */
        val_wd_set_ws0(g_wd_addr, g_wd_num, timer_expire_ticks);

        while (g_handler_flag && timeout--) {
            val_pe_data_cache_invalidate((uint64_t)&g_handler_flag);
            val_pe_data_cache_invalidate((uint64_t)&g_handler_counter);
            if (g_handler_flag == 0 && (g_handler_counter == 2))
                break;
        }
        if (g_handler_flag || (g_handler_counter != 2)) {
            val_print(ACS_LOG_ERR, "\n        Watchdog interrupt trigger failed");
            val_wd_set_ws0(g_wd_addr, g_wd_num, 0);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            goto free_interrupt;
        }

        if (g_status & SDEI_TEST_FAIL) {
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            goto free_interrupt;
        }

    } while (0);

    if (!ns_wdg) {
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        val_print(ACS_LOG_ERR, "\n        No non-secure Watchdogs reported");
        return;
    }

    val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);

free_interrupt:
    val_gic_end_of_interrupt(SPI_INTR_NUM1);
    val_gic_free_interrupt(SPI_INTR_NUM1);
event_unregister:
    timeout = TIMEOUT_MEDIUM;
    do {
        err = val_sdei_event_status(event.event_num, &res);
        if (err)
            val_print(ACS_LOG_ERR, "\n        SDEI event %d status failed err %x",
                                                                event.event_num, err);
         if (!(res & RUNNING_BIT))
             break;
    } while (timeout--);

    err = val_sdei_event_unregister(event.event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event %d unregister failed with err %x",
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

SDEI_SET_TEST_DEPS(test_029_deps, TEST_001_ID, TEST_002_ID);
SDEI_PUBLISH_TEST(test_029, TEST_029_ID, TEST_DESC, test_029_deps, test_entry, FALSE);

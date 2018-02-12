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

#define TEST_DESC "Verify SDEI event get info test                "

#define SDEI_UNKNOWN_EVENT 0x10000

static void payload(void)
{
    int32_t err, cpu_id =2;
    uint64_t result;
    struct sdei_event event;

    /* Verifying Event Get info for Private event */
    event.event_num = val_event_get(SDEI_EVENT_TYPE_PRIVATE, SDEI_EVENT_PRIORITY_ANY);
    if (event.event_num) {
        event.is_bound_irq = FALSE;
        err = val_sdei_event_get_info(event.event_num, SDEI_EVENT_INFO_EV_TYPE,
                            &result);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event get info failed with err %x", err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        }
        else if (result == SDEI_EVENT_TYPE_PRIVATE) {
            val_print(ACS_LOG_INFO, "\n        SDEI event %d type is private", event.event_num);
        }
        else {
            val_print(ACS_LOG_ERR, "\n        SDEI event get info failed");
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        }

        /* Testing software signal feature using Event Get info*/
        err = val_sdei_event_get_info(event.event_num, SDEI_EVENT_INFO_EV_SIGNALED,
                            &result);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event get info failed with err %x", err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        }
        else if (result) {
            val_print(ACS_LOG_INFO, "\n        SDEI event %d cannot be software signalled",
                                                                            event.event_num);
        }
        else {
            val_print(ACS_LOG_INFO, "\n        SDEI event %d can be software signalled",
                                                                            event.event_num);
        }

        /* Testing event priority feature using Event Get info*/
        err = val_sdei_event_get_info(event.event_num, SDEI_EVENT_INFO_EV_PRIORITY,
                            &result);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SDEI event get info failed with err %x", err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        }
        else if (result) {
            val_print(ACS_LOG_INFO, "\n        SDEI event %d is critical event", event.event_num);
        }
        else {
            val_print(ACS_LOG_INFO, "\n        SDEI event %d is normal event", event.event_num);
        }

        /* Testing event routing feature using Event Get info*/
        err = val_sdei_event_get_info(event.event_num, SDEI_EVENT_INFO_EV_ROUTING_MODE,
                            &result);
        if (err != SDEI_STATUS_INVALID) {
            val_print(ACS_LOG_ERR, "\n        SDEI event get info failed with err %x", err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        }

        /* Testing event routing affinity feature using Event Get info*/
        err = val_sdei_event_get_info(event.event_num, SDEI_EVENT_INFO_EV_ROUTING_AFF,
                            &result);
        if (err != SDEI_STATUS_INVALID) {
            val_print(ACS_LOG_ERR, "\n        SDEI event get info failed with err %x", err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        }
    } else
        val_print(ACS_LOG_TEST, "\n        SDEI private event not found");

    /* Verifying Event Get info for Shared event */
    event.event_num = val_event_get(SDEI_EVENT_TYPE_SHARED, SDEI_EVENT_PRIORITY_ANY);
    event.is_bound_irq = FALSE;
    err = val_sdei_event_register(event.event_num, (uint64_t)asm_event_handler, NULL, 0, 0);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event %d register failed with err %x",
                                                                    event.event_num, err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    err = val_sdei_event_routing_set(event.event_num, TRUE, cpu_id);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event routing set failed with err %x", err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_unregister;
    }

    /* Testing event type feature using Event Get info*/
    err = val_sdei_event_get_info(event.event_num, SDEI_EVENT_INFO_EV_TYPE,
                        &result);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event get info failed with err %x", err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }
    else if (result == SDEI_EVENT_TYPE_SHARED) {
        val_print(ACS_LOG_INFO, "\n        SDEI event %d type is shared", event.event_num);
    }
    else {
        val_print(ACS_LOG_ERR, "\n        SDEI event get info failed");
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    /* Testing event routing mode feature using Event Get info*/
    err = val_sdei_event_get_info(event.event_num, SDEI_EVENT_INFO_EV_ROUTING_MODE,
                        &result);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event get info failed with err %x", err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_unregister;
    }
    else if (result == SDEI_EVENT_REGISTER_RM_PE) {
        val_print(ACS_LOG_INFO, "\n        SDEI event %d routing mode is RM_PE", event.event_num);
    }
    else {
        val_print(ACS_LOG_ERR, "\n        SDEI event get info failed");
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_unregister;
    }

    /* Testing event routing affinity feature using Event Get info*/
    err = val_sdei_event_get_info(event.event_num, SDEI_EVENT_INFO_EV_ROUTING_AFF,
                        &result);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event get info failed with err %x", err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_unregister;
    }
    else {
        val_print(ACS_LOG_INFO, "\n        SDEI event %d affinity is %llx",
                                                            event.event_num ,result);
        err = val_sdei_event_unregister(event.event_num);
        if (err) {
            val_print(ACS_LOG_WARN, "\n        SDEI event %d unregister failed with err %x",
                                                                            event.event_num, err);
        }
    }

    event.event_num = val_event_get(SDEI_EVENT_TYPE_SHARED, SDEI_EVENT_PRIORITY_ANY);
    /* Testing DENIED error code for Event Get info*/
    err = val_sdei_event_get_info(event.event_num, SDEI_EVENT_INFO_EV_ROUTING_MODE,
                        &result);
    if (err != SDEI_STATUS_DENIED) {
        val_print(ACS_LOG_ERR, "\n        SDEI event get info failed with err %x", err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    err = val_sdei_event_get_info(event.event_num, SDEI_EVENT_INFO_EV_ROUTING_AFF,
                        &result);
    if (err != SDEI_STATUS_DENIED) {
        val_print(ACS_LOG_ERR, "\n        SDEI event get info failed with err %x", err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    event.event_num = 0;
    event.is_bound_irq = FALSE;
    /* Testing software signal feature using Event Get info for event number zero(0) */
    err = val_sdei_event_get_info(event.event_num, SDEI_EVENT_INFO_EV_SIGNALED,
                        &result);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event get info failed with err %x", err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }
    else if (result == 0) {
        val_print(ACS_LOG_INFO, "\n        SDEI event %d can be software signalled",
                                                                        event.event_num);
    }
    else {
        val_print(ACS_LOG_ERR, "\n        SDEI event get info failed");
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    err = val_gic_disable_interrupt(SPI_INTR_NUM);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Interrupt %d disable failed", SPI_INTR_NUM);
        return;
    }
    err = val_sdei_interrupt_bind(SPI_INTR_NUM, &event.event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SPI interrupt number %d bind failed with err %d",
                                                                                SPI_INTR_NUM, err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }
    event.is_bound_irq = TRUE;
    /* Testing EV_PRIORITY for binded interrupt using Get info */
    err = val_sdei_event_get_info(event.event_num, SDEI_EVENT_INFO_EV_PRIORITY,
                        &result);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event get info failed with err %x", err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_interrupt_release;
    }
    else if (result == 0) {
        val_print(ACS_LOG_INFO, "\n        SDEI event %d is normal priority event",
                                                                    event.event_num);
        err = val_sdei_interrupt_release(event.event_num);
        if (err) {
            val_print(ACS_LOG_WARN, "\n        Event number %d intrrupt release failed with err %d",
                                                                              event.event_num, err);
        }
    }
    else {
        val_print(ACS_LOG_ERR, "\n        SDEI event get info failed");
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        goto event_interrupt_release;
    }

    /* Verifying SDEI GET INFO for invalid event number */
    event.event_num = SDEI_UNKNOWN_EVENT;
    err = val_sdei_event_get_info(event.event_num, SDEI_EVENT_INFO_EV_ROUTING_AFF,
                        &result);
    if (err != SDEI_STATUS_INVALID) {
        val_print(ACS_LOG_ERR, "\n        SDEI event get info failed with err %x", err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);
    return;

event_unregister:
    err = val_sdei_event_unregister(event.event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SDEI event %d unregister failed with err %x",
                                                                    event.event_num, err);
    }
event_interrupt_release:
    if (event.is_bound_irq) {
        err = val_sdei_interrupt_release(event.event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        Event number %d intrrupt release failed with err %d",
                                                                              event.event_num, err);
        }
    }
}

static void test_entry(void)
{
    payload();
}

SDEI_SET_TEST_DEPS(test_012_deps, TEST_001_ID, TEST_002_ID);
SDEI_PUBLISH_TEST(test_012, TEST_012_ID, TEST_DESC, test_012_deps, test_entry, FALSE);

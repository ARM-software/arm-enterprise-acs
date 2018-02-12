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

#define TEST_DESC "Verify SDEI interrupt bind test (SPI,PPI, SGI)  "

/* This function verifies the shared and private interrupt binding to event.
 * it also checks sdei interrupt bind error checks.
 */
static void payload(void)
{
    uint32_t event_num, new_event_num;
    int32_t err;

    /* Interrupt bind test for SPI */
    err = val_gic_disable_interrupt(SPI_INTR_NUM);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Interrupt %d disable failed", SPI_INTR_NUM);
        return;
    }
    err = val_sdei_interrupt_bind(SPI_INTR_NUM, &event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        SPI interrupt number %d bind failed with err %d",
                                                                                SPI_INTR_NUM, err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    } else {
        err = val_sdei_interrupt_bind(SPI_INTR_NUM, &new_event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SPI interrupt number %d bind failed with err %d",
                                                                                SPI_INTR_NUM, err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        } else {
            if (event_num != new_event_num) {
                val_print(ACS_LOG_ERR, "\n        SPI interrupt number %d re-bind test"
                                                  "failed with err %d\n", SPI_INTR_NUM, err);
                val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
                return;
            }
        }
    }
    err = val_sdei_interrupt_release(event_num);
    if (err) {
        val_print(ACS_LOG_WARN, "\n        Event number %d intrrupt release failed with err %d",
                                                                            event_num, err);
    }

    /* Interrupt bind test for PPI */
    err = val_gic_disable_interrupt(PPI_INTR_NUM);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Interrupt %d disable failed", PPI_INTR_NUM);
        return;
    }
    err = val_sdei_interrupt_bind(PPI_INTR_NUM, &event_num);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        PPI interrupt number %d bind failed with err %d",
                                                                                PPI_INTR_NUM, err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    } else {
        err = val_sdei_interrupt_bind(PPI_INTR_NUM, &new_event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        PPI interrupt number %d bind failed with err %d",
                                                                                PPI_INTR_NUM, err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        } else {
            if (event_num != new_event_num) {
                val_print(ACS_LOG_ERR, "\n        PPI interrupt number %d re-bind test"
                                       "failed with err %d", PPI_INTR_NUM, err);
                val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
                return;
            }
        }
    }
    err = val_sdei_interrupt_release(event_num);
    if (err) {
        val_print(ACS_LOG_WARN, "\n        Event number %d intrrupt release failed with err %d",
                                                                        event_num, err);
    }

    /* Interrupt bind test for SGI */
    err = val_gic_disable_interrupt(SGI_INTR_NUM);
    if (err) {
        val_print(ACS_LOG_ERR, "\n        Interrupt %d disable failed", SGI_INTR_NUM);
        return;
    }
    err = val_sdei_interrupt_bind(SGI_INTR_NUM, &event_num);
    if (err != SDEI_STATUS_INVALID) {
        val_print(ACS_LOG_ERR, "\n        SGI interrupt number %d bind failed", SGI_INTR_NUM);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);
}

static void test_entry(void)
{
    payload();
}

SDEI_SET_TEST_DEPS(test_004_deps, TEST_NONE_ID);
SDEI_PUBLISH_TEST(test_004, TEST_004_ID, TEST_DESC, test_004_deps, test_entry, FALSE);

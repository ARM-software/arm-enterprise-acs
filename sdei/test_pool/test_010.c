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

#define TEST_DESC "Verify SDEI interrupt release test for SPI,PPI "

static void test_entry(void)
{
    uint32_t event_num;
    int32_t err;

    /* Interrupt release test for SPI */
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
        err = val_sdei_interrupt_release(event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        SPI event number %d release failed with err %d",
                                                                                   event_num, err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        }
    }

    /* Interrupt relase test for PPI */
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
        err = val_sdei_interrupt_release(event_num);
        if (err) {
            val_print(ACS_LOG_ERR, "\n        PPI event number %d release failed with err %d",
                                                                                    event_num, err);
            val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
            return;
        }
    }

    /* Interrupt Release test for unbounded event */
    err = val_sdei_interrupt_release(event_num);
    if (err != SDEI_STATUS_INVALID) {
        val_print(ACS_LOG_ERR, "\n        Interrupt release failed for unbounded event %d"
                                                                    "with err %d", event_num, err);
        val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_FAIL);
        return;
    }

    val_test_pe_set_status(val_pe_get_index(), SDEI_TEST_PASS);
}

SDEI_SET_TEST_DEPS(test_010_deps, TEST_001_ID);
SDEI_PUBLISH_TEST(test_010, TEST_010_ID, TEST_DESC, test_010_deps, test_entry, FALSE);

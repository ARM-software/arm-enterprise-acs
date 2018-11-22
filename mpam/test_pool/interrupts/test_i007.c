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

#include "val/include/val_infra.h"
#include "val/include/val_cache.h"
#include "val/include/val_memory.h"
#include "val/include/val_node_infra.h"


#define TEST_NUM   ACS_INTR_TEST_NUM_BASE  +  7
#define TEST_DESC  "Check MSMON config ID out-of-range error"

static uint8_t node_type;
static uint32_t node_index;
static uint32_t intr_num;

static void intr_handler()
{
    uint32_t pe_index = val_pe_get_index_mpid(val_pe_get_mpid());

    val_print(ACS_PRINT_DEBUG, "\n       Received MSMON oor Err interrupt %d     ", intr_num);
    val_set_status(pe_index, RESULT_PASS(TEST_NUM, 01));

    /* Write 0b0000 into MPAMF_ESR.ERRCODE to clear the interrupt */
    val_node_clear_intr(node_type, node_index);

    /* Send EOI to the CPU Interface */
    val_gic_end_of_interrupt(intr_num);
}

static void payload()
{

    uint16_t mon_count;
    uint32_t index;
    uint32_t pe_index;
    uint32_t total_nodes;
    uint32_t timeout;
    uint32_t intr_count = 0;

    pe_index = val_pe_get_index_mpid(val_pe_get_mpid());
    total_nodes = val_node_get_total(MPAM_NODE_CACHE) +
                      val_node_get_total(MPAM_NODE_MEMORY);

    for (index = 0; index < total_nodes; index++) {

        node_type = MPAM_NODE_CACHE;
        node_index = index;

        if (index >= val_node_get_total(MPAM_NODE_CACHE)) {
            node_type = MPAM_NODE_MEMORY;
            node_index = index - val_node_get_total(MPAM_NODE_CACHE);
        }

        intr_num = val_node_get_error_intrnum(node_type, node_index);

        /* Read the number of monitors implemented in this MSC */
        if (val_node_supports_mon(node_type, node_index)) {

            switch (node_type) {
                case MPAM_NODE_CACHE:
                    mon_count = val_cache_supports_csumon(node_index) ?
                                 val_cache_mon_count(node_index) : 0;
                    break;
                case MPAM_NODE_MEMORY:
                    mon_count = val_memory_supports_mbwumon(node_index) ?
                                 val_memory_mon_count(node_index) : 0;
                    break;
                default:
                     mon_count = 0;
                     break;
            }
        } else {
            mon_count = 0;
        }

        /*
         * Skip this MSC if it doesn't implement error interrupt
         * support (or) if it doesn't implement any monitors
         */
        if ((intr_num == 0) || (mon_count == 0)) {
            continue;
        } else {
            intr_count++;
        }

        /* Register the interrupt handler */
        if (val_gic_install_isr(intr_num, intr_handler) == ACS_STATUS_ERR) {
            val_set_status(pe_index, RESULT_FAIL(TEST_NUM, 02));
            return;
        }

        /* Enable affinity routing to receive intr_num on primary PE */
        val_gic_route_interrupt_to_pe(intr_num, val_pe_get_mpid_index(pe_index));

        /* Set the interrupt status to pending */
        val_set_status(pe_index, RESULT_PENDING(TEST_NUM));

        /* Generate MSMON config ID out-of-range error */
        val_node_generate_msmon_config_error(node_type, node_index, mon_count);
        val_gic_write_ispendreg(intr_num);

        /* PE busy polls to check the completion of interrupt service routine */
        timeout = TIMEOUT_LARGE;
        while ((--timeout > 0) && (IS_RESULT_PENDING(val_get_status(pe_index))));

        /* Restore Error Control Register original settings */
        val_node_restore_ecr(node_type, node_index);

        if (timeout == 0) {
            val_print(ACS_PRINT_ERR, "\n MSC MSMON oor Err Interrupt not received on %d   ", intr_num);
            val_set_status(pe_index, RESULT_FAIL(TEST_NUM, 02));
            return;
        }

    }

    /* Set the test status to Skip if none of the MPAM nodes implement error interrupts */
    if (intr_count == 0) {
        val_set_status(pe_index, RESULT_SKIP(TEST_NUM, 0));
        return;
    }

    val_set_status(pe_index, RESULT_PASS(TEST_NUM, 01));
    return;
}

uint32_t testi007_entry()
{

    uint32_t status = ACS_STATUS_FAIL;
    uint32_t num_pe = 1;

    status = val_initialize_test(TEST_NUM, TEST_DESC, num_pe);

    /* This check is when user is forcing us to skip this test */
    if (status != ACS_STATUS_SKIP)
        val_run_test_payload(TEST_NUM, num_pe, payload, 0);

    /* get the result from all PE and check for failure */
    status = val_check_for_error(TEST_NUM, num_pe);

    val_report_status(0, ACS_TEST_END(TEST_NUM));

    return status;
}

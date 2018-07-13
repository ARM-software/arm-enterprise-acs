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
#include "val/include/val_mpam_hwreg_defs.h"


#define TEST_NUM   ACS_INTR_TEST_NUM_BASE  +  8
#define TEST_DESC  "Check MBWU monitor overflow interrupt functionality"

static uint32_t node_index;
static uint32_t intr_num;
static uint64_t mpam2_el2_temp;

static void intr_handler()
{
    uint32_t pe_index = val_pe_get_index_mpid(val_pe_get_mpid());

    val_print(ACS_PRINT_DEBUG, "\n       Received Oflow error interrupt %d     ", intr_num);
    val_set_status(pe_index, RESULT_PASS(TEST_NUM, 01));

    /* Write 0b0000 into MPAMF_ESR.ERRCODE to clear the interrupt */
    val_node_clear_intr(MPAM_NODE_MEMORY, node_index);

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
    uint64_t mpam2_el2;

    pe_index = val_pe_get_index_mpid(val_pe_get_mpid());
    total_nodes = val_node_get_total(MPAM_NODE_MEMORY);

    mpam2_el2 = val_sysreg_read(MPAM2_SYSREG);
    mpam2_el2_temp = mpam2_el2;

    /* Clear the PARTID_D & PMG_D bits in mpam2_el2 before writing to them */
    mpam2_el2 = CLEAR_BITS_M_TO_N(mpam2_el2, MPAMn_ELx_PARTID_D_SHIFT+15, MPAMn_ELx_PARTID_D_SHIFT);
    mpam2_el2 = CLEAR_BITS_M_TO_N(mpam2_el2, MPAMn_ELx_PMG_D_SHIFT+7, MPAMn_ELx_PMG_D_SHIFT);

    /* Write default partid and default pmg to mpam2_el2 to generate PE traffic */
    mpam2_el2 |= (((uint64_t)DEFAULT_PMG << MPAMn_ELx_PMG_D_SHIFT) |
                  ((uint64_t)DEFAULT_PMG << MPAMn_ELx_PARTID_D_SHIFT));

    val_sysreg_write(MPAM2_SYSREG, mpam2_el2);

    for (index = 0; index < total_nodes; index++) {

        intr_num = val_node_get_oflow_intrnum(MPAM_NODE_MEMORY, node_index);

        /* Read the number of monitors implemented in this MSC */
        if (val_node_supports_mon(MPAM_NODE_MEMORY, node_index)) {
            mon_count = val_memory_supports_mbwumon(node_index) ?
                        val_memory_mon_count(node_index) : 0;
        }

        /*
         * Skip this MSC if it doesn't implement overflow interrupt
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
            val_sysreg_write(MPAM2_SYSREG, mpam2_el2_temp);
            return;
        }

        /* Enable affinity routing to receive intr_num on primary PE */
        val_gic_route_interrupt_to_pe(intr_num, val_pe_get_mpid_index(pe_index));

        /* Set the interrupt status to pending */
        val_set_status(pe_index, RESULT_PENDING(TEST_NUM));

        /* Generate MBWU monitor overflow error for this memory node */
        val_node_generate_msmon_oflow_error(node_index, mon_count);
        val_gic_write_ispendreg(intr_num);

        /* PE busy polls to check the completion of interrupt service routine */
        timeout = TIMEOUT_LARGE;
        while ((--timeout > 0) && (IS_RESULT_PENDING(val_get_status(pe_index))));

        /* Restore Error Control Register original settings */
        val_node_restore_ecr(MPAM_NODE_MEMORY, node_index);

        if (timeout == 0) {
            val_print(ACS_PRINT_ERR, "\n MSC MSMON oor Err Interrupt not received on %d   ", intr_num);
            val_set_status(pe_index, RESULT_FAIL(TEST_NUM, 02));
            val_sysreg_write(MPAM2_SYSREG, mpam2_el2_temp);
            return;
        }

    }

    /* Restore MPAM2_EL2 settings */
    val_sysreg_write(MPAM2_SYSREG, mpam2_el2_temp);

    /* Set the test status to Skip if none of the MPAM nodes implement error interrupts */
    if (intr_count == 0) {
        val_set_status(pe_index, RESULT_SKIP(TEST_NUM, 0));
        return;
    }

    val_set_status(pe_index, RESULT_PASS(TEST_NUM, 01));
    return;
}

uint32_t testi008_entry()
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

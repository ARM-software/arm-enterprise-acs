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
#include "val/include/val_pe.h"
#include "val/include/val_cache.h"
#include "val/include/val_memory.h"
#include "val/include/val_node_infra.h"
#include "val/include/val_mpam_hwreg_defs.h"

#define TEST_NUM   ACS_MEMORY_TEST_NUM_BASE  +  2
#define TEST_DESC  "Check MBWMIN Partitioning           "

#define MEMCPY_BUF_SIZE 256*1024*1024
#define BW1_PERCENTAGE  25
#define BW2_PERCENTAGE  75
#define MBWMIN_SCENARIO_MAX 2

static uint8_t contend_flag;

static int wait_for_secondary_off(uint32_t primary_pe_index)
{

    uint32_t pe_index;
    uint32_t payload_status;
    uint64_t timeout;
    uint32_t num_pe = val_pe_get_num();

    timeout = (num_pe * TIMEOUT_LARGE);

    /* Wait for all pe OFF or timeout, whichever is first */
    do {
        payload_status = 0;

        for (pe_index = 0; pe_index < num_pe; pe_index++) {

            if (pe_index != primary_pe_index) {
                payload_status |= IS_RESULT_PENDING(val_get_status(pe_index));
            }
        }
    } while (payload_status && (--timeout));

    /* Print all pending pe indices to console at timeout */
    if (!timeout) {

        for (pe_index = 0; pe_index < num_pe; pe_index++) {

            if ((pe_index != primary_pe_index) && IS_RESULT_PENDING(val_get_status(pe_index))) {
                val_print(ACS_PRINT_ERR, " Secondary PE %x OFF timedout \n", pe_index);
            }
        }
        return 1;
    }

    return 0;
}

static void config_mpam_params(uint32_t mpam2_el2)
{

    uint32_t node_index;
    uint32_t cache_node_cnt;
    uint32_t memory_node_cnt;
    uint16_t minmax_partid;

    minmax_partid = DEFAULT_PARTID_MAX;
    cache_node_cnt = val_node_get_total(MPAM_NODE_CACHE);
    memory_node_cnt = val_node_get_total(MPAM_NODE_MEMORY);

    /* Compute the min partition id supported among all MPAM nodes */
    for (node_index = 0; node_index < memory_node_cnt; node_index++) {

        minmax_partid = GET_MIN_VALUE(minmax_partid,
                             val_node_get_partid(MPAM_NODE_MEMORY, node_index));
    }

    for (node_index = 0; node_index < cache_node_cnt; node_index++) {

        minmax_partid = GET_MIN_VALUE(minmax_partid,
                             val_node_get_partid(MPAM_NODE_CACHE, node_index));
    }

    /* Disable all types of partitioning for all cache nodes */
    for (node_index = 0; node_index < cache_node_cnt; node_index++) {

        if (val_cache_supports_cpor(node_index)) {
            /* Disable CPOR partitioning for min(max(PARTID)) */
            val_cache_configure_cpor(node_index, minmax_partid, 100);
        }

        if (val_cache_supports_ccap(node_index)) {
            /* Disable CCAP partitioning for min(max(PARTID)) */
            val_cache_configure_ccap(node_index, minmax_partid, 0, 100);
        }
    }

    /* Clear the PARTID_D & PMG_D bits in mpam2_el2 before writing to them */
    mpam2_el2 = CLEAR_BITS_M_TO_N(mpam2_el2, MPAMn_ELx_PARTID_D_SHIFT+15, MPAMn_ELx_PARTID_D_SHIFT);
    mpam2_el2 = CLEAR_BITS_M_TO_N(mpam2_el2, MPAMn_ELx_PMG_D_SHIFT+7, MPAMn_ELx_PMG_D_SHIFT);

    /* Write MINMAX_PARTID & DEFAULT PMG to mpam2_el2 to generate PE traffic */
    mpam2_el2 |= (((uint64_t)DEFAULT_PMG << MPAMn_ELx_PMG_D_SHIFT) |
                  ((uint64_t)minmax_partid << MPAMn_ELx_PARTID_D_SHIFT));

    val_sysreg_write(MPAM2_SYSREG, mpam2_el2);

    return;
}

void static payload_secondary()
{

    uint32_t pe_index;
    uint8_t *src_buf = 0;
    uint8_t *dest_buf = 0;
    uint64_t buf_size;
    uint64_t mpam2_el2 = 0;

    pe_index = val_pe_get_index_mpid(val_pe_get_mpid());
    mpam2_el2 = val_sysreg_read(MPAM2_SYSREG);

    /* Make this PE configurations */
    config_mpam_params(mpam2_el2);

    /* Create buffers to perform memcopy (stream copy) */
    buf_size = MEMCPY_BUF_SIZE / 2;
    src_buf = (uint8_t *) val_get_shared_memcpybuf(pe_index);
    dest_buf = src_buf + buf_size;

    if ((src_buf == NULL) || (dest_buf == NULL)) {
        val_set_status(pe_index, RESULT_FAIL(TEST_NUM, 01));

        /* Restore MPAM2_EL2 settings */
        val_sysreg_write(MPAM2_SYSREG, mpam2_el2);
        return;
    }

    /* Generate memory bandwidth contention via PE traffic */
    while (contend_flag) {
        val_mem_copy((void *)src_buf, (void *)dest_buf, buf_size);
        val_data_cache_ops_by_va((addr_t)&contend_flag, INVALIDATE);
    }

    /* Restore MPAM2_EL2 settings */
    val_sysreg_write(MPAM2_SYSREG, mpam2_el2);

    val_set_status(pe_index, RESULT_PASS(TEST_NUM, 01));

    return;
}

static void payload_primary()
{

    uint32_t pe_index;
    uint32_t node_index;
    uint32_t primary_pe_index;
    uint32_t cache_node_cnt;
    uint32_t memory_node_cnt;
    uint32_t mbwmin_node_cnt = 0;
    uint16_t minmax_partid;
    uint64_t mpam2_el2 = 0;
    uint8_t alloc_status;
    uint8_t *src_buf = 0;
    uint8_t *dest_buf = 0;
    uint64_t buf_size;
    uint64_t start_time;
    uint64_t end_time;
    uint32_t scenario_index;
    uint32_t scenario_cnt = 0;
    uint64_t *latency_buf_ptr;
    uint32_t num_pe = val_pe_get_num();

    minmax_partid = DEFAULT_PARTID_MAX;
    primary_pe_index = val_pe_get_index_mpid(val_pe_get_mpid());
    cache_node_cnt = val_node_get_total(MPAM_NODE_CACHE);
    memory_node_cnt = val_node_get_total(MPAM_NODE_MEMORY);

    /*
     * Compute the number of MIN BW supported MPAM MEMORY nodes,
     * and the min partition id supported among all MPAM nodes
     */
    for (node_index = 0; node_index < memory_node_cnt; node_index++) {

        if (val_memory_supports_mbwmin(node_index)) {
            mbwmin_node_cnt++;
        }

        minmax_partid = GET_MIN_VALUE(minmax_partid,
                                      val_node_get_partid(MPAM_NODE_MEMORY, node_index));
    }

    for (node_index = 0; node_index < cache_node_cnt; node_index++) {

        minmax_partid = GET_MIN_VALUE(minmax_partid,
                                      val_node_get_partid(MPAM_NODE_CACHE, node_index));
    }

    /* Skip this test if no MIN BW supported MPAM memory node present in the system */
    if (mbwmin_node_cnt == 0) {
        val_set_status(primary_pe_index, RESULT_SKIP(TEST_NUM, 0));
        return;
    }

    /* Create shared latency buffer to store latencies of various scenarios */
    val_allocate_shared_latencybuf(memory_node_cnt, MBWMIN_SCENARIO_MAX);

    mpam2_el2 = val_sysreg_read(MPAM2_SYSREG);

    /* Make this PE configurations */
    config_mpam_params(mpam2_el2);

    for (node_index = 0; node_index < memory_node_cnt; node_index++) {

        if (val_memory_supports_mbwmin(node_index)) {

            /* Disable MBWPBM partitioning for the current memory node_index */
            val_memory_configure_mbwpbm(node_index, minmax_partid, 100);

            /* Disable MBWMAX partitioning for the current memory node_index */
            if (val_memory_supports_mbwmax(node_index)) {
                val_memory_configure_mbwmax(node_index, minmax_partid, HARDLIMIT_DIS, 100);
            }

            /* Create a shared memcopy buffer from this memory node */
            alloc_status = val_allocate_shared_memcpybuf(val_memory_get_base(node_index),
                                                         val_memory_get_size(node_index),
                                                         MEMCPY_BUF_SIZE,
                                                         num_pe
                                                         );

            if (alloc_status == 0) {
                val_set_status(primary_pe_index, RESULT_FAIL(TEST_NUM, 01));
                val_sysreg_write(MPAM2_SYSREG, mpam2_el2);
                return;
            }

            /* Create buffers to perform memcopy (stream copy) */
            buf_size = MEMCPY_BUF_SIZE / 2;
            src_buf = (uint8_t *) val_get_shared_memcpybuf(primary_pe_index);
            dest_buf = src_buf + buf_size;
            val_mem_copy((void *)src_buf, (void *)dest_buf, buf_size);

            /****************************************************************
             *                        SCENARIO ONE
             ***************************************************************/

            /* Get the latency buffer pointer for scenario two*/
            scenario_cnt = 0;
            latency_buf_ptr = val_get_shared_latencybuf(scenario_cnt, node_index);

            /* Configure the current memory node_index for MIN BW1 */
            val_memory_configure_mbwmin(node_index, minmax_partid, BW1_PERCENTAGE);

            contend_flag = 1;
            val_data_cache_ops_by_va((addr_t)&contend_flag, CLEAN);

            /* Create bandwidth contention on the current memory node */
            for (pe_index = 0; pe_index < num_pe; pe_index++) {

                if (pe_index != primary_pe_index) {
                    val_set_status(pe_index, RESULT_PENDING(TEST_NUM));
                    val_execute_on_pe(pe_index, payload_secondary, 0);
                }
            }

            /* Start mem copy and measure copy latency */
            val_measurement_start();
            start_time = val_measurement_read();
            val_mem_copy((void *)src_buf, (void *)dest_buf, buf_size);
            end_time = val_measurement_read();
            *latency_buf_ptr = end_time - start_time;
            val_measurement_stop();

            contend_flag = 0;
            val_data_cache_ops_by_va((addr_t)&contend_flag, CLEAN);

            /* Return from the test if any secondary pe is timed out */
            if (wait_for_secondary_off(primary_pe_index)) {
                goto error_secondary_pending;
            }

            /****************************************************************
             *                        SCENARIO TWO
             ***************************************************************/

            /* Get the latency buffer pointer for scenario two*/
            scenario_cnt++;
            latency_buf_ptr = val_get_shared_latencybuf(scenario_cnt, node_index);

            /* Configure the current memory node_index for MIN BW2 */
            val_memory_configure_mbwmin(node_index, minmax_partid, BW2_PERCENTAGE);

            contend_flag = 1;
            val_data_cache_ops_by_va((addr_t)&contend_flag, CLEAN);

            /* Create bandwidth contention on the current memory node */
            for (pe_index = 0; pe_index < num_pe; pe_index++) {

                if (pe_index != primary_pe_index) {
                    val_set_status(pe_index, RESULT_PENDING(TEST_NUM));
                    val_execute_on_pe(pe_index, payload_secondary, 0);
                }
            }

            /* Start mem copy and measure copy latency */
            val_measurement_start();
            start_time = val_measurement_read();
            val_mem_copy((void *)src_buf, (void *)dest_buf, buf_size);
            end_time = val_measurement_read();
            *latency_buf_ptr = end_time - start_time;
            val_measurement_stop();

            contend_flag = 0;
            val_data_cache_ops_by_va((addr_t)&contend_flag, CLEAN);

            /* Return from the test if any secondary is timed out */
            if (wait_for_secondary_off(primary_pe_index)) {
                goto error_secondary_pending;
            }

            scenario_cnt++;

            /* Free the copy buffers to the heap manager */
            val_mem_free_shared_memcpybuf(num_pe, MEMCPY_BUF_SIZE);
        }
    }

    /* Restore MPAM2_EL2 settings */
    val_sysreg_write(MPAM2_SYSREG, mpam2_el2);

    /* Compare the stream copy latencies for all the scenarios */
    for (node_index = 0; node_index < memory_node_cnt; node_index++) {

        for (scenario_index = 0; scenario_index < scenario_cnt-1; scenario_index++) {

            if (val_memory_supports_mbwmin(node_index)) {

                if ((*val_get_shared_latencybuf(scenario_cnt, node_index))
                      <  (*val_get_shared_latencybuf(scenario_cnt+1, node_index))) {
                          val_set_status(primary_pe_index, RESULT_FAIL(TEST_NUM, 01));
                          return;
                      }
            }
        }
    }

    /* Return the latency buffers to the heap manager */
    val_mem_free_shared_latencybuf(memory_node_cnt);

    /* Set the test status to pass */
    val_set_status(primary_pe_index, RESULT_PASS(TEST_NUM, 01));

    return;

error_secondary_pending:
    /* Restore MPAM2_EL2 settings */
    val_sysreg_write(MPAM2_SYSREG, mpam2_el2);

    /* Return the copy buffers to the heap manager */
    val_mem_free_shared_memcpybuf(num_pe, MEMCPY_BUF_SIZE);

    /* Return the latency buffers to the heap manager */
    val_mem_free_shared_latencybuf(memory_node_cnt);

    /* Set the test status to fail */
    val_set_status(primary_pe_index, RESULT_FAIL(TEST_NUM, 01));

    return;
}

uint32_t testd002_entry()
{

    uint32_t pe_index;
    uint32_t status = ACS_STATUS_FAIL;
    uint32_t num_pe = val_pe_get_num();

    status = val_initialize_test(TEST_NUM, TEST_DESC, num_pe);

    for (pe_index = 0; pe_index < num_pe; pe_index++)
        val_set_status(pe_index, RESULT_PASS(TEST_NUM, 01));

    /* This check is when user is forcing us to skip this test */
    if (status != ACS_STATUS_SKIP)
        payload_primary();

    /* get the result from all PE and check for failure */
    status = val_check_for_error(TEST_NUM, num_pe);

    return status;
}

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

#include "include/val_infra.h"
#include "include/val_pe.h"


/**
 * @brief  This API calls PAL layer to print a formatted string
 *         to the output console.
 *         1. Caller       - Application layer
 *         2. Prerequisite - None.
 *
 * @param level   the print verbosity (1 to 5)
 * @param string  formatted ASCII string
 * @param data    64-bit data. set to 0 if no data is to sent to console.
 *
 * @return        None
 */
void val_print(uint32_t level, char8_t *string, uint64_t data)
{

    if (level >= g_print_level)
        pal_print(string, data);
}

/**
  @brief  This API calls PAL layer to print a string to the output console.
            1. Caller       - Application layer
            2. Prerequisite - None.

  @param level   the print verbosity (1 to 5)
  @param string  formatted ASCII string
  @param data    64-bit data. set to 0 if no data is to sent to console.

  @return        None
  **/
void val_print_raw(uint32_t level, char8_t *string, uint64_t data)
{
    if (level >= g_print_level) {
        uint64_t uart_address = (uint64_t)0x7FF80000;
        pal_print_raw(uart_address, string, data);
     }
}

/**
 * @brief  This API calls PAL layer to read from a Memory address
 *         and return 32-bit data.
 *         1. Caller       - Test Suite
 *         2. Prerequisite - None.
 *
 * @param addr   64-bit address
 *
 * @return       32-bits of data
 */
uint32_t val_mmio_read(addr_t addr)
{
    return pal_mmio_read(addr);
}

/**
 * @brief  This function will call PAL layer to write 32-bit data to
 *         a Memory address.
 *       1. Caller       - Test Suite
 *       2. Prerequisite - None.
 *
 * @param addr   64-bit address
 * @param data   32-bit data
 *
 * @return       None
 */
void val_mmio_write(addr_t addr, uint32_t data)
{
    pal_mmio_write(addr, data);
}

/**
 * @brief   This API prints the test number, description and
 *          sets the test status to pending for the input number of PEs.
 *          1. Caller       - Application layer
 *          2. Prerequisite - val_allocate_shared_mem
 *
 * @param   test_num unique number identifying this test
 * @param   desc     brief description of the test
 * @param   num_pe   the number of PE to execute this test on.
 *
 * @return  Skip - if the user has overridden to skip the test.
 */
uint32_t val_initialize_test(uint32_t test_num, char8_t *desc, uint32_t num_pe)
{

    uint32_t i;
    uint32_t index = val_pe_get_index_mpid(val_pe_get_mpid());

    /*Always print this */
    val_print(ACS_PRINT_ERR, "%4d : ", test_num);
    val_print(ACS_PRINT_TEST, desc, 0);
    val_report_status(0, ACS_TEST_START(test_num));
    val_pe_initialize_default_exception_handler(val_pe_default_esr);

    g_acs_tests_total++;

    for (i = 0; i < num_pe; i++)
        val_set_status(i, RESULT_PENDING(test_num));

    for (i=0 ; i<MAX_TEST_SKIP_NUM ; i++) {
        if (g_skip_test_num[i] == test_num) {
            val_print(ACS_PRINT_TEST, "\n       USER OVERRIDE  - Skip Test        ", 0);
            val_set_status(index, RESULT_SKIP(test_num, 0));
            return ACS_STATUS_SKIP;
        }
     }

     return ACS_STATUS_PASS;
}

/**
 * @brief   Allocate memory which is to be shared across PEs
 *
 * @param   None
 *
 * @result  None
 */
void val_allocate_shared_mem()
{
     pal_mem_allocate_shared(val_pe_get_num(), sizeof(VAL_SHARED_MEM_t));
}

/**
 * @brief   Allocate a 2D buffers to share across PEs from specific input memory node
 *
 * @param   mem_base    base address of the memory node
 * @param   mem_size    size of the memory node
 * @param   buf_size    size of each shared buffer
 * @param   pe_cnt      number of pes to create shared buffers
 *
 * @result  status      1 for success, 0 for failure
 */
uint8_t val_allocate_shared_memcpybuf(uint64_t mem_base, uint64_t mem_size, uint64_t buf_size, uint32_t pe_cnt)
{
    return pal_mem_allocate_shared_memcpybuf(mem_base, mem_size, buf_size, pe_cnt);
}

/**
 * @brief   Allocate a two 2D buffer to record latencies across all PEs
 *
 * @param   node_cnt    - MPAM supported Memory node count
 * @param   scenario_cnt- Bandwidth scenarios tested on a memory node
 *
 * @result  None
 */
void val_allocate_shared_latencybuf(uint32_t node_cnt, uint32_t scenario_cnt)
{
    pal_mem_allocate_shared_latencybuf(node_cnt, scenario_cnt);
}

/**
 * @brief   Free the memory which was allocated by allocate_shared_mem
 *          1. Caller       - Application Layer
 *          2. Prerequisite - val_allocate_shared_mem
 *
 * @param   None
 *
 * @result  None
 */
void val_free_shared_mem()
{
    pal_mem_free_shared();
}

/**
 * @brief   Creates a buffer with length equal to size
 *
 * @param   size    - Buffer size to be created
 *
 * @result  Buffer address if SUCCESSFUL, else NULL
 */
void *val_allocate_buf(uint64_t size)
{
    return pal_mem_allocate_buf(size);
}

/**
 * @brief   Creates a buffer with length equal to size within the
 *          address range (mem_base, mem_base + mem_size)
 *
 * @param   mem_base    - Base address of the memory range
 * @param   mem_size    - Size of the memory range of interest
 * @param   size        - Buffer size to be created
 *
 * @result  Buffer address if SUCCESSFUL, else NULL
 */
void *val_allocate_address(uint64_t mem_base, uint64_t mem_size, uint64_t size)
{
    return pal_mem_allocate_address(mem_base, mem_size, size);
}

/**
 * @brief   Free the memory starting at address buffer
 *
 * @param   buffer  - Base address of the buffer
 * @param   size    - Length of the buffer to be freed
 *
 * @result  None
 */
void val_free_buf(void *buffer, uint64_t size)
{
    pal_mem_free_buf(buffer, size);
}

/**
 * @brief   Copy source buffer to destination buffer
 *
 * @param   src     - Source buf address
 * @param   dest    - Destination buf address
 * @param   size    - Length of the buffer to be copied
 *
 * @result  None
 */
void val_mem_copy(void *src, void *dest, uint64_t size)
{
    pal_mem_copy(src, dest, size);
}

/**
 * @brief   This function sets the address of the test entry and the test
 *          argument to the shared address space which is picked up by the
 *          secondary PE identified by index.
 *          1. Caller       - VAL
 *          2. Prerequisite - val_allocate_shared_mem
 *
 * @param   index       the PE Index
 * @param   addr        Address of the test payload which needs to be executed by PE
 * @param   test_data   64-bit data to be passed as a parameter to test payload
 *
 * @return  None
  */
void val_set_test_data(uint32_t index, uint64_t addr, uint64_t test_data)
{

    volatile VAL_SHARED_MEM_t *mem;

    if (index > val_pe_get_num()) {
        val_print(ACS_PRINT_ERR, "\n Incorrect PE index = %d", index);
        return;
    }

    mem = (VAL_SHARED_MEM_t *)pal_mem_get_shared_addr();
    mem = mem + index;

    mem->data0 = addr;
    mem->data1 = test_data;

    val_data_cache_ops_by_va((addr_t)&mem->data0, CLEAN_AND_INVALIDATE);
    val_data_cache_ops_by_va((addr_t)&mem->data1, CLEAN_AND_INVALIDATE);
}

/**
 * @brief   This API returns the optional data parameter between PEs
 *          to the output console.
 *          1. Caller       - Test Suite
 *          2. Prerequisite - val_set_test_data
 *
 * @param   index   PE index whose data parameter has to be returned.
 *
 * @return  64-bit data
 */

void val_get_test_data(uint32_t index, uint64_t *data0, uint64_t *data1)
{

    volatile VAL_SHARED_MEM_t *mem;

    if (index > val_pe_get_num()) {
        val_print(ACS_PRINT_ERR, "\n Incorrect PE index = %d", index);
        return;
    }

    mem = (VAL_SHARED_MEM_t *) pal_mem_get_shared_addr();
    mem = mem + index;

    val_data_cache_ops_by_va((addr_t)&mem->data0, INVALIDATE);
    val_data_cache_ops_by_va((addr_t)&mem->data1, INVALIDATE);

    *data0 = mem->data0;
    *data1 = mem->data1;
}

/**
 * @brief   This API returns the address to store memcopy latency on the current PE
 *
 * @param   pe_index    current PE index offset into latency buffer
 *
 * @return  64-bit source buf address, dest buf is assumed to be contiguous
 */
uint64_t val_get_shared_memcpybuf(uint32_t pe_index)
{

    uint8_t **smem_memcpy_buf = 0;

    if (pe_index > val_pe_get_num()) {
        return 0;
    }

    smem_memcpy_buf = (uint8_t **)pal_mem_get_shared_memcpybuf_addr();

    return (uint64_t)(smem_memcpy_buf[pe_index]);
}

/**
 * @brief   Free the shared memcopy buffers for num_pe PEs
 *
 * @param   num_pe      number of pes holding shared buffers
 * @param   PeCnt       number of pes holding shared buffers
 * @param   buf_size    size of shared buffer each pe holding
 *
 * @result  None
 */
void val_mem_free_shared_memcpybuf(uint32_t num_pe, uint64_t buf_size)
{
    pal_mem_free_shared_memcpybuf(num_pe, buf_size);
}

/**
 * @brief   This API returns the address to store memcopy latency on the current PE
 *
 * @param   scenario_index  Scenario offset offset into a specific memory node
 * @param   node_index      current memory node index which is under test
 *
 * @return  64-bit address
 */
uint64_t *val_get_shared_latencybuf(uint32_t scenario_index, uint32_t node_index)
{

    uint64_t **smem_latency_buf = 0;

    smem_latency_buf = (uint64_t **)pal_mem_get_shared_latencybuf_addr();
    return (uint64_t *)(smem_latency_buf[node_index]+scenario_index);
}

/**
 * @brief   Free the shared latency buffers for num_pe PEs
 *
 * @param   num_pe      number of pes holding shared buffers
 * @param   PeCnt       number of pes holding shared buffers
 *
 * @result  None
 */
void val_mem_free_shared_latencybuf(uint32_t node_cnt)
{
    pal_mem_free_shared_latencybuf(node_cnt);
}

/**
 * @brief   This function will wait for all PEs to report their status
 *          or we timeout and set a failure for the PE which timed-out
 *          1. Caller       - Application layer
 *          2. Prerequisite - val_set_status
 *
 * @param   test_num    Unique test number
 * @param   num_pe      Number of PE who are executing this test
 * @param   timeout     integer value ob expiry the API will timeout and return
 *
 * @return  None
 */
void val_wait_for_test_completion(uint32_t test_num, uint32_t num_pe, uint32_t timeout)
{

    uint32_t i = 0, j = 0;

    /* For single PE tests, there is no need to wait for the results */
    if (num_pe == 1)
        return;

    while (--timeout) {
        j = 0;
        for (i = 0; i < num_pe; i++) {
            if (IS_RESULT_PENDING(val_get_status(i))) {
                j = i+1;
            }
        }
        /* If None of the PE have the status as Pending, return */
        if (!j)
            return;
    }
    /* We are here if we timed-out, set the last index PE as failed */
    val_set_status(j-1, RESULT_FAIL(test_num, 0xF));
}

/**
 * @brief   This API Executes the payload function on secondary PEs
 *          1. Caller       - Application layer
 *          2. Prerequisite - val_pe_create_info_table
 *
 * @param   test_num    unique test number
 * @param   num_pe      The number of PEs to run this test on
 * @param   payload     Function pointer of the test entry function
 * @param   test_input  optional parameter for the test payload
 *
 * @return  None
 */
void val_run_test_payload(uint32_t test_num, uint32_t num_pe, void (*payload)(void), uint64_t test_input)
{

    uint32_t my_index = val_pe_get_index_mpid(val_pe_get_mpid());
    uint32_t i;

    /* This test run on current PE */
    payload();
    if (num_pe == 1)
        return;

    /* Now run the test on all other PE */
    for (i = 0; i < num_pe; i++) {
        if (i != my_index)
            val_execute_on_pe(i, payload, test_input);
    }

    val_wait_for_test_completion(test_num, num_pe, TIMEOUT_LARGE);
}

/**
 * @brief   Prints the status of the completed test
 *          1. Caller       - Test Suite
 *          2. Prerequisite - val_set_status
 *
 * @param   test_num    unique test number
 * @param   num_pe      The number of PEs to query for status
 *
 * @return  Success or on failure - status of the last failed PE
 */
uint32_t val_check_for_error(uint32_t test_num, uint32_t num_pe)
{

    uint32_t i;
    uint32_t status = 0;
    uint32_t error_flag = 0;
    uint32_t my_index = val_pe_get_index_mpid(val_pe_get_mpid());

    /*
     * This special case is needed when the Main PE is not the first
     * entry of pe_info_table but num_pe is 1 for SOC tests
     */
    if (num_pe == 1) {
        status = val_get_status(my_index);
        val_report_status(my_index, status);
        if (IS_TEST_PASS(status)) {
            g_acs_tests_pass++;
            return ACS_STATUS_PASS;
        }

        if (IS_TEST_SKIP(status))
            return ACS_STATUS_SKIP;

        g_acs_tests_fail++;
        return ACS_STATUS_FAIL;
    }

    for (i = 0; i < num_pe; i++) {
        status = val_get_status(i);
        //val_print(ACS_PRINT_ERR, "Status %4x \n", status);
        if (IS_TEST_FAIL_SKIP(status)) {
            val_report_status(i, status);
            error_flag += 1;
            break;
        }
    }

    if (!error_flag)
        val_report_status(my_index, status);

    if (IS_TEST_PASS(status)) {
        g_acs_tests_pass++;
        return ACS_STATUS_PASS;
    }

    if (IS_TEST_SKIP(status))
        return ACS_STATUS_SKIP;

    g_acs_tests_fail++;
    return ACS_STATUS_FAIL;
}

/**
 * @brief   Clean and Invalidate the Data cache line containing
 *          the input address tag
 */
void val_data_cache_ops_by_va(addr_t addr, uint32_t type)
{
    pal_pe_data_cache_ops_by_va(addr, type);

}

/**
 * @brief   Issues memory barrier to ensure ordering of the instructions
 */
void val_memory_ops_issue_barrier(uint32_t type)
{

    switch (type) {
        case DMB:
           arm64_issue_dmb();
           break;
        case DSB:
           arm64_issue_dsb();
           break;
        case ISB:
           arm64_issue_isb();
           break;
        default:
           arm64_issue_dsb();
    }

}

/**
 * @brief  Update ELR based on the offset provided
 */
void val_pe_update_elr(void *context, uint64_t offset)
{
    pal_pe_update_elr(context, offset);
}

/**
 * @brief  Get ESR from exception context
 */
uint64_t val_pe_get_esr(void *context)
{
    return pal_pe_get_esr(context);
}

/**
 * @brief  Get FAR from exception context
 */
uint64_t val_pe_get_far(void *context)
{
    return pal_pe_get_far(context);
}

/**
 * @brief  Write to an address, meant for debugging purpose
 */
void val_debug_brk(uint32_t data)
{
    /* address = pal_get_debug_address() */
    addr_t address = 0x9000F000;
    *(addr_t *)address = data;
}

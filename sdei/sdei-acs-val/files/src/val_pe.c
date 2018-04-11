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

#include "val_interface.h"

/**
 * @brief   Pointer to the memory location of the PE Information table
 */
pe_info_table_t *g_pe_info_table;

/**
 *  @brief   This API will clean and invalidate the given address in Cache
 *
 *  @param addr   64-bit address
 *
 *  @return  none
 */
void val_pe_data_cache_clean_invalidate(uint64_t addr)
{
    pal_pe_data_cache_clean_invalidate(addr);
}

/**
 *  @brief   This API will invalidate the given address in Cache
 *
 *  @param addr   64-bit address
 *
 *  @return  none
 */
void val_pe_data_cache_invalidate(uint64_t addr)
{
    pal_pe_data_cache_invalidate(addr);
}

/**
 *  @brief   This API provides a 'C' interface to call System register reads
 *           1. Caller       -  Test Suite
 *           2. Prerequisite -  None
 *  @param   reg_id  - the system register index for which data is returned
 *  @return  the value read from the system register.
 */
acs_status_t val_pe_reg_read(pe_reg_id_t reg_id, uint64_t *val)
{
    if (!val)
        return ACS_ERROR;
    switch(reg_id) {
        case MPIDR_EL1:
          *val = ArmReadMpidr();
          break;
        case ID_AA64PFR0_EL1:
          *val = ArmReadIdPfr0();
          break;
        case CurrentEL:
          *val = AA64ReadCurrentEL();
          break;
        case DAIF:
          *val = __EXTRACT_BITS(ArmReadDaif(), 6, 4);
          break;
        case SPsel:
          *val = ArmReadSPselPState();
          break;
        case ELR_EL:
          *val = ((CLIENT_EL == 0x2) ? ArmReadElrEl2() : ArmReadElrEl1());
          break;
        case SPSR_EL:
          *val = ((CLIENT_EL == 0x2) ? ArmReadSpsrEl2() : ArmReadSpsrEl1());
          break;
        default:
          return ACS_ERROR;
    }
    return ACS_SUCCESS;
}

/**
 *  @brief   This API provides a 'C' interface to call System register writes
 *           1. Caller       -  Test Suite
 *           2. Prerequisite -  None
 *  @param   reg_id  - the system register index for which data is written
 *  @param   write_data - the 64-bit data to write to the system register
 *  @return  None
 */
acs_status_t val_pe_reg_write(pe_reg_id_t reg_id, uint64_t write_data)
{
    return ACS_SUCCESS;
}

/**
 *  @brief   This API indicates the presence of exception level 3
 *           1. Caller       -  Test Suite
 *           2. Prerequisite -  None
 *  @param   None
 *  @return  1 if EL3 is present, 0 if EL3 is not implemented
 */
acs_status_t val_is_el3_enabled()
{
    uint64_t data;

    val_pe_reg_read(ID_AA64PFR0_EL1, &data);

    return ((data >> 12) & 0xF);
}

/**
 *  @brief   This API indicates the presence of exception level 2
 *           1. Caller       -  Test Suite
 *           2. Prerequisite -  None
 *  @param   None
 *  @return  1 if EL2 is present, 0 if EL2 is not implemented
 */
acs_status_t val_is_el2_enabled()
{
    uint64_t data;

    val_pe_reg_read(ID_AA64PFR0_EL1, &data);

    return ((data >> 8) & 0xF);
}

/**
 *  @brief   This API will call PAL layer to fill in the PE information
 *           into the g_pe_info_table pointer.
 *           1. Caller       -  Application layer.
 *           2. Prerequisite -  Memory allocated and passed as argument.
 *  @param   pe_info_table  pre-allocated memory pointer for pe_info
 *  @return  Error if Input param is NULL or num_pe is 0.
 */
acs_status_t val_pe_create_info_table(void *pe_info_table)
{
    if (pe_info_table == NULL) {
        val_print(ACS_LOG_ERR, "\n        Input memory for PE Info table cannot be NULL");
        return ACS_ERROR;
    }

    g_pe_info_table = (pe_info_table_t *)pe_info_table;
    pal_pe_create_info_table(g_pe_info_table);
    val_print(ACS_LOG_TEST, "\n        PE_INFO: Number of PE detected       : %4d",
                                                                        val_pe_get_num());

    if (val_pe_get_num() == 0) {
        val_print(ACS_LOG_ERR, "\n        *** CRITICAL ERROR: Num PE is 0x0 ***");
        return ACS_ERROR;
    }
    return ACS_SUCCESS;
}

/**
 *  @brief   This API will allocates memory that is shared by all PEs
 *
 *  @param  none
 *
 *  @return status
 */
acs_status_t val_shared_mem_alloc()
{
    return pal_pe_alloc_shared_mem(val_pe_get_num(), sizeof(pe_shared_mem_t));
}

/**
 *  @brief   This API will writes data to shared memory at the index provided as
 *           argument
 *
 *  @param index  PE index
 *
 *  @return status
 */
acs_status_t val_shared_mem_write(uint32_t index, pe_shared_mem_t *src)
{
    pe_shared_mem_t *dst = g_pe_shared_mem + index;
    dst->status = src->status;
    dst->data0 = src->data0;
    dst->data1 = src->data1;
    val_pe_data_cache_clean_invalidate((uint64_t)&dst->status);
    val_pe_data_cache_clean_invalidate((uint64_t)&dst->data0);
    val_pe_data_cache_clean_invalidate((uint64_t)&dst->data1);
    return ACS_SUCCESS;
}

/**
 *  @brief   This API will reads data from shared memory at the index provided as
 *           argument
 *
 *  @param index  PE index
 *
 *  @return status
 */
acs_status_t val_shared_mem_read(uint32_t index, pe_shared_mem_t *dst)
{
    pe_shared_mem_t *src = g_pe_shared_mem + index;
    val_pe_data_cache_invalidate((uint64_t)&src->status);
    val_pe_data_cache_invalidate((uint64_t)&src->data0);
    val_pe_data_cache_invalidate((uint64_t)&src->data1);
    dst->status = src->status;
    dst->data0 = src->data0;
    dst->data1 = src->data1;
    return ACS_SUCCESS;
}

/**
 *  @brief   This API frees the allocated shared memory
 *  @param   none 
 *  @return  none
 */
void val_shared_mem_free()
{
    pal_pe_free_shared_mem();
    pal_pe_clean_up();
}

/**
 *  @brief   This API returns the number of PE from the g_pe_info_table.
 *           1. Caller       -  Application layer, test Suite.
 *           2. Prerequisite -  val_pe_create_info_table.
 *  @param   None
 *  @return  the number of pe discovered
 */
uint32_t val_pe_get_num()
{
    if (g_pe_info_table == NULL)
        return 0;

    return g_pe_info_table->header.num_of_pe;
}

/**
 *  @brief   This API reads MPIDR system regiser and return the affx bits
 *           1. Caller       -  Test Suite, VAL
 *           2. Prerequisite -  None
 *  @param   None
 *  @return  32-bit affinity value
 */
uint64_t val_pe_get_mpid()
{
    uint64_t data;

    val_pe_reg_read(MPIDR_EL1, &data);
    /* return the affx bits */
    data = (((data >> 32) & 0xFF) << 24) | (data & 0xFFFFFF);

    return data;
}

/**
 *  @brief   This API returns the MPIDR value for the PE indicated by index
 *           1. Caller       -  Test Suite, VAL
 *           2. Prerequisite -  val_create_peinfo_table
 *  @param   index - the index of the PE whose mpidr value is required.
 *  @return  MPIDR value
 */
uint64_t val_pe_get_mpid_index(uint32_t index)
{
    pe_info_entry_t *entry;

    if (index > g_pe_info_table->header.num_of_pe)
        return INVALID_INDEX;

    entry = g_pe_info_table->pe_info;

    return entry[index].mpidr;
}

/**
 *  @brief   This API returns the index of the PE whose MPIDR matches with the input MPIDR
 *           1. Caller       -  Test Suite, VAL
 *           2. Prerequisite -  val_create_peinfo_table
 *  @param   mpid - the mpidr value of pE whose index is returned.
 *  @return  Index of PE
 */
uint32_t val_pe_get_index_mpid(uint64_t mpid)
{
    pe_info_entry_t *entry;
    uint32_t i = g_pe_info_table->header.num_of_pe;

    entry = g_pe_info_table->pe_info;
    while (i > 0) {
        if (entry->mpidr == mpid) {
            return entry->pe_num;
        }
        entry++;
        i--;
    }

    return INVALID_INDEX;
}

/**
 *  @brief   This API returns the index of the current PE
 *
 *  @param  none
 *
 *  @return  none
 */
uint32_t val_pe_get_index()
{
    uint64_t mpid = val_pe_get_mpid();
    return val_pe_get_index_mpid(mpid);
}

/**
 *  @brief   This API executes code addressed by given function pointer on each PE
 *           in the system.
 *  @param payload  function pointer
 *  @param arg   arguments to the fuction 
 *
 *  @return  status
 */
acs_status_t val_pe_execute_on_all(void *payload, uint64_t arg)
{
    acs_status_t status;
    int num_pe = val_pe_get_num();
    status = pal_pe_execute_on_all(num_pe, payload, arg);
    return status;
}

/**
 *  @brief   This API suspends the current PE using PSCI call
 *  @param  none
 *
 *  @return  none
 */
void val_pe_suspend(uint32_t power_down) {
    pal_pe_suspend(power_down);
}

/**
 *  @brief   This API will power off the given PE index using PSCI call
 *  @param pe_index  Index of the PE
 *
 *  @return  none
 */
void val_pe_poweroff(uint32_t pe_index)
{
    pal_pe_poweroff(pe_index);
}

/**
 *  @brief   This API will power on the given PE index using PSCI call
 *  @param pe_index  Index of the PE
 *
 *  @return  none
 */
void val_pe_poweron(uint32_t pe_index)
{
    uint64_t pe_mpidr = val_pe_get_mpid_index(pe_index);
    pal_pe_poweron(pe_mpidr);
}

/**
 *  @brief   This function frees the allocated memory for PE info table
 *  @param  none
 *
 *  @return  none
 */
void val_pe_free_info_table(void)
{
    pal_free_mem((uint64_t *)g_pe_info_table);
}

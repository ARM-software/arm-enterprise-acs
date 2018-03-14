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

#define GICD_CLRSPI_NSR     0x048
#define GICD_CLRLPIR        0x048
#define GICD_ISENABLER      0x100
#define GICD_ICENABLER      0x180
#define GICD_ISPENDR        0x200
#define GICD_ISACTIVER0     0x300
#define GICD_ICPENDR0       0x280
#define GICD_ICACTIVER0     0x380
#define GICD_IROUTER        0x6000

#define SZ_64KB             0x10000

typedef struct gic_intr_info {
    uint32_t mask;
    uint64_t base;
} gic_intr_t;

gic_info_table_t *g_gic_info_table;
static uint64_t g_gicd_base;

acs_status_t set_gicd_base(void)
{
    gic_info_entry_t  *gic_entry;

    gic_entry = g_gic_info_table->gic_info;

    while (gic_entry->type != 0xFF) {
        if (gic_entry->type == ENTRY_TYPE_GICD) {
            g_gicd_base = gic_entry->base;
            return ACS_SUCCESS;
        }
        gic_entry++;
    }
    return ACS_ERROR;
}

/**
 * @brief   This API will call PAL layer to fill in the GIC information
 *          into the g_gic_info_table pointer.
 *          1. Caller       -  Application layer.
 *          2. Prerequisite -  Memory allocated and passed as argument.
 * @param   gic_info_table  pre-allocated memory pointer for gic_info
 * @return  Error if Input param is NULL
 */
acs_status_t val_gic_create_info_table(uint64_t *gic_info_table)
{
    if (gic_info_table == NULL) {
        val_print(ACS_LOG_ERR, "\n       Input for Create Info table cannot be NULL");
        return ACS_ERROR;
    }

    g_gic_info_table = (gic_info_table_t *)gic_info_table;

    pal_gic_create_info_table(g_gic_info_table);

    val_print(ACS_LOG_TEST, "\n        GIC_INFO: Number of GICD             : %4d",
                                                                g_gic_info_table->header.num_gicd);
    val_print(ACS_LOG_TEST, "\n        GIC_INFO: Number of ITS              : %4d",
                                                                g_gic_info_table->header.num_its);

    if (g_gic_info_table->header.num_gicd == 0) {
        val_print(ACS_LOG_ERR, "\n        ** CRITICAL ERROR: GIC Distributor count is 0 **");
        return ACS_ERROR;
    }

    if (set_gicd_base())
        return ACS_ERROR;

    return ACS_SUCCESS;
}

/**
 * @brief   This API returns the base address of the GIC Distributor.
 *           The assumption is we have only 1 GIC Distributor. IS this true?
 *           1. Caller       -  VAL
 *           2. Prerequisite -  val_gic_create_info_table
 * @param   None
 * @return  Address of GIC Distributor
 */
acs_status_t val_get_gicd_base(uint64_t *gicd_base)
{
    *gicd_base = g_gicd_base;
    return ACS_SUCCESS;
}

/**
 * @brief   This function is a single point of entry to retrieve
 *           all GIC related information.
 *           1. Caller       -  Test Suite
 *           2. Prerequisite -  val_gic_create_info_table
 * @return  status
 */
acs_status_t val_gic_get_version(uint32_t *version)
{
    if (!version)
        return ACS_ERROR;

    if (g_gic_info_table->header.gic_version != 0) {
            val_print(ACS_LOG_INFO, "\n        Gic version from ACPI table = %d",
                                                        g_gic_info_table->header.gic_version);
            *version =  g_gic_info_table->header.gic_version;
            return ACS_SUCCESS;
    }

    *version = ((val_mmio_read(g_gicd_base + 0xFE8) >> 4) & 0xF);

    return ACS_SUCCESS;
}

/**
 * @brief   This function is installs the ISR pointed by the function pointer
 *          the input Interrupt ID.
 *          1. Caller       -  Test Suite
 *          2. Prerequisite -  val_gic_create_info_table
 * @param   int_id Interrupt ID to install the ISR
 * @param   isr    Function pointer of the ISR
 * @return  status
 */
acs_status_t val_gic_install_isr(uint32_t int_id, void *isr)
{
    uint32_t      reg_offset = int_id / 32;
    uint32_t      reg_shift  = int_id % 32;
    uint32_t      ret = 0;

    if ((int_id > 1019) || (int_id == 0)) {
        val_print(ACS_LOG_ERR, "\n        Invalid Interrupt ID number %d", int_id);
        return ACS_ERROR;
    }

    ret = pal_gic_install_isr(int_id, isr);
    if (ret)
        return ACS_ERROR;

    if (int_id > 31) {
        /**** UEFI GIC code is not enabling interrupt in the Distributor ***/
        /**** So, do this here as a fail-safe. Remove if PAL guarantees this ***/
        val_mmio_write(g_gicd_base + GICD_ISENABLER + (4 * reg_offset), 1 << reg_shift);
    }

    return ACS_SUCCESS;
}

/**
 * @brief    This function free the registered interrupt line
 *
 * @param int_id   Interrupt line to free
 *
 * @return status
 */
acs_status_t val_gic_free_interrupt(uint32_t int_id)
{
    int32_t ret = 0;

    ret = pal_gic_free_interrupt(int_id);
    if (ret)
        return ACS_ERROR;

    return ACS_SUCCESS;
}

/**
 * @brief    This function generates the interrupt for given
 *           input interrupt id.
 *
 * @param int_id   Interrupt ID.
 *
 * @return status
 */
acs_status_t val_gic_generate_interrupt(uint32_t int_id)
{
    uint64_t base = 0;
    uint32_t index = 0;
    uint32_t num_pe = val_pe_get_num();
    gic_info_entry_t  *gic_entry;
    gic_intr_t gic_intr;

    gic_intr.mask = 1 << (int_id % 32);

    if ((int_id > 0) && (int_id < 32)) {
        gic_entry = g_gic_info_table->gic_info;
        while (gic_entry->type != 0xFF) {
            if (gic_entry->type == ENTRY_TYPE_GICRD) {
                base = gic_entry->base;
                break;
            }
            gic_entry++;
        }
        gic_intr.base = (base + GICD_ISPENDR + (int_id/32) * 4);
        for (index = 0; index < num_pe; index++) {
            base = gic_intr.base + index * 2 * SZ_64KB;
            val_mmio_write(base, gic_intr.mask);
        }
    } else if ((int_id > 31) && (int_id < 1020)) {
        gic_intr.base = (g_gicd_base + GICD_ISPENDR + (int_id/32) * 4);
        val_mmio_write(gic_intr.base, gic_intr.mask);
    } else {
        val_print(ACS_LOG_ERR, "\n        Invalid Interrupt ID number %d", int_id);
        return ACS_ERROR;
    }

    return ACS_SUCCESS;
}

void gic_disable_interrupt(gic_intr_t *gic_intr)
{
    uint64_t base;
    uint32_t index = 0;
    uint32_t num_pe = val_pe_get_num();

    for (index = 0; index < num_pe; index++) {
        base = gic_intr->base + index * 2 * SZ_64KB;
        val_mmio_write(base, gic_intr->mask);
    }
}

/**
 * @brief    This function disables the interrupt in GIC for given
 *           interrupt id.
 *
 * @param int_id   Interrupt ID.
 *
 * @return status
 */
acs_status_t val_gic_disable_interrupt(uint32_t int_id)
{
    uint64_t base = 0;
    gic_info_entry_t  *gic_entry;
    gic_intr_t gic_intr;

    gic_intr.mask = 1 << (int_id % 32);

    if ((int_id > 0) && (int_id < 32)) {
        gic_entry = g_gic_info_table->gic_info;
        while (gic_entry->type != 0xFF) {
            if (gic_entry->type == ENTRY_TYPE_GICRD) {
                base = gic_entry->base;
                break;
            }
            gic_entry++;
        }
        gic_intr.base = (base + GICD_ICENABLER + (int_id/32) * 4);
        gic_disable_interrupt(&gic_intr);
    } else if ((int_id > 31) && (int_id < 1020)) {
        gic_intr.base = (g_gicd_base + GICD_ICENABLER + (int_id/32) * 4);
        val_mmio_write(gic_intr.base, gic_intr.mask);
    } else {
        val_print(ACS_LOG_ERR, "\n        Invalid Interrupt ID number %d", int_id);
        return ACS_ERROR;
    }

    return ACS_SUCCESS;
}

/**
 * @brief   This function writes to end of interrupt register for relevant
 *          interrupt group.
 *          1. Caller       -  Test Suite
 *          2. Prerequisite -  val_gic_create_info_table
 * @param   int_id Interrupt ID for which to disable the interrupt
 * @return  status
 */
acs_status_t val_gic_end_of_interrupt(uint32_t int_id)
{
    uint64_t base = 0;
    uint32_t index = 0;
    uint32_t num_pe = val_pe_get_num();
    gic_info_entry_t  *gic_entry;
    gic_intr_t gic_intr;

    if ((int_id > 0) && (int_id < 32)) {
        gic_entry = g_gic_info_table->gic_info;
        while (gic_entry->type != 0xFF) {
            if (gic_entry->type == ENTRY_TYPE_GICRD) {
                base = gic_entry->base;
                break;
            }
            gic_entry++;
        }
        gic_intr.base = (base + GICD_CLRLPIR);
        for (index = 0; index < num_pe; index++) {
            base = gic_intr.base + index * 2 * SZ_64KB;
            val_mmio_write(base, int_id);
        }
    }
    else if ((int_id > 31) && (int_id < 1020)) {
        val_mmio_write((g_gicd_base + GICD_CLRSPI_NSR), int_id);
    }
    else {
        val_print(ACS_LOG_ERR, "\n        Invalid Interrupt ID number %d", int_id);
        return ACS_ERROR;
    }

    pal_gic_end_of_interrupt(int_id);

    return ACS_SUCCESS;
}

/**
 * @brief   This function routes interrupt to specific PE.
 *          1. Caller       -  Test Suite
 *          2. Prerequisite -  val_gic_create_info_table
 * @param   int_id Interrupt ID to be routed
 * @param   mpidr MPIDR_EL1 reg value of the PE to which the interrupt should be routed
 * @return  status
 */
acs_status_t val_gic_route_interrupt_to_pe(uint32_t int_id, uint64_t mpidr)
{
    uint64_t aff = 0;

    aff = val_gic_mpidr_to_affinity(mpidr);
    if (int_id > 31) {
        val_mmio_write(g_gicd_base + GICD_IROUTER + (8 * int_id), (uint32_t)aff);
        val_mmio_write(g_gicd_base + GICD_IROUTER + (8 * int_id) + 0x4, (uint32_t)(aff >> 32));
        return ACS_SUCCESS;
    } else {
        val_print(ACS_LOG_ERR, "\n        Only SPIs can be routed, INTID = %d cannot be routed",
                                                                                        int_id);
        return ACS_ERROR;
    }
}

/**
 * @brief   This function will return '1' if an interrupt is either pending or active.
 *          1. Caller       -  Test Suite
 *          2. Prerequisite -  val_gic_create_info_table
 * @param   int_id Interrupt ID
 * @return  pending/active status
 */
acs_status_t val_gic_get_interrupt_state(uint32_t int_id, uint32_t *status)
{
    uint32_t reg_offset = int_id / 32;
    uint32_t reg_shift  = int_id % 32;
    uint32_t mask = (1 << reg_shift);
    uint32_t active, pending;

    if (!status)
        return ACS_ERROR;

    pending = val_mmio_read(g_gicd_base + GICD_ISPENDR + (4 * reg_offset));
    active = val_mmio_read(g_gicd_base + GICD_ISACTIVER0 + (4 * reg_offset));

    val_print(ACS_LOG_DEBUG, "\n        pending: %x active: %x", pending, active);
    *status = ((mask & active) || (mask & pending));
    return ACS_SUCCESS;
}

/**
 * @brief   This function will clear an interrupt that is pending or active.
 *          1. Caller       -  Test Suite
 *          2. Prerequisite -  val_gic_create_info_table
 * @param   int_id Interrupt ID
 * @return  none
 */
acs_status_t val_gic_clear_interrupt(uint32_t int_id)
{
    uint32_t reg_offset = int_id / 32;
    uint32_t reg_shift  = int_id % 32;

    if ((int_id > 31) && (int_id < 1020)) {
        val_mmio_write(g_gicd_base + GICD_ICPENDR0 + (4 * reg_offset), (1 << reg_shift));
        val_mmio_write(g_gicd_base + GICD_ICACTIVER0 + (4 * reg_offset), (1 << reg_shift));
    }
    else {
        val_print(ACS_LOG_ERR, "\n        Invalid Interrupt ID number %d", int_id);
        return ACS_ERROR;
    }

    return ACS_SUCCESS;
}

/**
 * @brief   This function converts mpidr to affinity as per GIC
 *
 * @param mpidr   PE mpidr value
 *
 * @return   affinity of the given PE mpidr
 */
uint64_t val_gic_mpidr_to_affinity(uint64_t mpidr)
{
    uint64_t aff;

    aff = ((uint64_t)PE_MPIDR_AFFINITY_LEVEL(mpidr, 3) << 32 |
           PE_MPIDR_AFFINITY_LEVEL(mpidr, 2) << 16 |
           PE_MPIDR_AFFINITY_LEVEL(mpidr, 1) << 8  |
           PE_MPIDR_AFFINITY_LEVEL(mpidr, 0));

    return aff;
}

/**
 * @brief   This function frees the allocated memory for GIC info table
 *
 * @return  none
 */
void val_gic_free_info_table(void)
{
    pal_free_mem((uint64_t *)g_gic_info_table);
}

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

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ShellLib.h>
#include <Protocol/AcpiTable.h>
#include "Foundation/Efi/Guid/Acpi/Acpi.h"
#include "Include/IndustryStandard/Acpi62.h"

#include "include/pal_uefi.h"
#include "include/pal_interface.h"

#define  NO_PROCESSOR_ID 0xFFFFFFFF
#define MPAM_SIMULATION_FVP         0

#if MPAM_SIMULATION_FVP
#define REG_MPAMF_IDR               0x0000
#define REG_MPAMF_CPOR_IDR          0x0030
#define REG_MPAMF_CCAP_IDR          0x0038
#define REG_MPAMF_MSMON_IDR         0x0080
#define REG_MPAMF_CSUMON_IDR        0x0088
#define REG_MSMON_CSU               0x0840

#define IDR_PARTID_MAX_SHIFT        0
#define IDR_PMG_MAX_SHIFT           16
#define IDR_HAS_CCAP_PART_SHIFT     24
#define IDR_HAS_CPOR_PART_SHIFT     25
#define IDR_HAS_MSMON_SHIFT         30
#define MSMON_IDR_CSU_SHIFT         16

#define PARTID_MAX_FVP              10
#define PMG_MAX_FVP                 25
#define CPBM_WD_FVP                 32
#define CMAX_WD_FVP                 8
#define CSU_NUM_MON_FVP             100
#define CSU_VALUE_FVP               0x80000000
                 //Set NRDY = 1 & CSU_VALUE = 0

#define REG_MPAMF_MBW_IDR           0x0040
#define REG_MPAMF_MBWUMON_IDR       0x0090

#define IDR_HAS_MBW_PART_SHIFT      26
#define IDR_BWA_WD_SHIFT            0
#define IDR_HAS_MIN_SHIFT           10
#define IDR_HAS_MAX_SHIFT           11
#define IDR_HAS_PBM_SHIFT           12
#define IDR_BWPBM_WD_SHIFT          16

#define BWA_WD_FVP                  8
#define BWPBM_WD_FVP                32
#endif

/**
 * @brief  Allocates specified amount of memory
 *
 * @param  NumInstances     number of instances
 * @param  SizeOfEntry    size of each entry
 *
 * @retval if SUCCESS    pointer to allocated memory
 * @retval if FAILURE    NULL
 */
STATIC
VOID*
PalMemAllocate (
  UINT32 NumInstances,
  UINT32 SizeOfEntry
  )
{

  EFI_STATUS  Status;
  UINT32*     gMemory = 0;

  Status = gBS->AllocatePool ( EfiBootServicesData,
             (NumInstances * SizeOfEntry),
             (VOID **) &gMemory );
  if (EFI_ERROR(Status)) {
    return NULL;
  }

  pal_pe_data_cache_ops_by_va((UINT64) &gMemory, CLEAN_AND_INVALIDATE);
  return (VOID*) gMemory;
}

/**
 * @brief  Returns MPIDR for the corresponding AcpiProcessorId
 *
 * @param  AcpiProcessorId    AcpiProcessorId for which the corresponding MPIDR has to be returned
 *
 * @retval if SUCCESS    pointer to allocated memory
 * @retval if FAILURE    0
 */
STATIC
UINT64
GetMpidrFromMadt (
  UINT32 AcpiProcessorId
  )
{

    EFI_ACPI_6_1_GIC_STRUCTURE*                          Entry = NULL;
    EFI_ACPI_6_1_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER* MadtHdr;
    UINT32                                               TableLength = 0;
    UINT32                                               Length = 0;

    MadtHdr = (EFI_ACPI_6_1_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER *) pal_get_madt_ptr();
    if (MadtHdr != NULL) {
      TableLength =  MadtHdr->Header.Length;
    }
    else {
      acs_print(ACS_PRINT_DEBUG, L"MADT not found  \n");
      return 0;
    }

    Entry = (EFI_ACPI_6_1_GIC_STRUCTURE *) (MadtHdr + 1);
    Length = sizeof (EFI_ACPI_6_1_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER);

    do {

        if (Entry->Type == EFI_ACPI_6_1_GIC && Entry->AcpiProcessorUid == AcpiProcessorId) {
		  return Entry->MPIDR;
	    }

        Length += Entry->Length;
        Entry = (EFI_ACPI_6_1_GIC_STRUCTURE *) ((UINT8 *)Entry + (Entry->Length));

    } while (Length < TableLength);

    return 0;
}

/**
 * @brief  Initialize MPAM Info table with number of cache, smmu, ddr node counts
 *   and allocates memory for each of the node array
 *
 * @param  MpamTable    Mpam info table to be initialized
 * @param  TableIterator   Pointer to the first node (table) in MPAM Table
 * @param  SizeOfTables    Size of Mpam node (table)s  ie (SizeOf(MPAM_TABLE) - SizeOf(MPAM_HEADER)
 *
 * @retval None
 */
STATIC
VOID
InitializeMpamInfoTable (
  MPAM_INFO_TABLE* MpamTable,
  EFI_ACPI_6_2_MPAM_NODE_HEADER* TableIterator,
  UINT32 SizeOfTables
  )
{

  EFI_ACPI_6_2_MPAM_STRUCTURE_CACHE* MpamCache;
  while (SizeOfTables > 0) {
    SizeOfTables -= TableIterator->Length;
    switch (TableIterator->Type) {
      case MPAM_NODE_SMMU:
              //MpamTable->num_of_smmu_nodes+=1;
              break;
      case MPAM_NODE_CACHE:
              MpamCache = (EFI_ACPI_6_2_MPAM_STRUCTURE_CACHE*) TableIterator;
              if (MpamCache->NodeHeader.BaseAddress == 0) {
                break;
              }
              MpamTable->num_cache_nodes++;
              break;
      case MPAM_NODE_MEMORY:
              MpamTable->num_memory_nodes+=1;
              break;
      default: acs_print (ACS_PRINT_DEBUG, L"Invalid type\n");
               break;
    }

    TableIterator = (EFI_ACPI_6_2_MPAM_NODE_HEADER*) ((CHAR8*)TableIterator + TableIterator->Length);
  }
  MpamTable->cache_node =  PalMemAllocate (MpamTable->num_cache_nodes, sizeof(CACHE_NODE_ENTRY));
  MpamTable->memory_node =  PalMemAllocate (MpamTable->num_memory_nodes, sizeof(MEMORY_NODE_ENTRY));
}

/**
 * @brief  Gets the ACPI processor Id for one of the Private Processor resources for
           the provided cluster Id
 *
 * @param  PpttProcessorStruct  Cluster from which a processor id is to be extracted
 *
 * @retval ACPI processor Id of the first Private Processor Struct
 *         NO_PROCESSOR_ID if no Processor struct present for a cluster
 */

STATIC
UINT64
GetProcessorIdFromCluster (
  EFI_ACPI_6_2_PPTT_STRUCTURE_PROCESSOR* PpttProcessorStruct,
  EFI_ACPI_6_2_PROCESSOR_PROPERTIES_TOPOLOGY_TABLE_HEADER* PpttHdr
  )
{

  UINT32       Iterator;
  CHAR8*       PrivateResource;
  UINT32*      PrivateResourceIterator = (UINT32*) (PpttProcessorStruct+1);
  EFI_ACPI_6_2_PPTT_STRUCTURE_PROCESSOR* PrivateProcessorStruct;

  for (Iterator = 0; Iterator < PpttProcessorStruct->NumberOfPrivateResources; ++Iterator, ++PrivateResourceIterator) {
    PrivateResource = (CHAR8*) PpttHdr;
    PrivateResource += *PrivateResourceIterator;
    if (*PrivateResource == EFI_ACPI_6_2_PPTT_TYPE_PROCESSOR) {
      PrivateProcessorStruct =  (EFI_ACPI_6_2_PPTT_STRUCTURE_PROCESSOR*) PrivateResource;
      return PrivateProcessorStruct->AcpiProcessorId;
    }
  }
  return NO_PROCESSOR_ID;
}

/**
 * @brief  Calculates scope and index fields for MPAM info table cache node,
 *         i.e. whether the cache conde is owned by system/cluster/cpu
 *
 * @param  PpttProcessorStruct  processor whose info field to be calculated
 * @param  PpttHdr      pointer to PPTT Header
 * @param  CacheNode    MPAM info table cache node
 *
 * @retval 1 if success, 0 if failure
 */
STATIC
UINT8
CalculateScopeAndIndex (
  EFI_ACPI_6_2_PPTT_STRUCTURE_PROCESSOR* PpttProcessorStruct,
  EFI_ACPI_6_2_PROCESSOR_PROPERTIES_TOPOLOGY_TABLE_HEADER* PpttHdr,
  CACHE_NODE_ENTRY* CacheNode
  )
{

  UINT64                                 Mpidr;
  CHAR8*                                 NextEntry = (CHAR8*) PpttHdr;
  EFI_ACPI_6_2_PPTT_STRUCTURE_PROCESSOR* ParentProcessor;
  UINT64                                 AcpiProcessorId;

  if (PpttProcessorStruct->Parent == 0) {
    CacheNode->info.node_scope = CACHE_SCOPE_SYSTEM;
  } else {
    NextEntry += PpttProcessorStruct->Parent;
    ParentProcessor = (EFI_ACPI_6_2_PPTT_STRUCTURE_PROCESSOR*) NextEntry;
    if (ParentProcessor->Parent == 0) {
      CacheNode->info.node_scope = CACHE_SCOPE_CLUSTER;
    } else {
      CacheNode->info.node_scope = CACHE_SCOPE_PRIVATE;
    }
  }

  if (CacheNode->info.node_scope == CACHE_SCOPE_CLUSTER) {
    AcpiProcessorId = GetProcessorIdFromCluster(PpttProcessorStruct, PpttHdr);
    if (AcpiProcessorId == NO_PROCESSOR_ID) {
      return 0;
    }
    Mpidr = GetMpidrFromMadt(AcpiProcessorId);
  } else {
    Mpidr = GetMpidrFromMadt(PpttProcessorStruct->AcpiProcessorId);
  }

  if (CacheNode->info.node_scope == CACHE_SCOPE_SYSTEM)  {
    CacheNode->info.scope_index = 0;
  } else if (CacheNode->info.node_scope == CACHE_SCOPE_CLUSTER) {
    CacheNode->info.scope_index = (((Mpidr >> 32) & 0xFF) << 16) | ((Mpidr >>8) & 0xFFFF00);
  } else {
    CacheNode->info.scope_index = Mpidr & 0xFF;
  }
  return 1;
}

/**
 * @brief  copies the attributes field of PpttCacheNode into MPAM info table
 *  cache node attributes
 *
 * @param  CacheNode     Destination
 * @param  PpttCacheNode    Source
 *
 * @retval None
 */
STATIC
VOID
CopyPpttCacheStructIntoCacheNode (
  CACHE_NODE_ENTRY* CacheNode,
  EFI_ACPI_6_2_PPTT_STRUCTURE_CACHE* PpttCacheNode
  )
{

  CacheNode->line_size = PpttCacheNode->LineSize;
  CacheNode->size = PpttCacheNode->Size;
  gBS->CopyMem(&(CacheNode->attributes), &(PpttCacheNode->Attributes), sizeof(CACHE_NODE_ATTR));
}

/**
 * @brief  Fills MPAM info table cache node from PPTT Cache Type Struct fields
 *
 * @param  CacheNode    MPAM info table cache node
 * @param  Offset       Cache Type Structure offset into PPTT table
 *
 * @retval 1   if success
 * @retval 0   if failure
 */
STATIC
UINT8
FillCacheFieldsFromPptt (
  CACHE_NODE_ENTRY* CacheNode,
  UINT32 Offset
  )
{

  EFI_ACPI_6_2_PROCESSOR_PROPERTIES_TOPOLOGY_TABLE_HEADER* PpttHdr;
  UINT32                                                   SizeOfTables;
  PPTT_NODE_HEADER*                                        TableIterator;
  CHAR8*                                                   NextEntry;
  EFI_ACPI_6_2_PPTT_STRUCTURE_CACHE*                       PpttCacheStruct;
  EFI_ACPI_6_2_PPTT_STRUCTURE_PROCESSOR*                   PpttProcessorStruct;
  UINT32                                                   Iterator;
  UINT32*                                                  PrivateResourceNextEntry;

  PpttHdr = (EFI_ACPI_6_2_PROCESSOR_PROPERTIES_TOPOLOGY_TABLE_HEADER*) pal_get_pptt_ptr();
  if (PpttHdr == 0) {
    acs_print(ACS_PRINT_ERR, L" Pptt Not found\n");
    return 0;
  }

  SizeOfTables = PpttHdr->Header.Length -
                 sizeof(EFI_ACPI_6_2_PROCESSOR_PROPERTIES_TOPOLOGY_TABLE_HEADER);
  TableIterator = (PPTT_NODE_HEADER*) (PpttHdr+1);
  NextEntry = (CHAR8*) PpttHdr;

  NextEntry += Offset;
  PpttCacheStruct = (EFI_ACPI_6_2_PPTT_STRUCTURE_CACHE*) NextEntry;

  while (SizeOfTables > 0) {

    SizeOfTables -= TableIterator->Length;
    PpttProcessorStruct = (EFI_ACPI_6_2_PPTT_STRUCTURE_PROCESSOR*) TableIterator;

    if (PpttProcessorStruct->Type == EFI_ACPI_6_2_PPTT_TYPE_PROCESSOR) {
      Iterator = 0;
      TableIterator = (PPTT_NODE_HEADER*) (PpttProcessorStruct+1);
      PrivateResourceNextEntry = (UINT32*) TableIterator;

      for ( ; Iterator < PpttProcessorStruct->NumberOfPrivateResources; ++Iterator) {

        SizeOfTables -= sizeof(UINT32);
        if (*PrivateResourceNextEntry == Offset) {
          CopyPpttCacheStructIntoCacheNode(CacheNode, PpttCacheStruct);
          if (CalculateScopeAndIndex(PpttProcessorStruct, PpttHdr, CacheNode) == 0) {
            return 0;
          }
          return 1;
        }
        PrivateResourceNextEntry += 1;
      }

      TableIterator = (PPTT_NODE_HEADER*) PrivateResourceNextEntry;
      continue;
    }

    NextEntry = (CHAR8*) TableIterator;
    NextEntry += TableIterator->Length;
    TableIterator = (PPTT_NODE_HEADER*) NextEntry;
  }
  return 0;
}

/**
 * @brief Fills MPAM info table cache fields with MPAM table cache fields
 *
 * @param  CacheNode    MPAM info table to be filled
 * @param  MpamCacheNode   MPAM cache
 *
 * @retval None
 */
STATIC
VOID
FillCacheFieldsFromMpam (
  CACHE_NODE_ENTRY* CacheNode,
  EFI_ACPI_6_2_MPAM_STRUCTURE_CACHE* MpamCacheNode
  )
{

  CacheNode->hwreg_base_addr = MpamCacheNode->NodeHeader.BaseAddress;
  CacheNode->not_ready_max_us = MpamCacheNode->NodeHeader.NotReadyMax;
  {
    CacheNode->intr_info.error_intr_num = MpamCacheNode->NodeHeader.ErrorInterrupt;
    CacheNode->intr_info.overflow_intr_num = MpamCacheNode->NodeHeader.OverflowInterrupt;
    if (MpamCacheNode->NodeHeader.ErrorInterruptFlags & 0x1) {
      CacheNode->intr_info.error_intr_type = INTR_LEVEL_TRIGGER;
    } else {
      CacheNode->intr_info.error_intr_type = INTR_EDGE_TRIGGER;
    }


    if (MpamCacheNode->NodeHeader.OverflowInterruptFlags & 0x1) {
      CacheNode->intr_info.overflow_intr_type = INTR_LEVEL_TRIGGER;
    } else {
      acs_print (ACS_PRINT_WARN,L" Unexpected overflow interrupt type for Cache node\n");
      acs_print (ACS_PRINT_WARN,L"          With Overflow Interrupt no: 0x%lx\n",
        MpamCacheNode->NodeHeader.OverflowInterrupt);
      CacheNode->intr_info.overflow_intr_type = INTR_EDGE_TRIGGER;
    }
  }
}

/**
 * @brief Fills MPAM info table memory fields with MPAM table memory fields
 *
 * @param  MemoryNode    MPAM info table to be filled
 * @param  MpamMemoryNode   MPAM Memory
 *
 * @retval None
 */
STATIC
VOID
FillMemoryFieldsFromMpam (
  MEMORY_NODE_ENTRY* MemoryNode,
  EFI_ACPI_6_2_MPAM_STRUCTURE_MEMORY* MpamMemoryNode
  )
{

  MemoryNode->hwreg_base_addr = MpamMemoryNode->NodeHeader.BaseAddress;
  MemoryNode->not_ready_max_us = MpamMemoryNode->NodeHeader.NotReadyMax;
  {
    MemoryNode->intr_info.error_intr_num = MpamMemoryNode->NodeHeader.ErrorInterrupt;
    MemoryNode->intr_info.overflow_intr_num = MpamMemoryNode->NodeHeader.OverflowInterrupt;
    if (MpamMemoryNode->NodeHeader.ErrorInterruptFlags & 0x1) {
      MemoryNode->intr_info.error_intr_type = INTR_LEVEL_TRIGGER;
    } else {
      MemoryNode->intr_info.error_intr_type = INTR_EDGE_TRIGGER;
    }

    if (MpamMemoryNode->NodeHeader.OverflowInterruptFlags & 0x1) {
      MemoryNode->intr_info.overflow_intr_type = INTR_LEVEL_TRIGGER;
    } else {
      acs_print (ACS_PRINT_WARN,L" Unexpected overflow interrupt type for memory node\n");
      acs_print (ACS_PRINT_WARN,L"          With Overflow Interrupt no: 0x%lx\n",
        MpamMemoryNode->NodeHeader.OverflowInterrupt);
      MemoryNode->intr_info.overflow_intr_type = INTR_EDGE_TRIGGER;
     }
  }
}

/**
 * @brief Fills MPAM info table Pptt Memory Affinity Fields

 * @param MemoryNode   destination structure
 * @param SratMemoryAffinityFields source structure
 * @param ProximityDomain  ProximityDomain to which memory belongs to
 *
 * @retval None
 */
STATIC
VOID
FillMemoryFields (
  MEMORY_NODE_ENTRY* MemoryNode,
  MEMORY_AFFINITY_FIELDS *SratMemoryAffinityFields
  )
{
  MemoryNode->base_address = SratMemoryAffinityFields->BaseAddr;
  MemoryNode->length       = SratMemoryAffinityFields->Length;
  MemoryNode->flags        = SratMemoryAffinityFields->Flags;
}

/**
 * @brief Prints the contents of MPAM info table
 * @param MpamInfoTable to be printed
 * @retval None
 */
STATIC
VOID
DumpMpamInfoTable (
  MPAM_INFO_TABLE* MpamTable
  )
{

  UINT32 Iterator = 0;

  acs_print (ACS_PRINT_DEBUG, L" Number Of Cache Node is  %d \n", MpamTable->num_cache_nodes);

  while (Iterator < MpamTable->num_cache_nodes) {
    acs_print (ACS_PRINT_DEBUG, L"    Line size is                   %d \n",
            MpamTable->cache_node[Iterator].line_size);
    acs_print (ACS_PRINT_DEBUG, L"    Size is                        %d \n",
            MpamTable->cache_node[Iterator].size);
    acs_print (ACS_PRINT_DEBUG, L"    Scope is                       %d \n",
            MpamTable->cache_node[Iterator].info.node_scope);
    acs_print (ACS_PRINT_DEBUG, L"    Scope Index is                 %d \n",
            MpamTable->cache_node[Iterator].info.scope_index);
    acs_print (ACS_PRINT_DEBUG, L"    Alloc type:                    %d \n",
            MpamTable->cache_node[Iterator].attributes.alloc_type);
    acs_print (ACS_PRINT_DEBUG, L"    Cache Type:                    %d \n",
            MpamTable->cache_node[Iterator].attributes.cache_type);
    acs_print (ACS_PRINT_DEBUG, L"    Write Policy:                  %d \n",
            MpamTable->cache_node[Iterator].attributes.write_policy);
    acs_print (ACS_PRINT_DEBUG, L"    Hardware Base Addr             0x%lx \n",
            MpamTable->cache_node[Iterator].hwreg_base_addr);
    acs_print (ACS_PRINT_DEBUG, L"    NotReady Max Usage             0x%lx \n",
            MpamTable->cache_node[Iterator].not_ready_max_us);
    acs_print (ACS_PRINT_DEBUG, L"    Error Interrupt Number         0x%lx \n",
            MpamTable->cache_node[Iterator].intr_info.error_intr_num);
    acs_print (ACS_PRINT_DEBUG, L"    Overflow Interrupt Number      0x%lx \n",
            MpamTable->cache_node[Iterator].intr_info.overflow_intr_num);
    acs_print (ACS_PRINT_DEBUG, L"    Error Interrupt Type           0x%lx \n",
            MpamTable->cache_node[Iterator].intr_info.error_intr_type);
    acs_print (ACS_PRINT_DEBUG, L"    Overflow Interrupt Type        0x%lx \n",
            MpamTable->cache_node[Iterator].intr_info.overflow_intr_type);
    acs_print (ACS_PRINT_DEBUG, L"\n");
    ++Iterator;
  }

  acs_print (ACS_PRINT_DEBUG, L"\n Number Of Memory Node is  %d \n", MpamTable->num_memory_nodes);
  Iterator = 0;

  while (Iterator < MpamTable->num_memory_nodes) {
    acs_print (ACS_PRINT_DEBUG, L"    ProximityDomain is             %d \n",
            MpamTable->memory_node[Iterator].proximity_domain);
    acs_print (ACS_PRINT_DEBUG, L"    BaseAddress is                 0x%lx \n",
            MpamTable->memory_node[Iterator].base_address);
    acs_print (ACS_PRINT_DEBUG, L"    Length is                      0x%lx \n",
            MpamTable->memory_node[Iterator].length);
    acs_print (ACS_PRINT_DEBUG, L"    Flags is                       %d \n",
            MpamTable->memory_node[Iterator].flags);
    acs_print (ACS_PRINT_DEBUG, L"    Enabled Flag is                %d \n",
            MpamTable->memory_node[Iterator].flags & 0x1);
    acs_print (ACS_PRINT_DEBUG, L"    Hot-Pluggable Flag is          %d \n",
            MpamTable->memory_node[Iterator].proximity_domain & 0x2);
    acs_print (ACS_PRINT_DEBUG, L"    Non-Volatile Flag is           %d \n",
            MpamTable->memory_node[Iterator].proximity_domain & 0x4);
    acs_print (ACS_PRINT_DEBUG, L"    Hardware Base Addr             0x%lx \n",
            MpamTable->memory_node[Iterator].hwreg_base_addr);
    acs_print (ACS_PRINT_DEBUG, L"    NotReady Max Usage             0x%lx \n",
            MpamTable->memory_node[Iterator].not_ready_max_us);
    acs_print (ACS_PRINT_DEBUG, L"    Error Interrupt Number         0x%lx \n",
            MpamTable->memory_node[Iterator].intr_info.error_intr_num);
    acs_print (ACS_PRINT_DEBUG, L"    Overflow Interrupt Number      0x%lx \n",
            MpamTable->memory_node[Iterator].intr_info.overflow_intr_num);
    acs_print (ACS_PRINT_DEBUG, L"    Error Interrupt Type           0x%lx \n",
            MpamTable->memory_node[Iterator].intr_info.error_intr_type);
    acs_print (ACS_PRINT_DEBUG, L"    Overflow Interrupt Type        0x%lx \n",
            MpamTable->memory_node[Iterator].intr_info.overflow_intr_type);
    acs_print (ACS_PRINT_DEBUG, L"\n");
    acs_print (ACS_PRINT_DEBUG, L"\n");
    ++Iterator;
  }
}

#if MPAM_SIMULATION_FVP
VOID
pal_mpam_nodes_init (
  MPAM_INFO_TABLE *MpamTable
  )
{
  UINT32 Index;
  UINT16 ErrIntr = 900;

  acs_print (ACS_PRINT_DEBUG, L"\n");
  for (Index=0; Index < MpamTable->num_cache_nodes; Index++) {

    acs_print (ACS_PRINT_DEBUG, L" Original BaseAddress   = 0x%lx \n", MpamTable->cache_node[Index].hwreg_base_addr);
    MpamTable->cache_node[Index].hwreg_base_addr = (UINT64)pal_mem_allocate_buf(64*1024);
    acs_print (ACS_PRINT_DEBUG, L" Reassigned BaseAddress = 0x%lx \n", MpamTable->cache_node[Index].hwreg_base_addr);

    /* Configure the default settings for this cache node */
    pal_mmio_write(MpamTable->cache_node[Index].hwreg_base_addr + REG_MPAMF_IDR,
         (1 << IDR_HAS_CPOR_PART_SHIFT) | (1 << IDR_HAS_CCAP_PART_SHIFT) | (1 << IDR_HAS_MSMON_SHIFT) |
          ((PARTID_MAX_FVP+10*Index) << IDR_PARTID_MAX_SHIFT) | ((PMG_MAX_FVP+Index) << IDR_PMG_MAX_SHIFT));
    pal_mmio_write(MpamTable->cache_node[Index].hwreg_base_addr + REG_MPAMF_CPOR_IDR, CPBM_WD_FVP);
    pal_mmio_write(MpamTable->cache_node[Index].hwreg_base_addr + REG_MPAMF_CCAP_IDR, CMAX_WD_FVP);
    pal_mmio_write(MpamTable->cache_node[Index].hwreg_base_addr + REG_MPAMF_MSMON_IDR, (1 << MSMON_IDR_CSU_SHIFT));
    pal_mmio_write(MpamTable->cache_node[Index].hwreg_base_addr + REG_MPAMF_CSUMON_IDR, CSU_NUM_MON_FVP + Index);
    pal_mmio_write(MpamTable->cache_node[Index].hwreg_base_addr + REG_MSMON_CSU, CSU_VALUE_FVP);

    MpamTable->cache_node[Index].intr_info.error_intr_num = ErrIntr--;
    if (Index % 2 == 0)
        MpamTable->cache_node[Index].intr_info.error_intr_type = INTR_LEVEL_TRIGGER;
    else
        MpamTable->cache_node[Index].intr_info.error_intr_type = INTR_EDGE_TRIGGER;
  }

  acs_print (ACS_PRINT_DEBUG, L"\n");
  for (Index=0; Index < MpamTable->num_memory_nodes; Index++) {

    acs_print (ACS_PRINT_DEBUG, L" Original BaseAddress     = 0x%lx \n", MpamTable->memory_node[Index].hwreg_base_addr);
    MpamTable->memory_node[Index].hwreg_base_addr = (UINT64)pal_mem_allocate_buf(64*1024);
    acs_print (ACS_PRINT_DEBUG, L" Reassigned BaseAddress   = 0x%lx \n", MpamTable->memory_node[Index].hwreg_base_addr);

    /* Configure the default settings for this memory node */
    pal_mmio_write(MpamTable->memory_node[Index].hwreg_base_addr + REG_MPAMF_IDR,
          (1 << IDR_HAS_MBW_PART_SHIFT) | (1 << IDR_HAS_MSMON_SHIFT) |
          ((PARTID_MAX_FVP+10*Index) << IDR_PARTID_MAX_SHIFT) | ((PMG_MAX_FVP+Index) << IDR_PMG_MAX_SHIFT));
    pal_mmio_write(MpamTable->memory_node[Index].hwreg_base_addr + REG_MPAMF_MBW_IDR,
          (1 << IDR_HAS_MIN_SHIFT) | (1 << IDR_HAS_MAX_SHIFT) | (1 << IDR_HAS_PBM_SHIFT) |
          (BWA_WD_FVP << IDR_BWA_WD_SHIFT) | (BWPBM_WD_FVP << IDR_BWPBM_WD_SHIFT));

    MpamTable->memory_node[Index].intr_info.error_intr_num = ErrIntr--;
    if (Index % 2 == 0)
        MpamTable->memory_node[Index].intr_info.error_intr_type = INTR_EDGE_TRIGGER;
    else
        MpamTable->memory_node[Index].intr_info.error_intr_type = INTR_LEVEL_TRIGGER;

  }
  acs_print (ACS_PRINT_DEBUG, L"\n");
}
#endif

/**
 * @brief  Populate Mpam info table
 * @param  MpamTable     Mpam info table to be populated
 * @retval None
 */
VOID
pal_mpam_create_info_table (
  MPAM_INFO_TABLE* MpamTable
  )
{

  EFI_ACPI_6_2_MEMORY_PARTITIONING_AND_MONITORING_TABLE_HEADER* MpamHdr;
  UINT32                                                        SizeOfTables;
  EFI_ACPI_6_2_MPAM_NODE_HEADER*                                TableIterator;
  //SMMU_NODE_ENTRY*                                            SmmuNode;
  CACHE_NODE_ENTRY*                                             CacheNode;
  MEMORY_NODE_ENTRY*                                            MemoryNode;
  EFI_ACPI_6_2_MPAM_STRUCTURE_CACHE*                            MpamCacheNode;
  EFI_ACPI_6_2_MPAM_STRUCTURE_MEMORY*                           MpamMemoryNode;
  CHAR8*                                                        NextEntry;
  MEMORY_AFFINITY_FIELDS                                        SratMemoryAffinityFields;

  //MpamTable->num_smmu_nodes = 0;
  MpamTable->num_cache_nodes = 0;
  MpamTable->num_memory_nodes = 0;
  MpamTable->cache_node = NULL;
  //MpamTable->smmu_node = NULL;
  MpamTable->memory_node = NULL;

  if (MpamTable == NULL) {
    acs_print(ACS_PRINT_ERR, L" MpamInfoTable is NULL, exiting\n");
    return;
  }

  if (pal_get_mpam_ptr() == 0) {
    acs_print(ACS_PRINT_ERR, L" Mpam not found\n");
    return;
  } else if (pal_get_madt_ptr() == 0) {
    acs_print(ACS_PRINT_ERR, L" Madt not found\n");
    return;
  } else if (pal_get_pptt_ptr() == 0) {
    acs_print(ACS_PRINT_ERR, L" Pptt not found\n");
    return;
  } else if (pal_get_srat_ptr() == 0) {
    acs_print(ACS_PRINT_ERR, L" Srat not found\n");
    return;
  }

  MpamHdr = (EFI_ACPI_6_2_MEMORY_PARTITIONING_AND_MONITORING_TABLE_HEADER*) pal_get_mpam_ptr();

  SizeOfTables = MpamHdr->Header.Length - sizeof(EFI_ACPI_6_2_MEMORY_PARTITIONING_AND_MONITORING_TABLE_HEADER);
  TableIterator = (EFI_ACPI_6_2_MPAM_NODE_HEADER*) (MpamHdr+1);

  InitializeMpamInfoTable (MpamTable, TableIterator, SizeOfTables);

  CacheNode = MpamTable->cache_node;
  MemoryNode = MpamTable->memory_node;
  //SmmuNode = MpamTable->smmu_node;
  //if (MpamTable->num_smmu_nodes || MpamTable->num_cache_nodes || MpamTable->num_memory_nodes) {
  if (MpamTable->num_cache_nodes > 0 || MpamTable->num_memory_nodes > 0) {

    while (SizeOfTables > 0)  {

      SizeOfTables -= TableIterator->Length;
      switch (TableIterator->Type) {

        case MPAM_NODE_SMMU:
          break;

        case MPAM_NODE_CACHE:
           MpamCacheNode = (EFI_ACPI_6_2_MPAM_STRUCTURE_CACHE*) TableIterator;

           if (MpamCacheNode->NodeHeader.BaseAddress == 0) {
             acs_print(ACS_PRINT_WARN, L" Skipping cache node population due to NULL Base address in MPAM table\n");
             break;
           }

           if (FillCacheFieldsFromPptt(CacheNode, MpamCacheNode->OffsetIntoPptt) == 0) {
             acs_print(ACS_PRINT_WARN, L" Skipping Cluster cache population due to zero PEs within Cluster\n");
             --MpamTable->num_cache_nodes;
             break;
           }

           FillCacheFieldsFromMpam(CacheNode, MpamCacheNode);
           ++CacheNode;
           break;

        case MPAM_NODE_MEMORY:
           MpamMemoryNode = (EFI_ACPI_6_2_MPAM_STRUCTURE_MEMORY*) TableIterator;

           if (pal_get_srat_info (MpamMemoryNode->ProximityDomain, &SratMemoryAffinityFields) == EFI_NOT_FOUND) {
             acs_print (ACS_PRINT_WARN, L" Skipping MEMORY node population due to absence of proximity domain 0x%lx in SRAT table\n", MpamMemoryNode->ProximityDomain);
             --MpamTable->num_memory_nodes;
             break;
           }
           MemoryNode->proximity_domain = MpamMemoryNode->ProximityDomain;
           FillMemoryFields (MemoryNode, &SratMemoryAffinityFields);
           FillMemoryFieldsFromMpam (MemoryNode, MpamMemoryNode);
           ++MemoryNode;
           break;
        default:
           acs_print (ACS_PRINT_WARN, L" Unknown MPAM node type\n");
           break;
      }

      NextEntry = (CHAR8*) TableIterator;
      NextEntry += TableIterator->Length;
      TableIterator = (EFI_ACPI_6_2_MPAM_NODE_HEADER*) NextEntry;
    }
    DumpMpamInfoTable(MpamTable);
#if MPAM_SIMULATION_FVP
    pal_mpam_nodes_init(MpamTable);
#endif
  }

}

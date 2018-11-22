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
#include "Include/IndustryStandard/Acpi61.h"

#include "include/pal_uefi.h"
#include "include/pal_interface.h"
#include "include/acpi_override.h"

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

extern platform_mpam_cfg acpi_override_cfg;
extern PE_INFO_TABLE *g_pe_info_table;

UINT64 Arm64ReadMpidrPAL();


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
 * @brief  Allocates specified amount of memory
 *
 * @param  NumInstances     number of instances
 * @param  SizeOfEntry    size of each entry
 *
 * @retval if SUCCESS    pointer to allocated memory
 * @retval if FAILURE    NULL
 */
STATIC
VOID
PalMemFree (
  VOID *Buffer
  )
{
  gBS->FreePool(Buffer);
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
    acs_print (ACS_PRINT_DEBUG, L"    Error Interrupt Type                   0x%lx \n",
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
            MpamTable->memory_node[Iterator].flags & 0x2);
    acs_print (ACS_PRINT_DEBUG, L"    Non-Volatile Flag is           %d \n",
            MpamTable->memory_node[Iterator].flags & 0x4);
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
    ++Iterator;
  }
}



/**
 *@brief  Counts the number of Clusters in the platform acpi table
 *@param  None
 *@retval ClusterCount from the table
 */
STATIC
UINT32
CountClusters () {

  UINT32  ClusterCount = 0;
  UINT32  AcpiNodeCnt = 0;

  for (; AcpiNodeCnt < acpi_override_cfg.num_cache_nodes; AcpiNodeCnt++) {

    if (acpi_override_cfg.cnode[AcpiNodeCnt].info.node_scope == CACHE_SCOPE_CLUSTER) {
      ++ClusterCount;
    }
  }
  return ClusterCount;
}


/**
 * @brief Initializes PrivateCaches
 *
 * @param PrivateCaches - Array to be initialized
 * @param c             - Count
 *
 * @retval None
 */
STATIC
VOID
SetPeInfoNull (
  PrivatePeCaches* PrivateCaches,
  UINT32 PeCnt
  )
{

    UINT32 PeIndex;

    for (PeIndex = 0; PeIndex < PeCnt; PeIndex++) {
      PrivateCaches[PeIndex].cache = NULL;
    }

}

/**
 * @brief Initializes ClusterCaches
 *
 * @param ClusterCaches - Array to be initialized
 * @param c             - Count
 *
 * @retval None
 */
STATIC
VOID
SetClusterInfoNull(
  mpam_cache_node_cfg** ClusterCaches,
  UINT32 ClusterCnt
  )
{

    UINT32 ClusterIndex;

    for (ClusterIndex = 0; ClusterIndex < ClusterCnt; ClusterIndex++) {
      ClusterCaches = NULL;
      ++ClusterCaches;
    }
}

/**
 * @brief Count number of caches in PrivateCaches
 * @param PrivateCaches - Array to be counted
 * @retval Cache Count
 */
STATIC
UINT32
GetCacheCount (
  PrivatePeCaches* PrivateCaches
  )
{

  UINT32 Count = 0;
  PrivatePeCaches* Iterator = PrivateCaches;

  while (Iterator!=NULL) {
    ++Count;
    Iterator = (PrivatePeCaches*) Iterator->next;
  }

  return Count;
}

/**
 * @brief Find which cluster the Mpidr belongs to
 *
 * @param Clusters     - array of Cluster Pointers
 * @param ClusterCount - Number of Clusters
 * @param Mpidr        - Mpidr to be searched
 *
 * @retval Pointer to the Cluster
 */
STATIC
mpam_cache_node_cfg*
GetClusterNode (
  mpam_cache_node_cfg** Clusters,
  UINT32 ClusterCount,
  UINT64 Mpidr
  )
{

  UINT32 Index = 0;

  while (Index < ClusterCount) {
    if (((Clusters[Index]->mpidr) & 0xFFFFFF00) == (Mpidr & 0xFFFFFF00)) {
      return Clusters[Index];
    }
    ++Index;
  }
  return NULL;
}

/**
 * @brief Copies the two source cache node to destination node
 *
 * @param Source - Source cache node
 * @param Dest   - Destination cache node
 *
 * @retval None
 */
STATIC
VOID
CopyCacheNode (
  mpam_cache_node_cfg* Source,
  CACHE_NODE_ENTRY* Dest
  )
{
  UINT32 Index;

  Dest->line_size = Source->line_size;
  Dest->size      = Source->size;

  for (Index = 0; Index < 4; Index++) {
    Dest->neighbours[Index] = Source->neighbours[Index];
  }

  {
    Dest->info.scope_index = Source->info.scope_index;
    Dest->info.node_scope  = Source->info.node_scope;
  }

  {
    Dest->attributes.alloc_type   = Source->attributes.alloc_type;
    Dest->attributes.cache_type   = Source->attributes.cache_type;
    Dest->attributes.write_policy = Source->attributes.write_policy;
    Dest->attributes.reserved     = Source->attributes.reserved;
  }

  Dest->hwreg_base_addr = Source->hwreg_base_addr;
  Dest->not_ready_max_us = Source->not_ready_max_us;

  {
    Dest->intr_info.error_intr_num     = Source->intr_info.error_intr_num;
    Dest->intr_info.overflow_intr_num  = Source->intr_info.overflow_intr_num;
    Dest->intr_info.error_intr_type    = Source->intr_info.error_intr_type;
    Dest->intr_info.overflow_intr_type = Source->intr_info.overflow_intr_type;
  }
}


/**
 * @brief Copies PrivateCache into MpamTable indexed via pe index
 *
 * @param PrivateCaches - Cache to be copied
 * @param MpamTable     - MpamTable where all the caches are stored
 *
 * @retval None
 */
STATIC
VOID
CopyPrivateCacheNodes (
  PrivatePeCaches* PrivateCaches,
  MPAM_INFO_TABLE* MpamTable,
  UINT32 IsPrimary
  )
{

  UINT32 NodeIndex = 0;
  UINT32 NodeCnt;
  PrivatePeCaches* CacheIterator = PrivateCaches;

  if (IsPrimary) {
     NodeCnt = MpamTable->num_cache_nodes-2;
  } else {
     NodeCnt = MpamTable->num_cache_nodes-1;
  }
  while (NodeIndex < NodeCnt) {
    CopyCacheNode (CacheIterator->cache, &(MpamTable->cache_node[NodeIndex]));
    ++NodeIndex;
    CacheIterator = (PrivatePeCaches*) CacheIterator->next;
  }
}

/**
 * @brief Copies Cluster into MpamTable indexed via pe index
 *
 * @param Cluster    - Cluster to be copied
 * @param MpamTable  - MpamTable where all the caches are stored
 *
 * @retval None
 */
STATIC
VOID
CopyClusterCacheNodes (
  mpam_cache_node_cfg* Cluster,
  MPAM_INFO_TABLE* MpamTable,
  UINT8 IsPrimary
  )
{

  if (IsPrimary) {
    CopyCacheNode (Cluster, &(MpamTable->cache_node[MpamTable->num_cache_nodes-2]));
  } else {
    CopyCacheNode (Cluster, &(MpamTable->cache_node[MpamTable->num_cache_nodes-1]));
  }
}

/**
 * @brief Copies SystemCache into MpamTable
 *
 * @param System      - Cache to be copied
 * @param MpamTable   - MpamTable where all the caches are stored
 *
 * @retval None
 */
STATIC
VOID
CopySystemCacheNode (
  mpam_cache_node_cfg* System,
  MPAM_INFO_TABLE* MpamTable
  )
{
  CopyCacheNode (System, &(MpamTable->cache_node[MpamTable->num_cache_nodes-1]));
}

/**
 * @brief get index for the mpidr
 * @param mpid - MPIDR whose index has to be found
 * @retval index
 */
STATIC
UINT32
pe_get_index_mpid(
  UINT64 mpid
  )
{

 PE_INFO_ENTRY *entry;
 uint32_t i = g_pe_info_table->header.num_of_pe;
 entry = g_pe_info_table->pe_info;

 while (i > 0) {
   if (entry->mpidr == mpid) {
     return entry->pe_num;
   }
   entry++;
   i--;
 }

   /* Return index zero as a safe failsafe value */
   return 0x0;
}


/**
 * @brief Fills MPAM_INFO_TABLE with memory_info
 * @param MpamTable - Table to be filled
 * @retval None
 */
STATIC
VOID
CopyMemoryNodes (
  MPAM_INFO_TABLE* MpamTable
  )
{

  UINT32 I;

  MpamTable->num_memory_nodes = acpi_override_cfg.num_memory_nodes;
  MpamTable->memory_node = PalMemAllocate (MpamTable->num_memory_nodes, sizeof(MEMORY_NODE_ENTRY));

  for (I = 0; I < acpi_override_cfg.num_memory_nodes; ++I) {
    MpamTable->memory_node[I].proximity_domain  = acpi_override_cfg.mnode[I].proximity_domain;
    MpamTable->memory_node[I].base_address      = acpi_override_cfg.mnode[I].base_address;
    MpamTable->memory_node[I].length            = acpi_override_cfg.mnode[I].length;
    MpamTable->memory_node[I].flags             = acpi_override_cfg.mnode[I].flags;
    MpamTable->memory_node[I].hwreg_base_addr   = acpi_override_cfg.mnode[I].hwreg_base_addr;
    MpamTable->memory_node[I].not_ready_max_us  = acpi_override_cfg.mnode[I].not_ready_max_us;

    {
      MpamTable->memory_node[I].intr_info.error_intr_num     = acpi_override_cfg.mnode[I].intr_info.error_intr_num;
      MpamTable->memory_node[I].intr_info.overflow_intr_num  = acpi_override_cfg.mnode[I].intr_info.overflow_intr_num;
      MpamTable->memory_node[I].intr_info.error_intr_type    = acpi_override_cfg.mnode[I].intr_info.error_intr_type;
      MpamTable->memory_node[I].intr_info.overflow_intr_type = acpi_override_cfg.mnode[I].intr_info.overflow_intr_type;
    }
  }
}

/**
 * @brief Frees the Buffer
 * @param PrivateCaches - Buffer to be freed
 * @retval Node
 */
STATIC
VOID
FreePrivateCaches (
  PrivatePeCaches* PrivateCaches
  )
{

  PrivatePeCaches *Iterator;
  PrivatePeCaches *InnerIterator;
  PrivatePeCaches *TempNode;
  UINT32 PeIndex;

  Iterator = PrivateCaches;
  for (PeIndex = 0; PeIndex < g_pe_info_table->header.num_of_pe; PeIndex++) {

    InnerIterator = Iterator;
    InnerIterator = (PrivatePeCaches*)InnerIterator->next;
    while (InnerIterator != NULL) {
      TempNode = InnerIterator;
      InnerIterator = (PrivatePeCaches*)InnerIterator->next;
      PalMemFree ((VOID*)TempNode);
    }
    ++Iterator;
  }
  PalMemFree((VOID*) PrivateCaches);
}

/**
 * @brief Fills MPAM_INFO_TABLE from platform initialization data
 * @param MpamTable Table to be filled
 * @retval None
 *
 *
 *  ############################
 *  #                          #
 *  #Structure of PrivateCaches#
 *  #                          #
 *  ############################
 *  Assumption for this explanation
 *      * Pe 0 is primary
 *  _   ___      ________________    _________________    ______________     ______________
 * |P| |_0_| -> |_private_cache_1|->|_private_cache_2_|->|_cluster_cache_|->|_system_cache_| -- only primary has system cache
 * | |  _|_      ________________    _________________    _______________
 * |E| |_1_| -> |_private_cache_1|->|_private_cache_2_|->|_cluster_cache_| -- Private Caches and Cluster cache
 * | |  _|_      ________________    _________________    _______________       for each PE
 * | | |_2_| -> |_private_cache_1|->|_private_cache_2_|->|_cluster_cache_|
 * | |  _|_      ________________    _________________    _______________
 * |n| |_3_| -> |_private_cache_1|->|_private_cache_2_|->|_cluster_cache_|
 * | |  _|_      ________________    _________________    _______________
 * |u| |_4_| -> |_private_cache_1|->|_private_cache_2_|->|_cluster_cache_|
 * | |  _|_      ________________    _________________    _______________
 * |m| |_5_| -> |_private_cache_1|->|_private_cache_2_|->|_cluster_cache_|
 * | |  _|_      ________________    _________________    _______________
 * | | |_6_| -> |_private_cache_1|->|_private_cache_2_|->|_cluster_cache_|
 * | |  _|_      ________________    _________________    _______________
 * |_| |_7_| -> |_private_cache_1|->|_private_cache_2_|->|_cluster_cache_|
 */

STATIC
MPAM_INFO_TABLE*
FillMpamInfoTable (
  MPAM_INFO_TABLE* MpamTable
  )
{

  UINT32 ClusterCount;
  UINT32 PeIndex;
  UINT32 ClusterIndex = 0;
  UINT32 NodeIndex;
  UINT8 IsPrimary = 0;
  UINT64 CurrentMpidr;
  PrivatePeCaches* PrivateCaches;
  mpam_cache_node_cfg** ClusterCaches;
  mpam_cache_node_cfg* SystemCache;
  PrivatePeCaches *TempNode;
  PrivatePeCaches *NextNode;
  PrivatePeCaches *Iterator;


  MpamTable = PalMemAllocate(g_pe_info_table->header.num_of_pe, sizeof(MPAM_INFO_TABLE));
  ClusterCount = CountClusters();
  PrivateCaches = PalMemAllocate(g_pe_info_table->header.num_of_pe, sizeof(PrivatePeCaches));
  ClusterCaches = PalMemAllocate(ClusterCount, sizeof(mpam_cache_node_cfg*));

  SetPeInfoNull(PrivateCaches, g_pe_info_table->header.num_of_pe);
  SetClusterInfoNull(ClusterCaches, ClusterCount);

  for (NodeIndex = 0; NodeIndex < acpi_override_cfg.num_cache_nodes; NodeIndex++) {

    if  (acpi_override_cfg.cnode[NodeIndex].info.node_scope == CACHE_SCOPE_PRIVATE) {

      PeIndex = pe_get_index_mpid(acpi_override_cfg.cnode[NodeIndex].mpidr);

      if (PrivateCaches[PeIndex].cache == NULL) {
        PrivateCaches[PeIndex].cache = &acpi_override_cfg.cnode[NodeIndex];
        PrivateCaches[PeIndex].next = NULL;
      } else {
        NextNode = (PrivatePeCaches*) PrivateCaches[PeIndex].next;
        TempNode = (PrivatePeCaches*) PalMemAllocate (1, sizeof(PrivatePeCaches));
        TempNode->cache = &acpi_override_cfg.cnode[NodeIndex];
        TempNode->next =  (struct PrivatePeCaches*) NextNode;
        PrivateCaches[PeIndex].next = (struct PrivatePeCaches*) TempNode;
      }
    } else if ( acpi_override_cfg.cnode[NodeIndex].info.node_scope == CACHE_SCOPE_CLUSTER) {

      ClusterCaches[ClusterIndex] = &acpi_override_cfg.cnode[NodeIndex];
      ++ClusterIndex;
    } else {
      SystemCache = &acpi_override_cfg.cnode[NodeIndex];
    }
  }

  Iterator = PrivateCaches;
  IsPrimary = 0;

  for (PeIndex = 0; PeIndex < g_pe_info_table->header.num_of_pe; PeIndex++) {

    CurrentMpidr = Arm64ReadMpidrPAL();
    CurrentMpidr = (((CurrentMpidr >> 32) & 0xFF) << 24) | (CurrentMpidr & 0xFFFFFF);

    if (Iterator->cache->mpidr == CurrentMpidr) {
      IsPrimary = 1;
    }

    if (IsPrimary) {
      /* Add cluster cache plus system cache to the private cache node cnt */
      MpamTable[PeIndex].num_cache_nodes = GetCacheCount(&PrivateCaches[PeIndex])+2;
    } else {
      /* Add cluster cache to the private cache node cnt */
      MpamTable[PeIndex].num_cache_nodes = GetCacheCount(&PrivateCaches[PeIndex])+1;
    }

    MpamTable[PeIndex].num_memory_nodes = 0;
    MpamTable[PeIndex].cache_node = PalMemAllocate (MpamTable->num_cache_nodes, sizeof(CACHE_NODE_ENTRY));
    CopyPrivateCacheNodes(Iterator, &MpamTable[PeIndex], IsPrimary);
    CopyClusterCacheNodes(GetClusterNode(ClusterCaches, ClusterCount, PrivateCaches[PeIndex].cache->mpidr),
                            &MpamTable[PeIndex],
                            IsPrimary
                            );

    if (IsPrimary) {
      CopySystemCacheNode(SystemCache, &MpamTable[PeIndex]);
    }

    ++Iterator;
    IsPrimary = 0;
  }

  CopyMemoryNodes (&MpamTable[pe_get_index_mpid(CurrentMpidr)]);



  for (PeIndex = 0; PeIndex < g_pe_info_table->header.num_of_pe; PeIndex++) {

    acs_print (ACS_PRINT_DEBUG, L"\nDumping Mpam info for pe_index:  %d\n", PeIndex);
    DumpMpamInfoTable(&MpamTable[PeIndex]);
  }

  FreePrivateCaches(PrivateCaches);
  PalMemFree((VOID*)ClusterCaches);


 return MpamTable;

}

#if MPAM_SIMULATION_FVP
VOID
pal_mpam_nodes_init (
  MPAM_INFO_TABLE *MpamTable
  )
{
  UINT32 Index;
  UINT16 ErrIntr = 900;

  for (Index=0; Index < MpamTable->num_cache_nodes; Index++) {
    acs_print (ACS_PRINT_DEBUG, L" Original BaseAddress   = 0x%lx \n", MpamTable->cache_node[Index].hwreg_base_addr);
    MpamTable->cache_node[Index].hwreg_base_addr = (UINT64)pal_mem_allocate_buf(64*1024);
    acs_print (ACS_PRINT_DEBUG, L" Reassigned BaseAddress = 0x%lx \n", MpamTable->cache_node[Index].hwreg_base_addr);

    /* Configure the default settings for this cache node */
    pal_mmio_write(MpamTable->cache_node[Index].hwreg_base_addr + REG_MPAMF_IDR,
         (1 << IDR_HAS_CPOR_PART_SHIFT) | (1 << IDR_HAS_CCAP_PART_SHIFT) | (1 << IDR_HAS_MSMON_SHIFT) |
          ((PARTID_MAX_FVP+10*Index) << IDR_PARTID_MAX_SHIFT) | ((PMG_MAX_FVP+Index) << IDR_PMG_MAX_SHIFT));
    pal_mmio_write(MpamTable->cache_node[Index].hwreg_base_addr + REG_MPAMF_CPOR_IDR, CPBM_WD_FVP);
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

    MpamTable->memory_node[Index].intr_info.error_intr_num = ErrIntr--;
    if (Index % 2 == 0)
        MpamTable->memory_node[Index].intr_info.error_intr_type = INTR_EDGE_TRIGGER;
    else
        MpamTable->memory_node[Index].intr_info.error_intr_type = INTR_LEVEL_TRIGGER;

    /* Configure the default settings for this dram node */
    pal_mmio_write(MpamTable->memory_node[Index].hwreg_base_addr + REG_MPAMF_IDR,
          ((PARTID_MAX_FVP+10*Index) << IDR_PARTID_MAX_SHIFT) | ((PMG_MAX_FVP+Index) << IDR_PMG_MAX_SHIFT));
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
  MPAM_INFO_TABLE** MpamTable
  )
{

  if (MpamTable == NULL) {
    acs_print(ACS_PRINT_ERR, L" MpamInfoTable is NULL, exiting\n");
    return;
  }

  *MpamTable = FillMpamInfoTable(*MpamTable);

#if MPAM_SIMULATION_FVP
  pal_mpam_nodes_init(*MpamTable);
#endif

}

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
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include "Include/IndustryStandard/Acpi61.h"
#include <Protocol/AcpiTable.h>
#include <Protocol/Cpu.h>

#include "pal_uefi.h"

static   EFI_ACPI_6_1_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER *gMadtHdr;
UINT8   *gSecondaryPeStack;
UINT64  gMpidrMax;
pe_shared_mem_t *g_pe_shared_mem;
pe_info_table_t *gPeTable;

#define SIZE_STACK_SECONDARY_PE  0x200		//512 bytes per core
#define UPDATE_AFF_MAX(src,dest,mask)  ((dest & mask) > (src & mask) ? (dest & mask) : (src & mask))
#define PeStackTop(i)                  ((UINT64)gSecondaryPeStack + (i-1) *  SIZE_STACK_SECONDARY_PE)
#define PSCI_STATE_POWER_DOWN (1 << 16)

UINT64
pal_get_madt_ptr();
VOID
ArmCallSmc (
  IN OUT ARM_SMC_ARGS *Args
  );

VOID
PsciCpuSuspend (
  IN OUT ARM_SMC_ARGS *Args
  );

UINT64
ArmReadMpidr (
  VOID
  );

VOID
ModuleEntryPoint();

UINT64
GetStackSize()
{
    return SIZE_STACK_SECONDARY_PE;
}

/**
  @brief   Return the base address of the region allocated for Stack use for the Secondary
           PEs.
  @param   None
  @return  base address of the Stack
**/
UINT64
PalGetSecondaryStackBase()
{
  return (UINT64)gSecondaryPeStack;
}

/**
  @brief   Returns the Max of each 8-bit Affinity fields in MPIDR.
  @param   None
  @return  Max MPIDR
**/
UINT64
PalGetMaxMpidr()
{
  return gMpidrMax;
}

VOID
PeSetData(UINT64 *Addr, UINT64 Payload, UINT64 Arg)
{
    *Addr = Payload;
    *(Addr + 1) = Arg;
    DataCacheCleanInvalidateVA((UINT64)Addr);
}

VOID PeGetData(UINT64 *Addr, UINT64 *ResAddr, UINT64 *Arg)
{
    *ResAddr = *Addr;
    *Arg = *(Addr + 1);
}

STATIC VOID
PowerOnPe(UINT64 Mpidr) {
    ARM_SMC_ARGS ArmSmcArgs;
    ArmSmcArgs.Arg0 = ARM_SMC_ID_PSCI_CPU_ON_AARCH64;
    ArmSmcArgs.Arg1 = Mpidr;
    ArmSmcArgs.Arg2 = (UINT64)ModuleEntryPoint;
    ArmCallSmc(&ArmSmcArgs);
}

STATIC VOID
PowerOffPe() {
    ARM_SMC_ARGS ArmSmcArgs;
    ArmSmcArgs.Arg0 = ARM_SMC_ID_PSCI_CPU_OFF;
    ArmSmcArgs.Arg1 = PSCI_STATE_POWER_DOWN;
    ArmCallSmc(&ArmSmcArgs);
}

STATIC UINT32
PsciPowerStateIdExtended(VOID)
{
    ARM_SMC_ARGS ArmSmcArgs;
    ArmSmcArgs.Arg0 = ARM_SMC_ID_PSCI_FEATURES;
    ArmSmcArgs.Arg1 = ARM_SMC_ID_PSCI_CPU_SUSPEND_AARCH64;
    ArmCallSmc(&ArmSmcArgs);
    return (ArmSmcArgs.Arg0 >> 1) & 0x1;
}

VOID
pal_pe_suspend(UINT32 PowerDown)
{
    UINT32 PowerState = 0;
    if (PowerDown) {
        if (PsciPowerStateIdExtended())
            PowerState = (1 << 30);
        else
            PowerState = (1 << 16);
    }
    ARM_SMC_ARGS ArmSmcArgs;
    ArmSmcArgs.Arg0 = ARM_SMC_ID_PSCI_CPU_SUSPEND_AARCH64;
    ArmSmcArgs.Arg1 = PowerState;
    ArmSmcArgs.Arg2 = 0;
    ArmSmcArgs.Arg3 = 0;
    PsciCpuSuspend(&ArmSmcArgs);
}

VOID
pal_pe_poweroff(UINT32 Index) {
    PeSetData((UINT64*)PeStackTop(Index), (UINT64)PowerOffPe, 0);
}

VOID
pal_pe_poweron(UINT64 Mpidr) {
    PowerOnPe(Mpidr);
}

INT32
pal_invoke_psci_fn(UINT64 Arg0, UINT64 Arg1, UINT64 Arg2, UINT64 Arg3)
{
    ARM_SMC_ARGS ArmSmcArgs;
    ArmSmcArgs.Arg0 = Arg0;
    ArmSmcArgs.Arg1 = Arg1;
    ArmSmcArgs.Arg2 = Arg2;
    ArmSmcArgs.Arg3 = Arg3;
    ArmCallSmc(&ArmSmcArgs);
    return ArmSmcArgs.Arg0;
}


VOID
CEntryPoint(UINT64 Addr) {
    UINT64 ResAddr = 0, Arg = 0;
    while (1) {
        while (!ResAddr) {
            DataCacheInvalidateVA(Addr);
            DataCacheInvalidateVA(Addr+8);
            ResAddr = *(UINT64*)Addr;
            Arg = *((UINT64*)Addr + 1);
        }
        ((VOID (*)(VOID*))ResAddr)((VOID*)Arg);
        *(UINT64*)Addr = 0;
        ResAddr = 0;
    }
}


/**
  @brief  Allocate memory region for secondary PE stack use. SIZE of stack for each PE
          is a #define

  @param  Number of PEs

  @return  None
**/
VOID
PalAllocateSecondaryStack(UINT64 mpidr)
{
  EFI_STATUS Status;
  UINT32 NumPe, Aff0, Aff1, Aff2, Aff3;

  Aff0 = ((mpidr & 0x00000000ff) >>  0);
  Aff1 = ((mpidr & 0x000000ff00) >>  8);
  Aff2 = ((mpidr & 0x0000ff0000) >> 16);
  Aff3 = ((mpidr & 0xff00000000) >> 32);

  NumPe = ((Aff3+1) * (Aff2+1) * (Aff1+1) * (Aff0+1));

  if (gSecondaryPeStack == NULL) {
      Status = gBS->AllocatePool ( EfiBootServicesData,
                    (NumPe * SIZE_STACK_SECONDARY_PE),
                    (VOID **) &gSecondaryPeStack);
      if (EFI_ERROR(Status)) {
          pal_print(ACS_LOG_ERR, "\n        FATAL-Allocation for Seconday stack failed %x", Status);
      }
      DataCacheCleanInvalidateVA((UINT64)&gSecondaryPeStack);
  }
}

/**
  @brief  This API fills in the PE_INFO Table with information about the PEs in the
          system. This is achieved by parsing the ACPI - MADT table.

  @param  PeTable  - Address where the PE information needs to be filled.

  @return  None
**/
VOID
pal_pe_create_info_table(pe_info_table_t *PeTable)
{
  EFI_ACPI_6_1_GIC_STRUCTURE    *Entry = NULL;
  pe_info_entry_t               *Ptr = NULL;
  UINT32                        TableLength = 0;
  UINT32                        Length = 0, Index;
  UINT64                        MpidrAff0Max = 0, MpidrAff1Max = 0, MpidrAff2Max = 0, MpidrAff3Max = 0;


  if (PeTable == NULL) {
    pal_print(ACS_LOG_ERR, "\n        Input PE Table Pointer is NULL. Cannot create PE INFO");
    return;
  }

  gMadtHdr = (EFI_ACPI_6_1_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER *) pal_get_madt_ptr();
  gPeTable = PeTable;

  if (gMadtHdr != NULL) {
    TableLength =  gMadtHdr->Header.Length;
    pal_print(ACS_LOG_INFO, "\n        MADT is at %x and length is %x", gMadtHdr, TableLength);
  } else {
    pal_print(ACS_LOG_ERR, "\n        MADT not found");
    return;
  }

  PeTable->header.num_of_pe = 0;

  Entry = (EFI_ACPI_6_1_GIC_STRUCTURE *) (gMadtHdr + 1);
  Length = sizeof (EFI_ACPI_6_1_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER);
  Ptr = PeTable->pe_info;

  do {

    if (Entry->Type == EFI_ACPI_6_1_GIC) {
      //Fill in the cpu num and the mpidr in pe info table
      Ptr->mpidr    = Entry->MPIDR;
      Ptr->pe_num   = PeTable->header.num_of_pe;
      Ptr->pmu_gsiv = Entry->PerformanceInterruptGsiv;
      pal_print(ACS_LOG_DEBUG, "\n        MPIDR %x PE num %x", Ptr->mpidr, Ptr->pe_num);
      DataCacheCleanInvalidateVA((UINT64)Ptr);
      Ptr++;
      PeTable->header.num_of_pe++;

      MpidrAff0Max = UPDATE_AFF_MAX(MpidrAff0Max, Entry->MPIDR, 0x000000ff);
      MpidrAff1Max = UPDATE_AFF_MAX(MpidrAff1Max, Entry->MPIDR, 0x0000ff00);
      MpidrAff2Max = UPDATE_AFF_MAX(MpidrAff2Max, Entry->MPIDR, 0x00ff0000);
      MpidrAff3Max = UPDATE_AFF_MAX(MpidrAff3Max, Entry->MPIDR, 0xff00000000);
    }

    Length += Entry->Length;
    Entry = (EFI_ACPI_6_1_GIC_STRUCTURE *) ((UINT8 *)Entry + (Entry->Length));

  }while (Length < TableLength);

  gMpidrMax = MpidrAff0Max | MpidrAff1Max | MpidrAff2Max | MpidrAff3Max;
  PalAllocateSecondaryStack(gMpidrMax);
  DataCacheCleanInvalidateVA((UINT64)&gMpidrMax);
  DataCacheCleanInvalidateVA((UINT64)PeTable);

  Ptr = PeTable->pe_info;
  Ptr++;
  for (Index = 1; Index < PeTable->header.num_of_pe; Index++) {
      PowerOnPe(Ptr->mpidr);
      Ptr++;
  }

}

int pal_pe_execute_on_all(int num_pe, VOID *payload, UINT64 arg) {
    int i;
    UINT64 *Addr, MyMpidr = ArmReadMpidr() & (~(0xffULL << 24));
    pe_info_entry_t *Ptr = gPeTable->pe_info;

    ((VOID(*)(VOID*))payload)((VOID*)arg);
    for (i = 0; i < num_pe; i++, Ptr++) {
       if (MyMpidr == Ptr->mpidr)
           continue;
       Addr = (UINT64*)PeStackTop(i);
       *(Addr+1) = arg;
       *Addr = (UINT64)payload;
       DataCacheCleanInvalidateVA((UINT64)(Addr + 1));
       DataCacheCleanInvalidateVA((UINT64)Addr);
       while (1) {
           DataCacheInvalidateVA((UINT64)Addr);
           if (*Addr == 0)
               break;
       }
    }
    return 0;
}

void pal_pe_clean_up() {
    for (int i = 1; i < gPeTable->header.num_of_pe; i++) {
        PeSetData((UINT64*)PeStackTop(i), (UINT64)PowerOffPe, 0);
    }
}

/**
@brief  Allocate memory which is to be used to share data across PEs

@param  num_pe      - Number of PEs in the system
@param  sizeofentry - Size of memory region allocated to each PE

@return None
**/
acs_status_t
pal_pe_alloc_shared_mem(UINT32 num_pe, UINT32 sizeofentry)
{
    EFI_STATUS Status;
    g_pe_shared_mem = NULL;

    Status = gBS->AllocatePool ( EfiBootServicesData,
                 (num_pe * sizeofentry),
                 (VOID **) &g_pe_shared_mem );

    pal_print(ACS_LOG_INFO, "\n        Shared memory is %llx", g_pe_shared_mem);

    if (EFI_ERROR(Status)) {
        pal_print(ACS_LOG_ERR, "\n        Allocate Pool shared memory failed %x", Status);
    }
    DataCacheCleanInvalidateVA((UINT64)&g_pe_shared_mem);
    return ACS_SUCCESS;
}

void
pal_pe_free_shared_mem()
{
    gBS->FreePool((VOID*)g_pe_shared_mem);
}

void
pal_pe_data_cache_invalidate(UINT64 Addr)
{
    DataCacheInvalidateVA(Addr);
}

void
pal_pe_data_cache_clean_invalidate(UINT64 Addr)
{
    DataCacheCleanInvalidateVA(Addr);
}

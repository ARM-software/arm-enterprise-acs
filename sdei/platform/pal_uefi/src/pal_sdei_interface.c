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

#include "Foundation/Efi/Guid/Acpi/Acpi.h"
#include <Protocol/AcpiTable.h>
#include "Include/IndustryStandard/Acpi61.h"

#include "pal_uefi.h"
#include "pal_sdei_interface.h"

#define GHES_VERSION_2 10
#define GHES_NOTIFY_TYPE_SDEI 0xb

/*
 * The call to use to reach the firmware.
 */
void (*sdei_firmware_call)(ARM_SMC_ARGS *args);
typedef struct {
    UINT32 a; //TODO to remove once supported
} EFI_ACPI_6_2_SOFTWARE_DELEGATION_EXCEPTION_INTERFACE;
static EFI_ACPI_6_2_SOFTWARE_DELEGATION_EXCEPTION_INTERFACE *gSdeiHdr; //TODO ADD SDEI SPECIFIC SIGNATURE

UINT64
pal_get_sdei_ptr();

UINT64
pal_get_fadt_ptr();

UINT64
pal_get_hest_ptr();

void
ArmCallSmc (
  IN OUT ARM_SMC_ARGS *Args
  );

void
ArmCallHvc (
  IN OUT ARM_SMC_ARGS *Args
  );

int pal_pe_get_mpidr_cpu_id(int cpu, UINT64 *fw_cpu)
{
    int err = SDEI_STATUS_INVALID;
    //TODO

    return err;
}

int pal_pe_get_cpu_id_mpidr(UINT64 fw_cpu, int *cpu)
{
    int err = SDEI_STATUS_INVALID;
    //TODO

    return err;
}

int pal_sdei_to_uefi_errno(unsigned long sdei_err)
{
    switch (sdei_err) {
    case SDEI_NOT_SUPPORTED:
        return SDEI_STATUS_NOT_SUPPORTED;
    case SDEI_INVALID_PARAMETERS:
        return SDEI_STATUS_INVALID;
    case SDEI_DENIED:
        return SDEI_STATUS_DENIED;
    case SDEI_PENDING:
        return SDEI_STATUS_PENDING;
    case SDEI_OUT_OF_RESOURCE:
        return SDEI_STATUS_OUT_OF_RESOURCE;
    }

    /* Not an error value ... */
    return sdei_err;
}

/*
 * If x0 is any of these values, then the call failed, use sdei_to_uefi_errno()
 * to translate.
 */
static int sdei_is_err(UINT64 res)
{
    switch (res) {
    case SDEI_NOT_SUPPORTED:
    case SDEI_INVALID_PARAMETERS:
    case SDEI_DENIED:
    case SDEI_PENDING:
    case SDEI_OUT_OF_RESOURCE:
        return TRUE;
    }

    return FALSE;
}

int pal_invoke_sdei_fn(unsigned long function_id, unsigned long arg0,
              unsigned long arg1, unsigned long arg2,
              unsigned long arg3, unsigned long arg4,
              UINT64 *result)
{
    int err = 0;
    UINT64 res;
    ARM_SMC_ARGS args;
    args.Arg0 = function_id;
    args.Arg1 = arg0;
    args.Arg2 = arg1;
    args.Arg3 = arg2;
    args.Arg4 = arg3;
    args.Arg5 = arg4;

    ArmCallSmc(&args);
    res = args.Arg0;

    if (sdei_is_err(res))
        err = pal_sdei_to_uefi_errno(res);

    if (result)
        *result = res;

    return err;
}

void pal_execute_eachpe(void *func, void *info, int wait){

    //TODO;
}

void *pal_intf_alloc(int size){
    return 0;
}

void pal_intf_free(void *handle) {

}

unsigned int pal_smp_pe_id(void) {

    //TODO;
    return 0;
}

void pal_intf_lock(void) {

}

void pal_intf_unlock(void) {

}

/**
  @brief  Make the SMC call using AARCH64 Assembly code
          SMC calls can take up to 7 arguments and return up to 4 return values.
          Therefore, the 4 first fields in the ARM_SMC_ARGS structure are used
          for both input and output values.

  @param  Argumets to pass to the EL3 firmware

  @return  None
**/
VOID
pal_call_smc(ARM_SMC_ARGS *ArmSmcArgs)
{
  ArmCallSmc (ArmSmcArgs);
}

void pal_interface_broken(void)
{
    sdei_firmware_call = NULL;
}

int pal_conduit_get(void)
{
    sdei_firmware_call = &pal_call_smc;
    return CONDUIT_SMC;
}

int pal_acpi_present(void)
{
    gSdeiHdr = (EFI_ACPI_6_2_SOFTWARE_DELEGATION_EXCEPTION_INTERFACE *) pal_get_sdei_ptr(); //TODO ADD SDEI SPECIFIC SIGNATURE

    if (gSdeiHdr == NULL) {
      pal_print(ACS_LOG_ERR, "\n        SDEI entry not found in ACPI table");
      return FALSE;
    }

    return TRUE;
}

/*
 * Read ACPI Hardware Error Source table and intialize the event info
 * table with event numbers read from Generic Hardware Error Source
 * structures in the table.
 */
int pal_sdei_create_event_info_table(event_info_table_t *EventTable)
{
    EFI_ACPI_6_1_HARDWARE_ERROR_SOURCE_TABLE_HEADER *Hest = NULL;
    EFI_ACPI_6_1_GENERIC_HARDWARE_ERROR_SOURCE_STRUCTURE *Ghes;
    event_info_t *event;
    int i;

    EventTable->num_events = 0;
    EventTable->hest_found = 0;
    EventTable->num_ghes_notify = 0;
    Hest = (EFI_ACPI_6_1_HARDWARE_ERROR_SOURCE_TABLE_HEADER *) pal_get_hest_ptr();
    if (Hest == NULL)
        return -1;

    //EventTable->num_events = Hest->ErrorSourceCount;
    EventTable->hest_found = 1;
    event = (event_info_t*) EventTable->info;

    Ghes = (EFI_ACPI_6_1_GENERIC_HARDWARE_ERROR_SOURCE_STRUCTURE*)(Hest + 1);
    for (i = 0; i < Hest->ErrorSourceCount; i++) {
        if (Ghes->NotificationStructure.Type == GHES_NOTIFY_TYPE_SDEI) {
            event->number = Ghes->NotificationStructure.Vector;
            pal_print(ACS_LOG_DEBUG, "\n        Found SDEI error event %d", event->number);
            EventTable->num_ghes_notify++;
            if (event->number)
                EventTable->num_events++;
            event++;
        }
        if (Ghes->Type == GHES_VERSION_2)
            Ghes = (EFI_ACPI_6_1_GENERIC_HARDWARE_ERROR_SOURCE_STRUCTURE*)
            (UINT8*)Ghes + sizeof(EFI_ACPI_6_1_GENERIC_HARDWARE_ERROR_SOURCE_VERSION_2_STRUCTURE);
        else
            Ghes++;

    }
    return 0;
}


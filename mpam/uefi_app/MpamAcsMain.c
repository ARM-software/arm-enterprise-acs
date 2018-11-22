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
#include <Library/ShellCEntryLib.h>
#include <Library/ShellLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Protocol/LoadedImage.h>

#include "val/include/val_infra.h"
#include "val/include/val_pe.h"

#include "MpamAcs.h"


UINT32  g_print_level;
UINT32  g_execute_secure;
UINT32  g_skip_test_num[3] = {10000, 10000, 10000};
UINT32  g_acs_tests_total;
UINT32  g_acs_tests_pass;
UINT32  g_acs_tests_fail;
UINT64  g_stack_pointer;
UINT64  g_exception_ret_addr;
UINT64  g_ret_addr;
SHELL_FILE_HANDLE g_acs_log_file_handle;

STATIC
VOID
FlushImage (
    VOID
    )
{

    EFI_LOADED_IMAGE_PROTOCOL   *ImageInfo;
    EFI_STATUS Status;
    Status = gBS->HandleProtocol (gImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **)&ImageInfo);
    if(EFI_ERROR (Status)) {
        return;
    }

    val_pe_cache_clean_range((UINT64)ImageInfo->ImageBase, (UINT64)ImageInfo->ImageSize);
}

EFI_STATUS
CreatePeInfoTable (
)
{

    EFI_STATUS Status;
    UINT64   *PeInfoTable;

    /*
     * Allowing room for growth, at present each entry is 16 bytes,
     * so we can support upto 511 PEs with 8192 bytes
     */
    Status = gBS->AllocatePool ( EfiBootServicesData,
                                   PE_INFO_TBL_SZ,
                                   (VOID **) &PeInfoTable );

    if (EFI_ERROR(Status)) {
        Print(L"Allocate Pool failed %x \n", Status);
        return Status;
    }

    Status = val_pe_create_info_table(PeInfoTable);

    return Status;
}

EFI_STATUS
CreateGicInfoTable (
)
{

    EFI_STATUS Status;
    UINT64     *GicInfoTable;

     Status = gBS->AllocatePool (EfiBootServicesData,
                                    GIC_INFO_TBL_SZ,
                                    (VOID **) &GicInfoTable);
     if (EFI_ERROR(Status)) {
         Print(L"Allocate Pool failed %x \n", Status);
         return Status;
     }

     Status = val_gic_create_info_table(GicInfoTable);

     return Status;
}

EFI_STATUS
CreateMpamInfoTable (
)
{

    return val_mpam_create_info_table();
}

VOID
FreeMpamAcsMem (
)
{
    val_pe_free_info_table();
    val_gic_free_info_table();
    val_free_shared_mem();
}

VOID
HelpMsg (
    VOID
    )
{

    Print (L"\nUsage: Mpam.efi [-v <n>] | [-f <filename>] | [-s] | [-skip <n>]\n"
             "Options:\n"
             "-v      Verbosity of the Prints\n"
             "        1 shows all prints, 5 shows Errors\n"
             "        As per MPAM spec, 0 to 3\n"
             "-f      Name of the log file to record the test results in\n"
             "-s      Enable the execution of secure tests\n"
             "-skip   Test(s) to be skipped\n"
             "        Refer to section 4 of MPAM_ACS_User_Guide\n"
             "        To skip a module, use Model_ID as mentioned in user guide\n"
             "        To skip a particular test within a module, use the exact testcase number\n"
             );
}

STATIC CONST SHELL_PARAM_ITEM ParamList[] = {
    {L"-v"    , TypeValue},    // -v    # Verbosity of the Prints. 1 shows all prints, 5 shows Errors
    {L"-f"    , TypeValue},    // -f    # Name of the log file to record the test results in.
    {L"-s"    , TypeFlag},     // -s    # Binary Flag to enable the execution of secure tests.
    {L"-skip" , TypeValue},    // -skip # test(s) to skip execution
    {L"-help" , TypeFlag},     // -help # help : info about commands
    {L"-h"    , TypeFlag},     // -h    # help : info about commands
    {NULL     , TypeMax}
};

/**
 * @brief   MPAM Compliance Suite Entry Point.
 *
 * Call the Entry points of individual modules.
 *
 * @retval  0       The application exited normally.
 * @retval  Other   An error occurred.
 */
INTN
EFIAPI
ShellAppMain (
    IN UINTN Argc,
    IN CHAR16 **Argv
    )
{

    LIST_ENTRY         *ParamPackage;
    CONST CHAR16       *CmdLineArg;
    CHAR16             *ProbParam;
    UINT32             Status;
    UINT32             i,j=0;
    VOID               *branch_label;

    /* Process Command Line arguments */
    Status = ShellInitialize();
    Status = ShellCommandLineParse (ParamList, &ParamPackage, &ProbParam, TRUE);
    if (Status) {
        Print(L"Shell command line parse error %x\n", Status);
        Print(L"Unrecognized option %s passed\n", ProbParam);
        HelpMsg();
        return SHELL_INVALID_PARAMETER;
    }

    for (i=1 ; i<Argc ; i++) {
        if (StrCmp(Argv[i], L"-skip") == 0) {
            CmdLineArg = Argv[i+1];
            i = 0;
            break;
        }
    }

    if (i == 0) {
        for (i=0 ; i < StrLen(CmdLineArg) ; i++) {
            g_skip_test_num[0] = StrDecimalToUintn((CONST CHAR16 *)(CmdLineArg+0));
            if (*(CmdLineArg+i) == L',') {
                g_skip_test_num[++j] = StrDecimalToUintn((CONST CHAR16 *)(CmdLineArg+i+1));
            }
        }
    }

    if (ShellCommandLineGetFlag (ParamPackage, L"-s")) {
        g_execute_secure = TRUE;
    } else {
        g_execute_secure = FALSE;
    }

    /* Options with Values */
    CmdLineArg  = ShellCommandLineGetValue (ParamPackage, L"-v");
    if (CmdLineArg == NULL) {
        g_print_level = G_PRINT_LEVEL;
    } else {
        g_print_level = StrDecimalToUintn(CmdLineArg);
        if (g_print_level > 5) {
            g_print_level = G_PRINT_LEVEL;
        }
    }

    /* Options with Values */
    CmdLineArg  = ShellCommandLineGetValue (ParamPackage, L"-f");
    if (CmdLineArg == NULL) {
        g_acs_log_file_handle = NULL;
    } else {
        Status = ShellOpenFileByName(CmdLineArg, &g_acs_log_file_handle,
                     EFI_FILE_MODE_WRITE | EFI_FILE_MODE_READ | EFI_FILE_MODE_CREATE, 0x0);
        if (EFI_ERROR(Status)) {
            Print(L"Failed to open log file %s\n", CmdLineArg);
            g_acs_log_file_handle = NULL;
        }
    }

    /* Options with Values */
    if ((ShellCommandLineGetFlag (ParamPackage, L"-help")) || (ShellCommandLineGetFlag (ParamPackage, L"-h"))) {
        HelpMsg();
        return 0;
    }

    /* Initialize global counters */
    g_acs_tests_total = 0;
    g_acs_tests_pass  = 0;
    g_acs_tests_fail  = 0;

    Print(L"\n\n MPAM System Architecture Compliance Suite \n");
    Print(L"    Version %d.%d  \n", MPAM_ACS_MAJOR_VER, MPAM_ACS_MINOR_VER);

    Print(L"\n Starting tests for Print level %2d\n\n", g_print_level);

    Print(L" Creating Platform Information Tables \n");
    Status = CreatePeInfoTable();
    if (Status)
        return Status;

    Status = CreateGicInfoTable();
    if (Status)
        return Status;

    Status = CreateMpamInfoTable();
    if (Status)
        return Status;

    val_allocate_shared_mem();

    /*
     * Initialise exception vector, so any unexpected exception gets handled
     * by default MPAM exception handler
     */
    branch_label = &&print_test_status;
    val_pe_context_save(arm64_read_sp(), (uint64_t)branch_label);
    val_pe_initialize_default_exception_handler(val_pe_default_esr);
    FlushImage();

    Print(L"\n      ***  Starting CACHE PARTITION tests ***  \n");
    Status |= val_cache_execute_tests(val_pe_get_num());

    if (Status == ACS_STATUS_EXIT) {
        val_print(ACS_PRINT_TEST, "\n      *** Exiting suite - No MPAM nodes *** \n", 0);
        goto print_test_status;
    }

    Print(L"\n      ***  Starting CSU MONITOR tests ***  \n");
    Status |= val_csumon_execute_tests(val_pe_get_num());

    Print(L"\n      ***  Starting ERROR INTERRUPT tests ***  \n");
    Status |= val_interrupts_execute_tests(val_pe_get_num());

    Print(L"\n      ***  Starting MEMORY PARTITION tests ***  \n");
    Status |= val_memory_execute_tests(val_pe_get_num());

print_test_status:
    val_print(ACS_PRINT_TEST, "\n     ------------------------------------------------------- \n", 0);
    val_print(ACS_PRINT_TEST, "     Total Tests run  = %4d;", g_acs_tests_total);
    val_print(ACS_PRINT_TEST, "  Tests Passed  = %4d", g_acs_tests_pass);
    val_print(ACS_PRINT_TEST, "  Tests Failed = %4d\n", g_acs_tests_fail);
    val_print(ACS_PRINT_TEST, "     --------------------------------------------------------- \n", 0);

    FreeMpamAcsMem();

    if (g_acs_log_file_handle) {
        ShellCloseFile(&g_acs_log_file_handle);
    }

    Print(L"\n      *** MPAM tests complete. Reset the system. *** \n\n");

    val_pe_context_restore(arm64_write_sp(g_stack_pointer));

    return(0);
}

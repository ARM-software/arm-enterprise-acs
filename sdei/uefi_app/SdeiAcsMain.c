/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0

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

#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/ShellCEntryLib.h>
#include  <Library/ShellLib.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Library/CacheMaintenanceLib.h>
#include  <Protocol/LoadedImage.h>

#include <val_interface.h>
#include "SdeiAcs.h"

sdei_log_control g_log_control;

VOID
HelpMsg (
  VOID
  )
{
  Print (L"\nUsage: Sdei.efi [-v <n>] | [-l <n>] | [-f <filename>] | [-s] | [-skip <n>]\n"
         "Options:\n"
         "-v      Verbosity of the Prints\n"
         "        1 shows all prints, 5 shows Errors\n"
         "-f      Name of the log file to record the test results in\n"
         "-skip   Test(s) to be skipped\n"
         "        Refer to section 4 of SDEI_ACS_User_Guide\n"
         "        To skip a particular test use the testcase number\n"
         "-run    Test(s) to run\n"
  );
}

STATIC CONST SHELL_PARAM_ITEM ParamList[] = {
  {L"-v"    , TypeValue},    // -v    # Verbosity of the Prints. 1 shows all prints, 5 shows Errors
  {L"-f"    , TypeValue},    // -f    # Name of the log file to record the test results in.
  {L"-skip" , TypeValue},    // -skip # test(s) to skip execution
  {L"-run"  , TypeValue},    // -run  # test(s) to execute
  {L"-help" , TypeFlag},     // -help # help : info about commands
  {L"-h"    , TypeFlag},     // -h    # help : info about commands
  {NULL     , TypeMax}
  };

EFI_STATUS
createPeInfoTable (
)
{

  EFI_STATUS Status;
  UINT64   *PeInfoTable;

  Status = gBS->AllocatePool ( EfiBootServicesData,
                               PE_INFO_TABLE_SZ,
                               (VOID **) &PeInfoTable );

  if (EFI_ERROR(Status))
  {
    Print(L"Allocate Pool failed %x \n", Status);
    return Status;
  }

  Status = val_pe_create_info_table(PeInfoTable);

  return Status;

}

EFI_STATUS
createGicInfoTable (
)
{
  EFI_STATUS Status;
  UINT64     *GicInfoTable;

  Status = gBS->AllocatePool (EfiBootServicesData,
                               GIC_INFO_TABLE_SZ,
                               (VOID **) &GicInfoTable);

  if (EFI_ERROR(Status))
  {
    Print(L"Allocate Pool failed %x \n", Status);
    return Status;
  }

  Status = val_gic_create_info_table(GicInfoTable);

  return Status;

}

EFI_STATUS
createEventInfoTable (
)
{
  EFI_STATUS Status;
  UINT64     *EventInfoTable;

  Status = gBS->AllocatePool (EfiBootServicesData,
                               EVENT_INFO_TABLE_SZ,
                               (VOID **) &EventInfoTable);

  if (EFI_ERROR(Status))
  {
    Print(L"Allocate Pool failed %x \n", Status);
    return Status;
  }

  Status = val_sdei_create_event_info_table(EventInfoTable);

  return Status;

}

EFI_STATUS
createWatchdogInfoTable(
)
{
  UINT64   *WdInfoTable;
  EFI_STATUS Status;

  Status = gBS->AllocatePool (EfiBootServicesData,
                              WD_INFO_TABLE_SZ,
                              (VOID **) &WdInfoTable);

  if (EFI_ERROR(Status))
  {
    Print(L"Allocate Pool failed %x \n", Status);
    return Status;
  }
  Status = val_wd_create_info_table(WdInfoTable);

  return Status;

}

EFI_STATUS
createTimerInfoTable(
)
{
  UINT64   *TimerInfoTable;
  EFI_STATUS Status;

  Status = gBS->AllocatePool (EfiBootServicesData,
                              TIMER_INFO_TABLE_SZ,
                              (VOID **) &TimerInfoTable);

  if (EFI_ERROR(Status))
  {
    Print(L"Allocate Pool failed %x \n", Status);
    return Status;
  }
  val_timer_create_info_table(TimerInfoTable);

  return Status;
}

/***
  SDEI Compliance Suite Entry Point.

  Call the Entry points of individual modules.

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
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
  UINT32             i, test_id;
  sdei_test_control  control;

  val_test_init(&control);

  //
  // Process Command Line arguments
  //
  Status = ShellInitialize();
  Status = ShellCommandLineParse (ParamList, &ParamPackage, &ProbParam, TRUE);
  if (Status) {
    Print(L"Shell command line parse error %x\n", Status);
    Print(L"Unrecognized option %s passed\n", ProbParam);
    HelpMsg();
    return SHELL_INVALID_PARAMETER;
  }

  for (i = 1; i<Argc; i++) {
    if (StrCmp(Argv[i], L"-skip") == 0) {
      CmdLineArg = Argv[++i];
      i=0;
      break;
    } else if (StrCmp(Argv[i], L"-run") == 0) {
      CmdLineArg = Argv[++i];
      i=100;
      break;
    }
  }
  if (i == 0){
    test_id = StrDecimalToUintn((CONST CHAR16 *)(CmdLineArg+0));
    val_test_disable(&control, test_id);

    for (i=0 ; i < StrLen(CmdLineArg) ; i++){
      if (*(CmdLineArg+i) == L','){
        test_id = StrDecimalToUintn((CONST CHAR16 *)(CmdLineArg+i+1));
        val_test_disable(&control, test_id);
      }
    }
  }
  if (i == 100){
    test_id = StrDecimalToUintn((CONST CHAR16 *)(CmdLineArg+0));
    val_test_run_specific(&control, test_id, 1);

    for (i=0 ; i < StrLen(CmdLineArg) ; i++){
      if (*(CmdLineArg+i) == L','){
        test_id = StrDecimalToUintn((CONST CHAR16 *)(CmdLineArg+i+1));
        val_test_run_specific(&control, test_id, 0);
      }
    }
  }


    // Options with Values
  CmdLineArg = ShellCommandLineGetValue (ParamPackage, L"-v");
  g_log_control.print_level = ACS_LOG_TEST;
  if (CmdLineArg != NULL) {
    g_log_control.print_level = StrDecimalToUintn(CmdLineArg);
    if (g_log_control.print_level > 5) {
      g_log_control.print_level = ACS_LOG_TEST;
    }
  }


    // Options with Values
  CmdLineArg = ShellCommandLineGetValue (ParamPackage, L"-f");
  g_log_control.log_file_handle = NULL;
  if (CmdLineArg != NULL) {
    Status = ShellOpenFileByName(CmdLineArg, &g_log_control.log_file_handle,
             EFI_FILE_MODE_WRITE | EFI_FILE_MODE_READ | EFI_FILE_MODE_CREATE, 0x0);
    if (EFI_ERROR(Status)) {
         Print(L"Failed to open log file %s\n", CmdLineArg);
    }
  }

    // Options with Values
  if ((ShellCommandLineGetFlag (ParamPackage, L"-help")) ||
            (ShellCommandLineGetFlag (ParamPackage, L"-h"))){
     HelpMsg();
     return 0;
  }
  Status = createPeInfoTable();
  if (Status)
    return Status;

  Status = createGicInfoTable();
  if (Status)
    return Status;

  Status = createEventInfoTable();
  if (Status)
    return Status;

  Status = createWatchdogInfoTable();
  if (Status)
    return Status;

  Status = createTimerInfoTable();
  if (Status)
    return Status;

  Status = val_sdei_initialization();
  if (Status) {
      val_print(ACS_LOG_ERR, "SDEI initialization failed\n");
      return Status;
  }

  val_shared_mem_alloc();

  //
  // Initialize global counters
  //
  control.tests_passed = 0;
  control.tests_failed = 0;
  control.tests_skipped = 0;
  control.tests_aborted = 0;

  Print(L"\n\n SDEI Architecture Compliance Suite \n");
  Print(L"    Version %d.%d  \n", SDEI_ACS_MAJOR_VER, SDEI_ACS_MINOR_VER);
  val_test_execute(&control);
  Print(L"Tests passed  : %d\n", control.tests_passed);
  Print(L"Tests failed  : %d\n", control.tests_failed);
  Print(L"Tests skipped : %d\n", control.tests_skipped);
  Print(L"Tests aborted : %d\n", control.tests_aborted);

  val_shared_mem_free();

  if (g_log_control.log_file_handle) {
    ShellCloseFile(g_log_control.log_file_handle);
  }

  Print(L"\n      *** SDEI tests complete. Reset the system. *** \n\n");

  return(0);
}

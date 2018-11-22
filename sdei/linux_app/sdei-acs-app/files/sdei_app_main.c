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

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "include/sdei_app_main.h"
#include "include/sdei_test_intf.h"

sdei_control_t control;

void print_help(){
    printf ("\nUsage: sdei [-v <n>] | [--skip <n>] | [--run <r>]\n"
           "Options:\n"
           "-v      Verbosity of the Prints\n"
           "        5 shows all prints, 1 shows Errors, 6 for Linux kernel printks\n"
           "--skip  Test(s) to be skipped\n"
           "--run   Test(s) to be run\n"
           "        Refer to section 4 of SDEI_ACS_User_Guide\n"
           "        To skip a module, use Model_ID as mentioned in user guide\n"
           "        To skip a particular test within a module, use the exact testcase number\n"
  );
}

int parse_cmdline(int argc, char **argv)
{
    int c = 0;
    int test_id = 0, count = 0;
    char *endptr, *pt;
    struct option long_opt[] =
    {
        {"skip", required_argument, NULL, 'n'},
        {"run", required_argument, NULL, 'r'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    /* Process Command Line arguments */
    while ((c = getopt_long(argc, argv, "hv:n:r:", long_opt, NULL)) != -1)
    {
        switch (c)
        {
        case 'v':
          control.log_control.print_level = strtol(optarg, &endptr, 10);
          if (control.log_control.print_level > 6) {
              printf("Verbosity level is invalid\n");
              print_help();
              return 1;
          }
          break;
        case 'h':
          print_help();
          return 1;
          break;
        case 'n':/*SKIP tests */
          pt = strtok(optarg, ",");
          while (pt != NULL) {
              test_id = atoi(pt);
              testlib_disable_test(&control, (test_id-1));
              pt = strtok(NULL, ",");
          }
          break;
        /* To run specific test */
        case 'r':
          pt = strtok(optarg, ",");
          while (pt != NULL) {
              test_id = atoi(pt);
              count++;
              testlib_run_specific(&control, (test_id-1), count);
              pt = strtok(NULL, ",");
          }
          break;
        default:
          printf("unknown commandline option\n");
          print_help();
          return 1;
        }
    }
    return 0;
}

int
main (int argc, char **argv)
{
    int   status;

    status = init_test_env(&control);
    if (status) {
        printf ("Cannot initialize test environment. Exiting.... \n");
        return 0;
    }

    status = parse_cmdline(argc, argv);
    if (status)
        return 1;

    printf ("\n ************ SDEI Architecture Compliance Suite ********* \n");
    printf ("                        Version %d.%d  \n",
                                        SDEI_APP_VERSION_MAJOR, SDEI_APP_VERSION_MINOR);

    printf ("\n Starting tests for Print level is %2d)\n\n", control.log_control.print_level);
    testlib_execute_tests(&control);

    printf("\n                    *** SDEI tests complete *** \n\n");
    testlib_cleanup();

    return 0;
}

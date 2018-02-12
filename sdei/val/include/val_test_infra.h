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

#define SDEI_TEST_PASS      0x0
#define SDEI_TEST_PENDING   0x2
#define SDEI_TEST_TIMEOUT   0x4
#define SDEI_TEST_FAIL      0x1
#define SDEI_TEST_ERROR     0x3
#define SDEI_TEST_ABORT     0x5
#define SDEI_TEST_SKIP      0x7

#define SDEI_NUM_TESTS      49

#define PE_INFO_TABLE_SZ    8192
#define GIC_INFO_TABLE_SZ   8192
#define EVENT_INFO_TABLE_SZ 64
#define WD_INFO_TABLE_SZ    512
#define TIMER_INFO_TABLE_SZ 1024

#define TEST_NONE_ID 0
#define SDEI_PUBLISH_TEST(test, id, desc, deps, entry, all_pe) \
                        sdei_test_desc test = {id, desc, deps, SDEI_TEST_PENDING, entry, all_pe}
#define SDEI_DECLARE_TEST(test) extern sdei_test_desc test
#define SDEI_SET_TEST_DEPS(deps, ...) static sdei_test_deps deps[] =  {__VA_ARGS__, TEST_NONE_ID}

#define TEST_TIMEOUT 0x10000

typedef void (*sdei_test_fn)(void);
typedef uint32_t sdei_test_deps;

typedef struct sdei_test_desc {
    uint32_t id;
    char description[124];
    sdei_test_deps *deps;
    uint32_t status;
    sdei_test_fn test_fn;
    int32_t all_pe;
} sdei_test_desc;

typedef uint64_t test_flags;

typedef struct sdei_test_control {
    test_flags flags[2]; //which tests to run?
    uint32_t tests_skipped;
    uint32_t tests_passed;
    uint32_t tests_failed;
    uint32_t tests_aborted;
} sdei_test_control;

void val_test_init(sdei_test_control *);
void val_test_execute(sdei_test_control *);
void val_test_disable(sdei_test_control *, int);
void val_test_run_specific(sdei_test_control *, int, int);

void val_test_pe_set_status(uint32_t index, uint32_t status);
void val_test_set_status(uint32_t num_pe, uint32_t status);
uint32_t val_test_pe_get_status(uint32_t index);
uint32_t val_test_get_status(uint32_t num_pe, uint64_t timeout);

#define TEST_001_ID 1
SDEI_DECLARE_TEST(test_001);

#define TEST_002_ID 2
SDEI_DECLARE_TEST(test_002);

#define TEST_003_ID 3
SDEI_DECLARE_TEST(test_003);

#define TEST_004_ID 4
SDEI_DECLARE_TEST(test_004);

#define TEST_005_ID 5
SDEI_DECLARE_TEST(test_005);

#define TEST_006_ID 6
SDEI_DECLARE_TEST(test_006);

#define TEST_007_ID 7
SDEI_DECLARE_TEST(test_007);

#define TEST_008_ID 8
SDEI_DECLARE_TEST(test_008);

#define TEST_009_ID 9
SDEI_DECLARE_TEST(test_009);

#define TEST_010_ID 10
SDEI_DECLARE_TEST(test_010);

#define TEST_011_ID 11
SDEI_DECLARE_TEST(test_011);

#define TEST_012_ID 12
SDEI_DECLARE_TEST(test_012);

#define TEST_013_ID 13
SDEI_DECLARE_TEST(test_013);

#define TEST_014_ID 14
SDEI_DECLARE_TEST(test_014);

#define TEST_015_ID 15
SDEI_DECLARE_TEST(test_015);

#define TEST_016_ID 16
SDEI_DECLARE_TEST(test_016);

#define TEST_017_ID 17
SDEI_DECLARE_TEST(test_017);

#define TEST_018_ID 18
SDEI_DECLARE_TEST(test_018);

#define TEST_019_ID 19
SDEI_DECLARE_TEST(test_019);

#define TEST_020_ID 20
SDEI_DECLARE_TEST(test_020);

#define TEST_021_ID 21
SDEI_DECLARE_TEST(test_021);

#define TEST_022_ID 22
SDEI_DECLARE_TEST(test_022);

#define TEST_023_ID 23
SDEI_DECLARE_TEST(test_023);

#define TEST_024_ID 24
SDEI_DECLARE_TEST(test_024);

#define TEST_025_ID 25
SDEI_DECLARE_TEST(test_025);

#define TEST_026_ID 26
SDEI_DECLARE_TEST(test_026);

#define TEST_027_ID 27
SDEI_DECLARE_TEST(test_027);

#define TEST_028_ID 28
SDEI_DECLARE_TEST(test_028);

#define TEST_029_ID 29
SDEI_DECLARE_TEST(test_029);

#define TEST_030_ID 30
SDEI_DECLARE_TEST(test_030);

#define TEST_031_ID 31
SDEI_DECLARE_TEST(test_031);

#define TEST_032_ID 32
SDEI_DECLARE_TEST(test_032);

#define TEST_033_ID 33
SDEI_DECLARE_TEST(test_033);

#define TEST_034_ID 34
SDEI_DECLARE_TEST(test_034);

#define TEST_035_ID 35
SDEI_DECLARE_TEST(test_035);

#define TEST_036_ID 36
SDEI_DECLARE_TEST(test_036);

#define TEST_037_ID 37
SDEI_DECLARE_TEST(test_037);

#define TEST_038_ID 38
SDEI_DECLARE_TEST(test_038);

#define TEST_039_ID 39
SDEI_DECLARE_TEST(test_039);

#define TEST_040_ID 40
SDEI_DECLARE_TEST(test_040);

#define TEST_041_ID 41
SDEI_DECLARE_TEST(test_041);

#define TEST_042_ID 42
SDEI_DECLARE_TEST(test_042);

#define TEST_043_ID 43
SDEI_DECLARE_TEST(test_043);

#define TEST_044_ID 44
SDEI_DECLARE_TEST(test_044);

#define TEST_045_ID 45
SDEI_DECLARE_TEST(test_045);

#define TEST_046_ID 46
SDEI_DECLARE_TEST(test_046);

#define TEST_047_ID 47
SDEI_DECLARE_TEST(test_047);

#define TEST_048_ID 48
SDEI_DECLARE_TEST(test_048);

#define TEST_049_ID 49
SDEI_DECLARE_TEST(test_049);

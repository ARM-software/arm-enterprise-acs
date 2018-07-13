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

#ifndef __MPAM_ACS_LEVEL_H__
#define __MPAM_ACS_LEVEL_H__


#define MPAM_ACS_MAJOR_VER  0
#define MPAM_ACS_MINOR_VER  5

#define G_MPAM_LEVEL        3
#define G_PRINT_LEVEL       ACS_PRINT_TEST

/*Supports at maximum 400 PEs*/
#define PE_INFO_TBL_SZ      8192
/*
 * Supports at maximum 256 redistributors,
 * 256 ITS blocks & 4 distributors
 */
#define GIC_INFO_TBL_SZ     8192

#define MPAM_INFO_TBL_SZ    8192

#endif

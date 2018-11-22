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

#ifndef __MPAM_ACS_MEASUREMENTS_H__
#define __MPAM_ACS_MEASUREMENTS_H__

typedef enum {
    PMCR_EL0 = 1,
    PMCCNTR_EL0,
    PMCCFILTR_EL0,
    PMCNTENSET_EL0
} MPAM_ACS_PMU_REGS;

void val_measurement_start();
void val_measurement_stop();

#endif


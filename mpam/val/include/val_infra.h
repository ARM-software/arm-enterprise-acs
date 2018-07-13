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

#ifndef __MPAM_ACS_VAL_H__
#define __MPAM_ACS_VAL_H__

#include "val_interface.h"
#include "pal_interface.h"
#include "val_cfg.h"
#include "val_common.h"


typedef struct {
    uint64_t    data0;
    uint64_t    data1;
    uint32_t    status;
} VAL_SHARED_MEM_t;

void val_report_status(uint32_t id, uint32_t status);
void val_set_status(uint32_t index, uint32_t status);
uint32_t val_get_status(uint32_t id);

#endif


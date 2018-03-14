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

#include "val_interface.h"

/**
 *  @brief   This function returns version of the PSCI
 *  @param   none
 *  @return  status
 */
uint32_t val_psci_version(uint32_t *version)
{
    *version = pal_invoke_psci_fn(PSCI_FN_PSCI_VERSION, 0, 0, 0);
    return 0;
}

/**
 *  @brief   This function returns ON/OFF/ON_Pending states of given affinity value
 *
 *  @param target_aff  Targeted PE affinity value.
 *  @param lowest_aff_level  Denotes the lowest affinity level field that is valid in
 *                           the target affinity value.
 *  
 *  @return  status
 */
uint32_t val_psci_affinity_info(uint64_t target_aff, uint64_t lowest_aff_level)
{
    int32_t err = -1;

    err = pal_invoke_psci_fn(PSCI_FN_AFFINITY_INFO, target_aff, lowest_aff_level, 0);
    if (err < 0)
        return 1;
    else
        return 0;
}

/**
 *  @brief   This function allows discovering whether a specific PSCI function is
 *           implemented
 * 
 *  @param fun_id  Function ID for a PSCI function
 *   
 *  @return  status
 */
uint32_t val_psci_features(uint32_t fun_id)
{
    int32_t err = -1;

    err = pal_invoke_psci_fn(PSCI_FN_PSCI_FEATURES, fun_id, 0, 0);
    if (err < 0)
        return 1;
    else
        return 0;
}

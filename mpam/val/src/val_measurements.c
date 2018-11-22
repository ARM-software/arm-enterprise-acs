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

#include "include/val_measurements.h"
#include "include/pal_interface.h"

/**
 * @brief   Configures necessary PMU registers & starts the Cycle Counter
 *
 * @param   None
 * @return  None
 */
void val_measurement_start()
{
    pal_pmu_cycle_counter_start();

}

/**
 * @brief   Stop the Cycle Counter and restores the PMU reg values
 *
 * @param   None
 * @return  None
 */
void val_measurement_stop()
{
    pal_pmu_cycle_counter_stop();
}

/**
 * @brief   Read the Cycle Counter
 *
 * @param   None
 * @return  None
 */
uint64_t val_measurement_read()
{
    return pal_pmu_reg_read(PMCCNTR_EL0);
}

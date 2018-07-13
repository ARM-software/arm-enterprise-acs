#/** @file
# Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
# SPDX-License-Identifier : Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#**/

.text
.align 2

GCC_ASM_EXPORT(sysreg_read_mpam1)
GCC_ASM_EXPORT(sysreg_write_mpam1)
GCC_ASM_EXPORT(sysreg_read_mpam2)
GCC_ASM_EXPORT(sysreg_write_mpam2)
GCC_ASM_EXPORT(sysreg_read_mpamidr)


ASM_PFX(sysreg_read_mpam1):
  //mrs   x0, mpam1_el1
  ret

ASM_PFX(sysreg_write_mpam1):
  //msr   mpam1_el1, x0

ASM_PFX(sysreg_read_mpam2):
  //mrs   x0, mpam2_el2
  ret

ASM_PFX(sysreg_write_mpam2):
  //msr   mpam2_el2, x0

ASM_PFX(sysreg_read_mpamidr):
  //mrs   x0, mpamidr_el1
  ret

ASM_FUNCTION_REMOVE_IF_UNREFERENCED

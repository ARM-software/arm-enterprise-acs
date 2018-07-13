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
.align 3

GCC_ASM_EXPORT (Arm64ReadPmcr)
GCC_ASM_EXPORT (Arm64ReadPmccntr)
GCC_ASM_EXPORT (Arm64ReadPmccfiltr)
GCC_ASM_EXPORT (Arm64ReadPmcntenset)
GCC_ASM_EXPORT (Arm64WritePmcr)
GCC_ASM_EXPORT (Arm64WritePmccntr)
GCC_ASM_EXPORT (Arm64WritePmccfiltr)
GCC_ASM_EXPORT (Arm64WritePmcntenset)


ASM_PFX(Arm64ReadPmcr):
  mrs   x0, pmcr_el0
  ret

ASM_PFX(Arm64ReadPmccntr):
  mrs   x0, pmccntr_el0
  ret

ASM_PFX(Arm64ReadPmccfiltr):
  mrs   x0, pmccfiltr_el0
  ret

ASM_PFX(Arm64ReadPmcntenset):
  mrs   x0, pmcntenset_el0
  ret

ASM_PFX(Arm64WritePmcr):
  msr   pmcr_el0, x0
  isb
  ret

ASM_PFX(Arm64WritePmccntr):
  msr   pmccntr_el0, x0
  isb
  ret

ASM_PFX(Arm64WritePmccfiltr):
  msr   pmccfiltr_el0, x0
  isb
  ret

ASM_PFX(Arm64WritePmcntenset):
  msr   pmcntenset_el0, x0
  isb
  ret


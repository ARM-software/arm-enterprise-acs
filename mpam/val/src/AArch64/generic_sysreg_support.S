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

//
// Private worker functions for ASM_PFX()
//
#define _CONCATENATE(a, b)  __CONCATENATE(a, b)
#define __CONCATENATE(a, b) a ## b

#define __USER_LABEL_PREFIX__
//
// The __USER_LABEL_PREFIX__ macro predefined by GNUC represents the prefix
// on symbols in assembly language.
//
#define ASM_PFX(name) _CONCATENATE (__USER_LABEL_PREFIX__, name)

#define GCC_ASM_EXPORT(func__)  \
       .global  _CONCATENATE (__USER_LABEL_PREFIX__, func__)    ;\
       .type ASM_PFX(func__), %function


.text
.align 3


GCC_ASM_EXPORT (arm64_read_mpidr)
GCC_ASM_EXPORT (arm64_read_idpfr0)
GCC_ASM_EXPORT (arm64_read_idpfr1)
GCC_ASM_EXPORT (arm64_read_mmfr0)
GCC_ASM_EXPORT (arm64_read_mmfr1)
GCC_ASM_EXPORT (arm64_read_mmfr2)
GCC_ASM_EXPORT (arm64_read_ctr)
GCC_ASM_EXPORT (arm64_read_isar0)
GCC_ASM_EXPORT (arm64_read_isar1)
GCC_ASM_EXPORT (arm64_read_sctlr3)
GCC_ASM_EXPORT (arm64_read_sctlr2)
GCC_ASM_EXPORT (arm64_read_pmcr)
GCC_ASM_EXPORT (arm64_read_iddfr0)
GCC_ASM_EXPORT (arm64_read_iddfr1)
GCC_ASM_EXPORT (arm_read_hcr)
GCC_ASM_EXPORT (arm64_read_cur_el)
GCC_ASM_EXPORT (AA64ReadIdMdrar)
GCC_ASM_EXPORT (arm64_read_mdcr2)
GCC_ASM_EXPORT (arm64_write_mdcr2)
GCC_ASM_EXPORT (arm64_read_vbar2)
GCC_ASM_EXPORT (arm64_write_vbar2)
GCC_ASM_EXPORT (arm64_write_pmcr)
GCC_ASM_EXPORT (arm64_write_pmovsset)
GCC_ASM_EXPORT (arm64_write_pmintenset)
GCC_ASM_EXPORT (arm64_write_pmovsclr)
GCC_ASM_EXPORT (arm64_write_pmintenclr)
GCC_ASM_EXPORT (arm64_read_ccsidr)
GCC_ASM_EXPORT (arm64_read_csselr)
GCC_ASM_EXPORT (arm64_write_csselr)
GCC_ASM_EXPORT (arm64_read_clidr)
GCC_ASM_EXPORT (arm_read_dfr0)
GCC_ASM_EXPORT (arm_read_isar0)
GCC_ASM_EXPORT (arm_read_isar1)
GCC_ASM_EXPORT (arm_read_isar2)
GCC_ASM_EXPORT (arm_read_isar3)
GCC_ASM_EXPORT (arm_read_isar4)
GCC_ASM_EXPORT (arm_read_isar5)
GCC_ASM_EXPORT (arm_read_midr)
GCC_ASM_EXPORT (arm_read_mvfr0)
GCC_ASM_EXPORT (arm_read_mvfr1)
GCC_ASM_EXPORT (arm_read_mvfr2)
GCC_ASM_EXPORT (arm64_read_pmceid0)
GCC_ASM_EXPORT (arm64_read_pmceid1)
GCC_ASM_EXPORT (arm64_read_vmpidr)
GCC_ASM_EXPORT (arm64_read_vpidr)
GCC_ASM_EXPORT (arm64_read_pmbidr)
GCC_ASM_EXPORT (arm64_read_pmsidr)
GCC_ASM_EXPORT (arm64_read_lorid)
GCC_ASM_EXPORT (arm64_read_erridr)
GCC_ASM_EXPORT (arm64_read_err0fr)
GCC_ASM_EXPORT (arm64_read_err1fr)
GCC_ASM_EXPORT (arm64_read_err2fr)
GCC_ASM_EXPORT (arm64_read_err3fr)
GCC_ASM_EXPORT (arm64_write_pmsirr)
GCC_ASM_EXPORT (arm64_write_pmscr2)
GCC_ASM_EXPORT (arm64_write_pmsfcr)
GCC_ASM_EXPORT (arm64_write_pmbptr)
GCC_ASM_EXPORT (arm64_write_pmblimitr)
GCC_ASM_EXPORT (arm64_read_esr2)
GCC_ASM_EXPORT (arm64_read_sp)
GCC_ASM_EXPORT (arm64_write_sp)
GCC_ASM_EXPORT (arm64_read_far2)
GCC_ASM_EXPORT (arm64_read_pmccntr)
GCC_ASM_EXPORT (arm64_read_pmccfiltr)
GCC_ASM_EXPORT (arm64_read_pmcntenset)
GCC_ASM_EXPORT (arm64_write_pmccntr)
GCC_ASM_EXPORT (arm64_write_pmccfiltr)
GCC_ASM_EXPORT (arm64_write_pmcntenset)
GCC_ASM_EXPORT (arm64_issue_dmb)
GCC_ASM_EXPORT (arm64_issue_dsb)
GCC_ASM_EXPORT (arm64_issue_isb)

ASM_PFX(arm64_read_mpidr):
  mrs   x0, mpidr_el1           // read EL1 MPIDR
  ret

ASM_PFX(arm64_read_idpfr0):
  mrs   x0, id_aa64pfr0_el1   // Read ID_AA64PFR0 Register
  ret

ASM_PFX(arm64_read_idpfr1):
  mrs   x0, id_aa64pfr1_el1   // Read ID_AA64PFR0 Register
  ret

ASM_PFX(arm64_read_mmfr0):
  mrs   x0, id_aa64mmfr0_el1
  ret

ASM_PFX(arm64_read_mmfr1):
  mrs   x0, id_aa64mmfr1_el1
  ret

ASM_PFX(arm64_read_mmfr2):
  mrs   x0, id_aa64mmfr2_el1
  ret

ASM_PFX(arm64_read_ctr):
  mrs   x0, ctr_el0
  ret

ASM_PFX(arm64_read_isar0):
  mrs   x0, id_aa64isar0_el1
  ret

ASM_PFX(arm64_read_isar1):
  mrs   x0, id_aa64isar1_el1
  ret

ASM_PFX(arm64_read_sctlr3):
  mrs   x0, sctlr_el3
  ret

ASM_PFX(arm64_read_sctlr2):
  mrs   x0, sctlr_el2
  ret

ASM_PFX(arm64_read_pmcr):
  mrs   x0, pmcr_el0
  ret

ASM_PFX(arm64_read_iddfr0):
  mrs   x0, id_aa64dfr0_el1
  ret

ASM_PFX(arm64_read_iddfr1):
  mrs   x0, id_aa64dfr1_el1
  ret

ASM_PFX(arm_read_hcr):
  mrs   x0, hcr_el2
  ret

ASM_PFX(arm64_read_cur_el):
  mrs   x0, CurrentEL
  ret

ASM_PFX(arm64_read_mdcr2):
  mrs   x0, mdcr_el2
  ret

ASM_PFX(arm64_write_mdcr2):
  msr   mdcr_el2, x0
  isb
  ret

ASM_PFX(arm64_read_vbar2):
  mrs   x0, vbar_el2
  ret

ASM_PFX(arm64_write_vbar2):
  msr   vbar_el2, x0
  isb
  ret

ASM_PFX(arm64_write_pmcr):
  msr   pmcr_el0, x0
  isb
  ret

ASM_PFX(arm64_write_pmovsset):
  msr   pmovsset_el0, x0
  isb
  ret

ASM_PFX(arm64_write_pmovsclr):
  msr   pmovsclr_el0, x0
  isb
  ret

ASM_PFX(arm64_write_pmintenset):
  msr   pmintenset_el1, x0
  isb
  ret

ASM_PFX(arm64_write_pmintenclr):
  msr   pmintenclr_el1, x0
  isb
  ret

ASM_PFX(arm64_read_ccsidr):
  mrs   x0, ccsidr_el1
  ret

ASM_PFX(arm64_read_csselr):
  mrs   x0, csselr_el1
  ret

ASM_PFX(arm64_write_csselr):
  msr   csselr_el1, x0
  isb
  ret

ASM_PFX(arm64_read_clidr):
  mrs   x0, clidr_el1
  ret

ASM_PFX(arm_read_dfr0):
  mrs   x0, id_dfr0_el1
  ret

ASM_PFX(arm_read_isar0):
  mrs   x0, id_isar0_el1
  ret

ASM_PFX(arm_read_isar1):
  mrs   x0, id_isar1_el1
  ret

ASM_PFX(arm_read_isar2):
  mrs   x0, id_isar2_el1
  ret

ASM_PFX(arm_read_isar3):
  mrs   x0, id_isar3_el1
  ret

ASM_PFX(arm_read_isar4):
  mrs   x0, id_isar4_el1
  ret

ASM_PFX(arm_read_isar5):
  mrs   x0, id_isar5_el1
  ret

ASM_PFX(arm_read_midr):
  mrs   x0, midr_el1
  ret

ASM_PFX(arm_read_mvfr0):
  mrs   x0, mvfr0_el1
  ret

ASM_PFX(arm_read_mvfr1):
  mrs   x0, mvfr1_el1
  ret

ASM_PFX(arm_read_mvfr2):
  mrs   x0, mvfr2_el1
  ret

ASM_PFX(arm64_read_pmceid0):
  mrs   x0, pmceid0_el0
  ret

ASM_PFX(arm64_read_pmceid1):
  mrs   x0, pmceid1_el0
  ret

ASM_PFX(arm64_read_vmpidr):
  mrs   x0, vmpidr_el2
  ret

ASM_PFX(arm64_read_vpidr):
  mrs   x0, vpidr_el2
  ret

ASM_PFX(arm64_read_pmbidr):
  //mrs   x0, pmbidr_el1
  ret

ASM_PFX(arm64_read_pmsidr):
  //mrs   x0, pmsidr_el1
  ret

ASM_PFX(arm64_read_lorid):
  //mrs   x0, lorid_el1
  ret

ASM_PFX(arm64_read_erridr):
  //mrs   x0, erridr_el1
  ret

ASM_PFX(arm64_read_err0fr):
 // mrs   x0, err0fr_el1
  ret

ASM_PFX(arm64_read_err1fr):
  //mrs   x0, err1fr_el1
  ret

ASM_PFX(arm64_read_err2fr):
  //mrs   x0, err2fr_el1
  ret

ASM_PFX(arm64_read_err3fr):
  //mrs   x0, err3fr_el1
  ret

ASM_PFX(arm64_write_pmsirr):
  //mrs   pmsirr_el1,x0
  isb
  ret

ASM_PFX(arm64_write_pmscr2):
  //mrs   pmscr_el2,x0
  isb
  ret

ASM_PFX(arm64_write_pmsfcr):
  //mrs   pmsfcr_el1,x0
  isb
  ret

ASM_PFX(arm64_write_pmbptr):
  //mrs   pmbptr_el1,x0
  isb
  ret

ASM_PFX(arm64_write_pmblimitr):
  //mrs   pmblimitr_el1,x0
  isb
  ret

ASM_PFX(arm64_read_esr2):
  mrs   x0, esr_el2
  ret

ASM_PFX(arm64_read_sp):
  mov   x0, sp
  ret

ASM_PFX(arm64_write_sp):
  mov   sp, x0
  ret

ASM_PFX(arm64_read_far2):
  mrs   x0, far_el2
  ret

ASM_PFX(arm64_read_pmccntr):
  mrs   x0, pmccntr_el0
  ret

ASM_PFX(arm64_read_pmccfiltr):
  mrs   x0, pmccfiltr_el0
  ret

ASM_PFX(arm64_read_pmcntenset):
  mrs   x0, pmcntenset_el0
  ret

ASM_PFX(arm64_write_pmccntr):
  msr   pmccntr_el0, x0
  isb
  ret

ASM_PFX(arm64_write_pmccfiltr):
  msr   pmccfiltr_el0, x0
  isb
  ret

ASM_PFX(arm64_write_pmcntenset):
  msr   pmcntenset_el0, x0
  isb
  ret

ASM_PFX(arm64_issue_dmb):
  dmb   sy
  ret

ASM_PFX(arm64_issue_dsb):
  dsb   sy
  ret

ASM_PFX(arm64_issue_isb):
  isb
  ret

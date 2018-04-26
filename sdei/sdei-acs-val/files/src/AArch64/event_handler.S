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

#define SDEI_EVENT_COMPLETE         0xc4000025
#define SDEI_EVENT_COMPLETE_RESUME  0xc4000026

.text
.align 3

.extern g_interrupted_pc
.extern g_interrupted_pstate

GCC_ASM_EXPORT (asm_event_handler)
GCC_ASM_EXPORT (asm_handler_resume)
GCC_ASM_EXPORT (asm_handler_resume_context)

ASM_PFX(asm_event_handler):
    stp x29, x30, [sp, #-128]!
    mov x29, sp

    blr  x1

    ldp x29, x30, [sp], #128
    ldr x0, =SDEI_EVENT_COMPLETE
    mov x1, #0
    smc #0

ASM_PFX(asm_handler_resume):
    stp x29, x30, [sp, #-32]!
    mov x29, sp

    ldp x29, x30, [sp], #32
    ldr x0, =SDEI_EVENT_COMPLETE_RESUME
    smc #0

ASM_PFX(asm_handler_resume_context):
    stp x29, x30, [sp, #-32]!
    mov x29, sp

    ldr x0, =g_interrupted_pc
    str x2, [x0]
    ldr x0, =g_interrupted_pstate
    str x3, [x0]

    ldp x29, x30, [sp], #32
    ldr x0, =SDEI_EVENT_COMPLETE_RESUME
    smc #0

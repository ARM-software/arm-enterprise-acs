## @file
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
##

export CROSS_COMPILE ?= aarch64-linux-gnu-
export ARCH ?= arm64
export TEST_POOL ?= ./tests/

obj-m += sdei_acs_test.o
sdei_acs_test-objs += $(TEST_POOL)/test_001.o \
    $(TEST_POOL)/test_002.o \
    $(TEST_POOL)/test_003.o \
    $(TEST_POOL)/test_004.o \
    $(TEST_POOL)/test_005.o \
    $(TEST_POOL)/test_006.o \
    $(TEST_POOL)/test_007.o \
    $(TEST_POOL)/test_008.o \
    $(TEST_POOL)/test_009.o \
    $(TEST_POOL)/test_010.o \
    $(TEST_POOL)/test_011.o \
    $(TEST_POOL)/test_012.o \
    $(TEST_POOL)/test_013.o \
    $(TEST_POOL)/test_014.o \
    $(TEST_POOL)/test_015.o \
    $(TEST_POOL)/test_016.o \
    $(TEST_POOL)/test_017.o \
    $(TEST_POOL)/test_018.o \
    $(TEST_POOL)/test_019.o \
    $(TEST_POOL)/test_020.o \
    $(TEST_POOL)/test_021.o \
    $(TEST_POOL)/test_022.o \
    $(TEST_POOL)/test_023.o \
    $(TEST_POOL)/test_024.o \
    $(TEST_POOL)/test_025.o \
    $(TEST_POOL)/test_026.o \
    $(TEST_POOL)/test_027.o \
    $(TEST_POOL)/test_028.o \
    $(TEST_POOL)/test_029.o \
    $(TEST_POOL)/test_030.o \
    $(TEST_POOL)/test_031.o \
    $(TEST_POOL)/test_032.o \
    $(TEST_POOL)/test_033.o \
    $(TEST_POOL)/test_034.o \
    $(TEST_POOL)/test_035.o \
    $(TEST_POOL)/test_036.o \
    $(TEST_POOL)/test_037.o \
    $(TEST_POOL)/test_038.o \
    $(TEST_POOL)/test_039.o \
    $(TEST_POOL)/test_040.o \
    $(TEST_POOL)/test_041.o \
    $(TEST_POOL)/test_042.o \
    $(TEST_POOL)/test_043.o \
    $(TEST_POOL)/test_044.o \
    $(TEST_POOL)/test_045.o \
    $(TEST_POOL)/test_046.o \
    $(TEST_POOL)/test_047.o \
    $(TEST_POOL)/test_048.o \
    $(TEST_POOL)/test_049.o

ccflags-y=-I$(PWD)/val/include/  -DTARGET_LINUX -Wall -Werror

all:
ifeq ($(KERNEL_SRC),)
	echo "	KERNEL_SRC variable should be set to kernel path "
	exit 1
else
	echo "Kernel source is set to $(KERNEL_SRC)"
endif

	make -C $(KERNEL_SRC) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules_install

clean:
	make -C $(KERNEL_SRC) M=$(PWD) clean

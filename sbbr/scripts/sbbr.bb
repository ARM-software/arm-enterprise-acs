# Copyright (c) 2017-2018, Arm Limited or its affiliates. All rights reserved.
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

SUMMARY = "SBBR is a test suite that runs Server Base Boot Requirements UEFI tests."

DESCRIPTION = "This Server Base Boot Requirements (SBBR) test suite is intended \
for SBSA[2]-compliant 64-bit ARMv8 servers. It defines the base firmware requirements \
for out-of-box support of any ARM SBSA-compatible Operating System or hypervisor. \
The tests are expected to be minimal yet complete for booting a multi-core \
ARMv8 server platform, while leaving plenty of room \
for OEM or ODM innovations and design details."

# Home Page
HOMEPAGE = "https://github.com/UEFI/UEFI-SCT"

LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

SBBRVERSION="v1.0"
PV="${SBBRVERSION}+git${SRCPV}"

S = "${WORKDIR}/git"

inherit deploy

# No information for SRC_URI yet (only an external source tree was specified)
SRCREV  = "1ff61591f11a1b92518f2273ffeacaa90e474e5e"
SRC_URI = "git://github.com/UEFI/UEFI-SCT.git;protocol=https;branch=sbbr;user="${SCTUSERNAME}":"${SCTPASSWORD}" \
           file://sbbr-sct.patch \
          "

# NOTE: no Makefile found, unable to determine what needs to be done

do_configure () {
	# Specify any needed configure commands here
	:
	echo "do_configure()"
	# Specify any needed configure commands here
	rm -rf edk2
	git clone https://github.com/tianocore/edk2
	cd edk2
	git checkout UDK2018
	ln -s ../SctPkg SctPkg
	chmod +x SctPkg/build_sbbr.sh
	cd ..

	mkdir -p "tools/gcc"
	cd "tools/gcc"
	wget -nv "https://releases.linaro.org/components/toolchain/binaries/4.9-2016.02/aarch64-linux-gnu/gcc-linaro-4.9-2016.02-x86_64_aarch64-linux-gnu.tar.xz"
	#Untar the GCC Linaro files.
	tar -xf gcc-linaro-4.9-2016.02-x86_64_aarch64-linux-gnu.tar.xz
	cd ../..
}

do_compile () {
	# Specify compilation commands here
	:
	echo "$PWD: do_compile()"
	export PATH=$PATH:"$PWD/tools/gcc/gcc-linaro-4.9-2016.02-x86_64_aarch64-linux-gnu/bin"
	export BB_ENV_EXTRAWHITE="$BB_ENV_EXTRAWHITE PATH"
	echo "New PATH = $PATH"
	export CROSS_COMPILE="$PWD/tools/gcc/gcc-linaro-4.9-2016.02-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-"
	export BB_ENV_EXTRAWHITE="$BB_ENV_EXTRAWHITE CROSS_COMPILE"
	echo "NEW CROSS_COMPILE: $CROSS_COMPILE"

	cd edk2
	./SctPkg/build_sbbr.sh AARCH64 GCC
	cd ..
}

do_install () {
	# Specify install commands here
	:
	echo "do_install()"
	echo "Destination Directory: ${D}"
	echo "Source Directory: ${B}"
	cp -r ${B}/edk2/Build/SbbrSct/DEBUG_GCC49/SctPackageAARCH64 ${D}/sbbr
}

do_deploy () {
	# Specify deploy commands here
	:
	echo "do_deploy()"
	echo "Destination Directory: ${DEPLOYDIR}"
	echo "Source Directory: ${D}"
	install -d ${DEPLOYDIR}
	cp -r ${D}/sbbr ${DEPLOYDIR}/sbbr
}

addtask deploy before do_build after do_install

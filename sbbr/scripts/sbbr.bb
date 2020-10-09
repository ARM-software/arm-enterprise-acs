# Copyright (c) 2017-2020, Arm Limited or its affiliates. All rights reserved.
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
HOMEPAGE = "https://github.com/tianocore/edk2-test"

LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

SBBRVERSION="v1.2"
PV="${SBBRVERSION}+git${SRCPV}"

S = "${WORKDIR}/git"

inherit deploy

# No information for SRC_URI yet (only an external source tree was specified)
SRCREV  = "ed8a7477d47459be07cac790f7182afe68ed53c6"
SRC_URI = "git://github.com/tianocore/edk2-test;protocol=https \
           file://SbbrBootServices/ \
           file://SbbrEfiSpecVerLvl/ \
           file://SbbrRequiredUefiProtocols/ \
           file://SbbrSmbios/ \
           file://SbbrSysEnvConfig/ \
           file://SBBRRuntimeServices/ \
           file://SBBR_SCT.dsc \
           file://build_sbbr.sh \
           file://edk2-test-sbbr.patch \
          "

# NOTE: no Makefile found, unable to determine what needs to be done

do_configure () {
	# Specify any needed configure commands here
	:
	echo "do_configure()"
	cd ${WORKDIR}

	# Specify any needed configure commands here
	cp -r SbbrBootServices git/uefi-sct/SctPkg/TestCase/UEFI/EFI/BootServices/
	cp -r SbbrEfiSpecVerLvl SbbrRequiredUefiProtocols SbbrSmbios SbbrSysEnvConfig git/uefi-sct/SctPkg/TestCase/UEFI/EFI/Generic/
	cp -r SBBRRuntimeServices git/uefi-sct/SctPkg/TestCase/UEFI/EFI/RuntimeServices/
	cp SBBR_SCT.dsc git/uefi-sct/SctPkg/UEFI/
	cp build_sbbr.sh git/uefi-sct/SctPkg/

	if [ ! -d ${WORKDIR}/edk2 ]
	then
		echo "do_configure: Cloning EDK2 repository."
		git clone --recursive -b edk2-stable202008 https://github.com/tianocore/edk2.git
	fi

        # Checking for latest tool version
        VERSION=`/usr/bin/lsb_release -r | awk -F ':' '{ print $2 }' | sed 's/[ \t]*//g'`
        MAJOR_VERSION=`echo $VERSION | awk -F '.' '{print $1}'`
        MINOR_VERSION=`echo $VERSION | awk -F '.' '{print $2}'`
        DISTRO=`/usr/bin/lsb_release -i | awk -F ':' '{print $2}' | sed 's/[ \t]*//g'`

        if [ "$DISTRO" == "Ubuntu" ] && [ $MAJOR_VERSION -ge 20 ] && [ $MINOR_VERSION -ge 04 ]
        then
            cd ${WORKDIR}/edk2
            echo "do_configure: Adding additional LUVOS patch in Ubuntu."
            git apply ${TOPDIR}/../../luvos/patches/distros_patches/Basetools_change_warning.patch
            cd ${WORKDIR}
        fi

        if [ "$DISTRO" == "Debian" ] && [ $MAJOR_VERSION -ge 10 ]
        then
            cd ${WORKDIR}/edk2
            echo "do_configure: Adding additional LUVOS patch."
            git apply ${TOPDIR}/../../luvos/patches/distros_patches/Basetools_change_warning.patch
            cd ${WORKDIR}
        fi

	ln -s ${WORKDIR}/git/uefi-sct/SctPkg SctPkg
	chmod +x SctPkg/build_sbbr.sh

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
	export PATH=$PATH:"${WORKDIR}/tools/gcc/gcc-linaro-4.9-2016.02-x86_64_aarch64-linux-gnu/bin"
	export BB_ENV_EXTRAWHITE="$BB_ENV_EXTRAWHITE PATH"
	echo "New PATH = $PATH"

        MACHINE=`uname -m`
        if [ $MACHINE = "aarch64" ]; then
           export CROSS_COMPILE="/usr/bin/"
        else
           export CROSS_COMPILE="${WORKDIR}/tools/gcc/gcc-linaro-4.9-2016.02-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-"
        fi

        export BB_ENV_EXTRAWHITE="$BB_ENV_EXTRAWHITE CROSS_COMPILE"
	echo "NEW CROSS_COMPILE: $CROSS_COMPILE"

	cd ${WORKDIR}
        pwd
	./SctPkg/build_sbbr.sh AARCH64 GCC
}

do_install () {
	# Specify install commands here
	:
	echo "do_install()"
	echo "Destination Directory: ${D}"
	echo "Source Directory: ${B}"
	cp -r ${WORKDIR}/Build/SbbrSct/DEBUG_GCC49/SctPackageAARCH64 ${D}/sbbr
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

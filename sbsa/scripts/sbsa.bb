# Copyright (c) 2016-2021, Arm Limited or its affiliates. All rights reserved.
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

LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

# SCT files placed in ${WORKDIR}/git
# GCC files placed in ${WORKDIR}/gcc-linaro-5.3.1-2016.05-x86_64_aarch64-linux-gnu
SRC_URI = "git://github.com/ARM-software/sbsa-acs.git;protocol=https \
           https://releases.linaro.org/components/toolchain/binaries/5.3-2016.05/aarch64-linux-gnu/gcc-linaro-5.3.1-2016.05-x86_64_aarch64-linux-gnu.tar.xz \
           file://compile.sh"

PV = "1.0+git${SRCPV}"
SRCREV = "3a0b377406850206db9dfba9d3b881a87e300d92"

# GCC checksum.
SRC_URI[md5sum] = "24ac2e26f50f49f3043f281440b41bba"

S = "${WORKDIR}/git"

inherit deploy

do_configure () {
    # Downloading EDK2 into ${WORKDIR}/edk2.
    echo "do_configure: Checking if EDK2 repository already exists."
    export GIT_SSL_NO_VERIFY=1
    cd ${WORKDIR}
    if [ ! -d ${WORKDIR}/edk2 ]
    then
        echo "do_configure: Cloning EDK2 repository."
        git clone -b edk2-stable202008 https://github.com/tianocore/edk2.git
        cd ${WORKDIR}/edk2
        git submodule update --init
        cd ${WORKDIR}
    fi

    # Checking for latest tool version
    VERSION=$(/usr/bin/lsb_release -r | awk -F ':' '{ print $2 }' | sed 's/[ \t]*//g')
    MAJOR_VERSION=$(echo $VERSION | awk -F '.' '{print $1}')
    MINOR_VERSION=$(echo $VERSION | awk -F '.' '{print $2}')
    DISTRO=$(/usr/bin/lsb_release -i | awk -F ':' '{print $2}' | sed 's/[ \t]*//g')

    if [ "$DISTRO" = "Ubuntu" ] &&  [ $MAJOR_VERSION -ge 20 ] && [ $MINOR_VERSION -ge 04 ]
    then
        cd ${WORKDIR}/edk2
        echo "do_configure: Adding additional LUVOS patch Ubuntu."
        git apply ${TOPDIR}/../../luvos/patches/distros_patches/Basetools_change_warning.patch
        cd ${WORKDIR}
    fi

    if [ "$DISTRO" = "Debian" ] && [ $MAJOR_VERSION -ge 10 ]
    then
        cd ${WORKDIR}/edk2
        echo "do_configure: Adding additional LUVOS patch."
        git apply ${TOPDIR}/../../luvos/patches/distros_patches/Basetools_change_warning.patch
        cd ${WORKDIR}
    fi

    if [ ! -d ${WORKDIR}/edk2-libc ]
    then
        echo "do_configure: Cloning EDK2 LibC repository."
        git clone https://github.com/tianocore/edk2-libc.git
    fi

    # Linking SBSA and EDK2.
    echo "do_configure: Linking SBSA to EDK2."
    cd ${WORKDIR}/git/uefi_app
    if [ -d val ]
    then
        echo "do_configure: Existing val found, deleted."
        rm val
    fi
    if [ -d pal ]
    then
        echo "do_configure: Existing pal found, deleted."
        rm pal
    fi
    if [ -d test_pool ]
    then
        echo "do_configure: Existing test_pool found, deleted."
        rm test_pool
    fi
    cd ${WORKDIR}/edk2/ShellPkg/Application
    if [ -d sbsa-acs ]
    then
        echo "do_configure: Existing sbsa-acs found, deleted."
        rm sbsa-acs
    fi
    cd ${WORKDIR}/git
    set -- "${WORKDIR}/git" "${WORKDIR}/edk2"
    . ${WORKDIR}/git/tools/scripts/avssetup.sh
    set --

    # Modifying EDK2 to build SBSA.
    echo "do_configure: Modifying edk2/ShellPkg/ShellPkg.dsc to build SBSA."
    if ! grep -q SbsaNistLib "${WORKDIR}/edk2/ShellPkg/ShellPkg.dsc"
    then
        sed -i '/LibraryClasses.common/ a \ \ !ifdef $(ENABLE_NIST)\n\ \ \ \ SbsaNistLib|ShellPkg/Application/sbsa-acs/test_pool/nist_sts/SbsaNistLib.inf\n\ \ \ \ SbsaValNistLib|ShellPkg/Application/sbsa-acs/val/SbsaValNistLib.inf\n\ \ \ \ SbsaPalNistLib|ShellPkg/Application/sbsa-acs/platform/pal_uefi/SbsaPalNistLib.inf\n\ \ !else\n\ \ \ \ SbsaValLib|ShellPkg/Application/sbsa-acs/val/SbsaValLib.inf\n\ \ \ \ SbsaPalLib|ShellPkg/Application/sbsa-acs/platform/pal_uefi/SbsaPalLib.inf\n\ \ !endif' ${WORKDIR}/edk2/ShellPkg/ShellPkg.dsc
    fi
    if ! grep -q UefiRuntimeLib "${WORKDIR}/edk2/ShellPkg/ShellPkg.dsc"
    then
        sed -i '/LibraryClasses.common/ a \ \ UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf' ${WORKDIR}/edk2/ShellPkg/ShellPkg.dsc
    fi
    if ! grep -q SbsaAvs "${WORKDIR}/edk2/ShellPkg/ShellPkg.dsc"
    then
        sed -i '/Components/ a \ \ !ifdef $(ENABLE_NIST)\n\ \ \ \ ShellPkg/Application/sbsa-acs/uefi_app/SbsaAvsNist.inf\n\ \ !else\n\ \ \ \ ShellPkg/Application/sbsa-acs/uefi_app/SbsaAvs.inf\n\ \ !endif' ${WORKDIR}/edk2/ShellPkg/ShellPkg.dsc
    fi
    if ! grep -q DENABLE_NIST "${WORKDIR}/edk2/ShellPkg/ShellPkg.dsc"
    then
        sed -i '/*_*_*_CC_FLAGS/c  !ifdef $(ENABLE_NIST)\n\ \ *_*_*_CC_FLAGS = -DENABLE_NIST\n!else\n\ \ *_*_*_CC_FLAGS =\n!endif\n\n!include StdLib/StdLib.inc' ${WORKDIR}/edk2/ShellPkg/ShellPkg.dsc
    fi
    if ! grep -q ShellAppMainsbsa "${WORKDIR}/edk2-libc/StdLib/LibC/Main/Main.c"
    then
        sed -i 's/main( /ShellAppMainsbsa( /g' ${WORKDIR}/edk2-libc/StdLib/LibC/Main/Main.c
    fi
    if grep -q "static const int map" "${WORKDIR}/edk2-libc/StdLib/LibC/Main/Arm/flt_rounds.c"
    then
        sed -i 's/static const int map/const int map/g' ${WORKDIR}/edk2-libc/StdLib/LibC/Main/Arm/flt_rounds.c
    fi
    MACHINE=$(uname -m)
    echo "Architecture Detected : $MACHINE"
    if [ $MACHINE = "aarch64" ]; then
        if ! grep -q AARCH64_BUILD "${WORKDIR}/edk2/ShellPkg/ShellPkg.dsc"
        then
            sed -i '/BuildOptions/ a \ \ *_*_*_CC_FLAGS = -D_AARCH64_BUILD_' ${WORKDIR}/edk2/ShellPkg/ShellPkg.dsc
        fi
    fi

}

do_compile () {
    export GIT_SSL_NO_VERIFY=1
    cd ${WORKDIR}
    ./compile.sh ${WORKDIR}
}

do_install () {
    echo "do_install: Copying SBSA files to destination ${D}"
    mkdir ${D}/sbsa
	cp ${WORKDIR}/edk2/Build/Shell/DEBUG_GCC49/AARCH64/Sbsa.efi ${D}/sbsa/Sbsa.efi
}

do_deploy () {
    echo "do_deploy: Deploying SBSA files to destionation ${DEPLOYDIR}"
    install -d ${DEPLOYDIR}
    cp -r ${D}/sbsa ${DEPLOYDIR}/sbsa
}

addtask deploy before do_build after do_install

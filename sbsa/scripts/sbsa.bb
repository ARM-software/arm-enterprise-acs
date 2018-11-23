# Copyright (c) 2016-2018, Arm Limited or its affiliates. All rights reserved.
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
SRC_URI = "git://github.com/ARM-software/sbsa-acs.git;protocol=https;branch=release \
           https://releases.linaro.org/components/toolchain/binaries/5.3-2016.05/aarch64-linux-gnu/gcc-linaro-5.3.1-2016.05-x86_64_aarch64-linux-gnu.tar.xz \
           file://compile.sh"

PV = "1.0+git${SRCPV}"
SRCREV = "e49b7b23203948d750623d59535cce9e8bc52285"

# GCC checksum.
SRC_URI[md5sum] = "24ac2e26f50f49f3043f281440b41bba"

S = "${WORKDIR}/git"

inherit deploy

do_configure () {
    # Downloading EDK2 into ${WORKDIR}/edk2.
    echo "do_configure: Checking if EDK2 repository already exists."
    cd ${WORKDIR}
    if [ ! -d ${WORKDIR}/edk2 ]
    then
        echo "do_configure: Cloning EDK2 repository."
        git clone -b UDK2017 https://github.com/tianocore/edk2.git
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
    cd ${WORKDIR}/edk2/AppPkg/Applications
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
    if ! grep -q SbsaPalLib "${WORKDIR}/edk2/ShellPkg/ShellPkg.dsc"
    then
        sed -i '/LibraryClasses.common/ a \ \ SbsaPalLib|AppPkg/Applications/sbsa-acs/platform/pal_uefi/SbsaPalLib.inf' ${WORKDIR}/edk2/ShellPkg/ShellPkg.dsc
    fi
    if ! grep -q SbsaValLib "${WORKDIR}/edk2/ShellPkg/ShellPkg.dsc"
    then
        sed -i '/LibraryClasses.common/ a \ \ SbsaValLib|AppPkg/Applications/sbsa-acs/val/SbsaValLib.inf' ${WORKDIR}/edk2/ShellPkg/ShellPkg.dsc
    fi
    if ! grep -q SbsaAvs "${WORKDIR}/edk2/ShellPkg/ShellPkg.dsc"
    then
        sed -i '/Components/ a \ \ AppPkg/Applications/sbsa-acs/uefi_app/SbsaAvs.inf' ${WORKDIR}/edk2/ShellPkg/ShellPkg.dsc
    fi
}

do_compile () {
    cd ${WORKDIR}
    ./compile.sh ${WORKDIR}
}

do_install () {
    echo "do_install: Copying SBSA files to destination ${D}"
    mkdir ${D}/sbsa
	cp ${WORKDIR}/edk2/Build/Shell/DEBUG_GCC49/AARCH64/Sbsa.efi ${D}/sbsa/Sbsa.efi
	cp ${WORKDIR}/edk2/ShellBinPkg/UefiShell/AArch64/Shell.efi ${D}/sbsa/Shell.efi
}

do_deploy () {
    echo "do_deploy: Deploying SBSA files to destionation ${DEPLOYDIR}"
    install -d ${DEPLOYDIR}
    cp -r ${D}/sbsa ${DEPLOYDIR}/sbsa
}

addtask deploy before do_build after do_install

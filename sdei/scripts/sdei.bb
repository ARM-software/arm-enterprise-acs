# Copyright (c) 2018-2020, Arm Limited or its affiliates. All rights reserved.
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
# This file was derived from the 'Hello World!' example recipe in the
# Yocto Project Development Manual.
#

DESCRIPTION = "Builds Uefi Application"
SECTION = "exampled"
LICENSE = "CLOSED"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
inherit deploy

SRC_URI =  " https://releases.linaro.org/components/toolchain/binaries/5.3-2016.05/aarch64-linux-gnu/gcc-linaro-5.3.1-2016.05-x86_64_aarch64-linux-gnu.tar.xz;name=name3 \
             file://sdei/ \
	     file://compile.sh"
SRC_URI[name3.md5sum] = "24ac2e26f50f49f3043f281440b41bba"

do_unpack () {
  tar -xf ${DL_DIR}/gcc-linaro*.tar.xz
  mv gcc-linaro-5.3.1-2016.05-x86_64_aarch64-linux-gnu/ ${S}/gcc-linaro/
  ls
}

do_configure () {
    rm -rf edk2
    git clone https://github.com/tianocore/edk2
    cd edk2
    git checkout UDK2018
    # Checking for latest tool version
    VERSION=`/usr/bin/lsb_release -r | awk -F ':' '{ print $2 }' | sed 's/[ \t]*//g'`
    MAJOR_VERSION=`echo $VERSION | awk -F '.' '{print $1}'`
    MINOR_VERSION=`echo $VERSION | awk -F '.' '{print $2}'`
    DISTRO=`/usr/bin/lsb_release -i | awk -F ':' '{print $2}' | sed 's/[ \t]*//g'`
    if [ "$DISTRO" == "Ubuntu" ] && [ $MAJOR_VERSION -ge 20 ] && [ $MINOR_VERSION -ge 04 ]
    then
        echo "do_configure: Adding additional LUVOS patch in Ubuntu."
        git apply ${TOPDIR}/../../luvos/patches/distros_patches/Basetools_change_warning_sdei.patch
    fi

    if [ "$DISTRO" == "Debian" ] && [ $MAJOR_VERSION -ge 10 ]
    then
        echo "do_configure: Adding additional LUVOS patch."
        git apply ${TOPDIR}/../../luvos/patches/distros_patches/Basetools_change_warning_sdei.patch
    fi

    cd ..

    cp -r ${THISDIR}/files/sdei edk2/AppPkg/Applications/sdei-acs
    sed -i '/LibraryClasses.common/ a\ SdeiValLib|AppPkg/Applications/sdei-acs/val/SdeiValLib.inf' edk2/ShellPkg/ShellPkg.dsc
    sed -i '/LibraryClasses.common/ a\ SdeiPalLib|AppPkg/Applications/sdei-acs/platform/pal_uefi/SdeiPalLib.inf' edk2/ShellPkg/ShellPkg.dsc
    sed -i '/Components/ a\ AppPkg/Applications/sdei-acs/uefi_app/SdeiAcs.inf' edk2/ShellPkg/ShellPkg.dsc
    
    MACHINE=`uname -m`
    echo "Architecture Detected : $MACHINE"
    if [ $MACHINE = "aarch64" ]; then
        if ! grep -q AARCH64_BUILD "edk2/ShellPkg/ShellPkg.dsc"
        then
            sed -i '/BuildOptions/ a \ \ *_*_*_CC_FLAGS = -D_AARCH64_BUILD_' edk2/ShellPkg/ShellPkg.dsc
        fi
    fi


}

do_compile () {
   #export GCC49_AARCH64_PREFIX=${S}/gcc-linaro/bin/aarch64-linux-gnu-
   
   MACHINE=`uname -m`
   if [ $MACHINE = "aarch64" ]; then
      export GCC49_AARCH64_PREFIX=/usr/bin/
   else
      export GCC49_AARCH64_PREFIX=${S}/gcc-linaro/bin/aarch64-linux-gnu-
   fi
   
   /bin/bash ${THISDIR}/files/compile.sh ${S}
}


do_install () {
    echo "do_install: Copying SDEI files to destination ${D}"
    mkdir ${D}/sdei
	cp ${S}/edk2/Build/Shell/DEBUG_GCC49/AARCH64/Sdei.efi ${D}/sdei/Sdei.efi
	cp ${S}/edk2/ShellBinPkg/UefiShell/AArch64/Shell.efi ${D}/sdei/Shell.efi
}


do_deploy () {
    echo "do_deploy: Deploying SDEI files to destination ${DEPLOYDIR}"
    install -d ${DEPLOYDIR}
    cp -r ${D}/sdei ${DEPLOYDIR}/sdei
}

addtask deploy before do_build after do_install


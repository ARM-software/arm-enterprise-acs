#!/bin/bash

W=${PWD}/workspace
S=${PWD}

if [ ! -d workspace ]
then
    mkdir workspace
fi
cd ${W}
if [ ! -d gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu ]
then
    rm -f gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu.tar.xz
    wget https://releases.linaro.org/components/toolchain/binaries/6.3-2017.05/aarch64-linux-gnu/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu.tar.xz
    xz -d gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu.tar.xz
    tar -xf gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu.tar
fi

if [ ! -d output ]
then
    mkdir output
fi

echo "Building SDEI ACS for UEFI"
if [ ! -d edk2 ]
then
    git clone https://github.com/tianocore/edk2.git
fi
cd ${W}/edk2
make -C BaseTools

if ! grep -q SdeiPalLib "ShellPkg/ShellPkg.dsc"
then
    sed -i '/LibraryClasses.common/ a \ \ SdeiPalLib|AppPkg/Applications/sdei-acs/platform/pal_uefi/SdeiPalLib.inf' ShellPkg/ShellPkg.dsc
fi
if ! grep -q SdeiValLib "ShellPkg/ShellPkg.dsc"
then
    sed -i '/LibraryClasses.common/ a \ \ SdeiValLib|AppPkg/Applications/sdei-acs/val/SdeiValLib.inf' ShellPkg/ShellPkg.dsc
fi
if ! grep -q SdeiAcs "ShellPkg/ShellPkg.dsc"
then
    sed -i '/Components/ a \ \ AppPkg/Applications/sdei-acs/uefi_app/SdeiAcs.inf' ShellPkg/ShellPkg.dsc
fi

rm -f AppPkg/Applications/sdei-acs
ln -s ${S} AppPkg/Applications/sdei-acs
export GCC49_AARCH64_PREFIX=${W}/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
source edksetup.sh
./AppPkg/Applications/sdei-acs/scripts/acsbuild.sh
cp Build/Shell/DEBUG_GCC49/AARCH64/Sdei.efi ../output

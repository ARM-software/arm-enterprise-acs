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

echo "Building SDEI ACS for Linux"
if [ ! -d linux-acs ]
then
    git clone git://linux-arm.org/linux-acs.git
fi
if [ ! -d linux ]
then
    git clone https://kernel.googlesource.com/pub/scm/linux/kernel/git/torvalds/linux
    cd linux
    git checkout -b v4.13 v4.13
    git apply ${W}/linux-acs/kernel/src/0001-Enterprise-acs-linux-v4.13.patch
    cd ${W}
fi
cd linux
export ARCH=arm64
export CROSS_COMPILE=${W}/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
make defconfig
make -j16
cp arch/arm64/boot/Image ../output
cd ${W}/linux-acs/sdei-acs-drv/files
./setup.sh ${S}
export KERNEL_SRC=${W}/linux
make
cp sdei_acs.ko ${W}/output

cd ${S}/linux_app/sdei-acs-app
make
cp sdei ${W}/output
cd ${S}

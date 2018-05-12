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

#if [ ! -d linux-acs ]
#then
#   git clone git://linux-arm.org/linux-acs.git
#fi


if [ ! -d linux ]
then
    git clone https://kernel.googlesource.com/pub/scm/linux/kernel/git/torvalds/linux
    cd linux
    git checkout -b v4.13 v4.13
    git apply ${S}/kernel/patches/0001-Enterprise-acs-linux-v4.13.patch
   cd ${W}
fi
cd linux
export ARCH=arm64
export CROSS_COMPILE=${W}/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
make defconfig
make -j16
cp arch/arm64/boot/Image ../output

#Creating temporary directories for compilation
mkdir ${W}/drv
mkdir ${W}/drv/pal
mkdir ${W}/drv/val
mkdir -p  ${W}/drv/test_pool/val/include
#Copying files to the temporary directories
cp -r ${S}/sdei-acs-drv/sdei-acs-drv/files/* ${W}/drv
cp -r ${S}/sdei-acs-drv/sdei-acs-pal/files/* ${W}/drv/pal/
cp -r ${S}/val/*  ${W}/drv/val/
cp -r ${S}/val/include/* ${W}/drv/test_pool/val/include/
cp -r ${S}/test_pool/ ${W}/drv/
cp ${W}/drv/val/include/pal_interface.h ${W}/drv/pal/include/

export KERNEL_SRC=${W}/linux
cd ${W}/drv/pal
make
cp sdei_acs_pal.o ../

cd ${W}/drv/val
make
cp sdei_acs_val.o ../

cd ${W}/drv/test_pool
make
cp sdei_acs_test.o ../

cd ${W}/drv
make
cp sdei_acs.ko ${W}/output

cd ${S}/linux_app/sdei-acs-app/files/
make
cp sdei ${W}/output
cd ${S}

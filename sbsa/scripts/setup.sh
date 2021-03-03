#!/bin/bash
# Copyright (c) 2017, ARM Limited or its affiliates. All rights reserved.
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


#create tmp directory
TOPDIR=$PWD
SRCDIR=$PWD/src
LUVDIR=$PWD/luv
SBSA_ACS_COMMIT=cc4e1eb3cd38c9565120fa24ce3641087e281d19
LINUX_ACS_COMMIT=8345992487e2f22cb5b148f5772be1da5801ab7c

rm -rf $SRCDIR

git clone https://github.com/ARM-software/sbsa-acs.git src
cd $SRCDIR || exit
git checkout $SBSA_ACS_COMMIT
cd $TOPDIR || exit
git clone git://linux-arm.org/linux-acs.git
cd linux-acs || exit
git checkout $LINUX_ACS_COMMIT
cd $TOPDIR || exit

mv linux-acs/sbsa-acs-drv/files/platform/pal_linux $SRCDIR/platform/
mv linux-acs/sbsa-acs-drv $SRCDIR
mv linux-acs/kernel/src/0001-Enterprise-acs-linux-v4.18.patch $LUVDIR/meta-luv/recipes-kernel/linux/linux-luv/
rm -rf linux-acs
cd $TOPDIR

rm -rf $LUVDIR/meta-luv/recipes-bsp/sbsa
mkdir -p $LUVDIR/meta-luv/recipes-bsp/sbsa/files
cp $TOPDIR/sbsa/scripts/sbsa.bb $LUVDIR/meta-luv/recipes-bsp/sbsa/sbsa.bb
cp $TOPDIR/sbsa/scripts/compile.sh $LUVDIR/meta-luv/recipes-bsp/sbsa/files/compile.sh

rm -rf $LUVDIR/meta-luv/recipes-utils/sbsa-acs-app
mkdir -p $LUVDIR/meta-luv/recipes-utils/sbsa-acs-app
cp -r $SRCDIR/linux_app/* $LUVDIR/meta-luv/recipes-utils/sbsa-acs-app/

rm -rf $LUVDIR/meta-luv/recipes-core/sbsa-acs-pal
mkdir -p $LUVDIR/meta-luv/recipes-core/sbsa-acs-pal
mkdir -p $SRCDIR/platform/pal_linux/files/val/include/
cp $SRCDIR/val/include/pal_interface.h $SRCDIR/platform/pal_linux/files/val/include/
cp -r $SRCDIR/platform/pal_linux/* $LUVDIR/meta-luv/recipes-core/sbsa-acs-pal/

rm -rf $LUVDIR/meta-luv/recipes-core/sbsa-acs-val
mkdir -p $LUVDIR/meta-luv/recipes-core/sbsa-acs-val/files
cp $TOPDIR/sbsa/scripts/sbsa-acs-val.bb $LUVDIR/meta-luv/recipes-core/sbsa-acs-val/sbsa-acs-val.bb
cp -r $SRCDIR/val/* $LUVDIR/meta-luv/recipes-core/sbsa-acs-val/files/

rm -rf $LUVDIR/meta-luv/recipes-core/sbsa-acs-test
mkdir -p $LUVDIR/meta-luv/recipes-core/sbsa-acs-test/files/val
cp $TOPDIR/sbsa/scripts/sbsa-acs-test.bb $LUVDIR/meta-luv/recipes-core/sbsa-acs-test/
cp -r $SRCDIR/test_pool/pcie $LUVDIR/meta-luv/recipes-core/sbsa-acs-test/files/
cp -r $SRCDIR/test_pool/exerciser $LUVDIR/meta-luv/recipes-core/sbsa-acs-test/files/
cp -r $SRCDIR/test_pool/Makefile $LUVDIR/meta-luv/recipes-core/sbsa-acs-test/files/
cp -r $SRCDIR/val/include $LUVDIR/meta-luv/recipes-core/sbsa-acs-test/files/val/
cp -r $SRCDIR/val/COPYING $LUVDIR/meta-luv/recipes-core/sbsa-acs-test/files/

rm -rf $LUVDIR/meta-luv/recipes-core/sbsa-acs-drv
cp -r $SRCDIR/sbsa-acs-drv $LUVDIR/meta-luv/recipes-core/
mkdir -p $LUVDIR/meta-luv/recipes-core/sbsa-acs-drv/files/val
cp -r $SRCDIR/val/include $LUVDIR/meta-luv/recipes-core/sbsa-acs-drv/files/val

cd $SRCDIR/..

rm -rf $SRCDIR

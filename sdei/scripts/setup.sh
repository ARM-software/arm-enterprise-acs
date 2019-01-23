#Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
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


#create tmp directory
TOPDIR=$PWD
SRCDIR=$PWD/sdei
LUVDIR=$PWD/luv

cd $SRCDIR 
git clone git://linux-arm.org/linux-acs.git
cd linux-acs
git checkout -b v19.01_REL2.1 v19.01_REL2.1
cd ..
cp -r linux-acs/sdei-acs-drv .
rm -rf linux-acs
cd $TOPDIR

#UEFI App
rm -rf $LUVDIR/meta-luv/recipes-bsp/sdei
mkdir -p $LUVDIR/meta-luv/recipes-bsp/sdei/files
cp $SRCDIR/scripts/sdei.bb    $LUVDIR/meta-luv/recipes-bsp/sdei/sdei.bb
cp $SRCDIR/scripts/compile.sh $LUVDIR/meta-luv/recipes-bsp/sdei/files/compile.sh
cp -r $SRCDIR                 $LUVDIR/meta-luv/recipes-bsp/sdei/files/

#Linux App
rm -rf $LUVDIR/meta-luv/recipes-utils/sdei-acs-app
cp -r $SRCDIR/linux_app/* $LUVDIR/meta-luv/recipes-utils/

#Linux kernel module

#PAL and module
cp -r $SRCDIR/sdei-acs-drv/*           $LUVDIR/meta-luv/recipes-core/

mkdir -p $LUVDIR/meta-luv/recipes-core/sdei-acs-drv/files/val/include
cp -r $SRCDIR/val/include/* $LUVDIR/meta-luv/recipes-core/sdei-acs-drv/files/val/include/
mkdir -p $LUVDIR/meta-luv/recipes-core/sdei-acs-pal/files/val/include
cp $SRCDIR/val/include/pal_interface.h $LUVDIR/meta-luv/recipes-core/sdei-acs-pal/files/val/include

#VAL
mkdir -p $LUVDIR/meta-luv/recipes-core/sdei-acs-val/files/
cp  $TOPDIR/sdei/scripts/sdei-acs-val_0.1.bb  $LUVDIR/meta-luv/recipes-core/sdei-acs-val/
cp  $TOPDIR/sdei/scripts/COPYING              $LUVDIR/meta-luv/recipes-core/sdei-acs-val/files/
cp -r $SRCDIR/val/*                           $LUVDIR/meta-luv/recipes-core/sdei-acs-val/files/

#Test
mkdir -p $LUVDIR/meta-luv/recipes-core/sdei-acs-test/files/
cp $TOPDIR/sdei/scripts/sdei-acs-test_0.1.bb $LUVDIR/meta-luv/recipes-core/sdei-acs-test/
cp -r $SRCDIR/test_pool/*                    $LUVDIR/meta-luv/recipes-core/sdei-acs-test/files/

mkdir -p $LUVDIR/meta-luv/recipes-core/sdei-acs-test/files/val/include
cp -r $SRCDIR/val/include/*      $LUVDIR/meta-luv/recipes-core/sdei-acs-test/files/val/include/
cp $TOPDIR/sdei/scripts/COPYING  $LUVDIR/meta-luv/recipes-core/sdei-acs-test/files/

rm -rf $SRCDIR/sdei-acs-drv

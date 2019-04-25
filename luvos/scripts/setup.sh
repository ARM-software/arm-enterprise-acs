#!/bin/bash
# Copyright (c) 2017-2019, ARM Limited or its affiliates. All rights reserved.
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

LUVDIR=$PWD/luv
LUVDIRTEMP=$PWD/luvtemp
TOPDIR=$PWD

CLONEDIR=$LUVDIR;
if [ -d $TOPDIR/luv ]; then
	CLONEDIR=$LUVDIRTEMP;
fi

git clone https://github.com/intel/luv-yocto.git $CLONEDIR
cd $CLONEDIR
git checkout -b v2.3 v2.3
git am $TOPDIR/luvos/patches/luvos.patch
cd $TOPDIR

if [ -d  $LUVDIRTEMP ]; then
	cp -r $LUVDIRTEMP/* $LUVDIR/
	echo "removing $LUVDIRTEMP"
	rm -rf $LUVDIRTEMP
fi

ln -s $TOPDIR/luvos/scripts/luv-collect-results $LUVDIR/meta-luv/recipes-core/luv-test/luv-test/luv-collect-results
ln -s $TOPDIR/luvos/scripts/luv-sbsa-test $LUVDIR/meta-luv/recipes-core/luv-test/luv-test/luv-sbsa-test
ln -s $TOPDIR/luvos/scripts/luv-sdei-test $LUVDIR/meta-luv/recipes-core/luv-test/luv-test/luv-sdei-test
ln -s $TOPDIR/luvos/patches/0001-Enterprise-acs-linux-v4.18.patch $LUVDIR/meta-luv/recipes-kernel/linux/linux-luv/

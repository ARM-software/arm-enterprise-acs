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

LUVDIR=$PWD/luv

rm -rf $LUVDIR/meta-luv/recipes-bsp/sbbr
mkdir -p $LUVDIR/meta-luv/recipes-bsp/sbbr/files
ln -s $PWD/sbbr/scripts/sbbr.bb $LUVDIR/meta-luv/recipes-bsp/sbbr/sbbr.bb
ln -s $PWD/sbbr/patches/sbbr-sct.patch $LUVDIR/meta-luv/recipes-bsp/sbbr/files/sbbr-sct.patch
ln -s $PWD/sbbr/patches/sbbr-fwts.patch $LUVDIR/meta-luv/recipes-core/fwts/fwts/sbbr-fwts.patch

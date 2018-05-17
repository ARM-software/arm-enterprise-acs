# Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
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

LICENSE = "Apachev2"

inherit module

SRC_URI = " file://tests/ \
            file://val/  \
            file://Makefile \
	    file://COPYING \
            "
S = "${WORKDIR}"
LIC_FILES_CHKSUM = "file://COPYING;md5=2a944942e1496af1886903d274dedb13"


do_install_append () {
        cp ${S}/sdei_acs_test.o ${DEPLOY_DIR_IMAGE}/
}

#! /bin/bash
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

ALLOWED_FIRST_PARAM="luvos sbbr sbsa sdei"

display_usage()
{
    echo
    echo "Usage: $0 [luvos|sbbr|sbsa|sdei]"
    echo
}

validate_param()
{
    occurrence=`echo $ALLOWED_FIRST_PARAM | grep -w $1 | wc -l`
    if [ $occurrence -eq 0 ]; then
        echo "Invalid option $1"
	display_usage;
        exit 1;
    fi
}

if [ $# -gt 1 ]; then
    display_usage;
    exit 1;
fi

if [ $# -gt 0 ]; then
    validate_param $*
fi

./check_deps.sh
if [ $? != 0 ]
then
    echo -e "\nDependencies have not been met to successfully download the software."
    echo -e "\nWould you like to install the dependencies? [Y/N]"
    read input
    INSTALL=${input^^}
    if [ "${INSTALL}" == "Y" ]
    then
        echo -e "Installing dependencies."
        sudo ./check_deps.sh -ip
        if [ $? != 0 ]
        then
            echo -e "$(tput setaf 1)Dependencies install failed. Exiting.$(tput sgr 0)"
            exit
        fi
    else
        exit
    fi
fi

if [ $# -eq 1 ]; then
    echo "Setting up $1 ..."
    ./$1/scripts/setup.sh
else
    ./luvos/scripts/setup.sh
    ./sbbr/scripts/setup.sh
    ./sbsa/scripts/setup.sh
    ./sdei/scripts/setup.sh
fi


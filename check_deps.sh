#! /bin/bash
############################################################################
#                                                                          #
# This software may be used only as authorized by a licensing              #
# agreement from ARM Limited.                                              #
#                                                                          #
# (C) Copyright 2016 ARM Limited                                           #
# The entire notice above must be reproduced on all authorized copies and  #
# copies may only be made to the extent permitted by a licensing agreement #
# from ARM Limited.                                                        #
#                                                                          #
# Version:       1.0                                                       #
# Date:          2016.12.01                                                #
# Description:   This script verifies dependencies are met to enable       #
#                the download and build of Architecture Compliance         #
#                Test Suite.                                               #
#                                                                          #
############################################################################


############################################################################
#                                                                          #
#  Global Variables                                                        #
#                                                                          #
############################################################################

LOGFILE="./sync.log"
INSTALL_PREREQS=0
REMOVE_PREREQS=0
NEEDED_DRV_SPACE=1000000  #1GB needed.  Value is in 1KB chunks (10=10KB)
TEST_SUITE="ACS"

PACKAGES_TO_CHECK=(
        "bison"
        "chrpath"
        "flex"
        "g++"
        "gawk"
        "git"
        "libssl-dev"
        "python-crypto"
        "python-wand"
        "texinfo"
        "uuid-dev"
	)

RC_ERROR=1
RC_SUCCESS=0
RC_HELP=2

############################################################################
#                                                                          #
#  Function:     cleanup                                                   #
#  Description:  Cleanup any resources, files or partial downloads before  #
#                exiting the script.                                       #
#                                                                          #
############################################################################
function cleanup()
{
        echo -e "\n$1\n" | tee -a $LOGFILE
        exit $RC
}

############################################################################
#                                                                          #
#  Function: check_space                                                   #
#  Description: Check available space to ensure there is enough to         #
#               download and install all packages.                         #
#                                                                          #
############################################################################
function check_space()
{
  AVAIL_SPACE=`df .  | awk '/[0-9]%/{print $(NF-2)}'`
  echo -n "Required space is"
  if [ $AVAIL_SPACE -gt $NEEDED_DRV_SPACE ]
  then
    RC=$RC_SUCCESS
    echo -e " $(tput setaf 2) available$(tput sgr 0).\n" | tee -a $LOGFILE
  else
    echo -e " $(tput setaf 1) not available$(tput sgr 0).\n" | tee -a $LOGFILE
    RC=$RC_ERROR
  fi
  return $RC
}


############################################################################
#                                                                          #
#  Function: remove_packages                                               #
#  Description: Remove existing packages to allow debug of the script.     #
#               List of the packages are a global value that is reversed   #
#               so packages are removed in the reverse order they were     #
#               installed.  The dpkg command is used to determine if a     #
#               package is already installed.  Remove is only attempted    #
#               for packages that are already installed.                   #
#                                                                          #
############################################################################
function remove_packages()
{
  for (( idx=${#PACKAGES_TO_CHECK[@]}-1 ; idx>=0 ; idx-- ))
  do
    if [ `dpkg -l ${PACKAGES_TO_CHECK[idx]} 2>/dev/null | awk '{ print $1 }' | grep -c "ii"` -eq 1 ]
    then
      echo -e "Removing package ${PACKAGES_TO_CHECK[idx]}" | tee -a $LOGFILE
      `apt-get -y -q remove ${PACKAGES_TO_CHECK[idx]} 1>$LOGFILE` | tee -a $LOGFILE
    fi
  done
  echo -e "Package removal process is complete.\n" | tee -a $LOGFILE
  exit $RC_SUCCESS
}

############################################################################
#                                                                          #
#  Function: check_packages                                                #
#  Description: Install any missing dependencies based on the global list  #
#               of packages needed to support the ACS test suite download  #
#               and build.                                                 #
#               The dpkg command is used to determine if a package         #
#               installation is needed.  The apt-get install command is    #
#               used to install the packages.                              #
#                                                                          #
############################################################################
function  check_packages()
{
  RC=0
  declare -a LIB_TO_INSTALL
  for (( idx=0 ; idx < ${#PACKAGES_TO_CHECK[@]} ; idx++ ))
  do
    if [ `dpkg -l ${PACKAGES_TO_CHECK[idx]} 2>/dev/null | awk '{ print $1 }' | grep -c "ii"` -ne 1 ]
    then
      if [ $INSTALL_PREREQS -eq 1 ]
      then
        echo -e "Installing prerequisite package ${PACKAGES_TO_CHECK[idx]}" | tee -a $LOGFILE
        `apt-get -y -q install ${PACKAGES_TO_CHECK[idx]} 1>$LOGFILE` | tee -a $LOGFILE
        if [ ${PIPESTATUS[0]} -ne 0 ]
        then
          RC=$RC_ERROR
        fi
      else
        RC=$RC_ERROR
      fi
      if [ $RC -ne 0 ]
      then
        echo -e "[$(tput setaf 1)ERROR$(tput sgr 0)] $(tput setaf 6)${PACKAGES_TO_CHECK[idx]}$(tput sgr 0) package is not installed." | tee -a $LOGFILE
        LIB_TO_INSTALL=("${LIB_TO_INSTALL[@]}" "$idx")
        RC=$RC_ERROR
      fi
    fi
  done
  if [ $RC -ne $RC_SUCCESS ]
  then
      echo
      while true; do
          read -p "$(tput setaf 3)Do you wish to install dependencies (y/n)? $(tput sgr 0)" yn
          case $yn in
              [Yy]* )
                    echo -e "\n Installing missing dependencies..."
                    echo -e " (Please provide sudo password while needed)\n"
                    for idx in "${LIB_TO_INSTALL[@]}"
                    do
                        `sudo apt-get -y -q install ${PACKAGES_TO_CHECK[$idx]} 1>$LOGFILE` | tee -a $LOGFILE
                         RC=$RC_SUCCESS
                     done
                     break
                     ;;
              [Nn]* )
                    echo -e "\nPlease install all dependencies.\n"
                    RC=$RC_ERROR
                    break
                    ;;
          esac
      done
  else
      echo -e "\n*$(tput setaf 2) All package prerequisites are installed.$(tput sgr 0)\n" | tee -a $LOGFILE
  fi
  return $RC
}

############################################################################
#                                                                          #
#  Function: check_prereqs                                                 #
#  Description: Remove packages if needed and make call to check the       #
#               system for installation of the necessary packages to       #
#               support the ACS test suite.                                #
#                                                                          #
############################################################################
function check_prereqs()
{
  local RC=$RC_SUCCESS

  if [ $REMOVE_PREREQS -eq 1 ]
  then
    remove_packages
  fi
  check_space
  RC=$?
  if [ $RC -ne $RC_SUCCESS ]
  then
    return $RC
  fi
  check_packages
  RC=$?
  if [ $RC -ne $RC_SUCCESS ]
  then
    return $RC
  fi
}

############################################################################
#                                                                          #
#  Function: get_options                                                   #
#  Description: Handle processing of the command-line parameters.          #
#                                                                          #
############################################################################
VALID_OPTION=("ip" "install_prereqs" "rm" "remove_prereqs" "?" "-h" "help")
function get_options()
{
  local OPT

  if [ $# -gt 0 ]
  then
    OPT=$(echo $1|tr [A-Z] [a-z])
    i=0
    while [ "${OPT:$i:1}" = "-" ]
    do
      ((i++))
    done
    if [ "${OPT:$i:2}" = "ip" ] || [ "${OPT:$i:15}" = "install_prereqs" ]
    then
      if [ $EUID -ne 0 ]
      then
        echo -e "Root access required to install prereqs." | tee -a $LOGFILE
        show_usage
        shutdown $RC_ERROR
      else
        INSTALL_PREREQS=1
        echo -e "Prereqs will be installed if needed.\n"
      fi
    elif [ "${OPT:$i:2}" = "rm" ] || [ "${OPT:$i:15}" = "remove_prereqs" ]
    then
      if [ $EUID -ne 0 ]
      then
        echo -e "Root access required to remove prereqs." | tee -a $LOGFILE
        show_usage
        shutdown $RC_ERROR
      else
        REMOVE_PREREQS=1
        echo -e "Prereqs will be removed if already installed. (if allowed by apt-get)\n"
      fi
    elif [ "${OPT:$i:1}" = "?" ] || [ "${OPT:$i:4}" = "help" ] || [ "${OPT:$i:4}" = "h" ]
    then
        show_usage
        shutdown $RC_HELP
    else
        show_usage "${OPT:$i}"
    shutdown $RC_ERROR
    fi
  fi
  return 0
}

############################################################################
#                                                                          #
#  Function: show_usage                                                    #
#  Description: Show the command line options for the script.              #
#                                                                          #
############################################################################
function show_usage()
{
  cat <<-EOF

  Usage: $0: [OPTIONS]

    Supported Options:
      -ip, --install_prereqs   Install missing prereqs to support download
      -rm, --remove_prereqs    Remove any prereqs previously installed
                               [To install / remove, please use this script with 'sudo' privileges]
      -h,  --help              Help

EOF
  return
}


############################################################################
#                                                                          #
#  Function: shutdown                                                      #
#  Description: Handle untrapping trapped signals and output of the final  #
#               return code from the script.                               #
#                                                                          #
############################################################################
function shutdown()
{
  trap '' 1 2 3 15
  if [ $1 -eq $RC_ERROR ]
  then
    echo -e "Dependency checking of prerequisites for the $TEST_SUITE ACS test suite $(tput setaf 1)FAILED$(tput sgr 0).\n" | tee -a $LOGFILE
  elif [ $1 -eq $RC_HELP ]
    then
    echo -e ""
  else
    echo -e "Dependency checking of prerequisites for the $TEST_SUITE ACS test suite was $(tput setaf 2)SUCCESSFUL$(tput sgr 0).\n" | tee -a $LOGFILE
  fi
  echo -e "Dependency checking of prerequisites for the $TEST_SUITE ACS test suite ended at `date`\n" | tee -a $LOGFILE
  echo
  exit $1
}


############################################################################
#                                                                          #
#  Function:     main                                                      #
#  Description:  Entry point for the script.                               #
#                                                                          #
############################################################################

  trap 'shutdown' 1 2 3 15
  RC=0
  if [ ! -f $LOGFILE ]
  then
    /bin/touch $LOGFILE &>/dev/null
  fi
  echo "LOGFILE is $LOGFILE"
  echo -e "\n\nDependency Checking of prerequisites for the $TEST_SUITE started on `date`\n" | tee -a $LOGFILE
  echo

  get_options $@
  RC=$?
  if [ $RC -eq $RC_SUCCESS ]
  then
    check_prereqs
    RC=$?
  fi
  shutdown $RC

#end of main

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

#Globals
LOGFILE="./sync.log"
NEEDED_DRV_SPACE=8000000  #8GB needed.  Value is in 1KB chunks (10=10KB)
CHECK_DEPS=1

#Return codes
RC_ERROR=1
RC_SUCCESS=0
RC_HELP=2

#LuvOS variables
LUVOS_DIR="luv"
DOWNLOAD_LUVOS=1
REMOVE_LUVOS=1
LUVOS_GIT_CLONE="git clone https://github.com/01org/luv-yocto.git"
LUVOS_GIT_CHECKOUT="git checkout c60e8de440c832708f70ac953c801150a4963262"
LUVOS_PATCH_FILE="../patches/luvos.patch"
LUVOS_GIT_APPLY="git apply $LUVOS_PATCH_FILE"

#Dependency Check Script Variables
CHECK_DEPS_SCRIPT_NAME="./check_deps.sh"

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
  echo -n "Available space"
  if [ $AVAIL_SPACE -gt $NEEDED_DRV_SPACE ]
  then
    RC=$RC_SUCCESS
    echo -e " $(tput setaf 2)is available$(tput sgr 0).\n" | tee -a $LOGFILE
  else
    echo -e " $(tput setaf 1)is not available$(tput sgr 0).\n" | tee -a $LOGFILE
    RC=$RC_ERROR
  fi
  return $RC
}

############################################################################
#                                                                          #
#  Function: download_luvos                                                #
#  Description: Perform the necessary commands to download the LuvOS       #
#               source files.  If an error occurs, log it and              #
#               terminate.                                                 #
#                                                                          #
############################################################################
function download_luvos()
{
  PARENT_DIR=$PWD
# make the directory
  echo -e "Download of the LuvOS kernel source files has started." | tee -a $PARENT_DIR/$LOGFILE
  pushd $PWD
  if [ ! -d "$LUVOS_DIR" ]
  then
    mkdir -p $LUVOS_DIR
    RC=$?
    if [ $RC != 0 ]
    then
      echo -e "Unable to create the $LUVOS_DIR directory.  Download terminating." | tee -a $PARENT_DIR/$LOGFILE
      shutdown $RC_ERROR
    fi
  fi
  cd $LUVOS_DIR
  echo -e "Cloning the LuvOS git repo..." | tee -a $PARENT_DIR/$LOGFILE
  $LUVOS_GIT_CLONE .
  echo -e "Checking out the correct branch in the LuvOS repo..." | tee -a $PARENT_DIR/$LOGFILE
  $LUVOS_GIT_CHECKOUT > /dev/null 2>&1
  if [ -e $LUVOS_PATCH_FILE ] && [ -s $LUVOS_PATCH_FILE ]
  then
    echo -e "Applying patches..." | tee -a $PARENT_DIR/$LOGFILE
    $LUVOS_GIT_APPLY
  fi
  echo -e "Download of the LuvOS kernel source files was successful." | tee -a $PARENT_DIR/$LOGFILE
  popd
}

############################################################################
#                                                                          #
#  Function: get_options                                                   #
#  Description: Handle processing of the command-line parameters. If a CLI #
#               parameter is found, set the value (assign "1" to it).      #
#               OPT is used for determing the command (-c, -d, -r) whereas #
#               OPT2 is used to store the directory to work on from among  #
#               the supported options.  Any errors will cause the help to  #
#               be displayed and the script to terminate.                  #
#                                                                          #
############################################################################
function get_options()
{
  local OPT
  local OPT2

#if no command-line parameters have been passed, clean up all directories and
#  download all files
  if [[ $# == 0 ]]
  then
    REMOVE_LUVOS=1
  fi

  while [[ $# > 0 ]]
  do
    OPT="$1"
#    echo 'Option is '$OPT
    case $OPT in
    -r|--rmdirs)
      shift
      OPT2="$1"
      case $OPT2 in
      "all")
           #Set flags to indicate the need to remove all directories
           REMOVE_LUVOS=1
      ;;
      "luvos")
           #Set flag to indicate the need to remove the LuvOS directory
           REMOVE_LUVOS=1
      ;;
      *)
        #If any other command or value is passed, print the help and terminate
        echo "Invalid parameter...."$OPT2
        show_usage
        shutdown $RC_HELP
      ;;
      esac
    ;;
    -c|--checkdeps)
        #Set flag to indicate the checkdeps.sh script should be executed
        #first to ensure the key dependencies are met.
        CHECK_DEPS=1
    ;;
    -d|--download)
      shift
      OPT2="$1"
      case $OPT2 in
      "all")
           #Set the flag to indicate the need to download all source code
           DOWNLOAD_LUVOS=1
      ;;
      "luvos")
           #Set the flag to indicate the need to download the LuvOS kernel
           #source files.
           DOWNLOAD_LUVOS=1
      ;;
      *)
        #If any other command or value is passed, print the help and terminate
        echo "Invalid parameter...."$OPT2
        show_usage
        shutdown $RC_HELP
      ;;
      esac
    ;;
    #Display the help and exit
    -?|-h|--help)
      show_usage
      shutdown $RC_HELP
    ;;
    #Invalid parameter...show the help and exit.
    *)
      echo "Invalid parameter...."$OPT
      show_usage
      shutdown $RC_HELP
    ;;
    esac
    shift
  done
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
    -r, --rmdirs     <all>|<luvos>
                     Clean the directories by removing all files previously
                     downloaded from the repositories
    -c, --chkdeps    Check the dependencies prior to attempting to download
                     any files from the repositories.  This option requires
                     the chkdeps.sh script be located in the same directory
                     as this script.
    -d, --download   <all>|<luvos>

                     Download the files from one or more repos as indicated

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
  if [ "$1" = $RC_ERROR ]
  then
    echo -e "Synchronization for the Architecture Compliance Test Suite $(tput setaf 1)FAILED$(tput sgr 0).\n" | tee -a $LOGFILE
    echo -e "Synchronization for the Architecture Compliance Test Suite ended at `date`\n" | tee -a $LOGFILE
  elif [ "$1" = $RC_HELP ]
    then
    echo -e ""
  else
    echo -e "Synchronization for the Architecture Compliance Test Suite was $(tput setaf 2)SUCCESSFUL$(tput sgr 0).\n" | tee -a $LOGFILE
    echo -e "Synchronization for the Architecture Compliance Test Suite ended at `date`\n" | tee -a $LOGFILE
  fi
  echo
  exit $1
}

############################################################################
#                                                                          #
#  Function: dump_options                                                  #
#  Description: Debug function to dump the command-line parameter values.  #
#               0 indicates the value is not set.  1 indicates the value   #
#               is set.                                                    #
#                                                                          #
############################################################################
function dump_options()
{
  echo -e "The selected directories will be removed if they exist:" | tee -a $LOGFILE
  if [ "$REMOVE_LUVOS" = "1" ]
  then
    echo -e "    - $LUVOS_DIR (LuvOS source files)" | tee -a $LOGFILE
  fi

  echo -e "" | tee -a $LOGFILE
  echo -e "The following directories will be downloaded from open source repos:" | tee -a $LOGFILE
  if [ "$DOWNLOAD_LUVOS" = "1" ]
  then
    echo -e "    - $LUVOS_DIR (LuvOS source files)" | tee -a $LOGFILE
  fi

  if [ "$CHECK_DEPS" = "1" ]
  then
    echo -e ""
    echo -e "Dependency checking is enabled." | tee -a $LOGFILE
  else
    echo -e ""
    echo -e "Dependency checking is NOT enabled." | tee -a $LOGFILE
  fi
}

############################################################################
#                                                                          #
#  Function: remove_single_dir                                             #
#  Description: Remove the directory passed into the function.  If an      #
#               error occurs, exit the script.  If successful, print a     #
#               message indicating success.                                #
#                                                                          #
############################################################################
function remove_single_dir()
{
  local DIR

  DIR="$1"
  echo -e "Removing the $DIR directory." | tee -a $LOGFILE
  rm -rf "$DIR"
  RC=$?
  if [ $RC != 0 ]
  then
    echo -e "Unable to remove the "$DIR" directory.  Directory removal terminating." | tee -a $LOGFILE
    shutdown $RC_ERROR
  else
    echo -e $DIR" directory was successfully removed." | tee -a $LOGFILE
  fi
}

############################################################################
#                                                                          #
#  Function: remove_dirs                                                   #
#  Description: Check if existing directories are to be removed.  If they  #
#               are to be removed, call the remove_single_dir function     #
#               with the name of the directory to remove.                  #
#                                                                          #
############################################################################
function remove_dirs()
{
  echo -e "Removing directories"
#Remove the Linux source files
  if [ $REMOVE_LUVOS = "1"  ] && [ -d "$LUVOS_DIR" ]
  then
    remove_single_dir "$LUVOS_DIR"
  fi
}

############################################################################
#                                                                          #
#  Function: download_files                                                #
#  Description: Function to download the files associated with each        #
#               open source package needed to complete the SW set for the  #
#               corresponding kit or subsystem.                            #
#                                                                          #
############################################################################
function download_files()
{
  echo -e "Downloading files..." | tee -a $LOGFILE
  if [ $DOWNLOAD_LUVOS = "1" ]
  then
    download_luvos
  fi
}

############################################################################
#                                                                          #
#  Function:     main                                                      #
#  Description:  Entry point for the script.                               #
#                                                                          #
############################################################################

  trap 'shutdown' 1 2 3 15  #trap SIGHUP(1), SIGINT(2), SIGQUIT(3) and SIGTERM(15)
  RC=0
  if [ ! -f $LOGFILE ]
  then
    /bin/touch $LOGFILE &>/dev/null
  fi
  echo "LOGFILE is $LOGFILE"

  get_options $@
  RC=$?
  if [ $RC = $RC_SUCCESS ]
  then
    echo
    echo -e "\n\nSynchronization for the Architecture Compliance Test Suite  started on `date`\n" | tee -a $LOGFILE
    echo
    dump_options
    if [ $CHECK_DEPS = "1" ]
    then
      if [ -s "$CHECK_DEPS_SCRIPT_NAME" ]
      then
        $CHECK_DEPS_SCRIPT_NAME
        RC=$?
        if [ $RC != $RC_SUCCESS ]
        then
          echo -e "\nDependencies have not been met to successfully download the software." | tee -a $LOGFILE
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
                  shutdown $RC_ERROR
              fi
           fi
        fi
      else
        echo -e "\nScript file $CHECK_DEPS_SCRIPT_NAME to check dependencies was not found." | tee -a $LOGFILE
        shutdown $RC_ERROR
      fi
    fi
    remove_dirs
    download_files
  fi
  shutdown $RC

#end of main
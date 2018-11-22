
if [ $# -ne 2 ];
then
    echo "Give ACS_PATH and EDK2_PATH as the arguments to the script"
    return 0
fi

export ACS_PATH=$1
echo -e "ACS_PATH is set to -> \e[93m $ACS_PATH\e[0m"
export EDK2_PATH=$2
echo -e "EDK2_PATH is set to -> \e[93m$EDK2_PATH\e[0m"

ln -s  $ACS_PATH/ $EDK2_PATH/AppPkg/Applications/mpam


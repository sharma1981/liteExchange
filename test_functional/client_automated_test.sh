#!/bin/bash
SERVER=$1
TEST_CASES_FILE=$2
NUMBER_OF_CLIENTS=$3
TEMPLATE_FILE="template.cfg"
CLIENT_DIRECTORY="./client_executable"
CLIENTS=

#Sudoable test
sudo ifconfig
initrv=$?
clear

function display_usage()
{
    echo ""
    echo "chmod +x ./client_automated_test.sh"
    echo "./client_automated_test.sh"
    echo ""
}

function run_client()
{
    local client_name=$1
    ./client $TEMPLATE_FILE $SERVER $client_name ../$TEST_CASES_FILE
}

if [[ $initrv > 0 ]]
then
    echo "Current user needs to have root rights in order to execute OME"
    display_usage
    exit 1
fi

echo ""
echo "Client automation is starting :"
echo ""

cd $CLIENT_DIRECTORY

start=$(($(date +%s%N)/1000000))

i=1
while [ "$i" -le "$NUMBER_OF_CLIENTS" ]; do
  CLIENTS[$i]="TEST_CLIENT$i"
  i=$(($i + 1))
done

for i in "${CLIENTS[@]}"
do
    echo "Starting $i"
    run_client $i &
done

#Wait for all forked child processes
wait

finish=$(($(date +%s%N)/1000000))
current_execution_time=$(($finish-$start))
echo ""
echo "Time : $current_execution_time milliseconds "

echo ""
echo "Client automation finished."
echo ""
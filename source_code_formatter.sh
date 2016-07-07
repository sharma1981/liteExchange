#/bin/bash
SED_ARGUMENT="-i" # -i.orig in order to save original files

IS_COMMAND_VALID_RESULT=0
function is_command_valid()
{
    local command_name=$1
    local which_command=`which ${command_name}`
    local valid_command=${#which_command}
    
    if [ $valid_command -eq 0 ]; then
        IS_COMMAND_VALID_RESULT=0   
        return
    fi
    IS_COMMAND_VALID_RESULT=1
}

function check_dos2unix()
{
    is_command_valid "dos2unix"
    if [ $IS_COMMAND_VALID_RESULT -eq 0 ]; then
        echo  "dos2unix is not available"
        exit -1
    else
        echo  "dos2unix is available"
    fi
}

function convert_end_of_lines()
{
    find . -name \*.cpp -o -name \*.hpp -o -name \*.h | xargs dos2unix
}

function convert_tabs_to_spaces()
{
    find . -name \*.cpp -o -name \*.hpp -o -name \*.h | xargs sed ${SED_ARGUMENT} $'s/\t/    /g'
}

function remove_trailing_whitespace()
{
	find . -name \*.cpp -o -name \*.hpp -o -name \*.h | xargs sed  ${SED_ARGUMENT} '' -e's/[ \t]*$//'
}

check_dos2unix
echo "Formatting started\n"
convert_end_of_lines
convert_tabs_to_spaces
remove_trailing_whitespace
echo "Formatting completed\n"
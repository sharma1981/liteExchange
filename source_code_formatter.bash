#/bin/bash
SAVE_ORIGINAL_FILE=0

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

function install_dos2unix_if_necessary()
{
    is_command_valid "dos2unix"
    if [ $IS_COMMAND_VALID_RESULT -eq 0 ]; then
        echo  "dos2unix is not available"
        sudo yum install dos2unix
    else
        echo  "dos2unix is available"
    fi
}

function convert_end_of_lines()
{
    find . -name \*.cpp -o -name \*.c -o -name \*.h | xargs dos2unix
}

function convert_tabs_to_spaces()
{
    if [ $SAVE_ORIGINAL_FILE -eq 1 ]; then
        find . -name \*.cpp -o -name \*.c -o -name \*.h | xargs sed -i $'s/\t/    /g'
    else
        find . -name \*.cpp -o -name \*.c -o -name \*.h | xargs sed -i.orig $'s/\t/    /g'
    fi  
}

function remove_trailing_whitespace()
{
    if [ $SAVE_ORIGINAL_FILE -eq 1 ]; then
        find . -name \*.cpp -o -name \*.c -o -name \*.h | xargs sed -i '' -e's/[ \t]*$//'
    else
        find . -name \*.cpp -o -name \*.c -o -name \*.h | xargs sed -i.orig '' -e's/[ \t]*$//'
    fi  
}

install_dos2unix_if_necessary
convert_end_of_lines
convert_tabs_to_spaces
remove_trailing_whitespace
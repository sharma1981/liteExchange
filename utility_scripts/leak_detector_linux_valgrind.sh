#/bin/bash
DEBUGGEE_DIR=../bin/
DEBUGGEE=./ome
cd $DEBUGGEE_DIR
valgrind --leak-check=yes $DEBUGGEE
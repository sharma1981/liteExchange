#!/usr/bin/wish
#
# To install tcl and tk on CentOS :
#
#   yum install tcl
#	yum install tk
set width 400
set height 250
set x [expr { ( [winfo vrootwidth  .] - $width  ) / 2 }]
set y [expr { ( [winfo vrootheight .] - $height ) / 2 }]
set server_address "127.0.0.1"
set test_case_file "test_cases.txt"
set number_of_test_clients "8"
wm title . "OME Functional Test"
wm geometry . ${width}x${height}+${x}+${y}
grid [label .labelServerAddress -text "Server address" ]
place .labelServerAddress -x 50 -y 50 
grid [entry .textboxServerAddress -textvariable server_address] 
place .textboxServerAddress -x 175 -y 50 
grid [label .labelTestCaseFile -text "Test case file" ]
place .labelTestCaseFile -x 50 -y 100 
grid [entry .textboxTestCaseFile -textvariable test_case_file] 
place .textboxTestCaseFile -x 175 -y 100 
grid [label .labelNumberOfClients -text "Number of clients" ]
place .labelNumberOfClients -x 50 -y 150 
grid [entry .textboxNumberOfClients -textvariable number_of_test_clients]
place .textboxNumberOfClients -x 175 -y 150 
grid [button .startButton  -text "Start" -command "start_button"]
place .startButton -x 150 -y 200

proc start_button {} {
	set current_server_address [.textboxServerAddress get]
	puts $current_server_address
	set current_test_case_file [.textboxTestCaseFile get]
	puts $current_test_case_file
	set current_number_of_clients [.textboxNumberOfClients get]
	puts $current_number_of_clients
	set ret_val [catch { exec /bin/bash ./client_automated_test.sh $current_server_address $current_test_case_file $current_number_of_clients >&@stdout }]
	puts $ret_val
}
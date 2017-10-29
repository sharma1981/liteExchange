<td><img src="https://img.shields.io/badge/LICENCE-PUBLIC%20DOMAIN-green.svg" alt="Licence badge"></td>

[![Build status](https://ci.appveyor.com/api/projects/status/hw8s5o46kcgr388l?svg=true)](https://ci.appveyor.com/project/akhin/cpp-multithreaded-order-matching-engine)

* Sections
	* [1. Introduction and features](#Introduction)
    * [2. Overview of multithreading system](#Overview)
    * [3. Build dependencies](#BuildDependencies)
	* [4. Runtime dependencies](#RuntimeDependencies)
	* [5. How to build](#HowToBuild)
	* [6. Server parameters and running the server](#ServerParams)
	* [7. Offline order entry mode](#OfflineOrderEntry)
	* [8. Example log messages with FIX ](#ExampleLog)
	* [9. Functional testing](#FunctionalTesting)
	* [10. Unit testing with GoogleTest](#UnitTesting)
	* [11. Utility scripts](#UtilityScripts)
	* [12. Coding and other guidelines](#CodingGuideline)
	* [13. Continous integration](#ContinousIntegration)
          
## <a name="Introduction"></a>**1. Introduction:** 
A multithreaded order matching engine written in C++11 using FIX protocol. For limit orders and matching engines , see https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/documentation/README_Orders_MatchingEngines.md

For FIX protocol , see https://en.wikipedia.org/wiki/Financial_Information_eXchange

Features can be seen in the table below :

| Feature                       | Details                                               |
| ----------------------------- |:-----------------------------------------------------:|
| Order entry					| FIX 4.2 & offline mode with files  			        |
| Order types                   | Limit                                                 |
| Order message types           | NewOrder, Cancel                                      |
| Exec report types			    | Accepted, Filled, PartiallyFilled, Rejected, Canceled |
| TIF                           | Not supported       			                        |
| Securities                    | Shares with RIC code                                  |

Technical implementation details are as below : 

| Feature               | Details                                                       |
| ----------------------|:-------------------------------------------------------------:|
| OS                    | Windows ( tested on 8.1), Linux ( tested on Ubuntu and CentOS)|
| C++                   | C++11                                                         |
| C++ Compiler Support  | GCC4.8 and MSVC120 (VS2013)									|
| C++ Libraries         | STD, STL, QuickFix			                                |
| C++ Platform APIs     | GNU LibC, POSIX, some POSIX NP ,WinAPI, MS CRT                |
| IDEs supported        | Netbeans for Linux, VS2013 for Windows 						|

Watch server when working :

<a href="https://asciinema.org/a/929cs0jcbso3g7v7b3cd6g82f" target="_blank"><img src="https://asciinema.org/a/929cs0jcbso3g7v7b3cd6g82f.png" width="589"/></a>


Client test automation when working :

<a href="https://asciinema.org/a/29km7ksm8ylrne24zkv9ripxu" target="_blank"><img src="https://asciinema.org/a/29km7ksm8ylrne24zkv9ripxu.png" width="589"/></a>
                        

## <a name="Overview">**2. Overview of multithreading system:** 

The engine currently is using :

* A thread class which you can set stack size, set OS priority and set names for debugging
* A thread pool with ability to pin threads to CPU cores and avoid hyperthreading
* 1 lock free SPSC ring buffer
* Other fine grained lock based ring buffer and queues
* Actor pattern
* Also the engine currently makes use of a set of CPU cache aligned allocators for memory allocations in order to avoid false sharing : https://github.com/akhin/cpp_multithreaded_order_matching_engine/tree/master/source/core/memory

For detailed low level implementation details please see https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/documentation/README_MultithreadingImplementationDetails.md

The core of order matching layer is called a central order book, which keeps order books per security symbol. Each order book has a table for bids and another table for asks.

1. The FIX engine will listen for session requests from the clients, and if a session is established then it listens for incoming ask/bid orders. If the order type is not supported, it sends “rejected” message. Otherwise the message will be submitted to an incoming message dispatcher which is a fine grained MPSC unbounded queue. ( The main reason being for MPSC here is the FIX engine being used is multithreaded and the worker queues in the thread pool are SPSC by design ) The incoming message dispatcher will submit messages to the central order book.

2. Central Order book has a thread pool :

	* The thread pool will have N lockfree SPSC queues for N threads ( N = num of symbol ). The thread pool  also has the ability to pin threads to CPU cores. And based on configuration it can also avoid hyperthreading/logical processors by pinning threads only to CPU cores with even indexes.

	* Central Order book also has 1 MPMC queue for outgoing messages.

	* When a new message arrives ( new order, or cancel ) from the incoming message dispatcher, it will be submitted to corresponding thread`s queue in the thread pool of the central order book.

3. Each thread in the thread pool will get message from their SPSC queue in the thread pool , and add them to corresponding order queue which is used by only itself and eventually trigger the order matching process for that queue. At the end of the order matching , worker threads will submit messages ( FILLED or PARTIALLY FILLED ) to the outgoing messages queue.

4. Outgoing message processor which is a fine grained MPMC queue will process the outgoing messages and send responses back to the clients.

## <a name="BuildDependencies">**3. Build dependencies:** 

For Linux , the project is built and tested with GCC4.8 only on CentOS7. 

As for Windows it is using MSVC1200(VS2013). An important note about VS2013 , its version shouldn`t be later then Update2 as the project is using C++11 curly brace initialisation in MILs and MSVC rollbacked that feature starting from Update3 :

https://connect.microsoft.com/VisualStudio/feedbackdetail/view/938122/list-initialization-inside-member-initializer-list-or-non-static-data-member-initializer-is-not-implemented

In the libraries side :

- QuickFix & its requirements : For Windows you don`t need to do anything as the static library for Windows is already in dependencies directory. For Linux you need to apply the steps on http://www.quickfixengine.org/quickfix/doc/html/install.html

## <a name="RuntimeDependencies">**4. Runtime dependencies:** 

For Windows, you need MSVC120 runtime. You can find them in the same directory with executables : bin

For Linux, you need GNU Libstd C++ 6 runtime and QuickFIX runtime.

How to install Quickfix runtime on Linux ( tested on Ubuntu ) :

        1. Navigate to dependencies/quickfix/linux_runtime
        2. sudo chmod +x ./install_quickfix_runtime.sh
        3. sudo ./install_quickfix_runtime.sh
        
Note : This script will copy shared object to library path, create soft links, will add library path to /etc/ld.so.conf and finally execute ldconfig.

## <a name="HowToBuild">**5. How to build:**
            
How to build the project on Linux :
    
    cd build/linux_gcc
    make clean
    make debug  OR make release
    or if you want to make a parallel build :
    ./build_in_parallel.sh release

How to build the project on Linux using Netbeans 8.0.2 C++ IDE:

    Open Netbeans.
    Open the project from the project directory. ( Choose "nbproject" directory )
    Build the project inside Netbeans IDE.

Why Netbeans : In Netbeans, it is too straightforward to setup remote debugging, therefore it is quite convenient to build and debug on Linux from Windows via SSH and Samba. You can see an article about this setup here in my blog. It is for Debian but it should be trivial to apply it to any other distribution : https://nativecoding.wordpress.com/2014/10/24/configuring-a-debian-virtual-machine-for-linux-c-development-via-windows-step-by-step/
    
How to build the project on Windows with Visual Studio  :
    
    You can build with Visual Studio 2013
    Go to "build/windows_msvc_visual_studio" directory
    Use SLN file to launch VS with the project
    
How to build the project on Windows with Visual Studio in command line :
    
    You can build with Visual Studio 2013
    Go to "build/windows_msvc_command_line" directory
    Execute one of batch files : build_debug.bat or build_release.bat

## <a name="ServerParams">**6. Server parameters and running the server :** 

The engine executable looks for "ome.ini" file. Here is the list of things you can set :

| Ini setting           							| Functionality                                                 |
| --------------------------------------------------|:-------------------------------------------------------------:|
| LOGGER_FILE_SINK      							| Enables/disables logging										|
| LOGGER_CONSOLE_SINK   							| Enables/disables output to stdout                             |
| LOGGER_BUFFER_SIZE 								| Maximum buffer size for the logging ring buffer				|
| SINGLE_INSTANCE_TCP_PORT  						| Port used in single instance check , applies to Linux only    |
| PROCESS_PRIORITY         							| Sets OS level priority of the main process                    |
| CENTRAL_ORDER_BOOK_MULTITHREADED_ORDER_MATCHING	| Toggles multithreading for order matching                     |
| CENTRAL_ORDER_BOOK_PIN_THREADS_TO_CORES	        | Whether to pin threads of the threadpool to CPU cores			|
| CENTRAL_ORDER_BOOK_HYPER_THREADING         		| If hyperthreading is off it will use cores with an even index |
| CENTRAL_ORDER_BOOK_WORK_QUEUE_SIZE_PER_THREAD     | Queue size per worker thread in the thread pool               |
| CENTRAL_ORDER_BOOK_THREAD_PRIORITY         		| OS-level priority of thread pool threads                      |
| CENTRAL_ORDER_BOOK_THREAD_STACK_SIZE         		| Stack size for thread pool threads                            |
| OFFLINE_ORDER_ENTRY_FILE							| Offline order entry mode see below for details 							|
        
You will also need to specify security symbols. The order matching engine`s thread pool will create a worker thread for each symbol.
For specifying symbols in ini file, you need to use brackets as  below :

        SYMBOL[]=MSFT
        SYMBOL[]=AAPL
        SYMBOL[]=INTC
        SYMBOL[]=GOOGL
        
You will also need to have "quickfix_FIX42.xml" and "quickfix_server.cfg" files to be in the same directory with OME executable. You can find them in "bin" directory.

Once you start the ome executable , initially you will see a screen like this :

        06-02-2016 16:22:00 : INFO , Main thread , starting
        06-02-2016 16:22:00 : INFO , Thread pool , Thread(0) MSFT starting
        06-02-2016 16:22:00 : INFO , Thread pool , Thread(1) AAPL starting
        06-02-2016 16:22:00 : INFO , Thread pool , Thread(2) INTC starting
        06-02-2016 16:22:00 : INFO , Thread pool , Thread(3) GOOGL starting
        06-02-2016 16:22:00 : INFO , Thread pool , Thread(4) QCOM starting
        06-02-2016 16:22:00 : INFO , Thread pool , Thread(5) QQQ starting
        06-02-2016 16:22:00 : INFO , Thread pool , Thread(6) BBRY starting
        06-02-2016 16:22:00 : INFO , Thread pool , Thread(7) SIRI starting
        06-02-2016 16:22:00 : INFO , Thread pool , Thread(8) ZNGA starting
        06-02-2016 16:22:00 : INFO , Thread pool , Thread(9) ARCP starting
        06-02-2016 16:22:00 : INFO , Thread pool , Thread(10) XIV starting
        06-02-2016 16:22:00 : INFO , Thread pool , Thread(11) FOXA starting
        06-02-2016 16:22:00 : INFO , Thread pool , Thread(12) TVIX starting
        06-02-2016 16:22:00 : INFO , Thread pool , Thread(13) YHOO starting
        06-02-2016 16:22:00 : INFO , Thread pool , Thread(14) HBAN starting
        06-02-2016 16:22:00 : INFO , Thread pool , Thread(15) BARC starting
        06-02-2016 16:22:00 : INFO , Incoming message dispatcher , Thread starting
        06-02-2016 16:22:00 : INFO , Outgoing message processor , Thread starting
        06-02-2016 16:22:00 : INFO , FIX Engine , Acceptor started

        Available commands :

                display : Shows all order books in the central order book
                quit : Shutdowns the server

## <a name="OfflineOrderEntry">**7. Offline order entry mode:** 

The default mode is FIX server mode. However, if you specify an order file in ome.ini as below :

					OFFLINE_ORDER_ENTRY_FILE=orders.txt
					
the order matcher will process all the orders in that file bypassing FIX protocol and produce offline_order_entry_results.txt as result file.
For an example offlone order file see :

https://github.com/akhin/multithreaded_order_matching_engine/blob/master/bin/sample_offline_order_file.txt


The output of offline order matching will have timestamps with microsecond precision and FIX execution report messages as below :


		16-09-2017 03:12:27.739729
		8=FIX.4.29=6835=86=011=14=017=120=037=38=239=054=255=GOOGL150=0151=210=001
		16-09-2017 03:12:27.755355
		8=FIX.4.29=6835=86=011=14=017=220=037=38=139=054=155=GOOGL150=0151=110=255
		16-09-2017 03:12:27.773486
		8=FIX.4.29=7835=86=111=14=117=320=031=132=137=38=139=254=155=GOOGL150=2151=010=173
		16-09-2017 03:12:27.777489
			

## <a name="ExampleLog">**8. Example log message from the engine:** 

The engine produces log messages below when it receives 1 buy order with quantity 1 and 1 sell order with quantity 1 for the same symbol :

    06-02-2016 20:16:09 : INFO , FIX Engine , New logon , session ID : FIX.4.2:OME->TEST_CLIENT1
    06-02-2016 20:16:09 : INFO , FIX Engine , Sending fix message : 8=FIX.4.29=15435=834=543=Y49=OME52=20160206-20:16:09.29556=TEST_CLIENT1122=20160206-20:15:03.9556=011=414=017=1820=037=438=139=054=155=MSFT150=0151=110=000
    06-02-2016 20:16:09 : INFO , FIX Engine , Sending fix message : 8=FIX.4.29=15535=834=643=Y49=OME52=20160206-20:16:09.29556=TEST_CLIENT1122=20160206-20:15:03.9556=011=514=017=1920=037=538=239=054=255=GOOGL150=0151=210=070
    06-02-2016 20:16:09 : INFO , FIX Engine , Sending fix message : 8=FIX.4.29=15535=834=743=Y49=OME52=20160206-20:16:09.29556=TEST_CLIENT1122=20160206-20:15:03.9716=011=614=017=2020=037=638=139=054=155=GOOGL150=0151=110=060
    06-02-2016 20:16:09 : INFO , FIX Engine , Sending fix message : 8=FIX.4.29=16435=834=843=Y49=OME52=20160206-20:16:09.29556=TEST_CLIENT1122=20160206-20:15:03.9716=111=414=117=2120=031=132=137=438=139=254=155=MSFT150=2151=010=168
    06-02-2016 20:16:09 : INFO , FIX Engine , Sending fix message : 8=FIX.4.29=16435=834=943=Y49=OME52=20160206-20:16:09.29556=TEST_CLIENT1122=20160206-20:15:03.9716=111=314=117=2220=031=132=137=338=139=254=255=MSFT150=2151=010=169
    06-02-2016 20:16:09 : INFO , FIX Engine , Sending fix message : 8=FIX.4.29=15635=834=1043=Y49=OME52=20160206-20:16:09.29556=TEST_CLIENT1122=20160206-20:15:03.9716=011=714=017=2320=037=738=139=054=155=GOOGL150=0151=110=108
    06-02-2016 20:16:09 : INFO , FIX Engine , Sending fix message : 8=FIX.4.29=15435=834=1143=Y49=OME52=20160206-20:16:09.29556=TEST_CLIENT1122=20160206-20:15:03.9716=011=814=017=2420=037=838=139=854=155=xxx150=8151=110=110
    06-02-2016 20:16:09 : INFO , FIX Engine , Sending fix message : 8=FIX.4.29=16635=834=1243=Y49=OME52=20160206-20:16:09.29556=TEST_CLIENT1122=20160206-20:15:03.9716=111=614=117=2520=031=132=137=638=139=254=155=GOOGL150=2151=010=027
    06-02-2016 20:16:09 : INFO , FIX Engine , Sending fix message : 8=FIX.4.29=16635=834=1343=Y49=OME52=20160206-20:16:09.29556=TEST_CLIENT1122=20160206-20:15:03.9716=111=514=117=2620=031=132=137=538=239=154=255=GOOGL150=1151=110=028
    06-02-2016 20:16:09 : INFO , FIX Engine , Sending fix message : 8=FIX.4.29=16635=834=1443=Y49=OME52=20160206-20:16:09.29556=TEST_CLIENT1122=20160206-20:15:03.9716=111=714=117=2720=031=132=137=738=139=254=155=GOOGL150=2151=010=033
    06-02-2016 20:16:09 : INFO , FIX Engine , Sending fix message : 8=FIX.4.29=16635=834=1543=Y49=OME52=20160206-20:16:09.29556=TEST_CLIENT1122=20160206-20:15:03.9716=111=514=217=2820=031=132=137=538=239=254=255=GOOGL150=2151=010=034
    06-02-2016 20:16:09 : INFO , FIX Engine , Receiving fix message : 8=FIX.4.29=12335=D34=1549=TEST_CLIENT152=20160206-20:16:09.34256=OME11=121=138=140=244=154=255=MSFT59=060=20160206-20:16:0910=124
    06-02-2016 20:16:09 : INFO , FIX Engine , New order message received :8=FIX.4.29=12335=D34=1549=TEST_CLIENT152=20160206-20:16:09.34256=OME11=121=138=140=244=154=255=MSFT59=060=20160206-20:16:0910=124
    06-02-2016 20:16:09 : INFO , Central Order Book , New order accepted, client TEST_CLIENT1, client order ID 1 
    06-02-2016 20:16:09 : INFO , Outgoing message processor , Processing ACCEPTED for order : Client TEST_CLIENT1 Client ID 1 Symbol MSFT Side SELL 
    06-02-2016 20:16:09 : INFO , Thread pool , MSFT thread got a new task to execute
    06-02-2016 20:16:09 : INFO , FIX Engine , Sending fix message : 8=FIX.4.29=12535=834=2349=OME52=20160206-20:16:09.34256=TEST_CLIENT16=011=114=017=11320=037=138=139=054=255=MSFT150=0151=110=079
    06-02-2016 20:16:09 : INFO , Central Order Book , Order processing for symbol MSFT took 0000000 milliseconds , num of processed orders : 0
    06-02-2016 20:16:09 : INFO , FIX Engine , Receiving fix message : 8=FIX.4.29=12335=D34=1649=TEST_CLIENT152=20160206-20:16:09.34256=OME11=221=138=140=244=154=155=MSFT59=060=20160206-20:16:0910=125
    06-02-2016 20:16:09 : INFO , FIX Engine , New order message received :8=FIX.4.29=12335=D34=1649=TEST_CLIENT152=20160206-20:16:09.34256=OME11=221=138=140=244=154=155=MSFT59=060=20160206-20:16:0910=125
    06-02-2016 20:16:09 : INFO , Central Order Book , New order accepted, client TEST_CLIENT1, client order ID 2 
    06-02-2016 20:16:09 : INFO , Outgoing message processor , Processing ACCEPTED for order : Client TEST_CLIENT1 Client ID 2 Symbol MSFT Side BUY 
    06-02-2016 20:16:09 : INFO , Thread pool , MSFT thread got a new task to execute
    06-02-2016 20:16:09 : INFO , FIX Engine , Sending fix message : 8=FIX.4.29=12535=834=2449=OME52=20160206-20:16:09.34256=TEST_CLIENT16=011=214=017=11420=037=238=139=054=155=MSFT150=0151=110=082
    06-02-2016 20:16:09 : INFO , Central Order Book , Order processing for symbol MSFT took 0000000 milliseconds , num of processed orders : 2
    06-02-2016 20:16:09 : INFO , Outgoing message processor , Processing FILLED for order : Client TEST_CLIENT1 Client ID 2 Symbol MSFT Side BUY 
    06-02-2016 20:16:09 : INFO , FIX Engine , Sending fix message : 8=FIX.4.29=13535=834=2549=OME52=20160206-20:16:09.34256=TEST_CLIENT16=111=214=117=11520=031=132=137=238=139=254=155=MSFT150=2151=010=001
    06-02-2016 20:16:09 : INFO , Outgoing message processor , Processing FILLED for order : Client TEST_CLIENT1 Client ID 1 Symbol MSFT Side SELL 
    06-02-2016 20:16:09 : INFO , FIX Engine , Sending fix message : 8=FIX.4.29=13535=834=2649=OME52=20160206-20:16:09.34256=TEST_CLIENT16=111=114=117=11620=031=132=137=138=139=254=255=MSFT150=2151=010=002
    06-02-2016 20:16:11 : INFO , FIX Engine , Logout , session ID : FIX.4.2:OME->TEST_CLIENT1

## <a name="FunctionalTesting">**9. Functional testing:** 

There is a Powershell script for Windows which can send orders from FIX files.
		   
Under "test_functional" directory :
   
For Windows

1. Modify test_data.txt which has the orders to send to the engine as you wish.

2. For firing Windows test client(s), you can use fix_client_automation.bat.

3. A GUI will appear. Simply specify target server , number of test clients and the test case file :

4. Press the start button.
        
<p align="center">  
<img src="https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/documentation/testfunctional_gui_windows.png">       
</p>
        
For Linux, the same automation will be available soon in Perl and Python.
    
## <a name="UnitTesting">**10. Unit testing with GoogleTest:** 

The project uses GoogleTest 1.7. You can find a makefile and vcproj under "test_unit" directory.
    
Building and running unit test on Linux : You have to build and install Google Test 1.7 first , the instructions for CentOS and Ubuntu :
            
            $ wget http://googletest.googlecode.com/files/gtest-1.7.0.zip
            $ unzip gtest-1.7.0.zip
            $ cd gtest-1.7.0
            $ ./configure
            $ make
            $ sudo cp -a include/gtest /usr/include
            
            On CentOS
            $ sudo cp -a lib/.libs/* /usr/lib64
            
            On Ubuntu
            $ sudo cp -a lib/.libs/* /usr/lib/
            
            Then you can either use Makefile or Netbeans project files under "test_unit" directory.

Building and running unit test on Windows : You can use VisualStudio solution in "test_unit" directory.

## <a name="UtilityScripts">**11. Utility scripts:**

You can find them under "utility_scripts" directory :


| Script		           			| Functionality		                                                 			|
| ----------------------------------|:-----------------------------------------------------------------------------:|
| source_code_formatter.sh 			| converts EOL to Unix, converts tabs to 4 spaces, removes trailing white space	|
| leak_detector_valgrind.sh			| memory leak detection with Valgrind                             				|
| build_with_thread_sanitizer.sh	| build with GCC thread sanitizer												|
| build_with_address_sanitizer.sh	| build with GCC address sanitizer    											|
| build_for_gprof.sh       			| build for gprof profiling                    									|
| profile_with_gprof.sh				| profile with gprof                     										|
| profiler_windows_visual_studio	| profile with Visual Studio`s vsperf											|
| leak_detector_drmemory.bat		| memory leak detection with Dr.Memory 											|
| tcpdump_capture.sh				| gets a tcpdump capture that you can view with Wireshark               		|
    
Viewing tcpdump captures with Wireshark :

<p align="center">      
<img src="https://github.com/akhin/cpp_multithreaded_order_matching_engine/blob/master/documentation/wireshark_fix.png">
</p>

## <a name="CodingGuideline">**12. Coding and other guidelines:**

Source code and file/directory naming conventions :

| Category		           		| Naming convention	                               	|
| ------------------------------|:-------------------------------------------------:|
| Directory names 				| lower_case_word									|
| File names					| lower_case_word                             		|
| Include guards				| _HEADER_H_										|
| Macros						| UPPER_CASE_WORD    								|
| Enums       					| UPPER_CASE_WORD                    				|
| Namespace names				| lower_case_word                     				|
| Class names					| CamelCase											|
| Method names					| pascalCase 										|
| Variable names				| pascalCase               							|
| Member variables starts with	| m_               									|
| Directories/Namespaces		| As in Boost, there is a namespace per directory	|


Source code indentations and new line usage :
    
	- 4 spaces, no tabs ( This needs to be setup in VS )
    - By default Netbeans editor uses spaces for tabs
    - Needs to be set in VS2013 : https://msdn.microsoft.com/en-gb/library/ms165330(v=vs.90).aspx
    - New lines : Unix CR only ( \n ) , VisualStudio can handle it even though Windows is \r\n
    
utility/source_code_formatter.sh : It is a Bash script that scans all cpp,h,hpp files in project directory and converts Windows end of lines to Linux, converts tabs to 4 spaces and removes trailing whitespace. It requires dos2unix.
    
Inclusions : Using forward slash as it works for both Linux and Windows :

~~~C++
		#include <core/concurrency/thread.h>
~~~
    
Warning level used for GCC : -Wall

Warning level used for MSVC : /W3
        
Precompiled header file usage : On Windows , the project is using /FI ( Force include parameter, therefore no need to include the pch header everywhere ) and specified the pch header to be precompiled_header.h. Note that this breaks edit-and-continue in Visual Studio. Also precompiled header setting of all projects is "use" whereas precompiled_header.cpp is "create".
For Linux , there is pch rule to enable it in the makefile ( build/linux/Makefile) , but currently that rule is not being used since it doesn`t appear as it is doing much improvement as on Windows.

For GCC see https://gcc.gnu.org/onlinedocs/gcc/Precompiled-Headers.html

For MSVC 120 see https://msdn.microsoft.com/en-us/library/8c5ztk84(v=vs.120).aspx

MSVC120 C++11 Limitations : Curly brace initialisation at MILs and noexcept is not supported. For noexcept usage please see compiler_portability/noexcept.h .

## <a name="ContinousIntegration">**13. Continous integration:**

For the time being , online CI has been setup for only MSVC using Appveyor. Planning to add GCC based online CI with TravisCI.
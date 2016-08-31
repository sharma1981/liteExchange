@echo off
cls
Color 0E
SET OUTPUT_FILE=ome.vsp
SET TARGET_EXECUTABLE=ome.exe
REM Change your drive letter accordingly
SET DRIVE_LETTER=D
SET PROFILER=vsperf
REM
REM MSDN Link for using vsperf from command line : https://msdn.microsoft.com/en-us/library/hh977161.aspx
REM MSDN Link for using vs perf cmd from command line : https://msdn.microsoft.com/en-us/library/dd255377.aspx
REM 
REM For VS 2013 commnad prompt
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64
REM For VS 2015 command prompt
REM call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86_amd64
REM
REM MSDN Link that shows how to install profiling tools standalone : https://msdn.microsoft.com/en-us/library/bb385771.aspx
REM 
cd /d %DRIVE_LETTER%:
cd %~dp0
SET TARGET_EXECUTABLE_DIRECTORY=%~dp0..\..\bin
echo %TARGET_EXECUTABLE_DIRECTORY%
REM
REM By default, performance data is sampled every 10,000,000 non-halted processor clock cycles.
REM This is approximately one time every 10 seconds on a 1GHz processor
REM
%PROFILER% /start:sample /file:%OUTPUT_FILE%
%PROFILER% /launch:%TARGET_EXECUTABLE_DIRECTORY%\%TARGET_EXECUTABLE% /console
echo
echo This script and profiler will stop when the target app finishes the execution
echo
@echo off
cls
Color 0E
SET SOLUTION=.\ome.sln
REM For VS 2013 commnad prompt
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64
REM For VS 2015 command prompt
REM call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86_amd64
REM
REM Arrange working directory
cd %~dp0
cd ..
cd windows
CD
msbuild %SOLUTION% /t:Clean /p:Configuration=Release /property:Platform=Win32
pause
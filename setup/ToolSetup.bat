@echo off

REM -- Set paths to external tools to upgrade versions easily. --
REM -- Build tools are build platform sepcific. unix paths / tools are in the shell script --
REM -- No trailing back slash should be used. Paths should also not be quoted here  --
REM -- links are provided for each tool. put them wherever, but I work with them in a
REM -- tools folder.
rem ---------------------------------------------------------------------------
rem obviously required. 
rem Link: https://cmake.org/files/v3.5/cmake-3.5.1-win32-x86.zip
set cmakepath=%proj_root%\tools\cmake-3.5.0-win32-x86\bin

rem only required if you want to build mingw
rem Link: http://sourceforge.net/projects/tdm-gcc/files/TDM-GCC%20Installer/tdm-gcc-5.1.0-3.exe/download
set gccpath=%proj_root%\tools\tdm-gcc-5.1.0-2\bin

rem Custom tool used to parallelize
rem other gitlab project.
set runnerpath=%proj_root%\tools\runner-2.0.1

rem ---------------------------------------------------------------------------
rem Needed only for doxygen target. can ignore otherwise.
rem link: http://ftp.stack.nl/pub/users/dimitri/doxygen-1.8.11.windows.x64.bin.zip
set doxypath=%proj_root%\tools\doxygen-1.8.11
rem link: http://gnuwin32.sourceforge.net/downlinks/gawk-bin-zip.php
set gawkpath=%proj_root%\tools\gawk-3.1.6-1\bin
rem link: http://go.microsoft.com/fwlink/?LinkId=14188
set hhcpath=%proj_root%\tools\HTML Help Workshop
rem link: http://www.graphviz.org/pub/graphviz/stable/windows/graphviz-2.38.zip
set dotpath=%proj_root%\tools\graphviz-2.38\bin

rem ---------------------------------------------------------------------------
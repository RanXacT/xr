@echo off
@SETLOCAL 
rem ---------------------------------------------------------------------------
REM set som variables at the top for convenience.
set proj_root=%~dp0
set proj_root=%proj_root:~0,-1%
set test_root=%proj_root%\test

set CMAKE_DEBUG=
set STARTING_FOLDER="%cd%"
rem ===========================================================================
rem ===========================================================================
pushd %proj_root%

call setup\ToolSetup.bat
REM Comment this to get some trace statements from CMAKE useful to track down issues.
REM set CMAKE_DEBUG=--debug-output

rem ===========================================================================
rem ===========================================================================
REM look for args (don't want to litter root folder with separate bat files.)
if "%1"=="" goto printargs
if "%1"=="setup" goto setup
if "%1"=="gen" goto gen
if "%1"=="build" goto build
if "%1"=="clean" goto clean
if "%1"=="test" goto test
if "%1"=="docs" goto docs
goto printargs
rem ---------------------------------------------------------------------------
:setup
rem --------------------------------------------------------------------------- 
REM if exist build rmdir build /S /Q
REM if exist build goto permissionerror
mkdir build
pushd build



IF NOT DEFINED VS140COMNTOOLS goto VS14_DONE

ECHO VC vs14 targets -----------------------------
call:vstarget pc-vs14-x86 "%VS140COMNTOOLS%..\IDE\devenv" "Visual Studio 14"
@IF NOT %ERRORLEVEL% EQU 0 g.,,,,,,,,,oto EXIT_BAT

ECHO VC vs14x64 targets -----------------------------
call:vstarget pc-vs14-x64 "%VS140COMNTOOLS%..\IDE\devenv" "Visual Studio 14 Win64"
@IF NOT %ERRORLEVEL% EQU 0 goto EXIT_BAT

ECHO VC vs14 Coverage targets -----------------------------
REM Let the function setup the batch files and run cmake for us.
call:vstarget pc-vs14-x86-cov "%VS140COMNTOOLS%..\IDE\devenv" "Visual Studio 14"
@IF NOT %ERRORLEVEL% EQU 0 goto EXIT_BAT

REM add a coverage.bat file
pushd pc-vs14-x86-cov

echo pushd %test_root%                                                >  coverage.bat
echo start cmd.exe /c "%VS140COMNTOOLS%\..\..\Team Tools\Performance Tools\VSPerfCmd.exe" /START:COVERAGE /OUTPUT:%proj_root%\XR.coverage /WAITSTART>> coverage.bat
echo "%VS140COMNTOOLS%\..\..\Team Tools\Performance Tools\vsinstr.exe" %cd%\source\test\core\Debug\xr_core_test.exe /COVERAGE >> coverage.bat
echo "%VS140COMNTOOLS%\..\..\Team Tools\Performance Tools\vsinstr.exe" %cd%\source\test\services\Debug\xr_services_test.exe /COVERAGE >> coverage.bat
echo "%cd%\source\test\core\Debug\xr_core_test.exe" >> coverage.bat
echo "%cd%\source\test\services\Debug\xr_services_test.exe" >> coverage.bat
echo "%VS140COMNTOOLS%\..\..\Team Tools\Performance Tools\VSPerfCmd.exe" /SHUTDOWN >> coverage.bat
echo popd                                                             >> coverage.bat

REM replace the all.bat file.
echo call %%~dp0build.bat > all.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit %%ERRORLEVEL%% >> all.bat
echo call %%~dp0coverage.bat >> all.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit %%ERRORLEVEL%% >> all.bat

popd

ECHO VC vs14x64 Coverage targets -----------------------------
REM Let the function setup the batch files and run cmake for us.
call:vstarget pc-vs14-x64-cov "%VS140COMNTOOLS%..\IDE\devenv" "Visual Studio 14 Win64"
@IF NOT %ERRORLEVEL% EQU 0 goto EXIT_BAT

REM add a coverage.bat file
pushd pc-vs14-x64-cov

echo pushd %test_root%                                                >  coverage.bat
echo start cmd.exe /c "%VS140COMNTOOLS%\..\..\Team Tools\Performance Tools\VSPerfCmd.exe" /START:COVERAGE /OUTPUT:%proj_root%\XR.coverage /WAITSTART>> coverage.bat
echo "%VS140COMNTOOLS%\..\..\Team Tools\Performance Tools\vsinstr.exe" %cd%\source\test\core\Debug\xr_core_test.exe /COVERAGE >> coverage.bat
echo "%VS140COMNTOOLS%\..\..\Team Tools\Performance Tools\vsinstr.exe" %cd%\source\test\services\Debug\xr_services_test.exe /COVERAGE >> coverage.bat
echo "%cd%\source\test\core\Debug\xr_core_test.exe" >> coverage.bat
echo "%cd%\source\test\services\Debug\xr_services_test.exe" >> coverage.bat
echo "%VS140COMNTOOLS%\..\..\Team Tools\Performance Tools\VSPerfCmd.exe" /SHUTDOWN >> coverage.bat
echo popd                                                                >> coverage.bat

REM replace the all.bat file.
echo call %%~dp0build.bat > all.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit %%ERRORLEVEL%% >> all.bat
echo call %%~dp0coverage.bat >> all.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit %%ERRORLEVEL%% >> all.bat

popd         

:VS14_DONE

rem ---------------------------------------------------------------------------
if "gccpath"=="" goto GCC_DONE

:gcc5

REM Use this to skip GCC targets
rem goto GCC_DONE
ECHO GCC 5 targets -------------------------------


rem ---------------------------------------------------------------------------
call:gcctarget pc-mingwgcc5-x86 "%gccpath%" "MinGW Makefiles"
REM call:gcctarget pc-mingwgcc4-x86-cov "%gccpath%" "MinGW Makefiles"
REM TODO Add coverage.bat file

rem ---------------------------------------------------------------------------
:gcc4x64
ECHO GCC 5 x64 targets -------------------------------
rem ---------------------------------------------------------------------------
call:gcctarget pc-mingwgcc5-x64 "%gccpath%" "MinGW Makefiles"
REM call:gcctarget pc-mingwgcc4-x64-cov "%gccpath%" "MinGW Makefiles"
REM TODO Add coverage.bat file

REM return to the root directory.
popd

:GCC_DONE

rem ---------------------------------------------------------------------------
REM fallthrough from "setup" into "gen" intentional.
rem ---------------------------------------------------------------------------
:gen
REM Now Actually Generate.
pushd %proj_root%\build
del %proj_root%\build\commands.txt 
FOR /D %%T IN ("*") DO (
IF EXIST %CD%\%%T\gen.bat ECHO :%%T:%CD%\%%T\gen.bat >> %proj_root%\build\commands.txt
)
%runnerpath%\Runner.exe -file %proj_root%\build\commands.txt
@IF NOT %ERRORLEVEL% EQU 0 goto EXIT_BAT
popd

REM All done
goto EXIT_BAT
rem ---------------------------------------------------------------------------
:build
rem ---------------------------------------------------------------------------
pushd build

del %proj_root%\build\commands.txt 
FOR /D %%T IN ("*") DO (
IF EXIST %CD%\%%T\build.bat ECHO :%%T:%CD%\%%T\build.bat >> %proj_root%\build\commands.txt
)

%runnerpath%\Runner.exe -file %proj_root%\build\commands.txt
@IF NOT %ERRORLEVEL% EQU 0 goto EXIT_BAT


popd
goto EXIT_BAT
rem ---------------------------------------------------------------------------
:clean
rem ---------------------------------------------------------------------------
pushd build

del %proj_root%\build\commands.txt 
FOR /D %%T IN ("*") DO (
IF EXIST %CD%\%%T\clean.bat ECHO :%%T:%CD%\%%T\clean.bat >> %proj_root%\build\commands.txt
)

%runnerpath%\Runner.exe -file %proj_root%\build\commands.txt
@IF NOT %ERRORLEVEL% EQU 0 goto EXIT_BAT

popd
goto EXIT_BAT

rem ---------------------------------------------------------------------------
:test
rem ---------------------------------------------------------------------------
pushd build

del %proj_root%\build\commands.txt 
FOR /D %%T IN ("*") DO (
IF EXIST %CD%\%%T\test.bat ECHO :%%T:%CD%\%%T\test.bat >> %proj_root%\build\commands.txt
)

%runnerpath%\Runner.exe -file %proj_root%\build\commands.txt
@IF NOT %ERRORLEVEL% EQU 0 goto EXIT_BAT

popd
goto EXIT_BAT

rem ---------------------------------------------------------------------------
:docs
rem ---------------------------------------------------------------------------
pushd build

if exist doxy rmdir doxy /S /Q
if exist doxy goto permissionerror
mkdir doxy
pushd doxy

REM replace tokens with full paths based on current dir and on tools paths provided above.
%gawkpath%\gawk {gsub(/__HHC_LOCATION__/,"\"%hhcpath:\=\\%\\hhc.exe\"");gsub(/__DOT_PATH__/,"\"%dotpath:\=\\%\"");gsub(/__OUTPUT_DIRECTORY__/,"\"%CD:\=\\%\"");gsub(/__INPUT__/,"\"%proj_root:\=\\%\\include\"");gsub(/__CHM_FILE__/,"\"%CD:\=\\%\\xr.chm\"");print} "%proj_root%\setup\doxyfile-pc" > "%CD%\doxyfile-pc"

REM pausing here can be helpful when debugging the above awk command
REM pause

REM run doxygen
"%doxypath%\doxygen.exe" "%CD%\doxyfile-pc" "%proj_root%\setup\DoxygenLayout.xml"
REM echo if exist html rmdir html /S /Q > clean.bat
REM echo del xr.chm >> clean.bat

REM run it for good measure!
rem call build.bat

popd

popd
goto EXIT_BAT
rem ---------------------------------------------------------------------------
:printargs
rem ---------------------------------------------------------------------------
echo Valid arguments are setup, build, test, docs

rem ---------------------------------------------------------------------------
:permissionerror
echo ERROR: Build folders could not be deleted. 
echo Please close open files and try again.
goto EXIT_BAT
SET ERRORLEVEL=1
rem ---------------------------------------------------------------------------
:EXIT_BAT
rem ---------------------------------------------------------------------------
@set ERROR_TEMP=%ERRORLEVEL%
@cd %STARTING_FOLDER%
exit /b %ERROR_TEMP%

rem ---------------------------------------------------------------------------
rem FUNCTIONS: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
rem ---------------------------------------------------------------------------

rem ---------------------------------------------------------------------------
REM vstarget folder devenv generator
rem ---------------------------------------------------------------------------
:vstarget 
if exist %~1 rmdir %~1 /S /Q
if exist %~1 goto permissionerror
mkdir %~1
pushd %~1
echo pushd %%~dp0                                                 >  gen.bat
echo "%cmakepath%\cmake.exe" %CMAKE_DEBUG% -G "%~3" -D XR_BUILD_TARGET:STRING="%~1" %proj_root% >> gen.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%            >> gen.bat
echo popd                                                         >> gen.bat
echo "%~2" %cd%\XR.sln /Build "Debug"   > build.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%           >> build.bat
echo "%~2" %cd%\XR.sln /Build "Release" >> build.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%           >> build.bat
echo "%~2" %cd%\XR.sln /Clean "Debug"   > clean.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%           >> clean.bat
echo "%~2" %cd%\XR.sln /Clean "Release" >> clean.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%           >> clean.bat
@IF NOT %ERRORLEVEL% EQU 0 goto EXIT_BAT
echo pushd %%~dp0                                                      >  test.bat
echo "%cmakepath%\ctest.exe" --force-new-ctest-process --output-on-failure -C debug test   >> test.bat
echo "%cmakepath%\ctest.exe" --force-new-ctest-process --output-on-failure -C release test >> test.bat
echo popd                                                              >> test.bat

echo call %%~dp0build.bat > all.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit %%ERRORLEVEL%% >> all.bat
echo call %%~dp0test.bat >> all.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit %%ERRORLEVEL%% >> all.bat

popd
goto:eof



rem ---------------------------------------------------------------------------
REM gcc folder makepath generator
rem ---------------------------------------------------------------------------
:gcctarget 
if exist %~1 rmdir %~1 /S /Q
if exist %~1 goto permissionerror
mkdir %~1
pushd %~1
REM Make bat files to call the debug and Release bat files.
echo pushd %%~dp0Debug 	  >  gen.bat
echo set gccpath=%gccpath% >> gen.bat
echo call gen.bat 	  >> gen.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%      >> gen.bat
echo popd                 >> gen.bat
echo pushd %%~dp0Release  >> gen.bat
echo call gen.bat       >> gen.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%      >> gen.bat
echo popd				  >> gen.bat
echo pushd %%~dp0Debug 	  >  build.bat
echo set gccpath=%gccpath% >> build.bat
echo call build.bat 	  >> build.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%      >> build.bat
echo popd                 >> build.bat
echo pushd %%~dp0Release  >> build.bat
echo call build.bat       >> build.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%      >> build.bat
echo popd				  >> build.bat
echo pushd %%~dp0Debug    >  clean.bat
echo call clean.bat 	  >> clean.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%      >> clean.bat
echo popd                 >> clean.bat
echo pushd %%~dp0Release  >> clean.bat
echo call clean.bat 	  >> clean.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%      >> clean.bat
echo popd                 >> clean.bat
echo pushd %%~dp0Debug    >  test.bat
echo call test.bat        >> test.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%      >> test.bat
echo popd                 >> test.bat
echo pushd %%~dp0Release  >> test.bat
echo call test.bat        >> test.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%      >> test.bat
echo popd                 >> test.bat
REM Make the directories.
mkdir Debug
mkdir Release
REM Make the build, clean and test bat files.
pushd Debug
echo pushd %%~dp0                              >  gen.bat
echo "%cmakepath%\cmake.exe" %CMAKE_DEBUG% -G "%~3" -D XR_BUILD_TARGET:STRING="%~1" -D CMAKE_MAKE_PROGRAM:FILEPATH="%~2/mingw32-make.exe" -D CMAKE_BUILD_TYPE:STRING="Debug" %proj_root% >> gen.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%            >> gen.bat
echo popd                                      >>  gen.bat
echo pushd %%~dp0 							   >  build.bat
echo "%~2/mingw32-make.exe" all								   >> build.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%      >> build.bat
echo popd	    							   >> build.bat
echo pushd %test_root%   							   >  test.bat
echo "%cd%\source\test\core\xr_core_test.exe"        >> test.bat
echo "%cd%\source\test\services\xr_services_test.exe"        >> test.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%      >> test.bat
echo popd	    							   >> test.bat
echo pushd %%~dp0 							   >  clean.bat
echo "%~2/mingw32-make.exe" clean							   >> clean.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%      >> clean.bat
echo popd	    							   >> clean.bat

echo call %%~dp0build.bat > all.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit %%ERRORLEVEL%% >> all.bat
echo call %%~dp0test.bat >> all.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit %%ERRORLEVEL%% >> all.bat

@IF NOT %ERRORLEVEL% EQU 0 goto EXIT_BAT
popd
pushd Release
echo pushd %%~dp0                              >  gen.bat
echo "%cmakepath%\cmake.exe" %CMAKE_DEBUG% -G "%~3" -D XR_BUILD_TARGET:STRING="%~1" -D CMAKE_MAKE_PROGRAM:FILEPATH="%~2/mingw32-make.exe" -D CMAKE_BUILD_TYPE:STRING="Release" %proj_root% >>  gen.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%            >> gen.bat
echo popd                                      >>  gen.bat
echo pushd %%~dp0 							   >  build.bat
echo "%~2/mingw32-make.exe" all								   >> build.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%      >> build.bat
echo popd	    							   >> build.bat
echo pushd %test_root%						   >  test.bat
echo "%cd%\source\test\core\xr_core_test.exe"        >> test.bat
echo "%cd%\source\test\services\xr_services_test.exe"        >> test.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%      >> test.bat
echo popd	    							   >> test.bat
echo pushd %%~dp0 							   >  clean.bat
echo "%~2/mingw32-make.exe" clean                               >> clean.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit /b %%ERRORLEVEL%%      >> clean.bat
echo popd	    							   >> clean.bat

echo call %%~dp0build.bat > all.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit %%ERRORLEVEL%% >> all.bat
echo call %%~dp0test.bat >> all.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit %%ERRORLEVEL%% >> all.bat

@IF NOT %ERRORLEVEL% EQU 0 goto EXIT_BAT
REM return to %~1 dir
popd

echo call %%~dp0build.bat > all.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit %%ERRORLEVEL%% >> all.bat
echo call %%~dp0test.bat >> all.bat
echo IF NOT %%ERRORLEVEL%% EQU 0 exit %%ERRORLEVEL%% >> all.bat

REM return to the build directory.
popd
goto:eof
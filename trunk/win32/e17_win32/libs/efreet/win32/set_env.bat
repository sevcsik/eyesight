@echo off

rem Set external libraries directory.
set EXT_DIR=%cd%\..\..\..\..\extern

if not exist %EXT_DIR% (
	set EXT_DIR=
	echo ERROR: External libs dir is not set.
	pause
	goto END
)

rem Add installation directory pathes.
set INCLUDE=%EXT_DIR%\include;%INCLUDE%
set LIB=%EXT_DIR%\lib;%LIB%

rem Add Evas lib path
set EvasPath=%cd%\..\src\lib
set INCLUDE=%EvasPath%\..\modules\engines\gl_common;%EvasPath%;%EvasPath%\include;%INCLUDE%

if "%DXSDK_DIR%" == "" (
	echo WARNING: Microsoft DirectX SDK is not installed.
	pause
	goto END
)

rem Add DirectX includes and libraries dirs.
set INCLUDE="%DXSDK_DIR%Include";%INCLUDE%
set LIB="%DXSDK_DIR%Lib\x86";%LIB%

rem Add Evil lib path
set EvilInclude=%cd%\..\..\..\proto\evil\src\lib
set EvilCommon=%cd%\..\..\..\proto\evil\win32\common
set EvilOut=%cd%\..\..\..\proto\evil\win32\%PROJECT_TYPE%\out

rem Add Eet lib path
set EetInclude=%cd%\..\..\eet\src\lib
set EetOut=%cd%\..\..\eet\win32\%PROJECT_TYPE%\out

rem Add Evas lib path
set EvasInclude=%cd%\..\..\evas\src\lib
set EvasOut=%cd%\..\..\evas\win32\%PROJECT_TYPE%\out

rem Add Ecore lib path
set EcoreInclude=%cd%\..\..\ecore\src\lib\ecore
set EcoreOut=%cd%\..\..\ecore\win32\%PROJECT_TYPE%\out

rem Add Embryo lib path
set EmbryoInclude=%cd%\..\..\embryo\src\lib
set EmbryoOut=%cd%\..\..\embryo\win32\%PROJECT_TYPE%\out


set INCLUDE=%EvilCommon%;%cd%\common;%EetInclude%;%EvilInclude%;%EvilInclude%\dlfcn;%EvilInclude%\mman;%INCLUDE%
set INCLUDE=%EcoreInclude%\..\ecore_file;%EcoreInclude%\..\ecore_job;%EmbryoInclude%;%EcoreInclude%;%EvasInclude%;%INCLUDE%
set INCLUDE=%EcoreInclude%\..\ecore_evas;%INCLUDE%
set LIB=%EmbryoOut%;%EcoreOut%;%EvasOut%;%EetOut%;%EvilOut%;%LIB%


:END

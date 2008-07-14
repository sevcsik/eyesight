@echo off

set Target=%cd%
set e17=%cd%\..\..\..\..\..\..
set Libs=%e17%\libs
set Proto=%e17%\proto
set Extern=%e17%\..\extern

rem Copy externals
cd %Extern%\bin
xcopy freetype6.dll %Target%\ /Q /Y
xcopy jpeg62.dll %Target%\ /Q /Y
xcopy zlib1.dll %Target%\ /Q /Y
xcopy glew32.dll %Target%\ /Q /Y

rem Copy proto
cd %Proto%\evil\win32\vs8\out
xcopy libdl.dll %Target%\ /Q /Y
if exist libdl.pdb xcopy libdl.pdb %Target%\ /Q /Y
xcopy libevil.dll %Target%\ /Q /Y
if exist libevil.pdb xcopy libevil.pdb %Target%\ /Q /Y
xcopy libmman.dll %Target%\ /Q /Y
if exist libmman.pdb xcopy libmman.pdb %Target%\ /Q /Y

rem Copy libs
cd %Libs%\eet\win32\vs8\out
xcopy libeet.dll %Target%\ /Q /Y

cd %Libs%\evas\win32\vs8\out
xcopy libevas.dll %Target%\ /Q /Y
if exist libevas.pdb xcopy libevas.pdb %Target%\ /Q /Y

cd %Libs%\ecore\win32\vs8\out
xcopy libecore.dll %Target%\ /Q /Y
if exist libecore.pdb xcopy libecore.pdb %Target%\ /Q /Y
xcopy libecore_evas.dll %Target%\ /Q /Y
if exist libecore_evas.pdb xcopy libecore_evas.pdb %Target%\ /Q /Y
xcopy libecore_win32.dll %Target%\ /Q /Y
if exist libecore_win32.pdb xcopy libecore_win32.pdb %Target%\ /Q /Y


set Modules=%Libs%\evas\win32\vs8\out\modules
cd %Modules%\engines\direct3d
xcopy module.dll %Target%\evas\modules\engines\direct3d\mingw32-i686\ /Q /Y
if exist module.pdb xcopy module.pdb %Target%\evas\modules\engines\direct3d\mingw32-i686\ /Q /Y
cd %Modules%\engines\software_ddraw
xcopy module.dll %Target%\evas\modules\engines\software_ddraw\mingw32-i686\ /Q /Y
cd %Modules%\engines\software_generic
xcopy module.dll %Target%\evas\modules\engines\software_generic\mingw32-i686\ /Q /Y
cd %Modules%\engines\gl_glew
xcopy module.dll %Target%\evas\modules\engines\gl_glew\mingw32-i686\ /Q /Y
cd %Modules%\loaders\png
xcopy module.dll %Target%\evas\modules\loaders\png\mingw32-i686\ /Q /Y

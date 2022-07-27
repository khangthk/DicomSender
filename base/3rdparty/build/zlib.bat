@echo off
call msvc.bat
if errorlevel 1 ( pause && exit )
set library=zlib
cd ..
set bin=..\..\bin\%library%
set build=..\..\build\%library%
cmake -G "Visual Studio %VS_NUMBER% %VS_VERSION%" -DCMAKE_CXX_STANDARD=17 -DCMAKE_INSTALL_PREFIX=%bin% -S %library% -B %build%
MSBuild %build%\ALL_BUILD.vcxproj -t:Build -p:Configuration=Debug -m:%NUMBER_OF_PROCESSORS%
MSBuild %build%\ALL_BUILD.vcxproj -t:Build -p:Configuration=Release -m:%NUMBER_OF_PROCESSORS%
echo f | xcopy %build%\Debug\zlibstaticd.lib %build%\bin\lib\zlib_d.lib /y >nul
echo f | xcopy %build%\Release\zlibstatic.lib %build%\bin\lib\zlib_o.lib /y >nul
echo f | xcopy %build%\zconf.h %build%\bin\include\zconf.h /y >nul
echo f | xcopy zlib\zlib.h %build%\bin\include\zlib.h /y >nul
echo f | xcopy zlib\zconf.h.included zlib\zconf.h /y >nul
del /f /s /q zlib\zconf.h.included >nul
pause
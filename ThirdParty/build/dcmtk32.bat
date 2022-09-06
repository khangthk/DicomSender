@echo off
call msvc.bat x86
if errorlevel 1 (pause && exit)
set library=dcmtk
cd ..
set bin=..\bin\%VS_ARCH%\%library%
set build=..\build\%VS_ARCH%\%library%
set zlib=..\build\%VS_ARCH%\zlib\bin

set CMAKE_ARCH=x64
if %VS_ARCH%==x86 (set CMAKE_ARCH=Win32)
set CMAKE_PARAM_A=-A %CMAKE_ARCH%
if %VS_VERSION%==2017 (set CMAKE_PARAM_A=&& set CMAKE_ARCH= %CMAKE_ARCH%) else (set CMAKE_ARCH=)

cmake -G "Visual Studio %VS_NUMBER% %VS_VERSION%%CMAKE_ARCH%" %CMAKE_PARAM_A% -DCMAKE_CXX_STANDARD=17 -DBUILD_APPS=OFF -DDCMTK_USE_DCMDICTPATH=OFF -DDCMTK_ENABLE_PRIVATE_TAGS=ON -DDCMTK_COMPILE_WIN32_MULTITHREADED_DLL=ON -DDCMTK_OVERWRITE_WIN32_COMPILER_FLAGS=OFF -DDCMTK_WITH_ZLIB=ON -DWITH_ZLIBINC=%zlib% -DCMAKE_INSTALL_PREFIX=%bin% -S %library% -B %build%
MSBuild %build%\ALL_BUILD.vcxproj -t:Build -p:Configuration=Debug -m:%NUMBER_OF_PROCESSORS%
MSBuild %build%\ALL_BUILD.vcxproj -t:Build -p:Configuration=Release -m:%NUMBER_OF_PROCESSORS%
pause
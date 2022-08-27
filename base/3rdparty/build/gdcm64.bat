@echo off
call msvc.bat x64
if errorlevel 1 (pause && exit)
set library=gdcm
cd ..
set bin=..\..\bin\%VS_ARCH%\%library%
set build=..\..\build\%VS_ARCH%\%library%
set zlib=..\..\build\%VS_ARCH%\zlib\bin

set CMAKE_ARCH=x64
if %VS_ARCH%==x86 (set CMAKE_ARCH=Win32)
set CMAKE_PARAM_A=-A %CMAKE_ARCH%
if %VS_VERSION%==2017 (set CMAKE_PARAM_A=&& set CMAKE_ARCH= %CMAKE_ARCH%) else (set CMAKE_ARCH=)

cmake -G "Visual Studio %VS_NUMBER% %VS_VERSION%%CMAKE_ARCH%" %CMAKE_PARAM_A% -DCMAKE_CXX_STANDARD=17 -DGDCM_BUILD_TESTING=OFF -DGDCM_USE_SYSTEM_ZLIB=ON -DZLIB_INCLUDE_DIR=%zlib%\include -DZLIB_LIBRARY_DEBUG=%zlib%\lib\zlib_d.lib -DZLIB_LIBRARY_RELEASE=%zlib%\lib\zlib_o.lib -DCMAKE_INSTALL_PREFIX=%bin% -S %library% -B %build%
MSBuild %build%\ALL_BUILD.vcxproj -t:Build -p:Configuration=Debug -m:%NUMBER_OF_PROCESSORS%
MSBuild %build%\ALL_BUILD.vcxproj -t:Build -p:Configuration=Release -m:%NUMBER_OF_PROCESSORS%
pause
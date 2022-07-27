@echo off
call msvc.bat
if errorlevel 1 ( pause && exit )
set library=dcmtk
cd ..
set bin=..\..\bin\%library%
set build=..\..\build\%library%
set zlib=..\..\build\zlib\bin
cmake -G "Visual Studio %VS_NUMBER% %VS_VERSION%" -DCMAKE_CXX_STANDARD=17 -DBUILD_APPS=OFF -DDCMTK_USE_DCMDICTPATH=OFF -DDCMTK_ENABLE_PRIVATE_TAGS=ON -DDCMTK_COMPILE_WIN32_MULTITHREADED_DLL=ON -DDCMTK_OVERWRITE_WIN32_COMPILER_FLAGS=OFF -DDCMTK_WITH_ZLIB=ON -DWITH_ZLIBINC=%zlib% -DCMAKE_INSTALL_PREFIX=%bin% -S %library% -B %build%
MSBuild %build%\ALL_BUILD.vcxproj -t:Build -p:Configuration=Debug -m:%NUMBER_OF_PROCESSORS%
MSBuild %build%\ALL_BUILD.vcxproj -t:Build -p:Configuration=Release -m:%NUMBER_OF_PROCESSORS%
pause
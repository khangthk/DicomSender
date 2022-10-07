@echo off
cd ..\..\gdcm
git apply ..\build\patch\gdcm\gdcmDefaultDicts.cxx.patch
git apply ..\build\patch\gdcm\gdcmTagKeywords.h.patch
git apply ..\build\patch\gdcm\gdcmTagToType.h.patch
git apply ..\build\patch\gdcm\gdcmTagToVR.cxx.patch
git apply ..\build\patch\gdcm\gdcmUIDs.cxx.patch
git apply ..\build\patch\gdcm\gdcmUIDs.h.patch
git apply ..\build\patch\gdcm\gdcmVM.h.patch
git apply ..\build\patch\gdcm\sockstream.cpp.patch
pause
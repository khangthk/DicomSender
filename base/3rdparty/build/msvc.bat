@echo off
setlocal
set "InstallerPath=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer"
if not exist "%InstallerPath%" set "InstallerPath=%ProgramFiles%\Microsoft Visual Studio\Installer"
if not exist "%InstallerPath%" goto :no-vswhere
set Path=%Path%;%InstallerPath%
where vswhere 2> nul > nul
if errorlevel 1 goto :no-vswhere

set VSWHERE_REQ=-requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64
set VSWHERE_PRP=-property installationPath

REM Visual Studio 2022 (17.X, toolset 14.3)
set VSWHERE_LMT=-version "[17.0,18.0)"
set VSWHERE_PRERELEASE=-prerelease
set VSWHERE_ARGS=-latest -products * %VSWHERE_REQ% %VSWHERE_PRP% %VSWHERE_LMT% %VSWHERE_PRERELEASE%
for /f "usebackq tokens=*" %%i in (`vswhere %VSWHERE_ARGS%`) do (
	endlocal
	call "%%i\VC\Auxiliary\Build\vcvars64.bat"
	set VS_NUMBER="17"
	set VS_VERSION="2022"
	exit /b 0
)

REM Visual Studio 2019 (16.X, toolset 14.2)
set VSWHERE_LMT=-version "[16.0,17.0)"
set VSWHERE_ARGS=-latest -products * %VSWHERE_REQ% %VSWHERE_PRP% %VSWHERE_LMT% %VSWHERE_PRERELEASE%
for /f "usebackq tokens=*" %%i in (`vswhere %VSWHERE_ARGS%`) do (
	endlocal
	call "%%i\VC\Auxiliary\Build\vcvars64.bat"
	set VS_NUMBER="16"
	set VS_VERSION="2019"
	exit /b 0
)

REM Visual Studio 2017 (15.X, toolset 14.1)
set VSWHERE_LMT=-version "[15.0,16.0)"
set VSWHERE_ARGS=-latest -products * %VSWHERE_REQ% %VSWHERE_PRP% %VSWHERE_LMT%
for /f "usebackq tokens=*" %%i in (`vswhere %VSWHERE_ARGS%`) do (
	endlocal
	call "%%i\VC\Auxiliary\Build\vcvars64.bat"
	set VS_NUMBER="15"
	set VS_VERSION="2017"
	exit /b 0
)

:no-vswhere
endlocal
echo could not find "vswhere"
exit /b 1

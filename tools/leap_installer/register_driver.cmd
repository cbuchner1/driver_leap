@echo off
cls
SET mypath=%~dp0

REM detect Steam install directory
for /f "tokens=2*" %%A in ('reg query hku /e /c /v /f SteamPath /t REG_SZ /s 2^>^NUL') DO (
	set SteamPath=%%B
	goto :start
)
:start

REM register driver
"%SteamPath%\steamapps\common\OpenVR\bin\win64\vrpathreg.exe" adddriver "%mypath:~0,-1%\leap"

REM backup steamvr.vrsettings in steam config dir
copy "%SteamPath%\config\steamvr.vrsettings" "%SteamPath%\config\steamvr.vrsettings.leap_backup"

REM copy steamvr.vrsettings to steam config dir
copy "%mypath:~0,-1%\steamvr.vrsettings" "%SteamPath%\config\"

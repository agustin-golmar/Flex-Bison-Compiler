@SETLOCAL

@set BASE_PATH=%~dp0
@set BASE_PATH=%BASE_PATH:~0,-1%
@set BASE_PATH=%BASE_PATH%\..\..

@echo [93mBuilding the compiler...[0m
@echo:
@rmdir /Q /S %BASE_PATH%\build >nul 2>&1
@cmake -S %BASE_PATH% -B %BASE_PATH%\build
@echo:
@echo [93mCMake finished scaffolding. Find the project inside 'build' folder.[0m
@echo:

@ENDLOCAL

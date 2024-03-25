@SETLOCAL ENABLEDELAYEDEXPANSION

@set BASE_PATH=%~dp0
@set BASE_PATH=%BASE_PATH:~0,-1%
@set BASE_PATH=%BASE_PATH%\..\..

@echo Compiler should accept...
@echo:

@set STATUS=0
@for /f %%f in ('dir /b !BASE_PATH!\src\test\c\accept') do @(
	@type !BASE_PATH!\src\test\c\accept\%%f | !BASE_PATH!\build\Debug\Compiler.exe >nul 2>&1
	@set RESULT=!ERRORLEVEL!
	if !RESULT! equ 0 (
		@echo     "%%f", [92mand it does[0m ^(status !RESULT!^)
	) else (
		@set STATUS=1
		@echo     "%%f", [91mbut it rejects[0m ^(status !RESULT!^)
	)
)
@echo:

@echo Compiler should reject...
@echo:

@for /f %%f in ('dir /b !BASE_PATH!\src\test\c\reject') do @(
	@type !BASE_PATH!\src\test\c\reject\%%f | !BASE_PATH!\build\Debug\Compiler.exe >nul 2>&1
	@set RESULT=!ERRORLEVEL!
	if !RESULT! neq 0 (
		@echo     "%%f", [92mand it does[0m ^(status !RESULT!^)
	) else (
		@set STATUS=1
		@echo     "%%f", [91mbut it accepts[0m ^(status !RESULT!^)
	)
)

@exit /B %STATUS%

@ENDLOCAL

@setlocal ENABLEDELAYEDEXPANSION

@echo Compiler should accept...
@echo:

@for /f %%f in ('dir /b test\accept') do @(
	@type test\accept\%%f | bin\Debug\Compiler.exe >nul 2>&1
	@set result=!ERRORLEVEL!
	if !result! equ 0 (
		@echo     "%%f", [92mand it does[0m ^(status !result!^)
	) else (
		@echo     "%%f", [91mbut it rejects[0m ^(status !result!^)
	)
)
@echo:

@echo Compiler should reject...
@echo:

@for /f %%f in ('dir /b test\reject') do @(
	@type test\reject\%%f | bin\Debug\Compiler.exe >nul 2>&1
	@set result=!ERRORLEVEL!
	if !result! neq 0 (
		@echo     "%%f", [92mand it does[0m ^(status !result!^)
	) else (
		@echo     "%%f", [91mbut it accepts[0m ^(status !result!^)
	)
)

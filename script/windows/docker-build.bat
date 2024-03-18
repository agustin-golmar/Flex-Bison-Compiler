@SETLOCAL

@set BASE_PATH=%~dp0
@set BASE_PATH=%BASE_PATH:~0,-1%
@set BASE_PATH=%BASE_PATH%\..\..

@REM https://docs.docker.com/reference/cli/docker/image/build/
@docker build ^
	--progress=plain ^
	--tag=flex-bison-compiler ^
	%BASE_PATH%

@ENDLOCAL

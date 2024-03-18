@SETLOCAL

@set BASE_PATH=%~dp0
@set BASE_PATH=%BASE_PATH:~0,-1%
@set BASE_PATH=%BASE_PATH%\..\..

@REM https://docs.docker.com/reference/cli/docker/container/run/
@docker run ^
	--hostname docker ^
	--interactive ^
	--rm ^
	--user ubuntu ^
	--tty ^
	--volume %BASE_PATH%:/home/ubuntu/Flex-Bison-Compiler ^
	--workdir=/home/ubuntu/Flex-Bison-Compiler ^
	flex-bison-compiler:latest

@ENDLOCAL

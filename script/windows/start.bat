@SETLOCAL

@set BASE_PATH=%~dp0
@set BASE_PATH=%BASE_PATH:~0,-1%
@set BASE_PATH=%BASE_PATH%\..\..

@type %1 | %BASE_PATH%\build\Debug\Compiler.exe

@ENDLOCAL

@ECHO ON

IF "%2"=="" GOTO usage:
IF "%1"=="" GOTO usage:

SET mod_dir=%1
SET env_dir=%2

IF NOT EXIST %mod_dir% GOTO error_mod_dir:
IF NOT EXIST %env_dir% MD %env_dir%

ECHO 正在复制模块文件到 %env_dir% ...
COPY /Y %mod_dir%\*.dll %env_dir%\
COPY /Y %mod_dir%\*.exe %env_dir%\
COPY /Y %mod_dir%\*.ocx %env_dir%\
GOTO end:

:error_mod_dir
  ECHO 出错: 模块目录不存在: %mod_dir%
  GOTO usage

:usage
  ECHO.
  ECHO 说明: 将模块文件（*.dll、*.exe）复制到环境目录中。
  ECHO copy_module_to_env.bat [ModuleDir] [EnvDir]
  ECHO.
  ECHO   [ModuleDir]
  ECHO               模块目录全路径，即生产 *.dll、*.exe的目录。
  ECHO   [EnvDir]
  ECHO               环境目录全路径，即用于运行或调试程序的目录。
  ECHO.

:end
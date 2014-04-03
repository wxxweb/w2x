@ECHO ON

IF "%2"=="" GOTO usage:
IF "%1"=="" GOTO usage:

SET res_dir=%1
SET env_dir=%2
SET env_res_dir=%env_dir%\res

IF NOT EXIST %res_dir% GOTO error_res_dir:
IF NOT EXIST %env_dir% MD %env_dir%

ECHO 正在复制资源文件到 %env_dir% ...

IF NOT EXIST %env_res_dir% MD %env_res_dir%
COPY /Y %res_dir%\*.* %env_dir%\res\


IF NOT EXIST %env_res_dir%\flash MD %env_res_dir%\flash
COPY /Y %res_dir%\flash\*.swf %env_res_dir%\flash\

GOTO end:

:error_res_dir
  ECHO 出错: 资源目录不存在: %res_dir%
  GOTO usage

:usage
  ECHO.
  ECHO 说明: 将资源文件（图片、动画等）复制到环境目录中。
  ECHO copy_res_to_env.bat [ResDir] [EnvDir]
  ECHO.
  ECHO   [ResDir]
  ECHO               资源目录全路径，该目录包含程序运行所需资源。
  ECHO   [EnvDir]
  ECHO               环境目录全路径，即用于运行或调试程序的目录。
  ECHO.

:end
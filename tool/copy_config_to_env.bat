@ECHO ON

IF "%2"=="" GOTO usage:
IF "%1"=="" GOTO usage:

SET config_dir=%1
SET env_dir=%2
SET env_config_dir=%env_dir%\config

IF NOT EXIST %config_dir% GOTO error_config_dir:
IF NOT EXIST %env_dir% MD %env_dir%

ECHO 正在复制资源文件到 %env_dir% ...

IF NOT EXIST %env_config_dir% MD %env_config_dir%
COPY /Y %config_dir%\*.xml %env_config_dir%

IF NOT EXIST %env_config_dir%\flash MD %env_config_dir%\flash
COPY /Y %config_dir%\flash\*.xml %env_config_dir%\flash\

GOTO end:

:error_config_dir
  ECHO 出错: 资源目录不存在: %config_dir%
  GOTO usage

:usage
  ECHO.
  ECHO 说明: 将配置文件复制到环境目录中。
  ECHO copy_res_to_env.bat [ConfigDir] [EnvDir]
  ECHO.
  ECHO   [ConfigDir]
  ECHO               配置目录全路径，该目录包含程序运行所需配置文件。
  ECHO   [EnvDir]
  ECHO               环境目录全路径，即用于运行或调试程序的目录。
  ECHO.

:end
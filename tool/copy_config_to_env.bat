@ECHO ON

IF "%2"=="" GOTO usage:
IF "%1"=="" GOTO usage:

SET config_dir=%1
SET env_dir=%2
SET env_config_dir=%env_dir%\config

IF NOT EXIST %config_dir% GOTO error_config_dir:
IF NOT EXIST %env_dir% MD %env_dir%

ECHO ���ڸ�����Դ�ļ��� %env_dir% ...

IF NOT EXIST %env_config_dir% MD %env_config_dir%
COPY /Y %config_dir%\*.xml %env_config_dir%

IF NOT EXIST %env_config_dir%\flash MD %env_config_dir%\flash
COPY /Y %config_dir%\flash\*.xml %env_config_dir%\flash\

GOTO end:

:error_config_dir
  ECHO ����: ��ԴĿ¼������: %config_dir%
  GOTO usage

:usage
  ECHO.
  ECHO ˵��: �������ļ����Ƶ�����Ŀ¼�С�
  ECHO copy_res_to_env.bat [ConfigDir] [EnvDir]
  ECHO.
  ECHO   [ConfigDir]
  ECHO               ����Ŀ¼ȫ·������Ŀ¼���������������������ļ���
  ECHO   [EnvDir]
  ECHO               ����Ŀ¼ȫ·�������������л���Գ����Ŀ¼��
  ECHO.

:end
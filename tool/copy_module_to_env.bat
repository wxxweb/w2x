@ECHO ON

IF "%2"=="" GOTO usage:
IF "%1"=="" GOTO usage:

SET mod_dir=%1
SET env_dir=%2

IF NOT EXIST %mod_dir% GOTO error_mod_dir:
IF NOT EXIST %env_dir% MD %env_dir%

ECHO ���ڸ���ģ���ļ��� %env_dir% ...
COPY /Y %mod_dir%\*.dll %env_dir%\
COPY /Y %mod_dir%\*.exe %env_dir%\
COPY /Y %mod_dir%\*.ocx %env_dir%\
GOTO end:

:error_mod_dir
  ECHO ����: ģ��Ŀ¼������: %mod_dir%
  GOTO usage

:usage
  ECHO.
  ECHO ˵��: ��ģ���ļ���*.dll��*.exe�����Ƶ�����Ŀ¼�С�
  ECHO copy_module_to_env.bat [ModuleDir] [EnvDir]
  ECHO.
  ECHO   [ModuleDir]
  ECHO               ģ��Ŀ¼ȫ·���������� *.dll��*.exe��Ŀ¼��
  ECHO   [EnvDir]
  ECHO               ����Ŀ¼ȫ·�������������л���Գ����Ŀ¼��
  ECHO.

:end
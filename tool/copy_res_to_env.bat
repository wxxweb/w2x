@ECHO ON

IF "%2"=="" GOTO usage:
IF "%1"=="" GOTO usage:

SET res_dir=%1
SET env_dir=%2
SET env_res_dir=%env_dir%\res

IF NOT EXIST %res_dir% GOTO error_res_dir:
IF NOT EXIST %env_dir% MD %env_dir%

ECHO ���ڸ�����Դ�ļ��� %env_dir% ...

IF NOT EXIST %env_res_dir% MD %env_res_dir%
COPY /Y %res_dir%\*.* %env_dir%\res\


IF NOT EXIST %env_res_dir%\flash MD %env_res_dir%\flash
COPY /Y %res_dir%\flash\*.swf %env_res_dir%\flash\

GOTO end:

:error_res_dir
  ECHO ����: ��ԴĿ¼������: %res_dir%
  GOTO usage

:usage
  ECHO.
  ECHO ˵��: ����Դ�ļ���ͼƬ�������ȣ����Ƶ�����Ŀ¼�С�
  ECHO copy_res_to_env.bat [ResDir] [EnvDir]
  ECHO.
  ECHO   [ResDir]
  ECHO               ��ԴĿ¼ȫ·������Ŀ¼������������������Դ��
  ECHO   [EnvDir]
  ECHO               ����Ŀ¼ȫ·�������������л���Գ����Ŀ¼��
  ECHO.

:end
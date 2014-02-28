@ECHO ON

IF "%3"=="" GOTO usage
IF /i NOT "%3"=="dll" (IF /i NOT "%3"=="lib" GOTO usage)
IF "%2"=="" GOTO usage:
IF "%1"=="" GOTO usage:
IF NOT EXIST %1 GOTO error_arg1
IF /i NOT "%3"=="lib" GOTO copy_dll

:copy_lib
  SET src_lib_dir=%1third-party\lib\%2
  SET dst_lib_dir=%1lib\%2
  IF NOT EXIST %src_lib_dir% GOTO error_src_lib_dir
  IF NOT EXIST %dst_lib_dir% GOTO error_dst_lib_dir
  ECHO ���ڸ��Ƶ�������̬�⵽ %dst_lib_dir% ...
  COPY /Y %src_lib_dir%\*.lib %dst_lib_dir%\
  GOTO end

:copy_dll
  SET src_dll_dir=%1third-party\dll\%2
  SET dst_dll_dir=%1bin\%2
  IF NOT EXIST %src_dll_dir% GOTO error_src_dll_dir
  IF NOT EXIST %dst_dll_dir% GOTO error_dst_dll_dir
  ECHO ���ڸ��Ƶ�������̬�⵽ %dst_dll_dir% ...
  COPY /Y %src_dll_dir%\*.dll %dst_dll_dir%\
  GOTO end

:error_arg1
  ECHO ����: �������Ŀ¼������: "%1"
  GOTO usage

:error_src_dll_dir
  ECHO ����: ������DLLĿ¼������: %src_dll_dir%
  GOTO usage

:error_dst_dll_dir
  ECHO ����: ��ĿĿ���ļ�Ŀ¼������: %dst_dll_dir%
  GOTO usage

:error_src_lib_dir
  ECHO ����: ������LIBĿ¼������: %src_lib_dir%
  GOTO usage

:error_dst_lib_dir
  ECHO ����: ��ĿLIBĿ¼������: %dst_dll_dir%
  GOTO usage

:usage
  ECHO.
  ECHO ˵��: ����������µ������⿽������ĿĿ¼�С�
  ECHO copy_third_party.bat [SolutionDir] [Configuration] [Extension]
  ECHO.
  ECHO   [SolutionDir]
  ECHO               ��Ŀ�������Ŀ¼ȫ·����
  ECHO   [Configuration]
  ECHO               ��������Ŀ¼���ƣ�Debug �� Release��
  ECHO   [Extension]
  ECHO               ���ļ����ͣ�dll �� lib��
  ECHO.

:end
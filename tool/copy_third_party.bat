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
  ECHO 正在复制第三方静态库到 %dst_lib_dir% ...
  COPY /Y %src_lib_dir%\*.lib %dst_lib_dir%\
  GOTO end

:copy_dll
  SET src_dll_dir=%1third-party\dll\%2
  SET dst_dll_dir=%1bin\%2
  IF NOT EXIST %src_dll_dir% GOTO error_src_dll_dir
  IF NOT EXIST %dst_dll_dir% GOTO error_dst_dll_dir
  ECHO 正在复制第三方动态库到 %dst_dll_dir% ...
  COPY /Y %src_dll_dir%\*.dll %dst_dll_dir%\
  GOTO end

:error_arg1
  ECHO 出错: 解决方案目录不存在: "%1"
  GOTO usage

:error_src_dll_dir
  ECHO 出错: 第三方DLL目录不存在: %src_dll_dir%
  GOTO usage

:error_dst_dll_dir
  ECHO 出错: 项目目标文件目录不存在: %dst_dll_dir%
  GOTO usage

:error_src_lib_dir
  ECHO 出错: 第三方LIB目录不存在: %src_lib_dir%
  GOTO usage

:error_dst_lib_dir
  ECHO 出错: 项目LIB目录不存在: %dst_dll_dir%
  GOTO usage

:usage
  ECHO.
  ECHO 说明: 将解决方案下第三方库拷贝到项目目录中。
  ECHO copy_third_party.bat [SolutionDir] [Configuration] [Extension]
  ECHO.
  ECHO   [SolutionDir]
  ECHO               项目解决方案目录全路径。
  ECHO   [Configuration]
  ECHO               配置类型目录名称，Debug 或 Release。
  ECHO   [Extension]
  ECHO               库文件类型，dll 或 lib。
  ECHO.

:end
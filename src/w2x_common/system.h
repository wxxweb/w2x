/*******************************************************************************
 * 文件:		system.h
 * 描述:		定义一些系统相关的函数。
 * 作者:		wu.xiongxing					
 * 邮箱:		wxxweb@gmail.com
 * 日期:		2014-03-28
 ******************************************************************************/

#ifndef __W2X_COMMON_SYSTEM_H__
#define __W2X_COMMON_SYSTEM_H__


#include "exports.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(sys)


/**
 * 函数 LoadLibrary 和函数 GetProcAddress 的组合调用，
 * 使用完成后请务必调用函数 FreeLibrary 来释放资源。
 */
W2X_COMMON_API FARPROC GetLibraryProcAddress(
	OUT HMODULE* _lib_handle_ptr,
	LPCTSTR _lib_name, 
	LPCSTR _proc_name
);

W2X_COMMON_API bool IsOcxRegistered(CLSID _cls_id);

W2X_COMMON_API bool RegisterOcx(LPCTSTR _ocx_path);

W2X_COMMON_API bool UnregisterOcx(LPCTSTR _ocx_path);

W2X_DEFINE_NAME_SPACE_END(sys)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_SYSTEM_H__ */
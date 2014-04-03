/*******************************************************************************
 * �ļ�:		system.h
 * ����:		����һЩϵͳ��صĺ�����
 * ����:		wu.xiongxing					
 * ����:		wxxweb@gmail.com
 * ����:		2014-03-28
 ******************************************************************************/

#ifndef __W2X_COMMON_SYSTEM_H__
#define __W2X_COMMON_SYSTEM_H__


#include "exports.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(sys)


/**
 * ���� LoadLibrary �ͺ��� GetProcAddress ����ϵ��ã�
 * ʹ����ɺ�����ص��ú��� FreeLibrary ���ͷ���Դ��
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
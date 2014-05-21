/******************************************************************************
�ļ�:	file_sys.h
����:	�������ļ�ϵͳ��صĹ��ܡ�
����:	wu.xiongxing
����:	wxxweb@gmail.com
����:	2014-05-21
*******************************************************************************/

#ifndef __W2X_COMMON_FILE_SYS_H__
#define __W2X_COMMON_FILE_SYS_H__

#include "exports.h"
#include "macros.h"
#include <WTypes.h>

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(file)

/** �ļ�ö�ٴ������ص���������ָ��, ����true��������, ����falseֹͣ������ */
typedef bool (CALLBACK *FileEnumHandler)(
	LPCTSTR pszDirPath,		///< �ļ�����Ŀ¼ȫ·��
	LPCTSTR pszFileName,	///< ������·�����ļ���
	DWORD dwFileAttributes,	///< ��Ӧ WIN32_FIND_DATA �ṹ��Ա dwFileAttributes
	PVOID pParam			///< TraverseDirectory �� pParam ����
);

/** ����Ŀ¼, ����ÿ���ļ�������Ŀ¼��Ӧ���ļ�ö�ٴ�������*/
W2X_COMMON_API bool TraverseDirectory(
	LPCTSTR pszDirPath,			///< ��������Ŀ¼ȫ·��
	FileEnumHandler pEnumProc,	///< �ļ�ö�ٴ���ص�����
	PVOID pParam = NULL,		///< �����������Ĳ���
	UINT dwHierarchy = 0		///< �������㼶, 0Ϊ������
	);

W2X_DEFINE_NAME_SPACE_END(file)
W2X_NAME_SPACE_END

#endif ///< !__W2X_COMMON_FILE_SYS_H__
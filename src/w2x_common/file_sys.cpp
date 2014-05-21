/*******************************************************************************
�ļ�:	file_sys.cpp
����:	�μ�file_sys.h
����:	wu.xiongxing
����:	wxxweb@gmail.com
����:	2014-05-21
�޸�:
*******************************************************************************/

#include "stdafx.h"
#include "file_sys.h"

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(file)


W2X_COMMON_API bool TraverseDirectory(LPCTSTR pszDirPath, 
	FileEnumHandler pEnumProc, 
	PVOID pParam,
	UINT nHierarchy)
{
	if (NULL == pszDirPath || NULL == pEnumProc)
	{
		ASSERT(false);
		return false;
	}

	if (FALSE == ::PathIsDirectory(pszDirPath))
	{
		ASSERT(false);
		return false;
	}

	size_t nLen = _tcslen(pszDirPath);
	TCHAR szDirPath[MAX_PATH] = TEXT("");
	TCHAR szFindPath[MAX_PATH] = TEXT("");
	_tcscpy_s(szDirPath, MAX_PATH, pszDirPath);
	_tcscpy_s(szFindPath, MAX_PATH, pszDirPath);
	_tcscpy_s(szFindPath + nLen, MAX_PATH - nLen, TEXT("\\*.*"));

	WIN32_FIND_DATA wfd = {0};
	HANDLE hFind = ::FindFirstFile(szFindPath, &wfd);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		ASSERT(false);
		return false;
	}
	do {
		if (FILE_ATTRIBUTE_DIRECTORY == wfd.dwFileAttributes &&
			(0 == _tcscmp(wfd.cFileName, TEXT("."))
			|| 0 == _tcscmp(wfd.cFileName, TEXT(".."))))
		{
			continue;
		}

		if (false == 
			(*pEnumProc)(szDirPath, wfd.cFileName, wfd.dwFileAttributes, pParam))
		{
			::FindClose(hFind);
			return false;
		}

		if (FILE_ATTRIBUTE_DIRECTORY == wfd.dwFileAttributes)
		{
			if (0 != nHierarchy && 1 >= nHierarchy)
			{
				continue;
			}

			_tcscpy_s(szFindPath + nLen + 1, MAX_PATH - nLen - 1, wfd.cFileName);
			if (false == 
				TraverseDirectory(szFindPath, pEnumProc, pParam, nHierarchy - 1))
			{
				::FindClose(hFind);
				return false;
			}
		}
	} 
	while (FALSE != ::FindNextFile(hFind, &wfd));

	::FindClose(hFind);
	return true;
}


W2X_DEFINE_NAME_SPACE_END(file)
W2X_NAME_SPACE_END
/*******************************************************************************
�ļ�:	file_sys.cpp
����:	�μ�file_sys.h
����:	wu.xiongxing
����:	wxxweb@gmail.com
����:	2014-05-21
�޸�:	2014-05-21
*******************************************************************************/

#include "stdafx.h"
#include "file_sys.h"

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(file)


W2X_COMMON_API bool TraverseDirectory(
	LPCTSTR pszDirPath, 
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


W2X_COMMON_API bool EnsureDirectoryExists(LPCTSTR _path)
{
	if (NULL == _path)
	{
		ASSERT(false);
		return false;
	}

	size_t offset = 0;
	TCHAR ch = TEXT('\0');
	TCHAR sub_folder[MAX_PATH] = TEXT("");

	for (size_t i = 0; i < MAX_PATH; ++i)
	{
		ch = _path[i];
		if (TEXT('\\') != ch && TEXT('/') != ch && TEXT('\0') != ch) {
			continue;
		}
		
		_tcsncpy_s(
			sub_folder + offset, MAX_PATH - offset,
			_path + offset, i - offset + 1);
		offset = i + 1;

		if (TRUE == ::PathFileExists(sub_folder)) {
			if (TEXT('\0') == ch) {
				break;
			}
			continue;
		}
		if (FALSE == ::CreateDirectory(sub_folder, NULL)) {
			return false;
		}
		if (TEXT('\0') == ch) {
			break;
		}
	}

	return true;
}


W2X_COMMON_API bool RemoveDirectoryRecursively(LPCTSTR _path)
{
	if (NULL == _path)
	{
		ASSERT(false);
		return false;
	}

	if (FALSE == ::PathIsDirectory(_path))
	{
		ASSERT(false);
		return false;
	}

	const size_t org_len = _tcslen(_path);
	TCHAR find_path[MAX_PATH] = TEXT("");
	_tcscpy_s(find_path, MAX_PATH, _path);
	_tcscpy_s(find_path + org_len, MAX_PATH - org_len, TEXT("\\*.*"));

	WIN32_FIND_DATA wfd = {0};
	HANDLE finder = ::FindFirstFile(find_path, &wfd);
	if (INVALID_HANDLE_VALUE == finder)
	{
		ASSERT(false);
		return false;
	}

	bool successed = true;
	do {
		if (FILE_ATTRIBUTE_DIRECTORY == wfd.dwFileAttributes
			&& (0 == _tcscmp(wfd.cFileName, TEXT(".")) ||
			    0 == _tcscmp(wfd.cFileName, TEXT(".."))))
		{
			continue;
		}

		_tcscpy_s(find_path + org_len + 1,
			MAX_PATH - org_len - 1, wfd.cFileName);
		if (FILE_ATTRIBUTE_DIRECTORY == wfd.dwFileAttributes)
		{
			successed = RemoveDirectoryRecursively(find_path);
		}
		else
		{
			successed = (FALSE != ::DeleteFile(find_path));
			ASSERT(successed);
		}
	} 
	while (false != successed && FALSE != ::FindNextFile(finder, &wfd));

	::FindClose(finder);

	if (false != successed)
	{
		successed = (FALSE != ::RemoveDirectory(_path));
		ASSERT(successed);
	}
	return successed;
}


W2X_COMMON_API LPCTSTR GetWorkingDirectoryPath(void)
{
	static TCHAR s_szDir[MAX_PATH] = TEXT("");
	if (TEXT('\0') != s_szDir[0])
	{
		return s_szDir;
	}

	::GetCurrentDirectory(MAX_PATH, s_szDir);
	_tcscat_s(s_szDir, TEXT("\\"));

	return s_szDir;
}


W2X_COMMON_API LPCTSTR GetModuleDirectoryPath(void)
{
	static TCHAR s_szDir[MAX_PATH] = TEXT("");
	if (TEXT('\0') != s_szDir[0])
	{
		return s_szDir;
	}

	::GetModuleFileName(NULL, s_szDir, MAX_PATH);
	::PathRemoveFileSpec(s_szDir);
	_tcscat_s(s_szDir, TEXT("\\"));

	return s_szDir;
}


W2X_DEFINE_NAME_SPACE_END(file)
W2X_NAME_SPACE_END
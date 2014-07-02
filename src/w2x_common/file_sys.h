/******************************************************************************
文件:	file_sys.h
描述:	定义与文件系统相关的功能。
作者:	wu.xiongxing
邮箱:	wxxweb@gmail.com
日期:	2014-05-21
*******************************************************************************/

#ifndef __W2X_COMMON_FILE_SYS_H__
#define __W2X_COMMON_FILE_SYS_H__

#include "macros.h"
#include <WTypes.h>

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(file)


/** 文件枚举处理器回调函数类型指针, 返回true继续遍历, 返回false停止遍历。 */
typedef bool (CALLBACK *FileEnumHandler)(
	LPCTSTR pszDirPath,		///< 文件所在目录全路径
	LPCTSTR pszFileName,	///< 不包含路径的文件名
	DWORD dwFileAttributes,	///< 对应 WIN32_FIND_DATA 结构成员 dwFileAttributes
	PVOID pParam			///< TraverseDirectory 的 pParam 参数
);

/** 遍历目录, 并对每个文件（包括目录）应用文件枚举处理函数。*/
W2X_COMMON_API bool TraverseDirectory(
	LPCTSTR pszDirPath,			///< 被遍历的目录全路径
	FileEnumHandler pEnumProc,	///< 文件枚举处理回调函数
	PVOID pParam = NULL,		///< 传给处理函数的参数
	UINT dwHierarchy = 0		///< 最大遍历层级, 0为无限制
	);

/** 
 * 获取主程序当前工作目录路径，路径字符串末尾包含字符 ‘\’。
 * @note 首次调用，内部通过系统 API GetCurrentDirectory 来获取当前工作目录路径并缓存下来，
 * 下次调用直接返回缓存值。因此，尽量在主程序入口处调用该函数，以免受 SetCurrentDirectory 影响。
 */
W2X_COMMON_API LPCTSTR GetWorkingDirectoryPath(void);

/** 
 * 获取主程序模块所在目录路径，路径字符串末尾包含字符 ‘\’。
 * @note 首次调用，内部通过系统 API GetModuleFileName 来获取主程序模块所在目录路径并缓存下来，
 * 下次调用直接返回缓存值。之前有发现因调用 GetModuleFileName 导致的死锁。
 */
W2X_COMMON_API LPCTSTR GetModuleDirectoryPath(void);


W2X_DEFINE_NAME_SPACE_END(file)
W2X_NAME_SPACE_END

#endif ///< !__W2X_COMMON_FILE_SYS_H__
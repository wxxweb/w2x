/*******************************************************************************
文件:	process.h
描述:	该文件中定义了进程相关的一些帮助器，如互斥、同步等.
作者:	wu.xiongxing					
邮箱:	wxxweb@gmail.com
日期:	2014-03-27
修改:	2014-05-21
*******************************************************************************/


#ifndef __W2X_COMMON_PROCESS_H__
#define __W2X_COMMON_PROCESS_H__

#include "macros.h"

W2X_NAME_SPACE_BEGIN


/**
 * 初始化用于进程间同步的命名互斥体，它使用 GUID 做为其命名的组成部分。
 * 该互斥体建立在全局命名空间中，可用于实现程序只允许运行一个实例。
 * 可以调用函数 UninitProcessMutex 提前释放该互斥体，或者等到程序退出后自动释放。
 * 如果已经有一个进程持有该命名互斥体返回 false，否则返回 true。
 */
W2X_COMMON_API bool InitProcessMutex(GUID _guid);

/** 释放调用函数 InitProcessMutex 创建的全局命名互斥体。 */
W2X_COMMON_API void UninitProcessMutex(void);

/** 获取调用函数 InitProcessMutex 创建的全局命名互斥体的名称。 */
W2X_COMMON_API LPCTSTR GetProcessMutex(void);

/** 通过进程 ID 获取进程的名称（执行文件名称） */
W2X_COMMON_API DWORD GetProcessName(
	OUT LPTSTR _buffer,
	DWORD _size_in_words,
	DWORD _process_id,
	bool _full_file_name
	);

/** 获取父进程 ID */
W2X_COMMON_API DWORD GetParentProcessId(DWORD _process_id);

/** 生成某一时刻的进程列表快照，并对其进行操作 */
class W2X_COMMON_API CProcessSnapshot
{
public:
	/** 存储快照中的单个进程信息 */
	struct ProcessInfo {
		DWORD   _process_id;
		DWORD   _thread_count;
		DWORD   _parent_process_id;
		LONG    _base_thread_priority;	///< Base priority of process's threads
		TCHAR   _exe_file_name[MAX_PATH];
	};

public:
	/** 对象构造时不创建进程列表快照，可调用方法 CreateSnapshot 来创建 */
	CProcessSnapshot(void);
	~CProcessSnapshot(void);

private:
	CProcessSnapshot(const CProcessSnapshot&);
	void operator =(const CProcessSnapshot&);

public:
	/** 创建当前时刻的系统进程列表快照 */
	bool CreateSnapshot(void);

	/** 获取进程列表快照中的进程数 */
	UINT GetProcessCount(void) const;

	/** 根据索引获取进程列表快照中的进程信息 */
	bool GetProcessInfo(OUT ProcessInfo* _info, UINT _index) const;

	/** 根据进程 ID 在进程列表快照查找相应进程信息，_info 可为 NULL */
	bool FindProcess(OUT ProcessInfo* _info, UINT _id) const;

	/** \brief 根据可执行文件的名称在进程列表快照中查找相应的进程信息，可能有多个同名进程.
	 * 
	 * @param _info_array [out] ProcessInfo 数组，用于存放匹配到的多个进程信息.
	 * @param _array_length [in][out] 传入 _info_array 数组长度，传出最终保存的进程信息数.
	 * @param _process_name 进程的可执行文件名称，根据此名称来查找进程，可能有多个同名进程.
	 * @return 返回最终找到的进程数.
	 * @note 当 _info_array 或 _array_length 均传入 NULL 时，可用于查找匹配的进程数.
	 */
	UINT FindProcesses(
		OUT ProcessInfo* _info_array,
		IN OUT UINT* _array_length,
		LPCTSTR _exe_file_name
		) const;

private:
	class CImpl;
	CImpl* const m_impl;
};


W2X_NAME_SPACE_END

#endif /* __W2X_COMMON_PROCESS_H__ */
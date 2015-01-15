/*******************************************************************************
�ļ�:	process.h
����:	���ļ��ж����˽�����ص�һЩ���������绥�⡢ͬ����.
����:	wu.xiongxing					
����:	wxxweb@gmail.com
����:	2014-03-27
�޸�:	2014-05-21
*******************************************************************************/


#ifndef __W2X_COMMON_PROCESS_H__
#define __W2X_COMMON_PROCESS_H__

#include "macros.h"

W2X_NAME_SPACE_BEGIN


/**
 * ��ʼ�����ڽ��̼�ͬ�������������壬��ʹ�� GUID ��Ϊ����������ɲ��֡�
 * �û����彨����ȫ�������ռ��У�������ʵ�ֳ���ֻ��������һ��ʵ����
 * ���Ե��ú��� UninitProcessMutex ��ǰ�ͷŸû����壬���ߵȵ������˳����Զ��ͷš�
 * ����Ѿ���һ�����̳��и����������巵�� false�����򷵻� true��
 */
W2X_COMMON_API bool InitProcessMutex(GUID _guid);

/** �ͷŵ��ú��� InitProcessMutex ������ȫ�����������塣 */
W2X_COMMON_API void UninitProcessMutex(void);

/** ��ȡ���ú��� InitProcessMutex ������ȫ����������������ơ� */
W2X_COMMON_API LPCTSTR GetProcessMutex(void);

/** ͨ������ ID ��ȡ���̵����ƣ�ִ���ļ����ƣ� */
W2X_COMMON_API DWORD GetProcessName(
	OUT LPTSTR _buffer,
	DWORD _size_in_words,
	DWORD _process_id,
	bool _full_file_name
	);

/** ��ȡ������ ID */
W2X_COMMON_API DWORD GetParentProcessId(DWORD _process_id);

/** ����ĳһʱ�̵Ľ����б���գ���������в��� */
class W2X_COMMON_API CProcessSnapshot
{
public:
	/** �洢�����еĵ���������Ϣ */
	struct ProcessInfo {
		DWORD   _process_id;
		DWORD   _thread_count;
		DWORD   _parent_process_id;
		LONG    _base_thread_priority;	///< Base priority of process's threads
		TCHAR   _exe_file_name[MAX_PATH];
	};

public:
	/** ������ʱ�����������б���գ��ɵ��÷��� CreateSnapshot ������ */
	CProcessSnapshot(void);
	~CProcessSnapshot(void);

private:
	CProcessSnapshot(const CProcessSnapshot&);
	void operator =(const CProcessSnapshot&);

public:
	/** ������ǰʱ�̵�ϵͳ�����б���� */
	bool CreateSnapshot(void);

	/** ��ȡ�����б�����еĽ����� */
	UINT GetProcessCount(void) const;

	/** ����������ȡ�����б�����еĽ�����Ϣ */
	bool GetProcessInfo(OUT ProcessInfo* _info, UINT _index) const;

	/** ���ݽ��� ID �ڽ����б���ղ�����Ӧ������Ϣ��_info ��Ϊ NULL */
	bool FindProcess(OUT ProcessInfo* _info, UINT _id) const;

	/** \brief ���ݿ�ִ���ļ��������ڽ����б�����в�����Ӧ�Ľ�����Ϣ�������ж��ͬ������.
	 * 
	 * @param _info_array [out] ProcessInfo ���飬���ڴ��ƥ�䵽�Ķ��������Ϣ.
	 * @param _array_length [in][out] ���� _info_array ���鳤�ȣ��������ձ���Ľ�����Ϣ��.
	 * @param _process_name ���̵Ŀ�ִ���ļ����ƣ����ݴ����������ҽ��̣������ж��ͬ������.
	 * @return ���������ҵ��Ľ�����.
	 * @note �� _info_array �� _array_length ������ NULL ʱ�������ڲ���ƥ��Ľ�����.
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
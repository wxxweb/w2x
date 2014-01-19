/*****************************************************************************
文件名：		Log.cpp
描述：		参见Log.h
作者：		吴雄兴					
版权：		www.utooo.com
日期：		2013.12.24
修改历史：
******************************************************************************/

#include "stdafx.h"
#include "msg_loop.h"
#include "thread.h"
#include "log.h"

#ifndef TSTDSTR
#  ifdef UNICODE
#  define TSTDSTR std::wstring
#  else
#  define TSTDSTR std::string
#  endif
#endif

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(log)

static const EPriority DEFAULT_LOG_PRIORITY = kPriorityNormal;
static const ECategory DEFAULT_LOG_CATEGORY = kCategoryInfo;
static TCHAR const DEFAULT_LOG_DIR_NAME[]	= TEXT("default");
static TCHAR const STR_PRIORITY_NORMAL[]	= TEXT("normal");
static TCHAR const STR_PRIORITY_URGENT[]	= TEXT("urgent");
static TCHAR const STR_PRIORITY_HIGH[]		= TEXT("high");
static TCHAR const STR_PRIORITY_LOW[]		= TEXT("low");
static TCHAR const STR_CATEGORY_INFO[]		= TEXT("info");
static TCHAR const STR_CATEGORY_ERROR[]		= TEXT("error");
static TCHAR const STR_CATEGORY_WARN[]		= TEXT("warn");
static TCHAR const STR_CATEGORY_DEBUG[]		= TEXT("debug");
static TCHAR const STR_LOG_LINE_END[]		= TEXT("\r\n");
static BYTE  const BYTE_LOG_FILE_HEAD[]		= {0xFF, 0xFE};

//----------------------------------------------------------------------------
// 检查文件名是否合法, 规则如下: 
// 1.不超过255个字符;
// 2.不包含/\:*?"<>|这些非法字符;
//----------------------------------------------------------------------------
inline bool IsFileNameValid(LPCTSTR _file_name)
{
	if (NULL == _file_name)
	{
		return false;
	}
	TSTDSTR file_name(_file_name);
	return (file_name.size() <= 255) 
		&& (TSTDSTR::npos == file_name.find_first_of(TEXT("/\\:*?\"<>|")));
}

//============================================================================
// 日志系统实现类.
//============================================================================
class CLog
{
public:
	enum {
		EXPIRED_DAY = 7,					// 日志过期天数
		MAX_LOG_HEAD = 64,					// 单条日志开头的缓冲区最大字符数
		MAX_LOG_INFO = MAX_LOG_INFO,		// 单条日志信息的缓冲区最大字符数
		MAX_STARTUP_COUNT = 3,				// 保存启动计数的缓冲区最大字符数
		SIZE_FILE_HEAD = 2,					// 日志文件头部字节数, 文件头部用于标识文件编码类型, 如UNICODE
	};

	typedef std::map<DirId, TSTDSTR> DirMap;

public:
	CLog(void);
	~CLog(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CLog)

public:
	static CLog& GetInstance(void);
	bool Initialize(void);
	void Uninitialize(void);
	bool SetGlobal(const Custom& _custom_ref);
	void GetGlobal(Custom& _custom_ref);
	const Custom& GetGlobal(void);
	void ResetGlobal(void);
	bool GetRootDir(LPTSTR _dir_path, size_t _size_in_words);
	DirId AddWorkDir(LPCTSTR _dir_name);
	bool GetLogFile(LPTSTR _str_buffer, size_t _size_in_words, DirId _work_dir_id);
	bool Log(const Custom* _custom_ptr, LPCTSTR _format, va_list& _arg_list_ref);

	/* 帮助函数 */
public:
	static bool ParseDateString(SYSTEMTIME* _date_ptr, LPCTSTR _date_str);

private:
	bool EnsureLogFile(LPCTSTR _file_path);
	bool WriteLogFile(LPCTSTR _file_path, LPCTSTR _log_record);
	bool InitStartupInfo(void);
	bool RemoveExpiredLogFiles(void);
	bool RemoveDir(LPCTSTR _dir_path);
	LPCTSTR GetRootDir(void);
	LPCTSTR GetTodayDir(void);
	LPCTSTR GetModuleName(void);

	LPCTSTR GetWorkDir(LPTSTR _str_buffer, 
					   size_t _size_in_words, 
					   DirId _dir_id);
	
	static bool CALLBACK HandleMsg(PVOID _handler_param, LPCTSTR _msg, PVOID _msg_param);

private:
W2X_IMPLEMENT_LOCKING(CLog, CAutoLock)

private:
	static CCriticalSection sm_critical_section;	// 用于对类成员函数操作加锁解锁

private:
	bool	m_is_init_startup_info;		// 记录是否已经初始化程序启动信息
	DWORD	m_process_id;
	TCHAR	m_log_root_dir[MAX_PATH];	// 保存日志根目录全路径, 不包含最后"\"
	TCHAR	m_today_dir[MAX_PATH];		// 保存当天日志目录全路径, 不包含最后"\"
	TCHAR	m_module_name[MAX_PATH];	// 主程序模块名称
	WORD	m_today_num;				// 当天日份, 比如5月2日, 则该值为2
	UINT	m_startup_count;			// 当天主程序启动计数
	UINT	m_log_record_count;			// 日志记录条数计数
	DirMap	m_work_dir_name_map;		// 存放日志读写工作目录全路径, 不包含最后"\"
	Custom	m_global_custom;			// 全局日志定制信息, 应用于每条做特殊定制的日志
	CMsgLoop m_msg_loop;				// 日志消息循环处理对象
};

::w2x::CCriticalSection CLog::sm_critical_section;

//----------------------------------------------------------------------------
// 以下为日志模块实现类的构造和析构函数.
//----------------------------------------------------------------------------
CLog::CLog(void)
	: m_is_init_startup_info(false)
	, m_today_num(0)
	, m_startup_count(0)
	, m_log_record_count(0)
{
	m_process_id = ::GetCurrentProcessId();
	::memset(m_log_root_dir, 0, sizeof(m_log_root_dir));
	::memset(m_today_dir, 0, sizeof(m_today_dir));
	::memset(m_module_name, 0, sizeof(m_module_name));
	this->ResetGlobal();
	this->Initialize();
}

CLog::~CLog(void)
{
	this->Uninitialize();
}

//----------------------------------------------------------------------------
// 获取日子模块实现类对象, 当对象不存在时创建之.
//----------------------------------------------------------------------------
CLog& CLog::GetInstance(void)
{
	static CLog* s_pInstance = NULL;

	// 使用DCLP(Double Checked Locking Pattern)保证线程安全
	if (NULL == s_pInstance)
	{
		sm_critical_section.Enter();
		if (NULL == s_pInstance)
		{
			// 日志对象在程序运行期间一直使用，
			// 无需delete，交给操作系统最终销毁吧
			s_pInstance = new CLog();
			//::atexit(UninitializeLog);
		}
		sm_critical_section.Leave();
	}

	return *s_pInstance;
}

// 为了较早创建日志模块对象, 不要在其他析构函数里使用s_rLog.
static CLog& s_rLog = CLog::GetInstance();

//----------------------------------------------------------------------------
// 初始化日志模块, 确保日志相关目录、文件被创建, 并缓存它们的全路径等信息.
//----------------------------------------------------------------------------
bool CLog::Initialize(void)
{
	this->RemoveExpiredLogFiles();
	this->GetTodayDir();
	m_msg_loop.StartLoopThread(CLog::HandleMsg, static_cast<PVOID>(this));
	
	return true;
}

//----------------------------------------------------------------------------
// 释放日志模块所获取的资源, 如关闭文件等.
//----------------------------------------------------------------------------
void CLog::Uninitialize(void)
{
	// 不主动停止日志循环, 让它打完
	m_msg_loop.StopLoopThread();
}

//----------------------------------------------------------------------------
// 设置全局日志定制信息, 未做特殊定制的日志均采用默认定制.
//----------------------------------------------------------------------------
bool CLog::SetGlobal(const Custom& _custom_ref)
{
	CAutoLock autoLock(this);

	if (0 < _custom_ref.work_dir_id)
	{
		DirMap::iterator it = m_work_dir_name_map.find(_custom_ref.work_dir_id);
		if (m_work_dir_name_map.end() == it)
		{
			return false;
		}
	}

	m_global_custom = _custom_ref;

	return true;
}

//----------------------------------------------------------------------------
// 获取当前的全局日志定制信息.
//----------------------------------------------------------------------------
void CLog::GetGlobal(Custom& _custom_ref)
{
	CAutoLock autoLock(this);
	_custom_ref = m_global_custom;
}

const Custom& CLog::GetGlobal(void)
{
	return m_global_custom;
}

//----------------------------------------------------------------------------
// 重置全局日志信息为最初状态.
//----------------------------------------------------------------------------
void CLog::ResetGlobal(void)
{
	m_global_custom.priority = DEFAULT_LOG_PRIORITY;
	m_global_custom.category = DEFAULT_LOG_CATEGORY;
	m_global_custom.work_dir_id = 0;
	m_global_custom.is_resue_file = false;
}

//----------------------------------------------------------------------------
// 添加读写日志的工作目录, dir_id最小值为1, 0为默认工作目录.
//----------------------------------------------------------------------------
DirId CLog::AddWorkDir(LPCTSTR _dir_name)
{
	if (NULL == _dir_name)
	{
		ASSERT(false);
		return false;
	}

	TSTDSTR strDirName = _dir_name;
	std::transform(strDirName.begin(), strDirName.end(), strDirName.begin(), _totlower);
	if (0 == strDirName.compare(DEFAULT_LOG_DIR_NAME))
	{
		return 0;
	}

	for (DirMap::iterator it = m_work_dir_name_map.begin();
		 m_work_dir_name_map.end() != it; ++it)
	{
		TSTDSTR& strName = it->second;
		if (0 == strDirName.compare(strName))
		{
			return 0;
		}
	}

	if (FALSE == IsFileNameValid(_dir_name))
	{
		ASSERT(false);
		return false;
	}

	m_work_dir_name_map[m_work_dir_name_map.size()+1] = _dir_name;

	return true;
}

bool CLog::GetRootDir(LPTSTR _dir_path, size_t _size_in_words)
{
	if (NULL == _dir_path)
	{
		ASSERT(false);
		return false;
	}
	_tcscpy_s(_dir_path, _size_in_words, this->GetRootDir());
	return true;
}

//----------------------------------------------------------------------------
// 当 _custom_ptr=NULL 时, 该条日志采用默认定制, 否则使用特殊定制
//----------------------------------------------------------------------------
bool CLog::Log(const Custom* _custom_ptr, LPCTSTR _format_str, va_list& _arg_list_ref)
{
	const Custom& custom_ref = (NULL != _custom_ptr) ? *_custom_ptr : s_rLog.GetGlobal();

#ifndef _DEBUG
	if (kCategoryDebug == custom_ref.category)
	{
		// Release模式下不打印调试日志
		return true;
	}
#endif

	LPCTSTR pszPriority = NULL;
	switch (custom_ref.priority)
	{
	case kPriorityNormal:	pszPriority = STR_PRIORITY_NORMAL;	break;
	case kPriorityUrgent:	pszPriority = STR_PRIORITY_URGENT;	break;
	case kPriorityHigh:	pszPriority = STR_PRIORITY_HIGH;	break;
	case kPriorityLow:	pszPriority = STR_PRIORITY_LOW;		break;
	default: ASSERT(false);		pszPriority = STR_PRIORITY_NORMAL;	break;
	}

	LPCTSTR pszCategory = NULL;
	switch (custom_ref.category)
	{
	case kCategoryInfo:		pszCategory = STR_CATEGORY_INFO;	break;
	case kCategoryWarn:		pszCategory = STR_CATEGORY_WARN;	break;
	case kCategoryError:	pszCategory = STR_CATEGORY_ERROR;	break;
	case kCategoryDebug:	pszCategory = STR_CATEGORY_DEBUG;	break;
	default: ASSERT(false);	pszCategory = STR_CATEGORY_INFO;	break;
	}

	TCHAR szBuffer[MAX_LOG_HEAD + MAX_LOG_INFO] = {0};
	SYSTEMTIME st = {0};
	::GetLocalTime(&st);

	//------------------------------------------------------------------
	// 日志格式示例:
	//------------------------------------------------------------------
	// 序号	优先级    类型   时间				内容
	//------------------------------------------------------------------
	// [1	normal	info	21:32:23.321]	this is a normal info. 
	// [2	urgent	error	22:42:21.001]	this is a urgent error.
	//------------------------------------------------------------------
	
	int nWritten = _stprintf_s(szBuffer, MAX_LOG_HEAD - 1, 
		TEXT("[%d\t%s\t%s\t%02d:%02d:%02d.%03d]\t"),
		++m_log_record_count, pszPriority, pszCategory,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	if (-1 == nWritten)
	{
		ASSERT(false);
		nWritten = 0;
	}

	// 不使用 wvsprintf(), 因为adb 输出 usage() 信息太多会导致越界崩溃.
	::StringCbVPrintf(szBuffer + nWritten,
		(sizeof(szBuffer) - nWritten * sizeof(TCHAR)), _format_str, _arg_list_ref);

#ifdef _DEBUG
	::OutputDebugString(TEXT("\n"));
	::OutputDebugString(szBuffer);
#endif

	m_msg_loop.AddMsg(szBuffer, reinterpret_cast<PVOID>(custom_ref.work_dir_id));

// #ifdef _DEBUG
// 	if (kCategoryWarn == custom_ref.category)
// 	{
// 		TCHAR szTitle[MAX_LOG_HEAD] = {0};
// 		_stprintf_s(szTitle, _countof(szTitle) - 1, 
// 			TEXT("%s - %s"), pszCategory, pszPriority);
// 		::MessageBox(NULL, szBuffer + nWritten, szTitle, MB_ICONWARNING);
// 	}
// #endif /* _DEBUG */

	return 0 >= nWritten;
}

LPCTSTR CLog::GetModuleName(void)
{
	if (TEXT('\0') != m_module_name[0])
	{
		return m_module_name;
	}

	TCHAR szFileName[MAX_PATH] = {0};
	::GetModuleFileName(NULL, szFileName, MAX_PATH);
	LPTSTR pszModuleName = ::PathFindFileName(szFileName);
	::PathRemoveExtension(pszModuleName);
	::_tcscpy_s(m_module_name, _countof(m_module_name), pszModuleName);

	return m_module_name;
}

LPCTSTR CLog::GetWorkDir(LPTSTR _str_buffer, 
							 size_t _size_in_words, 
							 DirId _dir_id)
{
	if (NULL == _str_buffer)
	{
		ASSERT(false);
		return NULL;
	}

	LPCTSTR pszWorkDirName = NULL;
	if (0 == _dir_id)
	{
		pszWorkDirName = DEFAULT_LOG_DIR_NAME;
	}
	else
	{
		CAutoLock autoLock(this);

		DirMap::iterator it = m_work_dir_name_map.find(_dir_id);
		if (m_work_dir_name_map.end() == it)
		{
			ASSERT(false);
			pszWorkDirName = DEFAULT_LOG_DIR_NAME;
		}
		else
		{
			pszWorkDirName = it->second.c_str();
		}
	}

	const int nWritten = _stprintf_s(_str_buffer, _size_in_words - 1, TEXT("%s\\%s"),
		this->GetTodayDir(), pszWorkDirName);

	BOOL bExist = TRUE;
	for (int i = 0; FALSE == ::PathFileExists(_str_buffer) && i < 2; ++i)
	{
		bExist = ::CreateDirectory(_str_buffer, NULL);
	}
	if (FALSE == bExist)
	{
		DWORD dwError = ::GetLastError();
		TCHAR szInfo[MAX_PATH] = {0};
		_stprintf_s(szInfo, _countof(szInfo) - 1,
			TEXT("The log work directory (%s) does not exist and create failed(%d)."),
			pszWorkDirName, dwError);
		::MessageBox(NULL, szInfo, TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return NULL;
	}

	return _str_buffer;
}

bool CLog::EnsureLogFile(LPCTSTR _file_path)
{
	if (NULL == _file_path)
	{
		ASSERT(false);
		return false;
	}

	if (TRUE == ::PathFileExists(_file_path))
	{
		return false;
	}

	CAutoLock autoLock(this);

	// 创建日志文件
	HANDLE hFile = NULL;
	for (int i = 0; FALSE == ::PathFileExists(_file_path) && i < 2; ++i)
	{
		hFile = ::CreateFile(_file_path, GENERIC_WRITE, FILE_SHARE_READ,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	if (NULL == hFile || INVALID_HANDLE_VALUE == hFile)
	{
		DWORD dwError = ::GetLastError();
		TCHAR szInfo[MAX_PATH] = {0};
		_stprintf_s(szInfo, _countof(szInfo) - 1,
			TEXT("The log file (%s) does not exist and create failed(%d)."),
			_file_path, dwError);
		::MessageBox(NULL, szInfo, TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return false;
	}

	// 写文件头
	DWORD dwNumberOfBytesWritten = 0;
	BOOL bWritten = ::WriteFile(hFile, static_cast<LPCVOID>(&BYTE_LOG_FILE_HEAD), 
		sizeof(BYTE_LOG_FILE_HEAD), &dwNumberOfBytesWritten, NULL);
	if (FALSE == bWritten || sizeof(BYTE_LOG_FILE_HEAD) != dwNumberOfBytesWritten)
	{
		DWORD dwError = ::GetLastError();
		TCHAR szInfo[MAX_PATH] = {0};
		_stprintf_s(szInfo, _countof(szInfo) - 1,
			TEXT("Write log file (%s) head failed(%d)."), _file_path, dwError);
		::MessageBox(NULL, szInfo, TEXT("ERROR"), MB_OK | MB_ICONERROR);
	}

	::CloseHandle(hFile);
	hFile = NULL;

	return true;
}

bool CLog::WriteLogFile(LPCTSTR _file_path, LPCTSTR _log_record)
{
	if (NULL == _file_path || NULL == _log_record)
	{
		ASSERT(false);
		return false;
	}

	this->EnsureLogFile(_file_path);

	CAutoLock autoLock(this);

	SECURITY_ATTRIBUTES sa = {0};
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	HANDLE hFile = ::CreateFile(_file_path, GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (NULL == hFile || INVALID_HANDLE_VALUE == hFile)
	{
		ASSERT(false);
		return false;
	}

	DWORD dwBytesWritten = 0;
	DWORD dwBytesToWrite = _tcslen(_log_record) * sizeof(TCHAR);
	::SetFilePointer(hFile, 0, NULL, FILE_END);
	::WriteFile(hFile, _log_record, dwBytesToWrite, &dwBytesWritten, NULL);
	::SetFilePointer(hFile, 0, NULL, FILE_END);
	dwBytesToWrite = _tcslen(STR_LOG_LINE_END) * sizeof(TCHAR);
	::WriteFile(hFile, STR_LOG_LINE_END, dwBytesToWrite, &dwBytesWritten, NULL);

	::CloseHandle(hFile);
	hFile = NULL;

	return true;
}

bool CLog::InitStartupInfo(void)
{
	CAutoLock autoLock(this);
	if (true == m_is_init_startup_info)
	{
		return false;
	}
	m_is_init_startup_info = true;

	LPCTSTR pszTodayDir = this->GetTodayDir();
	if (NULL == pszTodayDir)
	{
		ASSERT(false);
		return false;
	}

	TCHAR szStartupFile[MAX_PATH] = {0};
	_stprintf_s(szStartupFile, _countof(szStartupFile) - 1, 
		TEXT("%s\\startup.log"), pszTodayDir);

	this->EnsureLogFile(szStartupFile);

	SECURITY_ATTRIBUTES sa = {0};
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	HANDLE hFile = ::CreateFile(szStartupFile, GENERIC_READ, 
		FILE_SHARE_READ, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (NULL == hFile || INVALID_HANDLE_VALUE == hFile)
	{
		ASSERT(false);
		return false;
	}

	TCHAR szStartupCount[MAX_STARTUP_COUNT + 1] = {0};
	const DWORD dwBytesToRead = MAX_STARTUP_COUNT * sizeof(TCHAR);
	const DWORD dwBytesOffset = dwBytesToRead + _tcslen(STR_LOG_LINE_END) * sizeof(TCHAR);
	DWORD dwBytesRead = 0;
	
	const DWORD dwSetFilePtr = ::SetFilePointer(
		hFile, (-1 * static_cast<LONG>(dwBytesOffset)), NULL, FILE_END);

	::ReadFile(hFile, &szStartupCount, dwBytesToRead, &dwBytesRead, NULL);
	m_startup_count = static_cast<UINT>(_ttoi(szStartupCount));

	::CloseHandle(hFile);
	hFile = NULL;

	bool bSuccessed = true;
	if (false == m_global_custom.is_resue_file)
	{
		// 启动次数递增
		if (999 < ++m_startup_count)
		{
			m_startup_count = 999;
		}
		SYSTEMTIME st = {0};
		::GetLocalTime(&st);
		TCHAR szLine[MAX_LOG_INFO] = {0};
		_stprintf_s(szLine, _countof(szLine) - 1, 
			TEXT("%04d-%02d-%02d %02d:%02d:%02d.%03d %03d"), 
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, 
			st.wSecond, st.wMilliseconds, m_startup_count);

		bSuccessed = this->WriteLogFile(szStartupFile, szLine);
	}

	return bSuccessed;
}

bool CLog::ParseDateString(SYSTEMTIME* _date_ptr, LPCTSTR _date_str)
{
	if (NULL == _date_ptr || NULL == _date_str)
	{
		ASSERT(false);
		return false;
	}

	const size_t MAX_BUF_COUNT = 16;
	const size_t nLen = _tcslen(_date_str);
	if (MAX_BUF_COUNT <= nLen)
	{
		ASSERT(false);
		return false;
	}

	TCHAR szDate[MAX_BUF_COUNT] = {0};
	_tcscpy_s(szDate, _countof(szDate), _date_str);
	LPTSTR pszBegin = szDate;

	bool bSuccessed = true;
	memset(_date_ptr, 0, sizeof(SYSTEMTIME));
	for (WORD wTime = 0, i = 0; i < 3/*年月日*/; ++i)
	{
		LPTSTR pszEnd = _tcschr(pszBegin, TEXT('-'));
		if (NULL == pszEnd) {
			pszEnd = szDate + nLen;
		}
		else {
			*pszEnd = TEXT('\0');
		}

		wTime = static_cast<WORD>(_ttoi(pszBegin));
		if (0 == wTime)
		{
			bSuccessed = false;
			break;
		}
		switch (i)
		{
		case 0: _date_ptr->wYear  = wTime; break;
		case 1: _date_ptr->wMonth = wTime; break;
		case 2: _date_ptr->wDay   = wTime; break;
		default: break;
		}
		pszBegin = pszEnd + 1;
		if (i < 2 && pszBegin > szDate + nLen)
		{
			bSuccessed = false;
			break;
		}
	}
	if (false == bSuccessed || 12 < _date_ptr->wMonth || 31 < _date_ptr->wDay)
	{
		return false;
	}
	return true;
}

bool CLog::RemoveExpiredLogFiles(void)
{
	CAutoLock autoLock(this);

	LPCTSTR pszRootDir = this->GetRootDir();
	if (NULL == pszRootDir)
	{
		ASSERT(false);
		return false;
	}

	TCHAR szPath[MAX_PATH] = {0};
	_stprintf_s(szPath, _countof(szPath) - 1, TEXT("%s\\*.*"), pszRootDir);

	WIN32_FIND_DATA wfd = {0};
	HANDLE hFind = ::FindFirstFile(szPath, &wfd);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		ASSERT(false);
		return false;
	}

	// 获取当天日期时间
	FILETIME fileTime = {0};
	SYSTEMTIME sysTime = {0};
	SYSTEMTIME localTime = {0};
	ULARGE_INTEGER ulgTime = {0};
	::GetLocalTime(&localTime);
	::TzSpecificLocalTimeToSystemTime(NULL, &localTime, &sysTime);
	::SystemTimeToFileTime(&sysTime, &fileTime);
	memcpy_s(&ulgTime, sizeof(ULARGE_INTEGER), &fileTime, sizeof(FILETIME));
	const ULONGLONG ullToday = ulgTime.QuadPart / 10000000;

	//TCHAR szFileName[MAX_PATH] = {0};

	do {
		if (FILE_ATTRIBUTE_DIRECTORY != wfd.dwFileAttributes ||
			0 == _tcscmp(wfd.cFileName, TEXT(".")) ||
			0 == _tcscmp(wfd.cFileName, TEXT("..")))
		{
			continue;
		}

		if (false == CLog::ParseDateString(&localTime, wfd.cFileName))
		{
			continue;
		}
		
		// 检测是否过期
		::TzSpecificLocalTimeToSystemTime(NULL, &localTime, &sysTime);
		::SystemTimeToFileTime(&sysTime, &fileTime);
		memcpy_s(&ulgTime, sizeof(ULARGE_INTEGER), &fileTime, sizeof(FILETIME));
		const ULONGLONG ullTime = ulgTime.QuadPart / 10000000;
		if (ullTime < ullToday)
		{
			DWORD dwDay = static_cast<DWORD>((ullToday-ullTime)/3600/24);
			if (EXPIRED_DAY > dwDay)
			{
				continue;
			}
		}

		// 删除超过一周的日志文件
		_stprintf_s(szPath, 
			_countof(szPath) - 1, TEXT("%s\\%s"), pszRootDir, wfd.cFileName);
		this->RemoveDir(szPath);
	} 
	while (FALSE != ::FindNextFile(hFind, &wfd));

	::FindClose(hFind);
	hFind= NULL;
	return true;
}

bool CLog::RemoveDir(LPCTSTR _dir_path)
{
	if (NULL == _dir_path)
	{
		ASSERT(false);
		return false;
	}
	if (FALSE == ::PathIsDirectory(_dir_path))
	{
		return false;
	}

	TCHAR szPath[MAX_PATH] = {0};
	_stprintf_s(szPath, _countof(szPath) - 1, TEXT("%s\\*.*"), _dir_path);

	WIN32_FIND_DATA wfd = {0};
	HANDLE hFind = ::FindFirstFile(szPath, &wfd);
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
		else
		{
			TCHAR szSubPath[MAX_PATH] = {0};
			_stprintf_s(szSubPath, _countof(szSubPath) - 1, 
				TEXT("%s\\%s"), _dir_path, wfd.cFileName);

			if (FILE_ATTRIBUTE_DIRECTORY == wfd.dwFileAttributes)
			{
				this->RemoveDir(szSubPath);
			}
			else {
				::SetFileAttributes(szSubPath, FILE_ATTRIBUTE_NORMAL);
				::DeleteFile(szSubPath);
			}
		}
	} 
	while (FALSE != ::FindNextFile(hFind, &wfd));

	::FindClose(hFind);
	hFind= NULL;

	::SetFileAttributes(_dir_path, FILE_ATTRIBUTE_NORMAL);
	return TRUE == ::RemoveDirectory(_dir_path);
}

//----------------------------------------------------------------------------
// 确保日志根目录存在, 当检查到不存在的时候创建之, 并缓存其全路径.
//----------------------------------------------------------------------------
LPCTSTR CLog::GetRootDir(void)
{
	if (TEXT('\0') != m_log_root_dir[0] && TRUE == ::PathFileExists(m_log_root_dir))
	{
		return m_log_root_dir;
	}

	// 采用 DCLP（双重检查锁定模式）保证线程安全
	CAutoLock autoLock(this);
	if (TEXT('\0') != m_log_root_dir[0] && TRUE == ::PathFileExists(m_log_root_dir))
	{
		return m_log_root_dir;
	}

	::GetModuleFileName(NULL, m_log_root_dir, MAX_PATH);
	::PathRemoveFileSpec(m_log_root_dir);
	::_tcscat_s(m_log_root_dir, 
		sizeof(m_log_root_dir) / sizeof(TCHAR), TEXT("\\log"));

	BOOL bDirExists = TRUE;
	for (int i = 0; FALSE == ::PathFileExists(m_log_root_dir) && i < 2; ++i)
	{
		bDirExists = ::CreateDirectory(m_log_root_dir, NULL);
	}

	if (FALSE == bDirExists)
	{
		::MessageBox(NULL, 
			TEXT("The log root directory does not exist and create failed."), 
			TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return NULL;
	}
	
	return m_log_root_dir;
}

//----------------------------------------------------------------------------
// 获取今日的日志子目录, 当检查到不存在的时候创建之, 并缓存其全路径.
//----------------------------------------------------------------------------
LPCTSTR CLog::GetTodayDir(void)
{
	if (TEXT('\0') != m_today_dir && TRUE == ::PathFileExists(m_today_dir))
	{
		// 检查是否已经进入第二天, 若是第二天就要重新创建当天目录
		SYSTEMTIME st = {0};
		::GetLocalTime(&st);
		if (st.wDay == m_today_num)
		{
			return m_today_dir;
		}
	}

	// 采用 DCLP（双重检查锁定模式）保证线程安全
	CAutoLock autoLock(this);
	SYSTEMTIME st = {0};
	::GetLocalTime(&st);
	if (TEXT('\0') != m_today_dir && TRUE == ::PathFileExists(m_today_dir))
	{
		if (st.wDay == m_today_num)
		{
			return m_today_dir;
		}
	}

	LPCTSTR pszRootDir = this->GetRootDir();
	if (NULL == pszRootDir)
	{
		ASSERT(false);
		return NULL;
	}

	m_today_num = st.wDay;
	_stprintf_s(m_today_dir, _countof(m_today_dir) - 1, 
		TEXT("%s\\%04d-%02d-%02d"), pszRootDir, st.wYear, st.wMonth, st.wDay);

	BOOL bDirExists = TRUE;
	for (int i = 0; FALSE == ::PathFileExists(m_today_dir) && i < 2; ++i)
	{
		bDirExists = ::CreateDirectory(m_today_dir, NULL);
	}

	if (FALSE == bDirExists)
	{
		::MessageBox(NULL, 
			TEXT("The log today directory does not exist and create failed."), 
			TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return NULL;
	}

	return m_today_dir;
}

//----------------------------------------------------------------------------
// 确保日志文件存在, 当检查到不存在的时候创建之, 并缓存其全路径.
//----------------------------------------------------------------------------
bool CLog::GetLogFile(LPTSTR _str_buffer, 
						  size_t _size_in_words, 
						  DirId _work_dir_id)
{
	if (NULL == _str_buffer)
	{
		ASSERT(false);
		return false;
	}

	CAutoLock autoLock(this);

	TCHAR szWorkDir[MAX_PATH] = {0};
	LPCTSTR pszWorkDir = this->GetWorkDir(szWorkDir, _countof(szWorkDir), _work_dir_id);
	if (NULL == pszWorkDir)
	{
		ASSERT(false);
		return false;
	}

	if (false == m_is_init_startup_info)
	{
		this->InitStartupInfo();
	}

	_stprintf_s(_str_buffer, _size_in_words - 1, TEXT("%s\\%s_%s_%d.log"), 
		pszWorkDir, this->GetModuleName(), 
		::PathFindFileName(this->GetTodayDir()), m_startup_count);
// 	if (TRUE == ::PathFileExists(str_buffer))
// 	{
// 		return true;
// 	}

	//return this->EnsureLogFile(str_buffer);
	return true;
}

//----------------------------------------------------------------------------
// 日志消息处理函数, 被日志消息循环线程调用.
// 实现向日志文件中异步地写入日志.
//----------------------------------------------------------------------------
bool CALLBACK CLog::HandleMsg(PVOID _handler_param, LPCTSTR _msg, PVOID _msg_param)
{
	if (NULL == _handler_param)
	{
		ASSERT(false);
		return false;
	}

	CLog* pThis = static_cast<CLog*>(_handler_param);
	DirId work_dir_id = reinterpret_cast<DirId>(_msg_param);

	TCHAR szFile[MAX_PATH] = {0};

	// 打到其他工作目录的日志同时也会打到默认工作目录中
	if (true == pThis->GetLogFile(szFile, MAX_PATH, 0))
	{
		pThis->WriteLogFile(szFile, _msg);
	}
	if (0 != work_dir_id && true == pThis->GetLogFile(szFile, MAX_PATH, work_dir_id))
	{
		pThis->WriteLogFile(szFile, _msg);
	}

	return true;
}

//----------------------------------------------------------------------------
// 日志系统包装函数.
//----------------------------------------------------------------------------
bool SetGlobal(const Custom& _custom_ref)
{
	return s_rLog.SetGlobal(_custom_ref);
}

void GetGlobal(Custom& custom_ref)
{
	s_rLog.GetGlobal(custom_ref);
}

void ResetGlobal(void)
{
	s_rLog.ResetGlobal();
}

bool GetRootDir(LPTSTR _dir_path, size_t _size_in_words)
{
	return s_rLog.GetRootDir(_dir_path, _size_in_words);
}

DirId AddWorkDir(LPCTSTR _dir_name)
{
	return s_rLog.AddWorkDir(_dir_name);
}

bool GetLogFile(LPTSTR _file_path, size_t _size_in_words, DirId _work_dir_id)
{
	return s_rLog.GetLogFile(_file_path, _size_in_words, _work_dir_id);
}

DWORD GetLogFileHeaderSize(void)
{
	return sizeof(BYTE_LOG_FILE_HEAD);
}

bool GetLogFileHeader(PBYTE _byte_buffer, DWORD& _size_in_bytes)
{
	if (NULL == _byte_buffer || sizeof(BYTE_LOG_FILE_HEAD) > _size_in_bytes)
	{
		ASSERT(false);
		return false;
	}
	_size_in_bytes = sizeof(BYTE_LOG_FILE_HEAD);
	memcpy(_byte_buffer, BYTE_LOG_FILE_HEAD, _size_in_bytes);
	return true;
}

bool Log(const Custom* _custom_ptr, LPCTSTR _format_str, ...)
{
	IF_NULL_ASSERT_RETURN_VALUE(_format_str, false);

	va_list args;
	va_start(args, _format_str);
	bool bRetVal = s_rLog.Log(_custom_ptr, _format_str, args);
	va_end(args);

	return bRetVal;
}

bool Log(const Custom* _custom_ptr, LPCTSTR _format_str, va_list& _arg_list_ref)
{
	if (NULL == _format_str)
	{
		ASSERT(false);
		return false;
	}

	return s_rLog.Log(_custom_ptr, _format_str, _arg_list_ref);
}

bool LogInfo(LPCTSTR _format_str, ...)
{
	if (NULL == _format_str)
	{
		ASSERT(false);
		return false;
	}

	Custom custom;
	custom.category = kCategoryInfo;

	va_list args;
	va_start(args, _format_str);
	bool bRetVal = s_rLog.Log(&custom, _format_str, args);
	va_end(args);

	return bRetVal;
}

bool LogWarn(LPCTSTR _format_str, ...)
{
	if (NULL == _format_str)
	{
		ASSERT(false);
		return false;
	}

	Custom custom;
	custom.category = kCategoryWarn;

	va_list args;
	va_start(args, _format_str);
	bool bRetVal = s_rLog.Log(&custom, _format_str, args);
	va_end(args);

	return bRetVal;
}


bool LogError(LPCTSTR _format_str, ...)
{
	if (NULL == _format_str)
	{
		ASSERT(false);
		return false;
	}

	Custom custom;
	custom.category = kCategoryError;

	va_list args;
	va_start(args, _format_str);
	bool bRetVal = s_rLog.Log(&custom, _format_str, args);
	va_end(args);

	return bRetVal;
}

bool LogDebug(LPCTSTR _format_str, ...)
{
#ifndef _DEBUG
	return true;
#endif

	if (NULL == _format_str)
	{
		ASSERT(false);
		return false;
	}

	Custom custom;
	custom.category = kCategoryDebug;

	va_list args;
	va_start(args, _format_str);
	bool bRetVal = s_rLog.Log(&custom, _format_str, args);
	va_end(args);

	return bRetVal;
}

bool ParseDateString(SYSTEMTIME* _date_ptr, LPCTSTR _date_str)
{
	return s_rLog.ParseDateString(_date_ptr, _date_str);
}

W2X_DEFINE_NAME_SPACE_END(log)
W2X_NAME_SPACE_END
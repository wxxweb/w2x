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
#include "mutex.h"
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

static TCHAR const DEFAULT_LOG_DIR_NAME[]	= TEXT("default");
static TCHAR const STR_CATEGORY_INFO[]		= TEXT("info");
static TCHAR const STR_CATEGORY_ERROR[]		= TEXT("error");
static TCHAR const STR_CATEGORY_WARN[]		= TEXT("warn");
static TCHAR const STR_CATEGORY_DEBUG[]		= TEXT("debug");
static TCHAR const STR_LOG_LINE_END[]		= TEXT("\r\n");
static BYTE  const BYTE_LOG_FILE_HEAD[]		= {0xFF, 0xFE};

W2X_COMMON_API bool Log(
	const Custom* _custom_ptr, 
	LPCTSTR _format_str, 
	va_list& _arg_list_ref
);

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
class CLogImpl
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
	CLogImpl(void);
	~CLogImpl(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CLogImpl)
W2X_IMPLEMENT_LOCKING(CLogImpl, CAutoLock)

public:
	static CLogImpl& GetInstance(void);
	inline bool Initialize(void);
	inline void Uninitialize(void);
	inline bool SetGlobal(const Custom& _custom_ref);
	inline void GetGlobal(Custom& _custom_ref);
	inline const Custom& GetGlobal(void);
	inline void ResetGlobal(void);
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

	// 日志消息处理器
	static bool CALLBACK HandleMsg(
		PVOID _handler_param, 
		LPTSTR _msg, 
		size_t _bytes,
		PVOID _msg_param
	);

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

W2X_IMPLEMENT_LOCKING_CLASS(CLogImpl, CAutoLock)

//----------------------------------------------------------------------------
// 以下为日志模块实现类的构造和析构函数.
//----------------------------------------------------------------------------
CLogImpl::CLogImpl(void)
	: m_is_init_startup_info(false)
	, m_today_num(0)
	, m_startup_count(0)
	, m_log_record_count(0)
{
	debug::EnableExcptionHandle();
	m_process_id = ::GetCurrentProcessId();
	::memset(m_log_root_dir, 0, sizeof(m_log_root_dir));
	::memset(m_today_dir, 0, sizeof(m_today_dir));
	::memset(m_module_name, 0, sizeof(m_module_name));
	this->ResetGlobal();
	this->Initialize();
}

CLogImpl::~CLogImpl(void)
{
	this->Uninitialize();
}

//----------------------------------------------------------------------------
// 获取日子模块实现类对象, 当对象不存在时创建之.
//----------------------------------------------------------------------------
CLogImpl& CLogImpl::GetInstance(void)
{
	static std::auto_ptr<CLogImpl> sm_auto_this_ptr;

	// 使用DCLP(Double Checked Locking Pattern)保证线程安全
	if (NULL == sm_auto_this_ptr.get())
	{
		CLogImpl::sm_mutex_of_class_by_CAutoLock.Lock();
		if (NULL == sm_auto_this_ptr.get())
		{
			// 日志对象在程序运行期间一直使用，
			// 无需delete，交给操作系统最终销毁吧
			sm_auto_this_ptr.reset(new CLogImpl());
		}
		CLogImpl::sm_mutex_of_class_by_CAutoLock.Unlock();
	}

	return *sm_auto_this_ptr.get();
}

// 为了较早创建日志模块对象, 不要在其他析构函数里使用s_log_impl_ref.
//static CLogImpl& s_log_impl_ref = CLogImpl::GetInstance();

//----------------------------------------------------------------------------
// 初始化日志模块, 确保日志相关目录、文件被创建, 并缓存它们的全路径等信息.
//----------------------------------------------------------------------------
inline bool CLogImpl::Initialize(void)
{
	this->RemoveExpiredLogFiles();
	this->GetTodayDir();
	m_msg_loop.StartLoopThread(CLogImpl::HandleMsg, static_cast<PVOID>(this));
	
	return true;
}

//----------------------------------------------------------------------------
// 释放日志模块所获取的资源, 如关闭文件等.
//----------------------------------------------------------------------------
inline void CLogImpl::Uninitialize(void)
{
	// 不主动停止日志循环, 让它打完
	m_msg_loop.StopLoopThread();
}

//----------------------------------------------------------------------------
// 设置全局日志定制信息, 未做特殊定制的日志均采用默认定制.
//----------------------------------------------------------------------------
inline bool CLogImpl::SetGlobal(const Custom& _custom_ref)
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
inline void CLogImpl::GetGlobal(Custom& _custom_ref)
{
	_custom_ref = m_global_custom;
}

inline const Custom& CLogImpl::GetGlobal(void)
{
	return m_global_custom;
}

//----------------------------------------------------------------------------
// 重置全局日志信息为最初状态.
//----------------------------------------------------------------------------
inline void CLogImpl::ResetGlobal(void)
{
	m_global_custom = Custom();
}

//----------------------------------------------------------------------------
// 添加读写日志的工作目录, dir_id最小值为1, 0为默认工作目录.
//----------------------------------------------------------------------------
DirId CLogImpl::AddWorkDir(LPCTSTR _dir_name)
{
	if (NULL == _dir_name)
	{
		ASSERT(false);
		return false;
	}

	TSTDSTR dir_name = _dir_name;
	std::transform(dir_name.begin(), dir_name.end(), dir_name.begin(), _totlower);
	if (0 == dir_name.compare(DEFAULT_LOG_DIR_NAME))
	{
		return 0;
	}

	for (DirMap::iterator it = m_work_dir_name_map.begin();
		 m_work_dir_name_map.end() != it; ++it)
	{
		TSTDSTR& dir_name_ref = it->second;
		if (0 == dir_name.compare(dir_name_ref))
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

inline bool CLogImpl::GetRootDir(LPTSTR _dir_path, size_t _size_in_words)
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
bool CLogImpl::Log(const Custom* _custom_ptr, LPCTSTR _format_str, va_list& _arg_list_ref)
{
	const Custom& custom_ref = (NULL != _custom_ptr) ? *_custom_ptr : m_global_custom;

#ifndef _DEBUG
	if (kCategoryDebug == custom_ref.category)
	{
		// Release模式下不打印调试日志
		return true;
	}
#endif

	LPCTSTR category = NULL;
	switch (custom_ref.category)
	{
	case kCategoryInfo:		category = STR_CATEGORY_INFO;	break;
	case kCategoryWarn:		category = STR_CATEGORY_WARN;	break;
	case kCategoryError:	category = STR_CATEGORY_ERROR;	break;
	case kCategoryDebug:	category = STR_CATEGORY_DEBUG;	break;
	default: ASSERT(false);	category = STR_CATEGORY_INFO;	break;
	}

	TCHAR log_buffer[MAX_LOG_HEAD + MAX_LOG_INFO] = {0};
	SYSTEMTIME st = {0};
	::GetLocalTime(&st);

	//------------------------------------------------------------------
	// 日志格式示例:
	//------------------------------------------------------------------
	// 序号	类型   时间				内容
	//------------------------------------------------------------------
	// [1	info	21:32:23.321]	this is a normal info. 
	// [2	error	22:42:21.001]	this is a urgent error.
	//------------------------------------------------------------------
	
	int chars_written = _stprintf_s(log_buffer, MAX_LOG_HEAD - 1, 
		TEXT("[%d\t%s\t%02d:%02d:%02d.%03d]\t"),
		++m_log_record_count, category,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	if (-1 == chars_written)
	{
		ASSERT(false);
		chars_written = 0;
	}

	// 不使用 wvsprintf(), 因为adb 输出 usage() 信息太多会导致越界崩溃.
	::StringCbVPrintf(
		log_buffer + chars_written,
		(sizeof(log_buffer) - chars_written * sizeof(TCHAR)), 
		_format_str, _arg_list_ref);

#ifdef _DEBUG
	::OutputDebugString(TEXT("\n"));
	::OutputDebugString(log_buffer);
	if (kCategoryError == custom_ref.category) {
		::MessageBeep(MB_ICONERROR);
		::MessageBox(NULL, log_buffer, TEXT("Log Error"), MB_ICONERROR);
	}
	else if (kCategoryWarn == custom_ref.category) {
		::MessageBeep(MB_ICONWARNING);
	}
#endif /* _DEBUG */

	PVOID msg_param = reinterpret_cast<PVOID>(custom_ref.work_dir_id);
	if (false == custom_ref.is_immediately)
	{
		m_msg_loop.AddMsg(log_buffer, 0, msg_param);
	}
	else {
		CLogImpl::HandleMsg(static_cast<PVOID>(this), log_buffer, 0, msg_param);
	}

	return 0 >= chars_written;
}

inline LPCTSTR CLogImpl::GetModuleName(void)
{
	if (TEXT('\0') != m_module_name[0])
	{
		return m_module_name;
	}

	TCHAR file_name[MAX_PATH] = {0};
	::GetModuleFileName(NULL, file_name, MAX_PATH);
	LPTSTR module_name = ::PathFindFileName(file_name);
	::PathRemoveExtension(module_name);
	::_tcscpy_s(m_module_name, _countof(m_module_name), module_name);

	return m_module_name;
}

LPCTSTR CLogImpl::GetWorkDir(LPTSTR _str_buffer, 
							 size_t _size_in_words, 
							 DirId _dir_id)
{
	if (NULL == _str_buffer)
	{
		ASSERT(false);
		return NULL;
	}

	LPCTSTR work_dir_name = NULL;
	if (0 == _dir_id)
	{
		work_dir_name = DEFAULT_LOG_DIR_NAME;
	}
	else
	{
		CAutoLock autoLock(this);

		DirMap::iterator it = m_work_dir_name_map.find(_dir_id);
		if (m_work_dir_name_map.end() == it)
		{
			ASSERT(false);
			work_dir_name = DEFAULT_LOG_DIR_NAME;
		}
		else
		{
			work_dir_name = it->second.c_str();
		}
	}

	_stprintf_s(_str_buffer, _size_in_words - 1, 
		TEXT("%s\\%s"), this->GetTodayDir(), work_dir_name);

	BOOL is_dir_exist = TRUE;
	for (int i = 0; FALSE == ::PathFileExists(_str_buffer) && i < 2; ++i)
	{
		is_dir_exist = ::CreateDirectory(_str_buffer, NULL);
	}
	if (FALSE == is_dir_exist)
	{
		DWORD last_error = ::GetLastError();
		TCHAR error_info[MAX_PATH] = {0};
		_stprintf_s(error_info, _countof(error_info) - 1,
			TEXT("The log work directory (%s) does not exist and create failed(%d)."),
			work_dir_name, last_error);
		::MessageBox(NULL, error_info, TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return NULL;
	}

	return _str_buffer;
}

bool CLogImpl::EnsureLogFile(LPCTSTR _file_path)
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
	HANDLE file_handle = NULL;
	for (int i = 0; FALSE == ::PathFileExists(_file_path) && i < 2; ++i)
	{
		file_handle = ::CreateFile(_file_path, GENERIC_WRITE, FILE_SHARE_READ,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	if (NULL == file_handle || INVALID_HANDLE_VALUE == file_handle)
	{
		DWORD last_error = ::GetLastError();
		TCHAR error_info[MAX_PATH] = {0};
		_stprintf_s(error_info, _countof(error_info) - 1,
			TEXT("The log file (%s) does not exist and create failed(%d)."),
			_file_path, last_error);
		::MessageBox(NULL, error_info, TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return false;
	}

	// 写文件头
	DWORD num_of_bytes_written = 0;
	BOOL is_written = ::WriteFile(
		file_handle, static_cast<LPCVOID>(&BYTE_LOG_FILE_HEAD), 
		sizeof(BYTE_LOG_FILE_HEAD), &num_of_bytes_written, NULL);
	if (FALSE == is_written || sizeof(BYTE_LOG_FILE_HEAD) != num_of_bytes_written)
	{
		DWORD last_error = ::GetLastError();
		TCHAR error_info[MAX_PATH] = {0};
		_stprintf_s(error_info, _countof(error_info) - 1,
			TEXT("Write log file (%s) head failed(%d)."), _file_path, last_error);
		::MessageBox(NULL, error_info, TEXT("ERROR"), MB_OK | MB_ICONERROR);
	}

	::CloseHandle(file_handle);
	file_handle = NULL;

	::OutputDebugString(TEXT("\nCreate log file: "));
	::OutputDebugString(_file_path);
	::OutputDebugString(TEXT("\n"));

	return true;
}

bool CLogImpl::WriteLogFile(LPCTSTR _file_path, LPCTSTR _log_record)
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
	HANDLE file_handle = ::CreateFile(_file_path, GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (NULL == file_handle || INVALID_HANDLE_VALUE == file_handle)
	{
		ASSERT(false);
		return false;
	}

	DWORD bytes_written = 0;
	DWORD bytes_to_write = _tcslen(_log_record) * sizeof(TCHAR);
	::SetFilePointer(file_handle, 0, NULL, FILE_END);
	::WriteFile(file_handle, _log_record, bytes_to_write, &bytes_written, NULL);
	::SetFilePointer(file_handle, 0, NULL, FILE_END);
	bytes_to_write = _tcslen(STR_LOG_LINE_END) * sizeof(TCHAR);
	::WriteFile(file_handle, STR_LOG_LINE_END, bytes_to_write, &bytes_written, NULL);

	::CloseHandle(file_handle);
	file_handle = NULL;

	return true;
}

bool CLogImpl::InitStartupInfo(void)
{
	if (true == m_is_init_startup_info)
	{
		return false;
	}
	m_is_init_startup_info = true;

	LPCTSTR today_dir = this->GetTodayDir();
	if (NULL == today_dir)
	{
		ASSERT(false);
		return false;
	}

	TCHAR startup_file_path[MAX_PATH] = {0};
	_stprintf_s(startup_file_path, _countof(startup_file_path) - 1, 
		TEXT("%s\\startup.log"), today_dir);

	this->EnsureLogFile(startup_file_path);

	SECURITY_ATTRIBUTES sa = {0};
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	HANDLE file_handle = ::CreateFile(startup_file_path, GENERIC_READ, 
		FILE_SHARE_READ, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (NULL == file_handle || INVALID_HANDLE_VALUE == file_handle)
	{
		ASSERT(false);
		return false;
	}

	TCHAR startup_count[MAX_STARTUP_COUNT + 1] = {0};
	const DWORD bytes_to_read = MAX_STARTUP_COUNT * sizeof(TCHAR);
	const DWORD bytes_offset 
		= bytes_to_read + _tcslen(STR_LOG_LINE_END) * sizeof(TCHAR);
	DWORD bytes_read = 0;
	
	/*const DWORD file_pointer = */::SetFilePointer(
		file_handle, (-1 * static_cast<LONG>(bytes_offset)), NULL, FILE_END);

	::ReadFile(file_handle, &startup_count, bytes_to_read, &bytes_read, NULL);
	m_startup_count = static_cast<UINT>(_ttoi(startup_count));

	::CloseHandle(file_handle);
	file_handle = NULL;

	bool is_write_successed = true;
	if (false == m_global_custom.is_reuse_file)
	{
		// 启动次数递增
		if (999 < ++m_startup_count)
		{
			m_startup_count = 999;
		}
		SYSTEMTIME st = {0};
		::GetLocalTime(&st);
		TCHAR log_record[MAX_LOG_INFO] = {0};
		_stprintf_s(log_record, _countof(log_record) - 1, 
			TEXT("%04d-%02d-%02d %02d:%02d:%02d.%03d %03d"), 
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, 
			st.wSecond, st.wMilliseconds, m_startup_count);

		is_write_successed = this->WriteLogFile(startup_file_path, log_record);
	}

	return is_write_successed;
}

bool CLogImpl::ParseDateString(SYSTEMTIME* _date_ptr, LPCTSTR _date_str)
{
	if (NULL == _date_ptr || NULL == _date_str)
	{
		ASSERT(false);
		return false;
	}

	const size_t MAX_BUF_COUNT = 16;
	const size_t date_str_len = _tcslen(_date_str);
	if (MAX_BUF_COUNT <= date_str_len)
	{
		ASSERT(false);
		return false;
	}

	TCHAR date_str_buffer[MAX_BUF_COUNT] = {0};
	_tcscpy_s(date_str_buffer, _countof(date_str_buffer), _date_str);
	LPTSTR num_begin = date_str_buffer;

	bool is_date_num = true;
	memset(_date_ptr, 0, sizeof(SYSTEMTIME));
	for (WORD date_num = 0, i = 0; i < 3/*年月日*/; ++i)
	{
		LPTSTR num_end = _tcschr(num_begin, TEXT('-'));
		if (NULL == num_end) {
			num_end = date_str_buffer + date_str_len;
		}
		else {
			*num_end = TEXT('\0');
		}

		date_num = static_cast<WORD>(_ttoi(num_begin));
		if (0 == date_num)
		{
			is_date_num = false;
			break;
		}
		switch (i)
		{
		case 0: _date_ptr->wYear  = date_num; break;
		case 1: _date_ptr->wMonth = date_num; break;
		case 2: _date_ptr->wDay   = date_num; break;
		default: break;
		}
		num_begin = num_end + 1;
		if (i < 2 && num_begin > date_str_buffer + date_str_len)
		{
			is_date_num = false;
			break;
		}
	}
	if (false == is_date_num || 12 < _date_ptr->wMonth || 31 < _date_ptr->wDay)
	{
		return false;
	}
	return true;
}

bool CLogImpl::RemoveExpiredLogFiles(void)
{
	CAutoLock autoLock(this);

	LPCTSTR root_dir_path = this->GetRootDir();
	if (NULL == root_dir_path)
	{
		ASSERT(false);
		return false;
	}

	TCHAR find_path[MAX_PATH] = {0};
	_stprintf_s(find_path, _countof(find_path) - 1, TEXT("%s\\*.*"), root_dir_path);

	WIN32_FIND_DATA wfd = {0};
	HANDLE find_handle = ::FindFirstFile(find_path, &wfd);
	if (INVALID_HANDLE_VALUE == find_handle)
	{
		ASSERT(false);
		return false;
	}

	// 获取当天日期时间
	FILETIME file_time = {0};
	SYSTEMTIME sys_time = {0};
	SYSTEMTIME local_time = {0};
	ULARGE_INTEGER large_time = {0};
	::GetLocalTime(&local_time);
	::TzSpecificLocalTimeToSystemTime(NULL, &local_time, &sys_time);
	::SystemTimeToFileTime(&sys_time, &file_time);
	memcpy_s(&large_time, sizeof(ULARGE_INTEGER), &file_time, sizeof(FILETIME));
	const ULONGLONG today_since_1970 = large_time.QuadPart / 10000000;

	do {
		if (FILE_ATTRIBUTE_DIRECTORY != wfd.dwFileAttributes ||
			0 == _tcscmp(wfd.cFileName, TEXT(".")) ||
			0 == _tcscmp(wfd.cFileName, TEXT("..")))
		{
			continue;
		}

		if (false == CLogImpl::ParseDateString(&local_time, wfd.cFileName))
		{
			continue;
		}
		
		// 检测是否过期
		::TzSpecificLocalTimeToSystemTime(NULL, &local_time, &sys_time);
		::SystemTimeToFileTime(&sys_time, &file_time);
		memcpy_s(&large_time, sizeof(ULARGE_INTEGER), &file_time, sizeof(FILETIME));
		const ULONGLONG second_since_1970 = large_time.QuadPart / 10000000;
		if (second_since_1970 < today_since_1970)
		{
			DWORD diff_day = static_cast<DWORD>(
				(today_since_1970 - second_since_1970) / 3600 / 24);
			if (EXPIRED_DAY > diff_day)
			{
				continue;
			}
		}

		// 删除超过一周的日志文件
		_stprintf_s(find_path, _countof(find_path) - 1, 
			TEXT("%s\\%s"), root_dir_path, wfd.cFileName);
		this->RemoveDir(find_path);
	} 
	while (FALSE != ::FindNextFile(find_handle, &wfd));

	::FindClose(find_handle);
	find_handle= NULL;
	return true;
}

bool CLogImpl::RemoveDir(LPCTSTR _dir_path)
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

	TCHAR find_path[MAX_PATH] = {0};
	_stprintf_s(find_path, _countof(find_path) - 1, TEXT("%s\\*.*"), _dir_path);

	WIN32_FIND_DATA wfd = {0};
	HANDLE find_handle = ::FindFirstFile(find_path, &wfd);
	if (INVALID_HANDLE_VALUE == find_handle)
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
			TCHAR sub_path[MAX_PATH] = {0};
			_stprintf_s(sub_path, _countof(sub_path) - 1, 
				TEXT("%s\\%s"), _dir_path, wfd.cFileName);

			if (FILE_ATTRIBUTE_DIRECTORY == wfd.dwFileAttributes)
			{
				this->RemoveDir(sub_path);
			}
			else {
				::SetFileAttributes(sub_path, FILE_ATTRIBUTE_NORMAL);
				::DeleteFile(sub_path);
			}
		}
	} 
	while (FALSE != ::FindNextFile(find_handle, &wfd));

	::FindClose(find_handle);
	find_handle= NULL;

	::SetFileAttributes(_dir_path, FILE_ATTRIBUTE_NORMAL);
	return TRUE == ::RemoveDirectory(_dir_path);
}

//----------------------------------------------------------------------------
// 确保日志根目录存在, 当检查到不存在的时候创建之, 并缓存其全路径.
//----------------------------------------------------------------------------
LPCTSTR CLogImpl::GetRootDir(void)
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

	::GetCurrentDirectory(MAX_PATH, m_log_root_dir);
	::_tcscat_s(m_log_root_dir, TEXT("\\log"));

	BOOL is_dir_exist = TRUE;
	for (int i = 0; FALSE == ::PathFileExists(m_log_root_dir) && i < 2; ++i)
	{
		is_dir_exist = ::CreateDirectory(m_log_root_dir, NULL);
	}

	if (FALSE == is_dir_exist)
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
LPCTSTR CLogImpl::GetTodayDir(void)
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

	LPCTSTR root_dir_path = this->GetRootDir();
	if (NULL == root_dir_path)
	{
		ASSERT(false);
		return NULL;
	}

	m_today_num = st.wDay;
	_stprintf_s(m_today_dir, _countof(m_today_dir) - 1, 
		TEXT("%s\\%04d-%02d-%02d"), root_dir_path, st.wYear, st.wMonth, st.wDay);

	BOOL is_dir_exist = TRUE;
	for (int i = 0; FALSE == ::PathFileExists(m_today_dir) && i < 2; ++i)
	{
		is_dir_exist = ::CreateDirectory(m_today_dir, NULL);
	}

	if (FALSE == is_dir_exist)
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
bool CLogImpl::GetLogFile(LPTSTR _str_buffer, 
						  size_t _size_in_words, 
						  DirId _work_dir_id)
{
	if (NULL == _str_buffer)
	{
		ASSERT(false);
		return false;
	}

	TCHAR work_dir_path[MAX_PATH] = {0};
	if (NULL == this->GetWorkDir(
		work_dir_path, _countof(work_dir_path), _work_dir_id))
	{
		ASSERT(false);
		return false;
	}

	if (false == m_is_init_startup_info)
	{
		this->InitStartupInfo();
	}

	_stprintf_s(_str_buffer, _size_in_words - 1, TEXT("%s\\%s_%s_%d.log"), 
		work_dir_path, this->GetModuleName(), 
		::PathFindFileName(this->GetTodayDir()), m_startup_count);

	return true;
}

//----------------------------------------------------------------------------
// 日志消息处理函数, 被日志消息循环线程调用.
// 实现向日志文件中异步地写入日志.
//----------------------------------------------------------------------------
bool CALLBACK CLogImpl::HandleMsg(
	PVOID _handler_param, 
	LPTSTR _msg,
	size_t _bytes,
	PVOID _msg_param)
{
	if (NULL == _handler_param)
	{
		ASSERT(false);
		return false;
	}

	CLogImpl* this_ptr = static_cast<CLogImpl*>(_handler_param);
	DirId work_dir_id = reinterpret_cast<DirId>(_msg_param);

	TCHAR file_path[MAX_PATH] = {0};

	// 打到其他工作目录的日志同时也会打到默认工作目录中
	if (true == this_ptr->GetLogFile(file_path, MAX_PATH, 0))
	{
		this_ptr->WriteLogFile(file_path, _msg);
	}
	if (0 != work_dir_id && 
		true == this_ptr->GetLogFile(file_path, MAX_PATH, work_dir_id))
	{
		this_ptr->WriteLogFile(file_path, _msg);
	}

	return true;
}

//----------------------------------------------------------------------------
// 日志系统包装函数.
//----------------------------------------------------------------------------
W2X_COMMON_API bool SetGlobal(const Custom& _custom_ref)
{
	return CLogImpl::GetInstance().SetGlobal(_custom_ref);
}

W2X_COMMON_API void GetGlobal(Custom& custom_ref)
{
	CLogImpl::GetInstance().GetGlobal(custom_ref);
}

W2X_COMMON_API void ResetGlobal(void)
{
	CLogImpl::GetInstance().ResetGlobal();
}

W2X_COMMON_API bool GetRootDir(LPTSTR _dir_path, size_t _size_in_words)
{
	return CLogImpl::GetInstance().GetRootDir(_dir_path, _size_in_words);
}

W2X_COMMON_API DirId AddWorkDir(LPCTSTR _dir_name)
{
	return CLogImpl::GetInstance().AddWorkDir(_dir_name);
}

W2X_COMMON_API bool GetLogFile(
	LPTSTR _file_path, 
	size_t _size_in_words, 
	DirId _work_dir_id)
{
	return CLogImpl::GetInstance().GetLogFile(_file_path, _size_in_words, _work_dir_id);
}

W2X_COMMON_API DWORD GetLogFileHeaderSize(void)
{
	return sizeof(BYTE_LOG_FILE_HEAD);
}

W2X_COMMON_API bool GetLogFileHeader(PBYTE _byte_buffer, DWORD& _size_in_bytes)
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

W2X_COMMON_API bool Log(const Custom* _custom_ptr, LPCTSTR _format_str, ...)
{
	IF_NULL_ASSERT_RETURN_VALUE(_format_str, false);

	va_list args;
	va_start(args, _format_str);
	bool is_successed = CLogImpl::GetInstance().Log(_custom_ptr, _format_str, args);
	va_end(args);

	return is_successed;
}

W2X_COMMON_API bool Log(
	const Custom* _custom_ptr, 
	LPCTSTR _format_str, 
	va_list& _arg_list_ref)
{
	if (NULL == _format_str)
	{
		ASSERT(false);
		return false;
	}

	return CLogImpl::GetInstance().Log(_custom_ptr, _format_str, _arg_list_ref);
}

W2X_COMMON_API bool LogInfo(LPCTSTR _format_str, ...)
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
	bool is_sccessed = CLogImpl::GetInstance().Log(&custom, _format_str, args);
	va_end(args);

	return is_sccessed;
}

W2X_COMMON_API bool LogWarn(LPCTSTR _format_str, ...)
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
	bool is_successed = CLogImpl::GetInstance().Log(&custom, _format_str, args);
	va_end(args);

	return is_successed;
}

W2X_COMMON_API bool LogError(LPCTSTR _format_str, ...)
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
	bool is_successed = CLogImpl::GetInstance().Log(&custom, _format_str, args);
	va_end(args);

	return is_successed;
}

W2X_COMMON_API bool LogDebug(LPCTSTR _format_str, ...)
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
	bool is_successed = CLogImpl::GetInstance().Log(&custom, _format_str, args);
	va_end(args);

	return is_successed;
}


W2X_DEFINE_NAME_SPACE_END(log)
W2X_NAME_SPACE_END
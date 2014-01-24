/******************************************************************************
 * 文件：	command.h
 * 描述：命令行类，可使用命令行方式创建子进程，并通过匿名管道读取子进程输出结果，这个过程是
 *      阻塞的，只有当子进程正常结束或者超时被杀掉后，函数 Execute() 才会返回，可通过调
 *      用函数 GetOutput() 获取子进程输出结果。
 * 作者：	wu.xiongxing
 * 邮箱：	wxxweb@gmail.com
 * 时间：	2013-01-21
 ******************************************************************************/

#ifndef __W2X_COMMON_COMMAND_H__
#define __W2X_COMMON_COMMAND_H__

#include "exports.h"
#include "macros.h"
#include <WTypes.h>
#include <string>

W2X_NAME_SPACE_BEGIN

class W2X_COMMON_API CCommand
{
public:
	enum EExecuteStatus {
		ES_SUCCESSED			= 0,	// 子进程成功返回
		ES_CREATE_PIPE_FAILD	= 1,	// 创建管道失败
		ES_CREATE_PROCESS		= 2,	// 创建子进程失败
		ES_INVALID_ARG			= 3,	// 无效参数
		ES_READ_FAILD			= 4,	// 读管道失败
	};

public:
	CCommand(void);
	virtual ~CCommand(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CCommand);

public:
	EExecuteStatus Execute(
		LPCTSTR _app_path,			// 被运行的程序路径
		LPCTSTR _app_args,			// 程序运行参数
		DWORD _time_out = 5000,		// 程序运行的等待超时时间，单位毫秒
		bool _is_save_output = true	// 是否保存程序输出结果
	);

	EExecuteStatus Execute(
		LPCTSTR _cmd_line,			// 包含程序路径及参数的命令行
		DWORD _time_out = 5000,		// 程序运行的等待超时时间，单位毫秒
		bool _is_save_output = true	// 是否保存程序输出结果
	);

	// 获取子进程输出的字符串长度
	int GetOutputLength(void) const;

	// 获取子进程输出的字符串，成功则返回指向 str_buffer 的字符串指针
	LPTSTR GetOutput(LPTSTR _str_buffer, size_t _size_in_bytes) const;

	TSTDSTR GetOutput(void) const;

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};

W2X_NAME_SPACE_END

#endif /* __W2X_COMMON_COMMAND_H__ */
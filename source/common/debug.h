/******************************************************************************
 * 文件：	crt_dbg.h
 *
 * 描述：	用于在 DEBUG 模式下对数据有效性、堆内存泄漏等进行检测，并输出检测报告。
 *			若想让程序在 Debug 模式下检测泄漏并输出报告，请将本头文件 include 在 
 *			系统头文件之后，用户头文件之前。 如果有 stdafx.h ，可放在它之后。
 *			之后要做的就是在程序入口处调用 CCrtDbg::enable_leak_check()，可以调用
 *           CCrtDbg::set_report_mode() 来设置输出报告的目标位置。
 *
 * 邮箱：	wxxweb@gmail.com
 * 作者：	wu.xiongxing
 * 时间：	2013-05-24
 ******************************************************************************/

#ifndef __W2X_COMMON_CRT_DBG_H__
#define __W2X_COMMON_CRT_DBG_H__

#include "exports.h"


#define _CRTDBG_MAP_ALLOC	// 开启 DEBUG 模式下堆内存泄漏检测

// C 运行时头文件
#include <stdlib.h>
#include <crtdbg.h>


/* 将 new 替换成带文件名和行号参数的 new */
#ifdef _DEBUG
#  ifndef DEBUG_NEW 
#    define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#    define new DEBUG_NEW
#  else // 用于检测内存泄漏的 new 操作符在 MFC 有相应的定义
#    pragma message("DEBUG_NEW is defined in MFC.")
#  endif /* DEBUG_NEW */
#endif /* _DEBUG */


/*
 * 模仿 MFC 的断言，当表达式为假时弹出带有文件名、行号和表达式的提示框
 */
#ifndef ASSERT
#  ifdef _DEBUG
#    define ASSERT(expression) \
	   do { \
	     if ( !!(expression) || \
	       _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, #expression) ) \
	         __asm {int 3}; \
	   } while(0)
#  else
#    define ASSERT(expression) ((void)0)
#  endif
#endif /* ASSERT */


/*
 * 模仿 MFC 的 TRACE
 */
#ifndef TRACE
#  ifdef _DEBUG
#    define TRACE(format,...) \
       _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, NULL, format, __VA_ARGS__)
#  else
#    define TRACE(format) ((void)0)
#  endif
#endif


class CDebug
{
public:
	enum EReportMode {
		kModeDebug,
		kModeStderr,
	};

private:
	CDebug(void);
	~CDebug(void);

public:

	/*
	 * 设置输出检测报告的目标：VS 的 Debug 输出窗口，或者是标准错误输出 stderr
	 */
	static W2X_COMMON_API void set_report_mode(EReportMode mode);

	/*
	 * 该函数放在程序入口处，用于开启栈内存泄漏检测，当出现内存泄漏，将输出内存泄漏
	 * 的相关信息，如下：
	 * 
	 *	1.其中分配泄漏的内存的文件名；
	 *	2.文件内的行号（在括号中）；
	 *	3.内存分配编号（在大括号内）；
	 *	4.块类型（普通、客户端或 CRT）；
	 *	5.十六进制形式的内存位置；
	 *	6.以字节为单位的块大小；
	 *	7.前 16 字节的内容（亦为十六进制）。
	 */
	static W2X_COMMON_API void enable_leak_check(bool delay = false);
};


#endif /* __W2X_COMMON_CRT_DBG_H__ */
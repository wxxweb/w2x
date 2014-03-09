/******************************************************************************
 * 文件：	debug.h
 *
 * 描述：	功能一：
 *          用于在 DEBUG 模式下对数据有效性、堆内存泄漏等进行检测，并输出检测报告。
 *		    若想让程序在 Debug 模式下检测泄漏并输出报告，请将本头文件 include 在 
 *		    系统头文件之后，用户头文件之前。 如果有 stdafx.h，可放在它之后。
 *		    后要做的就是在程序入口处调用 w2x::debug::EnableLeakCheck()，
 *          可以调用w2x::debug::SetReportMode() 来设置输出报告的目标位置。
 *
 *          功能二：
 *          在程序出现异常时进行捕获，生成 dump 文件，并结合编译时生成的 pdb 文件
 *			对程序进行调试来定位崩溃点。同时并支持用户设置回调函数，可在回调函数中
 *          弹出友好提示，或将相关文件及信息上传到服务器供后期处理。想要使用此功能，
 *          可在程序入口点调用 w2x::debug::EnableExcptionHandle()
 *
 * 邮箱：	wxxweb@gmail.com
 * 作者：	wu.xiongxing
 * 时间：	2013-05-24
 ******************************************************************************/

#ifndef __W2X_COMMON_DEBUG_H__
#define __W2X_COMMON_DEBUG_H__

#include "exports.h"


// 开启 DEBUG 模式下堆内存泄漏检测
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW

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
#    define ASSERT(expression) (void) \
	   ( (!!(expression)) || \
	     (_CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, #expression)) && \
		 ([](){__asm int 3}(), 0) \
       )
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

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(debug)

/*
* Debug 调试报告模式
*/
enum EReportMode {
	kModeDebug,		// 弹出 Debug 对话框
	kModeStderr,	// 重定向到标准错误输出
};

/*
* 异常处理器返回值
*/
enum EExcptionReturn {
	kPassToNextHandler,	// 将异常传递给下一个处理器
	kEndProcess,		// 结束进程运行
};

/*
* 程序异常信息结构
*/
struct ExcptionInfo {
	LPCTSTR module_name;
	PVOID excption_address;
	DWORD excption_code;
	DWORD excption_flags;
};

/*
* 异常处理器函数类型，函数返回值见 EExcptionReturn
*/
typedef EExcptionReturn (CALLBACK* FExcptionHandler)(const ExcptionInfo&);

/*
* 设置输出检测报告的目标：VS 的 Debug 输出窗口，或者是标准错误输出 stderr
*/
W2X_COMMON_API void SetReportMode(EReportMode mode);

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
W2X_COMMON_API void EnableLeakCheck(bool delay = false);


/*
* 该函数放在程序入口处，用于开启进程顶层异常处理，当出现异常时，将调用异常处理器函数。如果
* 未设置自定义异常处理器函数 _handler_ptr，则使用默认异常处理器函数，弹框报告异常信息。
* 异常相关信息如下：
* 
*	1.发生异常的模块全路径；
*	2.异常编号；
*	3.异常地址；
*	4.异常标识。
*/
W2X_COMMON_API void EnableExcptionHandle(FExcptionHandler _handler_ptr = NULL);


W2X_DEFINE_NAME_SPACE_END(debug)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_DEBUG_H__ */
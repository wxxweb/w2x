/*****************************************************************************
文件:	ICefWebBrowserEventHandler.h
描述:	定义 Chromiumem 嵌入式浏览器事件处理器接口。
作者:	吴雄兴					
版权:	www.utooo.com
日期:	2013-09-21
修改:	2014-05-16
******************************************************************************/

#ifndef __W2X_CEF_WEB_BROWSER_EVENT_HANDLER_H__
#define __W2X_CEF_WEB_BROWSER_EVENT_HANDLER_H__
#pragma once

#include <string>

#ifdef W2X_CEF_EXPORTS
#  ifndef  W2X_CEF_API
#    define W2X_CEF_API __declspec(dllexport)
#  endif
#else
#  ifndef W2X_CEF_API
#    define W2X_CEF_API __declspec(dllimport)
#  endif
#endif

/** 根据UNICODE决定使用宽字符或多字符版C++字符串 */
#ifndef TSTDSTR
#  ifdef UNICODE
#    define TSTDSTR std::wstring
#  else
#    define TSTDSTR std::string
#  endif
#endif

class ICefWebBrowser;

class ICefWebBrowserEventHandler
{
public:
	virtual ~ICefWebBrowserEventHandler(void) {}

public:
	/** 注册浏览器接口 */
	virtual bool RegisterWebBrowser(ICefWebBrowser* _cef_web_browser) = 0;

	/** 注销浏览器接口 */
	virtual void UnregisterWebBrowser(void) = 0;

	/** 当浏览器窗口被创建后回调 */
	virtual void OnAfterCreated(const TSTDSTR& _startup_url) = 0;

	/** 当浏览器窗口被关闭前回调 */
	virtual void OnBeforeClose(void) = 0;

	/** 当 Web 文档被构建后回调，若为主框架页面则 _is_main = true */
	virtual void OnContextCreated(const TSTDSTR& _url, bool _is_main) = 0;

	/** 当 Web 文档开始加载时回调，若为主框架页面则 _is_main = true */
	virtual void OnLoadStart(const TSTDSTR& _url, bool _is_main) = 0;

	/** 当 Web 文档加载结束时回调，若为主框架页面则 _is_main = true */
	virtual void OnLoadEnd(const TSTDSTR& _url, bool _is_main) = 0;

	/** 当 Web 文档加载出错时回调 */
	virtual bool OnLoadError(
		/* [out] */ TSTDSTR& _error_text,
		/* [in] */ const TSTDSTR& _failed_url,
		/* [in] */ DWORD _error_code
		) = 0;

	/** 当导航状态改变时被回调 */
	virtual void OnNavStateChange(
		const TSTDSTR& _url,
		bool canGoBack,
		bool canGoForward
		) = 0;

	/** 当 URL 地址发生改变时被回调 */
	virtual void OnAddressChange(const TSTDSTR& _url) = 0;

	/**
	 * 执行 JS 的方法调用，传入方法的名称和参数字符串，并设置其返回值。
	 * 当方法被处理返回 true。
	 */
	virtual bool OnJsInvoke(
		LPCTSTR _fn_name,
		LPCTSTR _args
		) = 0;

	/** 当接收到控制台消息时被回调 */
	virtual void OnConsoleMessage(const TSTDSTR& _msg) = 0;
	
	/** 当 JavaScript 脚本出现未捕获异常时被回调 */
	virtual void OnUncaughtException(
		const TSTDSTR& _msg,
		const TSTDSTR& _file_name,
		const TSTDSTR& _src_line,
		int _line_num,
		int _column
		) = 0;
};

#endif ///< __W2X_CEF_WEB_BROWSER_EVENT_HANDLER_H__
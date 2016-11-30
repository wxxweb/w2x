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


#include "cef_web_browser.h"

enum CefErrorCode {
	CEF_ERR_FAILED = -2,
	CEF_ERR_ABORTED = -3,
	CEF_ERR_INVALID_ARGUMENT = -4,
	CEF_ERR_INVALID_HANDLE = -5,
	CEF_ERR_FILE_NOT_FOUND = -6,
	CEF_ERR_TIMED_OUT = -7,
	CEF_ERR_FILE_TOO_BIG = -8,
	CEF_ERR_UNEXPECTED = -9,
	CEF_ERR_ACCESS_DENIED = -10,
	CEF_ERR_NOT_IMPLEMENTED = -11,
	CEF_ERR_CONNECTION_CLOSED = -100,
	CEF_ERR_CONNECTION_RESET = -101,
	CEF_ERR_CONNECTION_REFUSED = -102,
	CEF_ERR_CONNECTION_ABORTED = -103,
	CEF_ERR_CONNECTION_FAILED = -104,
	CEF_ERR_NAME_NOT_RESOLVED = -105,
	CEF_ERR_INTERNET_DISCONNECTED = -106,
	CEF_ERR_SSL_PROTOCOL_ERROR = -107,
	CEF_ERR_ADDRESS_INVALID = -108,
	CEF_ERR_ADDRESS_UNREACHABLE = -109,
	CEF_ERR_SSL_CLIENT_AUTH_CERT_NEEDED = -110,
	CEF_ERR_TUNNEL_CONNECTION_FAILED = -111,
	CEF_ERR_NO_SSL_VERSIONS_ENABLED = -112,
	CEF_ERR_SSL_VERSION_OR_CIPHER_MISMATCH = -113,
	CEF_ERR_SSL_RENEGOTIATION_REQUESTED = -114,
	CEF_ERR_CERT_COMMON_NAME_INVALID = -200,
	CEF_ERR_CERT_DATE_INVALID = -201,
	CEF_ERR_CERT_AUTHORITY_INVALID = -202,
	CEF_ERR_CERT_CONTAINS_ERRORS = -203,
	CEF_ERR_CERT_NO_REVOCATION_MECHANISM = -204,
	CEF_ERR_CERT_UNABLE_TO_CHECK_REVOCATION = -205,
	CEF_ERR_CERT_REVOKED = -206,
	CEF_ERR_CERT_INVALID = -207,
	CEF_ERR_CERT_END = -208,
	CEF_ERR_INVALID_URL = -300,
	CEF_ERR_DISALLOWED_URL_SCHEME = -301,
	CEF_ERR_UNKNOWN_URL_SCHEME = -302,
	CEF_ERR_TOO_MANY_REDIRECTS = -310,
	CEF_ERR_UNSAFE_REDIRECT = -311,
	CEF_ERR_UNSAFE_PORT = -312,
	CEF_ERR_INVALID_RESPONSE = -320,
	CEF_ERR_INVALID_CHUNKED_ENCODING = -321,
	CEF_ERR_METHOD_NOT_SUPPORTED = -322,
	CEF_ERR_UNEXPECTED_PROXY_AUTH = -323,
	CEF_ERR_EMPTY_RESPONSE = -324,
	CEF_ERR_RESPONSE_HEADERS_TOO_BIG = -325,
	CEF_ERR_CACHE_MISS = -400,
	CEF_ERR_INSECURE_RESPONSE = -501,
};

class W2X_CEF_API ICefWebBrowserEventHandler
{
	friend class CCefWebBrowser;

public:
	ICefWebBrowserEventHandler(void) : m_browser(NULL) {}
	virtual ~ICefWebBrowserEventHandler(void) {}

public:
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
		/* [in] */ CefErrorCode _error_code
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
	virtual bool OnJsInvoke(LPCTSTR _fn_name, LPCTSTR _args) = 0;

	/** 当接收到控制台消息时被回调 */
//	virtual void OnConsoleMessage(const TSTDSTR& _msg) = 0;
	
	/** 当 JavaScript 脚本出现未捕获异常时被回调 */
	virtual void OnUncaughtException(
		const TSTDSTR& _msg,
		const TSTDSTR& _stack_trace,
		const TSTDSTR& _file_name,
		const TSTDSTR& _src_line,
		int _line_num,
		int _column
		) = 0;

protected:
	CCefWebBrowser* const m_browser;
};

#endif ///< __W2X_CEF_WEB_BROWSER_EVENT_HANDLER_H__
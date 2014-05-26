/*******************************************************************************
文件:	cef_web_browser_impl.h
描述:	类 CCefWebBrowserImpl 封装了用于显示 Web 页面的浏览器窗口，内部使用 Chromiumem 
		嵌入式框架来实现浏览器功能而非使用 IE。你可以直接使用它来显示 Web 页面， 或者继
		承它，做相应的处理。它可以实现 JavaScript 和 C++ 的交互。
作者:	吴雄兴					
版权:	www.utooo.com
日期:	2013-08-26
修改:	2014-05-16
*******************************************************************************/

#ifndef __W2X_CEF_WEB_BROWSER_IMPL_H__
#define __W2X_CEF_WEB_BROWSER_IMPL_H__
#pragma once

#include "cef_web_browser_i.h"
//#include "w2x_common/macros.h"
//#include "w2x_common/mutex.h"
#include "w2x_common/msg_loop.h"
#include "include/cef_client.h"
//#include "cefclient/download_handler.h"
#include <map>
#include <string>


class ICefWebBrowserEventHandler;

typedef bool (CALLBACK* CustomSchemeHandler)(
	const TSTDSTR& _scheme_name, 
	const TSTDSTR& _url
	);

// CCefWebBrowser implementation.
class CCefWebBrowserImpl
	: public CefClient
	, public CefLifeSpanHandler
	, public CefLoadHandler
	, public CefV8ContextHandler
	, public CefDisplayHandler
	, public ICefWebBrowser
	//, public CefRequestHandler
	//, public CefFocusHandler
	//, public CefKeyboardHandler
	//, public CefDragHandler
	//, public CefPermissionHandler
{
public:
	enum { 
		FRAME_CMD_REFRESH = 4001,
		FRAME_CMD_CONSOLE = 4002,
	};

public:
	CCefWebBrowserImpl(void);

	virtual ~CCefWebBrowserImpl();

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CCefWebBrowserImpl);
	// Include the default locking implementation.
	IMPLEMENT_LOCKING(CCefWebBrowserImpl);

public:

	// CefClient methods
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler()
	{ return this; }
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler()
	{ return this; }
	virtual CefRefPtr<CefV8ContextHandler> GetV8ContextHandler() 
	{ return this; }
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler()
	{ return this; }
//   virtual CefRefPtr<CefRequestHandler> GetRequestHandler()
// 	 { return this; }
//   virtual CefRefPtr<CefFocusHandler> GetFocusHandler()
// 	 { return this; }
//   virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler()
// 	 { return this; }
//   virtual CefRefPtr<CefDragHandler> GetDragHandler()
// 	 { return this; }
//   virtual CefRefPtr<CefPermissionHandler> GetPermissionHandler()
// 	 { return this; }

	// CefLifeSpanHandler methods
	virtual bool OnBeforePopup(
		CefRefPtr<CefBrowser> parentBrowser,
		const CefPopupFeatures& popupFeatures,
		CefWindowInfo& windowInfo,
		const CefString& url,
		CefRefPtr<CefClient>& client,
		CefBrowserSettings& settings
		);
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser);
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser);

	// CefLoadHandler methods
	virtual void OnLoadStart(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame
		);
	virtual void OnLoadEnd(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		int httpStatusCode
		);
	virtual bool OnLoadError(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& failedUrl,
		CefString& errorText
		);

	// CefDisplayHandler methods
	virtual void OnNavStateChange(
		CefRefPtr<CefBrowser> browser,
		bool canGoBack,
		bool canGoForward
		);
	virtual void OnAddressChange(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& url
		);
	virtual bool OnConsoleMessage(
		CefRefPtr<CefBrowser> browser,
		const CefString& message,
		const CefString& source,
		int line
		);

	// CefV8ContextHandler methods
	virtual void OnContextCreated(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context
		);
	virtual void OnUncaughtException(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context,
		CefRefPtr<CefV8Exception> exception,
		CefRefPtr<CefV8StackTrace> stackTrace
		);

//   // CefRequestHandler methods
//   virtual bool OnBeforeResourceLoad(
//	     CefRefPtr<CefBrowser> browser,
// 	     CefRefPtr<CefRequest> request,
// 	     CefString& redirectUrl,
// 	     CefRefPtr<CefStreamReader>& resourceStream,
// 	     CefRefPtr<CefResponse> response,
// 	     int loadFlags) 
//	 {
// 	    REQUIRE_IO_THREAD();
// 	    return false;
//   }
//
//   // CefFocusHandler methods.
//   virtual void OnFocusedNodeChanged(
//	     CefRefPtr<CefBrowser> browser,
// 	     CefRefPtr<CefFrame> frame,
// 	     CefRefPtr<CefDOMNode> node
//	     );
//
//   // CefKeyboardHandler methods.
//   virtual bool OnKeyEvent(
//       CefRefPtr<CefBrowser> browser,
// 	     KeyEventType type,
// 	     int code,
// 	     int modifiers,
// 	     bool isSystemKey,
// 	     bool isAfterJavaScript
//       );
//
//   // CefDragHandler methods.
//   virtual bool OnDragStart(
//	     CefRefPtr<CefBrowser> browser,
// 	     CefRefPtr<CefDragData> dragData,
// 	     DragOperationsMask mask
//	 );
//   virtual bool OnDragEnter(CefRefPtr<CefBrowser> browser,
// 	  CefRefPtr<CefDragData> dragData,
// 	  DragOperationsMask mask);
//
//   // CefPermissionHandler methods.
//   virtual bool OnBeforeScriptExtensionLoad(
//       CefRefPtr<CefBrowser> browser,
//       CefRefPtr<CefFrame> frame,
//       const CefString& extensionName
//	 );

	bool CallJsFunction(
		const TSTDSTR& _fn_name,
		const TSTDSTR& _args
		);

public:
	bool Create(
		const CefString& url = "about:blank",
		CefWindowHandle _parent_hwnd = NULL, 
		ICefWebBrowserEventHandler* _handler = NULL
		);

	void SetParentHwnd(CefWindowHandle hwnd);
	CefWindowHandle GetParentHwnd();

	//CefRefPtr<CefBrowser> GetBrowser();
	CefWindowHandle GetBrowserHwnd();

	TSTDSTR GetStartupURL() const;

	TSTDSTR GetUrl(void) const;

	bool LoadUrl(LPCTSTR _url);

	void Close(void);

	bool MoveBrowser(int _x, int _y, int _width, int _height);

	void SetCaptionMargin(
		int _height = 0, 
		int _margin_top = 0, 
		int _margin_left = 0, 
		int _margin_right = 0
		);

	bool AddExternal(const TSTDSTR& _fn_name);

	bool AddJsCode(const TSTDSTR& _js_code);

	static bool Initialize(LPCTSTR _cache_path, LPCTSTR _log_file);

	static bool RegisterCustomScheme(
		const TSTDSTR& _scheme_name, 
		CustomSchemeHandler _handler
		);

private:
	// Add the V8 bindings.
	void InitBinding(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Value> object);

	// DOM visitors will be called after the associated path is loaded.
	void AddDOMVisitor(const std::string& path, CefRefPtr<CefDOMVisitor> visitor);
	CefRefPtr<CefDOMVisitor> GetDOMVisitor(const std::string& path);

	bool ExecuteJsCode(const TSTDSTR& _js_code);

	static bool AddThisPtr(CefWindowHandle _hwnd, CCefWebBrowserImpl* _ptr);
	static bool RemoveThisPtr(CefWindowHandle _hwnd);
	static CCefWebBrowserImpl* GetThisPtr(CefWindowHandle _hwnd);

	static BOOL CALLBACK WebViewHostFinder(HWND hwnd, LPARAM lParam);
	static LRESULT WINAPI WebViewHostWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

	static bool CALLBACK ProcJsCodeMsg(
		PVOID pvProcParam, 
		LPTSTR pszJsonMsg, 
		size_t nBytes, 
		PVOID pvParam
		);

private:
	// The child browser window
	CefRefPtr<CefBrowser> m_browser;

	// The main frame window handle
	CefWindowHandle m_parent_hwnd;

	// The child browser window handle
	CefWindowHandle m_browser_hwnd;

	// Support for DOM visitors.
	typedef std::map<std::string, CefRefPtr<CefDOMVisitor> > DOMVisitorMap;
	DOMVisitorMap m_DOMVisitors;

	// True if a form element currently has focus
//	bool m_bFormElementHasFocus;

	// The URL to be loaded at application startup.
	TSTDSTR m_startup_url;

	// Support for V8 Handlers.
	typedef std::map<TSTDSTR, CefRefPtr<CefV8Handler> > V8HandlerMap;
	V8HandlerMap m_V8Handlers;

	// The V8 object in the global window object with the name "external".
	CefRefPtr<CefV8Value> m_external;

	ICefWebBrowserEventHandler* m_event_handler;

	RECT m_caption_margin;

	w2x::CMsgLoop m_cpp_msg_loop;

	typedef std::map<CefWindowHandle, CCefWebBrowserImpl*> ThisPtrMap;
	static ThisPtrMap sm_this_ptrs;

	static CefCriticalSection sm_this_ptrs_mutex;

	static bool sm_is_console_alloc;

	static WNDPROC sm_old_wnd_proc;
};

#endif ///< __W2X_CEF_WEB_BROWSER_IMPL_H__

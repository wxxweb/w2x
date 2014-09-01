/*******************************************************************************
文件:	cef_web_browser_impl.cpp
描述:	参见 cef_web_browser_impl.h
作者:	吴雄兴					
版权:	www.utooo.com
日期:	2013-08-26
修改:
*******************************************************************************/

#include "stdafx.h"
#include "cef_web_browser_impl.h"
#include "cef_web_browser_event_handler_i.h"
#include "include/cef_app.h"
#include "include/cef_scheme.h"
#include "include/cef_task.h"
#include "util.h"
//#include "w2x_common/log.h"
#include <string>

// #define REQUIRE_UI_THREAD()   ASSERT(CefCurrentlyOn(TID_UI));
// #define REQUIRE_IO_THREAD()   ASSERT(CefCurrentlyOn(TID_IO));
// #define REQUIRE_FILE_THREAD() ASSERT(CefCurrentlyOn(TID_FILE));

/**
 * @brief 通过继承 V8 引擎处理器类 CefV8Handler 来捕获 JavaScript 到 C++ 的调用请求。
 */
class CJsInvokeHandler: public CefV8Handler
{
public:
	CJsInvokeHandler(ICefWebBrowserEventHandler* _handler);
	virtual ~CJsInvokeHandler(void);

public:
	/**
	 * 执行 JS 的方法调用，传入方法的参数列表，并设置其返回值。当方法被处理返回 true。 
	 */
	virtual bool Execute(
		const CefString& name,
		CefRefPtr<CefV8Value> object,
		const CefV8ValueList& arguments,
		CefRefPtr<CefV8Value>& retval,
		CefString& exception
	);

	IMPLEMENT_REFCOUNTING(CJsInvokeHandler);

private:
	static bool CALLBACK ProcJsInvokeMsg(
		PVOID pvProcParam, 
		LPTSTR pszMsg, 
		size_t nBytes, 
		PVOID pvParam
		);

private:
	ICefWebBrowserEventHandler* m_handler;
	w2x::CMsgLoop m_js_msg_loop;
};

CJsInvokeHandler::CJsInvokeHandler(ICefWebBrowserEventHandler* _handler)
	: m_handler(_handler) 
{
	m_js_msg_loop.StartLoopThread(CJsInvokeHandler::ProcJsInvokeMsg,
		reinterpret_cast<PVOID>(this));
}

CJsInvokeHandler::~CJsInvokeHandler(void)
{

}

bool CJsInvokeHandler::Execute(
	const CefString& name,
	CefRefPtr<CefV8Value> object,
	const CefV8ValueList& arguments,
	CefRefPtr<CefV8Value>& retval,
	CefString& exception
	)
{
	const size_t argc = arguments.size();
	
	if (0 == argc)
	{
		exception = TEXT("C++: no argument");
		return false;
	}
	if (1 < argc)
	{
		exception = TEXT("C++: more then one argument");
		return false;
	}
	if (false == arguments[0]->IsString())
	{
		exception = TEXT("C++: not string argument");
		return false;
	}
	if (NULL == m_handler)
	{
		TSTDSTR exp = TEXT("C++: not found method '");
		exp += name;
		exp += TEXT("'");
		exception = exp;
		return false;
	}

	static TSTDSTR js_msg;
	js_msg = name;
	js_msg += TEXT(",");
	js_msg += arguments[0]->GetStringValue();

	bool succeeded = m_js_msg_loop.AddMsg(js_msg.c_str(), NULL);
	retval = CefV8Value::CreateBool(succeeded);
	return true;
}

bool CALLBACK CJsInvokeHandler::ProcJsInvokeMsg(
	PVOID pvProcParam, 
	LPTSTR pszMsg, 
	size_t nBytes, 
	PVOID pvParam
	)
{
	UNUSED_ALWAYS(nBytes);

	if (NULL == pvProcParam)
	{
		return false;
	}
	CJsInvokeHandler* const this_ptr = 
		reinterpret_cast<CJsInvokeHandler*>(pvProcParam);
	if (NULL == this_ptr)
	{
		return false;
	}
	LPTSTR args = _tcschr(pszMsg, TEXT(','));
	if (NULL != args)
	{
		*args = TEXT('\0');
		++args;
	}
	if (NULL != this_ptr->m_handler)
	{
		this_ptr->m_handler->OnJsInvoke(pszMsg, args);
	}
	return true;
}


class CCustomSchemeHandler : public CefSchemeHandler
{
public:
	CCustomSchemeHandler(
		const CefString& _scheme_name, CustomSchemeHandler _handler)
		: m_scheme_name(_scheme_name)
		, m_handler(_handler)
	{}

public:
	virtual bool ProcessRequest(
		CefRefPtr<CefRequest> request,
		CefRefPtr<CefSchemeHandlerCallback> callback
		);

	virtual void GetResponseHeaders(
		CefRefPtr<CefResponse> response,
		int64& response_length,
		CefString& redirectUrl) 
	{ /*REQUIRE_IO_THREAD();*/ }

	virtual void Cancel() 
	{ /*REQUIRE_IO_THREAD();*/ }

	virtual bool ReadResponse(
		void* data_out,
		int bytes_to_read,
		int& bytes_read,
		CefRefPtr<CefSchemeHandlerCallback> callback)
	{
		//REQUIRE_IO_THREAD();
		//AutoLock lock_scope(this);
		return false;
	}

private:
	IMPLEMENT_REFCOUNTING(CCustomSchemeHandler);
	IMPLEMENT_LOCKING(CCustomSchemeHandler);

private:
	CefString m_scheme_name;
	CustomSchemeHandler m_handler;
};


bool CCustomSchemeHandler::ProcessRequest(
	CefRefPtr<CefRequest> request,
	CefRefPtr<CefSchemeHandlerCallback> callback)
{
	REQUIRE_IO_THREAD();
	AutoLock lock_scope(this);

	if (NULL != m_handler)
	{
		CefString url = request->GetURL();
		return m_handler(m_scheme_name.c_str(), url.c_str());
	}

	return false;
}

// Implementation of the factory for for creating schema handlers.
class CCustomSchemeHandlerFactory : public CefSchemeHandlerFactory 
{
public:
	CCustomSchemeHandlerFactory(CustomSchemeHandler _handler)
		: m_handler(_handler)
	{}

	// Return a new scheme handler instance to handle the request.
	virtual CefRefPtr<CefSchemeHandler> Create(
		CefRefPtr<CefBrowser> browser,
		const CefString& scheme_name,
		CefRefPtr<CefRequest> request
		)
	{
		REQUIRE_IO_THREAD();
		return new CCustomSchemeHandler(scheme_name, m_handler);
	}

	IMPLEMENT_REFCOUNTING(CCustomSchemeHandlerFactory);

private:
	CustomSchemeHandler m_handler;
};


bool CCefWebBrowserImpl::sm_is_console_alloc = false;
WNDPROC CCefWebBrowserImpl::sm_old_wnd_proc = NULL;
CCefWebBrowserImpl::ThisPtrMap CCefWebBrowserImpl::sm_this_ptrs;
CefCriticalSection CCefWebBrowserImpl::sm_this_ptrs_mutex;


CCefWebBrowserImpl::CCefWebBrowserImpl(void)
	: m_parent_hwnd(NULL)
	, m_browser_hwnd(NULL)
	, m_event_handler(NULL)
	//, m_bFormElementHasFocus(false)
{
	memset(&m_caption_margin, 0, sizeof(m_caption_margin));
}


CCefWebBrowserImpl::~CCefWebBrowserImpl()
{
}


void CCefWebBrowserImpl::OnAfterCreated(CefRefPtr<CefBrowser> browser) 
{
	REQUIRE_UI_THREAD();

	AutoLock lock_scope(this);
	if (!m_browser.get())
	{
		// We need to keep the main child window, but not popup windows
		m_browser = browser;
		m_browser_hwnd = browser->GetWindowHandle();
		CCefWebBrowserImpl::AddThisPtr(m_browser_hwnd, this);

		// 替换WebViewHost的窗口处理函数
		::EnumChildWindows(m_browser_hwnd, WebViewHostFinder, 0);

		RECT rect = {0};
		::GetClientRect(::GetParent(m_parent_hwnd), &rect);
		::MoveWindow(m_browser_hwnd, 0, 0, 
			rect.right - rect.left, rect.bottom - rect.top, FALSE);

		if (NULL != m_event_handler)
		{
			m_event_handler->OnAfterCreated(m_startup_url);
		}
	}
}

void CCefWebBrowserImpl::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	REQUIRE_UI_THREAD();

	if (NULL != m_event_handler)
	{
		m_event_handler->UnregisterWebBrowser();
		m_event_handler->OnBeforeClose();
		m_event_handler = NULL;
	}

	m_cpp_msg_loop.StopLoopThread();

	if (m_browser_hwnd == browser->GetWindowHandle()) 
	{
		// Free the browser pointer so that the browser can be destroyed
		CCefWebBrowserImpl::RemoveThisPtr(m_browser_hwnd);
		m_browser = NULL;
		m_browser_hwnd = NULL;
	}
}

void CCefWebBrowserImpl::OnLoadStart(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame
	) 
{
	REQUIRE_UI_THREAD();

// 	if (m_browser_hwnd == browser->GetWindowHandle() && frame->IsMain()) {
// 		// We've just started loading a page
// 		SetLoading(true);
// 	}

	if (NULL != m_event_handler)
	{
		m_event_handler->OnLoadStart(frame->GetURL(), frame->IsMain());
	}
}

void CCefWebBrowserImpl::OnLoadEnd(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	int httpStatusCode
	) 
{
	REQUIRE_UI_THREAD();

	if (m_browser_hwnd == browser->GetWindowHandle() && frame->IsMain()) {
		// We've just finished loading a page
		//    SetLoading(false);
		CefRefPtr<CefDOMVisitor> visitor = GetDOMVisitor(frame->GetURL());
		if (visitor.get())
			frame->VisitDOM(visitor);

		if (NULL != m_event_handler)
		{
			m_cpp_msg_loop.StartLoopThread(CCefWebBrowserImpl::ProcJsCodeMsg, 
			reinterpret_cast<PVOID>(this));
		}
	}

	if (NULL != m_event_handler)
	{
		m_event_handler->OnLoadEnd(frame->GetURL(), frame->IsMain());
	}
}

bool CCefWebBrowserImpl::OnLoadError(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	ErrorCode errorCode,
	const CefString& failedUrl,
	CefString& errorText
	) 
{
	REQUIRE_UI_THREAD();

	if (errorCode == ERR_CACHE_MISS) {
		// Usually caused by navigating to a page with POST data via back or
		// forward buttons.
// 		errorText = "<html><head><title>Expired Form Data</title></head>"
// 			"<body><h1>Expired Form Data</h1>"
// 			"<h2>Your form request has expired. "
// 			"Click reload to re-submit the form data.</h2></body>"
// 			"</html>";
	} else {
		// All other messages.
// 		std::stringstream ss;
// 		ss <<       "<html><head><title>Load Failed</title></head>"
// 			"<body><h1>Load Failed</h1>"
// 			"<h2>Load of URL " << std::string(failedUrl) <<
// 			" failed with error code " << static_cast<int>(errorCode) <<
// 			".</h2></body>"
// 			"</html>";
// 		errorText = ss.str();
	}

	if (NULL != m_event_handler)
	{
		TSTDSTR text;
		m_event_handler->OnLoadError(text, failedUrl, errorCode);
		errorText = text;
	}

	return true;
}

void CCefWebBrowserImpl::OnNavStateChange(
	CefRefPtr<CefBrowser> browser,
	bool canGoBack,
	bool canGoForward
	)
{
	REQUIRE_UI_THREAD();

	if (NULL != m_event_handler)
	{
		m_event_handler->OnNavStateChange(
			browser->GetMainFrame()->GetURL(), canGoBack, canGoForward);
	}
}

bool CCefWebBrowserImpl::OnConsoleMessage(
	CefRefPtr<CefBrowser> browser,
	const CefString& message,
	const CefString& source,
	int line) 
{
	REQUIRE_UI_THREAD();

	if (true == sm_is_console_alloc)
	{
		_ftprintf_s(stdout, TEXT("\nCEF> %s"), message.c_str());
	}

	if (NULL != m_event_handler)
	{
		m_event_handler->OnConsoleMessage(message);
	}

	return true;
}

// void CCefWebBrowserImpl::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
//                                          CefRefPtr<CefFrame> frame,
//                                          CefRefPtr<CefDOMNode> node) {
//   REQUIRE_UI_THREAD();
// 
//   // Set to true if a form element has focus.
//   m_bFormElementHasFocus = (node.get() && node->IsFormControlElement());
// }

// bool CCefWebBrowserImpl::OnKeyEvent(CefRefPtr<CefBrowser> browser,
//                                KeyEventType type,
//                                int code,
//                                int modifiers,
//                                bool isSystemKey,
//                                bool isAfterJavaScript) {
//   REQUIRE_UI_THREAD();
// 
//   if (isAfterJavaScript && !m_bFormElementHasFocus && code == 0x20) {
//     // Special handling for the space character if a form element does not have
//     // focus.
//     if (type == KEYEVENT_RAWKEYDOWN) {
//       browser->GetMainFrame()->ExecuteJavaScript(
//           "alert('You pressed the space bar!');", "", 0);
//     }
//     return true;
//   }
// 
//   return false;
// }



void CCefWebBrowserImpl::OnContextCreated(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context) 
{
	REQUIRE_UI_THREAD();

	CefRefPtr<CefV8Value> object = context->GetGlobal();

	// Add the V8 bindings.
	this->InitBinding(browser, frame, object);

	//  std::string url = frame->GetURL();
	//   if (url == performance_test::kTestUrl)
	//     performance_test::InitTest(browser, frame, object);

	if (NULL != m_event_handler)
	{
		m_event_handler->OnContextCreated(frame->GetURL(), frame->IsMain());
	}
}

void CCefWebBrowserImpl::OnUncaughtException(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context,
	CefRefPtr<CefV8Exception> exception,
	CefRefPtr<CefV8StackTrace> stackTrace)
{
	if (true == sm_is_console_alloc)
	{
		_ftprintf_s(stderr,
			TEXT("CEF>\n")
			TEXT("CEF> [UNCAUGHT EXCPTION]\n")
			TEXT("CEF>   DESC: %s\n")
			TEXT("CEF>   FILE: %s\n")
			TEXT("CEF>   LINE: %d\n")
			TEXT("CEF>   COL:  %d\n")
			TEXT("CEF>   SRC:  %s\n")
			TEXT("CEF>\n"),
			exception->GetMessage().c_str(),
			exception->GetScriptResourceName().c_str(),
			exception->GetLineNumber(),
			exception->GetStartColumn(),
			exception->GetSourceLine().c_str());
	}

	if (NULL != m_event_handler)
	{
		m_event_handler->OnUncaughtException(
			exception->GetMessage(),
			exception->GetScriptResourceName(),
			exception->GetSourceLine(),
			exception->GetLineNumber(),
			exception->GetStartColumn());
	}
}

// bool CCefWebBrowserImpl::OnDragStart(CefRefPtr<CefBrowser> browser,
//                                 CefRefPtr<CefDragData> dragData,
//                                 DragOperationsMask mask) {
//   REQUIRE_UI_THREAD();
// 
//   // Forbid dragging of image files.
//   if (dragData->IsFile()) {
//     std::string fileName = dragData->GetFileName();
//     if (fileName.find(".png") != std::string::npos ||
//         fileName.find(".jpg") != std::string::npos ||
//         fileName.find(".gif") != std::string::npos)
//       return true;
//   }
// 
//   return false;
// }

// bool CCefWebBrowserImpl::OnDragEnter(CefRefPtr<CefBrowser> browser,
//                                 CefRefPtr<CefDragData> dragData,
//                                 DragOperationsMask mask) {
//   REQUIRE_UI_THREAD();
// 
//   // Forbid dragging of link URLs.
//   if (dragData->IsLink())
//     return true;
// 
//   return false;
// }

// bool CCefWebBrowserImpl::OnBeforeScriptExtensionLoad(
//     CefRefPtr<CefBrowser> browser,
//     CefRefPtr<CefFrame> frame,
//     const CefString& extensionName) {
//   return false;
// }

bool CCefWebBrowserImpl::ExecuteJsCode(const TSTDSTR& _js_code)
{
	if (NULL == m_browser)
	{
		return false;
	}

	m_browser->GetMainFrame()->ExecuteJavaScript(
		_js_code, m_browser->GetMainFrame()->GetURL(), 0);

	return true;
}

bool CCefWebBrowserImpl::CallJsFunction(
	const TSTDSTR& _fn_name,
	const TSTDSTR& _args
	)
{
	if (_fn_name.empty()) 
	{
		return false;
	}
	if (NULL == m_browser)
	{
		return false;
	}

	static TSTDSTR js_code;
	js_code = _fn_name;
	js_code += TEXT("(");
	js_code += _args;
	js_code += TEXT(");");

	m_browser->GetMainFrame()->ExecuteJavaScript(js_code, TEXT(""), 0);
	return true;
}

bool CCefWebBrowserImpl::Create(
	const CefString& _url,
	CefWindowHandle _parent_hwnd, 
	ICefWebBrowserEventHandler* _handler
	)
{
	if (NULL != m_browser)
	{
		return false;
	}

	m_startup_url = _url;
	m_event_handler = _handler;
	m_parent_hwnd = _parent_hwnd;

	// Initialize the browser settings.
	CefBrowserSettings browserSettings;
	CefBrowserSettingsTraits::init(&browserSettings);
	browserSettings.page_cache_disabled = true;
	browserSettings.application_cache_disabled = true;
	browserSettings.web_security_disabled = true;	///< 允许跨域访问

	// Initialize window info to the defaults for a child window
	RECT rect;
	CefWindowInfo info;
	::GetClientRect(m_parent_hwnd, &rect);
	info.SetAsChild(m_parent_hwnd, rect);

	// Creat the new child browser window
	bool is_successed = CefBrowser::CreateBrowser(info,
		static_cast<CefRefPtr<CefClient> >(this),
		m_startup_url, browserSettings);
	if (false == is_successed)
	{
		return false;
	}

	return true;
}

void CCefWebBrowserImpl::SetParentHwnd(CefWindowHandle hwnd) 
{
	AutoLock lock_scope(this);
	m_parent_hwnd = hwnd;
}

CefWindowHandle CCefWebBrowserImpl::GetParentHwnd()
{ 
	return m_parent_hwnd;
}

// CefRefPtr<CefBrowser> CCefWebBrowserImpl::GetBrowser() 
// { 
// 	return m_browser;
// }

CefWindowHandle CCefWebBrowserImpl::GetBrowserHwnd() 
{ 
	return m_browser_hwnd; 
}

TSTDSTR CCefWebBrowserImpl::GetStartupURL() const
{ 
	return m_startup_url;
}

void CCefWebBrowserImpl::AddDOMVisitor(const std::string& path,
	CefRefPtr<CefDOMVisitor> visitor)
{
	AutoLock lock_scope(this);
	DOMVisitorMap::iterator it = m_DOMVisitors.find(path);
	if (it == m_DOMVisitors.end())
		m_DOMVisitors.insert(std::make_pair(path, visitor));
	else
		it->second = visitor;
}

CefRefPtr<CefDOMVisitor> CCefWebBrowserImpl::GetDOMVisitor(const std::string& path)
{
	AutoLock lock_scope(this);
	DOMVisitorMap::iterator it = m_DOMVisitors.find(path);
	if (it != m_DOMVisitors.end())
		return it->second;
	return NULL;
}

void CCefWebBrowserImpl::InitBinding(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Value> object)
{

	// Create the new V8 object.
	m_external = CefV8Value::CreateObject(NULL);
	// Add the new V8 object to the global window object with the name
	// "external".
	object->SetValue("external", m_external, V8_PROPERTY_ATTRIBUTE_NONE);

	// Add V8 functions to the external object.
	for (V8HandlerMap::iterator itHandler = m_V8Handlers.begin();
		m_V8Handlers.end() != itHandler; ++itHandler) {

			const TSTDSTR& name = itHandler->first;
			CefRefPtr<CefV8Handler>& handlerPtr = itHandler->second;
			m_external->SetValue(name,
				CefV8Value::CreateFunction(name, handlerPtr),
				V8_PROPERTY_ATTRIBUTE_NONE);
	}
}

bool CCefWebBrowserImpl::AddExternal(const TSTDSTR& _fn_name)
{
	AutoLock lock_scope(this);

	// 浏览器创建后无效
	if (NULL != m_browser) {
		ASSERT(false);
		return false;
	}
	if (_fn_name.empty()) {
		ASSERT(false);
		return false;
	}

	m_V8Handlers[_fn_name] = new CJsInvokeHandler(m_event_handler);

	return true;
}

BOOL CCefWebBrowserImpl::WebViewHostFinder(HWND hwnd, LPARAM lParam)
{
	TCHAR szClassName[MAX_PATH] = {0};
	if (0 == GetClassName(hwnd, szClassName, MAX_PATH))
	{
		return TRUE;
	}

	if (0 == lstrcmpi(szClassName, TEXT("WebViewHost")))
	{
		sm_old_wnd_proc = (WNDPROC)::GetWindowLongPtr(hwnd, GWLP_WNDPROC);
		::SetWindowLongPtr(hwnd, GWLP_WNDPROC, 
			(LONG)CCefWebBrowserImpl::WebViewHostWndProc);
		return FALSE;
	}

	return TRUE;
}


LRESULT CCefWebBrowserImpl::WebViewHostWndProc(
	HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = 1;
	switch (nMsg)
	{
	case WM_LBUTTONDOWN:
		lResult = CCefWebBrowserImpl::OnLButtonDown(hWnd, wParam, lParam);
		break;

	case WM_RBUTTONDOWN:
		lResult = CCefWebBrowserImpl::OnRButtonDown(hWnd, wParam, lParam);
		return 0;

	case WM_COMMAND:
		lResult = CCefWebBrowserImpl::OnCommand(hWnd, wParam, lParam);
		break;

	default:
		break;
	}

	if (0 == lResult) {
		return lResult;
	}

	return ::CallWindowProc(sm_old_wnd_proc, hWnd, nMsg, wParam, lParam);
}

LRESULT CCefWebBrowserImpl::OnLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HWND parent_hwnd = ::GetParent(hWnd);
	CCefWebBrowserImpl* this_ptr = 
		CCefWebBrowserImpl::GetThisPtr(parent_hwnd);
	if (NULL == this_ptr) {
		return 1;
	}
	POINT pt = {0};
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	RECT rtCaption = {0};
	::GetWindowRect(parent_hwnd, &rtCaption);
	::ScreenToClient(parent_hwnd, (LPPOINT)&rtCaption);
	::ScreenToClient(parent_hwnd, ((LPPOINT)&rtCaption) + 1);
	rtCaption.left += this_ptr->m_caption_margin.left;
	rtCaption.right -= this_ptr->m_caption_margin.right;
	rtCaption.top += this_ptr->m_caption_margin.top;
	rtCaption.bottom = rtCaption.top + this_ptr->m_caption_margin.bottom;
	
	BOOL bRetVal = FALSE;
	if (TRUE == ::PtInRect(&rtCaption, pt))
	{
		HWND parent_parent_hwnd = ::GetParent(parent_hwnd);
		if (NULL != parent_parent_hwnd)
		{
			::PostMessage(parent_parent_hwnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
		}
		else {
			::PostMessage(parent_hwnd, WM_LBUTTONDOWN, wParam, lParam);
		}
		return 0;
	}
	return 1;
}

LRESULT CCefWebBrowserImpl::OnRButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (0 == (wParam & MK_CONTROL) || 0 == (wParam & MK_SHIFT))
	{
		return 1;
	}

	HWND parent_hwnd = GetParent(hWnd);
	if (NULL == parent_hwnd || FALSE == ::IsWindow(parent_hwnd))
	{
		ASSERT(false);
		return 1;
	}
	CCefWebBrowserImpl* this_ptr = 
		CCefWebBrowserImpl::GetThisPtr(parent_hwnd);
	if (NULL == this_ptr) {
		return 1;
	}
	
	UINT nGoBackFlags = this_ptr->m_browser->CanGoBack() ? MF_ENABLED : MF_GRAYED;
	UINT nGoForwardFlags = this_ptr->m_browser->CanGoForward() ? MF_ENABLED : MF_GRAYED;

	HMENU hMenu = ::CreatePopupMenu();
	::AppendMenu(hMenu, MF_STRING | MF_POPUP | nGoBackFlags, 
		FRAME_CMD_GO_BACK, TEXT("返回"));
	::AppendMenu(hMenu, MF_STRING | MF_POPUP | nGoForwardFlags, 
		FRAME_CMD_GO_FORWARD, TEXT("前进"));
	::AppendMenu(hMenu, MF_STRING | MF_POPUP, FRAME_CMD_SHOW_URL, TEXT("地址"));
	::AppendMenu(hMenu, MF_STRING | MF_POPUP, FRAME_CMD_REFRESH, TEXT("刷新"));
	::AppendMenu(hMenu, MF_STRING | MF_POPUP, FRAME_CMD_CONSOLE, TEXT("控制台"));

	POINT ptMouse = {0};
	::GetCursorPos(&ptMouse);
	::SendMessage(parent_hwnd, WM_ENTERMENULOOP, TRUE, 0);
	BOOL bTrack = ::TrackPopupMenu(
		hMenu, TPM_LEFTALIGN, ptMouse.x, ptMouse.y, 0, hWnd, NULL);
	ASSERT(TRUE == bTrack);
	::SendMessage(parent_hwnd, WM_EXITMENULOOP, FALSE, 0);
	::DestroyMenu(hMenu);

	return 0;
}

LRESULT CCefWebBrowserImpl::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (0 != lParam)
	{
		return 1;
	}

	// MENU
	HWND parent_hwnd = ::GetParent(hWnd);
	CCefWebBrowserImpl* this_ptr = 
		CCefWebBrowserImpl::GetThisPtr(parent_hwnd);
	if (NULL == this_ptr) {
		return 1;
	}
	switch (LOWORD(wParam))
	{
	case FRAME_CMD_GO_BACK:
		if (this_ptr->m_browser->CanGoBack()) {
			this_ptr->m_browser->GoBack();
		}
		break;

	case FRAME_CMD_GO_FORWARD:
		if (this_ptr->m_browser->CanGoForward()) {
			this_ptr->m_browser->GoForward();
		}
		break;

	case FRAME_CMD_SHOW_URL:
		::MessageBox(NULL, 
			this_ptr->m_browser->GetFocusedFrame()->GetURL().c_str(),
			TEXT("URL"), MB_OK);
		this_ptr->m_browser->ShowDevTools();
		break;

	case FRAME_CMD_REFRESH:
		this_ptr->m_browser->Reload();
		break;

	case FRAME_CMD_CONSOLE:
		if (TRUE == ::AllocConsole())
		{
			FILE* pFile = NULL;
			::freopen_s(&pFile, "CONOUT$", "w", stdout);
			::freopen_s(&pFile, "CONOUT$", "w", stderr);
			sm_is_console_alloc = true;
		}
		else
		{
			::FreeConsole();
			sm_is_console_alloc = false;
		}
		break;

	default:
		break;
	}

	return 0;
}

bool CCefWebBrowserImpl::OnBeforePopup(
	CefRefPtr<CefBrowser> parentBrowser,
	const CefPopupFeatures& popupFeatures,
	CefWindowInfo& windowInfo,
	const CefString& url,
	CefRefPtr<CefClient>& client,
	CefBrowserSettings& settings) 
{
		REQUIRE_UI_THREAD();

		// #ifdef TEST_REDIRECT_POPUP_URLS
		//   std::string urlStr = url;
		//   if (urlStr.find("Chromiumem-devtools:") == std::string::npos) {
		//     // Show all popup windows excluding DevTools in the current window.
		//     windowInfo.m_dwStyle &= ~WS_VISIBLE;
		//     client = new ClientPopupHandler(m_browser);
		//   }
		// #endif  // TEST_REDIRECT_POPUP_URLS

		return false;
}

void CCefWebBrowserImpl::OnAddressChange(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	const CefString& url) 
{
	REQUIRE_UI_THREAD();

//   if (m_browser_hwnd == browser->GetWindowHandle() && frame->IsMain())   {
//       // Set the edit window text
//       SetWindowText(m_EditHwnd, std::wstring(url).c_str());
//   }

	if (NULL != m_event_handler)
	{
		m_event_handler->OnAddressChange(url);
	}
}

void CCefWebBrowserImpl::Close(void)
{
	if (NULL != m_browser)
	{
		m_browser->ParentWindowWillClose();
		m_browser->CloseBrowser();
	}
}

bool CCefWebBrowserImpl::MoveBrowser(int _x, int _y, int _width, int _height)
{
	if (NULL != m_browser_hwnd)
	{
		return TRUE == ::MoveWindow(m_browser_hwnd, _x, _y, _width, _height, FALSE);
	}
	return false;
}

TSTDSTR CCefWebBrowserImpl::GetUrl(void) const
{
	if (NULL != m_browser)
	{
		return m_browser->GetMainFrame()->GetURL();
	}
	return TEXT("");
}

bool CCefWebBrowserImpl::LoadUrl(LPCTSTR _url)
{
	if (NULL == _url) {
		return false;
	}
	if (NULL == m_browser) {
		return false;
	}

	m_browser->GetMainFrame()->LoadURL(_url);
	return true;
}

void CCefWebBrowserImpl::SetCaptionMargin(
	int _height, 
	int _margin_top, 
	int _margin_left, 
	int _margin_right
	)
{
	m_caption_margin.left = _margin_left;
	m_caption_margin.right = _margin_right;
	m_caption_margin.top = _margin_top;
	m_caption_margin.bottom = _height;
}

bool CCefWebBrowserImpl::AddThisPtr(CefWindowHandle _hwnd, CCefWebBrowserImpl* _ptr)
{
	if (NULL == _hwnd) {
		return false;
	}
	sm_this_ptrs_mutex.Lock();
	sm_this_ptrs[_hwnd] = _ptr;
	sm_this_ptrs_mutex.Unlock();
	return true;
}

bool CCefWebBrowserImpl::RemoveThisPtr(CefWindowHandle _hwnd)
{
	if (NULL == _hwnd) {
		return false;
	}
	sm_this_ptrs_mutex.Lock();
	ThisPtrMap::iterator it = sm_this_ptrs.find(_hwnd);
	if (sm_this_ptrs.end() == it) {
		sm_this_ptrs_mutex.Unlock();
		return false;
	}
	sm_this_ptrs_mutex.Unlock();
	return true;
}

CCefWebBrowserImpl* CCefWebBrowserImpl::GetThisPtr(CefWindowHandle _hwnd)
{
	if (NULL == _hwnd) {
		return NULL;
	}
	sm_this_ptrs_mutex.Lock();
	ThisPtrMap::iterator it = sm_this_ptrs.find(_hwnd);
	if (sm_this_ptrs.end() == it) {
		sm_this_ptrs_mutex.Unlock();
		return NULL;
	}
	sm_this_ptrs_mutex.Unlock();
	return it->second;
}

bool CCefWebBrowserImpl::Initialize(LPCTSTR _cache_path, LPCTSTR _log_file)
{
	// Parse command line arguments. The passed in values are ignored on Windows.
	//AppInitCommandLine(0, NULL);

	CefSettings settings;
	CefRefPtr<CefApp> app;

	// Populate the settings based on command line arguments.
	CefSettingsTraits::init(&settings);
	//AppGetSettings(settings, app);

	if (NULL != _cache_path) {
		CefString(&settings.cache_path) = _cache_path;
	}

	if (NULL != _log_file) {
		settings.log_severity = LOGSEVERITY_WARNING;
		CefString(&settings.log_file) = _log_file;
	} else {
		settings.log_severity = LOGSEVERITY_DISABLE;
	}

	settings.multi_threaded_message_loop = true;
	settings.uncaught_exception_stack_size = 1;

	// 语言包位置
	TCHAR locales_dir_path[MAX_PATH] = TEXT("");
	::GetCurrentDirectory(MAX_PATH, locales_dir_path);
	_tcscat_s(locales_dir_path, TEXT("\\locales"));
	CefString(&settings.locales_dir_path) = locales_dir_path;

	// Initialize CEF.
	return CefInitialize(settings, app);
}

void CCefWebBrowserImpl::Uninitialize(void)
{
	CefShutdown();
}

bool CCefWebBrowserImpl::RegisterCustomScheme(
	LPCTSTR _scheme_name,
	CustomSchemeHandler _handler)
{
	if (NULL == _scheme_name)
	{
		return false;
	}

	return CefRegisterSchemeHandlerFactory(_scheme_name, TEXT(""),
		new CCustomSchemeHandlerFactory(_handler));
}

bool CALLBACK CCefWebBrowserImpl::ProcJsCodeMsg(
	PVOID pvProcParam, 
	LPTSTR pszJsonMsg, 
	size_t nBytes, 
	PVOID pvParam
	)
{
	if (NULL == pvProcParam) {
		return false;
	}
	if (NULL == pszJsonMsg) {
		return false;
	}

	CCefWebBrowserImpl* const this_ptr = 
		reinterpret_cast<CCefWebBrowserImpl*>(pvProcParam);
	if (NULL == this_ptr) {
		return false;
	}

	return this_ptr->ExecuteJsCode(pszJsonMsg);
}

bool CCefWebBrowserImpl::AddJsCode(const TSTDSTR& _js_code)
{
	return m_cpp_msg_loop.AddMsg(_js_code.c_str(), 
		(_js_code.length() + 1) * sizeof(TCHAR), NULL);
}
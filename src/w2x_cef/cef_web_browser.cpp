/*****************************************************************************
文件名：		DlgPhoneWebFrame.cpp
描述：		
作者：		吴雄兴					
版权：		www.utooo.com
日期：		2013.08.26
修改历史：
******************************************************************************/

#include "stdafx.h"
#include "cef_web_browser.h"
#include "cef_web_browser_impl.h"
#include <string>

// 
// class CCefWebBrowser::CImpl
// {
// public:
// 	CImpl(CCefWebBrowser* _parent);
// 	~CImpl(void);
// 
// public:
// 	bool Create(
// 		LPCTSTR _url,
// 		HWND _parent_hwnd,
// 		ICefWebBrowserEventHandler* _handler
// 		);
// 
// 	void Destory(void);
// 	inline LPCTSTR GetUrl(void) const;
// 	inline bool LoadUrl(LPCTSTR _url);
// 	inline HWND GetHwnd(void) const;
// 	//bool SendCommand(LPCTSTR pszFunction, LPCTSTR pszJsonParam);
// 
// 	bool ExecuteJsCode(
// 		const TSTDSTR& _js_code, 
// 		const TSTDSTR& _script_url,
// 		int _start_line
// 		);
// 
// 	bool CallJsFunction(
// 		const TSTDSTR& _fn_name,
// 		const TSTDSTR& _args
// 		);
// 
// private:
// 	static LRESULT CALLBACK WndProc(
// 		HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam);
// 
// 	int OnCreate(HWND _hwnd, LPCREATESTRUCT lpCreateStruct);
// 	void OnDestory(HWND _hwnd);
// 	void OnSize(HWND _hwnd, UINT nType, int cx, int cy);
// 	void OnClose();
// 	void OnSetFocus(HWND _hwnd, HWND _old_hwnd);
// 	void OnPaint(HWND _hwnd);
// 	void OnEnterMenuLoop(BOOL bIsTrackPopupMenu);
// 	void OnExitMenuLoop(BOOL bIsTrackPopupMenu);
// 	void OnLButtonDown(HWND _hwnd, UINT nFlags, int _x, int _y);
// 	BOOL OnEraseBkgnd(HWND _hwnd, HDC _hdc);
// 	void OnCommand(HWND _hwnd, int _id, HWND _hwnd_ctl, UINT _code_notify);
// 
// 	static bool CALLBACK OnUiCommand(
// 		PVOID pvProcParam, 
// 		LPTSTR pszJsonMsg, 
// 		size_t nBytes, 
// 		PVOID pvParam
// 		);
// 
// private:
// 	CCefWebBrowser* m_parent;
// 	HWND m_hwnd;
// 	CString m_url;
// 	CCefWebBrowser* m_handler;
// 	ICefWebBrowserEventHandler* m_event_handler;
// 	static size_t sm_frame_count;
// };
// 
// size_t CCefWebBrowser::CImpl::sm_frame_count = 0;
// 
// CCefWebBrowser::CImpl::CImpl(CCefWebBrowser* _parent)
// 	: m_parent(_parent)
// 	, m_hwnd(NULL)
// 	, m_event_handler(NULL)
// 	//, m_pMsgLoop(new w2x::CMsgLoop())
// 	, m_handler(NULL)
// {
// 	
// }
// 
// CCefWebBrowser::CImpl::~CImpl(void)
// {
// 	this->Destory();
// }
// 
// 
// LRESULT CALLBACK CCefWebBrowser::CImpl::WndProc(
// 	HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam)
// {
// 	CCefWebBrowser::CImpl* this_ptr = 
// 		reinterpret_cast<CCefWebBrowser::CImpl*>(GetWindowLongPtr(_hwnd, GWLP_USERDATA));
// 
// 	switch (_msg)
// 	{
// 	case WM_CREATE:
// 		{
// 			LPCREATESTRUCT pcs = reinterpret_cast<LPCREATESTRUCT>(_lparam);
// 			this_ptr = reinterpret_cast<CCefWebBrowser::CImpl*>(pcs->lpCreateParams);
// 			return this_ptr->OnCreate(_hwnd, pcs);
// 		}
// 	case WM_DESTROY:
// 		this_ptr->OnDestory(_hwnd);
// 		return 0;
// 
// 	case WM_SIZE:
// 		this_ptr->OnSize(_hwnd, static_cast<UINT>(_wparam), 
// 			static_cast<int>(LOWORD(_lparam)), 
// 			static_cast<int>(HIWORD(_lparam)));
// 		return 0;
// 
// 	case WM_SETFOCUS:
// 		this_ptr->OnSetFocus(_hwnd, reinterpret_cast<HWND>(_wparam));
// 		return 0;
// 
// 	case WM_PAINT:
// 		this_ptr->OnPaint(_hwnd);
// 		return 0;
// 
// 	case WM_ERASEBKGND:
// 		return this_ptr->OnEraseBkgnd(_hwnd, reinterpret_cast<HDC>(_wparam));
// 
// 	case WM_ENTERMENULOOP:
// 		this_ptr->OnEnterMenuLoop(static_cast<BOOL>(_wparam));
// 		return 0;
// 
// 	case WM_EXITMENULOOP:
// 		this_ptr->OnExitMenuLoop(static_cast<BOOL>(_wparam));
// 		return 0;
// 
// 	case WM_LBUTTONDOWN:
// 		this_ptr->OnLButtonDown(_hwnd, static_cast<UINT>(_wparam),
// 			static_cast<int>(LOWORD(_lparam)), 
// 			static_cast<int>(HIWORD(_lparam)));
// 		return 0;
// 
// 	case WM_COMMAND:
// 		this_ptr->OnCommand(_hwnd, 
// 			static_cast<int>(LOWORD(_wparam)), 
// 			reinterpret_cast<HWND>(_lparam), 
// 			static_cast<UINT>(HIWORD(_wparam)));
// 		return 0;
// 
// 	default: break;
// 	}
// 
// 	return ::DefWindowProc(_hwnd, _msg, _wparam, _lparam);
// }
// 
// 
// int CCefWebBrowser::CImpl::OnCreate(HWND _hwnd, LPCREATESTRUCT lpCreateStruct)
// {
// 	::SetWindowLongPtr(_hwnd, GWLP_USERDATA, reinterpret_cast<LONG>(this));
// 
// 	m_handler = new CCefWebBrowser(_hwnd, m_url.GetString(), m_event_handler);
// 	m_handler->AddRef();
// 
// 	// Create the child windows used for navigation
// 	RECT rect;
// 	::GetClientRect(_hwnd, &rect);
// 
// 	CefWindowInfo info;
// 	CefBrowserSettings browserSettings;
// 	CefBrowserSettingsTraits::init(&browserSettings);
// 	// Retrieve the current working directory.
// 	//AppRetrieveWorkingDirectory();
// 
// 	// Populate the settings based on command line arguments.
// 	//AppGetBrowserSettings(browserSettings);
// 	browserSettings.page_cache_disabled = true;
// 	browserSettings.application_cache_disabled = true;
// 
// 	// Initialize window info to the defaults for a child window
// 	info.SetAsChild(_hwnd, rect);
// 
// 	m_handler->AddExternal(TEXT("NotifyWebFrame"));
// 
// 	// Creat the new child browser window
// 	bool is_successed = CefBrowser::CreateBrowser(info,
// 		static_cast<CefRefPtr<CefClient> >(m_handler),
// 		m_handler->GetStartupURL(), browserSettings);
// 
// 	IF_FALSE_ASSERT (is_successed)
// 	{
// 		log::LogError(TEXT("CEF: Create browser failed."));
// 		return 0;
// 	}
// 
// 	if (NULL != m_event_handler)
// 	{
// 		m_event_handler->RegisterNotify(m_parent);
// 	}
// 
// 	return 0;
// }
// 
// void CCefWebBrowser::CImpl::OnDestory(HWND _hwnd)
// {
// 	IF_NULL_ASSERT_RETURN(m_hwnd);
// 
// 	if (NULL != m_event_handler)
// 	{
// 		m_event_handler->UnregisterNotify();
// 		m_event_handler->OnBeforeClose();
// 	}
// 
// 	CefRefPtr<CefBrowser> browser = m_handler->GetBrowser();
// 	if (browser.get()) {
// 		// Let the browser window know we are about to destroy it.
// 		browser->ParentWindowWillClose();
// 	}
// 	m_handler->Release();
// 	//SAFE_DELETE(m_handler);
// 
// 	if (NULL != m_hwnd) {
// 		m_hwnd = NULL;
// 	}
// }
// 
// void CCefWebBrowser::CImpl::OnSize(HWND _hwnd, UINT nType, int cx, int cy)
// {
// 	if (NULL != m_handler && m_handler->GetBrowserHwnd()) {
// 		// Resize the browser window and address bar to match the new frame
// 		// window size
// 		CRect rect;
// 		::GetClientRect(_hwnd, &rect);
// 		::MoveWindow(m_handler->GetBrowserHwnd(), 0, 0, rect.Width(), rect.Height(), FALSE);
// 	}
// }
// 
// void CCefWebBrowser::CImpl::OnSetFocus(HWND _hwnd, HWND _old_hwnd)
// {
// 	if (NULL != m_handler && m_handler->GetBrowserHwnd()) {
// 		// Pass focus to the browser window
// 		::PostMessage(m_handler->GetBrowserHwnd(), WM_SETFOCUS,
// 			reinterpret_cast<WPARAM>(_old_hwnd), NULL);
// 	}
// }
// 
// void CCefWebBrowser::CImpl::OnPaint(HWND _hwnd)
// {
// 	PAINTSTRUCT ps;
// 	::BeginPaint(_hwnd, &ps);
// 	::EndPaint(_hwnd, &ps);
// }
// 
// void CCefWebBrowser::CImpl::OnLButtonDown(HWND _hwnd, UINT nFlags, int _x, int _y)
// {
// 	RECT rtCaption = {0};
// 	::GetWindowRect(_hwnd, &rtCaption);
// 	//::InflateRect(&rtCaption, 0, -130);
// 	::ScreenToClient(_hwnd, (LPPOINT)&rtCaption);
// 	::ScreenToClient(_hwnd, ((LPPOINT)&rtCaption)+1);
// 	rtCaption.left = 200;
// 	rtCaption.right -= 130;
// 	rtCaption.bottom = 34;
// 
// 	POINT point = {0};
// 	point.x = _x;
// 	point.y = _y;
// 	if (TRUE == ::PtInRect(&rtCaption, point))
// 	{
// // 		IRemoteConfig* pRemoteConfig = IRemoteConfig::GetInstance();
// // 
// // 		if (!pRemoteConfig->IsConfigSwitchOn(IRemoteConfig::CSF_LOCK_SCREEN))
// // 		{
// 			::PostMessage(::GetParent(_hwnd), 
// 				WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(_x, _y));
// //		}
// 	}
// }
// 
// void CCefWebBrowser::CImpl::OnEnterMenuLoop(BOOL bIsTrackPopupMenu)
// {
// 	//if (!bIsTrackPopupMenu) {
// 		// Entering the menu loop for the application menu.
// 		CefSetOSModalLoop(true);
// 	//}
// }
// 
// void CCefWebBrowser::CImpl::OnExitMenuLoop(BOOL bIsTrackPopupMenu)
// {
// 	//if (!bIsTrackPopupMenu) {
// 		// Exiting the menu loop for the application menu.
// 		CefSetOSModalLoop(false);
// 	//}
// }
// 
// BOOL CCefWebBrowser::CImpl::OnEraseBkgnd(HWND _hwnd, HDC _hdc)
// {
// 	if (NULL != m_handler && m_handler->GetBrowserHwnd()) {
// 		// Dont erase the background if the browser window has been loaded
// 		// (this avoids flashing)
// 		return FALSE;
// 	}
// 
// 	return TRUE;
// }
// 
// void CCefWebBrowser::CImpl::OnCommand(HWND _hwnd, int _id, HWND _hwnd_ctl, UINT _code_notify)
// {
// 	switch (_id)
// 	{
// 	case CCefWebBrowser::FRAME_CMD_REFRESH:
// 		m_handler->GetBrowser()->Reload();
// 		break;
// 	default: break;
// 	}
// }
// 
// // bool CCefWebBrowser::CImpl::Create(
// // 	LPCTSTR _url,
// // 	HWND _parent_hwnd,
// // 	ICefWebBrowserEventHandler* _handler
// // 	)
// // {	
// // 	m_url = _url;
// // 	m_event_handler = _handler;
// // 
// // 	HINSTANCE instance = ::GetModuleHandle(NULL);
// // 	LPCTSTR pszWndClsName = TEXT("CefWebFrameClass");
// // 
// // 	WNDCLASSEX wcs = {0};
// // 	wcs.cbSize = sizeof(wcs);
// // 	wcs.cbClsExtra = 0;
// // 	wcs.cbWndExtra = 0;
// // 	wcs.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
// // 	wcs.hCursor = ::LoadCursor(NULL, IDC_ARROW);
// // 	wcs.hIcon = NULL;
// // 	wcs.hInstance = instance;
// // 	wcs.lpfnWndProc = CCefWebBrowser::CImpl::WndProc;
// // 	wcs.lpszClassName = pszWndClsName;
// // 	wcs.lpszMenuName = NULL;
// // 	wcs.style = CS_HREDRAW | CS_VREDRAW;
// // 
// // 	if (0 == ::RegisterClassEx(&wcs))
// // 	{
// // 		log::LogError(TEXT("CEF: Register window class '%s' failed(%d)."),
// // 			pszWndClsName, ::GetLastError());
// // 		ASSERT(false);
// // 		return NULL;
// // 	}
// // 
// // 	TCHAR wnd_name[MAX_PATH] = TEXT("");
// // 	_stprintf_s(wnd_name, TEXT("CefWebUiFrame:%d"), sm_frame_count);
// // 
// // 	m_hwnd = ::CreateWindow(pszWndClsName, wnd_name, WS_CHILD | WS_OVERLAPPED, 
// // 		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
// // 		_parent_hwnd, NULL, instance, reinterpret_cast<LPVOID>(this));
// // 
// // 	if (NULL == m_hwnd)
// // 	{
// // 		log::LogError(TEXT("CEF: Create window '%s' failed(%d)."),
// // 			wnd_name, ::GetLastError());
// // 		ASSERT(NULL != m_hwnd);
// // 		return NULL;
// // 	}
// // 
// // 	::ShowWindow(m_hwnd, SW_SHOW);
// // 	::UpdateWindow(m_hwnd);
// // 
// // 	return true;
// // }
// 
// 
// void CCefWebBrowser::CImpl::Destory(void)
// {
// 	IF_FALSE_ASSERT (NULL != m_hwnd && TRUE == ::IsWindow(m_hwnd))
// 	{
// 		return;
// 	}
// 	::DestroyWindow(m_hwnd);
// }
// 
// 
// inline LPCTSTR CCefWebBrowser::CImpl::GetUrl(void) const
// {
// 	IF_NULL_ASSERT_RETURN_VALUE(m_handler->GetBrowser().get(), NULL);
// 	return m_handler->GetBrowser()->GetMainFrame()->GetURL().c_str();
// }
// 
// inline bool CCefWebBrowser::CImpl::LoadUrl(LPCTSTR _url)
// {
// 	IF_NULL_ASSERT_RETURN_VALUE(_url, NULL);
// 	IF_NULL_ASSERT_RETURN_VALUE(m_handler->GetBrowser().get(), false);
// 	m_handler->GetBrowser()->GetMainFrame()->LoadURL(_url);
// 	return true;
// }
// 
// inline HWND CCefWebBrowser::CImpl::GetHwnd(void) const
// {
// 	return NULL != this ? m_hwnd : NULL;
// }
// 
// 
// bool CCefWebBrowser::CImpl::ExecuteJsCode(
// 	const TSTDSTR& _js_code,
// 	const TSTDSTR& _script_url,
// 	int _start_line
// 	)
// {
// 	CefRefPtr<CefFrame> spWebFrame;
// 	if (!spWebFrame.get() || 1 > spWebFrame->GetRefCt())
// 	{
// 		IF_NULL_ASSERT (m_handler)
// 		{
// 			log::LogWarn(TEXT("CCefWebBrowser::ExecuteJavaScript - Handle is NULL."));
// 			return false;
// 		}
// 
// 		CefRefPtr<CefBrowser> spBrowser = m_handler->GetBrowser();
// 		IF_NULL_ASSERT (spBrowser.get())
// 		{
// 			log::LogWarn(TEXT("CCefWebBrowser::ExecuteJavaScript - Browser is NULL."));
// 			return false;
// 		}
// 
// 		spWebFrame = spBrowser->GetMainFrame();
// 		IF_NULL_ASSERT (spWebFrame.get())
// 		{
// 			log::LogWarn(TEXT("CCefWebBrowser::ExecuteJavaScript - WebFrame is NULL."));
// 			return false;
// 		}
// 	}
// 
// 	log::LogInfo(TEXT("[JavaScript]: %s"), _js_code);
// 	if (spWebFrame.get())
// 	{
// 		spWebFrame->ExecuteJavaScript(_js_code, _script_url, _start_line);
// 		return true;
// 	}
// 	else
// 	{
// 		w2x::log::LogError(
// 			TEXT("CCefWebBrowser::ExecuteJavaScript - spWebFrame is NULL"));
// 	}
// 	return false;
// }
// 
// bool CCefWebBrowser::CImpl::CallJsFunction(
// 	const TSTDSTR& _fn_name,
// 	const TSTDSTR& _args
// 	)
// {
// 	IF_FALSE_ASSERT (_fn_name.empty()) 
// 	{
// 		return false;
// 	}
// 
// 	static TSTDSTR js_code;
// 	js_code = _fn_name;
// 	js_code += TEXT("(");
// 	js_code += _args;
// 	js_code += TEXT(");");
// 
// 	return this->ExecuteJsCode(js_code, TEXT(""), 0);
// }

CCefWebBrowser::CCefWebBrowser(void)
	: m_impl_ptr(new CCefWebBrowserImpl)
{
	m_impl_ptr->AddRef();
}

CCefWebBrowser::~CCefWebBrowser(void)
{
	m_impl_ptr->Release();
}


HWND CCefWebBrowser::GetHwnd(void) const
{
	return m_impl_ptr->GetBrowserHwnd();
}


void CCefWebBrowser::SetCaptionMargin(
	int _height, 
	int _margin_top, 
	int _margin_left, 
	int _margin_right
	)
{
	m_impl_ptr->SetCaptionMargin(_height, _margin_top, _margin_left, _margin_right);
}


bool CCefWebBrowser::AddExternal(const TSTDSTR& _fn_name)
{
	return m_impl_ptr->AddExternal(_fn_name);
}


bool CCefWebBrowser::Create(
	LPCTSTR _url,
	HWND _parent_hwnd,
	ICefWebBrowserEventHandler* _handler
	)
{
	return m_impl_ptr->Create(_url, _parent_hwnd, _handler);
}

void CCefWebBrowser::Destory(void)
{
	return m_impl_ptr->Close();
}

bool CCefWebBrowser::MoveBrowser(int _x, int _y, int _width, int _height)
{
	return m_impl_ptr->MoveBrowser(_x, _y, _width, _height);
}

TSTDSTR CCefWebBrowser::GetUrl(void) const
{
	return m_impl_ptr->GetUrl();
}

bool CCefWebBrowser::LoadUrl(LPCTSTR _url)
{
	return m_impl_ptr->LoadUrl(_url);
}

bool CCefWebBrowser::ExecuteJsCode(const TSTDSTR& _js_code)
{
	return m_impl_ptr->AddJsCode(_js_code);
}

bool CCefWebBrowser::CallJsFunction(
	const TSTDSTR& _fn_name,
	const TSTDSTR& _args
	)
{
	return m_impl_ptr->CallJsFunction(_fn_name, _args);
}

bool CCefWebBrowser::RegisterCustomScheme(
	const TSTDSTR& _scheme_name,
	CustomSchemeHandler _handler
	)
{
	return CCefWebBrowserImpl::RegisterCustomScheme(_scheme_name, _handler);
}

bool CCefWebBrowser::Initialize(LPCTSTR _cache_path, LPCTSTR _log_file)
{
	return CCefWebBrowserImpl::Initialize(_cache_path, _log_file);
}






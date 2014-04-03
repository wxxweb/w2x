/******************************************************************************
 * 文件:		sys_tray.cpp
 * 描述:		参见 sys_tray.h
 * 作者:		wu.xiongxing					
 * 邮箱:		wxxweb@gmail.com
 * 日期:		2014-03-17
*******************************************************************************/

#include "stdafx.h"
#include "sys_tray.h"
#include "macros.h"

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(ui)


class CSysTray::CImpl
{
public:
	CImpl(void);
	~CImpl(void);

public:
	void Initialise(void);
	void InstallIconPending(void);

	bool Create(
		HWND _main_wnd, UINT _callback_msg_id, LPCTSTR _tip, 
		HICON _icon, UINT _menu_id, bool _is_hidden = false
		);

	bool AddIcon(void);
	bool ShowIcon(void);
	bool HideIcon(void);
	bool RemoveIcon(void);

	void OnTrayNotification(WPARAM wParam, LPARAM lParam);

private:
	static LRESULT CALLBACK WndProc(
		HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam
		);

	LRESULT WndProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

	void OnTaskbarCreated(void);

private:
	HWND			m_icon_wnd;				// 托盘图标的窗口句柄
	HICON			m_icon_handle;
	NOTIFYICONDATA	m_icon_data;
	bool            m_is_hidden;			// 托盘图标是否被隐藏
	bool            m_is_removed;			// 托盘图标是否被移除
	bool            m_is_show_icon_pending;	// 当任务栏创建的时候是否显示过一次托盘图标
	UINT			m_def_menu_item_id;
	UINT			m_def_menu_item_by_pos;
	UINT			m_creation_flags;
	UINT			m_taskbar_created_msg;	// 

	static HINSTANCE sm_app_instance;
};


HINSTANCE CSysTray::CImpl::sm_app_instance = ::GetModuleHandle(NULL);


CSysTray::CImpl::CImpl(void)
{
	this->Initialise();
}


CSysTray::CImpl::~CImpl(void)
{
	this->RemoveIcon();
	if (FALSE != ::IsWindow(m_icon_wnd))
	{
		::DestroyWindow(m_icon_wnd);
	}
}


void CSysTray::CImpl::Initialise(void)
{
	memset(&m_icon_data, 0, sizeof(m_icon_data));

	m_icon_wnd = NULL;
	m_icon_handle = NULL;
	m_is_hidden = true;
	m_is_removed = true;
	m_is_show_icon_pending = false;
	m_creation_flags = 0;
	m_def_menu_item_id = 0;
	m_def_menu_item_by_pos = 0;
	m_taskbar_created_msg = 0;
	
}


bool CSysTray::CImpl::Create(
	HWND _target_wnd, UINT _callback_msg_id, LPCTSTR _tip, 
	HICON _icon, UINT _menu_id, bool _is_hidden
	)
{
	// 要避免和其他系统消息冲突
	ASSERT(_callback_msg_id >= WM_APP);

	IF_NULL_ASSERT (_tip) {
		_tip = TEXT("System Tray Tip");
	}
	const size_t max_tip_len = _countof(m_icon_data.szTip) - 1;
	size_t tip_len = _tcslen(_tip);
	IF_FALSE_ASSERT (tip_len <= max_tip_len)
	{
		tip_len = max_tip_len;
	}

	TCHAR wnd_class_name[MAX_PATH] = TEXT("W2xSysTray");
	_stprintf_s(wnd_class_name, TEXT("W2X:%d:%x"), sm_app_instance, 0);

	WNDCLASS wnd_class = {0};
	wnd_class.style = 0;
	wnd_class.lpfnWndProc = CImpl::WndProc;
	wnd_class.cbClsExtra = 0;
	wnd_class.cbWndExtra = 0;
	wnd_class.hInstance = sm_app_instance;
	wnd_class.hIcon = NULL;
	wnd_class.hCursor = NULL;
	wnd_class.hbrBackground = NULL;
	wnd_class.lpszMenuName = NULL;
	wnd_class.lpszClassName = wnd_class_name;

	RegisterClass(&wnd_class);

	// 创建一个不可见的窗口
	m_icon_wnd = ::CreateWindowEx(
		WS_EX_TOOLWINDOW, wnd_class_name, TEXT(""), WS_POPUP, 
		0,0,0,0, NULL, NULL, sm_app_instance, static_cast<LPVOID>(this));


	m_icon_data.cbSize = sizeof(NOTIFYICONDATA);
	m_icon_data.hWnd   = NULL != _target_wnd ? _target_wnd : m_icon_wnd;
	m_icon_data.uID    = _menu_id;
	m_icon_data.hIcon  = _icon;
	m_icon_data.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_icon_data.uCallbackMessage = _callback_msg_id;
	_tcsncpy_s(m_icon_data.szTip, _tip, tip_len);

	m_is_hidden = _is_hidden;
    
	if (m_is_hidden)
	{
		m_icon_data.uFlags = NIF_STATE;
		m_icon_data.dwState = NIS_HIDDEN;
		m_icon_data.dwStateMask = NIS_HIDDEN;
	}

	m_creation_flags = m_icon_data.uFlags;	// Store in case we need to recreate in OnTaskBarCreate

	BOOL add_result = TRUE;
	if (!m_is_hidden)
	{
		add_result = Shell_NotifyIcon(NIM_ADD, &m_icon_data);
		m_is_show_icon_pending = m_is_hidden = m_is_removed = (FALSE == add_result);
	}

	return TRUE == add_result;
}


bool CSysTray::CImpl::AddIcon(void)
{
	if (false == m_is_removed) {
		this->RemoveIcon();
	}

	m_icon_data.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	if (FALSE != ::Shell_NotifyIcon(NIM_ADD, &m_icon_data)) {
		m_is_show_icon_pending = true;
	}
	else {
		m_is_removed = m_is_hidden = false;
	}

	return false == m_is_removed;
}


bool CSysTray::CImpl::ShowIcon(void)
{
	if (true == m_is_removed) {
		return this->AddIcon();
	}

	if (false == m_is_hidden) {
		return true;
	}

	m_icon_data.uFlags = NIF_STATE;
	m_icon_data.dwState = 0;
	m_icon_data.dwStateMask = NIS_HIDDEN;
	m_is_hidden = FALSE != ::Shell_NotifyIcon (NIM_MODIFY, &m_icon_data);

	return false == m_is_hidden;
}


bool CSysTray::CImpl::HideIcon(void)
{
	if (true == m_is_removed || true == m_is_hidden) {
		return true;
	}

	m_icon_data.uFlags = NIF_STATE;
	m_icon_data.dwState = NIS_HIDDEN;
	m_icon_data.dwStateMask = NIS_HIDDEN;

	m_is_hidden = FALSE != ::Shell_NotifyIcon( NIM_MODIFY, &m_icon_data);

	return m_is_hidden;
}


bool CSysTray::CImpl::RemoveIcon(void)
{
	m_is_show_icon_pending = false;

	if (true == m_is_removed) {
		return true;
	}

	m_icon_data.uFlags = 0;
	if (FALSE != ::Shell_NotifyIcon(NIM_DELETE, &m_icon_data)) {
		m_is_removed = m_is_hidden = true;
	}

	return m_is_removed;
}


void CSysTray::CImpl::InstallIconPending(void)
{
	// Is the icon display pending, and it's not been set as "hidden"?
	if (false == m_is_show_icon_pending || true == m_is_hidden) {
		return;
	}
		
	// Reset the flags to what was used at creation
	m_icon_data.uFlags = m_creation_flags;

	// Try and recreate the icon
	m_is_hidden = TRUE != ::Shell_NotifyIcon(NIM_ADD, &m_icon_data);

	// If it's STILL hidden, then have another go next time...
	m_is_show_icon_pending = m_is_hidden;

	ASSERT(false == m_is_hidden);
}


LRESULT CALLBACK CSysTray::CImpl::WndProc(
	HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam
	)
{
	if (WM_CREATE == nMsg)
	{
		LPCREATESTRUCT create_struct_ptr 
			= reinterpret_cast<LPCREATESTRUCT>(lParam);
		::SetWindowLong(hWnd, GWL_USERDATA, 
			reinterpret_cast<LONG>(create_struct_ptr->lpCreateParams));
		return 0;
	}
	
	CImpl* wnd_ptr = reinterpret_cast<CImpl*>
		(::GetWindowLong(hWnd, GWL_USERDATA));
	if (NULL == wnd_ptr) 
	{
		return ::DefWindowProc(hWnd, nMsg, wParam, lParam);
	}

	return wnd_ptr->WndProc(nMsg, wParam, lParam);
}

LRESULT CSysTray::CImpl::WndProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	// 不要修改TaskbarCreated，这是系统任务栏自定义的消息
	static const UINT WM_TASKBARCREATED 
		= ::RegisterWindowMessage(TEXT("TaskbarCreated"));

	switch (nMsg)
	{
	case WM_CREATE:
		return 0;
	default:
		if (WM_TASKBARCREATED == nMsg) 
		{
			this->OnTaskbarCreated();
		}
		else if (nMsg == m_icon_data.uCallbackMessage)
		{
			if (wParam == m_icon_data.uID) {
				this->OnTrayNotification(wParam, lParam);
			}
		}
		return 0;
	}
	return ::DefWindowProc(m_icon_wnd, nMsg, wParam, lParam);
}

/** 
 * 每当系统任务栏被创建时，该函数被调用。 例如在 explorer.exe 崩溃或重启时。
 * @note WM_TASKBARCREATED 消息指传递给顶层窗口，就像 WM_QUERYNEWPALETTE
 */
void CSysTray::CImpl::OnTaskbarCreated(void)
{
	m_is_show_icon_pending = true; // !m_is_hidden;
	InstallIconPending();
}


void CSysTray::CImpl::OnTrayNotification(WPARAM wParam, LPARAM lParam) 
{
	if (wParam != m_icon_data.uID) {
		return;
	}
	HWND target_wnd = m_icon_data.hWnd;
	if (NULL == target_wnd) {
		return;
	}
	HMENU menu = NULL;
	HMENU sub_menu = NULL;

	// Clicking with right button brings up a context menu
	if (LOWORD(lParam) == WM_RBUTTONUP)
	{    
		menu = ::LoadMenu(sm_app_instance, MAKEINTRESOURCE(m_icon_data.uID));
		IF_NULL_ASSERT_RETURN(menu);

		sub_menu = ::GetSubMenu(menu, 0);
		IF_NULL_ASSERT_RETURN(sub_menu);

		// Make chosen menu item the default (bold font)
		::SetMenuDefaultItem(sub_menu, m_def_menu_item_id, m_def_menu_item_by_pos);

		// Display and track the popup menu
		POINT pos = {0};
		::GetCursorPos(&pos);
		::SetForegroundWindow(target_wnd); 
		::TrackPopupMenu(sub_menu, 0, pos.x, pos.y, 0, target_wnd, NULL);
		::PostMessage(target_wnd, WM_NULL, 0, 0);
		::DestroyMenu(menu);
	} 
	else if (LOWORD(lParam) == WM_LBUTTONDBLCLK) 
	{
		UINT item_id = 0;
		if (0 != m_def_menu_item_by_pos)
		{
			menu = ::LoadMenu(sm_app_instance, MAKEINTRESOURCE(m_icon_data.uID));
			IF_NULL_ASSERT_RETURN(menu);

			sub_menu = ::GetSubMenu(menu, 0);
			IF_NULL_ASSERT_RETURN(sub_menu);

			item_id = ::GetMenuItemID(sub_menu, m_def_menu_item_id);
			::DestroyMenu(menu);
		}
		else
		{
			item_id = m_def_menu_item_id;
		}
		::PostMessage(target_wnd, WM_COMMAND, item_id, 0);
		::SetForegroundWindow(target_wnd);
	}
}


CSysTray::CSysTray(void)
	: m_impl_ptr(new CImpl())
{
}


CSysTray::CSysTray(
	HWND _target_wnd, UINT _callback_msg_id, LPCTSTR _tip, HICON _icon, 
	UINT _menu_id, bool _is_hidden /* = false */)
	: m_impl_ptr(new CImpl())
{
	m_impl_ptr->Create(
		_target_wnd, _callback_msg_id, _tip, _icon, _menu_id, _is_hidden);
}


CSysTray::~CSysTray(void)
{
	SAFE_DELETE(const_cast<CImpl*>(m_impl_ptr));
}


bool CSysTray::Create(
	HWND _target_wnd, UINT _callback_msg_id, LPCTSTR _tip, 
	HICON _icon, UINT _menu_id, bool _is_hidden /* = false */
	)
{
	return m_impl_ptr->Create(
		_target_wnd, _callback_msg_id, _tip, _icon, _menu_id, _is_hidden);
}


void CSysTray::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	m_impl_ptr->OnTrayNotification(wParam, lParam);
}


W2X_DEFINE_NAME_SPACE_END(ui)
W2X_NAME_SPACE_END
/******************************************************************************
 * 文件:		sys_tray.h
 * 描述:		用于构建系统托盘图标及添加托盘图标右键菜单等。
 * 作者:		wu.xiongxing					
 * 邮箱:		wxxweb@gmail.com
 * 日期:		2014-03-17
*******************************************************************************/

#ifndef __W2X_COMMON_SYS_TRAY_H__
#define __W2X_COMMON_SYS_TRAY_H__


#include "exports.h"
#include "macros.h"
#include "base.h"

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(ui)


class W2X_COMMON_API CSysTray: public IBase
{
public:
	CSysTray(void);
	CSysTray(
		HWND _main_wnd, UINT _callback_msg_id, LPCTSTR _tip, 
		HICON _icon, UINT _menu_id, bool _is_hidden = false);
	~CSysTray(void);

W2X_IMPLEMENT_REFCOUNTING(CSysTray)

public:
	bool Create(
		HWND _main_wnd, UINT _callback_msg_id, LPCTSTR _tip, 
		HICON _icon, UINT _menu_id, bool _is_hidden = false
		);

	void OnTrayNotification(WPARAM wParam, LPARAM lParam);

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(ui)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_SYS_TRAY_H__ */

/******************************************************************************
�ļ�:	sys_tray.h
����:	���ڹ���ϵͳ����ͼ�꼰�������ͼ���Ҽ��˵��ȡ�
����:	wu.xiongxing					
����:	wxxweb@gmail.com
����:	2014-03-17
�޸�:	2014-05-21
*******************************************************************************/

#ifndef __W2X_COMMON_SYS_TRAY_H__
#define __W2X_COMMON_SYS_TRAY_H__

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

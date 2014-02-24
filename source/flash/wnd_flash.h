/*****************************************************************************
 * ������	wnd_flash.h
 * ������	���� FLASH �������ڷ�װ�� CWndFlash��
 * ���ߣ�	wu.xiongxing					
 * ���䣺	wxxweb@gmail.com
 * ���ڣ�	2014-02-17
 *****************************************************************************/

#ifndef __W2X_FLASH_WND_FALSH_H__
#define __W2X_FLASH_WND_FALSH_H__


#include "exports.h"
#include "..\common\macros.h"
#include "..\common\event_dispatcher.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(ui)


class W2X_FLASH_API CWndFlash
{
public:
	CWndFlash(void);
	virtual ~CWndFlash(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CWndFlash)

public:
	bool Create(
		LPCTSTR _swf_file_path, 
		HWND _parent_wnd_handle, 
		HINSTANCE _instance_handle);

	void Destroy(void);

	HWND GetHwnd(void) const;

	LPCTSTR CallFunction(LPCTSTR _funcion, LPCTSTR _arg);

	bool AddEventListener(LPCTSTR _type_name, 
		w2x::events::FEventListener _listener
	);

	bool RemoveEventListener(
		LPCTSTR _type_name, 
		w2x::events::FEventListener _listener
	);

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(ui)
W2X_NAME_SPACE_END


#endif /* __W2X_FLASH_WND_FALSH_H__ */
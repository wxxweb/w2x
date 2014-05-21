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
#include "w2x_common\macros.h"
#include "w2x_common\event_dispatcher.h"
#include "w2x_common\variable.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(ui)


class W2X_FLASH_API CWndFlash
{
public:
	CWndFlash(void);
	virtual ~CWndFlash(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CWndFlash)
W2X_IMPLEMENT_REFCOUNTING(CWndFlash)

public:
	bool Create(
		LPCTSTR _wnd_name,
		LPCTSTR _swf_file_path, 
		HWND _parent_wnd_handle, 
		HINSTANCE _instance_handle);

	HWND GetHwnd(void) const;

	LPCTSTR GetName(void) const;

	void SetCaptionArea(WORD _x, WORD _y, WORD _width, WORD _height);

	bool CallFunction(LPCTSTR _function, const w2x::Var& _args);

	/** @see CEventDispatch */
	virtual bool AddEventListener(
		LPCTSTR _type_name,
		w2x::events::EventListener _listener,
		PVOID _param
	);

	/** @see CEventDispatch */
	virtual bool HasEventListener(LPCTSTR _type_name) const;

	/** @see CEventDispatch */
	virtual bool RemoveEventListener(
		LPCTSTR _type_name, 
		w2x::events::EventListener _listener
	);

	static bool IsFlashPlayerInstalled(void);

	static bool EnsureFlashPlayerInstalled(void);

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(ui)
W2X_NAME_SPACE_END


#endif /* __W2X_FLASH_WND_FALSH_H__ */
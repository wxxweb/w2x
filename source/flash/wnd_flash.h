/*****************************************************************************
文件名：		wnd_flash.h
描述：		定义 FLASH 动画窗口封装类 CWndFlash。
作者：		wu.xiongxing					
邮箱：		wxxweb@gmail.com
日期：		2014.02.17
******************************************************************************/

#ifndef __W2X_FLASH_WND_FALSH_H__
#define __W2X_FLASH_WND_FALSH_H__


#include "exports.h"
#include "..\common\macros.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(ui)


class IFlashEventListener
{
public:
	IFlashEventListener(void) {}
	virtual ~IFlashEventListener(void) {}

public:
	virtual void OnFlashBeforeShow(void) = 0;
	virtual void OnFlashAfterShow(void) = 0;
	virtual void OnFlashEnterFrame(void) = 0;
	virtual void OnFlashCommand(LPCTSTR _command, LPCTSTR _args) = 0;
};


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

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(ui)
W2X_NAME_SPACE_END


#endif /* __W2X_FLASH_WND_FALSH_H__ */
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


class CWndFlashImpl;


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

private:
	CWndFlashImpl* m_wnd_ptr;
};


W2X_DEFINE_NAME_SPACE_END(ui)
W2X_NAME_SPACE_END


#endif /* __W2X_FLASH_WND_FALSH_H__ */
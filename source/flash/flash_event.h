/*****************************************************************************
 * 文名：	flash_event.h
 * 描述：	CFlashEvent 类为 FLASH 动画事件类。
 * 作者：	wu.xiongxing					
 * 邮箱：	wxxweb@gmail.com
 * 日期：	2014-02-17
 *****************************************************************************/

#ifndef __W2X_FLASH_EVENT_H__
#define __W2X_FLASH_EVENT_H__


#include "exports.h"
#include "..\common\macros.h"
#include "..\common\event.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(ui)


class CFlashEvent: public w2x::events::CEvent
{
public:
	static LPCTSTR EVENT_COMMAND;
	static LPCTSTR EVENT_BEFORE_SHOW;
	static LPCTSTR EVENT_AFTER_SHOW;

public:
	CFlashEvent(LPCTSTR _type_name);
	virtual ~CFlashEvent(void);

public:
	void SetCommand(LPCTSTR _command, LPCTSTR _args);

	LPCTSTR GetCommand(void) const;

	LPCTSTR GetCommandArgs(void) const;

private:
	class CFlashEventImpl;
	CFlashEventImpl* const m_flash_event_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(ui)
W2X_NAME_SPACE_END


#endif /* __W2X_FLASH_EVENT_H__ */
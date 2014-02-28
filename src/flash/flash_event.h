/*****************************************************************************
 * ������	flash_event.h
 * ������	CFlashEvent ��Ϊ FLASH �����¼��ࡣ
 * ���ߣ�	wu.xiongxing					
 * ���䣺	wxxweb@gmail.com
 * ���ڣ�	2014-02-17
 *****************************************************************************/

#ifndef __W2X_FLASH_EVENT_H__
#define __W2X_FLASH_EVENT_H__


#include "exports.h"
#include "..\common\macros.h"
#include "..\common\event.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)


class W2X_FLASH_API CFlashEvent: public w2x::events::CEvent
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


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END


#endif /* __W2X_FLASH_EVENT_H__ */
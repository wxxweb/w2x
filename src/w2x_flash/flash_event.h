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
#include "..\w2x_common\macros.h"
#include "..\w2x_common\event.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)


class W2X_FLASH_API CFlashEvent: public w2x::events::CEvent
{
public:
	static const TCHAR EVENT_FS_COMMAND[];
	static const TCHAR EVENT_FLASH_CALL[];
	static const TCHAR EVENT_ENTER_MAIN[];
	static const TCHAR EVENT_LOAD_PROGRESS[];
	static const TCHAR EVENT_STAGE_SIZE[];
	static const TCHAR EVENT_MOUSE_CLICK[];

public:
	CFlashEvent(LPCTSTR _type_name, LPCTSTR _command, LPCTSTR _args);
	virtual ~CFlashEvent(void);

public:
	virtual void Reset(void);

	LPCTSTR GetCommand(void) const;
	void SetCommand(LPCTSTR _command);

	LPCTSTR GetArguments(void) const;
	void SetArguments(LPCTSTR _args);

	void SetStageSize(int _width, int _height);
	int GetStageWidth(void) const;
	int GetStageHeight(void) const;

	void SetUiTraget(LPCTSTR _id);
	LPCTSTR GetUiTraget(void) const;

private:
	class CFlashEventImpl;
	CFlashEventImpl* const m_flash_event_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END


#endif /* __W2X_FLASH_EVENT_H__ */
/*****************************************************************************
 * 文名：	flash_event.h
 * 描述：	CFlashEvent 类为 FLASH 动画事件类。
 * 作者：	wu.xiongxing					
 * 邮箱：	wxxweb@gmail.com
 * 日期：	2014-02-17
 *****************************************************************************/

#include "stdafx.h"
#include "flash_event.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(events)


const TCHAR CFlashEvent::EVENT_FS_COMMAND[] = TEXT("SwfFSCommand");
const TCHAR CFlashEvent::EVENT_FLASH_CALL[] = TEXT("SwfFlashCall");
const TCHAR CFlashEvent::EVENT_ENTER_MAIN[] = TEXT("SwfEnterMain");
const TCHAR CFlashEvent::EVENT_LOAD_PROGRESS[] = TEXT("SwfLoadProgress");
const TCHAR CFlashEvent::EVENT_STAGE_SIZE[] = TEXT("SwfStageSize");
const TCHAR CFlashEvent::EVENT_MOUSE_CLICK[] = TEXT("SwfMouseClick");


class CFlashEvent::CFlashEventImpl
{
public:
	CFlashEventImpl(void);
	~CFlashEventImpl(void);

public:
	inline void Reset(void);

public:
	TSTDSTR m_str_command;
	TSTDSTR m_str_args;
	TSTDSTR m_ui_target_id;
	int m_stage_width;
	int m_stage_height;
};


CFlashEvent::CFlashEventImpl::CFlashEventImpl(void)
	: m_str_command(TEXT(""))
	, m_str_args(TEXT(""))
	, m_ui_target_id(TEXT(""))
	, m_stage_width(0)
	, m_stage_height(0)
{
}


CFlashEvent::CFlashEventImpl::~CFlashEventImpl(void)
{

}


inline void CFlashEvent::CFlashEventImpl::Reset(void)
{
	m_str_command = TEXT("");
	m_str_args = TEXT("");
	m_ui_target_id = TEXT("");
	m_stage_width = 0;
	m_stage_height = 0;
}


CFlashEvent::CFlashEvent(LPCTSTR _type_name, LPCTSTR _command, LPCTSTR _args)
	: CEvent(_type_name)
	, m_flash_event_impl_ptr(new CFlashEventImpl())
{
	m_flash_event_impl_ptr->m_str_command 
		= (NULL != _command) ? _command : TEXT("");
	m_flash_event_impl_ptr->m_str_args
		= (NULL != _args) ? _args : TEXT("");
}


CFlashEvent::~CFlashEvent(void)
{
	SAFE_DELETE(const_cast<CFlashEventImpl*>(m_flash_event_impl_ptr));
}


void CFlashEvent::Reset(void)
{
	CEvent::Reset();
	m_flash_event_impl_ptr->Reset();
}


LPCTSTR CFlashEvent::GetCommand(void) const
{
	return m_flash_event_impl_ptr->m_str_command.c_str();
}


void CFlashEvent::SetCommand(LPCTSTR _command)
{
	if (NULL == _command) {
		_command = TEXT("");
	}
	m_flash_event_impl_ptr->m_str_command = _command;
}


LPCTSTR CFlashEvent::GetArguments(void) const
{
	return m_flash_event_impl_ptr->m_str_args.c_str();
}


void CFlashEvent::SetArguments(LPCTSTR _args)
{
	if (NULL == _args) {
		_args = TEXT("");
	}
	m_flash_event_impl_ptr->m_str_args = _args;
}


void CFlashEvent::SetStageSize(int _width, int _height)
{
	m_flash_event_impl_ptr->m_stage_width = _width;
	m_flash_event_impl_ptr->m_stage_height = _height;
}


int CFlashEvent::GetStageWidth(void) const
{
	return m_flash_event_impl_ptr->m_stage_width;
}


int CFlashEvent::GetStageHeight(void) const
{
	return m_flash_event_impl_ptr->m_stage_height;
}


void CFlashEvent::SetUiTraget(LPCTSTR _id)
{
	if (NULL == _id) {
		_id = TEXT("");
	}
	m_flash_event_impl_ptr->m_ui_target_id = _id;
}


LPCTSTR CFlashEvent::GetUiTraget(void) const
{
	return m_flash_event_impl_ptr->m_ui_target_id.c_str();
}

W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END

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


LPCTSTR CFlashEvent::EVENT_COMMAND		= TEXT("Command");
LPCTSTR CFlashEvent::EVENT_BEFORE_SHOW	= TEXT("BeforeShow");
LPCTSTR CFlashEvent::EVENT_AFTER_SHOW	= TEXT("AfterShow");


class CFlashEvent::CFlashEventImpl
{
public:
	CFlashEventImpl(void);
	~CFlashEventImpl(void);

public:
	TSTDSTR m_str_command;
	TSTDSTR m_str_args;
};


CFlashEvent::CFlashEventImpl::CFlashEventImpl(void)
{

}


CFlashEvent::CFlashEventImpl::~CFlashEventImpl(void)
{

}


CFlashEvent::CFlashEvent(LPCTSTR _type_name)
	: CEvent(_type_name)
	, m_flash_event_impl_ptr(new CFlashEventImpl())
{

}


CFlashEvent::~CFlashEvent(void)
{
	SAFE_DELETE(const_cast<CFlashEventImpl*>(m_flash_event_impl_ptr));
}


void CFlashEvent::SetCommand(LPCTSTR _command, LPCTSTR _args)
{
	m_flash_event_impl_ptr->m_str_command 
		= (NULL != _command) ? _command : TEXT("");
	m_flash_event_impl_ptr->m_str_args 
		= (NULL != _args) ? _args : TEXT("");
}


LPCTSTR CFlashEvent::GetCommand(void) const
{
	return m_flash_event_impl_ptr->m_str_command.c_str();
}

LPCTSTR CFlashEvent::GetCommandArgs(void) const
{
	return m_flash_event_impl_ptr->m_str_args.c_str();
}


W2X_DEFINE_NAME_SPACE_END(events)
W2X_NAME_SPACE_END

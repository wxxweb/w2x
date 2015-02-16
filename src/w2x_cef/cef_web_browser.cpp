/*****************************************************************************
文件名：		DlgPhoneWebFrame.cpp
描述：		
作者：		吴雄兴					
版权：		www.utooo.com
日期：		2013.08.26
修改历史：
******************************************************************************/

#include "stdafx.h"
#include "cef_web_browser.h"
#include "cef_web_browser_impl.h"
#include "cef_web_browser_event_handler_i.h"
#include <string>
#include <shlwapi.h>


CCefWebBrowser::CCefWebBrowser(void)
	: m_impl_ptr(new CCefWebBrowserImpl)
{
	m_impl_ptr->AddRef();
}

CCefWebBrowser::~CCefWebBrowser(void)
{
	m_impl_ptr->Close();
	m_impl_ptr->Release();
}


HWND CCefWebBrowser::GetHwnd(void) const
{
	return m_impl_ptr->GetBrowserHwnd();
}


void CCefWebBrowser::SetCaptionMargin(
	int _height, 
	int _margin_top, 
	int _margin_left, 
	int _margin_right
	)
{
	m_impl_ptr->SetCaptionMargin(_height, _margin_top, _margin_left, _margin_right);
}


bool CCefWebBrowser::AddExternal(const TSTDSTR& _fn_name)
{
	return m_impl_ptr->AddExternal(_fn_name);
}


bool CCefWebBrowser::Create(
	LPCTSTR _url,
	HWND _parent_hwnd,
	ICefWebBrowserEventHandler* _handler
	)
{
	if (NULL == _url) {
		_url = TEXT("about:blank");
	}

	TCHAR path[MAX_PATH] = TEXT("");
	if (TRUE == ::PathFileExists(_url)) {
		::GetFullPathName(_url, MAX_PATH, path, NULL);
	} else {
		_tcscpy_s(path, _url);
	}
	if (true == m_impl_ptr->Create(path, _parent_hwnd, _handler))
	{
		if (NULL != _handler) {
			_handler->RegisterWebBrowser(this);
		}
		return true;
	}
	return false;
}

void CCefWebBrowser::Destory(void)
{
	return m_impl_ptr->Close();
}

bool CCefWebBrowser::MoveBrowser(int _x, int _y, int _width, int _height)
{
	return m_impl_ptr->MoveBrowser(_x, _y, _width, _height);
}

TSTDSTR CCefWebBrowser::GetUrl(void) const
{
	return m_impl_ptr->GetUrl();
}

bool CCefWebBrowser::LoadUrl(LPCTSTR _url)
{
	return m_impl_ptr->LoadUrl(_url);
}

bool CCefWebBrowser::ExecuteJsCode(const TSTDSTR& _js_code)
{
	return m_impl_ptr->AddJsCode(_js_code);
}

bool CCefWebBrowser::CallJsFunction(
	const TSTDSTR& _fn_name,
	const TSTDSTR& _args
	)
{
	return m_impl_ptr->CallJsFunction(_fn_name, _args);
}

bool CCefWebBrowser::RegisterCustomScheme(
	LPCTSTR _scheme_name,
	CustomSchemeHandler _handler
	)
{
	return CCefWebBrowserImpl::RegisterCustomScheme(_scheme_name, _handler);
}

bool CCefWebBrowser::Initialize(
	LPCTSTR _locales_path,
	LPCTSTR _cache_path,
	LPCTSTR _log_file
	)
{
	return CCefWebBrowserImpl::Initialize(_locales_path, _cache_path, _log_file);
}

void CCefWebBrowser::Uninitialize(void)
{
	CCefWebBrowserImpl::Uninitialize();
}


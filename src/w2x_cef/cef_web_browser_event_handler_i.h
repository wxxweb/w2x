/*****************************************************************************
�ļ�:	ICefWebBrowserEventHandler.h
����:	���� Chromiumem Ƕ��ʽ������¼��������ӿڡ�
����:	������					
��Ȩ:	www.utooo.com
����:	2013-09-21
�޸�:	2014-05-16
******************************************************************************/

#ifndef __W2X_CEF_WEB_BROWSER_EVENT_HANDLER_H__
#define __W2X_CEF_WEB_BROWSER_EVENT_HANDLER_H__
#pragma once

#include "exports.h"
//#include "w2x_common/macros.h"
#include <string>

/** ����UNICODE����ʹ�ÿ��ַ�����ַ���C++�ַ��� */
#ifndef TSTDSTR
#  ifdef UNICODE
#    define TSTDSTR std::wstring
#  else
#    define TSTDSTR std::string
#  endif
#endif

class ICefWebBrowser;

class W2X_CEF_API ICefWebBrowserEventHandler
{
public:
	ICefWebBrowserEventHandler(void) {}
	virtual ~ICefWebBrowserEventHandler(void) {}

public:
	/** ע��������ӿ� */
	virtual bool RegisterWebBrowser(ICefWebBrowser* _cef_web_browser) = 0;

	/** ע��������ӿ� */
	virtual void UnregisterWebBrowser(void) = 0;

	/** ����������ڱ�������ص� */
	virtual void OnAfterCreated(const TSTDSTR& _startup_url) {}

	/** ����������ڱ��ر�ǰ�ص� */
	virtual void OnBeforeClose(void) {}

	/** �� Web �ĵ���������ص� */
	virtual void OnContextCreated(const TSTDSTR& _url) {}

	/** �� Web �ĵ���ʼ����ʱ�ص� */
	virtual void OnLoadStart(const TSTDSTR& _url) {}

	/** �� Web �ĵ����ؽ���ʱ�ص� */
	virtual void OnLoadEnd(const TSTDSTR& _url) {}

	/** �� Web �ĵ����س���ʱ�ص� */
	virtual bool OnLoadError(
		/* [out] */ TSTDSTR& _error_text,
		/* [in] */ const TSTDSTR& _failed_url,
		/* [in] */ DWORD _error_code) 
	{ return true; }

	/** ������״̬�ı�ʱ���ص� */
	virtual void OnNavStateChange(
		const TSTDSTR& _url,
		bool canGoBack,
		bool canGoForward
		) {}

	/** �� URL ��ַ�����ı�ʱ���ص� */
	virtual void OnAddressChange(const TSTDSTR& _url) {}

	/**
	 * ִ�� JS �ķ������ã����뷽�������ƺͲ����ַ������������䷵��ֵ��
	 * �������������� true��
	 */
	virtual bool OnJsInvoke(
		LPCTSTR _fn_name,
		LPCTSTR _args)
	{ return true; }

	/** �����յ�����̨��Ϣʱ���ص� */
	virtual void OnConsoleMessage(const TSTDSTR& _msg) {}
	
	/** �� JavaScript �ű�����δ�����쳣ʱ���ص� */
	virtual void OnUncaughtException(
		const TSTDSTR& _msg,
		const TSTDSTR& _file_name,
		const TSTDSTR& _src_line,
		int _line_num,
		int _column) 
	{}
};

#endif ///< __W2X_CEF_WEB_BROWSER_EVENT_HANDLER_H__
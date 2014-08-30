/*****************************************************************************
�ļ�:		ICefWebBrowser.h
����:		��װ�� Chromiumem �������ܵ� Web ��������ڽӿڡ�
����:		������					
��Ȩ:		www.utooo.com
����:		2014-02-22
�޸�:		2014-05-16
******************************************************************************/


#ifndef __W2X_CEF_WEB_BROWSER_INTERFACE_H__
#define __W2X_CEF_WEB_BROWSER_INTERFACE_H__
#pragma once


#include <string>


#ifdef W2X_CEF_EXPORTS
#  ifndef  W2X_CEF_API
#    define W2X_CEF_API __declspec(dllexport)
#  endif
#else
#  ifndef W2X_CEF_API
#    define W2X_CEF_API __declspec(dllimport)
#  endif
#endif


/** ����UNICODE����ʹ�ÿ��ַ�����ַ���C++�ַ��� */
#ifndef TSTDSTR
#  ifdef UNICODE
#    define TSTDSTR std::wstring
#  else
#    define TSTDSTR std::string
#  endif
#endif


class ICefWebBrowser
{
public:
	virtual ~ICefWebBrowser(void) = 0 {}

public:
	/**
	 * @brief ִ�� JavaScript ���롣
	 *
	 * ����һ�� JavaScript �ַ������룬���� Web ҳ��ִ�У�
	 * �紫���ַ��� "alert('Hello World');"��Web ҳ�潫������ʾ "Hello World"��
	 *
	 * @return ������������δ������ɷ��� false�����򷵻� true��
	 */
	virtual bool ExecuteJsCode(const TSTDSTR& _js_code) = 0;

	/**
	 * @brief ���� JavaScript ������
	 *
	 * ����ָ���ĺ������������� Web ҳ���� JavaScript �����������ú�����ȫ���������������£�
	 * function _fn_name(_args);
	 *
	 * @param _fn_name �����õ� JavaScript �������ơ�
	 * @param _args �����õ� JavaScript �������ַ����������ɴ��� JSON �ַ�������չ������
	 * @return ������������δ������ɷ��� false�����򷵻� true��
	 */
	virtual bool CallJsFunction(
		const TSTDSTR& _fn_name,
		const TSTDSTR& _args
		) = 0;
};


#endif ///< !__W2X_CEF_WEB_BROWSER_INTERFACE_H__
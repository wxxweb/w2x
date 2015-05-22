/*******************************************************************************
�ļ�:	cef_web_browser.h
����:	��װ�� CCefWebBrowser ��װ�� Chromiumem Ƕ��ʽ�������ܵ� Web ��������ڣ�
		���ⲿ�ṩʹ�ýӿڣ������ֱ��ʹ��������ʾ Web ҳ�棬 ���߼̳���������Ӧ�Ĵ���
		������ʵ�� JavaScript �� C++ �Ľ�����
����:	������
��Ȩ:	www.utooo.com
����:	2013-08-26
�޸�:	2014-05-16
*******************************************************************************/

#ifndef __W2X_CEF_WEB_BROWSER_H__
#define __W2X_CEF_WEB_BROWSER_H__
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


class CCefWebBrowserImpl;
class ICefWebBrowserEventHandler;

class W2X_CEF_API CCefWebBrowser
{
public:
	/** �Զ���Э�鴦���������벶�񵽵Ķ�Ӧ URL */
	typedef bool (CALLBACK* CustomSchemeHandler)(
		LPCTSTR _scheme_name, 
		LPCTSTR _url
		);

public:
	/**
	 * @note ����ǰ������þ�̬��Ա���� Initialize ����ʼ��ȫ��������Ϣ��֮������ó�Ա���� Create ������ Web ��������ڡ�
	 */
	CCefWebBrowser(void);

	virtual ~CCefWebBrowser(void);

public:
	/**
	 * @brief ִ�� JavaScript ���롣
	 *
	 * ����һ�� JavaScript �ַ������룬���� Web ҳ��ִ�У�
	 * �紫���ַ��� "alert('Hello World');"��Web ҳ�潫������ʾ "Hello World"��
	 *
	 * @return ������������δ������ɷ��� false�����򷵻� true��
	 */
	bool ExecuteJsCode(LPCTSTR _js_code);

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
	bool CallJsFunction(const TSTDSTR& _fn_name, const TSTDSTR& _args);

	/**
	 * @brief ���� Web ��������ڡ�
	 * 
	 * @see ICefWebBrowserEventHandler
	 * @param _url ������ʾ����������ڵ� Web ҳ��� URL��
	 * @param _parent_hwnd ӵ�� Web ��������ڵĸ����ھ����
	 * @param _handler Web ������¼������������ָ�룬�� Web ����������¼�ʱ�����������������á�
	 * @return �����ɹ����� true��
	 */
	bool Create(
		LPCTSTR _url,
		HWND _parent_hwnd = NULL,
		ICefWebBrowserEventHandler* _handler = NULL
	);

	/** �ر�������������ڡ�*/
	void Destory(void);

	/** ��ȡ��������ھ����*/
	HWND GetHwnd(void) const;

	/** �ƶ����������λ�á�*/
	bool MoveBrowser(int _x, int _y);

	/** �ı���������ڴ�С��*/
	bool ChangeBrowserSize(int _width, int _height);

	/**
	 * @brief ���ñ��������εĸ߶ȼ���࣬��������϶���������ڡ�
	 * 
	 * @param _height ���������θ߶ȡ�
	 * @param _margin_top �����������ϱ߾࣬������������ڶ����ľ��롣
	 * @param _margin_left ������������߾࣬����������������ľ��롣
	 * @param _margin_right �����������ұ߾࣬��������������Ҳ�ľ��롣
	 */
	void SetCaptionMargin(
		int _height = 0, 
		int _margin_top = 0, 
		int _margin_left = 0, 
		int _margin_right = 0
		);

	/**
	 * @brief ����ṩ�� JavaScript ���õ��ⲿ������
	 * 
	 * ָ��һ���ⲿ�������� _fn_name���ⲿ�������������¶�� JavaScript ���á�����ָ����������
	 * Ϊ "notifyCpp"������� JavaScript �е��ã�"window.external.notifyCpp(args);"��
	 * ������ⲿ��������Ϊһ���ַ������������ JSON �ַ�������չ���õ��ý�����������¼����¼���
	 * ���������� ICefWebBrowserEventHandler::OnJsInvoke ���񣬾��崦������μ�������˵����
	 *
	 * @see ICefWebBrowserEventHandler
	 * @note �ڳ�Ա���� Create ֮ǰ���ò���Ч��
	 * @param _fn_name �ṩ�� JavaScript ���õ��ⲿ�������ơ�
	 * @return �ⲿ�������� _fn_name Ϊ�գ���������������Ѿ������򷵻� false��
	 */
	bool AddExternal(const TSTDSTR& _fn_name);

	/** ��ȡ��ǰ��ʾ Web ҳ��� URL��*/
	TSTDSTR GetUrl(void) const;

	/** ����ָ�� URL��*/
	bool LoadUrl(LPCTSTR _url);

	/**
	 * @brief ��ʼ��ȫ����������á�
	 * 
	 * ��ʼ��ȫ����������ã������û��汣��Ŀ¼·������־�ļ�ȫ·�����ơ�
	 *
	 * @note �ú������� CCefWebBrowser �������֮ǰ���ã����������������ڴ����á�
	 * @param _locales_path ���԰�Ŀ¼·����
	 * @param _cache_path ��������汣��Ŀ¼·����
	 * @param _log_file �������־�ļ�ȫ·�����ơ�
	 * @return ��ʼ���ɹ����� true��
	 */
	static bool Initialize(
		LPCTSTR _locales_path,
		LPCTSTR _cache_path,
		LPCTSTR _log_file
		);

	/**
	 * @brief �ͷ�ȫ���������Դ��
	 * 
	 * @note �˺����������������˳�ʱ���ã����򽫵���ģ���޷�ж�ض�ʹ����������޷��˳���
	 */
	static void Uninitialize(void);

	/**
	 * @brief ע���Զ���Э�顣
	 * 
	 * ע���Զ���Э�飬ָ��Э�����ƣ�����������Э�飨http��ftp�ȣ�������ϵͳ������ָ��Э���
	 * URL��ָ�����¼����������� _handler ��������������� URL�����磬ָ��Э������Ϊ "cef"��
	 * ��ĳ������������� IE����ַ������ "cef://www.xyz.me/x=1&y=2"���� URL �������񵽣�
	 * �����ɴ�������������
	 *
	 * @see CustomSchemeHandler
	 * @note ����ú���������������ڴ����á�
	 * @param _scheme_name �Զ���Э�����ơ�
	 * @param _handler �Զ���Э�鴦��������ָ�룬�������û����壬��Ϊ NULL���򲻴����񵽵� URL��
	 * @return ע��ɹ����� true��
	 */
	static bool RegisterCustomScheme(
		LPCTSTR _scheme_name, 
		CustomSchemeHandler _handler
		);

private:
	CCefWebBrowserImpl* const m_impl_ptr;
};

#endif ///< !__W2X_CEF_WEB_BROWSER_H__
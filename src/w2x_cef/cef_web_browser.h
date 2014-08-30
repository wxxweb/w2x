/*******************************************************************************
文件:	cef_web_browser.h
描述:	包装类 CCefWebBrowser 包装了 Chromiumem 嵌入式浏览器框架的 Web 浏览器窗口，
		向外部提供使用接口，你可以直接使用它来显示 Web 页面， 或者继承它，做相应的处理。
		它可以实现 JavaScript 和 C++ 的交互。
作者:	吴雄兴
版权:	www.utooo.com
日期:	2013-08-26
修改:	2014-05-16
*******************************************************************************/

#ifndef __W2X_CEF_WEB_BROWSER_H__
#define __W2X_CEF_WEB_BROWSER_H__
#pragma once


#include "cef_web_browser_i.h"


class ICefWebBrowserEventHandler;
class CCefWebBrowserImpl;

class W2X_CEF_API CCefWebBrowser: public ICefWebBrowser
{
public:
	/** 自定义协议处理器，传入捕获到的对应 URL */
	typedef bool (CALLBACK* CustomSchemeHandler)(
		LPCTSTR _scheme_name, 
		LPCTSTR _url
		);

public:
	/**
	 * @note 构建前必须调用静态成员函数 Initialize 来初始化全局配置信息，之后须调用成员函数 Create 来创建 Web 浏览器窗口。
	 */
	CCefWebBrowser(void);

	virtual ~CCefWebBrowser(void);

public:
	/**
	 * @brief 执行 JavaScript 代码。
	 * @see ICefWebBrowser
	 */
	virtual bool ExecuteJsCode(const TSTDSTR& _js_code);

	/**
	 * @brief 调用 JavaScript 函数。
	 * @see ICefWebBrowser
	 */
	virtual bool CallJsFunction(const TSTDSTR& _fn_name, const TSTDSTR& _args);

public:
	/**
	 * @brief 创建 Web 浏览器窗口。
	 * 
	 * @see ICefWebBrowserEventHandler
	 * @param _url 用于显示在浏览器窗口的 Web 页面的 URL。
	 * @param _parent_hwnd 拥有 Web 浏览器窗口的父窗口句柄。
	 * @param _handler Web 浏览器事件处理器类对象指针，当 Web 浏览器产生事件时，处理器函数被调用。
	 * @return 创建成功返回 true。
	 */
	bool Create(
		LPCTSTR _url,
		HWND _parent_hwnd = NULL,
		ICefWebBrowserEventHandler* _handler = NULL
	);

	/** 关闭销毁浏览器窗口。*/
	void Destory(void);

	/** 获取浏览器窗口句柄。*/
	HWND GetHwnd(void) const;

	/** 移动及改变浏览器窗口位置及大小。*/
	bool MoveBrowser(int _x, int _y, int _width, int _height);

	/**
	 * @brief 设置标题栏矩形的高度及间距，用于鼠标拖动浏览器窗口。
	 * 
	 * @param _height 标题栏矩形高度。
	 * @param _margin_top 标题栏矩形上边距，距离浏览器窗口顶部的距离。
	 * @param _margin_left 标题栏矩形左边距，距离浏览器窗口左侧的距离。
	 * @param _margin_right 标题栏矩形右边距，距离浏览器窗口右侧的距离。
	 */
	void SetCaptionMargin(
		int _height = 0, 
		int _margin_top = 0, 
		int _margin_left = 0, 
		int _margin_right = 0
		);

	/**
	 * @brief 添加提供给 JavaScript 调用的外部函数。
	 * 
	 * 指定一个外部函数名称 _fn_name，外部函数被浏览器暴露给 JavaScript 调用。例如指定函数名称
	 * 为 "notifyCpp"，则可在 JavaScript 中调用："window.external.notifyCpp(args);"。
	 * 传入的外部函数参数为一个字符串，建议采用 JSON 字符串来扩展。该调用将触发浏览器事件，事件由
	 * 处理器函数 ICefWebBrowserEventHandler::OnJsInvoke 捕获，具体处理方法请参见处理器说明。
	 *
	 * @see ICefWebBrowserEventHandler
	 * @note 在成员函数 Create 之前调用才有效。
	 * @param _fn_name 提供给 JavaScript 调用的外部函数名称。
	 * @return 外部函数名称 _fn_name 为空，或者浏览器窗口已经创建则返回 false。
	 */
	bool AddExternal(const TSTDSTR& _fn_name);

	/** 获取当前显示 Web 页面的 URL。*/
	TSTDSTR GetUrl(void) const;

	/** 加载指定 URL。*/
	bool LoadUrl(LPCTSTR _url);

	/**
	 * @brief 初始化全局浏览器配置。
	 * 
	 * 初始化全局浏览器配置，并配置缓存保存目录路径及日志文件全路径名称。
	 *
	 * @note 该函数须在 CCefWebBrowser 类对象构造之前调用，建议放在主程序入口处调用。
	 * @param _cache_path 浏览器缓存保存目录路径。
	 * @param _log_file 浏览器日志文件全路径名称。
	 * @return 初始化成功返回 true。
	 */
	static bool Initialize(LPCTSTR _cache_path, LPCTSTR _log_file);

	/**
	 * @brief 释放全局浏览器资源。
	 * 
	 * @note 此函数必须在主程序退出时调用，否则将导致模块无法卸载而使主程序进程无法退出。
	 */
	static void Uninitialize(void);

	/**
	 * @brief 注册自定义协议。
	 * 
	 * 注册自定义协议，指定协议名称，区别于内置协议（http、ftp等）。当在系统中运行指定协议的
	 * URL，指定的事件处理器函数 _handler 将被调用来捕获此 URL。例如，指定协议名称为 "cef"，
	 * 在某个浏览器（比如 IE）地址栏输入 "cef://www.xyz.me/x=1&y=2"，该 URL 将被捕获到，
	 * 并交由处理器函数处理。
	 *
	 * @see CustomSchemeHandler
	 * @note 建议该函数放在主程序入口处调用。
	 * @param _scheme_name 自定义协议名称。
	 * @param _handler 自定义协议处理器函数指针，函数由用户定义，若为 NULL，则不处理捕获到的 URL。
	 * @return 注册成功返回 true。
	 */
	static bool RegisterCustomScheme(
		LPCTSTR _scheme_name, 
		CustomSchemeHandler _handler
		);

private:
	CCefWebBrowserImpl* const m_impl_ptr;
};

#endif ///< !__W2X_CEF_WEB_BROWSER_H__
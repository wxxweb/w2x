/*****************************************************************************
文件:		ICefWebBrowser.h
描述:		封装了 Chromiumem 浏览器框架的 Web 浏览器窗口接口。
作者:		吴雄兴					
版权:		www.utooo.com
日期:		2014-02-22
修改:		2014-05-16
******************************************************************************/


#ifndef __W2X_CEF_WEB_BROWSER_INTERFACE_H__
#define __W2X_CEF_WEB_BROWSER_INTERFACE_H__
#pragma once

#include "exports.h"
#include <string>


/** 根据UNICODE决定使用宽字符或多字符版C++字符串 */
#ifndef TSTDSTR
#  ifdef UNICODE
#    define TSTDSTR std::wstring
#  else
#    define TSTDSTR std::string
#  endif
#endif


class W2X_CEF_API ICefWebBrowser
{
public:
	ICefWebBrowser(void) {}
	virtual ~ICefWebBrowser(void) = 0 {}

public:
	/**
	 * @brief 执行 JavaScript 代码。
	 *
	 * 传入一段 JavaScript 字符串代码，交由 Web 页面执行，
	 * 如传入字符串 "alert('Hello World');"，Web 页面将弹框显示 "Hello World"。
	 *
	 * @return 如果浏览器窗口未创建完成返回 false，否则返回 true。
	 */
	virtual bool ExecuteJsCode(const TSTDSTR& _js_code) = 0;

	/**
	 * @brief 调用 JavaScript 函数。
	 *
	 * 根据指定的函数名称来调用 Web 页面中 JavaScript 函数。所调用函数在全局作用域，声明如下：
	 * function _fn_name(_args);
	 *
	 * @param _fn_name 所调用的 JavaScript 函数名称。
	 * @param _args 所调用的 JavaScript 函数的字符串参数，可传入 JSON 字符串来扩展参数。
	 * @return 如果浏览器窗口未创建完成返回 false，否则返回 true。
	 */
	virtual bool CallJsFunction(
		const TSTDSTR& _fn_name,
		const TSTDSTR& _args
		) = 0;
};


#endif ///< !__W2X_CEF_WEB_BROWSER_INTERFACE_H__
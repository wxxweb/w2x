/*****************************************************************************
 * 文名：	ext_api_xml.h
 * 描述：	CExtApiXml 类用于封装和解析与 ActionScript 进行通信的外部 API 所使用的
 *          XML 字符串。
 *          该 XML 字符串是对函数调用也参数值进行的编码。
 * 作者：	wu.xiongxing					
 * 邮箱：	wxxweb@gmail.com
 * 日期：	2014-03-13
 *****************************************************************************/

#ifndef __W2X_EXT_API_XML_H__
#define __W2X_EXT_API_XML_H__


#include "exports.h"
#include "w2x_common/variable.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(ext)

class CExtApiXml
{
public:
	CExtApiXml(LPCTSTR _xml_str);
	CExtApiXml(LPCTSTR _function_name, const Var& _args);
	virtual ~CExtApiXml(void);

public:
	bool IsValid(void) const;
	size_t GetArgumentsSize(void) const;
	LPCTSTR GetFunctionName(void) const;
	bool SetFunctionName(LPCTSTR _name);
	bool GetArguments(OUT w2x::Var& _args) const;
	bool SetArguments(const w2x::Var& _args);
	LPCTSTR GetXmlString(void) const;

public:
	class CImpl;
	CImpl* const m_impl_ptr;
};


W2X_DEFINE_NAME_SPACE_END(ext)
W2X_NAME_SPACE_END

#endif /* __W2X_EXT_API_XML_H__ */
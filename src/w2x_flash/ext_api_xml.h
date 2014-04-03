/*****************************************************************************
 * ������	ext_api_xml.h
 * ������	CExtApiXml �����ڷ�װ�ͽ����� ActionScript ����ͨ�ŵ��ⲿ API ��ʹ�õ�
 *          XML �ַ�����
 *          �� XML �ַ����ǶԺ�������Ҳ����ֵ���еı��롣
 * ���ߣ�	wu.xiongxing					
 * ���䣺	wxxweb@gmail.com
 * ���ڣ�	2014-03-13
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
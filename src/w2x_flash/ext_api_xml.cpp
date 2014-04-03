/*****************************************************************************
 * 文名：	ext_api_xml.cpp
 * 描述：	参考 ext_api_xml.h
 * 作者：	wu.xiongxing					
 * 邮箱：	wxxweb@gmail.com
 * 日期：	2014-03-13
 *****************************************************************************/

#include "stdafx.h"
#include "ext_api_xml.h"
#include "w2x_common/macros.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(ext)

using namespace rapidxml;


class CExtApiXml::CImpl
{
public:
	CImpl(void);
	~CImpl(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CImpl)

public:
	bool ParseXml(LPCTSTR _xml_str);
	LPCTSTR GenerateXml(void);

	static bool GenerateApiXml(
		OUT TSTDSTR& _api_xml,
		LPCTSTR _fn_name,
		const Var& _arguments
		);

private:
	static bool ParseInvokeNode(
		OUT TSTDSTR& _fn_name,
		OUT Var& _args_value,
		LPCTSTR _xml_str
		);

	static bool ParsePropertyValueNode(
		OUT Var& _prop_value, 
		xml_node<TCHAR>* _prop_node
		);

	static bool ParseArrayNode(
		OUT Var::Array& _array, 
		xml_node<TCHAR>* _array_node
		);

	static bool ParseObjectNode(
		OUT Var::Object& _object, 
		xml_node<TCHAR>* _object_node
		);

	static bool GeneratePropertyXml(
		OUT TSTDSTR& _api_xml,
		const Var& _property
		);

public:
	TSTDSTR m_fn_name;
	TSTDSTR m_xml;
	bool m_is_valid;
	size_t m_args_size;
	Var* m_args_ptr;
};


CExtApiXml::CImpl::CImpl(void)
	: m_is_valid(false)
	, m_args_size(0)
	, m_args_ptr(NULL)
{

}


CExtApiXml::CImpl::~CImpl(void)
{
	SAFE_DELETE_ARRAY(m_args_ptr);
}


bool CExtApiXml::CImpl::ParseXml(LPCTSTR _xml_str)
{
	IF_NULL_ASSERT_RETURN_VALUE(_xml_str, false);

	SAFE_DELETE(m_args_ptr);
	m_args_ptr = new Var();
	if (false == CImpl::ParseInvokeNode(m_fn_name, *m_args_ptr, _xml_str)) {
		SAFE_DELETE(m_args_ptr);
		return false;
	}
	m_is_valid = true;
	m_args_size = m_args_ptr->GetLength();
	return true;
}


LPCTSTR CExtApiXml::CImpl::GenerateXml(void)
{
	if (false == m_xml.empty()) {
		return m_xml.c_str();
	}
	if (NULL == m_args_ptr) {
		m_args_ptr = new Var();
	}
	CImpl::GenerateApiXml(m_xml, m_fn_name.c_str(), *m_args_ptr);
	return m_xml.c_str();
}


bool CExtApiXml::CImpl::ParseInvokeNode(
	OUT TSTDSTR& _fn_name,
	OUT Var& _args_value,
	LPCTSTR _xml_str
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_xml_str, false);

	const size_t char_count = _tcslen(_xml_str) + 1;
	LPTSTR temp_xml_str = new TCHAR[char_count];
	memcpy(temp_xml_str, _xml_str, char_count * sizeof(TCHAR));

	xml_document<TCHAR> xml_doc;
	xml_doc.parse<0>(temp_xml_str);

	// <invoke> 根节点
	xml_node<TCHAR>* const invoke_node = xml_doc.first_node(TEXT("invoke"));
	IF_NULL_ASSERT (invoke_node) {
		SAFE_DELETE_ARRAY(temp_xml_str);
		return false;
	}

	// <invoke> 根节点的 name 属性
	xml_attribute<TCHAR>* node_attr = invoke_node->first_attribute(TEXT("name"));
	IF_NULL_ASSERT (node_attr) {
		SAFE_DELETE_ARRAY(temp_xml_str);
		return false;
	}

	// 函数名称
	LPCTSTR fn_name = node_attr->value();
	IF_NULL_ASSERT (fn_name) {
		SAFE_DELETE_ARRAY(temp_xml_str);
		return false;
	}
	_fn_name = fn_name;

	// 参数节点
	rapidxml::xml_node<TCHAR>* args_node
		= invoke_node->first_node(TEXT("arguments"));
	IF_NULL_ASSERT (args_node) {
		SAFE_DELETE_ARRAY(temp_xml_str);
		return false;
	}

	// 检测是否有参数
	xml_node<TCHAR>* property_node = args_node->first_node();
	if (NULL == property_node) {
		SAFE_DELETE_ARRAY(temp_xml_str);
		return true;
	}

	// 解析参数节点
	CImpl::ParsePropertyValueNode(_args_value, property_node);
	SAFE_DELETE_ARRAY(temp_xml_str);

	return true;
}


bool CExtApiXml::CImpl::ParsePropertyValueNode(
	OUT Var& _prop_value, 
	xml_node<TCHAR>* _prop_value_node
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_prop_value_node, false);

	// 基本数据类型参数
	LPCTSTR prop_type = _prop_value_node->name();
	LPCTSTR prop_value = _prop_value_node->value();
	if (0 == _tcscmp(prop_type, TEXT("null"))) {
		_prop_value = Var();
	}
	else if (0 == _tcscmp(prop_type, TEXT("true"))) {
		_prop_value = Var(true);
	}
	else if (0 == _tcscmp(prop_type, TEXT("false"))) {
		_prop_value = Var(false);
	}
	else if (0 == _tcscmp(prop_type, TEXT("string"))) {
		_prop_value = Var(prop_value);
	}
	else if (0 == _tcscmp(prop_type, TEXT("number"))) {
		if (NULL != _tcschr(prop_value, TEXT('.'))) {
			DOUBLE dbl = _ttof(prop_value);
			_prop_value = Var(dbl);
		}
		else if (NULL != _tcschr(prop_value, TEXT('-'))) {
			INT nt = _ttoi(prop_value);
			_prop_value = Var(nt);
		}
		else {
			UINT unt = static_cast<UINT>(_ttoi(prop_value));
			_prop_value = Var(unt);
		}
	}
	else if (0 == _tcscmp(prop_type, TEXT("array"))) {
		Var::Array ary;
		if (true == CImpl::ParseArrayNode(ary, _prop_value_node)) {
			_prop_value = Var(ary);
		}
	}
	else if (0 == _tcscmp(prop_type, TEXT("object"))) {
		Var::Object obj;
		if (true == CImpl::ParseObjectNode(obj, _prop_value_node)) {
			_prop_value = Var(obj);
		}
	}
	else {
		return false;
	}

	return true;
}


bool CExtApiXml::CImpl::ParseArrayNode(
	OUT Var::Array& _array,
	xml_node<TCHAR>* _array_node
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_array_node, false);

	// 遍历容器节点的子节点
	for (xml_node<TCHAR>* prop_node = _array_node->first_node(TEXT("property")); 
		 NULL != prop_node; prop_node = prop_node->next_sibling(TEXT("property")))
	{
		xml_node<TCHAR>* prop_value_node = prop_node->first_node();
		IF_NULL_ASSERT (prop_value_node) {
			continue;
		}
		Var val;
		if (false == CImpl::ParsePropertyValueNode(val, prop_value_node)) {
			continue;
		}
		_array.push_back(val);
	}
	return true;
}


bool CExtApiXml::CImpl::ParseObjectNode(
	OUT Var::Object& _object,
	xml_node<TCHAR>* _object_node
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_object_node, false);

	// 遍历容器节点的子节点
	for (xml_node<TCHAR>* prop_node = _object_node->first_node(TEXT("property")); 
		NULL != prop_node; prop_node = prop_node->next_sibling(TEXT("property")))
	{
		// 获取子节点的 id 属性
		xml_attribute<TCHAR>* prop_id
			= prop_node->first_attribute(TEXT("id"));
		IF_NULL_ASSERT (prop_id) {
			continue;
		}
		xml_node<TCHAR>* prop_value_node = prop_node->first_node();
		IF_NULL_ASSERT (prop_value_node) {
			continue;
		}
		Var val;
		if (false == CImpl::ParsePropertyValueNode(val, prop_value_node)) {
			continue;
		}
		LPCTSTR id_str = prop_id->value();
		_object[id_str] = val;
	}
	return true;
}


bool CExtApiXml::CImpl::GenerateApiXml(
	OUT TSTDSTR& _api_xml,
	LPCTSTR _fn_name,
	const Var& _arguments
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_fn_name, false);

	_api_xml =  TEXT("<invoke name=\"");
	_api_xml += _fn_name;
	_api_xml += TEXT("\" returntype=\"xml\"><arguments>");
	if (0 < _arguments.GetLength())
	{
		CImpl::GeneratePropertyXml(_api_xml, _arguments);
	}
	_api_xml += TEXT("</arguments></invoke>");

	return true;
}


bool CExtApiXml::CImpl::GeneratePropertyXml(
	OUT TSTDSTR& _api_xml,
	const Var& _property
	)
{
	const Var::Type prop_type = _property.GetType();
	switch (prop_type)
	{
	case Var::kNull:
		_api_xml += TEXT("<null/>");
		break;
	case Var::kBool:
		_api_xml += true == _property ? TEXT("<true/>") : TEXT("<false/>");
		break;
	case Var::kInt:
	case Var::kUint:
	case Var::kDword:
	case Var::kNumber:
		_api_xml += TEXT("<number>");
		_api_xml += _property;
		_api_xml += TEXT("</number>");
		break;
	case Var::kString:
		_api_xml += TEXT("<string>");
		_api_xml += _property;
		_api_xml += TEXT("</string>");
		break;
	case Var::kArray:
		_api_xml += TEXT("<array>");
		{
			Var::Array* prop_array = _property;
			if (NULL == prop_array || true == prop_array->empty()) {
				_api_xml += TEXT("<null/>");
			}
			else {
				const size_t elem_count = prop_array->size();
				for (size_t i = 0; i < elem_count; ++i)
				{
					_api_xml += TEXT("<property id=\"");
					_api_xml += Var(i);
					_api_xml += TEXT("\">");
					CImpl::GeneratePropertyXml(_api_xml, (*prop_array)[i]);
					_api_xml += TEXT("</property>");
				}
			}
		}
		_api_xml += TEXT("</array>");
		break;
	case Var::kObject:
		_api_xml += TEXT("<object>");
		{
			Var::Object* prop_obj = _property;
			if (NULL == prop_obj || true == prop_obj->empty()) {
				_api_xml += TEXT("<null/>");
			}
			else {
				for (Var::Object::iterator it = prop_obj->begin(); 
					prop_obj->end() != it; ++it)
				{
					_api_xml += TEXT("<property id=\"");
					_api_xml += it->first;
					_api_xml += TEXT("\">");
					CImpl::GeneratePropertyXml(_api_xml, it->second);
					_api_xml += TEXT("</property>");
				}
			}
		}
		_api_xml += TEXT("</object>");
		break;
	default:
		ASSERT(false);
		return false;
		break;
	}
	return true;
}


CExtApiXml::CExtApiXml(LPCTSTR _xml_str)
	: m_impl_ptr(new CImpl())
{
	IF_NULL_ASSERT_RETURN(_xml_str);
	
	m_impl_ptr->m_xml = _xml_str;
	m_impl_ptr->ParseXml(_xml_str);
}


CExtApiXml::CExtApiXml(LPCTSTR _function_name, const Var& _args)
	: m_impl_ptr(new CImpl())
{
	IF_NULL_ASSERT_RETURN(_function_name);

	CImpl::GenerateApiXml(m_impl_ptr->m_xml, _function_name, _args);
}


CExtApiXml::~CExtApiXml(void)
{
	SAFE_DELETE(const_cast<CImpl*>(m_impl_ptr));
}


bool CExtApiXml::IsValid(void) const
{
	return m_impl_ptr->m_is_valid;
}


LPCTSTR CExtApiXml::GetFunctionName(void) const
{
	return m_impl_ptr->m_fn_name.c_str();
}


size_t CExtApiXml::GetArgumentsSize(void) const
{
	return m_impl_ptr->m_args_size;
}


bool CExtApiXml::SetFunctionName(LPCTSTR _name)
{
	IF_NULL_ASSERT_RETURN_VALUE(_name, false);
	m_impl_ptr->m_fn_name = _name;
	return true;
}


bool CExtApiXml::GetArguments(OUT w2x::Var& _args) const
{
	if (0 == m_impl_ptr->m_args_size) {
		_args = Var();
		return true;
	}
	if (NULL == m_impl_ptr->m_args_ptr) {
		_args = Var();
		return false;
	}
	_args = *(m_impl_ptr->m_args_ptr);
	return true;
}


bool CExtApiXml::SetArguments(const w2x::Var& _args)
{
	SAFE_DELETE(m_impl_ptr->m_args_ptr);
	m_impl_ptr->m_args_ptr = new Var(_args);
	return true;
}


LPCTSTR CExtApiXml::GetXmlString(void) const
{
	return m_impl_ptr->GenerateXml();
}


W2X_DEFINE_NAME_SPACE_END(ext)
W2X_NAME_SPACE_END

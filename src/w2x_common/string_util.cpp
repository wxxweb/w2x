/******************************************************************************
文件:	string_util.cpp
描述:	参见 string_util.h
作者:	wu.xiongxing
邮箱:	wu.xiongxing@utooo.com
创建:	2014-12-05
修改:
*******************************************************************************/

#include "stdafx.h"
#include "string_util.h"


CStringUtil::CStringUtil(void)
	: m_is_mbs(false)
	, m_str(nullptr)
	, m_data(nullptr)
	, m_data_count(0)
{}


CStringUtil::~CStringUtil(void)
{
	m_data_count = 0;
	if (nullptr != m_data)
	{
		delete [] m_data;
		m_data = nullptr;
	}
}


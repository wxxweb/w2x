/******************************************************************************
�ļ�:	string_util.cpp
����:	�μ� string_util.h
����:	wu.xiongxing
����:	wu.xiongxing@utooo.com
����:	2014-12-05
�޸�:
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


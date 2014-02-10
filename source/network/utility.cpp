/*******************************************************************************
 * �ļ���	utility.cpp
 * ������	�μ� utility.h
 * ���䣺	wxxweb@gmail.com
 * ���ߣ�	wu.xiongxing
 * ʱ�䣺	2014-04-05
 ******************************************************************************/

#include "stdafx.h"
#include "utility.h"
#include "..\common\common.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)


W2X_NETWORK_API bool IsValidIpAddress(LPCTSTR _ip_addr_str)
{
	std::string ascii_ip_addr;
	if (0 == w2x::encode::Unicode2Ascii(ascii_ip_addr, _ip_addr_str))
	{
		return false;
	}

	addrinfo addr_info = {0};
	memset(&addr_info, 0, sizeof(addr_info));
	addr_info.ai_family = PF_UNSPEC;
	addr_info.ai_flags = AI_NUMERICHOST;

	addrinfo* addr_info_ptr = NULL;
	if (0 != getaddrinfo(ascii_ip_addr.c_str(), NULL, &addr_info, &addr_info_ptr)) 
	{
		return false;
	}

	freeaddrinfo(addr_info_ptr);
	return true;
}


W2X_NETWORK_API bool GetBroadcastIpAddress(OUT DWORD& _ip_address)
{
	bool is_alloc_enough = false;
	PMIB_IPADDRTABLE _ip_addr_table_ptr = NULL;
	DWORD _ip_addr_table_size = sizeof(MIB_IPADDRTABLE);
	DWORD	dwIPAddr;				// IP��ַ
	DWORD	dwMask;					// ��������

	// ������γ��Ի�ȡIP��ַ���������������Ҫ���·����ڴ棩
	for (size_t i = 0; i < 2 && false == is_alloc_enough; ++i)
	{
		// ΪIP��ַ������ڴ�
		_ip_addr_table_ptr = reinterpret_cast<MIB_IPADDRTABLE*>(
			::GlobalAlloc(GMEM_FIXED, _ip_addr_table_size));

		IF_NULL_ASSERT (_ip_addr_table_ptr)
		{	
			// IP��ַ��ṹ�ڴ����ʧ��
			break;
		}

		const DWORD get_ret_val = ::GetIpAddrTable(
			_ip_addr_table_ptr, &_ip_addr_table_size, 0);

		if (ERROR_INSUFFICIENT_BUFFER == get_ret_val)
		{
			// ����� IP ��ַ��ṹ�Ļ��治�㣬���·����ڴ�
			is_alloc_enough = false;
			::GlobalFree(_ip_addr_table_ptr);
			_ip_addr_table_ptr = NULL;
			continue;
		}

		IF_FALSE_ASSERT (NO_ERROR == get_ret_val)
		{
			// ��ȡ IP ��ַ��ṹ�ڴ�ʧ��
			::GlobalFree(_ip_addr_table_ptr);
			_ip_addr_table_ptr = NULL;
		}
		break;
	}

	IF_NULL_ASSERT (_ip_addr_table_ptr)
	{
		const DWORD error_code = ::GetLastError();
		TCHAR error_msg[MAX_PATH] = TEXT("");
		w2x::log::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("Get IP address table falid. %d - %s."),
			error_code, error_msg);

		return false;
	}

	// IP��ַ���ȡ�ɹ�

	dwIPAddr = _ip_addr_table_ptr->table[1].dwAddr;		// ��ȡIP��ַ
	dwMask = _ip_addr_table_ptr->table[1].dwMask;		// �����������
	_ip_address = (dwIPAddr & dwMask) | ~dwMask;		// ����㲥��ַ

	::GlobalFree(_ip_addr_table_ptr);
	_ip_addr_table_ptr = NULL;

	return true;
}


W2X_NETWORK_API int ParseIpAddressString(
	OUT	LPTSTR _str_buffer,
	const DWORD _size_in_words,
	const DWORD _ip_address
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_str_buffer, -1);
	IF_NULL_ASSERT_RETURN_VALUE(MAX_IP_ADDR_STR <= _size_in_words, -1);

	return _stprintf_s(_str_buffer, _size_in_words, TEXT("%d.%d.%d.%d"),
		(_ip_address & 0x000000FF),
		(_ip_address & 0x0000FF00) >> 8,
		(_ip_address & 0x00FF0000) >> 16,
		(_ip_address & 0xFF000000) >> 24);
}


W2X_DEFINE_NAME_SPACE_END(network)
W2X_NAME_SPACE_END
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


W2X_NETWORK_API bool ParseHostToIpAddress(
	OUT DWORD& _ip_address,
	LPCTSTR _host
	)
{
	char ip_addr_str[MAX_IP_ADDR_STR] = "";

	if (true == internal::IsValidIpAddress(_host))
	{
		std::string ascii_ip_addr;
		if (false == w2x::encode::Unicode2Ascii(ascii_ip_addr, _host))
		{
			return false;
		}
		strcpy_s(ip_addr_str, MAX_IP_ADDR_STR, ascii_ip_addr.c_str());
	}
	else
	{
		std::string host_name;
		if (false == w2x::encode::Unicode2Ascii(host_name, _host))
		{
			return false;
		}

		HOSTENT* host_ent_ptr = gethostbyname(host_name.c_str());
		IF_FALSE_ASSERT (NULL != host_ent_ptr && NULL != host_ent_ptr->h_addr_list[0])
		{
			w2x::log::LogError(TEXT("Get IP address by name(%s) failed."), 
				_host);
			return false;
		}

		memcpy(&ip_addr_str, 
			inet_ntoa(*(in_addr*)host_ent_ptr->h_addr_list[0]), 
			MAX_IP_ADDR_STR);
	}

	_ip_address = inet_addr(ip_addr_str);

	return true;
}


W2X_NETWORK_API bool GetBroadcastIpAddress(OUT DWORD& _ip_address)
{
	bool is_alloc_enough = false;
	PMIB_IPADDRTABLE ip_addr_table_ptr = NULL;
	DWORD ip_addr_table_size = sizeof(MIB_IPADDRTABLE);

	// ������γ��Ի�ȡIP��ַ�����������������Ҫ���·����ڴ棩
	for (size_t i = 0; i < 2 && false == is_alloc_enough; ++i)
	{
		// ΪIP��ַ�������ڴ�
		ip_addr_table_ptr = reinterpret_cast<MIB_IPADDRTABLE*>(
			::GlobalAlloc(GMEM_FIXED, ip_addr_table_size));

		IF_NULL_ASSERT (ip_addr_table_ptr)
		{	
			// IP��ַ���ṹ�ڴ����ʧ��
			break;
		}

		const DWORD get_ret_val = ::GetIpAddrTable(
			ip_addr_table_ptr, &ip_addr_table_size, 0);

		if (ERROR_INSUFFICIENT_BUFFER == get_ret_val)
		{
			// ����� IP ��ַ���ṹ�Ļ��治�㣬���·����ڴ�
			is_alloc_enough = false;
			::GlobalFree(ip_addr_table_ptr);
			ip_addr_table_ptr = NULL;
			continue;
		}

		IF_FALSE_ASSERT (NO_ERROR == get_ret_val)
		{
			// ��ȡ IP ��ַ���ṹ�ڴ�ʧ��
			::GlobalFree(ip_addr_table_ptr);
			ip_addr_table_ptr = NULL;
		}
		break;
	}

	IF_NULL_ASSERT (ip_addr_table_ptr)
	{
		const DWORD error_code = ::GetLastError();
		TCHAR error_msg[MAX_PATH] = TEXT("");
		w2x::log::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("Get IP address table falid. %d - %s."),
			error_code, error_msg);

		return false;
	}

	// IP��ַ����ȡ�ɹ�
	for (DWORD i = 0; i < ip_addr_table_ptr->dwNumEntries; ++i)
	{
		DWORD local_ip_addr = ip_addr_table_ptr->table[i].dwAddr;	// ��ȡIP��ַ

		if (0x7F != ((ntohl(local_ip_addr) >> 24) & 0xFF))
		{
			DWORD net_mask = ip_addr_table_ptr->table[i].dwMask;		// �����������
			_ip_address = (local_ip_addr & net_mask) | ~net_mask;		// ����㲥��ַ
			break;
		}
	}

	::GlobalFree(ip_addr_table_ptr);
	ip_addr_table_ptr = NULL;

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
/*******************************************************************************
 * 文件：	utility.cpp
 * 描述：	参见 utility.h
 * 邮箱：	wxxweb@gmail.com
 * 作者：	wu.xiongxing
 * 时间：	2014-04-05
 ******************************************************************************/

#include "stdafx.h"
#include "utility.h"
#include "w2x_common/encode.h"
#include "w2x_common/error.h"
#include "w2x_common/log.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(net)


W2X_NETWORK_API void IpAddr2Str(
	OUT	W2X_IP_ADDR_STR& _str,
	DWORD _ip_addr
	)
{
// 	_stprintf_s(reinterpret_cast<LPTSTR>(&_str), 
// 		MAX_IP_ADDR_STR, TEXT("%d.%d.%d.%d"),
// 		(_ip_addr & 0x000000FF),
// 		(_ip_addr & 0x0000FF00) >> 8,
// 		(_ip_addr & 0x00FF0000) >> 16,
// 		(_ip_addr & 0xFF000000) >> 24);
	memset(&(_str.str), 0, sizeof(_str.str));

	int str_idx = 0;
	for (DWORD part = 0, part_mask = 0x000000FF, part_count = 0;
		 4 > part_count && MAX_IP_ADDR_STR > str_idx; ++part_count)
	{
		part = _ip_addr & part_mask;
		part_mask <<= 8;
		part >>= 8 * part_count;
		if (100 <= part) {
			_str.str[str_idx++] = static_cast<TCHAR>(TEXT('0') + (part / 100));
		}
		if (100 <= part || 10 <= (part % 100)) {
			part %= 100;
			_str.str[str_idx++] = static_cast<TCHAR>(TEXT('0') + (part / 10));
			part %= 10;
		} 
		_str.str[str_idx++] = static_cast<TCHAR>(TEXT('0') + part);
		_str.str[str_idx++] = TEXT('.');
	}
	_str.str[str_idx - 1] = TEXT('\0');
}

W2X_NETWORK_API bool Str2IpAddr(
	OUT DWORD& _ip_addr,
	IN const W2X_IP_ADDR_STR& _str
	)
{
	_ip_addr = INADDR_ANY;
	LPCTSTR str_ptr = _str.str;
	int dot_count = 0;

	DWORD len = _tcslen(str_ptr);
	IF_FALSE_ASSERT (0 < len && MAX_IP_ADDR_STR >= len) {
		return false;
	}

	int part = 0;
	for (int str_idx = len - 1, part_rd = 1; 
		0 <= str_idx && 4 > dot_count; --str_idx)
	{
		const TCHAR ch = str_ptr[str_idx];
		if (TEXT('\0') == ch) {
			continue;
		}
		if (TEXT('.') == ch)
		{
			++dot_count;
			_ip_addr <<= 8;
			_ip_addr |= part & 0x000000FF;
			part = 0;
			part_rd = 1;
			continue;
		}
		const int num = ch - TEXT('0');
		IF_FALSE_ASSERT (10 > num) {
			_ip_addr = INADDR_ANY;
			return false;
		}
		part += part_rd * num;
		part_rd *= 10;
		IF_FALSE_ASSERT (0xFF >= part) {
			_ip_addr = INADDR_ANY;
			return false;
		}
	}

	IF_FALSE_ASSERT (4 >= dot_count) 
	{
		_ip_addr = INADDR_ANY;
		return false;
	} 
	else 
	{
		_ip_addr <<= 8;
		_ip_addr |= part & 0x000000FF;
	}
	return true;
}


W2X_NETWORK_API bool Name2IpAddr(
	OUT DWORD& _ip_addr,
	LPCTSTR _name
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_name, false);

	const W2X_IP_ADDR_STR* name_may_be_ip = 
		reinterpret_cast<const W2X_IP_ADDR_STR*>(_name);

	if (true == internal::Str2IpAddr(_ip_addr, *name_may_be_ip))
	{
		return true;
	}

	_ip_addr = INADDR_ANY;
	LPCSTR ascii_name_ptr = W2X_ALLOC_STR_T2A(_name);
	IF_NULL_ASSERT (ascii_name_ptr)
	{
		return false;
	}

	addrinfo addr_info = {0};
	addr_info.ai_family = AF_INET;
	addr_info.ai_flags = AI_PASSIVE;

	addrinfo* addr_info_ptr = NULL;
	const int ret_val = getaddrinfo(
		ascii_name_ptr, NULL, &addr_info, &addr_info_ptr);
	if (0 != ret_val) 
	{
		TCHAR err_str[MAX_PATH] = TEXT("");
		debug::FormatError(err_str, MAX_PATH, ret_val);
		log::LogWarn(TEXT("[Host2IpAddr] %d - %s"), ret_val, err_str);
		W2X_FREE_STR_A(ascii_name_ptr);
		ASSERT(false);
		return false;
	}

	const SOCKADDR_IN& sai = *reinterpret_cast<const SOCKADDR_IN*>(
		addr_info_ptr->ai_addr);
	_ip_addr = sai.sin_addr.S_un.S_addr;

	freeaddrinfo(addr_info_ptr);
	W2X_FREE_STR_A(ascii_name_ptr);
	return true;
}


static bool GetWorkIpAddrRow(
	OUT MIB_IPADDRROW& _ip_addr_row
	)
{
	bool is_alloc_enough = false;
	PMIB_IPADDRTABLE ip_addr_table_ptr = NULL;
	DWORD ip_addr_table_size = sizeof(MIB_IPADDRTABLE);

	// 最多两次尝试获取IP地址表（如果多张网卡需要重新分配内存）
	for (size_t i = 0; i < 2 && false == is_alloc_enough; ++i)
	{
		// 为IP地址表分配内存
		ip_addr_table_ptr = reinterpret_cast<MIB_IPADDRTABLE*>(
			::GlobalAlloc(GMEM_FIXED, ip_addr_table_size));

		IF_NULL_ASSERT (ip_addr_table_ptr)
		{	
			// IP地址表结构内存分配失败
			break;
		}

		const DWORD get_ret_val = ::GetIpAddrTable(
			ip_addr_table_ptr, &ip_addr_table_size, 0);

		if (ERROR_INSUFFICIENT_BUFFER == get_ret_val)
		{
			// 分配给 IP 地址表结构的缓存不足，重新分配内存
			is_alloc_enough = false;
			::GlobalFree(ip_addr_table_ptr);
			ip_addr_table_ptr = NULL;
			continue;
		}

		IF_FALSE_ASSERT (NO_ERROR == get_ret_val)
		{
			// 获取 IP 地址表结构内存失败
			::GlobalFree(ip_addr_table_ptr);
			ip_addr_table_ptr = NULL;
		}
		break;
	}

	IF_NULL_ASSERT (ip_addr_table_ptr)
	{
		const DWORD error_code = ::GetLastError();
		TCHAR error_msg[MAX_PATH] = TEXT("");
		w2x::debug::FormatError(error_msg, MAX_PATH, error_code);
		w2x::log::LogError(
			TEXT("Get IP address table falid. %d - %s."),
			error_code, error_msg);

		return false;
	}

	// IP地址表获取成功
	for (DWORD i = 0; i < ip_addr_table_ptr->dwNumEntries; ++i)
	{
		DWORD local_ip_addr = ip_addr_table_ptr->table[i].dwAddr;	// 获取IP地址

		// 排除 127.0.0.1
		if (INADDR_ANY != local_ip_addr && INADDR_LOOPBACK != ntohl(local_ip_addr))
		{
			memcpy(&_ip_addr_row, 
				&(ip_addr_table_ptr->table[i]), sizeof(MIB_IPADDRROW));
			::GlobalFree(ip_addr_table_ptr);
			ip_addr_table_ptr = NULL;
			return true;
		}
	}

	::GlobalFree(ip_addr_table_ptr);
	ip_addr_table_ptr = NULL;
	return false;
}


W2X_NETWORK_API bool GetWorkIpAddr(
	OUT DWORD& _ip_address
	)
{
	MIB_IPADDRROW ip_addr_row = {0};

	IF_FALSE_ASSERT (false != GetWorkIpAddrRow(ip_addr_row))
	{
		w2x::log::LogError(TEXT("Get work ip addr row failed!"));
		return false;
	}
	_ip_address = ip_addr_row.dwAddr;
	return true;
}


W2X_NETWORK_API bool GetBroadcastIpAddr(OUT DWORD& _ip_addr)
{
	MIB_IPADDRROW ip_addr_row = {0};

	IF_FALSE_ASSERT (false != GetWorkIpAddrRow(ip_addr_row))
	{
		w2x::log::LogError(TEXT("Get broadcast ip addr failed!"));
		_ip_addr = INADDR_BROADCAST;
		return false;
	}

	DWORD net_mask = ip_addr_row.dwMask;					// 获得子网掩码
	_ip_addr = (ip_addr_row.dwAddr & net_mask) | ~net_mask;	// 计算广播地址

	return true;
}


W2X_DEFINE_NAME_SPACE_END(net)
W2X_NAME_SPACE_END
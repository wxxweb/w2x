/*******************************************************************************
 * 文件：	utility.cpp
 * 描述：	参见 utility.h
 * 邮箱：	wxxweb@gmail.com
 * 作者：	wu.xiongxing
 * 时间：	2014-04-05
 ******************************************************************************/

#include "stdafx.h"
#include "utility.h"
#include "..\common\common.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(network)


W2X_NETWORK_API bool GetBroadcastIpAddress(OUT DWORD& _ip_address)
{
	bool is_alloc_enough = false;
	PMIB_IPADDRTABLE _ip_addr_table_ptr = NULL;
	DWORD _ip_addr_table_size = sizeof(MIB_IPADDRTABLE);
	DWORD	dwIPAddr;				// IP地址
	DWORD	dwMask;					// 子网掩码

	// 最多两次尝试获取IP地址表（如果多张网卡需要重新分配内存）
	for (size_t i = 0; i < 2 && false == is_alloc_enough; ++i)
	{
		// 为IP地址表分配内存
		_ip_addr_table_ptr = reinterpret_cast<MIB_IPADDRTABLE*>(
			::GlobalAlloc(GMEM_FIXED, _ip_addr_table_size));

		IF_NULL_ASSERT (_ip_addr_table_ptr)
		{	
			// IP地址表结构内存分配失败
			break;
		}

		const DWORD get_ret_val = ::GetIpAddrTable(
			_ip_addr_table_ptr, &_ip_addr_table_size, 0);

		if (ERROR_INSUFFICIENT_BUFFER == get_ret_val)
		{
			// 分配给 IP 地址表结构的缓存不足，重新分配内存
			is_alloc_enough = false;
			::GlobalFree(_ip_addr_table_ptr);
			_ip_addr_table_ptr = NULL;
			continue;
		}

		IF_FALSE_ASSERT (NO_ERROR == get_ret_val)
		{
			// 获取 IP 地址表结构内存失败
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

	// IP地址表获取成功

	dwIPAddr = _ip_addr_table_ptr->table[1].dwAddr;		// 获取IP地址
	dwMask = _ip_addr_table_ptr->table[1].dwMask;		// 获得子网掩码
	_ip_address = (dwIPAddr & dwMask) | ~dwMask;		// 计算广播地址

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
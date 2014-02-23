/*****************************************************************************
文件名：		wnd_flash.cpp
描述：		参见 wnd_flash.h
作者：		wu.xiongxing					
邮箱：		wxxweb@gmail.com
日期：		2014.02.17
******************************************************************************/

#include "stdafx.h"
#include "wnd_flash.h"
#include "wnd_flash_impl.h"
#include "..\common\macros.h"
#include "..\common\thread.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(ui)


class CWndFlash::CImpl
{
	typedef int (WINAPI *FGetDeviceCaps)(HDC hdc, int nIndex);

public:
	CImpl(void);
	~CImpl(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CImpl)
W2X_IMPLEMENT_LOCKING(CImpl, CAutoLock)

public:
	inline bool Create(
		LPCTSTR _swf_file_path, 
		HWND _parent_wnd_handle, 
		HINSTANCE _instance_handle
	);

	inline void Destroy(void);

	inline HWND GetHwnd(void) const;

	inline LPCTSTR CallFunction(LPCTSTR _funcion, LPCTSTR _arg);

private:
	static bool Hook(void);

	static void Unhook(void);

	static int WINAPI GetDeviceCapsAlter(HDC hdc, int nIndex);

private:
	CWndFlashImpl* m_wnd_ptr;
	TSTDSTR m_call_function_xml;

	static bool sm_has_been_hooked;			// 是否已经做了 Hook 操作
	static BYTE sm_orgn_gds_fun_enter[5];	// 保存原 GetDeviceCaps() 的函数入口
	static BYTE sm_asm_jmp_cmd[5];			// 保存汇编长跳指令
	static FGetDeviceCaps sm_gds_fun_ptr;	// 保存被篡改的 GetDeviceCaps() 函数指针
};


W2X_IMPLEMENT_LOCKING_CLASS(CWndFlash::CImpl, CAutoLock)


bool CWndFlash::CImpl::sm_has_been_hooked = false;
BYTE CWndFlash::CImpl::sm_orgn_gds_fun_enter[5] = {0};
BYTE CWndFlash::CImpl::sm_asm_jmp_cmd[5] = {0};
CWndFlash::CImpl::FGetDeviceCaps CWndFlash::CImpl::sm_gds_fun_ptr = NULL;


CWndFlash::CImpl::CImpl(void)
	: m_wnd_ptr(NULL)
{

}


CWndFlash::CImpl::~CImpl(void)
{
	this->Destroy();
}


/*
 * 为了弥补透明背景 FLASH 窗口在 16 位和 24 位色下处理偏差的问题，通过钩子技术，用自定义的
 * 函数 GetDeviceCapsAlter() 来欺骗 FlashPlayer 目前的色位是 32 位，而不管实际的色位
 * 是多少。这样做的理由是，GetDevicesCap() 是获得色位的 API，而且发现 FlashPlayer.ocx
 * 的导入表中确实对该函数进行了调用，所以 Hook 它。
 */
int WINAPI CWndFlash::CImpl::GetDeviceCapsAlter(HDC hdc, int nIndex)
{
	if (BITSPIXEL == nIndex)
	{
		return 32;
	}

	HANDLE current_process_handle = ::GetCurrentProcess();

	//写回 GetDeviceCaps() 的前5个字节的指令
	memcpy(reinterpret_cast<LPVOID>(sm_gds_fun_ptr), sm_orgn_gds_fun_enter, 5);

	// 刷新指令 cache 中相关地址指令的内容
	::FlushInstructionCache(current_process_handle, 
		reinterpret_cast<LPVOID>(sm_gds_fun_ptr), 5);

	int ret_val = ::GetDeviceCaps(hdc, nIndex);

	// 写回长跳转指令到 GetDeviceCaps() 的前5个字节
	memcpy(reinterpret_cast<LPVOID>(sm_gds_fun_ptr), sm_asm_jmp_cmd, 5);

	// 刷新指令 cache 中相关地址指令的内容
	::FlushInstructionCache(current_process_handle, 
		reinterpret_cast<LPVOID>(sm_gds_fun_ptr), 5);

	return ret_val;
}


void CWndFlash::CImpl::Unhook(void)
{
	if (false == sm_has_been_hooked)
	{
		return;
	}

	// 还原GetDeviceCaps()，写回 GetDeviceCaps() 的前5个字节的指令
	memcpy(reinterpret_cast<LPVOID>(sm_gds_fun_ptr), sm_orgn_gds_fun_enter, 5);

	// 刷新指令 cache 中相关地址指令的内容
	::FlushInstructionCache(::GetCurrentProcess(), 
		reinterpret_cast<LPVOID>(sm_gds_fun_ptr), 5);
}


bool CWndFlash::CImpl::Hook(void)
{
	// 如果色位不是 32 位则需要 Hook GetDeviceCaps()

	if (true == sm_has_been_hooked || 32 == ::GetDeviceCaps(0, BITSPIXEL))
	{
		return true;
	}

	HMODULE gdi32_dll_handle = ::GetModuleHandle(TEXT("gdi32.dll"));

	IF_NULL_ASSERT (gdi32_dll_handle)
	{
		gdi32_dll_handle = ::LoadLibrary(TEXT("gdi32.dll"));
		IF_NULL_ASSERT_RETURN_VALUE(gdi32_dll_handle, false);
	}

	sm_gds_fun_ptr = reinterpret_cast<FGetDeviceCaps>(
		::GetProcAddress(gdi32_dll_handle, "GetDeviceCaps"));
	IF_NULL_ASSERT_RETURN_VALUE(sm_gds_fun_ptr, false);

	// 保存原 GetDeviceCaps() 函数入口
	memcpy(sm_orgn_gds_fun_enter, reinterpret_cast<LPVOID>(sm_gds_fun_ptr), 5);

	// 构造长跳转指令
	sm_asm_jmp_cmd[0] = 0xE9;
	*reinterpret_cast<int*>(&sm_asm_jmp_cmd[1]) 
		= reinterpret_cast<int>(&GetDeviceCapsAlter) 
		- (reinterpret_cast<int>(sm_gds_fun_ptr) + 5);

	// 将长跳转指令写入 GetDeviceCaps() 前 5 个字节，
	// Word 调用 GetDeviceCaps() 时会跳到我们的函数中
	DWORD old_protect = 0;
	IF_FALSE_ASSERT (FALSE != ::VirtualProtect(
		sm_gds_fun_ptr, 5, PAGE_EXECUTE_READWRITE, &old_protect))
	{
		return false;
	}

	memcpy(sm_gds_fun_ptr, sm_asm_jmp_cmd, 5);

	// 刷新指令 cache 中相关地址指令的内容
	::FlushInstructionCache(::GetCurrentProcess(), 
		reinterpret_cast<LPVOID>(sm_gds_fun_ptr), 5);

	sm_has_been_hooked = true;

	// 在退出程序时自动 Unhook
	::atexit(CImpl::Unhook);

	return true;
}


inline bool CWndFlash::CImpl::Create(
	LPCTSTR _swf_file_path, 
	HWND _parent_wnd_handle, 
	HINSTANCE _instance_handle
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_swf_file_path, false);
	IF_NULL_ASSERT_RETURN_VALUE(_parent_wnd_handle, false);

	::OleInitialize(NULL);

	m_wnd_ptr = new CWndFlashImpl();
	IF_NULL_ASSERT_RETURN_VALUE(m_wnd_ptr, false);

	IF_FALSE_ASSERT (false != m_wnd_ptr->SetMovie(_swf_file_path))
	{
		return false;
	}

	CImpl::Hook();

	// create windowless control
	return FALSE != m_wnd_ptr->Create(
		ShockwaveFlashObjects::CLSID_ShockwaveFlash,
		WS_EX_LAYERED, WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS,
		_parent_wnd_handle, _instance_handle);
}


inline void CWndFlash::CImpl::Destroy(void)
{
	SAFE_DELETE(m_wnd_ptr);

	::OleUninitialize();
}


inline HWND CWndFlash::CImpl::GetHwnd(void) const
{
	IF_NULL_ASSERT_RETURN_VALUE(m_wnd_ptr, NULL);

	return m_wnd_ptr->GetHWND();
}


inline LPCTSTR CWndFlash::CImpl::CallFunction(LPCTSTR _funcion, LPCTSTR _arg)
{
	IF_NULL_ASSERT_RETURN_VALUE(_funcion, NULL);

	// TODO: 最好判断一下函数名称合法性

	CAutoLock this_lock(this);

	m_call_function_xml = TEXT("<invoke name='");
	m_call_function_xml += _funcion;
	m_call_function_xml += TEXT("'>");
	if (NULL != _arg)
	{
		m_call_function_xml += TEXT("<arguments><string>");
		m_call_function_xml += _arg;
		m_call_function_xml += TEXT("</string></arguments>");
	}
	m_call_function_xml += TEXT("</invoke>");

	return m_wnd_ptr->CallFunction(m_call_function_xml.c_str());
}


CWndFlash::CWndFlash(void)
	: m_impl_ptr(new CImpl())
{

}


CWndFlash::~CWndFlash(void)
{
	SAFE_DELETE(const_cast<CImpl*>(m_impl_ptr));
}


bool CWndFlash::Create(
	LPCTSTR _swf_file_path, 
	HWND _parent_wnd_handle, 
	HINSTANCE _instance_handle
	)
{
	return m_impl_ptr->Create(_swf_file_path, _parent_wnd_handle, _instance_handle);
}

void CWndFlash::Destroy(void)
{
	m_impl_ptr->Destroy();
}

HWND CWndFlash::GetHwnd(void) const
{
	return m_impl_ptr->GetHwnd();
}


LPCTSTR CWndFlash::CallFunction(LPCTSTR _funcion, LPCTSTR _arg)
{
	return m_impl_ptr->CallFunction(_funcion, _arg);
}


W2X_DEFINE_NAME_SPACE_END(ui)
W2X_NAME_SPACE_END
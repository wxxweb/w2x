/*****************************************************************************
�ļ�����		wnd_flash.cpp
������		�μ� wnd_flash.h
���ߣ�		wu.xiongxing					
���䣺		wxxweb@gmail.com
���ڣ�		2014.02.17
******************************************************************************/

#include "stdafx.h"
#include "wnd_flash.h"
#include "wnd_flash_impl.h"
#include "..\common\macros.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(ui)


typedef int (WINAPI *FGetDeviceCaps)(HDC hdc, int nIndex);

static bool gs_has_been_hooked = false;	// �Ƿ��Ѿ����� Hook ����
static BYTE gs_orgn_gds_fun_enter[5];	// ����ԭ GetDeviceCaps() �ĺ������
static BYTE gs_asm_jmp_cmd[5];			// �����೤��ָ��
static FGetDeviceCaps gs_gds_fun_ptr;	// ���汻�۸ĵ� GetDeviceCaps() ����ָ��

/*
 * Ϊ���ֲ�͸������ FLASH ������ 16 λ�� 24 λɫ�´���ƫ������⣬ͨ�����Ӽ��������Զ����
 * ���� GetDeviceCapsAlter() ����ƭ FlashPlayer Ŀǰ��ɫλ�� 32 λ��������ʵ�ʵ�ɫλ
 * �Ƕ��١��������������ǣ�GetDevicesCap() �ǻ��ɫλ�� API�����ҷ��� FlashPlayer.ocx
 * �ĵ������ȷʵ�Ըú��������˵��ã����� Hook ����
 */
int WINAPI GetDeviceCapsAlter(HDC hdc, int nIndex)
{
	if (BITSPIXEL == nIndex)
	{
		return 32;
	}

	HANDLE current_process_handle = ::GetCurrentProcess();

	//д�� GetDeviceCaps() ��ǰ5���ֽڵ�ָ��
	memcpy(reinterpret_cast<LPVOID>(gs_gds_fun_ptr), gs_orgn_gds_fun_enter, 5);

	// ˢ��ָ�� cache ����ص�ַָ�������
	::FlushInstructionCache(current_process_handle, 
		reinterpret_cast<LPVOID>(gs_gds_fun_ptr), 5);

	int ret_val = ::GetDeviceCaps(hdc, nIndex);

	// д�س���תָ� GetDeviceCaps() ��ǰ5���ֽ�
	memcpy(reinterpret_cast<LPVOID>(gs_gds_fun_ptr), gs_asm_jmp_cmd, 5);

	// ˢ��ָ�� cache ����ص�ַָ�������
	::FlushInstructionCache(current_process_handle, 
		reinterpret_cast<LPVOID>(gs_gds_fun_ptr), 5);

	return ret_val;
}



static void Unhook(void)
{
	if (false == gs_has_been_hooked)
	{
		return;
	}

	// ��ԭGetDeviceCaps()��д�� GetDeviceCaps() ��ǰ5���ֽڵ�ָ��
	memcpy(reinterpret_cast<LPVOID>(gs_gds_fun_ptr), gs_orgn_gds_fun_enter, 5);

	// ˢ��ָ�� cache ����ص�ַָ�������
	::FlushInstructionCache(::GetCurrentProcess(), 
		reinterpret_cast<LPVOID>(gs_gds_fun_ptr), 5);
}


static bool Hook(void)
{
	// ���ɫλ���� 32 λ����Ҫ Hook GetDeviceCaps()

	if (true == gs_has_been_hooked || 32 == ::GetDeviceCaps(0, BITSPIXEL))
	{
		return true;
	}

	HMODULE gdi32_dll_handle = ::GetModuleHandle(TEXT("gdi32.dll"));

	IF_NULL_ASSERT (gdi32_dll_handle)
	{
		gdi32_dll_handle = ::LoadLibrary(TEXT("gdi32.dll"));
		IF_NULL_ASSERT_RETURN_VALUE(gdi32_dll_handle, false);
	}

	gs_gds_fun_ptr = reinterpret_cast<FGetDeviceCaps>(
		::GetProcAddress(gdi32_dll_handle, "GetDeviceCaps"));
	IF_NULL_ASSERT_RETURN_VALUE(gs_gds_fun_ptr, false);

	// ����ԭ GetDeviceCaps() �������
	memcpy(gs_orgn_gds_fun_enter, reinterpret_cast<LPVOID>(gs_gds_fun_ptr), 5);

	// ���쳤��תָ��
	gs_asm_jmp_cmd[0] = 0xE9;
	*reinterpret_cast<int*>(&gs_asm_jmp_cmd[1]) 
		= reinterpret_cast<int>(&GetDeviceCapsAlter) 
		- (reinterpret_cast<int>(gs_gds_fun_ptr) + 5);

	// ������תָ��д�� GetDeviceCaps() ǰ 5 ���ֽڣ�
	// Word ���� GetDeviceCaps() ʱ���������ǵĺ�����
	DWORD old_protect = 0;
	IF_FALSE_ASSERT (FALSE != ::VirtualProtect(
		gs_gds_fun_ptr, 5, PAGE_EXECUTE_READWRITE, &old_protect))
	{
		return false;
	}

	memcpy(gs_gds_fun_ptr, gs_asm_jmp_cmd, 5);

	// ˢ��ָ�� cache ����ص�ַָ�������
	::FlushInstructionCache(::GetCurrentProcess(), 
		reinterpret_cast<LPVOID>(gs_gds_fun_ptr), 5);

	gs_has_been_hooked = true;

	// ���˳�����ʱ�Զ� Unhook
	::atexit(internal::Unhook);

	return true;
}


CWndFlash::CWndFlash(void)
	: m_wnd_ptr(NULL)
{
	
}


CWndFlash::~CWndFlash(void)
{
	this->Destroy();
}


bool CWndFlash::Create(
	LPCTSTR _swf_file_path, 
	HWND _parent_wnd_handle, 
	HINSTANCE _instance_handle)
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

	internal::Hook();

	// create windowless control
	return FALSE != m_wnd_ptr->Create(
		ShockwaveFlashObjects::CLSID_ShockwaveFlash,
		WS_EX_LAYERED, WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS,
		_parent_wnd_handle, _instance_handle);
}


void CWndFlash::Destroy(void)
{
	SAFE_DELETE(m_wnd_ptr);

	::OleUninitialize();
}


HWND CWndFlash::GetHwnd(void) const
{
	IF_NULL_ASSERT_RETURN_VALUE(m_wnd_ptr, NULL);

	return m_wnd_ptr->GetHWND();
}


W2X_DEFINE_NAME_SPACE_END(ui)
W2X_NAME_SPACE_END
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

	inline void SetCaptionArea(WORD _x, WORD _y, WORD _width, WORD _height);

	inline LPCTSTR CallFunction(LPCTSTR _funcion, LPCTSTR _arg);

	inline bool AddEventListener(LPCTSTR _type_name, 
		w2x::events::FEventListener _listener
	);

	inline bool RemoveEventListener(
		LPCTSTR _type_name, 
		w2x::events::FEventListener _listener
	);

private:
	static bool Hook(void);

	static void Unhook(void);

	static int WINAPI GetDeviceCapsAlter(HDC hdc, int nIndex);

private:
	CWndFlashImpl* m_wnd_ptr;
	TSTDSTR m_call_function_xml;

	static bool sm_has_been_hooked;			// �Ƿ��Ѿ����� Hook ����
	static BYTE sm_orgn_gds_fun_enter[5];	// ����ԭ GetDeviceCaps() �ĺ������
	static BYTE sm_asm_jmp_cmd[5];			// �����೤��ָ��
	static FGetDeviceCaps sm_gds_fun_ptr;	// ���汻�۸ĵ� GetDeviceCaps() ����ָ��
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
 * Ϊ���ֲ�͸������ FLASH ������ 16 λ�� 24 λɫ�´���ƫ������⣬ͨ�����Ӽ��������Զ����
 * ���� GetDeviceCapsAlter() ����ƭ FlashPlayer Ŀǰ��ɫλ�� 32 λ��������ʵ�ʵ�ɫλ
 * �Ƕ��١��������������ǣ�GetDevicesCap() �ǻ��ɫλ�� API�����ҷ��� FlashPlayer.ocx
 * �ĵ������ȷʵ�Ըú��������˵��ã����� Hook ����
 */
int WINAPI CWndFlash::CImpl::GetDeviceCapsAlter(HDC hdc, int nIndex)
{
	if (BITSPIXEL == nIndex)
	{
		return 32;
	}

	HANDLE current_process_handle = ::GetCurrentProcess();

	//д�� GetDeviceCaps() ��ǰ5���ֽڵ�ָ��
	memcpy(reinterpret_cast<LPVOID>(sm_gds_fun_ptr), sm_orgn_gds_fun_enter, 5);

	// ˢ��ָ�� cache ����ص�ַָ�������
	::FlushInstructionCache(current_process_handle, 
		reinterpret_cast<LPVOID>(sm_gds_fun_ptr), 5);

	int ret_val = ::GetDeviceCaps(hdc, nIndex);

	// д�س���תָ� GetDeviceCaps() ��ǰ5���ֽ�
	memcpy(reinterpret_cast<LPVOID>(sm_gds_fun_ptr), sm_asm_jmp_cmd, 5);

	// ˢ��ָ�� cache ����ص�ַָ�������
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

	// ��ԭGetDeviceCaps()��д�� GetDeviceCaps() ��ǰ5���ֽڵ�ָ��
	memcpy(reinterpret_cast<LPVOID>(sm_gds_fun_ptr), sm_orgn_gds_fun_enter, 5);

	// ˢ��ָ�� cache ����ص�ַָ�������
	::FlushInstructionCache(::GetCurrentProcess(), 
		reinterpret_cast<LPVOID>(sm_gds_fun_ptr), 5);
}


bool CWndFlash::CImpl::Hook(void)
{
	// ���ɫλ���� 32 λ����Ҫ Hook GetDeviceCaps()

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

	// ����ԭ GetDeviceCaps() �������
	memcpy(sm_orgn_gds_fun_enter, reinterpret_cast<LPVOID>(sm_gds_fun_ptr), 5);

	// ���쳤��תָ��
	sm_asm_jmp_cmd[0] = 0xE9;
	*reinterpret_cast<int*>(&sm_asm_jmp_cmd[1]) 
		= reinterpret_cast<int>(&GetDeviceCapsAlter) 
		- (reinterpret_cast<int>(sm_gds_fun_ptr) + 5);

	// ������תָ��д�� GetDeviceCaps() ǰ 5 ���ֽڣ�
	// Word ���� GetDeviceCaps() ʱ���������ǵĺ�����
	DWORD old_protect = 0;
	IF_FALSE_ASSERT (FALSE != ::VirtualProtect(
		sm_gds_fun_ptr, 5, PAGE_EXECUTE_READWRITE, &old_protect))
	{
		return false;
	}

	memcpy(sm_gds_fun_ptr, sm_asm_jmp_cmd, 5);

	// ˢ��ָ�� cache ����ص�ַָ�������
	::FlushInstructionCache(::GetCurrentProcess(), 
		reinterpret_cast<LPVOID>(sm_gds_fun_ptr), 5);

	sm_has_been_hooked = true;

	// ���˳�����ʱ�Զ� Unhook
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


inline void CWndFlash::CImpl::SetCaptionArea(
	WORD _x, WORD _y, WORD _width, WORD _height
	)
{
	m_wnd_ptr->SetCaptionArea(_x, _y, _width, _height);
}

inline LPCTSTR CWndFlash::CImpl::CallFunction(LPCTSTR _funcion, LPCTSTR _arg)
{
	IF_NULL_ASSERT_RETURN_VALUE(_funcion, NULL);

	// TODO: ����ж�һ�º������ƺϷ���

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


bool CWndFlash::CImpl::AddEventListener(LPCTSTR _type_name, 
	w2x::events::FEventListener _listener
	)
{
	return m_wnd_ptr->m_event_disp.AddEventListener(_type_name, _listener);
}


bool CWndFlash::CImpl::RemoveEventListener(
	LPCTSTR _type_name, 
	w2x::events::FEventListener _listener
	)
{
	return m_wnd_ptr->m_event_disp.RemoveEventListener(_type_name, _listener);
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


void CWndFlash::SetCaptionArea(WORD _x, WORD _y, WORD _width, WORD _height)
{
	m_impl_ptr->SetCaptionArea(_x, _y, _width, _height);
}


LPCTSTR CWndFlash::CallFunction(LPCTSTR _funcion, LPCTSTR _arg)
{
	return m_impl_ptr->CallFunction(_funcion, _arg);
}


bool CWndFlash::AddEventListener(LPCTSTR _type_name, 
	w2x::events::FEventListener _listener
	)
{
	return m_impl_ptr->AddEventListener(_type_name, _listener);
}


bool CWndFlash::RemoveEventListener(
	LPCTSTR _type_name, 
	w2x::events::FEventListener _listener
	)
{
	return m_impl_ptr->RemoveEventListener(_type_name, _listener);
}


W2X_DEFINE_NAME_SPACE_END(ui)
W2X_NAME_SPACE_END
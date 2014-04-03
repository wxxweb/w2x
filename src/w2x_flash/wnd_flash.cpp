/*****************************************************************************
 * �ļ�:		wnd_flash.cpp
 * ����:		�μ� wnd_flash.h
 * ����:		wu.xiongxing
 * ����:		wxxweb@gmail.com
 * ����:		2014-02-17
 *****************************************************************************/

#include "stdafx.h"
#include "wnd_flash.h"
#include "wnd_flash_impl.h"
#include "flash_event.h"
#include "ext_api_xml.h"
#include "w2x_common/macros.h"
#include "w2x_common/mutex.h"
#include "w2x_common/msg_loop.h"
#include "w2x_common/log.h"
#include "w2x_common/ref_ptr.h"
#include "w2x_common/system.h"
#include "third-party/include/rapidxml/rapidxml.hpp"

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(ui)


class CWndFlash::CImpl: public IExtApiXmlParser
{
	//static TCHAR UI_CMD_SHOW_WND[] = TEXT("ShowWnd");

	typedef int (WINAPI *FGetDeviceCaps)(HDC hdc, int nIndex);

public:
	CImpl(void);
	virtual ~CImpl(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CImpl)
W2X_IMPLEMENT_LOCKING(CImpl, CAutoLock)
W2X_IMPLEMENT_REFCOUNTING(CImpl)

public:
	inline bool Create(
		LPCTSTR _wnd_name,
		LPCTSTR _swf_file_path, 
		HWND _parent_wnd_handle, 
		HINSTANCE _instance_handle
	);

	inline HWND GetHwnd(void) const;

	inline LPCTSTR GetName(void) const;

	inline void SetCaptionArea(WORD _x, WORD _y, WORD _width, WORD _height);

	inline bool CallFunction(LPCTSTR _funcion, const Var& _args);

public:
	bool ParseExtApiXml(LPCTSTR _ext_api_xml);

private:
	static bool Hook(void);

	static void Unhook(void);

	static int WINAPI GetDeviceCapsAlter(HDC hdc, int nIndex);

	static bool CALLBACK HandleFunctionCall(
		PVOID _this_ptr,
		LPTSTR _ext_api_xml,
		size_t _bytes = 0,
		PVOID _msg_param = NULL
	);
	
public:
	w2x::CMsgLoop			m_call_loop;
	CWndFlashImpl* const	m_wnd_ptr;
	CWndFlash*				m_interface_ptr;
	w2x::CRefPtr<events::IEventDispatcher> m_event_disp;

private:
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
	: m_interface_ptr(NULL)
	, m_wnd_ptr(new CWndFlashImpl())
{
	ASSERT(NULL != m_wnd_ptr);
	if (NULL == m_event_disp)
	{
		m_event_disp = new events::CEventDispatcher(NULL);
	}
	m_call_loop.StartLoopThread(
		CImpl::HandleFunctionCall, reinterpret_cast<PVOID>(this));
}


CWndFlash::CImpl::~CImpl(void)
{
	m_call_loop.StopLoopThread();
	SAFE_DELETE(const_cast<CWndFlashImpl*>(m_wnd_ptr));
	::OleUninitialize();
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
	LPCTSTR _wnd_name,
	LPCTSTR _swf_file_path,
	HWND _parent_wnd_handle,
	HINSTANCE _instance_handle
	)
{
	ASSERT(NULL != _wnd_name);
	IF_NULL_ASSERT_RETURN_VALUE(_swf_file_path, false);

	m_wnd_ptr->SetWndName(_wnd_name);

	::OleInitialize(NULL);

	IF_FALSE_ASSERT (false != m_wnd_ptr->SetMovie(_swf_file_path))
	{
		return false;
	}

	CImpl::Hook();

	m_wnd_ptr->SetExtApiXmlParser(*this);

	const BOOL is_successed = m_wnd_ptr->Create(
		ShockwaveFlashObjects::CLSID_ShockwaveFlash,
		WS_EX_LAYERED | WS_EX_TOOLWINDOW, 
		WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS,
		_parent_wnd_handle, _instance_handle);

	if (FALSE == is_successed)
	{
		m_call_loop.StopLoopThread();
	}

	return FALSE != is_successed;
}


inline HWND CWndFlash::CImpl::GetHwnd(void) const
{
	IF_NULL_ASSERT_RETURN_VALUE(m_wnd_ptr, NULL);

	return m_wnd_ptr->GetHWND();
}


inline LPCTSTR CWndFlash::CImpl::GetName(void) const
{
	return m_wnd_ptr->GetWndName();
}


inline void CWndFlash::CImpl::SetCaptionArea(
	WORD _x, WORD _y, WORD _width, WORD _height
	)
{
	m_wnd_ptr->SetCaptionArea(_x, _y, _width, _height);
}


inline bool CWndFlash::CImpl::CallFunction(LPCTSTR _function, const Var& _args)
{
	IF_NULL_ASSERT_RETURN_VALUE(_function, NULL);

	// TODO: ����ж�һ�º������ƺϷ���

	CAutoLock this_lock(this);

	ext::CExtApiXml api_xml(_function, _args);
	LPCTSTR xml_str = api_xml.GetXmlString();

	//m_wnd_ptr->CallFunction(xml_str);

	//return true;
	return m_call_loop.AddMsg(xml_str);
}


bool CWndFlash::CImpl::ParseExtApiXml(LPCTSTR _ext_api_xml)
{
	IF_NULL_ASSERT_RETURN_VALUE(_ext_api_xml, false);
	IF_NULL_ASSERT_RETURN_VALUE(m_event_disp, false);

	w2x::ext::CExtApiXml api_xml(_ext_api_xml);
	IF_NULL_ASSERT_RETURN_VALUE(api_xml.IsValid(), false);

	LPCTSTR fn_name = api_xml.GetFunctionName();
	if (0 == _tcsicmp(fn_name, TEXT("SwfEvent")))
	{
		Var fn_args;
		api_xml.GetArguments(fn_args);
		
		// ��ȡ�¼����ͼ��¼�����
		Var evt_type = events::CEvent::EVENT_UNKNOW;
		Var evt_args;
		if (0 < fn_args.GetLength()) 
		{
			//Var::Array ary = fn_args;
			evt_type = fn_args[0];
			if (1 < fn_args.GetLength()) {
				evt_args = fn_args[1];
			}
		}

		events::CFlashEvent evt(evt_type, NULL, NULL);
		evt.SetTargetName(this->GetName());

		if (true == evt.IsTypeOf(events::CFlashEvent::EVENT_LOAD_PROGRESS))
		{
			const int progress = evt_args[TEXT("progress")];
		}
		else if (true == evt.IsTypeOf(events::CFlashEvent::EVENT_STAGE_SIZE))
		{
			const int width = evt_args[TEXT("width")];
			const int height = evt_args[TEXT("height")];
			evt.SetStageSize(width, height);
		}
		else if (true == evt.IsTypeOf(events::CFlashEvent::EVENT_MOUSE_CLICK))
		{
			evt.SetTypeName(events::CFlashEvent::EVENT_MOUSE_CLICK);

			// ��ȡ����� UI ����Դ������
			Var target_id = evt_args[TEXT("id")];
			evt.SetUiTraget(target_id);
		}
		
		m_event_disp->DispatchEvent(evt);
	}

	return true;
}


bool CALLBACK CWndFlash::CImpl::HandleFunctionCall(
	PVOID _this_ptr,
	LPTSTR _ext_api_xml,
	size_t _bytes,
	PVOID _msg_param
	)
{
	IF_NULL_ASSERT_RETURN_VALUE(_this_ptr, true);

	CImpl* const this_ptr = reinterpret_cast<CImpl*>(_this_ptr);

	this_ptr->m_wnd_ptr->CallFunction(_ext_api_xml);

	return true;
}


CWndFlash::CWndFlash(void)
	: m_impl_ptr(new CImpl())
{
	ASSERT(NULL != m_impl_ptr);
	m_impl_ptr->m_interface_ptr = this;
}


CWndFlash::~CWndFlash(void)
{
	SAFE_DELETE(const_cast<CImpl*>(m_impl_ptr));
}


bool CWndFlash::Create(
	LPCTSTR _wnd_name,
	LPCTSTR _swf_file_path,
	HWND _parent_wnd_handle,
	HINSTANCE _instance_handle
	)
{
	return m_impl_ptr->Create(
		_wnd_name, _swf_file_path, _parent_wnd_handle, _instance_handle);
}


HWND CWndFlash::GetHwnd(void) const
{
	return m_impl_ptr->GetHwnd();
}

LPCTSTR CWndFlash::GetName(void) const
{
	return m_impl_ptr->GetName();
}

void CWndFlash::SetCaptionArea(WORD _x, WORD _y, WORD _width, WORD _height)
{
	m_impl_ptr->SetCaptionArea(_x, _y, _width, _height);
}


bool CWndFlash::CallFunction(LPCTSTR _function, const Var& _args)
{
	return m_impl_ptr->CallFunction(_function, _args);
}


bool CWndFlash::AddEventListener(LPCTSTR _type_name, 
	w2x::events::FEventListener _listener
	)
{
	return m_impl_ptr->m_event_disp->AddEventListener(_type_name, _listener);
}


bool CWndFlash::HasEventListener(LPCTSTR _type_name) const
{
	return m_impl_ptr->m_event_disp->HasEventListener(_type_name);
}


bool CWndFlash::RemoveEventListener(
	LPCTSTR _type_name, 
	w2x::events::FEventListener _listener
	)
{
	return m_impl_ptr->m_event_disp->RemoveEventListener(_type_name, _listener);
}


bool CWndFlash::IsFlashPlayerInstalled(void)
{
	return w2x::sys::IsOcxRegistered(ShockwaveFlashObjects::CLSID_ShockwaveFlash);
}


bool CWndFlash::EnsureFlashPlayerInstalled(void)
{
	if (true == w2x::sys::IsOcxRegistered(
		ShockwaveFlashObjects::CLSID_ShockwaveFlash))
	{
		return true;
	}

	TCHAR ocx_path[MAX_PATH] = TEXT("");
	::GetCurrentDirectory(MAX_PATH, ocx_path);
	_tcscat_s(ocx_path, TEXT("\\Flash.ocx"));

	IF_FALSE_ASSERT (FALSE != ::PathFileExists(ocx_path))
	{
		w2x::log::LogWarn(TEXT("[WndFlash] Not found file: %s"), ocx_path);
		return false;
	}

	TCHAR sys_ocx_path[MAX_PATH] = 
		TEXT("C:\\Windows\\System32\\Macromed\\Flash\\Flash.ocx");
	if (FALSE == ::PathFileExists(sys_ocx_path))
	{
		::CopyFile(ocx_path, sys_ocx_path, TRUE);
	}

	return w2x::sys::RegisterOcx(sys_ocx_path);
}


W2X_DEFINE_NAME_SPACE_END(ui)
W2X_NAME_SPACE_END
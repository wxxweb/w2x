/******************************************************************
***
***
***				FREE WINDOWLESS FLASH CONTROL
***
***					   by Makarov Igor
***
***		for questions and remarks mailto: mak_july@list.ru
***
***
*******************************************************************/
// flash.cpp : Defines the entry point for the application.
//

/*
 * ��ʵ�ֺ�������ָ����ʵ����QQ��������Ч���ı��ʷ�������Ψһ����
 * ������16λ24λɫ�´���Ĳ��á������������϶�������ȥ���á�
 * �������16λ��24λɫ�Ľ��������
 * 1�������Ƚ�Flashÿһ֡��AlphaֵԤ�ȱ���������Ȼ��
 *	�ڴ˸������ؼ������ý�ȥ����Ϊ��Draw�Ĺ����У�����
 *	��32λ�豸�޹ص�λͼ��������FlashPlayer�ڸ�DC�ϻ���
 *	��Ϻ󣬿��Խ�DC�е�λͼ���ݵõ�������Alphaͨ��������д
 *	��ȥ����ϸ�۲�һ�£���������16λɫģʽ�£�FlashPlayer
 *	������һ������ֻ���������ֽڣ�����ʹ����3���ֽڣ�����
 *	��Alpha��д�����ĸ��ֽھ��ܽ�����⡣��������̫��׾�ˣ�
 *	Ԥ��Ҫ��ȡÿһ֡��Alphaֵ��������Ҫ��ÿһ���������¼��㣬
 *	Ч��̫�͡�
 *   2���ڶ��ַ���һ�����ݰ����Ǿ�����ƭFlashPlayer��ͨ��
 *	���Ӽ������Լ�����ĺ�������FlashPlayerĿǰ��ɫλ��32
 *	λ��������ʵ�ʵ�ɫλ�Ƕ��١�GetDevicesCap�ǻ��ɫλ��
 *	API�����ҷ���FlashPlayer.ocx�ĵ������ȷʵ�Ըú�������
 *	�˵��ã�����Hook����------------�½��� 2009-5-2
 */


#include "stdafx.h"
#include "resource.h"
#include "flash/wnd_flash.h"
#include "flash/flash_event.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING] = _T("test");								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING] = _T("WindowlessFlashContainerApp"); // The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void				OnFlashCommand(const w2x::events::CEvent& _event_ref);

HWND g_hWnd = NULL;
HINSTANCE g_hInst = NULL;
w2x::ui::CWndFlash *g_flashWnd = NULL;

HINSTANCE app_instance_;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	w2x::debug::EnableExcptionHandle(/*HandleExcption*/);
	//w2x::debug::SetReportMode(w2x::debug::kModeStderr);
	w2x::debug::EnableLeakCheck(true);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	app_instance_ = hInstance;

	// Initialize global strings
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	g_hInst = hInst;

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_FLASH);

	TCHAR szSwfFilePath[MAX_PATH] = TEXT("");
	::GetModuleFileName(NULL, szSwfFilePath, MAX_PATH);
	::PathRemoveFileSpec(szSwfFilePath);
	_tcscat_s(szSwfFilePath, TEXT("\\GLinking.swf"));

	g_flashWnd = new w2x::ui::CWndFlash();
	if (true == g_flashWnd->Create(szSwfFilePath, g_hWnd, g_hInst))
	{
		g_flashWnd->SetCaptionArea(0, 0, 600, 20);
		g_flashWnd->AddEventListener(
			w2x::events::CFlashEvent::EVENT_COMMAND, OnFlashCommand);
	}

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	delete g_flashWnd;
	
	return msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, 
	   WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
      CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   //ShowWindow(hWnd, nCmdShow);
   
   UpdateWindow(hWnd);

   g_hWnd = hWnd;

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
		
	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_DESTROY:
			SendMessage(g_flashWnd->GetHwnd(), WM_CLOSE, 0, 0);
			PostQuitMessage(0);
			break;
		case WM_MOVE:
			{
				RECT r;
				GetWindowRect(hWnd, &r);
				if (g_flashWnd) {
					SetWindowPos(g_flashWnd->GetHwnd(), NULL, r.left, r.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				}
			}
			break;
		case WM_SIZE:
			{
				RECT r;
				GetWindowRect(hWnd, &r);
				if (g_flashWnd) {
					SetWindowPos(g_flashWnd->GetHwnd(), NULL, 0, 0, (r.right-r.left), (r.bottom-r.top), SWP_NOMOVE | SWP_NOZORDER);
				}
			}
			break;
		case WM_LBUTTONDBLCLK:
			{
				::PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

void OnFlashCommand(const w2x::events::CEvent& _event_ref)
{
	using namespace w2x::events;
	const CFlashEvent& flash_event = static_cast<const CFlashEvent&>(_event_ref);
	LPCTSTR cmmmand = flash_event.GetCommand();
	LPCTSTR args = flash_event.GetCommandArgs();
	_event_ref.GetTypeName();
	//g_flashWnd->CallFunction(TEXT("Call"))
}
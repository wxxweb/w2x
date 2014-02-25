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
 * 该实现很完美，指出了实现像QQ宠物那样效果的本质方法，但唯一不足
 * 就是在16位24位色下处理的不好。如果这样处理肯定不能拿去商用。
 * 下面给出16位和24位色的解决方案。
 * 1、可以先将Flash每一帧的Alpha值预先保存起来，然后
 *	在此根据像素计算设置进去。因为在Draw的过程中，创建
 *	了32位设备无关的位图，所以在FlashPlayer在该DC上绘制
 *	完毕后，可以将DC中的位图数据得到，并将Alpha通过计算填写
 *	进去。仔细观察一下，即便是在16位色模式下，FlashPlayer
 *	并不是一个像素只适用两个字节，而是使用了3个字节，所以
 *	将Alpha填写到第四个字节就能解决问题。但这样做太笨拙了，
 *	预先要截取每一帧的Alpha值，而且又要对每一个像素重新计算，
 *	效率太低。
 *   2、第二种方法一劳永逸啊，那就是欺骗FlashPlayer。通过
 *	钩子技术用自己定义的函数告诉FlashPlayer目前的色位是32
 *	位，而不管实际的色位是多少。GetDevicesCap是获得色位的
 *	API，而且发现FlashPlayer.ocx的导入表中确实对该函数进行
 *	了调用，所以Hook它。------------陈建化 2009-5-2
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
	_tcscat_s(szSwfFilePath, TEXT("\\AS3VsCpp.swf"));

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

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
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
					g_flashWnd->SetCaptionArea(0, 0, r.right-r.left, 20);
				}
			}
			break;
		case WM_SIZE:
			{
				RECT r;
				GetWindowRect(hWnd, &r);
				if (g_flashWnd) {
					SetWindowPos(g_flashWnd->GetHwnd(), NULL, 0, 0, (r.right-r.left), (r.bottom-r.top), SWP_NOMOVE | SWP_NOZORDER);
					g_flashWnd->SetCaptionArea(0, 0, r.right-r.left, 20);
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
	_event_ref.GetTypeName();
	//g_flashWnd->CallFunction(TEXT("Call"))
}
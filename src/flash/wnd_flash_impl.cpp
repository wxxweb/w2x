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
// FlashWnd.cpp: implementation of the COleContainerWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wnd_flash_impl.h"
#include "..\common\macros.h"
#include "flash_event.h"
#include <ddraw.h>

enum {
	DISP_EVENT_ID_FSCOMMAND = 150,	// ActionScript 的 fscommand 调用事件
	DISP_EVENT_ID_FLASH_CALL = 197,	// ActionScript 3.0 的 ExternalInterface.call 调用事件
};


#ifndef DEFINE_GUID2
#define DEFINE_GUID2(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	const GUID name \
	= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#endif

#define NOTIMPLEMENTED return E_NOTIMPL

#define OLECONTAINER_DEF template <class TObj>
#define OLECONTAINER_DEF2 TObj

#define OLECONTAINER_CONSTRUCT OLECONTAINER_DEF COleContainerWnd<OLECONTAINER_DEF2>
#define OLECONTAINER(type) OLECONTAINER_DEF type COleContainerWnd<OLECONTAINER_DEF2>

#define OLE_IUNKNOWN_DECLARE \
	int m_iRef; \
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject); \
	ULONG STDMETHODCALLTYPE AddRef(); \
	ULONG STDMETHODCALLTYPE Release();

typedef BOOL (WINAPI *MYFUNC)(HWND,HDC,POINT*,SIZE*,HDC,POINT*,COLORREF,BLENDFUNCTION*,DWORD);

//DirectDraw GUIDS

DEFINE_GUID2(CLSID_DirectDraw,0xD7B70EE0,0x4340,0x11CF,0xB0,0x63,0x00,0x20,0xAF,0xC2,0xCD,0x35);
DEFINE_GUID2(CLSID_DirectDraw7,0x3c305196,0x50db,0x11d3,0x9c,0xfe,0x00,0xc0,0x4f,0xd9,0x30,0xc5);

DEFINE_GUID2(IID_IDirectDraw,0x6C14DB80,0xA733,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60);
DEFINE_GUID2(IID_IDirectDraw3,0x618f8ad4,0x8b7a,0x11d0,0x8f,0xcc,0x0,0xc0,0x4f,0xd9,0x18,0x9d);
DEFINE_GUID2(IID_IDirectDraw4,0x9c59509a,0x39bd,0x11d1,0x8c,0x4a,0x00,0xc0,0x4f,0xd9,0x30,0xc5);
DEFINE_GUID2(IID_IDirectDraw7,0x15e65ec0,0x3b9c,0x11d2,0xb9,0x2f,0x00,0x60,0x97,0x97,0xea,0x5b);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OLECONTAINER_CONSTRUCT::COleContainerWnd()
{
	m_lpControl = NULL;
	m_lpO = NULL;
	m_lpViewObjectEx = NULL;
	m_lpViewObject = NULL;
	m_lpInPlaceObj = NULL;
	m_lpInPlaceObjWindowless = NULL;
	m_lpConCont = NULL;
	m_lpConPoint = NULL;

	m_hdcBack = NULL;
	m_bmpBack = NULL;
	m_hdcBackW = NULL;
	m_bmpBackW = NULL;
	m_rcBounds.left = m_rcBounds.top = m_rcBounds.right = m_rcBounds.bottom = 0;
	m_lpBitsOnly = NULL;
	m_lpBitsOnlyW = NULL;
	m_iBPP = 0;

	m_dwConPointID = 0;
	m_bTransparent = FALSE;
	m_bFixTransparency = FALSE;
	m_iRef = 0;
} 

OLECONTAINER_CONSTRUCT::~COleContainerWnd()
{
	if (m_lpControl)
	{
		if (m_lpConPoint)
		{
			if (m_dwConPointID)
				m_lpConPoint->Unadvise(m_dwConPointID);
			m_lpConPoint->Release();
		}
		if (m_lpConCont)
			m_lpConCont->Release();
		m_lpO->Close(OLECLOSE_NOSAVE);
		if (m_lpViewObjectEx)
			m_lpViewObjectEx->Release();
		if (m_lpViewObject)
			m_lpViewObject->Release();
		if (m_lpInPlaceObjWindowless)
			m_lpInPlaceObjWindowless->Release();
		if (m_lpInPlaceObj)
			m_lpInPlaceObj->Release();
		if (m_lpO)
			m_lpO->Release();
		m_lpControl->Release();
	}
	if (m_hdcBack)
		::DeleteDC(m_hdcBack);
	if (m_bmpBack)
		::DeleteObject(m_bmpBack);
	if (m_hdcBackW)
		::DeleteDC(m_hdcBackW);
	if (m_bmpBackW)
		::DeleteObject(m_bmpBackW);
}
OLECONTAINER(BOOL)::Create(GUID clsid, DWORD dwExStyle, DWORD dwStyle, HWND hWndParent, HINSTANCE hInst)
{
	m_hWndParent = hWndParent;
	m_hInst = hInst;

	m_CLSID = clsid;
	m_bTransparent = dwExStyle & WS_EX_LAYERED;
	m_bChild = dwStyle & WS_CHILD;

	WNDCLASSEX wcs = {0};
	wcs.cbSize = sizeof(WNDCLASSEX);
	wcs.lpfnWndProc = COleContainerWnd<TObj>::WndProcStatic;
	wcs.hInstance = hInst;
	wcs.lpszClassName = TEXT("MyOleControl");
	RegisterClassEx(&wcs);

	RECT r;
	if (m_bChild)
		::GetClientRect(hWndParent, &r);
	else
		::GetWindowRect(hWndParent, &r);
	m_hWnd = CreateWindowEx(dwExStyle,
		TEXT("MyOleControl"), TEXT("MyOleControlWindow"),
		dwStyle,
		r.left, r.top, r.right-r.left, r.bottom-r.top, hWndParent, NULL, hInst, (void *)this);
	//::SetWindowPos(GetHWND(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	HRESULT hr;
	hr = OleCreate(m_CLSID, IID_IOleObject, OLERENDER_DRAW,
		0, (IOleClientSite *)this, (IStorage *)this, (void **)&m_lpO);
	if (FAILED(hr))
	{
		if (hr == REGDB_E_CLASSNOTREG)
		{
			OnErrorClassNotReg();
		}
		return FALSE;
	}
	hr = OleSetContainedObject(m_lpO, TRUE);
	if (FAILED(hr))
		return FALSE;
	hr = m_lpO->QueryInterface(__uuidof(TObj), (void **)&m_lpControl);
	if (FAILED(hr))
		return FALSE;
	hr = m_lpO->QueryInterface(IID_IViewObjectEx, (void **)&m_lpViewObjectEx);
	if (FAILED(hr))
	{
		m_lpViewObjectEx = NULL;
		hr = m_lpO->QueryInterface(IID_IViewObject, (void **)&m_lpViewObject);
		if (FAILED(hr))
			return FALSE;
	}
	if (m_bTransparent)
	{
		hr = m_lpO->QueryInterface(IID_IOleInPlaceObjectWindowless, (void **)&m_lpInPlaceObjWindowless);
		
		if (FAILED(hr))
		{
			hr = m_lpO->QueryInterface(IID_IOleInPlaceObject, (void **)&m_lpInPlaceObj);
			if (FAILED(hr))
				return FALSE;
			m_bTransparent = FALSE;
		}
	}
	else
	{
		hr = m_lpO->QueryInterface(IID_IOleInPlaceObject, (void **)&m_lpInPlaceObj);
		if (FAILED(hr))
			return FALSE;
	}

	if (!OnBeforeShowingContent())
		return FALSE;
	hr = m_lpO->DoVerb(OLEIVERB_SHOW, NULL, (IOleClientSite *)this, 0, NULL, NULL);
	if (FAILED(hr))
		return FALSE;
	if (!OnAfterShowingContent())
		return FALSE;
	return TRUE;
}

//interface methods

//IUnknown

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::QueryInterface(REFIID riid, void ** ppvObject)
{
	if (IsEqualGUID(riid, IID_IUnknown))
		*ppvObject = (void*)(this);
	else if (IsEqualGUID(riid, IID_IOleInPlaceSite))
		*ppvObject = (void*)dynamic_cast<IOleInPlaceSite *>(this);
	else if (IsEqualGUID(riid, IID_IOleInPlaceSiteEx))
		*ppvObject = (void*)dynamic_cast<IOleInPlaceSiteEx *>(this);
	else if (IsEqualGUID(riid, IID_IOleInPlaceSiteWindowless))
		*ppvObject = (void*)dynamic_cast<IOleInPlaceSiteWindowless *>(this);
	else if (IsEqualGUID(riid, IID_IOleInPlaceFrame))
		*ppvObject = (void*)dynamic_cast<IOleInPlaceFrame *>(this);
	else if (IsEqualGUID(riid, IID_IStorage))
		*ppvObject = (void*)dynamic_cast<IStorage *>(this);
	else
	{
		*ppvObject = 0;
		return E_NOINTERFACE;
	}
	if (!(*ppvObject))
		return E_NOINTERFACE; //if dynamic_cast returned 0
	m_iRef++;
	return S_OK;
}

OLECONTAINER(ULONG STDMETHODCALLTYPE)::AddRef()
{
	m_iRef++;
	return m_iRef;
}

OLECONTAINER(ULONG STDMETHODCALLTYPE)::Release()
{
	m_iRef--;
	return m_iRef;
}

//IOleClientSite

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::SaveObject() { NOTIMPLEMENTED; }
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker ** ppmk) { NOTIMPLEMENTED; }
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::GetContainer(LPOLECONTAINER FAR* ppContainer)
{
	*ppContainer = 0;
  	return E_NOINTERFACE;
}
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::ShowObject() 
{
	return S_OK;
}
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::OnShowWindow(BOOL fShow) { NOTIMPLEMENTED; }
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::RequestNewObjectLayout() { NOTIMPLEMENTED; }

//IOleInPlaceSite

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::GetWindow(HWND FAR* lphwnd)
{
	*lphwnd = GetHWND();
	return S_OK;
}
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::ContextSensitiveHelp(BOOL fEnterMode) { NOTIMPLEMENTED; }
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::CanInPlaceActivate()
{
	return(S_OK);
}
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::OnInPlaceActivate()
{
	return(S_OK);
}
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::OnUIActivate()
{
	return(S_OK);
}
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::GetWindowContext(LPOLEINPLACEFRAME FAR* lplpFrame, LPOLEINPLACEUIWINDOW FAR* lplpDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	*lplpFrame = (LPOLEINPLACEFRAME)this;

	*lplpDoc = 0;

	lpFrameInfo->fMDIApp = FALSE;
	lpFrameInfo->hwndFrame = GetHWND();
	lpFrameInfo->haccel = 0;
	lpFrameInfo->cAccelEntries = 0;
	
	RECT r;
	::GetClientRect(GetHWND(), &r);
	*lprcPosRect = r;
	*lprcClipRect = r;
	return(S_OK);
}
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::Scroll(SIZE scrollExtent) { NOTIMPLEMENTED; }
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::OnUIDeactivate(BOOL fUndoable)
{
	return(S_OK);
}
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::OnInPlaceDeactivate()
{
	return(S_OK);
}
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::DiscardUndoState() { NOTIMPLEMENTED; }
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::DeactivateAndUndo() { NOTIMPLEMENTED; }
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::OnPosRectChange(LPCRECT lprcPosRect)
{
	return(S_OK);
}


//IOleInPlaceSiteEx

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::OnInPlaceActivateEx(BOOL __RPC_FAR *pfNoRedraw, DWORD dwFlags)
{
	if (pfNoRedraw)
		*pfNoRedraw = FALSE;
	return S_OK;
}
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::OnInPlaceDeactivateEx(BOOL fNoRedraw)
{
	return S_FALSE;
}
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::RequestUIActivate(void)
{
	return S_FALSE;
}


//IOleInPlaceSiteWindowless

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::CanWindowlessActivate( void)
{
	return S_OK;
}
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::GetCapture( void)
{
	return S_FALSE;
} 
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::SetCapture( 
    /* [in] */ BOOL fCapture)
{
	return S_FALSE;
}	 
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::GetFocus( void)
{
	return S_OK;
} 
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::SetFocus( 
    /* [in] */ BOOL fFocus)
{
	return S_OK;
}	  
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::GetDC( 
    /* [in] */ LPCRECT pRect,
    /* [in] */ DWORD grfFlags,
    /* [out] */ HDC __RPC_FAR *phDC)
{
	return S_FALSE;
}			 
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::ReleaseDC( 
    /* [in] */ HDC hDC)
{
	return S_FALSE;
} 
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::InvalidateRect( 
    /* [in] */ LPCRECT pRect,
    /* [in] */ BOOL fErase)
{
	Draw(NULL, pRect, fErase);
	return S_OK;
} 
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::InvalidateRgn( 
    /* [in] */ HRGN hRGN,
    /* [in] */ BOOL fErase)
{
	return S_OK;
} 
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::ScrollRect( 
    /* [in] */ INT dx,
    /* [in] */ INT dy,
    /* [in] */ LPCRECT pRectScroll,
    /* [in] */ LPCRECT pRectClip)
{
	NOTIMPLEMENTED;
} 
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::AdjustRect( 
    /* [out][in] */ LPRECT prc)
{
	return S_FALSE;
}
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::OnDefWindowMessage( 
    /* [in] */ UINT msg,
    /* [in] */ WPARAM wParam,
    /* [in] */ LPARAM lParam,
    /* [out] */ LRESULT __RPC_FAR *plResult)
{
	return S_FALSE;
} 
#include <stdio.h>
void PreMultiplyRGBChannels(long w, long h, LPBYTE pBitmapBits)
{
	// pre-multiply rgb channels with alpha channel
/*
	static int c = 0;
	int inc = 0;
	unsigned char alpha_bytes[102400];
	c++;
	if (c != 3)
	{
		return;
	}

	FILE *f = fopen("e:\\ab.bin", "rb");
	
	if (!f)
	{
		return;
	}

	int len = fread(alpha_bytes, 30000, 1, f);
	fclose(f);

	for (int y=0; y<h; ++y)
	{
		BYTE *pPixel= pBitmapBits + w * 4 * y;
		
		for (int x=0; x < w ; ++x)
		{
// 			pPixel[0]= pPixel[0]*pPixel[3]/255;
// 			pPixel[1]= pPixel[1]*pPixel[3]/255;
// 			pPixel[2]= pPixel[2]*pPixel[3]/255;
			pPixel[3] = alpha_bytes[inc++];

			pPixel+= 4;
		}
	}

// 	FILE *f = fopen("e:\\ab.bin", "wb+");
// 	fwrite(alpha_bytes, inc, 1, f);
// 	fclose(f);
*/
}

#include <stdio.h>
OLECONTAINER(void)::Draw(HDC hdcDraw, const RECT *rcDraw, BOOL bErase)
{
	HWND hwnd = GetHWND();
	HRESULT hr;
	RECT r;
	MYFUNC UpdateLayeredWindow;
	
	HINSTANCE hFuncInst = LoadLibrary(TEXT("User32.DLL")); 
	BOOL bRet=FALSE;
	if(hFuncInst) 
		UpdateLayeredWindow=(MYFUNC)GetProcAddress(hFuncInst, "UpdateLayeredWindow");
	else
	{
		exit(0);
	}

	IOleObject *lpO = m_lpO;
	IViewObject *lpV = m_lpViewObjectEx ? (IViewObject *)m_lpViewObjectEx : m_lpViewObject;

	if (!m_bTransparent)
	{
		RECT rTotal;
			::GetClientRect(hwnd, &rTotal);
		if (lpV)
		{
			if (!hdcDraw)
			{
				hdcDraw = ::GetDC(hwnd);
				hr = OleDraw(lpV, DVASPECT_CONTENT, hdcDraw, &rTotal);
				::ReleaseDC(hwnd, hdcDraw);
			}
			else
			{
				hr = OleDraw(lpV, DVASPECT_CONTENT, hdcDraw, &rTotal);
			}
		}
		return;
	}
	
	::GetWindowRect(hwnd, &r);
	
	if (!m_hdcBack || !EqualRect(&r, &m_rcBounds))
	{
		if (m_hdcBack)
			::DeleteDC(m_hdcBack);
		if (m_bmpBack)
			::DeleteObject(m_bmpBack);
		if (m_hdcBackW)
			::DeleteDC(m_hdcBackW);
		if (m_bmpBackW)
			::DeleteObject(m_bmpBackW);

		m_rcBounds = r;
		HDC hdc = ::GetDC(hwnd);
		BITMAPINFOHEADER bih = {0};
		bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biBitCount = 32;
		bih.biCompression = BI_RGB;
		bih.biPlanes = 1;
		bih.biWidth = r.right - r.left;
		bih.biHeight = -(r.bottom - r.top);
		m_hdcBack = CreateCompatibleDC(hdc);
		m_bmpBack = CreateDIBSection(hdc, (BITMAPINFO *)&bih, DIB_RGB_COLORS, (void **)&m_lpBitsOnly, NULL, 0x0);
		SelectObject(m_hdcBack, m_bmpBack);
		if (m_bFixTransparency)
		{
			m_hdcBackW = CreateCompatibleDC(hdc);
			m_bmpBackW = CreateDIBSection(hdc, (BITMAPINFO *)&bih, DIB_RGB_COLORS, (void **)&m_lpBitsOnlyW, NULL, 0x0);
			SelectObject(m_hdcBackW, m_bmpBackW);
		}
		::ReleaseDC(hwnd, hdc);
		if (m_iBPP == 0)
			m_iBPP = GetDeviceCaps(m_hdcBack, BITSPIXEL);
	}

	POINT p = {r.left, r.top};
	POINT p2 = {0, 0};
	SIZE sz = {r.right-r.left, r.bottom-r.top};

	if (lpO && lpV)
	{
		RECT rTotal;
		::GetClientRect(hwnd, &rTotal);
		RECTL rcBounds = {rTotal.left, rTotal.top, rTotal.right, rTotal.bottom};
		BYTE *dst = m_lpBitsOnly, *dstW;
		if (1)
		{
			if (!m_bFixTransparency) //if flash player version is other than 8, do usual painting
			{
				memset(m_lpBitsOnly, 0, sz.cx * sz.cy * 4);
				hr = OleDraw(lpV, DVASPECT_TRANSPARENT, m_hdcBack, &rTotal);
			}
			else //if player version is 8, we need to fix flash player 8 control transparency bug
			{
				memset(m_lpBitsOnly, 0, sz.cx * sz.cy * 4);
				memset(m_lpBitsOnlyW, 255, sz.cx * sz.cy * 4);
				hr = OleDraw(lpV, DVASPECT_TRANSPARENT, m_hdcBack, &rTotal);
				hr = OleDraw(lpV, DVASPECT_TRANSPARENT, m_hdcBackW, &rTotal);
				dst = m_lpBitsOnly;
				dstW = m_lpBitsOnlyW;
				BYTE r, g, b, a, rw, gw, bw, aw, alpha_r, alpha_g, alpha_b, alpha;
				for (int y = 0; y < sz.cy; y++)
				{
					for (int x = 0; x < sz.cx; x++)
					{
						//the idea is that we draw the same data onto black and white DC's
						//and then calculate per pixel alpha based on difference, produced by alpha blending
						r = *dst++;
						g = *dst++;
						b = *dst++;
						a = *dst++;
						rw = *dstW++;
						gw = *dstW++;
						bw = *dstW++;
						aw = *dstW++;
						alpha_r = rw-r;
						alpha_g = gw-g;
						alpha_b = bw-b;
						//division by 3 is for accuracy and can be replaced by
						//alpha = alpha_g; for example
						alpha = (alpha_r + alpha_g + alpha_b) / 3;
						*(dst - 1) = 255 - alpha;
						//this algorithm should be optimized for MMX to achieve best performance
					}
				} 
			}
		}
		else //in 8/16/24 bit screen depth UpdateLayeredWindow produces wrong results - we use underlaying DC to paint to
		{
			//解决方法见flash.cpp的说明部分
			//这段代码以后永远不会被执行！
			HWND hwndParent = ::GetParent(hwnd);
			HDC hdcParent = ::GetWindowDC(hwndParent);
			BOOL bRet = BitBlt(m_hdcBack, 0, 0, rTotal.right, rTotal.bottom, hdcParent, 0, 0, SRCCOPY);
			::ReleaseDC(hwndParent, hdcParent);
			hr = OleDraw(lpV, DVASPECT_TRANSPARENT, m_hdcBack, &rTotal);
			dst = m_lpBitsOnly;
		}
	}
	
	//BitBlt(::GetDC(0), 0, 0, 250, 250, m_hdcBack, 0, 0, SRCCOPY);
	//不管系统设置的是多少为，但在UpdateLayeredWindow前一定要转换成32为带Alpha通道的位图
	/*BITMAPINFO   _bmpInfo;   
	ZeroMemory(&_bmpInfo,   sizeof(BITMAPINFO));   
	_bmpInfo.bmiHeader.biSize   =   sizeof(BITMAPINFO);   
	int _res = GetDIBits(m_hdcBack, m_bmpBack, 0, 0, NULL, &_bmpInfo, DIB_RGB_COLORS);
	if(!_res)   
		return;   
	//get   data   
	_res   =   _bmpInfo.bmiHeader.biHeight   *   _bmpInfo.bmiHeader.biWidth   *   _bmpInfo.bmiHeader.biBitCount   /   8;   
	unsigned char *buf   =   new   unsigned   char[_res];   
	GetDIBits(m_hdcBack,   m_bmpBack,   (UINT)0,   (UINT)_bmpInfo.bmiHeader.biHeight,   (LPVOID)buf,   &_bmpInfo,   DIB_RGB_COLORS);
	
	FILE *ff = fopen ("e:\\f3.bin", "wb+");

	for (int i = 0; i < _res; ++i)
	{
		char temp[10];
		
		if ((i + 1) % 4 == 0)
		{
			sprintf(temp, "%02X\n", buf[i]);
		}
		else
		{
			sprintf(temp, "%02X ", buf[i]);
		}

		fwrite(temp, 3, 1, ff);
	}

	fclose(ff);

	PreMultiplyRGBChannels (_bmpInfo.bmiHeader.biWidth,
		_bmpInfo.bmiHeader.biHeight, buf);
	
	_res = SetDIBits(m_hdcBack, m_bmpBack, (UINT)0, (UINT)_bmpInfo.bmiHeader.biHeight, (LPVOID)buf, &_bmpInfo, DIB_RGB_COLORS);
	delete[] buf;
	buf = NULL;*/

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.AlphaFormat = AC_SRC_ALPHA;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bRet = UpdateLayeredWindow(hwnd, NULL, &p, &sz, m_hdcBack, &p2, 0, &bf, ULW_ALPHA);
}


//IStorage

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::CreateStream(const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm)
{
	NOTIMPLEMENTED;
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::OpenStream(const WCHAR * pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm)
{
	NOTIMPLEMENTED;
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::CreateStorage(const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg)
{
	NOTIMPLEMENTED;
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::OpenStorage(const WCHAR * pwcsName, IStorage * pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg)
{
	NOTIMPLEMENTED;
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::CopyTo(DWORD ciidExclude, IID const *rgiidExclude, SNB snbExclude,IStorage *pstgDest)
{
	NOTIMPLEMENTED;
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::MoveElementTo(const OLECHAR *pwcsName,IStorage * pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags)
{
	NOTIMPLEMENTED;
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::Commit(DWORD grfCommitFlags)
{
	NOTIMPLEMENTED;
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::Revert()
{
	NOTIMPLEMENTED;
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::EnumElements(DWORD reserved1, void * reserved2, DWORD reserved3, IEnumSTATSTG ** ppenum)
{
	NOTIMPLEMENTED;
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::DestroyElement(const OLECHAR *pwcsName)
{
	NOTIMPLEMENTED;
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::RenameElement(const WCHAR *pwcsOldName, const WCHAR *pwcsNewName)
{
	NOTIMPLEMENTED;
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::SetElementTimes(const WCHAR *pwcsName, FILETIME const *pctime, FILETIME const *patime, FILETIME const *pmtime)
{
	NOTIMPLEMENTED;
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::SetClass(REFCLSID clsid)
{
	return(S_OK);
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::SetStateBits(DWORD grfStateBits, DWORD grfMask)
{
	NOTIMPLEMENTED;
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::Stat(STATSTG * pstatstg, DWORD grfStatFlag)
{
	NOTIMPLEMENTED;
}


//IOleInPlaceFrame

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::GetBorder(LPRECT lprectBorder)
{
	NOTIMPLEMENTED;
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
	NOTIMPLEMENTED;
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::SetBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
	NOTIMPLEMENTED;
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName)
{
	return(S_OK);
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
	NOTIMPLEMENTED;
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
{
	return(S_OK);
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::RemoveMenus(HMENU hmenuShared)
{
	NOTIMPLEMENTED;
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::SetStatusText(LPCOLESTR pszStatusText)
{
	return(S_OK);
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::EnableModeless(BOOL fEnable)
{
	return(S_OK);
}

OLECONTAINER(HRESULT STDMETHODCALLTYPE)::TranslateAccelerator(LPMSG lpmsg, WORD wID)
{
	NOTIMPLEMENTED;
}


//IDispatch
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::GetTypeInfoCount(UINT __RPC_FAR *pctinfo)
{
	NOTIMPLEMENTED;
}
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::GetTypeInfo( 
    /* [in] */ UINT iTInfo,
    /* [in] */ LCID lcid,
    /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo)
{
	NOTIMPLEMENTED;
}
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::GetIDsOfNames( 
    /* [in] */ REFIID riid,
    /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
    /* [in] */ UINT cNames,
    /* [in] */ LCID lcid,
    /* [size_is][out] */ DISPID __RPC_FAR *rgDispId)
{
	NOTIMPLEMENTED;
}
OLECONTAINER(HRESULT STDMETHODCALLTYPE)::Invoke( 
    /* [in] */ DISPID dispIdMember,
    /* [in] */ REFIID riid,
    /* [in] */ LCID lcid,
    /* [in] */ WORD wFlags,
    /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
    /* [out] */ VARIANT __RPC_FAR *pVarResult,
    /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
    /* [out] */ UINT __RPC_FAR *puArgErr)
{
/*	switch (dispIdMember)
	{
	default:
		return S_OK;
	}*/
	return S_OK;
}

OLECONTAINER(void)::OnErrorClassNotReg()
{
}
OLECONTAINER(BOOL)::OnBeforeShowingContent()
{
	return TRUE;
}
OLECONTAINER(BOOL)::OnAfterShowingContent()
{
	return TRUE;
}

OLECONTAINER(LRESULT CALLBACK)::WndProcStatic(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CREATE)
	{
		LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
		SetWindowLong(hWnd, GWL_USERDATA, (long)lpcs->lpCreateParams);
		return 0;
	}
	COleContainerWnd<TObj> *lpWnd = (COleContainerWnd<TObj> *)GetWindowLong(hWnd, GWL_USERDATA);
	if (lpWnd)
		return lpWnd->WndProc(hWnd, uMsg, wParam, lParam);
	else
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
OLECONTAINER(LRESULT)::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		{
			LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
			SetWindowLong(hWnd, GWL_USERDATA, (long)lpcs->lpCreateParams);
			return 0;
		}
		break;
	case WM_PAINT:
		{
			if (!m_bTransparent)
			{
				PAINTSTRUCT ps;
				HDC hdc = ::BeginPaint(GetHWND(), &ps);
				Draw(hdc, &ps.rcPaint, ps.fErase);
				::EndPaint(GetHWND(), &ps);
				return 0;
			}
		}
		break;
	case WM_NCHITTEST:
		{
			int x = LOWORD(lParam), y = HIWORD(lParam);
			if (m_lpO && m_lpViewObjectEx)
			{
				IViewObjectEx *lpV = m_lpViewObjectEx;
				POINT p = {x, y};
				DWORD dwRes;
				RECT rTotal;
				GetWindowRect(GetHWND(), &rTotal);
				HRESULT hr = lpV->QueryHitPoint(DVASPECT_CONTENT, &rTotal, p, 1, &dwRes);
				if (hr == S_OK)
				{
					if (dwRes == HITRESULT_OUTSIDE)
						return HTTRANSPARENT;
					else
					{
						return HTCLIENT;
					}
				}
			}
		}
		break;
	case WM_LBUTTONDOWN:
		::ReleaseCapture();
		if (0 != m_caption_rect.right - m_caption_rect.left || 
			0 != m_caption_rect.bottom - m_caption_rect.top)
		{
			POINT pt = {0};
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			if (TRUE == ::PtInRect(&m_caption_rect, pt))
			{
				::SendMessage (hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
			}
		}
		break;
	case WM_RBUTTONDOWN:
		return TRUE;
	case WM_SIZE:
		{
			HRESULT hr;
			RECT rPos;
			GetClientRect(GetHWND(), &rPos);
			RECT rClip = rPos;
			if (m_lpInPlaceObjWindowless)
				hr = m_lpInPlaceObjWindowless->SetObjectRects(&rPos, &rClip);
			else if (m_lpInPlaceObj)
				hr = m_lpInPlaceObj->SetObjectRects(&rPos, &rClip);
			return 0;
		}
		break;
	}
	if (m_lpInPlaceObjWindowless)
	{
		if (uMsg == WM_MOUSEMOVE || uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_LBUTTONDBLCLK
			|| uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP || uMsg == WM_RBUTTONDBLCLK
			|| uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP || uMsg == WM_MBUTTONDBLCLK
			|| uMsg == WM_MOUSEWHEEL 
			|| uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || uMsg == WM_CHAR
			|| uMsg == WM_SETCURSOR
			)
		{
			HRESULT hr;
			LRESULT res;
			hr = m_lpInPlaceObjWindowless->OnWindowMessage(uMsg, wParam, lParam, &res);
			if (hr == S_OK)
				return res;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

CWndFlashImpl::CWndFlashImpl()
	: m_lVersion(0)
	, m_lpDD4(NULL)
	, m_pszSwfFilePath(NULL)
	, m_event_disp(NULL)
{
	memset(&m_caption_rect, 0, sizeof(m_caption_rect));
}

CWndFlashImpl::~CWndFlashImpl()
{
	if (m_lpDD4)
		m_lpDD4->Release();

	SAFE_DELETE_ARRAY(m_pszSwfFilePath);
}

//DShockwaveFlashEvents
HRESULT STDMETHODCALLTYPE CWndFlashImpl::OnReadyStateChange(long newState)
{
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CWndFlashImpl::OnProgress(long percentDone)
{
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CWndFlashImpl::FSCommand(_bstr_t command, _bstr_t args)
{
	using namespace w2x::events;
	static CFlashEvent s_flash_event(CFlashEvent::EVENT_COMMAND);

	s_flash_event.SetCommand(command, args);

	m_event_disp.DispatchEvent(s_flash_event);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CWndFlashImpl::OnFlashCall(_bstr_t _args)
{
	using namespace w2x::events;
	static CFlashEvent s_flash_event(CFlashEvent::EVENT_COMMAND);

	s_flash_event.SetCommand(TEXT("FlashCall"), _args);

	m_event_disp.DispatchEvent(s_flash_event);

	return S_OK;
}

void CWndFlashImpl::OnErrorClassNotReg()
{
	//some notification code here
}
BOOL CWndFlashImpl::OnBeforeShowingContent()
{
	m_lVersion = m_lpControl->FlashVersion();
	if ((m_lVersion & 0x00FF0000) == 0x00080000)
		m_bFixTransparency = TRUE;
	else
		m_bFixTransparency = FALSE;
	HRESULT hr;
	hr = m_lpControl->QueryInterface(IID_IConnectionPointContainer, (void**)&m_lpConCont);
	if (FAILED(hr))
		return FALSE;
	hr = m_lpConCont->FindConnectionPoint(ShockwaveFlashObjects::DIID__IShockwaveFlashEvents, &m_lpConPoint);
	if (FAILED(hr))
		return FALSE;
	hr = m_lpConPoint->Advise((ShockwaveFlashObjects::_IShockwaveFlashEvents *)this, &m_dwConPointID);
	if (FAILED(hr))
		return FALSE;

	if (m_bTransparent) {
		m_lpControl->PutWMode(L"transparent");
	}
	m_lpControl->PutScale(L"showAll");
	m_lpControl->PutBackgroundColor(0x00000000);
	m_lpControl->PutEmbedMovie(FALSE);
	return TRUE;
}
BOOL CWndFlashImpl::OnAfterShowingContent()
{
	IF_NULL_ASSERT_RETURN_VALUE(m_pszSwfFilePath, FALSE);

	HRESULT hr;
	//ShowWindow(m_hWndParent, SW_HIDE);
	m_lpControl->PutEmbedMovie(TRUE);

	hr = m_lpControl->LoadMovie(0, _bstr_t(m_pszSwfFilePath));
	SAFE_DELETE_ARRAY(m_pszSwfFilePath);

	if (FAILED(hr))
		return FALSE;
	hr = m_lpControl->Play();
	if (FAILED(hr))
		return FALSE;
	return TRUE;
}

//IUnknown

HRESULT STDMETHODCALLTYPE CWndFlashImpl::QueryInterface(REFIID riid, void ** ppvObject)
{
	HRESULT hr = COleContainerWnd<ShockwaveFlashObjects::IShockwaveFlash>::QueryInterface(riid, ppvObject);
	if (hr != E_NOINTERFACE)
		return hr;
	if (IsEqualGUID(riid, ShockwaveFlashObjects::DIID__IShockwaveFlashEvents))
		*ppvObject = (void*)dynamic_cast<ShockwaveFlashObjects::_IShockwaveFlashEvents *>(this);
	else if (IsEqualGUID(riid, IID_IServiceProvider))
		*ppvObject = (void*)dynamic_cast<IServiceProvider *>(this);
	else
	{
		*ppvObject = 0;
		return E_NOINTERFACE;
	}
	if (!(*ppvObject))
		return E_NOINTERFACE; //if dynamic_cast returned 0
	m_iRef++;
	return S_OK;
}

ULONG STDMETHODCALLTYPE CWndFlashImpl::AddRef()
{
	m_iRef++;
	return m_iRef;
}

ULONG STDMETHODCALLTYPE CWndFlashImpl::Release()
{
	m_iRef--;
	return m_iRef;
}

//IDispatch
HRESULT STDMETHODCALLTYPE CWndFlashImpl::GetTypeInfoCount(UINT __RPC_FAR *pctinfo)
{
	NOTIMPLEMENTED;
}
HRESULT STDMETHODCALLTYPE CWndFlashImpl::GetTypeInfo( 
    /* [in] */ UINT iTInfo,
    /* [in] */ LCID lcid,
    /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo)
{
	NOTIMPLEMENTED;
}
HRESULT STDMETHODCALLTYPE CWndFlashImpl::GetIDsOfNames( 
    /* [in] */ REFIID riid,
    /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
    /* [in] */ UINT cNames,
    /* [in] */ LCID lcid,
    /* [size_is][out] */ DISPID __RPC_FAR *rgDispId)
{
	NOTIMPLEMENTED;
}
HRESULT STDMETHODCALLTYPE CWndFlashImpl::Invoke( 
    /* [in] */ DISPID dispIdMember,
    /* [in] */ REFIID riid,
    /* [in] */ LCID lcid,
    /* [in] */ WORD wFlags,
    /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
    /* [out] */ VARIANT __RPC_FAR *pVarResult,
    /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
    /* [out] */ UINT __RPC_FAR *puArgErr)
{
	switch(dispIdMember)   
	{    
	case DISP_EVENT_ID_FSCOMMAND:             
		if ((pDispParams->cArgs == 2) &&   
			(pDispParams->rgvarg[0].vt == VT_BSTR) &&   
			(pDispParams->rgvarg[1].vt == VT_BSTR))   
		{   
			this->FSCommand(pDispParams->rgvarg[1].bstrVal, 
				pDispParams->rgvarg[0].bstrVal);   
		}
		break;
	case DISP_EVENT_ID_FLASH_CALL:
		if (1 == pDispParams->cArgs && VT_BSTR == pDispParams->rgvarg[0].vt)   
		{   
			this->OnFlashCall(pDispParams->rgvarg[0].bstrVal);
		}   
		break;
	case DISPID_READYSTATECHANGE:                      
		break;   
	default:               
		return DISP_E_MEMBERNOTFOUND;  
	}    

	return NOERROR;
}

HRESULT __stdcall CWndFlashImpl::raw_RemoteQueryService (REFGUID guidService, REFIID riid,IUnknown ** ppvObject )
{
	HRESULT hr;
	if (IsEqualGUID(guidService, IID_IDirectDraw3))
	{
		if (!m_lpDD4)
		{
			m_lpDD4 = new IDirectDraw4Ptr;
			hr = m_lpDD4->CreateInstance(CLSID_DirectDraw, NULL, CLSCTX_INPROC_SERVER); 
			if (FAILED(hr))
			{
				delete m_lpDD4;
				m_lpDD4 = NULL;
				return E_NOINTERFACE;
			}
		}
		if (m_lpDD4 && m_lpDD4->GetInterfacePtr())
		{
			*ppvObject = m_lpDD4->GetInterfacePtr();
			m_lpDD4->AddRef();
			return S_OK;
		}
	}
	return E_NOINTERFACE;
}


bool CWndFlashImpl::SetMovie(LPCTSTR pszSwfFilePath)
{
	if (FALSE == ::PathFileExists(pszSwfFilePath))
	{
		return false;
	}

	const size_t nCount = _tcslen(pszSwfFilePath) + 1;
	m_pszSwfFilePath = new TCHAR[nCount];
	if (NULL == m_pszSwfFilePath)
	{
		return false;
	}

	memcpy(m_pszSwfFilePath, pszSwfFilePath, nCount * sizeof(TCHAR));

	return true;
}
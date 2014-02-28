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
// FlashWnd.h: interface for the CWndFlashImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLASHWND_H__D74EA89B_5D79_4D87_8BB9_4DB90A5FE8FC__INCLUDED_)
#define AFX_FLASHWND_H__D74EA89B_5D79_4D87_8BB9_4DB90A5FE8FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\common\event_dispatcher.h"
#include <ddraw.h>

//#import "c:\\winnt\\system32\\macromed\\flash\\flash.ocx" named_guids
//#import "c:\\windows\\system32\\macromed\\flash\\Flash.ocx" named_guids
#import "Flash64_12_0_0_44.ocx" named_guids

struct __declspec(uuid("9C59509A-39BD-11D1-8C4A-00C04FD930C5")) IDirectDraw4;
_COM_SMARTPTR_TYPEDEF(IDirectDraw4,   __uuidof(IDirectDraw4));


template <class TObj>
class COleContainerWnd : 
	public IOleClientSite,
	public IOleInPlaceSiteWindowless,
	public IOleInPlaceFrame,
	public IStorage
{
public:						 
	GUID m_CLSID;
	TObj *m_lpControl;
	IOleObject *m_lpO;
	IViewObjectEx *m_lpViewObject;
	IViewObjectEx *m_lpViewObjectEx;
	IOleInPlaceObject *m_lpInPlaceObj;
	IOleInPlaceObjectWindowless *m_lpInPlaceObjWindowless;
	IConnectionPointContainer *m_lpConCont;
	IConnectionPoint *m_lpConPoint;

	HWND m_hWnd, m_hWndParent;
	HINSTANCE m_hInst;

	BOOL m_bChild;
	BOOL m_bTransparent;
	BOOL m_bFixTransparency;
	DWORD m_dwConPointID;
	HDC m_hdcBack;
	HBITMAP m_bmpBack;
	RECT m_rcBounds;
	BYTE *m_lpBitsOnly;
	int m_iBPP;
	HDC m_hdcBackW;
	HBITMAP m_bmpBackW;
	BYTE *m_lpBitsOnlyW;
	RECT m_caption_rect;
public:
	COleContainerWnd();
	virtual ~COleContainerWnd();
	static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HWND GetHWND() { assert(m_hWnd); return m_hWnd; }
	HWND GetParentWindow() { assert(m_hWndParent); return m_hWndParent; }
	HWND GetInstance() { assert(m_hInst); return m_hInst; }
public:
	virtual BOOL Create(GUID clsid, DWORD dwExStyle, DWORD dwStyle, HWND hWndParent, HINSTANCE hInst);
	virtual void Draw(HDC hdcDraw, const RECT *rcDraw, BOOL bErase);
	//ole container events
	virtual void OnErrorClassNotReg();
	virtual	BOOL OnBeforeShowingContent();
	virtual	BOOL OnAfterShowingContent();
public:
	//interface methods

	//IUnknown 
	int m_iRef;
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	//IOleClientSite
	virtual HRESULT STDMETHODCALLTYPE SaveObject();
	virtual HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker ** ppmk);
	virtual HRESULT STDMETHODCALLTYPE GetContainer(LPOLECONTAINER FAR* ppContainer);
	virtual HRESULT STDMETHODCALLTYPE ShowObject();
	virtual HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow);
	virtual HRESULT STDMETHODCALLTYPE RequestNewObjectLayout();

	//IOleInPlaceSite
	virtual HRESULT STDMETHODCALLTYPE GetWindow(HWND FAR* lphwnd);
	virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);
	virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate();
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivate();
	virtual HRESULT STDMETHODCALLTYPE OnUIActivate();
	virtual HRESULT STDMETHODCALLTYPE GetWindowContext(LPOLEINPLACEFRAME FAR* lplpFrame,LPOLEINPLACEUIWINDOW FAR* lplpDoc,LPRECT lprcPosRect,LPRECT lprcClipRect,LPOLEINPLACEFRAMEINFO lpFrameInfo);
	virtual HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtent);
	virtual HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable);
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate();
	virtual HRESULT STDMETHODCALLTYPE DiscardUndoState();
	virtual HRESULT STDMETHODCALLTYPE DeactivateAndUndo();
	virtual HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect);

	//IOleInPlaceSiteEx
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivateEx(BOOL __RPC_FAR *pfNoRedraw, DWORD dwFlags);
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivateEx(BOOL fNoRedraw);
	virtual HRESULT STDMETHODCALLTYPE RequestUIActivate(void);

	//IOleInPlaceSiteWindowless
    virtual HRESULT STDMETHODCALLTYPE CanWindowlessActivate( void);
    virtual HRESULT STDMETHODCALLTYPE GetCapture( void);
    virtual HRESULT STDMETHODCALLTYPE SetCapture( 
        /* [in] */ BOOL fCapture);
    virtual HRESULT STDMETHODCALLTYPE GetFocus( void);
    virtual HRESULT STDMETHODCALLTYPE SetFocus( 
        /* [in] */ BOOL fFocus);
    virtual HRESULT STDMETHODCALLTYPE GetDC( 
        /* [in] */ LPCRECT pRect,
        /* [in] */ DWORD grfFlags,
        /* [out] */ HDC __RPC_FAR *phDC);
    virtual HRESULT STDMETHODCALLTYPE ReleaseDC( 
        /* [in] */ HDC hDC);
    virtual HRESULT STDMETHODCALLTYPE InvalidateRect( 
        /* [in] */ LPCRECT pRect,
        /* [in] */ BOOL fErase);
    virtual HRESULT STDMETHODCALLTYPE InvalidateRgn( 
        /* [in] */ HRGN hRGN,
        /* [in] */ BOOL fErase);
    virtual HRESULT STDMETHODCALLTYPE ScrollRect( 
        /* [in] */ INT dx,
        /* [in] */ INT dy,
        /* [in] */ LPCRECT pRectScroll,
        /* [in] */ LPCRECT pRectClip);
    virtual HRESULT STDMETHODCALLTYPE AdjustRect( 
        /* [out][in] */ LPRECT prc);
    virtual HRESULT STDMETHODCALLTYPE OnDefWindowMessage( 
        /* [in] */ UINT msg,
        /* [in] */ WPARAM wParam,
        /* [in] */ LPARAM lParam,
        /* [out] */ LRESULT __RPC_FAR *plResult);

	//IOleInPlaceFrame
//	virtual HRESULT STDMETHODCALLTYPE GetWindow(HWND FAR* lphwnd);
//	virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);
	virtual HRESULT STDMETHODCALLTYPE GetBorder(LPRECT lprectBorder);
	virtual HRESULT STDMETHODCALLTYPE RequestBorderSpace(LPCBORDERWIDTHS pborderwidths);
	virtual HRESULT STDMETHODCALLTYPE SetBorderSpace(LPCBORDERWIDTHS pborderwidths);
	virtual HRESULT STDMETHODCALLTYPE SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName);
	virtual HRESULT STDMETHODCALLTYPE InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths);
	virtual HRESULT STDMETHODCALLTYPE SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject);
	virtual HRESULT STDMETHODCALLTYPE RemoveMenus(HMENU hmenuShared);
	virtual HRESULT STDMETHODCALLTYPE SetStatusText(LPCOLESTR pszStatusText);
	virtual HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable);
	virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpmsg, WORD wID);

	//IStorage
	virtual HRESULT STDMETHODCALLTYPE CreateStream(const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm);
	virtual HRESULT STDMETHODCALLTYPE OpenStream(const WCHAR * pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm);
	virtual HRESULT STDMETHODCALLTYPE CreateStorage(const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg);
	virtual HRESULT STDMETHODCALLTYPE OpenStorage(const WCHAR * pwcsName, IStorage * pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg);
	virtual HRESULT STDMETHODCALLTYPE CopyTo(DWORD ciidExclude, IID const *rgiidExclude, SNB snbExclude,IStorage *pstgDest);
	virtual HRESULT STDMETHODCALLTYPE MoveElementTo(const OLECHAR *pwcsName,IStorage * pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags);
	virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags);
	virtual HRESULT STDMETHODCALLTYPE Revert();
	virtual HRESULT STDMETHODCALLTYPE EnumElements(DWORD reserved1, void * reserved2, DWORD reserved3, IEnumSTATSTG ** ppenum);
	virtual HRESULT STDMETHODCALLTYPE DestroyElement(const OLECHAR *pwcsName);
	virtual HRESULT STDMETHODCALLTYPE RenameElement(const WCHAR *pwcsOldName, const WCHAR *pwcsNewName);
	virtual HRESULT STDMETHODCALLTYPE SetElementTimes(const WCHAR *pwcsName, FILETIME const *pctime, FILETIME const *patime, FILETIME const *pmtime);
	virtual HRESULT STDMETHODCALLTYPE SetClass(REFCLSID clsid);
	virtual HRESULT STDMETHODCALLTYPE SetStateBits(DWORD grfStateBits, DWORD grfMask);
	virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG * pstatstg, DWORD grfStatFlag);

	//IDispatch
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( 
        /* [out] */ UINT __RPC_FAR *pctinfo);
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( 
        /* [in] */ UINT iTInfo,
        /* [in] */ LCID lcid,
        /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
    virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( 
        /* [in] */ REFIID riid,
        /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
        /* [in] */ UINT cNames,
        /* [in] */ LCID lcid,
        /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke( 
        /* [in] */ DISPID dispIdMember,
        /* [in] */ REFIID riid,
        /* [in] */ LCID lcid,
        /* [in] */ WORD wFlags,
        /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
        /* [out] */ VARIANT __RPC_FAR *pVarResult,
        /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
        /* [out] */ UINT __RPC_FAR *puArgErr);
};


//OM_SMARTPTR_TYPEDEF(IDirectDraw4, IID_IDirectDraw4);

class CWndFlashImpl :
	public COleContainerWnd<ShockwaveFlashObjects::IShockwaveFlash>,
	public ShockwaveFlashObjects::_IShockwaveFlashEvents,
	public IServiceProvider
{
public:
	CWndFlashImpl();
	virtual ~CWndFlashImpl();

public:
	bool SetMovie(LPCTSTR pszSwfFilePath);
	inline void SetCaptionArea(WORD _x, WORD _y, WORD _width, WORD _height);
	inline LPCTSTR CallFunction(LPCTSTR _request);

public:
	virtual void OnErrorClassNotReg();
	virtual	BOOL OnBeforeShowingContent();
	virtual	BOOL OnAfterShowingContent();

public:
	//IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	//IDispatch
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( 
        /* [out] */ UINT __RPC_FAR *pctinfo);
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( 
        /* [in] */ UINT iTInfo,
        /* [in] */ LCID lcid,
        /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
    virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( 
        /* [in] */ REFIID riid,
        /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
        /* [in] */ UINT cNames,
        /* [in] */ LCID lcid,
        /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke( 
        /* [in] */ DISPID dispIdMember,
        /* [in] */ REFIID riid,
        /* [in] */ LCID lcid,
        /* [in] */ WORD wFlags,
        /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
        /* [out] */ VARIANT __RPC_FAR *pVarResult,
        /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
        /* [out] */ UINT __RPC_FAR *puArgErr);

	//DShockwaveFlashEvents
    HRESULT STDMETHODCALLTYPE OnReadyStateChange (
        long newState );
    HRESULT STDMETHODCALLTYPE OnProgress (
        long percentDone );
    HRESULT STDMETHODCALLTYPE FSCommand (
        _bstr_t command,
        _bstr_t args );

	//iServiceProvider
	HRESULT STDMETHODCALLTYPE QueryService(REFGUID guidService, REFIID riid, 
	void **ppvObject)
	{
	return raw_RemoteQueryService(guidService, riid, (IUnknown**)ppvObject);
	}

/*   HRESULT RemoteQueryService (
       GUID * guidService,
       GUID * riid,
       IUnknown * * ppvObject );*/
    HRESULT __stdcall raw_RemoteQueryService (REFGUID guidService, REFIID riid,
IUnknown ** ppvObject );

public:
	long m_lVersion;
	IDirectDraw4Ptr *m_lpDD4;
	LPTSTR m_pszSwfFilePath;
	w2x::events::CEventDispatcher m_event_disp;
};


inline void CWndFlashImpl::SetCaptionArea(
	WORD _x, WORD _y, WORD _width, WORD _height
	)
{
	m_caption_rect.left = _x;
	m_caption_rect.top = _y;
	m_caption_rect.right = _x + _width;
	m_caption_rect.bottom = _y + _height;
}


inline LPCTSTR CWndFlashImpl::CallFunction(LPCTSTR _request)
{ 
	return m_lpControl->CallFunction(_request); 
}


#endif // !defined(AFX_FLASHWND_H__D74EA89B_5D79_4D87_8BB9_4DB90A5FE8FC__INCLUDED_)

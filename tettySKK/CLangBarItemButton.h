#pragma once
#include <ctfutb.h>
#include <atlbase.h>
#include <Windows.h>
#include <msctf.h>


#define SKKIME_CLANGBAR_SINK_COOKIE 0x50ab8cd0
class CSkkIme;

class CLangBarItemButton :
    public ITfLangBarItemButton,
    public ITfSource
{
public:
	CLangBarItemButton(CSkkIme *cskkime, REFGUID guid);
	~CLangBarItemButton();

	//IUnknown 	
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) override;
	STDMETHODIMP_(ULONG) AddRef() override;
	STDMETHODIMP_(ULONG) Release() override;
	// ITfLangBarItem
	STDMETHODIMP GetInfo(TF_LANGBARITEMINFO* pInfo) override;
	STDMETHODIMP GetStatus(DWORD* pdwStatus) override;
	STDMETHODIMP Show(BOOL fShow) override;
	STDMETHODIMP GetTooltipString(BSTR* pbstrToolTip) override;
	// ITfLangBarItemButton
	STDMETHODIMP OnClick(TfLBIClick click, POINT pt, const RECT* prcArea) override;
	STDMETHODIMP InitMenu(ITfMenu* pMenu) override;
	STDMETHODIMP OnMenuSelect(UINT wID) override;
	STDMETHODIMP GetIcon(HICON* phIcon) override;
	STDMETHODIMP GetText(BSTR* pbstrText) override;
	// ITfSource
	STDMETHODIMP AdviseSink(REFIID riid, IUnknown* punk, DWORD* pdwCookie) override;
	STDMETHODIMP UnadviseSink(DWORD dwCookie) override;

	HRESULT _UpdateIcon();
	HRESULT _Init();
	HRESULT _Uninit();

private:
	LONG _refCount;
	//GUID _guid;
	CComPtr<ITfLangBarItemSink> _pLangBarItemSink;//通知用シンク
	CSkkIme *_pSkkIme;

	TF_LANGBARITEMINFO _LangBarItemInfo;
	//DWORD _dwCookie;//登録時のクッキー
};


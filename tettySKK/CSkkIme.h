#pragma once
#include "pch.h"
#include <msctf.h>
#include <strsafe.h>
#include <atlbase.h>

//ref https://github.com/nathancorvussolis/corvusskk/blob/2904b3ad7ba80e66e717aef6805164c74fcec71d/imcrvtip/TextService.h
//ref https://github.com/nathancorvussolis/corvusskk/blob/2904b3ad7ba80e66e717aef6805164c74fcec71d/imcrvtip/TextService.cpp
class CSkkIme :
	public ITfTextInputProcessor,
	public ITfKeyEventSink,
	public ITfCompositionSink,
	public ITfDisplayAttributeProvider
{
public:
	CSkkIme();
	~CSkkIme();

	//IUnknown methods
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	//ITfTextInputProcessor methods
	STDMETHODIMP Activate(ITfThreadMgr* ptim, TfClientId tid);
	STDMETHODIMP Deactivate();

	//ITfKeyEventSink methods
	STDMETHODIMP OnSetFocus(BOOL fForeground);
	STDMETHODIMP OnTestKeyDown(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten);
	STDMETHODIMP OnTestKeyUp(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten);
	STDMETHODIMP OnKeyDown(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten);
	STDMETHODIMP OnKeyUp(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten);
	STDMETHODIMP OnPreservedKey(ITfContext* pic, REFGUID rguid, BOOL* pfEaten);

	//ITfCompositionSink methods
	STDMETHODIMP OnCompositionTerminated(TfEditCookie ecWrite, ITfComposition* pComposition);

	//ITfDisplayAttributeProvider methods
	STDMETHODIMP EnumDisplayAttributeInfo(IEnumTfDisplayAttributeInfo** ppEnum);
	STDMETHODIMP GetDisplayAttributeInfo(REFGUID guid, ITfDisplayAttributeInfo** ppInfo);

	//テキスト挿入補助
	HRESULT _DoInsertText(TfEditCookie ec, ITfContext* pContext, const WCHAR* text);
private:
	LONG _refCount;
	//TSFとの連絡用
	CComPtr<ITfThreadMgr> _pThreadMgr;
	TfClientId _clientId;

	//未確定文字列管理用
	CComPtr<ITfComposition> _pComposition;
	void _InsertText(ITfContext* pic, const WCHAR* text);

	BOOL _InitKeyEventSink();
	void _UninitKeyEventSink();


};



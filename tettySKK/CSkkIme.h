#pragma once
#include "pch.h"
#include <msctf.h>
#include <strsafe.h>
#include <atlbase.h>

//ref https://github.com/nathancorvussolis/corvusskk/blob/2904b3ad7ba80e66e717aef6805164c74fcec71d/imcrvtip/TextService.h
//ref https://github.com/nathancorvussolis/corvusskk/blob/2904b3ad7ba80e66e717aef6805164c74fcec71d/imcrvtip/TextService.cpp
class CSkkIme :
	public ITfTextInputProcessor,
	public ITfKeyEventSink
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


private:
	LONG _refCount;
	//TSFÇ∆ÇÃòAóçóp
	CComPtr<ITfThreadMgr> _pThreadMgr;
	TfClientId _clientId;

	BOOL _InitKeyEventSink();
	void _UninitKeyEventSink();
};


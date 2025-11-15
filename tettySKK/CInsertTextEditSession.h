#pragma once
#include <msctf.h>
#include "CSkkIme.h"
#include <string>

//ref https://github.com/nathancorvussolis/corvusskk/blob/2904b3ad7ba80e66e717aef6805164c74fcec71d/imcrvtip/EditSession.h#L6
//テキスト挿入のためのEditSession
class CInsertTextEditSession :
    public ITfEditSession
{
public:
	CInsertTextEditSession(CSkkIme* pIme, ITfContext* pContext, const WCHAR* text, BOOL isDetermined);
	~CInsertTextEditSession();
	//IUnknown methods
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	//ITfEditSession methods
	STDMETHODIMP DoEditSession(TfEditCookie ec);
private:	
	LONG _refCount;
	CComPtr<CSkkIme> _pIme;
	CComPtr<ITfContext> _pContext;
	WCHAR* _text;

	//確定したか否か?
	BOOL _isDetermined;
};

//Compositionのテキストを取得するためのEditSession
class CGetTextEditSession :public ITfEditSession
{
public:
	CGetTextEditSession(ITfRange* pRange, std::wstring* pOutput);
	~CGetTextEditSession();
	//IUnknown methods
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	//ITfEditSession methods
	STDMETHODIMP DoEditSession(TfEditCookie ec);
private:
	LONG _refCount;
	CComPtr<ITfRange> _pRange;
	std::wstring* _pOutput;
};
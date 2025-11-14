#pragma once
#include <msctf.h>
#include "CSkkIme.h"

//ref https://github.com/nathancorvussolis/corvusskk/blob/2904b3ad7ba80e66e717aef6805164c74fcec71d/imcrvtip/EditSession.h#L6
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

	//Šm’è‚µ‚½‚©”Û‚©?
	BOOL _isDetermined;
};


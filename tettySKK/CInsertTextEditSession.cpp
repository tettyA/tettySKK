#include "pch.h"
#include "CInsertTextEditSession.h"

CInsertTextEditSession::CInsertTextEditSession(CSkkIme* pIme, ITfContext* pContext, const WCHAR* text)
{
	_refCount = 1;
	_pIme = pIme;//–¾Ž¦“I‚ÈAddRef‚Í•s—v (CComPtr‚ª‚â‚Á‚Ä‚­‚ê‚é)
	_pContext = pContext;
	size_t len = wcslen(text);
	_text = new WCHAR[len + 1];
	wcscpy_s(_text, len + 1, text);	
}

CInsertTextEditSession::~CInsertTextEditSession()
{
	if (_text)
	{
		delete[] _text;
		_text = nullptr;
	}
	_pContext.Release();
	_pIme.Release();
}

STDMETHODIMP CInsertTextEditSession::QueryInterface(REFIID riid, void** ppvObj)
{ 
	if (ppvObj == nullptr)
	{
		return E_INVALIDARG;
	}

	ppvObj = nullptr;
	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ITfEditSession)) {
		*ppvObj = static_cast<ITfEditSession*>(this);

	}
	
	if (*ppvObj) {
		AddRef();
		return S_OK;
	}
	
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CInsertTextEditSession::AddRef() {
	return ++_refCount;
}

STDMETHODIMP_(ULONG) CInsertTextEditSession::Release() {
	if (--_refCount == 0) {
		delete this;
		return 0;
	}
	return _refCount;
}

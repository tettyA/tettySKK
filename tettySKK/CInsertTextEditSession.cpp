#include "pch.h"
#include "CInsertTextEditSession.h"

CInsertTextEditSession::CInsertTextEditSession(CSkkIme* pIme, ITfContext* pContext, const WCHAR* text, BOOL isDetermined)
{
	_refCount = 1;
	_pIme = pIme;//–¾Ž¦“I‚ÈAddRef‚Í•s—v (CComPtr‚ª‚â‚Á‚Ä‚­‚ê‚é)
	_pContext = pContext;
	_isDetermined = isDetermined;

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



CGetTextEditSession::CGetTextEditSession(ITfRange* pRange, std::wstring* pOutput)
{
	_refCount = 1;
	_pRange = pRange;
	_pOutput = pOutput;


}
CGetTextEditSession::~CGetTextEditSession()
{
	_pRange.Release();
	_pOutput = nullptr;
}

STDMETHODIMP CGetTextEditSession::QueryInterface(REFIID riid, void** ppvObj)
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


STDMETHODIMP_(ULONG) CGetTextEditSession::AddRef()
{
	return ++_refCount;
}
STDMETHODIMP_(ULONG) CGetTextEditSession::Release()
{
	if (--_refCount == 0) {
		delete this;
		return 0;
	}
	return _refCount;
}

STDMETHODIMP_(HRESULT __stdcall) CGetTextEditSession::DoEditSession(TfEditCookie ec)
{
	WCHAR buffer[256];
	ULONG cchFetched = 0;

	if (SUCCEEDED(_pRange->GetText(ec, 0, buffer, 256, &cchFetched)))
	{
		if (cchFetched > 0) {
			_pOutput->assign(buffer, cchFetched);
		}
	}

	return S_OK;
}


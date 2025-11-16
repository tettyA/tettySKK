#include "pch.h"
#include "CEditSessionBase.h"

CEditSessionBase::CEditSessionBase()
{
	_refCount = 1;
}
CEditSessionBase::~CEditSessionBase()
{
}
STDMETHODIMP CEditSessionBase::QueryInterface(REFIID riid, void** ppvObj)
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

STDMETHODIMP_(ULONG) CEditSessionBase::AddRef() {
	return ++_refCount;
}

STDMETHODIMP_(ULONG) CEditSessionBase::Release() {
	if (--_refCount == 0) {
		delete this;
		return 0;
	}
	return _refCount;
}


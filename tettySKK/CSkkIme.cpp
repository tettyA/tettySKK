#include "pch.h"
#include "CSkkIme.h"

#include "Global.h"

CSkkIme::CSkkIme()
{
	DllAddRef();

	_refCount = 1;
	_pThreadMgr = nullptr;
	_clientId = TF_CLIENTID_NULL;
	_pComposition = nullptr;
}
CSkkIme::~CSkkIme()
{
	if(_pThreadMgr)
	{
		_pThreadMgr.Release();
		_pThreadMgr = nullptr;
	}
	DllRelease();
}

STDAPI CSkkIme::QueryInterface(REFIID riid, void** ppvObj)
{
	if (ppvObj == nullptr)
	{
		return E_INVALIDARG;
	}
	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ITfTextInputProcessor))
	{
		*ppvObj = static_cast<ITfTextInputProcessor*>(this);
	}
	else if (IsEqualIID(riid, IID_ITfKeyEventSink))
	{
		*ppvObj = static_cast<ITfKeyEventSink*>(this);
	}
	else if (IsEqualGUID(riid, IID_ITfCompositionSink))
	{
		*ppvObj = static_cast<ITfCompositionSink*>(this);
	}
	else
	{
		*ppvObj = nullptr;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}
STDAPI_(ULONG) CSkkIme::AddRef()
{
	return InterlockedIncrement(&_refCount);
}
STDAPI_(ULONG) CSkkIme::Release()
{
	LONG refCount = InterlockedDecrement(&_refCount);
	if (refCount == 0)
	{
		delete this;
		return 0;
	}
	return refCount;
}

STDAPI CSkkIme::Activate(ITfThreadMgr* ptim, TfClientId tid) {
	if (ptim == nullptr) return E_INVALIDARG;

	_pThreadMgr = ptim; // CComPtrが自動的にAddRefするので明示的なAddRefは不要
	_clientId = tid;

#ifdef _DEBUG
	OutputDebugString(L"CSkkIme::Activate Called!");
#endif

	if (!_InitKeyEventSink())
	{
		return E_FAIL;
	}
	return S_OK;
}

STDAPI CSkkIme::Deactivate() {
	if (_pThreadMgr) {
		_pThreadMgr.Release();
		_pThreadMgr = nullptr;
	}
	_clientId = TF_CLIENTID_NULL;
#ifdef _DEBUG
	OutputDebugString(L"CSkkIme::Deactivate Called!");
#endif

	_UninitKeyEventSink();
	return S_OK;
}

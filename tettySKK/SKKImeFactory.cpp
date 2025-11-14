#include "pch.h"
#include "SKKImeFactory.h"
#include "CSkkIme.h"
#include <strsafe.h>
#include <atlbase.h>

#include "Global.h"



static LONG g_dllRefCount=0;

STDAPI CSkkImeFactory::QueryInterface(REFIID riid, void** ppvObject) {
	if (ppvObject == NULL) {
		return E_INVALIDARG;
	}

	*ppvObject = NULL;

	if (IsEqualIID(riid,IID_IUnknown) ||IsEqualIID(riid,IID_IClassFactory )) {
		*ppvObject = static_cast<IClassFactory*>(this);
		DllAddRef();
	}
	else {
		return E_NOINTERFACE;
	}

	return S_OK;
}

STDAPI_(ULONG) CSkkImeFactory::AddRef() {
	return DllAddRef();
}
STDAPI_(ULONG) CSkkImeFactory::Release() {
	return DllRelease();
}

STDAPI CSkkImeFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) {
	if (ppvObject == NULL) {
		return E_INVALIDARG;
	}
	*ppvObject = NULL;
	//コンポーネントの集約はサポートしない
	if (pUnkOuter != NULL) {
		return CLASS_E_NOAGGREGATION;
	}
	//IMEオブジェクトの生成
	CSkkIme* pSkkIme = new CSkkIme();
	if (pSkkIme == NULL) {
		return E_OUTOFMEMORY;
	}
	//要求されたインターフェースを取得
	HRESULT hr = pSkkIme->QueryInterface(riid, ppvObject);
	pSkkIme->Release();//CreateInstanceで1参照，QueryInterfaceで1参照増えるので，Releaseしておく。
	return hr;
}

STDAPI CSkkImeFactory::LockServer(BOOL fLock) {
	if (fLock) {
		DllAddRef();
	}
	else {
		DllRelease();
	}
	return S_OK;
}

LONG DllAddRef(void) {
	return InterlockedIncrement(&g_dllRefCount);
}
LONG DllRelease(void) {
	return InterlockedDecrement(&g_dllRefCount);
}

_Check_return_
STDAPI DllGetClassObject(
    _In_ REFCLSID rclsid,
    _In_ REFIID riid,
    _Outptr_ LPVOID FAR* ppv
) {
    if (ppv == NULL) {
        return E_INVALIDARG;
    }
    *ppv = NULL;
    if (!IsEqualIID(rclsid, CLSID_tettySKK)) {
        return CLASS_E_CLASSNOTAVAILABLE;
    }
    CSkkImeFactory* pFactory = new CSkkImeFactory();
    if (pFactory == NULL) {
        return E_OUTOFMEMORY;
    }
    
    HRESULT hr = pFactory->QueryInterface(riid, ppv);
    pFactory->Release();//QueryInterfaceで1参照増えるので，Releaseしておく。
    return hr;
}
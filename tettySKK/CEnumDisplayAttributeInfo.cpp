#include "pch.h"
#include "Global.h"
#include "CDisplayAttributeInfo.h"
#include "CEnumDisplayAttributeInfo.h"

CEnumDisplayAttributeInfo::CEnumDisplayAttributeInfo() {
	DllAddRef();

	_refCount = 1;
	_nIndex = 0;
}

CEnumDisplayAttributeInfo::~CEnumDisplayAttributeInfo() {
	DllRelease();
}

STDMETHODIMP CEnumDisplayAttributeInfo::QueryInterface(REFIID riid, void** ppvObj) {
	if (ppvObj == nullptr)return E_INVALIDARG;

	ppvObj = nullptr;
	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IEnumTfDisplayAttributeInfo)) {
		*ppvObj = static_cast<IEnumTfDisplayAttributeInfo*>(this);
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CEnumDisplayAttributeInfo::AddRef(void) {
	return InterlockedIncrement(&_refCount);
}

STDMETHODIMP_(ULONG) CEnumDisplayAttributeInfo::Release(void) {
	LONG refC = InterlockedDecrement(&_refCount);
	if (refC == 0)delete this;
	return refC;
}

STDMETHODIMP CEnumDisplayAttributeInfo::Clone(IEnumTfDisplayAttributeInfo** ppEnum) {
	if (ppEnum == nullptr)return E_INVALIDARG;
	*ppEnum = nullptr;

	CEnumDisplayAttributeInfo* pClone = nullptr;

	try {
		pClone = new CEnumDisplayAttributeInfo();
	}
	catch (...) {
		return E_OUTOFMEMORY;
	}

	pClone->_nIndex = _nIndex;

	*ppEnum = pClone;

	return S_OK;
}

STDMETHODIMP CEnumDisplayAttributeInfo::Next(ULONG ulCount, ITfDisplayAttributeInfo** rgInfo, ULONG* pcFetched) {
	ULONG cFetched = 0;
	ITfDisplayAttributeInfo* pInfo = nullptr;

	if (rgInfo == nullptr)return E_INVALIDARG;
	if (ulCount == 0)return S_OK;

	for (ULONG i = 0; i < ulCount; i++) {
		*(rgInfo + i) = nullptr;
	}

	if (pcFetched != nullptr) {
		*pcFetched = 0;
	}

	while (cFetched < ulCount) {
		if (_nIndex >= ENUM_DISPLAYATTRIBUTE_COUNT)break;

		try {
			pInfo = new CDisplayAttributeInfo();

		}
		catch (...) {
			for (ULONG j = 0; j < cFetched; j++)
			{
				ITfDisplayAttributeInfo* pReleaseInfo = *(rgInfo + j);
				if (pReleaseInfo != nullptr) {
					pReleaseInfo->Release();
					*(rgInfo + j) = nullptr;
				}
			}
			return E_OUTOFMEMORY;
		}

		*(rgInfo + cFetched) = pInfo;
		cFetched++;
		_nIndex++;
	}

	if (pcFetched != nullptr) {
		*pcFetched = cFetched;
	}
	return (cFetched == ulCount) ? S_OK : S_FALSE;
}

STDMETHODIMP CEnumDisplayAttributeInfo::Reset(void) {
	_nIndex = 0;
	return S_OK;
}
STDMETHODIMP CEnumDisplayAttributeInfo::Skip(ULONG ulCount) {
	_nIndex += ulCount;
	if (_nIndex >= ENUM_DISPLAYATTRIBUTE_COUNT) {
		_nIndex = ENUM_DISPLAYATTRIBUTE_COUNT;
		return S_FALSE;
	}
	return S_OK;
}
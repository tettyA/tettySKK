#include "pch.h"
#include "CDisplayAttributeInfo.h"
#include "CEnumDisplayAttributeInfo.h"
#include "Global.h"
#include "CSkkIme.h"
#include <atlbase.h>
#include <Windows.h>

//ref https://github.com/nathancorvussolis/corvusskk/blob/2904b3ad7ba80e66e717aef6805164c74fcec71d/imcrvtip/DisplayAttributeProvider.cpp

STDMETHODIMP CSkkIme::EnumDisplayAttributeInfo(IEnumTfDisplayAttributeInfo** ppEnum)
{
	IEnumTfDisplayAttributeInfo* pEnumDisplayAttributeInfo = nullptr;

	if (ppEnum == nullptr)return E_INVALIDARG;

	*ppEnum = nullptr;

	try {
		pEnumDisplayAttributeInfo = new CEnumDisplayAttributeInfo();
	}
	catch (...) {
		return E_OUTOFMEMORY;
	}

	*ppEnum = pEnumDisplayAttributeInfo;

	return S_OK;
}

STDMETHODIMP CSkkIme::GetDisplayAttributeInfo(REFGUID guid, ITfDisplayAttributeInfo** ppInfo)
{
	ITfDisplayAttributeInfo* pDisplayAttributeInfo = nullptr;
	if (ppInfo == nullptr)return E_INVALIDARG;

	*ppInfo = nullptr;

	if (IsEqualIID(guid, GUID_Skk_DisplayAttirbute_Input)) {
		*ppInfo = new CDisplayAttributeInfo();
		if (*ppInfo == nullptr)return E_OUTOFMEMORY;
		return S_OK;
	}

	return E_INVALIDARG;
}
#include "pch.h"
#include "CDisplayAttributeInfo.h"
#include "Global.h"
#include <Windows.h>

CDisplayAttributeInfo::CDisplayAttributeInfo() {
	_refCount = 1;
}
CDisplayAttributeInfo::~CDisplayAttributeInfo(){}

STDMETHODIMP CDisplayAttributeInfo::QueryInterface(REFIID riid, void** ppvObj) {
	if (ppvObj == nullptr)return E_INVALIDARG;

	ppvObj = nullptr;

	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ITfDisplayAttributeInfo)) {
		*ppvObj = static_cast<ITfDisplayAttributeInfo*>(this);
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CDisplayAttributeInfo::AddRef(void) {
	return InterlockedIncrement(&_refCount);
}
STDMETHODIMP_(ULONG) CDisplayAttributeInfo::Release(void) {
	LONG refC=InterlockedDecrement(&_refCount);
	if (refC == 0)delete this;
	return refC;
}

STDMETHODIMP CDisplayAttributeInfo::GetGUID(GUID* pguid) {
	if (pguid==nullptr)return E_INVALIDARG;
	*pguid = GUID_Skk_DisplayAttirbute_Input;
	return S_OK;
}

STDMETHODIMP CDisplayAttributeInfo::GetDescription(BSTR* bstrDesc) {
	if (bstrDesc == nullptr)return E_INVALIDARG;
	*bstrDesc = SysAllocString(L"Skk Input Text");
	return (*bstrDesc) == nullptr ? E_OUTOFMEMORY : S_OK;
}

STDMETHODIMP CDisplayAttributeInfo::GetAttributeInfo(TF_DISPLAYATTRIBUTE* pda) {
	if (pda == nullptr)return E_INVALIDARG;
	//文字色
	pda->crText.type = TF_CT_SYSCOLOR;
	pda->crText.nIndex = COLOR_WINDOWTEXT;

	//背景色
	pda->crBk.type = TF_CT_NONE;
	pda->crBk.nIndex = 0;

	//下線
	pda->lsStyle = TF_LS_SOLID;

	//太字下線
	pda->fBoldLine = FALSE;

	//下線の色
	pda->crLine.type = TF_CT_SYSCOLOR;
	pda->crLine.nIndex = COLOR_WINDOWTEXT;

	//属性情報
	pda->bAttr = TF_ATTR_INPUT;
	return S_OK;
}

STDMETHODIMP CDisplayAttributeInfo::SetAttributeInfo(const TF_DISPLAYATTRIBUTE* pda) {
	//設定不可
	return E_NOTIMPL;
}
STDMETHODIMP CDisplayAttributeInfo::Reset() {
	//リセット不要
//	return S_OK;
	return E_NOTIMPL;
}


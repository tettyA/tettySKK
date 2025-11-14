#include "pch.h"
#include "CInsertTextEditSession.h"
#include "Global.h"

STDMETHODIMP CInsertTextEditSession::DoEditSession(TfEditCookie ec) {
	
	return _pIme->_DoInsertText(ec, _pContext, _text);
}

void CSkkIme::_InsertText(ITfContext* pic, const WCHAR* text) {
	CInsertTextEditSession* pSession = new CInsertTextEditSession(this, pic, text);

	HRESULT hr;
	pic->RequestEditSession(_clientId, pSession, TF_ES_SYNC | TF_ES_READWRITE, &hr);
	pSession->Release();
}

//ref https://github.com/nathancorvussolis/corvusskk/blob/2904b3ad7ba80e66e717aef6805164c74fcec71d/imcrvtip/Composition.cpp#L78
HRESULT CSkkIme::_DoInsertText(TfEditCookie ec, ITfContext* pContext, const WCHAR* text) {
	HRESULT hr=E_FAIL;
	
	if (_pComposition) {
		CComPtr<ITfRange> pRange;
		if (SUCCEEDED(_pComposition->GetRange(&pRange))) {
			pRange->SetText(ec, 0, text, (ULONG)wcslen(text));
		}
		return S_OK;
	}

	CComPtr<ITfInsertAtSelection> pInsertAtSelection;
	
	if (FAILED(pContext->QueryInterface(IID_PPV_ARGS(&pInsertAtSelection))) || (pInsertAtSelection == nullptr))
	{
		return hr;
	}

	CComPtr<ITfRange> pRange;
	if (FAILED(pInsertAtSelection->InsertTextAtSelection(
		ec,
		0,
		text,
		(ULONG)wcslen(text),
		&pRange
	))|| (pRange==nullptr)) {
		return hr;
	}

	//属性プロパティ設定
	CComPtr<ITfProperty> pProperty;
	if (FAILED(pContext->GetProperty(GUID_PROP_ATTRIBUTE, &pProperty)) || (pProperty == nullptr)) {
		return hr;
	}

	{
		TfGuidAtom gatom = TF_INVALID_GUIDATOM;
		CComPtr<ITfCategoryMgr> pCategoryMgr;
		if (SUCCEEDED(pCategoryMgr.CoCreateInstance(CLSID_TF_CategoryMgr))) {
			pCategoryMgr->RegisterGUID(GUID_Skk_DisplayAttirbute_Input, &gatom);
		}
		if (gatom != TF_INVALID_GUIDATOM) {
			VARIANT var;
			var.vt = VT_I4;
			var.lVal = (LONG)gatom;

			pProperty->SetValue(ec, pRange, &var);
		}
	}

	CComPtr<ITfContextComposition> pContextComposition;
	if (FAILED(pContext->QueryInterface(IID_PPV_ARGS(&pContextComposition))) || (pContextComposition == nullptr)) {
		return hr;
	}
	
	CComPtr<ITfComposition> pComposition;
	if (FAILED(pContextComposition->StartComposition(
		ec,
		pRange,
		this,
		&pComposition
	)) || pComposition==nullptr) {
		return hr;
	}

	pRange->Collapse(ec, TF_ANCHOR_END);

	TF_SELECTION sel = { 0 };
	sel.range = pRange;
	sel.style.ase = TF_AE_NONE;
	sel.style.fInterimChar = FALSE;

	pContext->SetSelection(ec, 1, &sel);
	return S_OK;
}

STDMETHODIMP CSkkIme::OnCompositionTerminated(TfEditCookie ecWrite, ITfComposition* pComposition) {
	if (pComposition && this->_pComposition == pComposition) {
		_pComposition.Release();
		_pComposition = nullptr;
	}
	return S_OK;
}

#include "pch.h"
#include "CInsertTextEditSession.h"

STDMETHODIMP CInsertTextEditSession::DoEditSession(TfEditCookie ec) {
	
	return _pIme->_DoInsertText(ec, _pContext, _text);
}

void CSkkIme::_InsertText(ITfContext* pic, const WCHAR* text) {
	CInsertTextEditSession* pSession = new CInsertTextEditSession(this, pic, text);

	HRESULT hr;
	pic->RequestEditSession(_clientId, pSession, TF_ES_SYNC | TF_ES_READWRITE, &hr);
	if (FAILED(hr)) {
		MessageBox(NULL, L"RequestEditSession failed", L"Error", MB_OK);
	}
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
		MessageBox(NULL, L"failed Make pInsertAtSelection",L"Error",MB_OK);
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
		MessageBox(NULL, L"failed Make pRange", L"Error", MB_OK);
		return hr;
	}

	CComPtr<ITfContextComposition> pContextComposition;
	if (FAILED(pContext->QueryInterface(IID_PPV_ARGS(&pContextComposition))) || (pContextComposition == nullptr)) {
		MessageBox(NULL, L"Failed Make pContextComposition", L"Error", MB_OK);
		return hr;
	}
	
	CComPtr<ITfComposition> pComposition;
	if (FAILED(pContextComposition->StartComposition(
		ec,
		pRange,
		this,
		&pComposition
	)) || pComposition==nullptr) {
		MessageBox(NULL, L"Failed Start Composition", L"Error", MB_OK);
		return hr;
	}

	return S_OK;
	/*
	if (_pComposition == nullptr) {
		ITfContextComposition* pContextComposition;
		if (SUCCEEDED(pContext->QueryInterface(IID_ITfContextComposition, (void**)&pContextComposition))) {
			HRESULT hr = pContextComposition->StartComposition(
				ec,
				NULL,
				(ITfCompositionSink*)(this),
				&_pComposition
			);
			if (FAILED(hr)) {
				MessageBox(NULL, L"StartComposition failed", L"Error", MB_OK);
			}
			pContextComposition->Release();
		}
	}

	if (_pComposition) {
		ITfRange* pRange;
		if (SUCCEEDED(_pComposition->GetRange(&pRange))) {
			//•¶Žš‚ðƒZƒbƒg

			HRESULT hr = pRange->SetText(ec, 0, text, (ULONG)(wcslen(text)));
			if (FAILED(hr)) {
				MessageBox(NULL, L"SetText failed", L"Error", MB_OK);
			}

			pRange->Release();
		}
		else {
			MessageBox(NULL, L"GetRange failed", L"Error", MB_OK);
		}
	}*/
}

STDMETHODIMP CSkkIme::OnCompositionTerminated(TfEditCookie ecWrite, ITfComposition* pComposition) {
	if (pComposition) {
		_pComposition.Release();
		_pComposition = nullptr;
	}
	return S_OK;
}

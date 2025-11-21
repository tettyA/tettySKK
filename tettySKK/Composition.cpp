#include "pch.h"

#include "CInsertTextEditSession.h"
#include "Global.h"

//テキスト編集の予約
void CSkkIme::_InsertText(ITfContext* pic, const WCHAR* text,BOOL isDetermined) {
	CInsertTextEditSession* pSession/*;
	if (m_currentMode == SKKMode::Katakana) {
		std::wstring strText = text;
		_ConvertToKatakana(strText);

		pSession = new CInsertTextEditSession(this, pic, text, isDetermined);
	}
	else
	pSession */= new CInsertTextEditSession(this, pic, text, isDetermined);

	HRESULT hr;
	pic->RequestEditSession(_clientId, pSession, TF_ES_SYNC | TF_ES_READWRITE, &hr);
	pSession->Release();
}

HRESULT CSkkIme::_SetInputDisplayAttributeInfo(ITfContext* pContext, TfEditCookie ec, ITfRange* pRange) {
	HRESULT hr = E_FAIL;
	//属性プロパティ設定
	CComPtr<ITfProperty> pProperty;
	if (FAILED(pContext->GetProperty(GUID_PROP_ATTRIBUTE, &pProperty)) || (pProperty == nullptr)) {
		return hr;
	}


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

	return S_OK;
}

//実際にテキストの編集を行う
//ref https://github.com/nathancorvussolis/corvusskk/blob/2904b3ad7ba80e66e717aef6805164c74fcec71d/imcrvtip/Composition.cpp#L78
HRESULT CSkkIme::_DoInsertText(TfEditCookie ec, ITfContext* pContext, const WCHAR* text,BOOL isDetermined) {
	HRESULT hr=E_FAIL;
	
	if (_pComposition) {
		CComPtr<ITfRange> pRange;
		if (SUCCEEDED(_pComposition->GetRange(&pRange))) {
			pRange->SetText(ec, 0, text, (ULONG)wcslen(text));

			_SetInputDisplayAttributeInfo(pContext, ec, pRange);

			//カーソルを末尾に移動
			pRange->Collapse(ec, TF_ANCHOR_END);
			TF_SELECTION sel = { 0 };
			sel.range = pRange;
			sel.style.ase = TF_AE_NONE;
			pContext->SetSelection(ec, 1, &sel);
		}

	}
	else {
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
		)) || (pRange == nullptr)) {
			return hr;
		}

		if (FAILED(_SetInputDisplayAttributeInfo(pContext, ec, pRange))) {
			return hr;
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
		)) || pComposition == nullptr) {
			return hr;
		}


		

		_pComposition = pComposition;


		pRange->Collapse(ec, TF_ANCHOR_END);

		TF_SELECTION sel = { 0 };
		sel.range = pRange;
		sel.style.ase = TF_AE_NONE;
		sel.style.fInterimChar = FALSE;

		pContext->SetSelection(ec, 1, &sel);
	}

	if (isDetermined && _pComposition) {
		_pComposition->EndComposition(ec);

		_pComposition.Release();
		_pComposition = nullptr;
	}
	return S_OK;
}

STDMETHODIMP CSkkIme::OnCompositionTerminated(TfEditCookie ecWrite, ITfComposition* pComposition) {
	if (pComposition && this->_pComposition == pComposition) {
		_pComposition.Release();
		_pComposition = nullptr;
	}
	return S_OK;
}

BOOL CSkkIme::_GetCompositionString(std::wstring& compositionString)
{
	compositionString.clear();

	CComPtr<ITfRange> pRange;
	if (_pComposition == nullptr) {
		return FALSE;
	}
	if (FAILED(_pComposition->GetRange(&pRange)) || (pRange == nullptr)) {
		return FALSE;
	}


	CGetTextEditSession* pSession = new CGetTextEditSession(pRange, &compositionString);

	HRESULT hr;
	ITfContext* pContext ;
	if (SUCCEEDED(pRange->GetContext(&pContext)))
	{
		pContext->RequestEditSession(_clientId, pSession, TF_ES_SYNC | TF_ES_READ, &hr);
		pContext->Release();

	}
	pSession->Release();

	return !compositionString.empty();
}

void CSkkIme::_CommitComposition(ITfContext* pic)
{
	if (_pComposition) {
		CTerminateCompositionEditSession* pSession = new CTerminateCompositionEditSession(_pComposition);
		HRESULT hr;
		pic->RequestEditSession(_clientId, pSession, TF_ES_SYNC | TF_ES_READWRITE, &hr);
		pSession->Release();


		_pComposition.Release();
		_pComposition = nullptr;

	}
	if(m_pCandidateWindow->IsWindowExists()){
		m_pCandidateWindow->HideWindow();
	}
	
	m_CurrentCandidates.clear();
	m_CurrentShowCandidateIndex = 0;
	m_RomajiToKanaTranslator.Reset();
	m_currentMode = SKKMode::Kakutei;

	m_Gokan = L"";
	m_OkuriganaFirstChar = L'\0';

	return;
}

void CSkkIme::_UpDateCandidateWindowPosition(ITfContext* pic)
{
	if (!_pComposition || m_pCandidateWindow == nullptr || !m_pCandidateWindow->IsWindowExists()) {
		return;
	}

	CComPtr<ITfRange> pRange;
	if (FAILED(_pComposition->GetRange(&pRange)) || (pRange == nullptr)) {
		return;
	}

	RECT rc = { 0 };
	CGetCandidateWindowPosEditSession* pSession = new CGetCandidateWindowPosEditSession(pic, pRange, &rc);

	HRESULT hr;
	pic->RequestEditSession(_clientId, pSession, TF_ES_SYNC | TF_ES_READ, &hr);
	pSession->Release();

	if (SUCCEEDED(hr)) {
		m_pCandidateWindow->ShowWindow(rc.left, rc.bottom);
	}
}
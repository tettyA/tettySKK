#include "pch.h"
#include "CSkkIme.h"

#include "Global.h"

bool CSkkIme::_IsKeyEaten(WPARAM wParam) {
	WCHAR key = (WCHAR)wParam;
	if ((key >= L'A' && key <= L'Z') || key == VK_SPACE || key == VK_RETURN)
	{
		return true;
	}
	return false;
}

//キーが押された瞬間
STDAPI CSkkIme::OnKeyDown(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten) {
	*pfEaten = FALSE;

	//FIX: ひらがな以外も対応
	WCHAR key = (WCHAR)wParam;

	//変換の処理
	if (key == VK_SPACE) {
		//未入力ならば，そのまま返す
		if (!_pComposition)
		{
			return S_OK;
		}

		*pfEaten = TRUE;
	
		if (m_CurrentCandidates.empty())
		{
			std::wstring compositionString;

			
			if (_GetCompositionString(compositionString)) {

				m_SKKDictionaly.GetCandidates(compositionString, m_CurrentCandidates);

				if (!m_CurrentCandidates.empty()) {
					m_CurrentShowCandidateIndex = 0;
					//確定はさせない
					_InsertText(pic, m_CurrentCandidates[m_CurrentShowCandidateIndex].c_str(), FALSE);
				}
				else {
					//TODO:新しい語の登録
				}

				return S_OK;
			}
		}
		//2回目以降
		else {
			m_CurrentShowCandidateIndex++;
			if (m_CurrentShowCandidateIndex >= m_CurrentCandidates.size()) {
				//TODO:新しい語の登録
				m_CurrentShowCandidateIndex = 0;//とりあえず最初に戻す
			}

			_InsertText(pic, m_CurrentCandidates[m_CurrentShowCandidateIndex].c_str(), FALSE);

			return S_OK;
		}
	
	}

	//確定処理
	if (key == VK_RETURN) {
		if (_pComposition) {
			*pfEaten = TRUE;

			_pComposition->EndComposition(TF_INVALID_EDIT_COOKIE);
			_pComposition.Release();
			_pComposition = nullptr;

			m_CurrentCandidates.clear();
			m_CurrentShowCandidateIndex = 0;
			m_RomajiToKanaTranslator.Reset();

			return S_OK;
		}
	}

	if (_IsKeyEaten(wParam))
	{
		//何かのキーが押されたら候補リストのクリア
		m_CurrentCandidates.clear();
		m_CurrentShowCandidateIndex = 0;


		key += L'a' - L'A';
		*pfEaten = TRUE;

		std::wstring output;
		if (m_RomajiToKanaTranslator.Translate(key, output))
		{
			//FIX:Debugが終了したらTRUEに戻す!
			_InsertText(pic, output.c_str(), FALSE);
		}
		else {
			//ひらがなの変換に達していない場合はバッファを表示
			_InsertText(pic, m_RomajiToKanaTranslator.GetBuffer().c_str(), FALSE);
		}

		return S_OK;
	}

	m_RomajiToKanaTranslator.Reset();

	return S_OK;
}

//キーが離された瞬間
STDAPI CSkkIme::OnKeyUp(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten) {
	*pfEaten = FALSE;
	if (_IsKeyEaten(wParam))
	{
		*pfEaten = TRUE;
		return S_OK;
	}
	return S_OK;
}

//このキーを処理しますかどうかを問い合わせる(キーが押された瞬間)
STDAPI CSkkIme::OnTestKeyDown(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten) {
	*pfEaten = FALSE;
	if (_IsKeyEaten(wParam))
	{
		*pfEaten = TRUE;
		return S_OK;
	}
	return S_OK;
}

STDAPI CSkkIme::OnTestKeyUp(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten) {
	*pfEaten = FALSE;
	if (_IsKeyEaten(wParam))
	{
		*pfEaten = TRUE;
		return S_OK;
	}
	return S_OK;
}
STDAPI CSkkIme::OnPreservedKey(ITfContext* pic, REFGUID rguid, BOOL* pfEaten) {
	*pfEaten = FALSE;

	return S_OK;
}
STDAPI CSkkIme::OnSetFocus(BOOL fForeground) {
	return S_OK;
}

BOOL CSkkIme::_InitKeyEventSink() {
	HRESULT hr = E_FAIL;

	CComPtr<ITfKeystrokeMgr> pKeystrokeMgr;
	if (FAILED(_pThreadMgr->QueryInterface(IID_ITfKeystrokeMgr, (void**)&pKeystrokeMgr))) {
		return FALSE;
	}

	//キーが押されたときに通知を受け取るように登録
	hr = pKeystrokeMgr->AdviseKeyEventSink(
		_clientId,
		static_cast<ITfKeyEventSink*>(this),
		TRUE
	);
	return SUCCEEDED(hr);
}

void CSkkIme::_UninitKeyEventSink() {
	CComPtr<ITfKeystrokeMgr> pKeystrokeMgr;
	if (SUCCEEDED(_pThreadMgr->QueryInterface(IID_ITfKeystrokeMgr, (void**)&pKeystrokeMgr))) {
		pKeystrokeMgr->UnadviseKeyEventSink(_clientId);
	}
}
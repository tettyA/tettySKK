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


			//TODO:位置を調整
			

			_InsertText(pic, m_CurrentCandidates[m_CurrentShowCandidateIndex].c_str(), FALSE);

			m_pCandidateWindow->SetCandidates(m_CurrentCandidates, m_CurrentShowCandidateIndex);
			_UpDateCandidateWindowPosition(pic);

			return S_OK;
		}
	
	}

	//確定処理
	if (key == VK_RETURN) {
		if (_pComposition) {
			*pfEaten = TRUE;

			_CommitComposition(pic);

			return S_OK;
		}
	}

	if (_IsKeyEaten(wParam))
	{
		key += L'a' - L'A';
		*pfEaten = TRUE;

		// 工廠(変換中) + n(新規) => 工廠(確定) + n(変換中)
		if (!m_CurrentCandidates.empty()) {
			
			_CommitComposition(pic);
		}

		//まず未確定変換文字列画面の文字を取得 ex: しm
		std::wstring textonScreen=L"";
		_GetCompositionString(textonScreen);
		
		//前回のローマ字変換バッファ長さを取得 ex: m = 1
		size_t prevRomajilen = m_RomajiToKanaTranslator.GetBuffer().size();
		
		//新しいキーを変換  ex: m + a = ま
		std::wstring newkana;
		m_RomajiToKanaTranslator.Translate(key, newkana);

		//今の画面[しm] - ローマ字[m] = ひらがな確定済部分[し]
		std::wstring baseKana = textonScreen.substr(0, textonScreen.size() - prevRomajilen);

		//ひらがな確定済部分[し] + 新かな[ま] + 新ローマ字バッファ[]
		std::wstring finalText = baseKana + newkana + m_RomajiToKanaTranslator.GetBuffer();
		
		_InsertText(pic, finalText.c_str(), FALSE);

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
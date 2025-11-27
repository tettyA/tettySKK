#include "pch.h"
#include "CSkkIme.h"

#include "Global.h"
#include "CInsertTextEditSession.h"
#include "CCandidateWindow.h"
#include <utility>

bool CSkkIme::_IsKeyEaten(WPARAM wParam) {
	WCHAR key = (WCHAR)wParam;
	if (_IsCtrlKeyPressed() && key == L'J') {
		_ChangeCurrentMode(SKKMode::Kakutei);
		_ChangeCurrenKanaMode(KanaMode::Hiragana);
		return true;
	}
	if (m_currentMode == SKKMode::Hankaku ) {
		return false;
	}
	if (_IsCtrlKeyPressed()) {
		return false;
	}
	if ((key >= L'A' && key <= L'Z') || (m_currentMode == SKKMode::Henkan && (key == VK_SPACE || key == VK_RETURN)))
	{
		return true;
	}
	return false;
}


//キーが押された瞬間
STDAPI CSkkIme::OnKeyDown(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten) {
	*pfEaten = FALSE;


	WCHAR key = (WCHAR)wParam;
	//新しい語の登録処理
	/*if (m_isRegiteringNewWord) {
		_TreatNewRegWord(key, pic);
		*pfEaten = TRUE;
		return S_OK;
	}*/
	//変換の処理
	if (_IsCtrlKeyPressed() && key == L'J') {
		_ChangeCurrentMode(SKKMode::Kakutei);
		_ChangeCurrenKanaMode(KanaMode::Hiragana);
		*pfEaten = TRUE;
		return S_OK;
	}
	else if (key == VK_SPACE) {
		//未入力もしくは変換モードでないならば，そのまま返す
		if (((!_pComposition) || m_currentMode != SKKMode::Henkan))
		{
			if (m_isRegiteringNewWord == FALSE)
				return S_OK;
		}

		*pfEaten = TRUE;
		if (m_isRegiteringNewWord) {
			return _HandleRegSpaceKey(pic, key);
		}
		else {
			return _HandleSpaceKey(pic, key);
		}
	}

	//確定処理
	else if (key == VK_RETURN) {
		if (m_isRegiteringNewWord) {
			//TODO: 実装
			*pfEaten = TRUE;
			if (m_currentMode == SKKMode::Henkan) {
				_CommitRegComposition(pic);
			}
			else {
				if (m_RegInputDetermined.empty()) {
					//何も登録されていないときは何もしない
					
					m_CurrentShowCandidateIndex--;

					m_isRegiteringNewWord = FALSE;
					m_RegInputDetermined = L"";
					m_RegInputUndetermined = L"";
					m_RegKey = L"";

					std::wstring additionalStr = L"";
					//TODO: 処理の共通化

					if (!m_Gokan.empty() && m_OkuriganaFirstChar != L'\0') {

						additionalStr = m_currentInputKana.substr(m_Gokan.length());
					}

					_ChangeCurrentMode(SKKMode::Henkan);

					__InsertTextMakeCandidateWindow(pic,
						(m_CurrentCandidates[m_CurrentShowCandidateIndex]_Candidate + additionalStr).c_str(),
						(m_currentInputKana).c_str()
					);


					return S_OK;
				}
				//TODO: ファイルにも保存されるようにする
				m_isRegiteringNewWord = FALSE;
				_Output(pic, m_RegInputDetermined.c_str(), TRUE);
				m_SKKDictionaly.AddCandidate(m_RegKey, m_RegInputDetermined);
				_EndRegiterNewWord();
			}
			return S_OK;
		}
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
		
		return _HandleCharKey(pic, key);
	}


	if (m_isRegiteringNewWord ) {
		if (m_currentMode == SKKMode::Hankaku) {
			if ((key >= L'A' && key <= L'Z') || (key == VK_SPACE))
			{
				key += L'a' - L'A';
				*pfEaten = TRUE;
				_Output(pic, std::wstring(1, key), FALSE);

				return S_OK;
			}
		}
		if (key == VK_BACK) {
			*pfEaten = TRUE;
			if (m_currentMode == SKKMode::Henkan) {
				//変換中のとき
				if (m_RegInputUndetermined.length() > 0) {
					m_RegInputUndetermined.pop_back();
					__InsertNewRegWord(pic, m_RegInputUndetermined, FALSE);
				}
				else {
					//未確定文字列が空なら確定文字列を削る
					if (m_RegInputDetermined.length() > 0) {
						m_RegInputDetermined.pop_back();
						__InsertNewRegWord(pic, L"", FALSE);
					}
				}
			}
			else {
				//確定中のとき
				if (m_RegInputDetermined.length() > 0) {
					m_RegInputDetermined.pop_back();
					__InsertNewRegWord(pic, L"", FALSE);
				}
			}
			return S_OK;
		}
	}

	if (key != VK_SHIFT) {
		m_RomajiToKanaTranslator.Reset();
	}

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

bool CSkkIme::_IsShiftKeyPressed()
{
	return ((GetKeyState(VK_SHIFT) & 0x8000) != 0);
}

bool CSkkIme::_IsCtrlKeyPressed()
{
	return ((GetKeyState(VK_CONTROL) & 0x8000) != 0);
}

void CSkkIme::_BgnRegiterNewWord(ITfContext* pic, std::wstring regKey)
{
	if (m_isRegiteringNewWord)return;

	m_isRegiteringNewWord = TRUE;
	m_RegInputDetermined = L"";
	m_RegInputUndetermined = L"";
	m_RegKey = regKey;

	m_currentMode = SKKMode::Kakutei;

	m_RegCurrentShowCandidateIndex = 0;
	m_RegCurrentCandidates.clear();

	__InsertText(pic, L"", FALSE);

	SKKCandidates regKeyCandidate = { {L"",regKey} };
	m_pCandidateWindow->SetCandidates(regKeyCandidate, 0, CANDIDATEWINDOW_MODE_REGWORD);
	_UpDateCandidateWindowPosition(pic);
}

void CSkkIme::_EndRegiterNewWord()
{
	m_isRegiteringNewWord = FALSE;
	m_RegInputDetermined = L"";
	m_RegInputUndetermined = L"";
	m_RegKey = L"";
	m_RegCurrentCandidates.clear();
	m_pCandidateWindow->MustHideWindow();
}


void CSkkIme::__InsertTextMakeCandidateWindow(ITfContext* pic,const WCHAR* _multiIntsertText,const WCHAR* _singleInsertText)
{
	if (m_CurrentShowCandidateIndex < BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) {
		//FIX: 名前が実情に沿っていない!
		_Output(pic, _multiIntsertText, FALSE);
		if (!m_isRegiteringNewWord) {
			m_pCandidateWindow->SetCandidates(m_CurrentCandidates, m_CurrentShowCandidateIndex, CANDIDATEWINDOW_MODE_SINGLE);
			_UpDateCandidateWindowPosition(pic);
		}
	}
	else {
		_Output(pic, _singleInsertText, FALSE);
		if (!m_isRegiteringNewWord) {
			m_pCandidateWindow->SetCandidates(m_CurrentCandidates, m_CurrentShowCandidateIndex, CANDIDATEWINDOW_MODE_MULTIPLE);
			_UpDateCandidateWindowPosition(pic);
		}
	}
}

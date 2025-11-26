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
		return false;
	}
	if (m_currentMode == SKKMode::Hankaku) {
		
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
	if (key == VK_SPACE) {
		//未入力もしくは変換モードでないならば，そのまま返す
		if ((!_pComposition) || m_currentMode != SKKMode::Henkan)
		{
			return S_OK;
		}

		*pfEaten = TRUE;
	
		return _HandleSpaceKey(pic, key);	
	}

	//確定処理
	if (key == VK_RETURN) {
		if (m_isRegiteringNewWord) {
			//TODO: 実装
			*pfEaten = TRUE;
			_EndRegiterNewWord();
		}
		if (_pComposition) {
			*pfEaten = TRUE;
	
	//		else {
				_CommitComposition(pic);
			//}
			return S_OK;
		}
	}

	if (_IsKeyEaten(wParam))
	{
		key += L'a' - L'A';
		*pfEaten = TRUE;
		
		return _HandleCharKey(pic, key);
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
	m_RegInput = L"";
	m_RegKey = regKey;

	m_currentMode = SKKMode::Kakutei;

	__InsertText(pic, L"", FALSE);

	m_pCandidateWindow->SetCandidates(SKKCandidates{ {L"",regKey}}, 0, CANDIDATEWINDOW_MODE_REGWORD);
	_UpDateCandidateWindowPosition(pic);
}

void CSkkIme::_EndRegiterNewWord()
{
	
	m_isRegiteringNewWord = FALSE;
	m_RegInput = L"";
	m_RegKey = L"";
	m_pCandidateWindow->MustHideWindow();
}


void CSkkIme::__InsertTextMakeCandidateWindow(ITfContext* pic,const WCHAR* _multiIntsertText,const WCHAR* _singleInsertText)
{
	if (m_CurrentShowCandidateIndex < BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) {

		_Output(pic, _multiIntsertText, FALSE);

		m_pCandidateWindow->SetCandidates(m_CurrentCandidates, m_CurrentShowCandidateIndex, CANDIDATEWINDOW_MODE_SINGLE);
		_UpDateCandidateWindowPosition(pic);
	}
	else {
		//TODO: ひらがなで表示したい
		_Output(pic, _singleInsertText, FALSE);

		m_pCandidateWindow->SetCandidates(m_CurrentCandidates, m_CurrentShowCandidateIndex, CANDIDATEWINDOW_MODE_MULTIPLE);
		_UpDateCandidateWindowPosition(pic);
	}
}

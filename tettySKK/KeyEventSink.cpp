#include "pch.h"
#include "CSkkIme.h"

#include "Global.h"
#include "CInsertTextEditSession.h"
#include "CCandidateWindow.h"
#include <utility>

#define __DEBUGOUTPUT(dbgstr) __InsertText(pic, (L"["+(dbgstr)+L"]").c_str(), TRUE)

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
	if ((key >= L'A' && key <= L'Z') || ((key == VK_OEM_PERIOD || key == VK_OEM_COMMA || key == VK_OEM_MINUS) && m_currentMode != SKKMode::Hankaku) || (m_currentMode == SKKMode::Henkan && (key == VK_SPACE || key == VK_RETURN)))
	{
		return true;
	}
	//新しい語の登録中は，+(=JIS配列で;)キーも食う
	if (key == VK_OEM_PLUS || key == L';') {
		return true;
	}

	//変換中のときは，BackSpaceも食う
	if (m_currentMode == SKKMode::Henkan && key == VK_BACK && !m_isRegiteringNewWord) {
		return true;
	}

	return false;
}

bool CSkkIme::_IsKeyEatenTest(WPARAM wParam)
{
	WCHAR key = (WCHAR)wParam;
	if (m_isRegiteringNewWord && (key == VK_RETURN || key == VK_BACK)) {
		return true;
	}
	return false;
}


//キーが押された瞬間
STDAPI CSkkIme::OnKeyDown(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten) {
	*pfEaten = FALSE;

	WCHAR key = (WCHAR)wParam;
	if (((key < L'A' || key > L'Z')) && m_RomajiToKanaTranslator.GetBuffer() == L"n" && m_currentMode!=SKKMode::Hankaku) {
		//撥音が残っており，かつ，ローマ字入力以外のキーが押された場合は，撥音を確定する

		if (m_currentMode == SKKMode::Kakutei) {

			std::wstring output;
			if (m_CurrentKanaMode == KanaMode::Hiragana) {
				output = L"ん";
			}
			else {
				output = L"ン";
			}
			_Output(pic, output, TRUE);

		}
		else if (m_currentMode==SKKMode::Henkan) {
			std::wstring  tempCompStr;
			std::wstring finalText;
			_GetCompositionString(tempCompStr);
			// bufferが nn になるので，'ん'が追加されるはず.
			_AddToTextonScreenAndUpdateCurrentInputKana(L'n', tempCompStr, finalText);

			_Output(pic, finalText, FALSE);
		}
		

		m_RomajiToKanaTranslator.Reset();
	}
	
	if (m_RomajiToKanaTranslator.isMustNOTExecuteKigou(key) && m_currentMode != SKKMode::Hankaku) {
		//下については，後で処理するのでOK
		if (
			//特定の条件下のピリオドは除く(ピリオドを押すと，第一優先候補を確定したいため)
			!(
				(!m_isRegiteringNewWord && m_currentMode == SKKMode::Henkan && m_CurrentCandidates.empty() &&
					key == VK_OEM_PERIOD)

				)
			) {


			//約物の処理

			*pfEaten = TRUE;
			std::wstring compStr;
			_GetCompositionString(compStr);
			//約物は即変換(但し，セミコロンやEnter,Backspaceなどのものは除く)
			std::wstring kigou; //= m_RomajiToKanaTranslator.GetBuffer();
			m_RomajiToKanaTranslator.TranslateKigou(key, kigou);

			std::wstring output = compStr + kigou;
			if (//変換中の ー は確定させない。
				(m_currentMode == SKKMode::Henkan && key == VK_OEM_MINUS)) {
				_Output(pic, output, FALSE);
			}
			else {
				m_RomajiToKanaTranslator.Reset();
				_Output(pic, output, TRUE);
				_CommitComposition(pic);
			}


			return S_OK;
		}
	}

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
			if (!m_RegInputUndetermined.empty()) {
				//未確定文字列の確定
				_CommitRegComposition(pic);
				return S_OK;
			}
			else if (m_RegInputDetermined.empty()) {
				//文字列がないときは，キャンセル扱いにする
				//候補がある時は，最後の候補ウィンドウを，なければ表示しない
				if (m_CurrentCandidates.size() > 0 && m_CurrentShowCandidateIndex > 0) {
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

				}
				else {
					_EndRegiterNewWord();
					_ChangeCurrentMode(SKKMode::Henkan);
					_Output(pic, m_currentInputKana.c_str(), FALSE);
				}

				return S_OK;
			}
			else {

				//送り仮名の取得(Commitして前のcompStrを潰してしまう前に取得しておく)
				//TODO: 送り仮名が複数文字の場合の対応(そんなことはないと思うが...)
				std::wstring tempStr;
				{
					//regの方ではなく，通常のcomposition文字列を取得したいので，一旦FALSEにする。
					m_isRegiteringNewWord = FALSE;
					_GetCompositionString(tempStr);
					if (std::isalpha(m_RegKey.back())) {
						//送り仮名がある(あった)場合は，送りがなもOutputする。
						tempStr = tempStr.back();
					}
					else {
						tempStr.clear();
					}
					m_isRegiteringNewWord = TRUE;
				}


				//新しい単語の登録確定
				_CommitComposition(pic);
				
				m_isRegiteringNewWord = FALSE;

				_Output(pic, (m_RegInputDetermined.substr(0, m_RegInputDetermined.find(SKK_CANDIDOTATES_ANNOTATION_SEPARATOR_CHAR)) + tempStr).c_str(), TRUE);
				
				
				m_SKKDictionaly.AddCandidate(m_RegKey, m_RegInputDetermined);
				m_SKKDictionaly.SaveDictionaryToUserFile(SKK_USER_DICTIONARY_FILEPATH);

				
				_EndRegiterNewWord();

				return S_OK;
			}
			return S_OK;
		}
		if (_pComposition) {
			*pfEaten = TRUE;
			if (m_CurrentShowCandidateIndex < BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX && m_CurrentCandidates.size() > m_CurrentShowCandidateIndex) {
				{
					std::wstring searchKey = m_currentInputKana;
					if (m_OkuriganaFirstChar != L'\0') {
						searchKey = m_Gokan + m_OkuriganaFirstChar;
					}
					m_SKKDictionaly.AddHistoryCandidate(searchKey, m_CurrentCandidates[m_CurrentShowCandidateIndex]);
				}
				//m_SKKDictionaly.AddHistoryCandidate(m_currentInputKana, m_CurrentCandidates[m_CurrentShowCandidateIndex]);
			}
			_CommitComposition(pic);
			return S_OK;
		}
	}
	else if (key == VK_BACK) {
		if (m_RomajiToKanaTranslator.GetBuffer().length() > 0) {
			//__DEBUGOUTPUT(L"1:"+m_RomajiToKanaTranslator.GetBuffer());
			m_RomajiToKanaTranslator.PopBackBuffer();
			*pfEaten = TRUE;
			//__DEBUGOUTPUT(L"2:" + m_RomajiToKanaTranslator.GetBuffer());

		}
		if (m_isRegiteringNewWord) {
			//pass
		}
		else if (_pComposition) {
			*pfEaten = TRUE;


			HRESULT hr = S_OK;
			std::wstring currentCompStr;
			_GetCompositionString(currentCompStr);
			if (currentCompStr.length() > 0) {
				currentCompStr.pop_back();
				if (m_currentMode == SKKMode::Henkan) {
					m_currentInputKana.pop_back();
					if (m_CurrentCandidates.size() > 0) {
						__InsertText(pic, currentCompStr.c_str(), TRUE);
						if (m_CurrentShowCandidateIndex < BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX && m_CurrentCandidates.size() > m_CurrentShowCandidateIndex) {
						//	m_SKKDictionaly.AddHistoryCandidate(m_currentInputKana, m_CurrentCandidates[m_CurrentShowCandidateIndex]);
							{
								std::wstring searchKey = m_currentInputKana;
								if (m_OkuriganaFirstChar != L'\0') {
									searchKey = m_Gokan + m_OkuriganaFirstChar;
								}
								m_SKKDictionaly.AddHistoryCandidate(searchKey, m_CurrentCandidates[m_CurrentShowCandidateIndex]);
							}
							//m_SKKDictionaly.AddHistoryCandidate(m_currentInputKana);
						}
						_CommitComposition(pic);
					}
					else {
						__InsertText(pic, currentCompStr.c_str(), FALSE);
						if (currentCompStr.length() > 0) {
							_SearchMostHighestPriorityCandidateWordAndVisualizePredictiveCandidateWindowFromHistory(pic);
						}
						else {
							m_pCandidateWindow->MustHideWindow();
							_CommitComposition(pic);
						}
					}
				}
				else {
					__InsertText(pic, currentCompStr.c_str(), FALSE);
				}
			}
			else {
				m_pCandidateWindow->MustHideWindow();
				_CommitComposition(pic);
			}
		//	m_RomajiToKanaTranslator.Reset();
			
			return hr;
		}
		if (!m_isRegiteringNewWord) {
			return S_OK;
		}
	}
	else if (!m_isRegiteringNewWord && m_currentMode == SKKMode::Henkan && m_CurrentCandidates.empty() &&
		key == VK_OEM_PERIOD
		) {
			*pfEaten = TRUE;
			//予測候補で変換。
			
			//TODO: 送り仮名の処理
			//TODO: 変換した後に，スペースなどで候補が入れ替わるようにする。
			SKKCandidate predictionCandidate;
			m_SKKDictionaly.GetPredictionCandidate(m_currentInputKana, predictionCandidate);
			if (!predictionCandidate _Candidate.empty()) {
				_Output(pic, predictionCandidate _Candidate.c_str(), TRUE);
				//m_SKKDictionaly.AddHistoryCandidate(m_currentInputKana);
				{
					std::wstring searchKey = m_currentInputKana;
					if (m_OkuriganaFirstChar != L'\0') {
						searchKey = m_Gokan + m_OkuriganaFirstChar;
					}
					m_SKKDictionaly.AddHistoryCandidate(searchKey, predictionCandidate);
				}
				//m_SKKDictionaly.AddHistoryCandidate(m_currentInputKana, predictionCandidate);
				_CommitComposition(pic);
			}

			return S_OK;
	}
	if (_IsKeyEaten(wParam))
	{
		key += ToSmallAlphabet;
		*pfEaten = TRUE;

		return _HandleCharKey(pic, key);
	}


	if (m_isRegiteringNewWord) {
		if (m_currentMode == SKKMode::Hankaku) {
			if ((key >= L'A' && key <= L'Z') || (key == VK_SPACE))
			{
				key += ToSmallAlphabet;
				*pfEaten = TRUE;
				_Output(pic, std::wstring(1, key), FALSE);

				return S_OK;
			}
		}
		if (key == VK_BACK) {
			*pfEaten = TRUE;
			//	if (m_currentMode == SKKMode::Henkan) {
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

		//	m_RomajiToKanaTranslator.Reset();
			return S_OK;
		}
	}

	if (key != VK_SHIFT) {
//		m_RomajiToKanaTranslator.Reset();
	}

	return S_OK;
}


//キーが離された瞬間
STDAPI CSkkIme::OnKeyUp(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten) {
	*pfEaten = FALSE;
	if (_IsKeyEaten(wParam) || _IsKeyEatenTest(wParam))
	{
		*pfEaten = TRUE;
		return S_OK;
	}
	return S_OK;
}

//このキーを処理しますかどうかを問い合わせる(キーが押された瞬間)
STDAPI CSkkIme::OnTestKeyDown(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten) {
	*pfEaten = FALSE;
	if (_IsKeyEaten(wParam) || _IsKeyEatenTest(wParam))
	{
		*pfEaten = TRUE;
		return S_OK;
	}
	return S_OK;
}

STDAPI CSkkIme::OnTestKeyUp(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten) {
	*pfEaten = FALSE;
	if (_IsKeyEaten(wParam) || _IsKeyEatenTest(wParam))
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

	//__InsertText(pic, (std::to_wstring(m_CurrentShowCandidateIndex)).c_str(), FALSE);
	//__InsertText(pic, L"", FALSE);

	SKKCandidates regKeyCandidate = { {L"",regKey},{L"",L""}};
	m_pCandidateWindow->SetCandidates(regKeyCandidate, 0, CANDIDATEWINDOW_MODE_REGWORD);
	_UpDateCandidateWindowPosition(pic);
}

void CSkkIme::__FinishCandidateWindowShow()
{
	m_pCandidateWindow->MustHideWindow();
	if (m_isRegiteringNewWord) {
		m_RegCurrentCandidates.clear();
		m_RegCurrentShowCandidateIndex = 0;
	}
	m_CurrentCandidates.clear();
	m_CurrentShowCandidateIndex = 0;
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

void CSkkIme::_SearchMostHighestPriorityCandidateWordAndVisualizePredictiveCandidateWindowFromHistory(ITfContext* pic)
{
	//変換履歴からの予測変換候補表示
	if (!m_isRegiteringNewWord && m_CurrentCandidates.empty()) {
		
		SKKCandidate predictionCandidate;
		m_SKKDictionaly.GetPredictionCandidate(m_currentInputKana, predictionCandidate);

		if (!predictionCandidate _Candidate.empty()) {
			SKKCandidates predictionCandidateVec = { predictionCandidate };
			m_pCandidateWindow->SetCandidates(predictionCandidateVec, 0, CANDIDATEWINDOW_MODE_PREDICT);
			_UpDateCandidateWindowPosition(pic);
		}
		else {
			if (m_pCandidateWindow->IsWindowExists()) {
				m_pCandidateWindow->HideWindow();
			}
		}
	}
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

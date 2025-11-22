#include "pch.h"
#include "CSkkIme.h"

#include "Global.h"
#include "CInsertTextEditSession.h"
#include "CCandidateWindow.h"

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

	//FIX: ひらがな以外も対応
	WCHAR key = (WCHAR)wParam;

	//変換の処理
	if (key == VK_SPACE) {
		//未入力もしくは変換モードでないならば，そのまま返す
		if ((!_pComposition) || m_currentMode != SKKMode::Henkan)
		{
			return S_OK;
		}

		*pfEaten = TRUE;
	
		if (m_CurrentCandidates.empty())
		{
			std::wstring compositionString;
	
			if (_GetCompositionString(compositionString)) {
				{
					std::wstring searchStr = compositionString;
					if (!m_Gokan.empty() && m_OkuriganaFirstChar != L'\0') {
						//送り仮名付きのとき
						searchStr = m_Gokan + m_OkuriganaFirstChar;
					}
					m_SKKDictionaly.GetCandidates(searchStr, m_CurrentCandidates);
				}
				if (m_CurrentCandidates.size() == 0) {
					//TODO: 新しい語の登録
				}
				else {

					std::wstring displayStr = m_CurrentCandidates[0];

					if (!m_Gokan.empty() && m_OkuriganaFirstChar != L'\0'/* && compositionString.length() > m_Gokan.length()*/) {
						//displayStr = 書   compositionString = 書k or 書く
						displayStr += compositionString.substr(m_Gokan.length());
						//		MessageBox(NULL, (displayStr+L" "+compositionString+L" "+m_Gokan+L" "+m_OkuriganaFirstChar).c_str(), L"debug", MB_OK);
					}

					if (!m_CurrentCandidates.empty()) {
						m_CurrentShowCandidateIndex = 0;
						//確定はさせない

						_InsertText(pic, (displayStr).c_str(), FALSE);
					}
					else {
						//TODO:新しい語の登録
					}

				}
				return S_OK;
			}
		}
		//2回目以降
		else {
			
			std::wstring additionalStr = L"";
			std::wstring compositionString;
			_GetCompositionString(compositionString);
			//送り仮名付きのとき
			if (!m_Gokan.empty() && m_OkuriganaFirstChar != L'\0') {
				//書k or 書く  -> k or く
				additionalStr = compositionString.substr(compositionString.length() - 1);
			}

			m_CurrentShowCandidateIndex++;
			if (m_CurrentShowCandidateIndex >= m_CurrentCandidates.size() ||
				(m_CurrentShowCandidateIndex >= BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX &&
					(BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX +
						(m_CurrentShowCandidateIndex -BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX ) *
						NUM_SHOW_CANDIDATE_MULTIPLE)
					>= m_CurrentCandidates.size())
				) {
				//TODO:新しい語の登録
				m_CurrentShowCandidateIndex = 0;//とりあえず最初に戻す
			}

			if (m_CurrentShowCandidateIndex < BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) {

				_InsertText(pic, (m_CurrentCandidates[m_CurrentShowCandidateIndex] + additionalStr).c_str(), FALSE);

				m_pCandidateWindow->SetCandidates(m_CurrentCandidates, m_CurrentShowCandidateIndex, CANDIDATEWINDOW_MODE_SINGLE);
				_UpDateCandidateWindowPosition(pic);
			}
			else {
				//TODO: ひらがなで表示したい
				_InsertText(pic, (m_CurrentCandidates[0] + additionalStr).c_str(), FALSE);

				m_pCandidateWindow->SetCandidates(m_CurrentCandidates, m_CurrentShowCandidateIndex, CANDIDATEWINDOW_MODE_MULTIPLE);
				_UpDateCandidateWindowPosition(pic);
			}
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
		if (key == L'q') {
		 _ChangeCurrenKanaMode(KanaMode::Katakana);
			return S_OK;
		}
		else if (key == L'l') {
			//TODO: 処理の共通化
			if (m_pCandidateWindow->IsWindowExists()) {
				m_pCandidateWindow->HideWindow();
			}

			m_CurrentCandidates.clear();
			m_CurrentShowCandidateIndex = 0;

			m_Gokan = L"";
			m_OkuriganaFirstChar = L'\0';
			_ChangeCurrentMode(SKKMode::Hankaku);
			return S_OK;
		}

		// 工廠(変換中) + n(新規) => 工廠(確定) + n(変換中)  (暗黙確定)
		if (!m_CurrentCandidates.empty()) {
			if (m_CurrentShowCandidateIndex >= BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) {
			//TODO: ASDFJKL を打ち込む時に，それ以外のものが打ち込まれたらCandidates[0]で確定	
			}
			else {
				_CommitComposition(pic);
			}
		}

		bool isShift = _IsShiftKeyPressed();
		//変換の開始
		if (isShift && m_currentMode == SKKMode::Kakutei) {

			if (_pComposition) {
				//_CommitComposition(pic);
				//_CommitComposition without 確定 and m_RomajiToKanaTranslator.Reset();

				if (m_pCandidateWindow->IsWindowExists()) {
					m_pCandidateWindow->HideWindow();
				}

				m_CurrentCandidates.clear();
				m_CurrentShowCandidateIndex = 0;
			//	m_RomajiToKanaTranslator.Reset();
			//	m_currentMode = SKKMode::Henkan;

				m_Gokan = L"";
				m_OkuriganaFirstChar = L'\0';
			}

			_ChangeCurrentMode(SKKMode::Henkan);

			// ↑↑ 以上で_CommitComposition が終了 ↑↑

			isShift = false;
		}
		

		//FIX: 書k (no shift) の場合，書 で検索する。 つまり，変換中のローマ字がシフトが押されずに確定された場合は，削除する。
		//_InsertText(pic, (L"[Debug2:]"), TRUE);
		//変換中
		if (m_currentMode == SKKMode::Henkan) {
	
			//ASDFJKL で選ぶ段階
			if (m_CurrentShowCandidateIndex >= BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) {
				
				int cnt = 0;
				switch (key)
				{
				case L'l':
					cnt++;
					[[fallthrough]];
				case L'k':
					cnt++;
					[[fallthrough]];
				case L'j':
					cnt++;
					[[fallthrough]];
				case L'f':
					cnt++;
					[[fallthrough]];
				case L'd':
					cnt++;
					[[fallthrough]];
				case L's':
					cnt++;
					[[fallthrough]];
				case L'a':
				{
					std::wstring baseword = m_CurrentCandidates[BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX + (m_CurrentShowCandidateIndex - BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) * NUM_SHOW_CANDIDATE_MULTIPLE + cnt];
					std::wstring additionalStr = L"";
					_InsertText(pic, baseword.c_str(), FALSE);
					_CommitComposition(pic);
					return S_OK;
					break;
				}
				case L'x':
					//TODO: 一個まえのページに戻る。処理の共通化。BeGINSHOWよりも小さくてもやる
					break;
				default:
					return S_OK;
					break;
				}

				return S_OK;
			}

			//まず未確定変換文字列画面の文字を取得 ex: しm
			std::wstring textonScreen = L"";
			_GetCompositionString(textonScreen);

			//前回のローマ字変換バッファ長さを取得 ex: m = 1
			size_t prevRomajilen = m_RomajiToKanaTranslator.GetBuffer().length();
			std::wstring prevRomaji = m_RomajiToKanaTranslator.GetBuffer();

			//新しいキーを変換  ex: m + a = ま
			std::wstring newkana;
			m_RomajiToKanaTranslator.Translate(key, newkana, m_CurrentKanaMode);

			//今の画面[しm] - ローマ字[m] = ひらがな確定済部分[し]
			std::wstring baseKana = textonScreen.substr(0, textonScreen.length() - prevRomajilen);

			//ひらがな確定済部分[し] + 新かな[ま] + 新ローマ字バッファ[]
			std::wstring finalText = baseKana + newkana + m_RomajiToKanaTranslator.GetBuffer();


			_InsertText(pic, finalText.c_str(), FALSE);

			//送り仮名の指定
			if (!textonScreen.empty() && (isShift = _IsShiftKeyPressed())) {
				// 一回目のシフトで変換モードの開始。(ここに来る前に処理済み)
				// ↓↓↓↓ここからここで処理する↓↓↓↓
				// 二回目のシフトの時，かな|漢字 確定部分.length() == 0 ならば無視。
				// かな|漢字 確定部分.length() >= 1 ならば，送り仮名開始し，語幹を設定。
				// もし，既に語幹が設定されていたら無視。
				//_InsertText(pic, (L"[Debug1:"+ m_Gokan + L"]").c_str(), TRUE);
				if (baseKana.length() >= 1 && m_Gokan.empty() && m_OkuriganaFirstChar == L'\0') {
					m_Gokan = baseKana;
					m_OkuriganaFirstChar = key;
					if (prevRomajilen >= 1) {
						m_OkuriganaFirstChar = prevRomaji[0];
					}
				}

			}
		}

		if (m_currentMode == SKKMode::Kakutei) {
			std::wstring newkana;
			if (m_RomajiToKanaTranslator.Translate(key, newkana, m_CurrentKanaMode)) {
				_InsertText(pic, newkana.c_str(), TRUE);
			}
			else {
				//変換に達していない場合は，バッファをそのまま表示
				_InsertText(pic, m_RomajiToKanaTranslator.GetBuffer().c_str(), FALSE);
			}
		}

		return S_OK;
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
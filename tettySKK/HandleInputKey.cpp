#include "pch.h"
#include "CSkkIme.h"

#include "Global.h"
#include "CCandidateWindow.h"


HRESULT CSkkIme::_HandleSpaceKey(ITfContext* pic, WCHAR key)
{

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
				_BgnRegiterNewWord(pic, m_currentInputKana);
			}
			else {

				std::wstring displayStr = m_CurrentCandidates[0]_Candidate;

				if (!m_Gokan.empty() && m_OkuriganaFirstChar != L'\0'/* && compositionString.length() > m_Gokan.length()*/) {
					//displayStr = 書   compositionString = 書k or 書く
					displayStr += compositionString.substr(m_Gokan.length());
				}

				if (!m_CurrentCandidates.empty()) {
					m_CurrentShowCandidateIndex = 0;
					//確定はさせない
					if (m_isRegiteringNewWord) {
						//TODO: 何かする
					}
					else {
						_Output(pic, (displayStr), FALSE);
					}
				}
				else {
					//TODO:新しい語の登録
					_BgnRegiterNewWord(pic, m_currentInputKana);
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
					(m_CurrentShowCandidateIndex - BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) *
					NUM_SHOW_CANDIDATE_MULTIPLE)
				>= m_CurrentCandidates.size())
			) {
			//TODO:新しい語の登録
		//	m_CurrentShowCandidateIndex = 0;//とりあえず最初に戻す
			//TODO: ひらがなにしたい
			_BgnRegiterNewWord(pic, m_currentInputKana);
			return S_OK;
		}

		__InsertTextMakeCandidateWindow(pic,
			(m_CurrentCandidates[m_CurrentShowCandidateIndex]_Candidate + additionalStr).c_str(),
			(m_currentInputKana).c_str()
		);


		return S_OK;
	}

}

HRESULT CSkkIme::_HandleCharKey(ITfContext* pic, WCHAR key)
{
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
	else if (key == L'x' && m_currentMode == SKKMode::Henkan) {

		//前の候補
		//TODO:  処理の共通化

		std::wstring additionalStr = L"";
		std::wstring compositionString;
		_GetCompositionString(compositionString);
		//送り仮名付きのとき
		if (!m_Gokan.empty() && m_OkuriganaFirstChar != L'\0') {
			//書k or 書く  -> k or く
			additionalStr = compositionString.substr(compositionString.length() - 1);
		}
		m_CurrentShowCandidateIndex = max(0, (int)m_CurrentShowCandidateIndex - 1);

		__InsertTextMakeCandidateWindow(pic,
			(m_CurrentCandidates[m_CurrentShowCandidateIndex]_Candidate + additionalStr).c_str(),
			(m_currentInputKana).c_str()
		);

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
			case SKK_CHOOSE_CANDIDATES_SMLSTR[6]:
				cnt++;
				[[fallthrough]];
			case SKK_CHOOSE_CANDIDATES_SMLSTR[5]:
				cnt++;
				[[fallthrough]];
			case SKK_CHOOSE_CANDIDATES_SMLSTR[4]:
				cnt++;
				[[fallthrough]];
			case SKK_CHOOSE_CANDIDATES_SMLSTR[3]:
				cnt++;
				[[fallthrough]];
			case SKK_CHOOSE_CANDIDATES_SMLSTR[2]:
				cnt++;
				[[fallthrough]];
			case  SKK_CHOOSE_CANDIDATES_SMLSTR[1]:
				cnt++;
				[[fallthrough]];
			case  SKK_CHOOSE_CANDIDATES_SMLSTR[0]:
			{
				std::wstring baseword = m_CurrentCandidates[BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX + (m_CurrentShowCandidateIndex - BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) * NUM_SHOW_CANDIDATE_MULTIPLE + cnt]_Candidate;
				std::wstring additionalStr = L"";

				if (!m_Gokan.empty() && m_OkuriganaFirstChar != L'\0') {
					additionalStr = m_currentInputKana.substr(m_Gokan.length());
				}
				_Output(pic, (baseword + additionalStr), FALSE);
				_CommitComposition(pic);
				return S_OK;
				break;
			}
			default:
				return S_OK;
				break;
			}

			return S_OK;
		}

		//TODO: 処理共通化。一方を変更したら，もう一方も変更
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

		m_currentInputKana = baseKana + newkana;

		_Output(pic, finalText, FALSE);

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
			_Output(pic, newkana, TRUE);
		}
		else {
			//変換に達していない場合は，バッファをそのまま表示
			_Output(pic, m_RomajiToKanaTranslator.GetBuffer(), FALSE);
		}
	}

	return S_OK;
}

/*


void CSkkIme::__TreatNewRegWord(WCHAR key, ITfContext* pic)
{
	//TODO: 片仮名語は検索が出来ないので，改善(辞書から検索出来ない。ひらがなに強制変換されるので)
	if (key == VK_RETURN) {
		if (m_RegInput.empty()) {

			if (m_CurrentCandidates.empty()) {
				_EndRegiterNewWord();
			}
			else {
				m_CurrentShowCandidateIndex--;

				m_isRegiteringNewWord = FALSE;
				m_RegInput = L"";
				m_RegKey = L"";

				std::wstring additionalStr = L"";
				//TODO: 処理の共通化

				if (!m_Gokan.empty() && m_OkuriganaFirstChar != L'\0') {

					additionalStr = m_currentInputKana.substr(m_Gokan.length());
				}

				__InsertTextMakeCandidateWindow(pic,
					(m_CurrentCandidates[m_CurrentShowCandidateIndex]_Candidate + additionalStr).c_str(),
					(m_currentInputKana).c_str()
				);

			}
			return;
		}

		//TODO: ファイルにも保存されるようにする
		_Output(pic, m_RegInput.c_str(), TRUE);
		m_SKKDictionaly.AddCandidate(m_RegKey, m_RegInput);
		_EndRegiterNewWord();
		return;
	}
	else if (key == VK_BACK && m_RegInput.length() >= 1) {
		m_RegInput.pop_back();
	}
	else if (_IsKeyEaten(key))
	{
		key += L'a' - L'A';
		if (m_currentMode == SKKMode::Hankaku)
		{
			m_RegInput += key;
		}
		else {
			//TODO: 処理共通化。一方を変更したら，もう一方も変更
			std::wstring textonScreen = L"";
			SKKCandidates textonCandidates;
			m_pCandidateWindow->GetCandidates(textonCandidates);

			textonScreen = textonCandidates[0]_Candidate;

			size_t prevRomajilen = m_RomajiToKanaTranslator.GetBuffer().length();
			std::wstring newkana;
			m_RomajiToKanaTranslator.Translate(key, newkana, m_CurrentKanaMode);

			std::wstring baseKana = textonScreen.substr(0, textonScreen.length() - prevRomajilen);

			std::wstring finalText = baseKana + newkana + m_RomajiToKanaTranslator.GetBuffer();
			m_RegInput = finalText;
		}
	}

	if (m_pCandidateWindow->IsWindowExists()) {
		m_pCandidateWindow->SetCandidates(SKKCandidates{ {m_RegInput,L""} }, 0, CANDIDATEWINDOW_MODE_REGWORD);
		_UpDateCandidateWindowPosition(pic);
	}
}*/

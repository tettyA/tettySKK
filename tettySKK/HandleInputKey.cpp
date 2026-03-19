#include "pch.h"
#include "CSkkIme.h"

#include "Global.h"
#include "CCandidateWindow.h"

#include "CInsertTextEditSession.h"
#define __DEBUGOUTPUT(dbgstr) __InsertText(pic, (L"["+(dbgstr)+L"]").c_str(), TRUE)
HRESULT CSkkIme::_HandleRegSpaceKey(ITfContext* pic, WCHAR key)
{
	if (g_currentMode != SKKMode::Henkan) {
		_Output(pic, std::wstring(1, L' '), TRUE);
		return S_OK;
	}
	if (m_RegCurrentCandidates.empty())
	{
		std::wstring compositionString;

		if (_GetCompositionString(compositionString)) {
			{
				std::wstring searchStr = compositionString;
				if (!m_Gokan.empty() && m_OkuriganaFirstChar != L'\0') {
					//送り仮名付きのとき
					searchStr = m_Gokan + m_OkuriganaFirstChar;
				}
				m_SKKDictionaly.GetCandidates(searchStr, m_RegCurrentCandidates);
			}
			if (m_RegCurrentCandidates.size() == 0) {
				//TODO: 新しい語の登録
				//_BgnRegiterNewWord(pic, m_currentInputKana);
			}
			else {

				std::wstring displayStr = m_RegCurrentCandidates[0]_Candidate;

				if (!m_Gokan.empty() && m_OkuriganaFirstChar != L'\0'/* && compositionString.length() > m_Gokan.length()*/) {
					//displayStr = 書   compositionString = 書k or 書く
					displayStr += compositionString.substr(m_Gokan.length());
				}

				if (!m_RegCurrentCandidates.empty()) {
					m_RegCurrentShowCandidateIndex = 0;
					//確定はさせない
					_Output(pic, (displayStr), FALSE);
					
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

		m_RegCurrentShowCandidateIndex++;
		if ((int)m_RegCurrentShowCandidateIndex >=(int)( (int)m_RegCurrentCandidates.size() - 2) ||
			(m_RegCurrentShowCandidateIndex >= BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX &&
				(BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX +
					(m_RegCurrentShowCandidateIndex - BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) *
					NUM_SHOW_CANDIDATE_MULTIPLE)
				>= (int)((int)m_RegCurrentCandidates.size() - 2))
			) {
			m_RegCurrentShowCandidateIndex = 0;//とりあえず最初に戻す
			//return S_OK;
		}

		__InsertTextMakeCandidateWindow(pic,
			(m_RegCurrentCandidates[m_RegCurrentShowCandidateIndex]_Candidate + additionalStr).c_str(),
			(m_RegInputDetermined).c_str()
		);
		//TODO: m_RegInputではないようにする。

		return S_OK;
	}

	return S_OK;
}

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
				if (!m_Gokan.empty() && m_OkuriganaFirstChar != L'\0') {
					//送り仮名あり用
					_BgnRegiterNewWord(pic, m_Gokan + m_OkuriganaFirstChar);
				}
				else {
					_BgnRegiterNewWord(pic, m_currentInputKana);
				}

				return S_OK;
			}
			else {

				std::wstring displayStr = m_CurrentCandidates[0]_Candidate;

				m_isExplictingConversionMode = true;

				if (!m_Gokan.empty() && m_OkuriganaFirstChar != L'\0'/* && compositionString.length() > m_Gokan.length()*/) {
					//displayStr = 書   compositionString = 書k or 書く
					displayStr += compositionString.substr(m_Gokan.length());
					//__InsertText(pic, (L"[go:" + m_Gokan + L" co:"+compositionString+L"]").c_str(), TRUE);
				}

				__InsertTextMakeCandidateWindow(pic,
					(displayStr).c_str(),
					(displayStr).c_str()
				);

				return S_OK;
			}
			return S_OK;
		}
	}
	//2回目以降
	else {

		std::wstring additionalStr = L"";
		std::wstring compositionString;

		_GetCompositionString(compositionString);
		m_isExplictingConversionMode = true;
		//送り仮名付きのとき
		if (!m_Gokan.empty() && m_OkuriganaFirstChar != L'\0') {
			//書k or 書く  -> k or く
			additionalStr = m_currentInputKana.substr(m_Gokan.length());
		}

		m_CurrentShowCandidateIndex++;
		if (m_CurrentShowCandidateIndex >= m_CurrentCandidates.size() ||
			(m_CurrentShowCandidateIndex >= BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX &&
				(BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX +
					(m_CurrentShowCandidateIndex - BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) *
					NUM_SHOW_CANDIDATE_MULTIPLE)
				>= m_CurrentCandidates.size())
			) {
			
			if (!m_Gokan.empty() && m_OkuriganaFirstChar != L'\0') {
				_BgnRegiterNewWord(pic, m_Gokan + m_OkuriganaFirstChar);
			}
			else {
				_BgnRegiterNewWord(pic, m_currentInputKana);
			}
			return S_OK;
		}

		__InsertTextMakeCandidateWindow(pic,
			(m_CurrentCandidates[m_CurrentShowCandidateIndex]_Candidate + additionalStr).c_str(),
			(m_currentInputKana).c_str()
		);


		return S_OK;
	}
	return S_OK;
}

HRESULT CSkkIme::_HandleCharKey(ITfContext* pic, WCHAR key)
{

	// 工廠(変換中) + n(新規) => 工廠(確定) + n(変換中)  (暗黙確定)
	if (!m_isRegiteringNewWord && !m_CurrentCandidates.empty() && m_isExplictingConversionMode) {
		if (m_CurrentShowCandidateIndex >= BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) {
			//TODO: ASDFJKL を打ち込む時に，それ以外のものが打ち込まれたらCandidates[0]で確定	
		}
		else {
			//送り仮名を打つ場合は，確定をしない
		//	if (m_OkuriganaFirstChar != L'\0') {
				//m_SKKDictionaly.AddHistoryCandidate(m_currentInputKana);
			{
				std::wstring searchKey = m_currentInputKana;
				if (m_OkuriganaFirstChar != L'\0') {
					searchKey= m_Gokan + m_OkuriganaFirstChar;
				}
				m_SKKDictionaly.AddHistoryCandidate(searchKey, m_CurrentCandidates[m_CurrentShowCandidateIndex]);
			}
				std::wstring nextChar(1, key);
				std::wstring newxtKana;
				std::wstring nextinsert;
				m_RomajiToKanaTranslator.Reset();
			//	__DEBUGOUTPUT(L"next:" + nextChar);
				if (m_RomajiToKanaTranslator.Translate(key, newxtKana)) {
					nextinsert = newxtKana;
					
					// Shiftが押されていたら変換モードを開始
					if (_IsShiftKeyPressed()) {
						if (_pComposition) {
							_CommitAndStartComposition(pic, nextinsert);
						}
						_ChangeCurrentMode(SKKMode::Henkan);
						m_currentInputKana = newxtKana;

						//FIX: 変換モードに入ったときに，候補予測ウィンドウが出ない問題の対策
					}
					else {
						std::wstring tmp;
						_GetCompositionString(tmp);
						tmp += nextinsert;
						_Output(pic, tmp, TRUE);
						_CommitComposition(pic);
					}

					return S_OK;
				}
				else {
					//変換に達していない場合は，バッファをそのまま表示
					nextinsert = m_RomajiToKanaTranslator.GetBuffer();
				}
				if (_pComposition) {
					//	nextinsert = nextChar;
					_CommitAndStartComposition(pic, nextinsert);
				}
				//_Output(pic, nextinsert, FALSE);
				return S_OK;
		//	}
		}
	}

	if (m_isRegiteringNewWord && !m_RegCurrentCandidates.empty()) {
		if (m_RegCurrentShowCandidateIndex >= BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) {
			//TODO: ASDFJKL を打ち込む時に，それ以外のものが打ち込まれたらCandidates[0]で確定	
		}
		else {
			_CommitComposition(pic);
		}
	}
	
	bool isShift = _IsShiftKeyPressed();
	//変換の開始
	if (isShift && g_currentMode == SKKMode::Kakutei) {

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
	if (g_currentMode == SKKMode::Henkan) {
		//ASDFJKL で選ぶ段階
		if (m_CurrentShowCandidateIndex >= BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX && m_isRegiteringNewWord==FALSE) {

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
               size_t selectedCandidateIndex = BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX + (m_CurrentShowCandidateIndex - BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) * NUM_SHOW_CANDIDATE_MULTIPLE + cnt;
				if (selectedCandidateIndex
					>= m_CurrentCandidates.size()
					) {
					return S_OK;
				}
               std::wstring baseword = m_CurrentCandidates[selectedCandidateIndex]_Candidate;
				std::wstring additionalStr = L"";

				if (!m_Gokan.empty() && m_OkuriganaFirstChar != L'\0') {
					additionalStr = m_currentInputKana.substr(m_Gokan.length());
				}

				//__InsertText(pic, (L"[debug]"), FALSE);
				{
					std::wstring searchKey = m_currentInputKana;
					if (m_OkuriganaFirstChar != L'\0') {
						searchKey = m_Gokan + m_OkuriganaFirstChar;
					}
                 m_SKKDictionaly.AddHistoryCandidate(searchKey, m_CurrentCandidates[selectedCandidateIndex]);
				}
				//m_SKKDictionaly.AddHistoryCandidate(m_currentInputKana, m_CurrentCandidates[BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX + (m_CurrentShowCandidateIndex - BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) * NUM_SHOW_CANDIDATE_MULTIPLE + cnt]);


				//m_SKKDictionaly.AddHistoryCandidate(m_currentInputKana);
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
		if (m_isRegiteringNewWord==TRUE && m_RegCurrentShowCandidateIndex >= BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) {

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
				//TODO: もう一方の方(Reg)も修正してら替える。
				if (BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX + (m_RegCurrentShowCandidateIndex - BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) * NUM_SHOW_CANDIDATE_MULTIPLE + cnt
					>= m_RegCurrentCandidates.size()
					) {
					return S_OK;
				}
				std::wstring baseword = m_RegCurrentCandidates[BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX + (m_RegCurrentShowCandidateIndex - BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) * NUM_SHOW_CANDIDATE_MULTIPLE + cnt]_Candidate;
				std::wstring additionalStr = L"";

				//TODO: Reg語幹を追加した時，書き直す。
				//if (!m_Gokan.empty() && m_OkuriganaFirstChar != L'\0') {
			//		additionalStr = m_currentInputKana.substr(m_Gokan.length());
			//	}

			
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

		std::wstring baseKana;
		std::wstring newkana;
		size_t prevRomajilen = 0;
		std::wstring finalText;
		std::wstring prevRomaji;

		_AddToTextonScreenAndUpdateCurrentInputKana(key, textonScreen, baseKana, newkana, prevRomaji, prevRomajilen, finalText);


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
				/*if ((newkana == L"ん" && m_CurrentKanaMode == KanaMode::Hiragana) ||
					(newkana == L"ン" && m_CurrentKanaMode == KanaMode::Katakana)) {
					m_Gokan += newkana;
					m_OkuriganaFirstChar = key;// 非n に戻す
					//ん に変換させる。

					finalText = baseKana + newkana;
					m_currentInputKana = finalText;

				}
				*/
				
				//__InsertText(pic, (L"[Debug:" + std::wstring(1, m_OkuriganaFirstChar) + L"]").c_str(), TRUE);
			}

		}

		//送り仮名が1文字以上指定された場合，確定処理  
		if (m_OkuriganaFirstChar != L'\0' && !m_Gokan.empty() &&
			m_currentInputKana.length() - m_Gokan.length() >= 1) {
			//	__InsertText(pic, L"[okuri]", TRUE);
			if (
				(
					(
						(newkana == L"っ" && g_CurrentKanaMode == KanaMode::Hiragana) ||
						(newkana == L"ッ" && g_CurrentKanaMode == KanaMode::Katakana) 
					//	(newkana == L"ん" && g_CurrentKanaMode == KanaMode::Hiragana) ||
						//(newkana == L"ン" && g_CurrentKanaMode == KanaMode::Katakana
					) &&
					m_currentInputKana.length() - m_Gokan.length() >= 1
				 )
				) {
				//"っ" が入力される時は一回待つ
			}
			else {
			//	__InsertText(pic, (L"[newkana:" + newkana + L",baseKana:" + baseKana + L"]").c_str(), TRUE);
				if (m_isRegiteringNewWord) {
					if (m_RegCurrentCandidates.empty()) {
						_HandleRegSpaceKey(pic, VK_SPACE);
					}
				}
				else {
					
					if (m_CurrentCandidates.empty()) {
						_HandleSpaceKey(pic, VK_SPACE);
					}
				}
			}
			return S_OK;
		}

		//変換履歴からの予測変換候補表示
		_SearchMostHighestPriorityCandidateWordAndVisualizePredictiveCandidateWindowFromHistory(pic);

	

		return S_OK;
	}

	if (g_currentMode == SKKMode::Kakutei) {
		std::wstring newkana;
		if (m_RomajiToKanaTranslator.Translate(key, newkana)) {
			_Output(pic, newkana, TRUE);
		}
		else {
			//変換に達していない場合は，バッファをそのまま表示
			std::wstring buf = m_RomajiToKanaTranslator.GetBuffer();
			if (!buf.empty()) {
				_Output(pic, buf, FALSE);
			}
		}
		return S_OK;
	}

	return S_OK;
}

HRESULT CSkkIme::_HandleKana(ITfContext* pic, std::wstring kanas, std::wstring& fromajis) {
	// ========================================
    // 確定モードの場合は一括出力
    // ========================================
	if (g_currentMode == SKKMode::Kakutei && !_IsShiftKeyPressed()) {
		// Shiftが押されていない確定モードでは、すべての文字を一度に出力
		_Output(pic, kanas, TRUE);
		return S_OK;
	}
	
	// ========================================
	// それ以外（変換モード、Shift押下時など）は1文字ずつ処理
	// ========================================
	for (int i = 0; i < kanas.length(); i++) {
		//_HandleCharの内容をかな向きにしていく。
		WCHAR kana = kanas[i];

		// ========================================
		// 1. 暗黙確定の処理（変換候補表示中に新しい文字が入力された場合）
		// ========================================
	// 工廠(変換中) + n(新規) => 工廠(確定) + n(変換中)  (暗黙確定)
		if (!m_isRegiteringNewWord && !m_CurrentCandidates.empty() && m_isExplictingConversionMode) {
			if (m_CurrentShowCandidateIndex >= BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) {
				//TODO: ASDFJKL を打ち込む時に，それ以外のものが打ち込まれたらCandidates[0]で確定	
			}
			else {
				{
					//現在の候補を変換履歴に追加。
					std::wstring searchKey = m_currentInputKana;
					if (m_OkuriganaFirstChar != L'\0') {
						searchKey = m_Gokan + m_OkuriganaFirstChar;
					}
					m_SKKDictionaly.AddHistoryCandidate(searchKey, m_CurrentCandidates[m_CurrentShowCandidateIndex]);
				}

				std::wstring nextinsert(1, kana);

				// Shiftが押されていたら変換モードを開始
				if (_IsShiftKeyPressed()) {
					if (_pComposition) {
						_CommitAndStartComposition(pic, nextinsert);
					}
					_ChangeCurrentMode(SKKMode::Henkan);
					m_currentInputKana = nextinsert;

					//FIX: 変換モードに入ったときに，候補予測ウィンドウが出ない問題の対策
				}
				else {
					std::wstring tmp;
					_GetCompositionString(tmp);
					tmp += nextinsert;
					_Output(pic, tmp, TRUE);
					_CommitComposition(pic);
				}

				continue;

			}
		}

		//新語登録中で候補がある場合
		if (m_isRegiteringNewWord && !m_RegCurrentCandidates.empty()) {
			if (m_RegCurrentShowCandidateIndex >= BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) {
				//TODO: ASDFJKL を打ち込む時に，それ以外のものが打ち込まれたらCandidates[0]で確定	
			}
			else {
				_CommitComposition(pic);
			}
		}

		bool isShift = _IsShiftKeyPressed();
		// ========================================
		// 2. 変換モード開始の判定（確定モードでShift押下時）
		// ========================================
		if (isShift && g_currentMode == SKKMode::Kakutei) {

			if (_pComposition) {
				if (m_pCandidateWindow->IsWindowExists()) {
					m_pCandidateWindow->HideWindow();
				}

				m_CurrentCandidates.clear();
				m_CurrentShowCandidateIndex = 0;
				m_Gokan = L"";
				m_OkuriganaFirstChar = L'\0';
			}

			_ChangeCurrentMode(SKKMode::Henkan);

			// ↑↑ 以上で_CommitComposition が終了 ↑↑

			isShift = false;
		}


		// ========================================
		// 3. 変換モードでの処理
		// ========================================
		//FIX: 書k (no shift) の場合，書 で検索する。 つまり，変換中のローマ字がシフトが押されずに確定された場合は，削除する。
		//変換中
		if (g_currentMode == SKKMode::Henkan) {
			//ASDFJKL で選ぶ段階
			//TODO: 別枠で処理をする。
			//まず未確定変換文字列画面の文字を取得 ex: しm
			std::wstring textonScreen = L"";
			_GetCompositionString(textonScreen);

			//新しいかなを追加
			std::wstring finalText = textonScreen + kana;
			// mcurrentInputKanaの更新
			if (m_isRegiteringNewWord) {
				m_RegInputUndetermined = textonScreen + kana;
			}
			else {
				m_currentInputKana = textonScreen + kana;
			}

			_Output(pic, finalText, FALSE);

			//送り仮名の指定。
			if (!textonScreen.empty() && (isShift = m_q_elftranslater.Get_kmp_is_key_pushed_shift())) {
				// 語幹が未設定で、確定済みかながあれば送り仮名モード開始
				if (m_currentInputKana.length() >= 2 && m_Gokan.empty() && m_OkuriganaFirstChar == L'\0') {
					// 語幹 = 最後の1文字を除いた部分
					m_Gokan = m_currentInputKana.substr(0, m_currentInputKana.length() - 1);
					// 送り仮名の最初の文字（かなベースだが，ローマ字に変換してその一文字目を使用。）

					m_OkuriganaFirstChar = fromajis[i];
				}
			}

			//送り仮名が1文字以上指定された場合，確定処理  
			if (m_OkuriganaFirstChar != L'\0' && !m_Gokan.empty() &&
				m_currentInputKana.length() - m_Gokan.length() >= 1) {
				//"っ" が入力される時は一回待つ
				if (!(
					(kana == L'っ' && g_CurrentKanaMode == KanaMode::Hiragana) ||
					(kana == L'ッ' && g_CurrentKanaMode == KanaMode::Katakana))
					) {

					if (m_isRegiteringNewWord) {
						if (m_RegCurrentCandidates.empty()) {
							_HandleRegSpaceKey(pic, VK_SPACE);
						}
					}
					else {
						if (m_CurrentCandidates.empty()) {
							_HandleSpaceKey(pic, VK_SPACE);
						}
					}
				}
				continue;
			}

			//変換履歴からの予測変換候補表示
			_SearchMostHighestPriorityCandidateWordAndVisualizePredictiveCandidateWindowFromHistory(pic);
			continue;
		}

		// ========================================
		// 4. 確定モードでの直接出力
		// ========================================
		if (g_currentMode == SKKMode::Kakutei) {
			_Output(pic, std::wstring(1, kana), TRUE);
			continue;
		}
	}

	// Shiftフラグをリセット
	m_q_elftranslater.Set_kmp_is_key_pushed_shift(false);

	return S_OK;
}

void CSkkIme::_AddToTextonScreenAndUpdateCurrentInputKana(WCHAR key, std::wstring& textonScreen, std::wstring& baseKana, std::wstring& newkana, std::wstring& prevRomaji, size_t& prevRomajilen, std::wstring& finalText)
{
	//前回のローマ字変換バッファ長さを取得 ex: m = 1
	//size_t 
	prevRomajilen = m_RomajiToKanaTranslator.GetBuffer().length();
	
	//std::wstring
		prevRomaji = m_RomajiToKanaTranslator.GetBuffer();

	//新しいキーを変換  ex: m + a = ま
	//std::wstring newkana;
	m_RomajiToKanaTranslator.Translate(key, newkana);

	//今の画面[しm] - ローマ字[m] = ひらがな確定済部分[し]
	//std::wstring 
		baseKana = textonScreen.substr(0, textonScreen.length() - prevRomajilen);

	//ひらがな確定済部分[し] + 新かな[ま] + 新ローマ字バッファ[]
	//std::wstring
	finalText = baseKana + newkana + m_RomajiToKanaTranslator.GetBuffer();

	m_currentInputKana = baseKana + newkana;
}

void CSkkIme::_AddToTextonScreenAndUpdateCurrentInputKana(WCHAR key,std::wstring& textOnscreen,std::wstring & finalText)
{
	std::wstring baseKana;
	std::wstring newkana;
	size_t prevRomajilen = 0;
	//std::wstring finalText;
	std::wstring prevRomaji;
	_AddToTextonScreenAndUpdateCurrentInputKana(key, textOnscreen, baseKana, newkana, prevRomaji, prevRomajilen, finalText);
}


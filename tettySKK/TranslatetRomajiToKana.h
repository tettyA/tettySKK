#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <windows.h>
#include "Global.h"

#define TrR2K_INDEX_HIRAGANA 0
#define TrR2K_INDEX_KATAKANA 1

#define MAKETRANSTABLE(romaji,hiragana,katakana) {L#romaji, {L#hiragana, L#katakana}}
#define MAKETKIGOUTRANSTABLE(keycode,jpnsymbol) { keycode, L#jpnsymbol}

class TranslatetRomajiToKana {
public:
	__declspec(noinline) TranslatetRomajiToKana() {
		m_RomajiToKana = {

#include "RomajiToKanaTransTable.h"

		};

		m_KigouToJpnKigou = {
			#include "KigouToKanaTransTable.h"
		
		};
	}
	~TranslatetRomajiToKana() {
	}
	
	//true: 変換成功  false: 変換に達していない
	ATL_NOINLINE bool TranslateKigou(WCHAR key, std::wstring& output) {
		output.clear();
		auto it = m_KigouToJpnKigou.find((int)key);
		if (it != m_KigouToJpnKigou.end()) {
			output =it->second;
			return true;
		}
		return false;
	}

	//true: 変換成功  false: 変換に達していない 
	__declspec(noinline) bool Translate(WCHAR key, std::wstring& output) {
		output.clear();

		if (m_buffer.size() == 1) {
			//促音処理
			if (key == m_buffer[0]) {
				if (key != L'n') {
					output = (g_CurrentKanaMode == KanaMode::Hiragana ? L'っ' : L'ッ');
					return true;
				}
			}
			//撥音の処理
			if (m_buffer[0] == L'n' && (key != L'n' && key != L'a' && key != 'i' && key != 'u' && key != 'e' && key != 'o' && key != 'y')) {
				output = (g_CurrentKanaMode == KanaMode::Hiragana ? L'ん' : L'ン');
				m_buffer.clear();
				m_buffer += key;
				return true;
			}
		}

		/*if (key == VK_OEM_COMMA + ToSmallAlphabet || key == VK_OEM_PERIOD + ToSmallAlphabet || key == VK_OEM_MINUS + ToSmallAlphabet) {
			//記号は即変換
			output = m_buffer;
			output += m_RomajiToKana[std::wstring(1, (wchar_t)(key-(ToSmallAlphabet)))][mode == KanaMode::Hiragana ? TrR2K_INDEX_HIRAGANA : TrR2K_INDEX_KATAKANA];
			m_buffer.clear();
			return true;
		}*/
		{
			auto it = m_KigouToJpnKigou.find((int)key);
			if ((!__isAlphabet(key)) && (it != m_KigouToJpnKigou.end())) {
				output = m_buffer;
				output += it->second[key];
				m_buffer.clear();
				return true;
			}
		}

		m_buffer += key;
		auto it = m_RomajiToKana.find(m_buffer);
		if (it != m_RomajiToKana.end()) {
			output = (*it).second[g_CurrentKanaMode == KanaMode::Hiragana ? TrR2K_INDEX_HIRAGANA : TrR2K_INDEX_KATAKANA];
			m_buffer.clear();
			return true;
		}



		//shaなどはこの前ので出るのでOK
		if (m_buffer.size() >= 3) {
			output = m_buffer;
			m_buffer.clear();
			return true;
		}
		return false;
	}

	//かなの変換に達していない場合のバッファを取得 2文字以下であることが証明される。(かなに変換された場合は，バッファがクリアされるため)
	std::wstring GetBuffer() const {
		return m_buffer;
	}
	
	bool isMustNOTExecuteKigou(WCHAR key) const {
		auto it = m_KigouToJpnKigou.find((int)key);
		if ((!__isAlphabet(key)) && (it != m_KigouToJpnKigou.end())) {
			return true;
		}
		return false;
	}

	void Reset() {
		m_buffer.clear();
	}
	void PopBackBuffer() {
		if (!m_buffer.empty()) {
			m_buffer.pop_back();
		}
	}



private:
	std::wstring m_buffer;
	std::unordered_map<std::wstring, std::vector<std::wstring>> m_RomajiToKana;
	std::unordered_map<int, std::wstring>m_KigouToJpnKigou;
};
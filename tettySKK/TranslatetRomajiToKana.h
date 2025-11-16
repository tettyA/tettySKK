#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <windows.h>

#define TrR2K_INDEX_HIRAGANA 0
#define TrR2K_INDEX_KATAKANA 1

#define MAKETRANSTABLE(romaji,hiragana,katakana) {L#romaji, {L#hiragana, L#katakana}}

class TranslatetRomajiToKana {
public:
	__declspec(noinline) TranslatetRomajiToKana() {
		m_RomajiToKana = {
#include "RomajiToKanaTransTable.txt"
		};

	}
	~TranslatetRomajiToKana() {
	}

	//true: 変換成功  false: 変換に達していない 
	__declspec(noinline) bool Translate(WCHAR key, std::wstring& output) {
		output.clear();

		m_buffer += key;
		auto it = m_RomajiToKana.find(m_buffer);
		if (it != m_RomajiToKana.end()) {
			output = (*it).second[TrR2K_INDEX_HIRAGANA];
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

	void Reset() {
		m_buffer.clear();
	}

private:
	std::wstring m_buffer;
	std::unordered_map<std::wstring, std::vector<std::wstring>> m_RomajiToKana;
};
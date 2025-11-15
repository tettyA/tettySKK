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
	TranslatetRomajiToKana() {
		m_RomajiToKana = {
#include "RomajiToKanaTransTable.txt"
		};

	}
	~TranslatetRomajiToKana() {
	}

	//true: •ÏŠ·¬Œ÷  false: •ÏŠ·‚É’B‚µ‚Ä‚¢‚È‚¢ 
	bool Translate(WCHAR key, std::wstring& output) {
		output.clear();

		m_buffer += key;
		auto it = m_RomajiToKana.find(m_buffer);
		if (it != m_RomajiToKana.end()) {
			output = (*it).second[TrR2K_INDEX_HIRAGANA];
			m_buffer.clear();
			return true;
		}
		
		//sha‚È‚Ç‚Í‚±‚Ì‘O‚Ì‚Åo‚é‚Ì‚ÅOK
		if (m_buffer.size() >= 3) {
			output = m_buffer;
			m_buffer.clear();
			return true;
		}
		return false;
	}

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
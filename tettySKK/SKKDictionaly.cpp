#include "pch.h"
#include "SKKDictionaly.h"
#include "Global.h"

#include <sstream>



CSKKDictionaly::CSKKDictionaly()
{
	
}

CSKKDictionaly::~CSKKDictionaly()
{
}

void CSKKDictionaly::GetCandidates(std::wstring& key, SKKCandidates& candidates) const
{
	_ConvertToHiragana(key);
	auto it = m_dictionary.find(key);
	if (it != m_dictionary.end()) {
		candidates = it->second;
	}
}

BOOL CSKKDictionaly::LoadDictionaryFromFile(const std::wstring& filepath)
{
	m_dictionary.clear();
	
	std::ifstream file(filepath, std::ios::binary);
	if (!file.is_open()) {
		return FALSE;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty() || line[0] == ';') {
			continue; 
		}

		//文字コードの変換
		std::wstring linew;
		int len = MultiByteToWideChar(EUC_JP_CODEPAGE, 0, line.c_str(), line.length(), nullptr, 0);
		if (len > 0) {
			linew.resize(len);
			MultiByteToWideChar(EUC_JP_CODEPAGE, 0, line.c_str(), line.length(), &linew[0], len);
		}


		//辞書解析
		/*
		* 構文
		* 
		* [ひらがな] /候補1/候補2/候補3/...
		* 候補の中に;がある場合，その後は解説的なもの
		*/
		

		size_t spacePos = linew.find(L' ');
		if (spacePos == std::wstring::npos) {
			continue; 
		}

		std::wstring key = linew.substr(0, spacePos);
		std::wstring candidateStr = linew.substr(spacePos + 1);
		SKKCandidates candidateslist;

		std::wstringstream ss(candidateStr);
		std::wstring tmpcandidate;
		while (std::getline(ss, tmpcandidate, L'/')) {
			if (tmpcandidate.empty()) continue;

			if (tmpcandidate.find(SKK_CANDIDOTATES_ANNOTATION_SEPARATOR_CHAR) != std::wstring::npos) {
				size_t sepPos = tmpcandidate.find(SKK_CANDIDOTATES_ANNOTATION_SEPARATOR_CHAR);
				std::wstring candidate = tmpcandidate.substr(0, sepPos);
				std::wstring annotation = tmpcandidate.substr(sepPos + 1);
				candidateslist.push_back(std::make_pair(candidate, annotation));
			}
			else {
				candidateslist.push_back(std::make_pair(tmpcandidate, L""));
			}

		}
		if (!candidateslist.empty()) {
			m_dictionary[key] = candidateslist;
		}
	

	}
	return TRUE;
}


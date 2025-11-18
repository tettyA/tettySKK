#include "pch.h"
#include "SKKDictionaly.h"
#include <sstream>



CSKKDictionaly::CSKKDictionaly()
{
	
}

CSKKDictionaly::~CSKKDictionaly()
{
}

void CSKKDictionaly::GetCandidates(const std::wstring& key, SKKCandidates& candidates) const
{
	auto it = m_dictionary.find(key);
	if (it != m_dictionary.end()) {
		candidates = it->second;
	}
}

BOOL CSKKDictionaly::LoadDictionaryFromFile(const std::wstring& filepath)
{
	m_dictionary.clear();
	m_dictionary[L"あのめ"] = SKKCandidates{ L"test1",L"test2", L"test3",L"test4", L"test5",L"test6", L"test7",L"test8", L"test9",L"test10", L"test11",L"test12",L"text13"};
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
		SKKCandidate candidate;
		while (std::getline(ss, candidate, L'/')) {
			if (candidate.empty()) continue;

			//; は一旦無視 
			//TODO: 無視しない

			candidateslist.push_back(candidate);
		}
		if (!candidateslist.empty()) {
			m_dictionary[key] = candidateslist;
		}
	

	}
	return TRUE;
}


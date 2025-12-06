#include "pch.h"
#include "SKKDictionaly.h"
#include "Global.h"

#include <sstream>
#include <fstream>



CSKKDictionaly::CSKKDictionaly()
{
	m_dictionary = SKKDictionary();
	m_userdictionary = SKKDictionary();
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
	auto uit = m_userdictionary.find(key);
	if (uit != m_userdictionary.end()) {
		//ユーザ辞書の方を後ろに追加
		candidates.insert(candidates.begin(), uit->second.begin(), uit->second.end());
	}
	//TODO: ユーザ辞書と通常辞書の重複排除(変換履歴により生じる)
}

void CSKKDictionaly::AddCandidate(const std::wstring& key, const std::wstring& candidate)
{

	if (candidate.find(SKK_CANDIDOTATES_ANNOTATION_SEPARATOR_STR) != std::wstring::npos) {
		//;が含まれている場合，分割する
		size_t sepPos = candidate.find(SKK_CANDIDOTATES_ANNOTATION_SEPARATOR_STR);
		std::wstring cand = candidate.substr(0, sepPos);
		std::wstring annotation = candidate.substr(sepPos + 1);
		m_userdictionary[key].push_back(std::make_pair(cand, annotation));
		return;
	}
	else
		m_userdictionary[key].push_back(std::make_pair(candidate, L""));
	//TODO: 重複チェック

}

BOOL CSKKDictionaly::LoadDictionaryFromFile(const std::wstring& filepath, const std::wstring& userfilepath)
{
	_LoadDictionaryFromFile(filepath, m_dictionary);
	_LoadDictionaryFromFile(userfilepath, m_userdictionary);

	for (auto& [wkey,wcandidates] : m_userdictionary)
	{
		for (auto& wcandidate : wcandidates) {
			for (auto it = m_dictionary[wkey].begin(); it != m_dictionary[wkey].end(); ++it)
			{
				if ((*it)  == wcandidate) {
					//重複している場合，通常辞書側を削除
					m_dictionary[wkey].erase(it);
					
					break;
				}
			}
		}
	}
	return TRUE;
}

/*
BOOL CSKKDictionaly::LoadUserDictionaryFromFile(const std::wstring& filepath)
{
	return _LoadDictionaryFromFile(filepath, m_userdictionary);
}*/

BOOL CSKKDictionaly::SaveDictionaryToUserFile(const std::wstring& filepath) const
{
	//EUC-JPで保存する
	std::ofstream file(filepath, std::ios::binary);

	if (!file.is_open()) {
		return FALSE;
	}

	for (const auto& [wkey, candidates] : m_userdictionary) {
		//文字コードの変換
		std::string key;
		{
			int len = WideCharToMultiByte(EUC_JP_CODEPAGE, 0, wkey.c_str(), wkey.length(), nullptr, 0, nullptr, 0);
			if (len > 0) {
				key.resize(len);
				WideCharToMultiByte(EUC_JP_CODEPAGE, 0, wkey.c_str(), wkey.length(), &key[0], len, nullptr, 0);
			}
		}

		file << key << " ";

		for (const auto& [wword, wannotation] : candidates) {
			//文字コードの変換
			std::string word;
			{
				int len = WideCharToMultiByte(EUC_JP_CODEPAGE, 0, wword.c_str(), wword.length(), nullptr, 0, nullptr, 0);
				if (len > 0) {
					word.resize(len);
					WideCharToMultiByte(EUC_JP_CODEPAGE, 0, wword.c_str(), wword.length(), &word[0], len, nullptr, 0);
				}
			}
			std::string annotation;
			if (!wannotation.empty()) {
				int len = WideCharToMultiByte(EUC_JP_CODEPAGE, 0, wannotation.c_str(), wannotation.length(), nullptr, 0, nullptr, 0);
				if (len > 0) {
					annotation.resize(len);
					WideCharToMultiByte(EUC_JP_CODEPAGE, 0, wannotation.c_str(), wannotation.length(), &annotation[0], len, nullptr, 0);
				}
			}

			file << "/" << word;
			if (!annotation.empty()) {
				file << SKK_CANDIDOTATES_ANNOTATION_SEPARATOR_ASTR;
				file << annotation;
			}
		}
		file << "/" << std::endl;
	}

	file.close();

	return TRUE;
}

void CSKKDictionaly::GetPredictionCandidate(std::wstring& prefix, SKKCandidate& candidate) const
{
	_ConvertToHiragana(prefix);
	candidate = SKKCandidate(L"", L"");
	for (const auto& e : m_history)
	{
		if (e.starts_with(prefix)) {
			auto uit=m_userdictionary.find(e);
			if( uit!= m_userdictionary.end() &&!uit->second.empty())
			{
				candidate = uit->second[0];
				return;
			}
			else {
				auto it = m_dictionary.find(e);
				if (it != m_dictionary.end() && !it->second.empty()) {
					candidate = it->second[0];
					return;
				}
			}
			return;
		}
	}

	auto uit = m_userdictionary.lower_bound(prefix);
	if (uit != m_userdictionary.end()) {
		if (uit->first.starts_with(prefix)) {
			if (!uit->second.empty()) {
				candidate = uit->second[0];
				return;
			}
		}
	}

	auto it = m_dictionary.lower_bound(prefix);
	if (it != m_dictionary.end()) {
		if (it->first.starts_with(prefix)) {
			if (!it->second.empty()) {
				candidate = it->second[0];
				return;
			}
		}
	}

	return;
}

//TODO: user辞書が変更されるので，保存をするか考える。
void CSKKDictionaly::AddHistoryCandidate(const std::wstring& yomi, const SKKCandidate& candidate)
{
	//重複排除
	for (auto it = m_history.begin(); it != m_history.end(); ++it) {
		if ((*it) == yomi ) {
			m_history.erase(it);
			break;
		}
	}

	m_history.push_front(yomi);
	if (m_history.size() > MAX_SKK_DICTIONARY_HISTORY) {
		m_history.pop_back();
	}
	
	for (auto it = m_userdictionary[yomi].begin(); it != m_userdictionary[yomi].end(); ++it)
	{
		if ((*it)_Candidate == candidate _Candidate) {
			m_userdictionary[yomi].erase(it);
			break;
		}
	}
	m_userdictionary[yomi].push_front(candidate);

	//通常辞書からも削除(重複の排除)
	for (auto it = m_dictionary[yomi].begin(); it != m_dictionary[yomi].end(); ++it)
	{
		if ((*it)_Candidate == candidate _Candidate) {
			m_dictionary[yomi].erase(it);
			break;
		}
	}
}

BOOL CSKKDictionaly::_LoadDictionaryFromFile(const std::wstring& filepath, SKKDictionary& _dict)
{

	if (_dict.size() > 0)return TRUE;

	_dict.clear();

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
			_dict[key] = candidateslist;
		}


	}
	return TRUE;
}

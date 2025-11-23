#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <utility>


#define _Candidate .first
#define _Annotation .second

#define SKK_CANDIDOTATES_ANNOTATION_SEPARATOR_STR L";"
#define SKK_CANDIDOTATES_ANNOTATION_SEPARATOR_CHAR L';'

constexpr std::wstring SKK_CHOOSE_CANDIDATES_BIGSTR = L"ASDFJKL";
constexpr std::wstring SKK_CHOOSE_CANDIDATES_SMLSTR = L"asdfjkl";

using SKKCandidate = std::pair<std::wstring, std::wstring>;//一つの候補文字列，註釈
using SKKCandidates = std::vector<SKKCandidate>;//候補文字列群
using SKKDictionary = std::map<std::wstring, SKKCandidates>;//読み，候補文字列群

//TODO: 辞書ファイルパスの設定方法を考える
#define SKK_DICTIONARY_FILEPATH LR"(D:\SKK-JISYO.L)"
#define EUC_JP_CODEPAGE 20932

class CSKKDictionaly
{
public:
	CSKKDictionaly();
	~CSKKDictionaly();

	//副作用:keyがカタカナの場合ひらがなに変更されます。
	void GetCandidates(std::wstring& key, SKKCandidates& candidates) const;

	void AddCandidate(const std::wstring& key, const std::wstring& candidate) {
		m_dictionary[key].push_back(std::make_pair( candidate, L"" ));
		//TODO: 重複チェック & 辞書に書き込み & ;確認
	}

	BOOL LoadDictionaryFromFile(const std::wstring& filepath);

private:
	SKKDictionary m_dictionary;
};


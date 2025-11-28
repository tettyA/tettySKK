#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <utility>


#define _Candidate .first
#define _Annotation .second

#define SKK_CANDIDOTATES_ANNOTATION_SEPARATOR_STR L";"
#define SKK_CANDIDOTATES_ANNOTATION_SEPARATOR_ASTR ";"
#define SKK_CANDIDOTATES_ANNOTATION_SEPARATOR_CHAR L';'

constexpr std::wstring SKK_CHOOSE_CANDIDATES_BIGSTR = L"ASDFJKL";
constexpr std::wstring SKK_CHOOSE_CANDIDATES_SMLSTR = L"asdfjkl";

using SKKCandidate = std::pair<std::wstring, std::wstring>;//一つの候補文字列，註釈
using SKKCandidates = std::vector<SKKCandidate>;//候補文字列群
using SKKDictionary = std::map<std::wstring, SKKCandidates>;//読み，候補文字列群

//TODO: 辞書ファイルパスの設定方法を考える
#define SKK_DICTIONARY_FILEPATH LR"(D:\SKK-JISYO.L)"
#define SKK_USER_DICTIONARY_FILEPATH LR"(D:\SKK-JISYO.USER.L)"

#define EUC_JP_CODEPAGE 20932

class CSKKDictionaly
{
public:
	CSKKDictionaly();
	~CSKKDictionaly();

	//副作用:keyがカタカナの場合ひらがなに変更されます。
	void GetCandidates(std::wstring& key, SKKCandidates& candidates) const;

	//辞書の書き込みは行いません。メモリ上のみ追加されます。
	void AddCandidate(const std::wstring& key, const std::wstring& candidate);

	BOOL LoadDictionaryFromFile(const std::wstring& filepath);
	BOOL LoadUserDictionaryFromFile(const std::wstring& filepath);
	BOOL SaveDictionaryToUserFile(const std::wstring& filepath) const;

private:
	SKKDictionary m_dictionary;
	SKKDictionary m_userdictionary;
	BOOL _LoadDictionaryFromFile(const std::wstring& filepath, SKKDictionary& _dict);
};


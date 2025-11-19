#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>

using SKKCandidate = std::wstring;//一つの候補文字列
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

	BOOL LoadDictionaryFromFile(const std::wstring& filepath);

private:
	SKKDictionary m_dictionary;
};


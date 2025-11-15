#pragma once
#include <string>
#include <vector>
#include <map>

using SKKCandidate = std::wstring;//ˆê‚Â‚ÌŒó•â•¶š—ñ
using SKKCandidates = std::vector<SKKCandidate>;//Œó•â•¶š—ñŒQ
using SKKDictionary = std::map<std::wstring, SKKCandidates>;//“Ç‚İCŒó•â•¶š—ñŒQ

class CSKKDictionaly
{
public:
	CSKKDictionaly();
	~CSKKDictionaly();

	void GetCandidates(const std::wstring& key, SKKCandidates& candidates) const;

private:
	SKKDictionary m_dictionary;
};


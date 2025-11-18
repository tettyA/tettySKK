#include "pch.h"
#include "SKKDictionaly.h"


CSKKDictionaly::CSKKDictionaly()
{
	m_dictionary[L"‚ "] = { L"ˆŸ", L"ˆ¢",L"Œá" };
	m_dictionary[L"‚©"] = {  L"‰Á", L"‰á" ,L"‰Â",L"‰¿" };
	m_dictionary[L"‚³‚¢"] = { L"Ù", L"×", L"Ë", L"·ˆÙ" };
	m_dictionary[L"‚·‚¢"] = { L"…",L"ˆ",L"",L"Ž",L"†",L"„",L"‰",};
	m_dictionary[L"‚±‚¤‚µ‚å‚¤"] = { L"ŒðÂ",L"ŒöÌ",L"‚®",L"H±" };
	m_dictionary[L"‚©k"] = { L"‘",L"Œ‡",L"Š|",L"‹ì" };//Š|‚­C‹ì‚¯‚éC‘‚­CŒ‡‚­...
	m_dictionary[L"‚©‚©"] = { L"›m",L"‰Ô‰º",L"‰¼‰Ê" };
	m_dictionary[L"‚©‚«"] = { L"Š`",L"‰º‹L",L"‰Ä‹G",L"‰²åy" };
	m_dictionary[L"‚©‚­"] = { L"Šp",L"Še",L"Ši",L"Šj",L"Št",L"Žz‚­"};
	m_dictionary[L"‚½b"] = { L"H" };
	m_dictionary[L"‚©‚¯"] = { L"“q‚¯" ,L"‰¿‹C"};
	m_dictionary[L"‚©‚±"] = { L"‰ß‹Ž",L"‰ÁŒÃ",L"‰ÕŒÎ" };
	m_dictionary[L"‚¢r"] = { L"“ü",L"ŽË",L"—v",L"‹",L"àu",L"’’",L"Ÿ¹" };
	m_dictionary[L"‚µ‚ås"] = { L"ˆ",L"‘",L"" };
	
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


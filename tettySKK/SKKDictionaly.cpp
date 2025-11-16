#include "pch.h"
#include "SKKDictionaly.h"


CSKKDictionaly::CSKKDictionaly()
{
	m_dictionary[L"‚ "] = { L"ˆŸ", L"ˆ¢",L"Œá" };
	m_dictionary[L"‚©"] = {  L"‰Á", L"‰á" ,L"‰Â",L"‰¿" };
	m_dictionary[L"‚³‚¢"] = { L"Ù", L"×", L"Ë", L"·ˆÙ" };
	m_dictionary[L"‚·‚¢"] = { L"…",L"ˆ",L"",L"Ž",L"†",L"„",L"‰",};
	m_dictionary[L"‚±‚¤‚µ‚å‚¤"] = { L"ŒðÂ",L"ŒöÌ",L"‚®",L"H±" };
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


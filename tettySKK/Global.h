#pragma once

#include <string>

extern HMODULE g_hModule;
//SKK IME 本体のCLSID
// {C021919B-A5A1-463E-A1BF-C71F7B6313A2}
DEFINE_GUID(CLSID_tettySKK,
	0xc021919b, 0xa5a1, 0x463e, 0xa1, 0xbf, 0xc7, 0x1f, 0x7b, 0x63, 0x13, 0xa2);

//プロファイルを識別するGUID
// {96D3865F-EF38-4ED3-BC25-E85B87D05016}
DEFINE_GUID(GUID_Profile_SKKIme,
	0x96d3865f, 0xef38, 0x4ed3, 0xbc, 0x25, 0xe8, 0x5b, 0x87, 0xd0, 0x50, 0x16);

//カテゴリ識別GUID
// {B7D48160-7F03-42FB-828B-DDAD9A4965D2}
DEFINE_GUID(GUID_Category_SkkIme,
	0xb7d48160, 0x7f03, 0x42fb, 0x82, 0x8b, 0xdd, 0xad, 0x9a, 0x49, 0x65, 0xd2);

//入力中の文字装飾用GUID
// {BC24D976-911F-49AF-9C50-82C6269EAA9E}
DEFINE_GUID(GUID_Skk_DisplayAttirbute_Input ,
	0xbc24d976, 0x911f, 0x49af, 0x9c, 0x50, 0x82, 0xc6, 0x26, 0x9e, 0xaa, 0x9e);

//言語バーアイテム識別用GUID
// {F1F04704-463D-4E47-94DD-721D7649E256}
DEFINE_GUID(GUID_LangBarItem_SkkIme ,
	0xf1f04704, 0x463d, 0x4e47, 0x94, 0xdd, 0x72, 0x1d, 0x76, 0x49, 0xe2, 0x56);


#define gtettySKK_IME_NAME L"Hyper Great tettySKK IME"



LONG DllAddRef(void);
LONG DllRelease(void);

void _ConvertToKatakana(std::wstring& str);
void _ConvertToHiragana(std::wstring& str);

enum class SKKMode {
	Kakutei,
	Henkan,
	Hankaku,
};

enum class KanaMode {
	Hiragana,
	Katakana
};

#pragma once
#include "pch.h"

extern HMODULE g_hModule;

//SKK IME 本体のCLSID
// {C021919B-A5A1-463E-A1BF-C71F7B6313A2}
DEFINE_GUID(CLSID_tettySKK ,
	0xc021919b, 0xa5a1, 0x463e, 0xa1, 0xbf, 0xc7, 0x1f, 0x7b, 0x63, 0x13, 0xa2);

//プロファイルを識別するGUID
// {96D3865F-EF38-4ED3-BC25-E85B87D05016}
DEFINE_GUID(GUID_Profile_SKKIme ,
	0x96d3865f, 0xef38, 0x4ed3, 0xbc, 0x25, 0xe8, 0x5b, 0x87, 0xd0, 0x50, 0x16);

//カテゴリ識別GUID
// {B7D48160-7F03-42FB-828B-DDAD9A4965D2}
DEFINE_GUID(GUID_Category_SkkIme,
	0xb7d48160, 0x7f03, 0x42fb, 0x82, 0x8b, 0xdd, 0xad, 0x9a, 0x49, 0x65, 0xd2);

//レジストリ登録用の補助関数
BOOL SetRegistryValue(
	HKEY hKeyRoot,
	LPCWSTR lpSubKey,
	LPCWSTR lpValueName,
	DWORD dwType,
	CONST BYTE* lpData,
	DWORD cbData
);
BOOL DeleteRegistryKey(
	HKEY hKeyRoot,
	LPCWSTR lpSubKey
);

#define CLSIDLEN 39
#define tettySKK_IME_NAME L"tettySKK IME"
#define _INPROCSERVER32STR L"InprocServer32"
#define _THREADINGMODELSTR L"ThreadingModel"
#define APARTMENTSTR L"Apartment"
#define CLSIDSTR_ L"CLSID"

#define TEXTSURVICE_LANGID MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT)
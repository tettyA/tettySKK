#include "pch.h"
#include "Global.h"
#include <strsafe.h>
#include <atlbase.h>

#define CLSIDLEN 39

#define _INPROCSERVER32STR L"InprocServer32"
#define _THREADINGMODELSTR L"ThreadingModel"
#define APARTMENTSTR L"Apartment"
#define CLSIDSTR_ L"CLSID"

#define TEXTSURVICE_LANGID MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT)

BOOL SetRegistryValue(
	HKEY hKeyRoot,
	LPCWSTR lpSubKey,
	LPCWSTR lpValueName,
	DWORD dwType,
	CONST BYTE* lpData,
	DWORD cbData
)
{
	HKEY hKey;
	if (RegCreateKeyEx(hKeyRoot, lpSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL)) {
		return FALSE;
	}
	BOOL bResult = (RegSetValueEx(hKey, lpValueName, 0, dwType, lpData, cbData) == ERROR_SUCCESS);
	RegCloseKey(hKey);

	return bResult;
}

//FIX:サブキーもろとも削除される！！
BOOL DeleteRegistryKey(HKEY hKeyRoot, LPCWSTR lpSubKey)
{
	return (RegDeleteKey(hKeyRoot, lpSubKey) == ERROR_SUCCESS);
}

//ref https://github.com/nathancorvussolis/corvusskk/blob/2904b3ad7ba80e66e717aef6805164c74fcec71d/imcrvtip/Register.cpp#L49
STDAPI DllRegisterServer(void) {
	HRESULT hr = E_FAIL;
	WCHAR szClsidString[CLSIDLEN];
	WCHAR szDllPath[MAX_PATH];
	WCHAR szRegKeyPath[256];

	//CLSIDを文字列に変換
	if (!StringFromGUID2(CLSID_tettySKK, szClsidString, ARRAYSIZE(szClsidString))) {
		return E_FAIL;
	}

	//HKEY_CLASSES_ROOT/CLSID/{XXX....}にキーを作成
	StringCchPrintf(szRegKeyPath, ARRAYSIZE(szRegKeyPath), CLSIDSTR_ L"\\%s", szClsidString);
	if (!SetRegistryValue(HKEY_CLASSES_ROOT, szRegKeyPath, NULL, REG_SZ, (const BYTE*)gtettySKK_IME_NAME, (wcslen(gtettySKK_IME_NAME) + 1) * sizeof(WCHAR))) {
		return E_FAIL;
	}

	//InprocServer32サブキーにこのDLLのフルパスを書き込む
	StringCchPrintf(szRegKeyPath, ARRAYSIZE(szRegKeyPath), CLSIDSTR_ L"\\%s\\" _INPROCSERVER32STR, szClsidString);
	GetModuleFileName(g_hModule, szDllPath, ARRAYSIZE(szDllPath));
	if (!SetRegistryValue(HKEY_CLASSES_ROOT, szRegKeyPath, NULL, REG_SZ, (const BYTE*)szDllPath, (wcslen(szDllPath) + 1) * sizeof(WCHAR))) {
		return E_FAIL;
	}

	//スレッドモデルをApartmentとして書き込む。
	if (!SetRegistryValue(HKEY_CLASSES_ROOT, szRegKeyPath, _THREADINGMODELSTR, REG_SZ, (const BYTE*)APARTMENTSTR, (wcslen(APARTMENTSTR) + 1) * sizeof(WCHAR))) {
		return E_FAIL;
	}


	//Register Profiles
	//ref https://github.com/nathancorvussolis/corvusskk/blob/2904b3ad7ba80e66e717aef6805164c74fcec71d/imcrvtip/Register.cpp#L35
	//TSFにIMEとして登録

	CoInitialize(NULL);

	//プロファイルの登録
	{
		CComPtr<ITfInputProcessorProfileMgr> pProfileMgr;
		hr = pProfileMgr.CoCreateInstance(CLSID_TF_InputProcessorProfiles);

		if (SUCCEEDED(hr) && pProfileMgr != nullptr) {
			hr = pProfileMgr->RegisterProfile(
				CLSID_tettySKK,
				TEXTSURVICE_LANGID,
				GUID_Profile_SKKIme,
				gtettySKK_IME_NAME,
				(ULONG)wcslen(gtettySKK_IME_NAME),
				NULL, 0,//TODO:アイコンを追加する。
				NULL,
				nullptr, 0, TRUE, 0
			);
		}
		if (FAILED(hr))return hr;
	}

	//カテゴリの登録
	{
		CComPtr<ITfCategoryMgr> pCategoryMgr;
		HRESULT hrCat = pCategoryMgr.CoCreateInstance(CLSID_TF_CategoryMgr);
		if (SUCCEEDED(hrCat) && pCategoryMgr != nullptr) {
			pCategoryMgr->RegisterCategory(
				CLSID_tettySKK,
				GUID_TFCAT_TIP_KEYBOARD,
				CLSID_tettySKK
			);

			//TODO:GUID_TFCAT_DISPLAYATTERBUTEPROVIDER なども登録する！(削除も忘れないように)
		}
	}

	return hr;
}

STDAPI DllUnregisterServer(void) {
	WCHAR szClsidString[CLSIDLEN];
	WCHAR szRegKeyPath[MAX_PATH];

	if (!StringFromGUID2(CLSID_tettySKK, szClsidString, ARRAYSIZE(szClsidString))) {
		return E_FAIL;
	}

	//レジストリ削除
	StringCchPrintf(szRegKeyPath, ARRAYSIZE(szRegKeyPath), CLSIDSTR_ L"\\%s\\" _INPROCSERVER32STR, szClsidString);
	DeleteRegistryKey(HKEY_CLASSES_ROOT, szRegKeyPath);

	StringCchPrintf(szRegKeyPath, ARRAYSIZE(szRegKeyPath), CLSIDSTR_ L"\\%s\\", szClsidString);
	DeleteRegistryKey(HKEY_CLASSES_ROOT, szRegKeyPath);

	//プロファイルの削除
	{
		CComPtr<ITfInputProcessorProfileMgr> pProfileMgr;
		if (SUCCEEDED(pProfileMgr.CoCreateInstance(CLSID_TF_InputProcessorProfiles))) {
			pProfileMgr->UnregisterProfile(
				CLSID_tettySKK,
				TEXTSURVICE_LANGID,
				GUID_Profile_SKKIme,
				TF_URP_ALLPROFILES
			);
		}
	}
	//カテゴリの削除
	{
		CComPtr<ITfCategoryMgr> pCategoryMgr;
		if (SUCCEEDED(pCategoryMgr.CoCreateInstance(CLSID_TF_CategoryMgr))) {
			pCategoryMgr->UnregisterCategory(
				CLSID_tettySKK,
				GUID_TFCAT_TIP_KEYBOARD,
				CLSID_tettySKK
				//TODO:GUID_TFCAT_DISPLAYATTERBUTERPROVIDERなども登録した場合，これも削除する！
			);
		}
	}

	return S_OK;
}
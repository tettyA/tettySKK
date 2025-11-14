#pragma once
#pragma once
#include "pch.h"



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


//ref https://github.com/nathancorvussolis/corvusskk/blob/2904b3ad7ba80e66e717aef6805164c74fcec71d/imcrvtip/Server.cpp


class CSkkImeFactory : public IClassFactory
{
public:
	//IUnknown 
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	//IClassFactory
	STDMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject);
	STDMETHODIMP LockServer(BOOL fLock);

};

class CSkkIme;
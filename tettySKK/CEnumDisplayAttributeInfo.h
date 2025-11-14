#pragma once
#include <msctf.h>

//ref https://github.com/nathancorvussolis/corvusskk/blob/2904b3ad7ba80e66e717aef6805164c74fcec71d/imcrvtip/EnumDisplayAttributeInfo.h

#define ENUM_DISPLAYATTRIBUTE_COUNT 1

class CEnumDisplayAttributeInfo :
    public IEnumTfDisplayAttributeInfo
{
public:
    CEnumDisplayAttributeInfo();
    ~CEnumDisplayAttributeInfo();

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObj);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    // IEnumTfDisplayAttributeInfo
    STDMETHODIMP Clone(IEnumTfDisplayAttributeInfo** ppEnum);
    STDMETHODIMP Next(ULONG ulCount, ITfDisplayAttributeInfo** rgInfo, ULONG* pcFetched);
    STDMETHODIMP Reset(void);
	STDMETHODIMP Skip(ULONG ulCount);

private:
    LONG _refCount;
	ULONG _nIndex;
};


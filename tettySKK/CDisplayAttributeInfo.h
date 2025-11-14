#pragma once
#include <msctf.h>
//ï∂éöëïè¸ÇTSFÇ…ã≥Ç¶ÇÈÇΩÇﬂÇÃÉNÉâÉX
class CDisplayAttributeInfo :
    public ITfDisplayAttributeInfo
{
public:
    CDisplayAttributeInfo();
    ~CDisplayAttributeInfo();

    //IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObj);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    //ITfDisplayAttributeInfo
    STDMETHODIMP GetGUID(GUID* pguid);
    STDMETHODIMP GetDescription(BSTR* pbstrDesc);
    STDMETHODIMP GetAttributeInfo(TF_DISPLAYATTRIBUTE* pda);
    STDMETHODIMP SetAttributeInfo(const TF_DISPLAYATTRIBUTE* pda);
    STDMETHODIMP Reset();

private:
    LONG _refCount;

};


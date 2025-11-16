#pragma once
#include <msctf.h>

//EditSession‚ÌŠî’êƒNƒ‰ƒX
class CEditSessionBase :
    public ITfEditSession
{
public:
	CEditSessionBase();
	~CEditSessionBase();
	//IUnknown methods
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	//ITfEditSession methods
	virtual STDMETHODIMP DoEditSession(TfEditCookie ec) override = 0;

protected:
	LONG _refCount;
};


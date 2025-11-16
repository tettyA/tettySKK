#pragma once
#include <msctf.h>
#include "CSkkIme.h"
#include <string>
#include "CEditSessionBase.h"

//ref https://github.com/nathancorvussolis/corvusskk/blob/2904b3ad7ba80e66e717aef6805164c74fcec71d/imcrvtip/EditSession.h#L6
//テキスト挿入のためのEditSession
class CInsertTextEditSession :
	public  CEditSessionBase
{
public:
	__declspec(noinline) CInsertTextEditSession(CSkkIme* pIme, ITfContext* pContext, const WCHAR* text, BOOL isDetermined) {
		_pIme = pIme;//明示的なAddRefは不要 (CComPtrがやってくれる)
		_pContext = pContext;
		_isDetermined = isDetermined;

		_text = text;
	}

	__declspec(noinline) ~CInsertTextEditSession() {
		_pContext.Release();
		_pIme.Release();
	}

	//ITfEditSession methods
	STDMETHODIMP DoEditSession(TfEditCookie ec)override {
		return _pIme->_DoInsertText(ec, _pContext, _text.c_str(), _isDetermined);
	}

private:	
	CComPtr<CSkkIme> _pIme;
	CComPtr<ITfContext> _pContext;
	std::wstring _text;

	//確定したか否か?
	BOOL _isDetermined;
};

//Compositionのテキストを取得するためのEditSession
class CGetTextEditSession :public CEditSessionBase
{
public:
	CGetTextEditSession(ITfRange* pRange, std::wstring* pOutput) {
		_pRange = pRange;
		_pOutput = pOutput;
	}

	~CGetTextEditSession() {
		_pRange.Release();
		_pOutput = nullptr;
	}

	//ITfEditSession methods
	__declspec(noinline) STDMETHODIMP DoEditSession(TfEditCookie ec) override{
		WCHAR buffer[512];
		ULONG cchFetched = 0;

		if (SUCCEEDED(_pRange->GetText(ec, 0, buffer, 512, &cchFetched)))
		{
			if (cchFetched > 0) {
				_pOutput->assign(buffer, cchFetched);
			}
		}

		return S_OK;
	}

private:
	CComPtr<ITfRange> _pRange;
	std::wstring* _pOutput;
};

//Compositionを強制終了させるためのEditSession
class CTerminateCompositionEditSession :public CEditSessionBase
{
public:
	CTerminateCompositionEditSession(ITfComposition* pComposition) {
		_pComposition = pComposition;
	}
	~CTerminateCompositionEditSession() {
		_pComposition.Release();
	}

	//ITfEditSession methods
	STDMETHODIMP DoEditSession(TfEditCookie ec) override {
		if (_pComposition) {
			_pComposition->EndComposition(ec);
		}
		return S_OK;
	}
private:
	CComPtr<ITfComposition> _pComposition;
};

//候補ウィンドウの座標を取得するためのEditSession
class CGetCandidateWindowPosEditSession :public CEditSessionBase
{
public:

	CGetCandidateWindowPosEditSession(ITfContext* pContext, ITfRange* pRange, RECT* pRECT) {
		_pContext = pContext;
		_pRange = pRange;
		_pRECT = pRECT;
	}

	~CGetCandidateWindowPosEditSession() {
		_pContext.Release();
		_pRange.Release();
		_pRECT = nullptr;
	}

	//ITfEditSession methods
	__declspec(noinline) STDMETHODIMP DoEditSession(TfEditCookie ec) override {
		CComPtr<ITfContextView> pContextView;
		if (FAILED(_pContext->GetActiveView(&pContextView)) || (pContextView == nullptr)) {
			return E_FAIL;
		}

		BOOL fClipped = FALSE;
		return pContextView->GetTextExt(ec, _pRange, _pRECT, &fClipped);
	}

private:
	CComPtr<ITfContext> _pContext;
	CComPtr<ITfRange> _pRange;
	RECT* _pRECT;
};
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

//確定と新規入力を同時に行うためのEditSession
class CShiftStartEditSession :public CEditSessionBase
{
public:
	_declspec(noinline) CShiftStartEditSession(CSkkIme*pIme,ITfComposition * pComp, ITfContext * pic, const std::wstring & newText, TfClientId tid) {
		_pComposition = pComp;
		_pContext = pic;
		_newText = newText;
		_clientId = tid;
		_pIme = pIme;
	}

	__declspec(noinline) ~CShiftStartEditSession() {
		_pComposition.Release();
		_pContext.Release();
		_pIme.Release();
	}

	STDMETHODIMP DoEditSession(TfEditCookie ec)override {
		CComPtr<ITfRange> pRange;
		if (FAILED(_pComposition->GetRange(&pRange)) || (pRange == nullptr)) {
			return E_FAIL;
		}

		//確定する部分の下線を消す
		CComPtr<ITfProperty> pProp;
		if (SUCCEEDED(_pContext->GetProperty(GUID_PROP_ATTRIBUTE, &pProp)) && pProp) {
			pProp->Clear(ec, pRange);
		}


		CComPtr<ITfRange> pEndRange;
		pRange->Clone(&pEndRange);
		pEndRange->Collapse(ec, TF_ANCHOR_END);//末尾に移動

		if (FAILED(_pComposition->ShiftStart(ec, pEndRange))) {
			return E_FAIL;
		}



		//新しいテキストを挿入
		CComPtr<ITfRange> pNewRange;
		if (SUCCEEDED(_pComposition->GetRange(&pNewRange)) && pNewRange) {
			pNewRange->SetText(ec, 0, _newText.c_str(), (LONG)_newText.length());

			//新たな下線を引く(CSkkIme::_SetInputDisplayAttributeInfoを流用)
			_pIme->_SetInputDisplayAttributeInfo(_pContext, ec, pNewRange);

			//カーソルの移動
			pNewRange->Collapse(ec, TF_ANCHOR_END);
			TF_SELECTION sl = { 0 };
			sl.range = pNewRange;
			sl.style.ase = TF_AE_NONE;

			_pContext->SetSelection(ec, 1, &sl);
		}

		return S_OK;
	}
private:
	CComPtr<CSkkIme> _pIme;
	CComPtr<ITfComposition> _pComposition;
	CComPtr<ITfContext> _pContext;
	std::wstring _newText;
	TfClientId _clientId;
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
	CTerminateCompositionEditSession(ITfComposition* pComposition,ITfContext* pContext) {
		_pComposition = pComposition;
		_pContext = pContext;
	}
	~CTerminateCompositionEditSession() {
		_pComposition.Release();
	}

	//ITfEditSession methods
	STDMETHODIMP DoEditSession(TfEditCookie ec) override {
		if (_pComposition) {
			CComPtr<ITfRange> pRange;
			if (SUCCEEDED(_pComposition->GetRange(&pRange)) && pRange) {
				CComPtr<ITfProperty> pProp;
				if (SUCCEEDED(_pContext->GetProperty(GUID_PROP_ATTRIBUTE, &pProp)) && pProp) {
					pProp->Clear(ec, pRange);
				}
			}

			_pComposition->EndComposition(ec);
		}
		return S_OK;
	}
private:
	CComPtr<ITfComposition> _pComposition;
	CComPtr<ITfContext> _pContext;
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

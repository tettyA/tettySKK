#include "pch.h"
#include "CSkkIme.h"

#include "Global.h"

//キーが押された瞬間
STDAPI CSkkIme::OnKeyDown(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten) {
	*pfEaten = FALSE;

	// 'Q'キーが押されたら入力を吃る例
	if (wParam == 'R')
	{
		*pfEaten = TRUE;
		_InsertText(pic, L"あ");
		return S_OK;
	}
	return S_OK;
}

//キーが離された瞬間
STDAPI CSkkIme::OnKeyUp(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten) {
	*pfEaten = FALSE;
	return S_OK;
}

//このキーを処理しますかどうかを問い合わせる(キーが押された瞬間)
STDAPI CSkkIme::OnTestKeyDown(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten) {
	*pfEaten = FALSE;
	if (wParam == 'R')
	{
		*pfEaten = TRUE;
		return S_OK;
	}
	return S_OK;
}

STDAPI CSkkIme::OnTestKeyUp(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten) {
	*pfEaten = FALSE;
	return S_OK;
}
STDAPI CSkkIme::OnPreservedKey(ITfContext* pic, REFGUID rguid, BOOL* pfEaten) {
	*pfEaten = FALSE;
	return S_OK;
}
STDAPI CSkkIme::OnSetFocus(BOOL fForeground) {
	return S_OK;
}

BOOL CSkkIme::_InitKeyEventSink() {
	HRESULT hr = E_FAIL;

	CComPtr<ITfKeystrokeMgr> pKeystrokeMgr;
	if (FAILED(_pThreadMgr->QueryInterface(IID_ITfKeystrokeMgr, (void**)&pKeystrokeMgr))) {
		return FALSE;
	}

	//キーが押されたときに通知を受け取るように登録
	hr = pKeystrokeMgr->AdviseKeyEventSink(
		_clientId,
		static_cast<ITfKeyEventSink*>(this),
		TRUE
	);
	return SUCCEEDED(hr);
}

void CSkkIme::_UninitKeyEventSink() {
	CComPtr<ITfKeystrokeMgr> pKeystrokeMgr;
	if (SUCCEEDED(_pThreadMgr->QueryInterface(IID_ITfKeystrokeMgr, (void**)&pKeystrokeMgr))) {
		pKeystrokeMgr->UnadviseKeyEventSink(_clientId);
	}
}
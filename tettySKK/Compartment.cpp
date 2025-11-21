#include "pch.h"
#include "CSkkIme.h"

void CSkkIme::_SetCompartment(REFGUID guid, const VARIANT& var)
{
	if (_pThreadMgr == nullptr)return;

	CComPtr<ITfCompartmentMgr> pCompartmentMgr;
	if (FAILED(_pThreadMgr->QueryInterface(IID_ITfCompartmentMgr, (void**)&pCompartmentMgr)))
	{
		return;
	}

	CComPtr<ITfCompartment> pCompartment;
	if (FAILED(pCompartmentMgr->GetCompartment(guid, &pCompartment)))
	{
		return;
	}

	HRESULT hr = pCompartment->SetValue(_clientId, &var);
	if (FAILED(hr)) {
		// もしここに来るなら、Windowsが「変更を受け付けない」状態です
		// デバッグ出力ウィンドウで確認できます
		wchar_t msg[64];
		swprintf_s(msg, L"SetValue Failed! HR=0x%08X\n", hr);
		OutputDebugString(msg);
		
		// 念のためメッセージボックスでも
		MessageBox(NULL, msg, L"Error", MB_OK); 
	}
}

void CSkkIme::__UpdateInputMode()
{
	VARIANT varOpen; varOpen.vt = VT_I4; varOpen.lVal = TRUE;
	VARIANT varConv; varConv.vt = VT_I4; varConv.lVal = 0;
	
	{
		VARIANT var;
		V_VT(&var) = VT_I4;
		V_I4(&var) = TF_SENTENCEMODE_PHRASEPREDICT;
		_SetCompartment(GUID_COMPARTMENT_KEYBOARD_INPUTMODE_SENTENCE, var);
	}
	if(m_currentMode == SKKMode::Hankaku) {
		varOpen.lVal = FALSE;
	}
	else if (m_currentMode == SKKMode::Henkan || m_currentMode == SKKMode::Kakutei) {
		varOpen.lVal = TRUE;
	}

	if (m_CurrentKanaMode == KanaMode::Hiragana) {
		varConv.lVal = TF_CONVERSIONMODE_NATIVE | TF_CONVERSIONMODE_FULLSHAPE;
	}
	else if (m_CurrentKanaMode == KanaMode::Katakana) {
		varConv.lVal = TF_CONVERSIONMODE_NATIVE | TF_CONVERSIONMODE_FULLSHAPE | TF_CONVERSIONMODE_KATAKANA;
	}
	else {
		varConv.lVal = 0;
	}

	

	_SetCompartment(GUID_COMPARTMENT_KEYBOARD_OPENCLOSE, varOpen);
	if (varOpen.lVal) {
		_SetCompartment(GUID_COMPARTMENT_KEYBOARD_INPUTMODE_CONVERSION, varConv);
	}
	if (m_pLangBarItemButton != nullptr) {
		m_pLangBarItemButton->_UpdateIcon();
	}
}

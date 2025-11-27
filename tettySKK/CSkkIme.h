#pragma once
#include "pch.h"
#include <msctf.h>
#include <atlbase.h>
#include "TranslatetRomajiToKana.h"
#include "SKKDictionaly.h"
#include "CCandidateWindow.h"
#include "CLangBarItemButton.h"
#include "Global.h"


//ref https://github.com/nathancorvussolis/corvusskk/blob/2904b3ad7ba80e66e717aef6805164c74fcec71d/imcrvtip/TextService.h
//ref https://github.com/nathancorvussolis/corvusskk/blob/2904b3ad7ba80e66e717aef6805164c74fcec71d/imcrvtip/TextService.cpp
class CSkkIme :
	public ITfTextInputProcessor,
	public ITfKeyEventSink,
	public ITfCompositionSink,
	public ITfDisplayAttributeProvider
{
public:
	CSkkIme();
	~CSkkIme();

	//IUnknown methods
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	//ITfTextInputProcessor methods
	STDMETHODIMP Activate(ITfThreadMgr* ptim, TfClientId tid);
	STDMETHODIMP Deactivate();

	//ITfKeyEventSink methods
	STDMETHODIMP OnSetFocus(BOOL fForeground);
	STDMETHODIMP OnTestKeyDown(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten);
	STDMETHODIMP OnTestKeyUp(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten);
	STDMETHODIMP OnKeyDown(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten);
	STDMETHODIMP OnKeyUp(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten);
	STDMETHODIMP OnPreservedKey(ITfContext* pic, REFGUID rguid, BOOL* pfEaten);

	//ITfCompositionSink methods
	STDMETHODIMP OnCompositionTerminated(TfEditCookie ecWrite, ITfComposition* pComposition);

	//ITfDisplayAttributeProvider methods
	STDMETHODIMP EnumDisplayAttributeInfo(IEnumTfDisplayAttributeInfo** ppEnum);
	STDMETHODIMP GetDisplayAttributeInfo(REFGUID guid, ITfDisplayAttributeInfo** ppInfo);

	//テキスト挿入補助
	HRESULT _DoInsertText(TfEditCookie ec, ITfContext* pContext, const WCHAR* text,BOOL _isDetermined);

	void _GetCurrentMode(SKKMode& mode, KanaMode& kanaMode) {
		mode = m_currentMode;
		kanaMode = m_CurrentKanaMode;
	}
	void _GetThreadMgr(ITfThreadMgr** ppThreadMgr) {
		if (ppThreadMgr == nullptr)return;
		*ppThreadMgr = _pThreadMgr;
		(*ppThreadMgr)->AddRef();
	}
private:
	LONG _refCount;
	//TSFとの連絡用
	CComPtr<ITfThreadMgr> _pThreadMgr;
	TfClientId _clientId;

	//未確定文字列管理用
	CComPtr<ITfComposition> _pComposition;
	void __InsertText(ITfContext* pic, const WCHAR* text,BOOL _isDetermined);
	void __InsertNewRegWord(ITfContext* pic, const std::wstring& text, BOOL _isDetermined);

	void _Output(ITfContext* pic, const std::wstring& text, BOOL _isDetermined);

	BOOL _InitKeyEventSink();
	void _UninitKeyEventSink();

	HRESULT _SetInputDisplayAttributeInfo(ITfContext* pContext, TfEditCookie ec, ITfRange* pRange);

	bool _IsKeyEaten(WPARAM wParam);

	HRESULT _HandleRegSpaceKey(ITfContext* pic, WCHAR key);
	HRESULT _HandleSpaceKey(ITfContext* pic, WCHAR key);
	HRESULT _HandleCharKey(ITfContext* pic, WCHAR key);
	

	TranslatetRomajiToKana m_RomajiToKanaTranslator;

	CSKKDictionaly m_SKKDictionaly;
	SKKCandidates m_CurrentCandidates;
	SKKCandidates m_RegCurrentCandidates;

	size_t m_CurrentShowCandidateIndex;
	size_t m_RegCurrentShowCandidateIndex;

	BOOL _GetCompositionString(std::wstring& compositionString);
	std::wstring m_Gokan;//語幹部分 なければ未確定文字列全体             諦め(る) -> 諦
	WCHAR m_OkuriganaFirstChar;//送り仮名の最初の一文字 ない場合はL'\0'  諦め(る) -> m  

	
	void _CommitComposition(ITfContext* pic);
	void _CommitRegComposition(ITfContext* pic);

	CCandidateWindow* m_pCandidateWindow;
	void _UpDateCandidateWindowPosition(ITfContext* pic);
	void _EndCandidateWindow();

	 SKKMode m_currentMode;
	 KanaMode m_CurrentKanaMode;
	void _ChangeCurrentMode(const SKKMode mode) {
		m_currentMode = mode;
		__UpdateInputMode();
	}
	void _ChangeCurrenKanaMode(const KanaMode mode) {
		m_CurrentKanaMode = mode;
		__UpdateInputMode();
	}

	void __UpdateInputMode();

	bool _IsShiftKeyPressed();
	bool _IsCtrlKeyPressed();

	CComPtr<CLangBarItemButton> m_pLangBarItemButton;

	void _SetCompartment(REFGUID guid, const VARIANT& var);
	
	BOOL m_isRegiteringNewWord;
	void _BgnRegiterNewWord(ITfContext* pic,std::wstring regKey);

	std::wstring m_RegKey;
	std::wstring m_RegInputDetermined;
	std::wstring m_RegInputUndetermined;

	std::wstring m_currentInputKana;
	void __InsertTextMakeCandidateWindow(ITfContext* pic,const WCHAR* _multiIntsertText,const WCHAR* _singleInsertText);




	void _EndRegiterNewWord();

	
};



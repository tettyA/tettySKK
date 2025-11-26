#include "pch.h"
#include "CSkkIme.h"

#include "Global.h"
#include "CDisplayAttributeInfo.h"


#include "msctf.h"
#include "ctffunc.h"

CSkkIme::CSkkIme()
{
	DllAddRef();

	_refCount = 1;
	_pThreadMgr = nullptr;
	_clientId = TF_CLIENTID_NULL;
	_pComposition = nullptr;

	m_CurrentShowCandidateIndex = 0;

	m_CurrentCandidates = SKKCandidates();

	m_pCandidateWindow = new CCandidateWindow(g_hModule);

	m_currentMode = SKKMode::Hankaku;
	m_CurrentKanaMode = KanaMode::Hiragana;

	m_Gokan = L"";
	m_OkuriganaFirstChar = L'\0';


	
	m_pLangBarItemButton = new CLangBarItemButton(this, GUID_LBI_INPUTMODE);
	m_isRegiteringNewWord = FALSE;
}

CSkkIme::~CSkkIme()
{
	if(m_pLangBarItemButton)
	{
		m_pLangBarItemButton.Release();
		m_pLangBarItemButton = nullptr;
	}
	if(_pThreadMgr)
	{
		_pThreadMgr.Release();
		_pThreadMgr = nullptr;
	}
	if(m_pCandidateWindow)
	{
		delete m_pCandidateWindow;
		m_pCandidateWindow = nullptr;
	}
	DllRelease();
}

STDAPI CSkkIme::QueryInterface(REFIID riid, void** ppvObj)
{
	if (ppvObj == nullptr)
	{
		return E_INVALIDARG;
	}
	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ITfTextInputProcessor))
	{
		*ppvObj = static_cast<ITfTextInputProcessor*>(this);
	}
	else if (IsEqualIID(riid, IID_ITfKeyEventSink))
	{
		*ppvObj = static_cast<ITfKeyEventSink*>(this);
	}
	else if (IsEqualIID(riid, IID_ITfCompositionSink))
	{
		*ppvObj = static_cast<ITfCompositionSink*>(this);
	}
	else if (IsEqualIID(riid, IID_ITfDisplayAttributeProvider))
	{
		*ppvObj = static_cast<ITfDisplayAttributeProvider*>(this);
	}
	else
	{
		*ppvObj = nullptr;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}
STDAPI_(ULONG) CSkkIme::AddRef()
{
	return InterlockedIncrement(&_refCount);
}
STDAPI_(ULONG) CSkkIme::Release()
{
	LONG refCount = InterlockedDecrement(&_refCount);
	if (refCount == 0)
	{
		delete this;
		return 0;
	}
	return refCount;
}

STDAPI CSkkIme::Activate(ITfThreadMgr* ptim, TfClientId tid) {
	if (ptim == nullptr) return E_INVALIDARG;

	_pThreadMgr = ptim; // CComPtrが自動的にAddRefするので明示的なAddRefは不要
	_clientId = tid;

#ifdef _DEBUG
	OutputDebugString(L"CSkkIme::Activate Called!");
#endif

	if (!_InitKeyEventSink())
	{
		return E_FAIL;
	}
	
	
	m_SKKDictionaly.LoadDictionaryFromFile(SKK_DICTIONARY_FILEPATH);

	if (m_pLangBarItemButton) {
		m_pLangBarItemButton->_Init();
	}

	__UpdateInputMode();
	return S_OK;
}

STDAPI CSkkIme::Deactivate() {
	if (_pThreadMgr == nullptr)
	{
		return S_OK;
	}

	_EndCandidateWindow();
	_UninitKeyEventSink();



	if (_pComposition) {
		_pComposition.Release();
		//_pComposition = nullptr;
	}

	if (m_pLangBarItemButton) {
		m_pLangBarItemButton->_Uninit();
	}

	if (_pThreadMgr) {
		_pThreadMgr.Release();
		//_pThreadMgr = nullptr;
	}
	_clientId = TF_CLIENTID_NULL;
#ifdef _DEBUG
	OutputDebugString(L"CSkkIme::Deactivate Called!");
#endif

	_clientId = TF_CLIENTID_NULL;
	
	__UpdateInputMode();

	return S_OK;
}

void CSkkIme::_EndCandidateWindow()
{
	if (m_pCandidateWindow) {
		m_pCandidateWindow->HideWindow();
	}
}

void _ConvertToKatakana(std::wstring& str)
{
	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] >= L'ぁ' && str[i] <= L'ん') {
			str[i] = str[i] - L'ぁ' + L'ァ';
		}
	}
}

void _ConvertToHiragana(std::wstring& str)
{
	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] >= L'ァ' && str[i] <= L'ン') {
			str[i] = str[i] + L'ぁ' - L'ァ';
		}
	}
}
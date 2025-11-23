#include "pch.h"

#include <windows.h>
#include <msctf.h>
#include "CCandidateWindow.h"



CCandidateWindow::CCandidateWindow(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
	m_hWnd = NULL;

	m_CurrentPageIndex = CANDIDATEWINDOW_MODE_SINGLE;
	m_Mode = 0;
	constexpr LPCWSTR windowClsName = TEXT("SKK_Candidates_Window");
	WNDCLASS winc;
	winc.style = CS_HREDRAW | CS_VREDRAW;	       //ウィンドウの基本スタイル
	winc.lpfnWndProc = WndProc;                    //ウィンドウプロシージャ
	winc.cbClsExtra = 0;                           //クラス構造体追加領域予約
	winc.cbWndExtra = 0;                           //ウィンドウ構造体追加領域予約
	winc.hInstance = hInstance;                    //インスタンスハンドル
	winc.hIcon = LoadIcon(NULL, IDI_APPLICATION);  //アイコン
	winc.hCursor = LoadCursor(NULL, IDC_ARROW);    //マウスカーソル
	winc.hbrBackground = NULL;                     //背景色
	winc.lpszMenuName = NULL;                      //クラスメニュー
	winc.lpszClassName = windowClsName;            //クラス名

	if (!RegisterClass(&winc)) {
		return;
	}

	m_hWnd = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
		windowClsName, TEXT(""),
		WS_POPUP | WS_BORDER,
		0, 0, 200, 300,
		NULL, NULL,
		hInstance, NULL);

	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);
}

CCandidateWindow::~CCandidateWindow()
{
	if (IsWindowExists()) {
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

// Mode = 0 : 一つのみ表示
// Mode = 1 : 複数(ASDFJKL)表示
// Mode = 2 : 登録語(candidates[0]に登録したい語，indexは無視)
void CCandidateWindow::SetCandidates(SKKCandidates candidates, size_t index, int Mode)
{
	if (!IsWindowExists()) {
		return;
	}

	m_Candidates = candidates;
	m_CurrentPageIndex = index;
	m_Mode = Mode;
}

void CCandidateWindow::HideWindow()
{
	if (!IsWindowExists()) {
		return;
	}
	::ShowWindow(m_hWnd, SW_HIDE);
}

void CCandidateWindow::ShowWindow(int x, int y)
{
	if (!IsWindowExists()) {
		return;
	}
	SetWindowPos(m_hWnd, HWND_TOPMOST, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
	InvalidateRect(m_hWnd, NULL, TRUE);

	UpdateWindow(m_hWnd);
}

LRESULT CALLBACK CCandidateWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CCandidateWindow* pThis = reinterpret_cast<CCandidateWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		if (pThis) {
			pThis->_OnPaint(hdc);
		}
		EndPaint(hWnd, &ps);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

#define CANDIDATES_WINDOW_BACKCOLOR_HBURUSH (HBRUSH)(GetStockObject(WHITE_BRUSH)) 
#define CANDIDATES_WINDOW_TEXTCOLOR_RGB RGB(0, 0, 0)
#define CANDIDATES_WINDOW_HIGHLIGHTTEXTCOLOR_RGB RGB(255, 0, 0)
#define CANDIDATES_WINDOW_ASDFJKL_TEXTCOLOR_RGB RGB(0, 0, 255)

#define CANDIDATES_WINDOW_ANNOTATIONTEXTCOLOR_RGB RGB(128, 128, 128)

#define CANDIDATES_WINDOW_FONT_HSIZE (46)

#define WSTR_AND_WLEN(wstr) (wstr).c_str(), static_cast<int>((wstr).length())


#define CANDIDATES_WINDOW_MAXWIDTH (900)

void CCandidateWindow::_OnPaint(HDC hdc)
{
	RECT cc;
	GetClientRect(m_hWnd, &cc);
	FillRect(hdc, &cc, CANDIDATES_WINDOW_BACKCOLOR_HBURUSH);

	HFONT hFont =
		CreateFont(
			CANDIDATES_WINDOW_FONT_HSIZE,
			0,
			0,
			0,
			FW_REGULAR,
			FALSE,
			FALSE,
			FALSE,
			SHIFTJIS_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			PROOF_QUALITY,
			FIXED_PITCH | FF_MODERN,
			L"Meiryo UI"
		);
		
	SelectObject(hdc, hFont);
	SetBkMode(hdc, TRANSPARENT);

	int y = 0, x = 5;
	int maxx = 0;
	SIZE strsize ;
	TextOut(hdc, x, y, WSTR_AND_WLEN(L"Debug:" + std::to_wstring(m_Mode)));
	y += 12;
	if (m_Mode == CANDIDATEWINDOW_MODE_SINGLE) {
		TextOut(hdc, x, y, WSTR_AND_WLEN(m_Candidates[m_CurrentPageIndex]_Candidate));
		GetTextExtentPoint(hdc, WSTR_AND_WLEN(m_Candidates[m_CurrentPageIndex]_Candidate + L"  "), &strsize);

		if (m_Candidates[m_CurrentPageIndex]_Annotation.length() > 0) {
			x += strsize.cx;
			//註釈表示
			SetTextColor(hdc, CANDIDATES_WINDOW_ANNOTATIONTEXTCOLOR_RGB);
			std::wstring annotationText = SKK_CANDIDOTATES_ANNOTATION_SEPARATOR_STR + m_Candidates[m_CurrentPageIndex]_Annotation;
			TextOut(hdc, x, y, WSTR_AND_WLEN(annotationText));
			GetTextExtentPoint32(hdc, WSTR_AND_WLEN(annotationText), &strsize);
		}

		SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, strsize.cx + 2, strsize.cy + 2, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE | SWP_NOREPOSITION | SWP_SHOWWINDOW);
	}
	else if (m_Mode == CANDIDATEWINDOW_MODE_MULTIPLE) {
		for (size_t i = 0; i < NUM_SHOW_CANDIDATE_MULTIPLE; ++i) {
			size_t candidateIndex = BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX + (m_CurrentPageIndex - BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX) * NUM_SHOW_CANDIDATE_MULTIPLE + i;
			if (candidateIndex >= m_Candidates.size()) {
				break;
			}
			//A S D F J K L の表示 
			SetTextColor(hdc, CANDIDATES_WINDOW_ASDFJKL_TEXTCOLOR_RGB);

			wchar_t keyChar = SKK_CHOOSE_CANDIDATES_BIGSTR[i];

			TextOut(hdc, x, y, &keyChar, 1);
			GetTextExtentPoint32(hdc, &keyChar, 1, &strsize);
			x += strsize.cx;
			//候補文字列の表示
			SetTextColor(hdc, CANDIDATES_WINDOW_TEXTCOLOR_RGB);
			std::wstring text = L":" + m_Candidates[candidateIndex]_Candidate + L" ";
			TextOut(hdc, x, y, WSTR_AND_WLEN(text));
			GetTextExtentPoint32(hdc, WSTR_AND_WLEN(text), &strsize);

			x += strsize.cx;
			if (m_Candidates[candidateIndex]_Annotation.length() > 0) {
				//註釈表示
				SetTextColor(hdc, CANDIDATES_WINDOW_ANNOTATIONTEXTCOLOR_RGB);
				std::wstring annotationText = SKK_CANDIDOTATES_ANNOTATION_SEPARATOR_STR + m_Candidates[candidateIndex]_Annotation + L" ";
				TextOut(hdc, x, y, WSTR_AND_WLEN(annotationText));
				GetTextExtentPoint32(hdc, WSTR_AND_WLEN(annotationText), &strsize);
				x += strsize.cx;
			}

			if (x >= CANDIDATES_WINDOW_MAXWIDTH) {

				maxx = max(x, maxx);
				x = 5;
				y += CANDIDATES_WINDOW_FONT_HSIZE + 2;
			}
		}
		maxx = max(x, maxx);

		//残りの候補数表示
		size_t remainingCandidates =
			max((int)m_Candidates.size() - (int)(BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX - 1 + (m_CurrentPageIndex - BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX + 1) * NUM_SHOW_CANDIDATE_MULTIPLE) - 1, 0);

		std::wstring remainingText = L"残" + std::to_wstring(remainingCandidates) + L"  ";
		SetTextColor(hdc, CANDIDATES_WINDOW_HIGHLIGHTTEXTCOLOR_RGB);
		TextOut(hdc, x, y, remainingText.c_str(), static_cast<int>(remainingText.length()));
		GetTextExtentPoint32(hdc, WSTR_AND_WLEN(remainingText), &strsize);
		x += strsize.cx;

		SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, maxx + 2, y + strsize.cy + 2, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE | SWP_NOREPOSITION | SWP_SHOWWINDOW);
	} 
	else if (m_Mode == CANDIDATEWINDOW_MODE_REGWORD) {
		//登録語モード
		
		SetTextColor(hdc, CANDIDATES_WINDOW_ANNOTATIONTEXTCOLOR_RGB);
		TextOut(hdc, x, y, WSTR_AND_WLEN(std::wstring(L"登録:   ")));
		GetTextExtentPoint32(hdc, WSTR_AND_WLEN(std::wstring(L"登録:   ")), &strsize);
		x += strsize.cx;

		if (!m_Candidates[0]_Candidate.empty()) {
			SetTextColor(hdc, CANDIDATES_WINDOW_TEXTCOLOR_RGB);
			TextOut(hdc, x, y, WSTR_AND_WLEN(m_Candidates[0]_Candidate));
			GetTextExtentPoint32(hdc, WSTR_AND_WLEN(m_Candidates[0]_Candidate), &strsize);
			x += strsize.cx;
		}

		if (!m_Candidates[0]_Annotation.empty()) {
			SetTextColor(hdc, CANDIDATES_WINDOW_ANNOTATIONTEXTCOLOR_RGB);
			std::wstring annotationText = SKK_CANDIDOTATES_ANNOTATION_SEPARATOR_STR + m_Candidates[0]_Annotation;
			TextOut(hdc, x, y, WSTR_AND_WLEN(annotationText));
			GetTextExtentPoint32(hdc, WSTR_AND_WLEN(annotationText), &strsize);
			x += strsize.cx;
		}


		SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 500, 500, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE | SWP_NOREPOSITION | SWP_SHOWWINDOW);
		
	}
	
}

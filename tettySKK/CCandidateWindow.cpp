#include "pch.h"

#include <windows.h>
#include <msctf.h>
#include "CCandidateWindow.h"

CCandidateWindow::CCandidateWindow(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
	m_hWnd = NULL;

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

void CCandidateWindow::SetCandidates(const SKKCandidates& candidates, size_t index)
{
	if (!IsWindowExists()) {
		return;
	}

	m_Candidates = candidates;
	m_CurrentIndex = index;
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

void CCandidateWindow::_OnPaint(HDC hdc)
{
	RECT cc;
	GetClientRect(m_hWnd, &cc);
	FillRect(hdc, &cc, CANDIDATES_WINDOW_BACKCOLOR_HBURUSH);

	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SelectObject(hdc, hFont);
	SetBkMode(hdc, TRANSPARENT);

	int y = 0;
	int lineHeight = 20;
	for (size_t i = 0; i < m_Candidates.size(); ++i) {
		if (i == m_CurrentIndex) {
			SetTextColor(hdc, CANDIDATES_WINDOW_HIGHLIGHTTEXTCOLOR_RGB);
		}
		else {
			SetTextColor(hdc, CANDIDATES_WINDOW_TEXTCOLOR_RGB);
		}

		std::wstring text = std::to_wstring(i + 1) + L"." + m_Candidates[i];

		TextOut(hdc, 5, y, text.c_str(), static_cast<int>(text.length()));

		y += lineHeight;
	}
}

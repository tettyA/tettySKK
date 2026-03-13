#include "pch.h"
#include "CStateWindow.h"

#define STATEWINDOW_BACKCOLOR_RGB RGB(32, 32, 32)
#define STATEWINDOW_TEXTCOLOR_RGB RGB(90, 230, 255)
#define STATEWINDOW_PADDING_X 8
#define STATEWINDOW_PADDING_Y 4
#define STATEWINDOW_FONT_NAME L"Meiryo UI"
#define STATEWINDOW_FONT_HEIGHT 24
#define STATEWINDOW_MAX_TEXT_WIDTH 520

CStateWindow::CStateWindow(HINSTANCE hInstance)
	: m_hWnd(nullptr), m_hInstance(hInstance), m_hFont(nullptr), m_textSize{ 0,0 },
	  m_pendingCaret{ 0,0 }, m_hasPendingShow(false)
{
	constexpr LPCWSTR clsName = TEXT("SKK_State_Window");

	WNDCLASS wc{};
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = nullptr;
	wc.lpszClassName = clsName;

	RegisterClass(&wc);

	m_hWnd = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
		clsName, TEXT(""),
		WS_POPUP | WS_BORDER,
		0, 0, 120, 40,
		nullptr, nullptr, hInstance, nullptr);

	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

	m_hFont = CreateFont(
		STATEWINDOW_FONT_HEIGHT, 0, 0, 0, FW_BOLD,
		FALSE, FALSE, FALSE,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
		FIXED_PITCH | FF_MODERN,
		STATEWINDOW_FONT_NAME);
}

CStateWindow::~CStateWindow()
{
	if (m_hWnd) {
		KillTimer(m_hWnd, SHOW_TIMER_ID);
	}
	if (m_hFont) {
		DeleteObject(m_hFont);
		m_hFont = nullptr;
	}
	if (m_hWnd) {
		DestroyWindow(m_hWnd);
		m_hWnd = nullptr;
	}
}

void CStateWindow::SetText(const std::wstring& text)
{
	m_text = text;
	_MeasureText();
}

void CStateWindow::_MeasureText()
{
	if (!m_hWnd) return;

	HDC hdc = GetDC(m_hWnd);
	if (!hdc) return;

	HFONT old = (HFONT)SelectObject(hdc, m_hFont);

	if (!m_text.empty()) {
		RECT rc = { 0, 0, STATEWINDOW_MAX_TEXT_WIDTH, 0 };
		DrawText(hdc, m_text.c_str(), -1, &rc, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK | DT_CALCRECT);
		m_textSize.cx = rc.right - rc.left;
		m_textSize.cy = rc.bottom - rc.top;
	}
	else {
		m_textSize = { 0,0 };
	}

	SelectObject(hdc, old);
	ReleaseDC(m_hWnd, hdc);
}

void CStateWindow::_ShowNowBeforeCaret(int caretLeft, int caretTop)
{
	if (!m_hWnd) return;
	if (m_text.empty()) {
		HideWindow();
		return;
	}

	int w = m_textSize.cx + STATEWINDOW_PADDING_X * 2;
	int h = max(m_textSize.cy + STATEWINDOW_PADDING_Y * 2, 28);

	int x = max(0, caretLeft - w - 6);
	int y = max(0, caretTop - 2);

	SetWindowPos(
		m_hWnd, HWND_TOPMOST, x, y, w, h,
		SWP_NOACTIVATE | SWP_SHOWWINDOW);

	InvalidateRect(m_hWnd, nullptr, TRUE);
	UpdateWindow(m_hWnd);
}

void CStateWindow::ShowBeforeCaret(int caretLeft, int caretTop, UINT delayMs)
{
	if (!m_hWnd) return;

	if (m_text.empty()) {
		HideWindow();
		return;
	}

	if (delayMs == 0) {
		KillTimer(m_hWnd, SHOW_TIMER_ID);
		m_hasPendingShow = false;
		_ShowNowBeforeCaret(caretLeft, caretTop);
		return;
	}

	// 最新要求で上書き（デバウンス）
	m_pendingCaret.x = caretLeft;
	m_pendingCaret.y = caretTop;
	m_hasPendingShow = true;

	KillTimer(m_hWnd, SHOW_TIMER_ID);
	SetTimer(m_hWnd, SHOW_TIMER_ID, delayMs, nullptr);
}

void CStateWindow::_OnTimer(WPARAM timerId)
{
	if (timerId != SHOW_TIMER_ID || !m_hWnd) {
		return;
	}

	KillTimer(m_hWnd, SHOW_TIMER_ID);

	if (!m_hasPendingShow) {
		return;
	}
	m_hasPendingShow = false;

	_ShowNowBeforeCaret(m_pendingCaret.x, m_pendingCaret.y);
}

void CStateWindow::HideWindow()
{
	if (!m_hWnd) return;

	KillTimer(m_hWnd, SHOW_TIMER_ID);
	m_hasPendingShow = false;

	::ShowWindow(m_hWnd, SW_HIDE);
}

void CStateWindow::_OnPaint(HDC hdc)
{
	RECT rc{};
	GetClientRect(m_hWnd, &rc);

	HBRUSH bg = CreateSolidBrush(STATEWINDOW_BACKCOLOR_RGB);
	FillRect(hdc, &rc, bg);
	DeleteObject(bg);

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, STATEWINDOW_TEXTCOLOR_RGB);

	HFONT old = (HFONT)SelectObject(hdc, m_hFont);

	RECT trc{
		STATEWINDOW_PADDING_X,
		STATEWINDOW_PADDING_Y,
		rc.right - STATEWINDOW_PADDING_X,
		rc.bottom - STATEWINDOW_PADDING_Y
	};
	DrawText(hdc, m_text.c_str(), -1, &trc, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK);

	SelectObject(hdc, old);
}

LRESULT CALLBACK CStateWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto* pThis = reinterpret_cast<CStateWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	switch (message) {
	case WM_PAINT:
	{
		PAINTSTRUCT ps{};
		HDC hdc = BeginPaint(hWnd, &ps);
		if (pThis) pThis->_OnPaint(hdc);
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_TIMER:
		if (pThis) {
			pThis->_OnTimer(wParam);
			return 0;
		}
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
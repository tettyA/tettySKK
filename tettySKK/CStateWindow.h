#pragma once
#include "pch.h"
#include <Windows.h>
#include <string>

class CStateWindow
{
public:
	explicit CStateWindow(HINSTANCE hInstance);
	~CStateWindow();

	bool IsWindowExists() const { return m_hWnd != nullptr; }

	void SetText(const std::wstring& text);
	void ShowBeforeCaret(int caretLeft, int caretTop, UINT delayMs = 0); // delay‘Î‰ž
	void HideWindow();

private:
	HWND m_hWnd;
	HINSTANCE m_hInstance;
	std::wstring m_text;

	HFONT m_hFont;
	SIZE m_textSize;

	POINT m_pendingCaret;
	bool m_hasPendingShow;

	enum : UINT_PTR { SHOW_TIMER_ID = 1 };

	void _MeasureText();
	void _OnPaint(HDC hdc);
	void _OnTimer(WPARAM timerId);
	void _ShowNowBeforeCaret(int caretLeft, int caretTop);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
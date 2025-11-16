#pragma once
#include "pch.h"
#include <Windows.h>
#include <atlbase.h>
#include "SKKDictionaly.h"

class CCandidateWindow
{
public:
	CCandidateWindow(HINSTANCE hInstance);
	~CCandidateWindow();

	void SetCandidates(const SKKCandidates& candidates, size_t index);
	void HideWindow();

	bool IsWindowExists() const {
		return m_hWnd != NULL;
	}

	void ShowWindow(int x, int y);
private:
	HWND m_hWnd;
	HINSTANCE m_hInstance;

	SKKCandidates m_Candidates;
	size_t m_CurrentIndex;
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void _OnPaint(HDC hdc);
};


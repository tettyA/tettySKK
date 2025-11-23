#pragma once
#include "pch.h"
#include <Windows.h>
#include <atlbase.h>
#include "SKKDictionaly.h"

#define CANDIDATEWINDOW_MODE_SINGLE 0
#define CANDIDATEWINDOW_MODE_MULTIPLE 1
#define CANDIDATEWINDOW_MODE_REGWORD 2

#define BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX 5
#define NUM_SHOW_CANDIDATE_MULTIPLE 7
class CCandidateWindow
{
public:
	CCandidateWindow(HINSTANCE hInstance);
	~CCandidateWindow();

	void SetCandidates(SKKCandidates candidates, size_t index, int Mode);
	void GetCandidates(SKKCandidates& candidates) const {
		candidates = m_Candidates;
	}
	void HideWindow();

	bool IsWindowExists() const {
		return m_hWnd != NULL;
	}

	void ShowWindow(int x, int y);
private:
	HWND m_hWnd;
	HINSTANCE m_hInstance;

	SKKCandidates m_Candidates;
	//size_t m_CurrentIndex;
	size_t m_CurrentPageIndex;
	BOOL m_Mode; // 0: single, 1: multiple
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void _OnPaint(HDC hdc);
};


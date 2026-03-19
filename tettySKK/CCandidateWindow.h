#pragma once
#include "pch.h"
#include <Windows.h>
#include <atlbase.h>
#include "SKKDictionaly.h"

#define CANDIDATEWINDOW_MODE_SINGLE              (0b00001)
#define CANDIDATEWINDOW_MODE_MULTIPLE            (0b00010)
#define CANDIDATEWINDOW_MODE_REGWORD             (0b00100)
#define CANDIDATEWINDOW_MODE_PREDICT             (0b01000)

#define BEGIN_SHOW_CANDIDATE_MULTIPLE_INDEX 5
#define NUM_SHOW_CANDIDATE_MULTIPLE 7
class CCandidateWindow
{
public:
	CCandidateWindow(HINSTANCE hInstance);
	~CCandidateWindow();

	// Mode = CANDIDATEWINDOW_MODE_SINGLE   : 一つのみ表示
    // Mode = CANDIDATEWINDOW_MODE_MULTIPLE : 複数(ASDFJKL)表示
    // Mode = CANDIDATEWINDOW_MODE_REGWORD  : 登録語(candidates[-2]に確定文字列，candidates[-1]に未確定文字列，candidates[0...-3]に候補。indexは無視)
	void SetCandidates(SKKCandidates& candidates, size_t index, int Mode);
	void GetCandidates(SKKCandidates& candidates) const {
		candidates = m_Candidates;
	}
	void HideWindow();
	void MustHideWindow();

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
  int m_Mode;
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void _OnPaint(HDC hdc);
	void __PaintSingleMode(HDC hdc, int _bgnx, int _bgny, SIZE& _rectsize);
	void __PaintMultipleMode(HDC hdc, int _bgnx, int _bgny, SIZE& _rectsize);
	void __PaintPredictMode(HDC hdc, int _bgnx, int _bgny, SIZE& _rectsize);
};


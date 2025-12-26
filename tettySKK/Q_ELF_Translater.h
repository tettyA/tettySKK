#pragma once
#include "kmp.h"

class Q_ELF_Translater
{
public:
	Q_ELF_Translater();
	~Q_ELF_Translater() {};

	/// <summary>
	/// QWERTY配列からQ-ELF配列への変換を行います。
	/// 全ての引数は大文字で指定又は返却されます。
	/// </summary>
	/// <param name="key">QWERTY配列時のキー</param>
	/// <param name="output">戻り値がtrueの時，変換したローマ字が入ります</param>
	/// <returns>trueの時，変換を行った。falseの時，変換に達しなかった</returns>
	bool TranslateQWERTYtoQ_ELF(WCHAR key, std::wstring& output);

	bool isNeedExecuteTranslate(WCHAR key) {
		return kmp.KMPs.contains(key);
	}
	void LoadKmpFileFromFile(std::wstring &path);
	bool Get_kmp_is_key_pushed_shift() {
		return kmp.is_key_pushed.shift;
	}
	void Set_kmp_is_key_pushed_shift(bool value) {
		kmp.is_key_pushed.shift = value;
	}

private:
	std::wstring m_buffer;
	KMP kmp;

	int DSendCnt;
	struct DsendElements{
		KMP::Dir dir;
		union{
			KeyCode samedirkc;
			KeyCode dirkc;
		}dir1;
		KeyCode otherdirkc;
	}Send1st;
};


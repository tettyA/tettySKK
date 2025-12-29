#pragma once

#include "Global.h"

//#ifdef TETTYSKK_CONTAIN_Q_ELF

#include "kmp.h"
#define KMP_SHOWTRANSINFO

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

	bool isNeedExecuteTranslate(WCHAR key) const{
		return kmp.KMPs.contains(key);
	}
	void LoadKmpFileFromFile(std::wstring &path);
	bool Get_kmp_is_key_pushed_shift() {
		return kmp.is_key_pushed.shift;
	}
	void Set_kmp_is_key_pushed_shift(bool value) {
		kmp.is_key_pushed.shift = value;
	}
	void ResetKmpState(){
		DSendCnt = 0;
		kmp.is_key_pushed = { false,false,false };
	}
#ifdef KMP_SHOWTRANSINFO
	KMP::SpecialKeyStates GetkeyState() const {
		return kmp.is_key_pushed;
	}
	int GetDSendCnt() const{
		return DSendCnt;
	}
	KMP::Dir GetSend1stDir() const {
		return Send1st.dir;
	}
#endif

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

//#endif //TETTYSKK_CONTAIN_Q_ELF
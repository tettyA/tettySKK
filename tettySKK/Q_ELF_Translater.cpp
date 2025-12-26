#include "pch.h"
#include "Q_ELF_Translater.h"
#include "Global.h"

#include <sstream>
#include <fstream>
#include <filesystem>


#define __DEBUGOUTPUT(dbgstr) __InsertText(pic, (L"["+(dbgstr)+L"]").c_str(), TRUE)

Q_ELF_Translater::Q_ELF_Translater()
{
    kmp = KMP();
    m_buffer = L"";
    DSendCnt = 0;
}

bool Q_ELF_Translater::TranslateQWERTYtoQ_ELF(WCHAR key, std::wstring& output)
{
    if (kmp.KMPs.contains(key)) {
        output.clear();
        KMP::KMPelm selectedkmp = kmp.KMPs[key];
        std::wstring tmpbuf;
        if (selectedkmp.cmd == KMP::KMPelm::Cmd::Send) {
            if ((GetKeyState(VK_LSHIFT) & 0xf0000000) || (GetKeyState(VK_RSHIFT) & 0xf0000000)
                || (GetKeyState(VK_SHIFT) & 0xf0000000)) {
                kmp.is_key_pushed.shift = true;
            }

            switch (selectedkmp.to)
            {
            case ex_smalltu:
                kmp.is_key_pushed.sokuon = true;
                return false;
                break;
            case ex_nn:
                output = L"NN";
                return true;
                break;
            case ex_youon:
                kmp.is_key_pushed.youon = true;
                return false;
            case ex_boin:
                return false;
                break;
            case ex_sakujo:
                ResetKmpState();
            {
                INPUT input = { 0 };
                input.type = INPUT_KEYBOARD;
                input.ki.wVk = VK_BACK;
                SendInput(1, &input, sizeof(INPUT));
            }
                return false;
                break;
            default:
                output = selectedkmp.to;
                return true;
                break;
            }

            return false;
        }
        else if (selectedkmp.cmd == KMP::KMPelm::Cmd::DSend) {
            if (DSendCnt == 0) {
                DSendCnt++;

                Send1st = DsendElements{ selectedkmp.dir,selectedkmp.to,selectedkmp.tootherdir };
                if ((GetKeyState(VK_LSHIFT) & 0xf0000000) || (GetKeyState(VK_RSHIFT) & 0xf0000000)
                    || (GetKeyState(VK_SHIFT) & 0xf0000000)) {
                    kmp.is_key_pushed.shift = true;
                }
            }
            else {
                DSendCnt = 0;

                DsendElements Send2nd = DsendElements{ selectedkmp.dir,selectedkmp.to,selectedkmp.tootherdir };

                if (Send1st.dir1.dirkc != ex_boin) {
                    if (Send1st.dir == KMP::Dir::None || Send2nd.dir == KMP::Dir::None) {
                        output = std::wstring(1, WCHAR(Send1st.dir1.dirkc));
                    }
                    else if (Send1st.dir != Send2nd.dir) {//濁音
                        output = std::wstring(1, WCHAR(Send1st.dir1.samedirkc));
                        if (kmp.is_key_pushed.sokuon) {
                            output += std::wstring(1, WCHAR(Send1st.dir1.samedirkc));
                            kmp.is_key_pushed.sokuon = false;
                        }
                    }
                    else if (Send1st.dir == Send2nd.dir) {//清音
                        output = std::wstring(1, WCHAR(Send1st.otherdirkc));
                        if (kmp.is_key_pushed.sokuon) {
                            output += std::wstring(1, WCHAR(Send1st.otherdirkc));
                            kmp.is_key_pushed.sokuon = false;
                        }
                    }



                    
                    if (kmp.is_key_pushed.youon) {
                        output += L"Y";
                        kmp.is_key_pushed.youon = false;
                    }

                    //shift の処理は，呼出側が行う。
                }

                output += WCHAR(selectedkmp.secboin);
               
                return true;
            }
        }
    }

    return false;
}


void split(std::wstring string, char separator, std::vector<std::wstring>& list) {
    int separator_length = 1;

    list = std::vector<std::wstring>();

    auto offset = std::wstring::size_type(0);
    while (1) {
        auto pos = string.find(separator, offset);
        if (pos == std::wstring::npos) {
            list.push_back(string.substr(offset));
            break;
        }
        list.push_back(string.substr(offset, pos - offset));
        offset = pos + separator_length;
    }
}

void Q_ELF_Translater::LoadKmpFileFromFile(std::wstring& path)
{
	std::locale::global(std::locale("japanese"));

    kmp = KMP();

	std::wifstream kmpfile(path);

    kmpfile.imbue(std::locale("japanese"));
    enum CMode
    {
        def, keymap
    };

    CMode currentmode = def;

    std::unordered_map<std::wstring, KeyCode>ids;
    std::unordered_map<std::wstring, KMP::Dir>dirs;

    std::wstring line;
    while (std::getline(kmpfile, line)) {


        auto newwend = std::remove_if(line.begin(), line.end(), ::isspace);
        line.erase(newwend, line.end());

        if (line[0] == L'-')continue;
        if (line.empty())continue;

        if (line[0] == L'@') {
            line = line.substr(1);

            if (line == L"def") {
                currentmode = def;
            }
            else if (line == L"keymap") {
                currentmode = keymap;
            }
            continue;
        }

        if (line[0] == L'#' && currentmode == def) {
            line = line.substr(1);
            std::vector<std::wstring>sepl;
            split(line, L':', sepl);

            if (sepl[0] == L"!iddef") {
                ids[sepl[1]] = std::stoi(sepl[2], nullptr, 0);
                dirs[sepl[1]] = KMP::Dir::None;
            }
            else if (sepl[0] == L"dirdef") {
                if (sepl[2] == L"R") dirs[sepl[1]] = KMP::Dir::R;
                else if (sepl[2] == L"L") dirs[sepl[1]] = KMP::Dir::L;

            }
            continue;
        }

        if (currentmode == keymap) {
            std::vector<std::wstring>sepl;
            split(line, L':', sepl);

            KMP::KMPelm telm;
            KeyCode from = 0;

            telm.dir = dirs[sepl[0]];

            if (sepl[1] == L"Send") {
                telm.cmd = KMP::KMPelm::Cmd::Send;
                telm.to = ids[sepl[2]];
            }
            else if (sepl[1] == L"DSend") {
                telm.cmd = KMP::KMPelm::Cmd::DSend;

                std::vector<std::wstring>seplfs;
                split(sepl[2], L'-', seplfs);

                std::vector<std::wstring>seplfrl;
                split(seplfs[0], L'/', seplfrl);

                telm.to = ids[seplfrl[0]];

                if (seplfrl.size() >= 2) telm.tootherdir = ids[seplfrl[1]];
                else telm.tootherdir = ids[seplfrl[0]];

                telm.secboin =ids[seplfs[1]];
            }

            if (ids[sepl[0]] == ids[sepl[2]])continue;

            kmp.KMPs[ids[sepl[0]]] = telm;
        }
    }
    kmpfile.close();

}

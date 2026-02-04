#include <iostream>
#include <ShlObj.h>
#include <filesystem>
#include <urlmon.h>

#pragma comment(lib, "urlmon.lib")

namespace fs = std::filesystem;

bool no_installDir = false;

void PutSep() {
    std::cout << "-----------------------------------------" << std::endl;

}
void Log(const std::string& message) {
    std::cout << "[Installer] " << message << std::endl;
}
void skkUninstall();
void LogError(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;

    skkUninstall();
}

void WaitEnter() {
	system("pause");
}

bool DownloadDict(const std::wstring& url, const fs::path& destPath) {
    HRESULT hr=
    URLDownloadToFileW(NULL, url.c_str(), destPath.c_str(), 0, NULL);
    return SUCCEEDED(hr);
}

bool MoveDLLFile(fs::path& source_dll_path, fs::path& installdir, fs::path& install_dll_path) {
    if (!fs::exists(source_dll_path)) {
        LogError(source_dll_path.string() + "が見つかりませんでした。適切な位置に配置してください。");
        WaitEnter();
        return false;
    }
    Log("source_dll_path: " + source_dll_path.string());


    {
     
        if (!fs::exists(installdir)) {
            Log(installdir.string() + "が見つかりませんでした。作成します。");
            fs::create_directories(installdir);
            if (!fs::exists(installdir)) {
                LogError(installdir.string() + "を作成することが出来ませんでした。");
                WaitEnter();
                return false;
            }
        }

        fs::copy_file(source_dll_path, install_dll_path, fs::copy_options::overwrite_existing);

        Log(source_dll_path.string() + "は" + install_dll_path.string() + "適切に複製されました。");
    }
    return true;
}

bool ExecuteCmdRegSvr(fs::path &sysdir,fs::path& install_dll_path) {
    {


        std::string cmd = sysdir.string() + "\\regsvr32 /s \"" + install_dll_path.string() + "\"";
        if (std::system(cmd.c_str()) != 0) {
            LogError("コマンド[" + cmd + "]に失敗しました。");
            return false;
        }
        Log("コマンド[" + cmd + "]を実行しました。");
    }

    return true;
}

void skkInstall() {
    PutSep();
    Log("インストールを開始します。");
    WaitEnter();


    PutSep();

    //x64版

    fs::path source_dll_path = fs::current_path().string() + "\\tettySkk.dll";
    WCHAR sysPath[MAX_PATH];
    GetSystemDirectory(sysPath, MAX_PATH);
    fs::path installdir = std::wstring(sysPath) + L"\\IME\\tettySKK\\";
    fs::path install_dll_path = installdir.wstring() + L"tettySkk.dll";
    Log("IMEに必要なDLLファイルを適切な位置に設置しています。[1/2]");

    if (!MoveDLLFile(source_dll_path, installdir, install_dll_path))return;


    //x86版

    fs::path source_dll_pathx86 = fs::current_path().string() + "\\tettySkk32.dll";
    WCHAR sysPathx86[MAX_PATH];

    GetSystemWow64Directory(sysPathx86, MAX_PATH);

    fs::path installdirx86 = std::wstring(sysPathx86) + L"\\IME\\tettySKK\\";
    fs::path install_dll_pathx86 = installdirx86.wstring() + L"tettySkk.dll";
    Log("IMEに必要なDLLファイルを適切な位置に設置しています。[2/2]");

    if (!MoveDLLFile(source_dll_pathx86, installdirx86, install_dll_pathx86))return;

    

    Log("全てのdllファイルは適切に複製されました。");

    PutSep();

    {
        Log("IMEをwindowsに登録します。");

        fs::path psysPath = std::wstring(sysPath);
        if (!ExecuteCmdRegSvr(psysPath, install_dll_path))return;

        fs::path  psysPathx86 = std::wstring(sysPathx86);
        if (!ExecuteCmdRegSvr(psysPathx86, install_dll_pathx86))return;

        Log("windowsにIMEを登録することに成功しました。");

    }


    PutSep();

    {

        WCHAR wdir[MAX_PATH];
        SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, wdir);

        fs::path windowsDir = std::wstring(wdir);


        std::wstring dictUrl = L"http://openlab.jp/skk/skk/dic/SKK-JISYO.L";
        fs::path dictDir = windowsDir.string() + "\\tettySKK\\";
        fs::path dictPath = windowsDir.string() + "\\tettySKK\\skk-dict.txt";

        if (!fs::exists(dictPath.parent_path())) {
            Log(dictPath.parent_path().string() + "が見つかりませんでした。作成します。");
            fs::create_directories(dictPath.parent_path());
            if (!fs::exists(dictPath.parent_path())) {
                LogError(dictPath.parent_path().string() + "を作成することが出来ませんでした。");
                WaitEnter();
                return;
            }
        }

        if (!no_installDir) {

            Log("SKK-JISYO.L を " + dictPath.string() + " にインストールします。時間がかかる場合があります。");



            if (DownloadDict(dictUrl, dictPath)) {
                Log("辞書のダウンロードに成功しました。");

            }
            else {
                LogError("辞書のダウンロードに失敗しました。");

                WaitEnter();
                return;
            }
        }


        Log("Q-ELF配列で使用するkmp.kmpを移動します。");

        //kmp版

        fs::path source_dll_pathkmp = fs::current_path().string() + "\\kmp.kmp";
        fs::path install_dll_pathkmp = dictDir.wstring() + L"kmp.kmp";

        if (!MoveDLLFile(source_dll_pathkmp, dictDir, install_dll_pathkmp))return;
    }

    Log("必要な全てのファイルのインストールが完了しました。");
    Log("再起動しますか?[y:再起動 それ以外:再起動せず終了]");

    std::string res;
    std::cin >> res;
    if (res == "y") {
        Log("再起動します。");
        ExitWindowsEx(EWX_RESTARTAPPS, SHTDN_REASON_MINOR_TERMSRV);
    }

    return;
}

void skkUninstall() {
    PutSep();
    Log("アンインストールを開始します。");
    WaitEnter();

    PutSep();

    {

        WCHAR wdir[MAX_PATH];
        SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, wdir);

        fs::path windowsDir = std::wstring(wdir);

        fs::path dictPath = windowsDir.string() + "\\tettySKK\\skk-dict.txt";

        Log(dictPath.string() + "を削除します。時間がかかる場合があります。");
        if (fs::exists(dictPath)) {
            fs::remove(dictPath);
        }
    }


    PutSep();

    fs::path install_dll_path;
    fs::path install_dll_pathx86;
    {
        Log("IMEのwindowsの登録を外します。");

        {
            WCHAR sysPath[MAX_PATH];

            GetSystemDirectory(sysPath, MAX_PATH);

            fs::path installdir = std::wstring(sysPath);

            install_dll_path = installdir.wstring() + L"\\IME\\tettySKK\\"+ L"tettySkk.dll";
            std::string cmd = installdir.string() + "\\regsvr32 /s /u \"" + install_dll_path.string() + "\"";
            std::system(cmd.c_str());
            Log("コマンド[" + cmd + "]を実行しました。");
        }

        {
            WCHAR sysPath[MAX_PATH];

            GetSystemWow64Directory(sysPath, MAX_PATH);

            fs::path installdirx86 = std::wstring(sysPath);
            install_dll_pathx86 = installdirx86.wstring() + L"\\IME\\tettySKK\\" + L"tettySkk.dll";
            std::string cmd = installdirx86.string() + "\\regsvr32 /s /u \"" + install_dll_pathx86.string() + "\"";
            std::system(cmd.c_str()) ;
            Log("コマンド[" + cmd + "]を実行しました。");
        }
    }


    {
        {
            WCHAR sysPath[MAX_PATH];
            GetSystemDirectory(sysPath, MAX_PATH);

            fs::path installdir = std::wstring(sysPath) + L"\\IME\\tettySKK\\";
            install_dll_path = installdir.wstring() + L"tettySkk.dll";

            if (fs::exists(installdir)) {
				fs::remove(install_dll_path);
                fs::remove(installdir);
                
				Log(installdir.string() + "を削除しました。");
            }
        }
    }



    {
        {
            WCHAR sysPath[MAX_PATH];

            GetSystemWow64Directory(sysPath, MAX_PATH);

            fs::path installdirx86 = std::wstring(sysPath) + L"\\IME\\tettySKK\\";
            install_dll_pathx86 = installdirx86.wstring() + L"tettySkk.dll";

            if (fs::exists(installdirx86)) {
                fs::remove(install_dll_pathx86);
				fs::remove(installdirx86);
				Log(installdirx86.string() + "を削除しました。");
            }


        }
    }

    PutSep();
    Log("アンインストールが完了しました。最後にこのプログラムやプログラムの付属物を削除してください。");
    Log("プログラムを終了します。");
    WaitEnter();


}

int main()
{


    Log("Welcome tettySKK Installer");
    PutSep();
    Log("このプログラムでは，tettySKKのインストールもしくはアンインストールを行います。");

    if (IsUserAnAdmin() == FALSE) {
        LogError("管理者権限が必要です。");
        WaitEnter();
        return 0;
    }

    Log("インストールを希望する場合は，i を，アンインストール希望する場合は， u を入力してください。iiを入力すると辞書インストールなしでIMEを登録します。");

    {
        std::string ho;
        std::cin >> ho;
        if (ho == "i" || ho=="ii") {
            if (ho == "ii") {
                no_installDir = true;
            }
            skkInstall();
        }
        else if (ho == "u") {
            skkUninstall();
        }
        else {
            LogError("不明なコマンド[" + ho + "]です。");
            WaitEnter();
			return 0;
        }
    }


    return 0;    
}

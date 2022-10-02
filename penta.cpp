#include <Windows.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <time.h>
#include <winuser.h>

#include <fstream>
#include <iostream>
#include <vector>
#define buffSize 255

std::vector<wchar_t *> szText;

wchar_t *char2wchar(char *cchar, int ssize,int wsize) {
    wchar_t *m_wchar;
    int len = MultiByteToWideChar(CP_UTF8, 0, cchar, ssize, NULL, 0);
    m_wchar = new wchar_t[len + 1];
    MultiByteToWideChar(CP_UTF8, 0, cchar, ssize, m_wchar, wsize+1);
    m_wchar[len] = '\0';
    return m_wchar;
}

void readData() {
    char filename[] = "./data.txt";
    std::ifstream inFile(filename);

    printf("Welcome to HIT-PENREN tool. Come on, let's pensi these SB\n");

    int n = 0;
    std::string buff;
    while (std::getline(inFile, buff)) {
        // wchar_t *wchar = (wchar_t *)malloc(sizeof(wchar_t) * buffSize);
        // ZeroMemory(wchar, sizeof(wchar_t) * buffSize);

        // swprintf(wchar, buffSize, L"%S", buff);
        // cout << i << ": " << buff << endl;
        wchar_t *wt = char2wchar(&buff[0], strlen(&buff[0]),buff.size());
        szText.push_back(wt);
        buff.clear();
    }
    inFile.close();
}

void sendPaste() {
    INPUT inputs[4] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 0x56;

    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 0x56;
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_CONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

    UINT uSent = SendInput(4, inputs, sizeof(INPUT));
}

void sendEnter() {
    INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_RETURN;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_RETURN;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    UINT uSent = SendInput(2, inputs, sizeof(INPUT));
}

void sendClipboard() {
    srand((unsigned int)time(NULL));
    int ra = rand() % szText.size();
    wchar_t *text = szText[ra];
    // printf("%d: ", ra);
    // wprintf(L"%S\n", text);

    HWND hWnd = GetDesktopWindow();
    if (OpenClipboard(hWnd)) {
        EmptyClipboard();
        HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, buffSize);
        LPWSTR pData = (LPWSTR)GlobalLock(hData);

        CopyMemory(pData, text, buffSize);

        GlobalUnlock(hData);

        SetClipboardData(CF_UNICODETEXT, hData);

        CloseClipboard();
    }
}

bool mark = false;
void *kaipen(void *arg) {
    while (1) {
        if (mark) {
            sendClipboard();
            sendPaste();
            sendEnter();
        }
        Sleep(1000);
    }
}

void process() {
    if (RegisterHotKey(NULL, 1, MOD_ALT | 0x4000, 0x42))  // 0x42 is 'b'
    {
        _tprintf(_T("'alt+b' start\n"));
    }

    if (RegisterHotKey(NULL, 1, MOD_ALT | 0x4000, 0x43))  // 0x42 is 'c'
    {
        _tprintf(_T("'alt+c' stop\n"));
    }

    pthread_t ntid;
    int err;
    err = pthread_create(&ntid, NULL, kaipen, NULL);
    if (err != 0) return;

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        if (msg.message == WM_HOTKEY) {
            if (msg.lParam == 4390913) {
                _tprintf(_T("stop penren\n"));
                mark = false;
            }

            if (msg.lParam == 4325377) {
                _tprintf(_T("start penren\n"));
                _tprintf(_T("running\n"));
                mark = true;
            }
        }
    }
}

int main() {

    readData();
    process();
    return 0;
}
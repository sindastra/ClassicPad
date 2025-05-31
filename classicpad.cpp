#include <windows.h>
#include <commdlg.h>

const char CLASS_NAME[] = "MiniNotepad";
HWND hEdit;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        hEdit = CreateWindowEx(0, "EDIT", "",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE |
            ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_TABSTOP,
            0, 0, 0, 0, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);
        break;
    }
    case WM_SIZE: {
        MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        break;
    }
    case WM_SETFOCUS: {
        SetFocus(hEdit);
        return 0;
    }
    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case 100: { // Open
            OPENFILENAME ofn = { sizeof(ofn) };
            char fileName[MAX_PATH] = "";
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
            ofn.lpstrFile = fileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
            if (GetOpenFileName(&ofn)) {
                HANDLE hFile = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
                if (hFile != INVALID_HANDLE_VALUE) {
                    DWORD size = GetFileSize(hFile, NULL);
                    char* buffer = new char[size + 1];
                    DWORD read;
                    ReadFile(hFile, buffer, size, &read, NULL);
                    buffer[read] = '\0';
                    SetWindowText(hEdit, buffer);
                    delete[] buffer;
                    CloseHandle(hFile);
                }
            }
            break;
        }
        case 101: { // Save
            OPENFILENAME ofn = { sizeof(ofn) };
            char fileName[MAX_PATH] = "";
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
            ofn.lpstrFile = fileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_OVERWRITEPROMPT;
            if (GetSaveFileName(&ofn)) {
                int length = GetWindowTextLength(hEdit);
                char* buffer = new char[length + 1];
                GetWindowText(hEdit, buffer, length + 1);
                HANDLE hFile = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
                if (hFile != INVALID_HANDLE_VALUE) {
                    DWORD written;
                    WriteFile(hFile, buffer, length, &written, NULL);
                    CloseHandle(hFile);
                }
                delete[] buffer;
            }
            break;
        }
        case 102: SendMessage(hEdit, WM_CUT, 0, 0); break;
        case 103: SendMessage(hEdit, WM_COPY, 0, 0); break;
        case 104: SendMessage(hEdit, WM_PASTE, 0, 0); break;
        case 105: PostQuitMessage(0); break;
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void AddMenus(HWND hwnd) {
    HMENU hMenubar = CreateMenu();
    HMENU hFile = CreateMenu();
    HMENU hEditMenu = CreateMenu();

    AppendMenu(hFile, MF_STRING, 100, "Open");
    AppendMenu(hFile, MF_STRING, 101, "Save");
    AppendMenu(hFile, MF_STRING, 105, "Exit");

    AppendMenu(hEditMenu, MF_STRING, 102, "Cut");
    AppendMenu(hEditMenu, MF_STRING, 103, "Copy");
    AppendMenu(hEditMenu, MF_STRING, 104, "Paste");

    AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFile, "File");
    AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hEditMenu, "Edit");

    SetMenu(hwnd, hMenubar);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Mini Notepad",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
        NULL, NULL, hInstance, NULL);

    AddMenus(hwnd);

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
        DispatchMessage(&msg);
    return 0;
}

#include <windows.h>
#include <commdlg.h>
#include <fstream>
#include <string>

#define IDM_FILE_OPEN 9000
#define IDM_FILE_SAVE 9001
#define IDM_FILE_EXIT 9002

const char CLASS_NAME[] = "ClassicPadWindow";

HWND hEdit;

void LoadFile(HWND hwnd, const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        MessageBox(hwnd, "Cannot open file.", "Error", MB_OK | MB_ICONERROR);
        return;
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    SetWindowText(hEdit, content.c_str());
}

void SaveFile(HWND hwnd, const char* filename) {
    int length = GetWindowTextLength(hEdit);
    char* buffer = new char[length + 1];
    GetWindowText(hEdit, buffer, length + 1);

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        MessageBox(hwnd, "Cannot save file.", "Error", MB_OK | MB_ICONERROR);
        delete[] buffer;
        return;
    }
    file.write(buffer, length);
    delete[] buffer;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        hEdit = CreateWindowEx(
            0, "EDIT", "",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_TABSTOP,
            0, 0, 0, 0,
            hwnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        if (!hEdit) {
            MessageBox(hwnd, "Could not create edit control.", "Error", MB_OK | MB_ICONERROR);
            return -1;
        }

        HMENU hMenu = CreateMenu();
        HMENU hFileMenu = CreatePopupMenu();
        AppendMenu(hFileMenu, MF_STRING, IDM_FILE_OPEN, "&Open");
        AppendMenu(hFileMenu, MF_STRING, IDM_FILE_SAVE, "&Save");
        AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hFileMenu, MF_STRING, IDM_FILE_EXIT, "E&xit");
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, "&File");
        SetMenu(hwnd, hMenu);
    } break;

    case WM_SIZE:
        if (hEdit) {
            MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        }
        break;

    case WM_SETFOCUS:
        if (hEdit) SetFocus(hEdit);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_FILE_OPEN: {
            OPENFILENAME ofn = {};
            char filename[MAX_PATH] = {};
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = filename;
            ofn.nMaxFile = sizeof(filename);
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
            ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
            if (GetOpenFileName(&ofn)) {
                LoadFile(hwnd, filename);
            }
        } break;

        case IDM_FILE_SAVE: {
            OPENFILENAME ofn = {};
            char filename[MAX_PATH] = {};
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = filename;
            ofn.nMaxFile = sizeof(filename);
            ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
            ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
            if (GetSaveFileName(&ofn)) {
                SaveFile(hwnd, filename);
            }
        } break;

        case IDM_FILE_EXIT:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_IBEAM);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Failed to register window class.", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "ClassicPad\x99",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
        NULL, NULL, hInstance, NULL);

    if (!hwnd) {
        MessageBox(NULL, "Failed to create window.", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

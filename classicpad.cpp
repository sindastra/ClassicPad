#include <windows.h>

const char CLASS_NAME[] = "ClassicPadWindow";

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hEdit;

    switch (uMsg) {
    case WM_CREATE:
        hEdit = CreateWindowEx(
            0, "EDIT", "",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_TABSTOP,
            0, 0, 0, 0,
            hwnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        if (!hEdit) {
            MessageBox(hwnd, "Could not create edit control.", "Error", MB_OK | MB_ICONERROR);
            return -1;
        }
        break;

    case WM_SIZE:
        if (hEdit) {
            MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        }
        break;

    case WM_SETFOCUS:
        if (hEdit) {
            SetFocus(hEdit);
        }
        break;

    case WM_ACTIVATE:
        if (hEdit && wParam != WA_INACTIVE) {
            SetFocus(hEdit);
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

// WinMain entry point to suppress console window
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
        "ClassicPad™",
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

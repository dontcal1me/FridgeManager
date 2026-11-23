/* gui_dialog.c
 * 담당: 윤종하
 * 설명: 모달 대화상자 틀. 창 클래스 하나를 공유하고, 대화상자마다 다른 동작은 DlgBase의 함수 포인터로 연결한다.
 */
#include "gui_dialog.h"

#define DLG_CLASS  L"FridgeDlgWnd"
/* ---------------------------------------------------------------- 모달 대화상자 틀 */

static LRESULT CALLBACK DlgProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    DlgBase *b = (DlgBase *)GetWindowLongPtrW(h, GWLP_USERDATA);

    switch (m) {
    case WM_CREATE:
        b = (DlgBase *)((CREATESTRUCTW *)l)->lpCreateParams;
        SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)b);
        if (b->onCreate) b->onCreate(h, b);
        return 0;
    case WM_COMMAND:
        if (b && b->onCommand) b->onCommand(h, b, LOWORD(w), HIWORD(w));
        return 0;
    case WM_NOTIFY:
        if (b && b->onNotify) return b->onNotify(h, b, (NMHDR *)l);
        return 0;
    case WM_TIMER:
        if (b && b->onTimer) b->onTimer(h, b, (UINT_PTR)w);
        return 0;
    case WM_CLOSE:
        if (b) b->done = 1;
        return 0;
    }
    if (m >= WM_APP && m < 0xC000 && b && b->onUser)
        return b->onUser(h, b, m, w, l);
    return DefWindowProcW(h, m, w, l);
}

/* 대화상자를 owner 위에 띄우고 닫힐 때까지 자기 메시지 루프를 돈다 (cw, ch: 클라이언트 크기, 96dpi) */
void runDialog(HWND owner, const wchar_t *title, int cw, int ch, DlgBase *b)
{
    RECT r, o;
    DWORD style = WS_POPUP | WS_CAPTION | WS_SYSMENU;
    DWORD ex = WS_EX_DLGMODALFRAME;
    HWND h;
    MSG m;
    int x, y, rc = 1;

    r.left = 0; r.top = 0; r.right = S(cw); r.bottom = S(ch);
    AdjustWindowRectEx(&r, style, FALSE, ex);
    GetWindowRect(owner, &o);
    x = o.left + ((o.right - o.left) - (r.right - r.left)) / 2;
    y = o.top + ((o.bottom - o.top) - (r.bottom - r.top)) / 2;
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    b->done = 0;
    h = CreateWindowExW(ex, DLG_CLASS, title, style, x, y, r.right - r.left, r.bottom - r.top,
                        owner, NULL, GetModuleHandleW(NULL), b);
    if (h == NULL) return;

    EnableWindow(owner, FALSE);
    ShowWindow(h, SW_SHOW);
    UpdateWindow(h);
    while (!b->done && (rc = GetMessageW(&m, NULL, 0, 0)) > 0) {
        if (!IsDialogMessageW(h, &m)) {
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }
    }
    if (rc == 0) PostQuitMessage((int)m.wParam);        /* 종료 요청은 메인 루프에 다시 전달 */
    EnableWindow(owner, TRUE);
    DestroyWindow(h);
    SetActiveWindow(owner);
}

/* 대화상자 창 클래스를 등록한다 (프로그램 시작 때 한 번). 성공하면 1 */
int registerDialogClass(HINSTANCE hInst)
{
    WNDCLASSEXW wc;

    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.hInstance = hInst;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpfnWndProc = DlgProc;
    wc.lpszClassName = DLG_CLASS;
    return RegisterClassExW(&wc) != 0;
}

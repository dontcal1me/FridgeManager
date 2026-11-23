/* ui_gui.c
 * 담당: 윤종하
 * 설명: GUI 메인 창 (식재료 표, 기능 버튼, 시작 알림). 대화상자는 dlg_*.c, 공통 도우미는 gui_common.c에 있다.
 *       로직 모듈의 함수만 호출하고 구조체 내부는 직접 고치지 않는다.
 */
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "ui_gui.h"
#include "gui_common.h"
#include "gui_dialog.h"
#include "dlg_food.h"
#include "dlg_simple.h"
#include "dlg_recipe.h"
#include "dlg_recommend.h"
#include "food.h"
#include "expiry.h"
#include "recipe.h"
#include "stats.h"

#define MAIN_CLASS  L"FridgeMainWnd"

/* 메인 창 컨트롤 ID */
enum {
    IDC_LIST = 100, IDC_ADD, IDC_EDIT, IDC_QTY, IDC_DEL,
    IDC_RECOMMEND, IDC_ADDRECIPE, IDC_DISCARD, IDC_STATS, IDC_STATUS
};
/* ---------------------------------------------------------------- 메인 창 동작 */

static int selIndex(void)
{
    return (int)SendMessageW(g_list, LVM_GETNEXTITEM, (WPARAM)-1, LVNI_SELECTED);
}

/* 선택된 행 번호를 반환한다. 선택이 없으면 안내하고 -1 */
static int needSel(void)
{
    int i = selIndex();
    if (i < 0 || i >= g_app->fridge.count) {
        msg(g_main, L"목록에서 재료를 먼저 선택하세요.", MB_ICONINFORMATION);
        return -1;
    }
    return i;
}

/* 이름·유통기한·보관 방법이 같은 재료의 위치를 찾는다. skip번은 건너뛴다. 없으면 -1 */
static int findSame(const Food *f, int skip)
{
    int i;
    for (i = 0; i < g_app->fridge.count; i++) {
        const Food *x = &g_app->fridge.items[i];
        if (i == skip) continue;
        if (strcmp(x->name, f->name) == 0 && x->storage == f->storage &&
            x->expire.year == f->expire.year && x->expire.month == f->expire.month &&
            x->expire.day == f->expire.day)
            return i;
    }
    return -1;
}

/* 합쳐진 재료를 표에서 찾아 선택해 보여 주고 안내한다 */
static void showMerged(const Food *f, int oldQty, int newQty)
{
    wchar_t w[96], m[200];
    int idx = findSame(f, -1);

    ansiToWide(f->name, w, 96);
    if (idx >= 0) lvSelect(g_list, idx);
    swprintf(m, 200, L"'%s'은(는) 이미 같은 재료(유통기한·보관 방법 동일)가 있어\n수량을 합쳤습니다.\n\n%d개 → %d개", w, oldQty, newQty);
    setNote(L"'%s' 수량을 합쳤습니다 (%d개 → %d개)", w, oldQty, newQty);
    msg(g_main, m, MB_ICONINFORMATION);
}

static void onAdd(void)
{
    Food f;
    wchar_t w[96];
    int same, oldQty = 0;
    ResultCode rc;

    memset(&f, 0, sizeof(f));
    if (!foodDialog(g_main, &f, 0)) return;

    same = findSame(&f, -1);                          /* 같은 재료가 이미 있는지 (합치기 안내용) */
    if (same >= 0) oldQty = g_app->fridge.items[same].quantity;

    rc = addFood(&g_app->fridge, &f);                 /* 같은 재료면 addFood가 수량을 합쳐 준다 */
    if (rc == RC_NO_MEMORY) { msg(g_main, L"메모리가 부족합니다.", MB_ICONERROR); return; }
    if (rc != RC_OK)        { msg(g_main, L"입력값이 올바르지 않습니다.", MB_ICONERROR); return; }

    if (same >= 0) {
        refreshMain();
        showMerged(&f, oldQty, oldQty + f.quantity);
        refreshMain();                                /* 안내 문구를 상태줄에 반영 */
        return;
    }
    ansiToWide(f.name, w, 96);
    setNote(L"'%s' 등록 완료", w);
    refreshMain();
    {
        int idx = findSame(&f, -1);
        if (idx >= 0) lvSelect(g_list, idx);          /* 방금 등록한 재료를 선택 */
    }
}

static void onEdit(void)
{
    int i = needSel();
    int same;
    Food f;
    wchar_t w[96];

    if (i < 0) return;
    f = g_app->fridge.items[i];
    if (!foodDialog(g_main, &f, 1)) return;

    same = findSame(&f, i);                           /* 수정한 결과가 다른 재료와 같아지면 합친다 */
    if (same >= 0) {
        Food other = g_app->fridge.items[same];
        int oldQty = other.quantity;
        other.quantity += f.quantity;
        if (updateFood(&g_app->fridge, same, &other) != RC_OK) {
            msg(g_main, L"합치지 못했습니다.", MB_ICONERROR);
            return;
        }
        removeFood(&g_app->fridge, i);                /* 수정하던 재료는 합쳤으니 삭제 */
        refreshMain();
        showMerged(&other, oldQty, other.quantity);
        refreshMain();
        return;
    }

    if (updateFood(&g_app->fridge, i, &f) != RC_OK) {
        msg(g_main, L"수정하지 못했습니다.", MB_ICONERROR);
        return;
    }
    ansiToWide(f.name, w, 96);
    setNote(L"'%s' 수정 완료", w);
    refreshMain();
}

/* 삭제. 폐기로 기록할지 묻는다 (F6 연결) */
static void onDelete(void)
{
    int i = needSel();
    Food item;
    wchar_t w[96], m[200];

    if (i < 0) return;
    item = g_app->fridge.items[i];
    ansiToWide(item.name, w, 96);
    swprintf(m, 200, L"'%s'을(를) 삭제할까요?", w);
    if (!ask(g_main, m)) return;
    if (ask(g_main, L"폐기로 기록할까요?\n(\"예\"를 누르면 폐기 통계에 반영됩니다)"))
        addWaste(&g_app->waste, &item, getToday());
    removeFood(&g_app->fridge, i);
    setNote(L"'%s' 삭제 완료", w);
    refreshMain();
}

/* 수량 변경: 줄이면 useFood, 늘리면 updateFood */
static void onQty(void)
{
    int i = needSel();
    Food item;
    int oldQty, newQty;
    wchar_t w[96], m[200];

    if (i < 0) return;
    item = g_app->fridge.items[i];
    oldQty = item.quantity;
    ansiToWide(item.name, w, 96);
    swprintf(m, 200, L"'%s'의 새 수량을 입력하세요. (1~999)", w);
    if (!promptNumber(g_main, L"수량 변경", m, oldQty, 1, 999, &newQty)) return;

    if (newQty == oldQty) return;
    if (newQty < oldQty) {
        useFood(&g_app->fridge, i, oldQty - newQty);
    } else {
        item.quantity = newQty;
        updateFood(&g_app->fridge, i, &item);
    }
    setNote(L"'%s' 수량 %d → %d", w, oldQty, newQty);
    refreshMain();
}

/* 기한 지난 재료 일괄 폐기 */
static void onDiscard(void)
{
    int expired, soon, n;
    wchar_t m[128];

    countAlerts(&g_app->fridge, &expired, &soon);
    if (expired == 0) {
        msg(g_main, L"기한 지난 재료가 없습니다.", MB_ICONINFORMATION);
        return;
    }
    swprintf(m, 128, L"기한 지난 재료 %d개를 폐기 처리할까요?", expired);
    if (!ask(g_main, m)) return;
    n = discardExpired(&g_app->fridge, &g_app->waste);
    setNote(L"기한 지난 재료 %d개를 폐기 처리했습니다", n);
    refreshMain();
}

/* G5: 폐기 통계 창 */
static void onStats(void)
{
    WasteStats s;
    wchar_t text[600], nm[96];
    int fridgePct, freezerPct;

    computeWasteStats(&g_app->waste, getToday(), &s);
    if (s.totalRecords == 0) {
        textDialog(g_main, L"폐기 기록 통계", L"아직 폐기 기록이 없습니다.");
        return;
    }
    fridgePct = s.fridgeCount * 100 / s.totalRecords;
    freezerPct = s.freezerCount * 100 / s.totalRecords;
    ansiToWide(s.topName, nm, 96);

    text[0] = L'\0';
    catf(text, 600, L"총 폐기 횟수:  %d회 (재료 %d개)\n\n", s.totalRecords, s.totalQuantity);
    catf(text, 600, L"냉장 / 냉동 비율:  냉장 %d%% / 냉동 %d%%\n\n", fridgePct, freezerPct);
    catf(text, 600, L"가장 많이 버린 재료:  %s (%d회)\n\n", nm, s.topCount);
    catf(text, 600, L"이번 달 폐기 횟수:  %d회\n\n", s.thisMonth);
    if (fridgePct >= 70)
        catf(text, 600, L"[팁] 냉장 재료 폐기가 많아요. 장보기 양을 조금 줄여 보세요!");
    else if (freezerPct >= 50)
        catf(text, 600, L"[팁] 냉동 재료도 오래 두면 버리게 돼요. 냉동실을 가끔 확인해 보세요!");
    else
        catf(text, 600, L"[팁] 재료를 잘 관리하고 있어요. 임박 알림을 계속 활용해 보세요!");
    textDialog(g_main, L"폐기 기록 통계", text);
}

static void onAddRecipe(void)
{
    wchar_t nm[64];
    if (recipeDialog(g_main, nm, 64)) {
        setNote(L"레시피 '%s'을(를) 추가했습니다", nm);
        refreshMain();
    }
}

/* ---------------------------------------------------------------- 메인 창 */

/* 오른쪽 버튼 줄과 표, 상태줄의 위치를 창 크기에 맞춘다 */
static void layoutMain(int w, int h)
{
    static const int ids[] = { IDC_ADD, IDC_EDIT, IDC_QTY, IDC_DEL,
                               IDC_RECOMMEND, IDC_ADDRECIPE, IDC_DISCARD, IDC_STATS };
    static const int groupGap[] = { 0, 0, 0, 0, 1, 0, 1, 0 };     /* 1이면 이 버튼 앞에 간격을 더 둔다 */
    int M = S(12), bw = S(170), bh = S(34), gap = S(8), sh = S(24), y = M, i;

    MoveWindow(g_list, M, M, w - bw - 3 * M, h - 3 * M - sh, TRUE);
    MoveWindow(g_status, M, h - M - sh, w - 2 * M, sh, TRUE);
    for (i = 0; i < 8; i++) {
        if (groupGap[i]) y += M;
        MoveWindow(GetDlgItem(g_main, ids[i]), w - M - bw, y, bw, bh, TRUE);
        y += bh + gap;
    }
}

static LRESULT CALLBACK MainProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    switch (m) {
    case WM_CREATE:
        g_main = h;
        g_list = mk(h, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | WS_TABSTOP,
                    IDC_LIST, 0, 0, 100, 100);
        SendMessageW(g_list, LVM_SETEXTENDEDLISTVIEWSTYLE, 0,
                     LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
        lvCol(g_list, 0, L"이름", 170, LVCFMT_LEFT);
        lvCol(g_list, 1, L"수량", 60, LVCFMT_RIGHT);
        lvCol(g_list, 2, L"보관", 60, LVCFMT_CENTER);
        lvCol(g_list, 3, L"유통기한", 110, LVCFMT_CENTER);
        lvCol(g_list, 4, L"남은 기간", 130, LVCFMT_LEFT);
        g_status = mk(h, L"STATIC", L"", SS_LEFT | SS_CENTERIMAGE, IDC_STATUS, 0, 0, 100, 24);
        mk(h, L"BUTTON", L"식재료 등록", BS_PUSHBUTTON | WS_TABSTOP, IDC_ADD, 0, 0, 10, 10);
        mk(h, L"BUTTON", L"수정", BS_PUSHBUTTON | WS_TABSTOP, IDC_EDIT, 0, 0, 10, 10);
        mk(h, L"BUTTON", L"수량 변경", BS_PUSHBUTTON | WS_TABSTOP, IDC_QTY, 0, 0, 10, 10);
        mk(h, L"BUTTON", L"삭제", BS_PUSHBUTTON | WS_TABSTOP, IDC_DEL, 0, 0, 10, 10);
        mk(h, L"BUTTON", L"요리 추천", BS_PUSHBUTTON | WS_TABSTOP, IDC_RECOMMEND, 0, 0, 10, 10);
        mk(h, L"BUTTON", L"레시피 추가", BS_PUSHBUTTON | WS_TABSTOP, IDC_ADDRECIPE, 0, 0, 10, 10);
        mk(h, L"BUTTON", L"기한 지난 재료 폐기", BS_PUSHBUTTON | WS_TABSTOP, IDC_DISCARD, 0, 0, 10, 10);
        mk(h, L"BUTTON", L"폐기 통계", BS_PUSHBUTTON | WS_TABSTOP, IDC_STATS, 0, 0, 10, 10);
        refreshMain();
        return 0;

    case WM_SIZE:
        if (w != SIZE_MINIMIZED) layoutMain(LOWORD(l), HIWORD(l));
        return 0;

    case WM_GETMINMAXINFO: {
        MINMAXINFO *mm = (MINMAXINFO *)l;
        mm->ptMinTrackSize.x = S(720);
        mm->ptMinTrackSize.y = S(480);
        return 0;
    }

    case WM_SETFOCUS:
        SetFocus(g_list);
        return 0;

    case WM_COMMAND:
        if (HIWORD(w) == BN_CLICKED) {
            switch (LOWORD(w)) {
            case IDC_ADD:       onAdd();       break;
            case IDC_EDIT:      onEdit();      break;
            case IDC_QTY:       onQty();       break;
            case IDC_DEL:       onDelete();    break;
            case IDC_RECOMMEND: recommendDialog(h); break;
            case IDC_ADDRECIPE: onAddRecipe(); break;
            case IDC_DISCARD:   onDiscard();   break;
            case IDC_STATS:     onStats();     break;
            }
        }
        return 0;

    case WM_NOTIFY: {
        NMHDR *nh = (NMHDR *)l;
        if (nh->hwndFrom == g_list) {
            if (nh->code == NM_DBLCLK) {                         /* 더블클릭: 수정 */
                if (selIndex() >= 0) onEdit();
            } else if (nh->code == LVN_KEYDOWN) {                /* Delete 키: 삭제 */
                if (((NMLVKEYDOWN *)l)->wVKey == VK_DELETE && selIndex() >= 0) onDelete();
            } else if (nh->code == NM_CUSTOMDRAW) {              /* G5: 기한 지남 빨강, 임박 주황 */
                NMLVCUSTOMDRAW *cd = (NMLVCUSTOMDRAW *)l;
                if (cd->nmcd.dwDrawStage == CDDS_PREPAINT)
                    return CDRF_NOTIFYITEMDRAW;
                if (cd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
                    int i = (int)cd->nmcd.dwItemSpec;
                    if (i >= 0 && i < g_app->fridge.count) {
                        int d = daysLeft(g_app->fridge.items[i].expire);
                        if (d < 0)                cd->clrText = RGB(192, 0, 0);
                        else if (d <= ALERT_DAYS) cd->clrText = RGB(214, 104, 0);
                    }
                    return CDRF_NEWFONT;
                }
                return CDRF_DODEFAULT;
            }
        }
        return 0;
    }

    case WM_CLOSE:
        DestroyWindow(h);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

/* ---------------------------------------------------------------- G1: 시작 알림 */

/* 기한이 지난(expiredPass=1) 또는 임박한(0) 재료 이름을 한 줄씩 덧붙인다. 많으면 8개까지만 */
static void appendAlertItems(wchar_t *buf, size_t cap, const Fridge *f, int expiredPass)
{
    int i, shown = 0, total = 0;
    wchar_t nm[96], dd[32];

    for (i = 0; i < f->count; i++) {
        int d = daysLeft(f->items[i].expire);
        int hit = expiredPass ? (d < 0) : (d >= 0 && d <= ALERT_DAYS);
        char a[16];
        if (!hit) continue;
        total++;
        if (shown < 8) {
            ansiToWide(f->items[i].name, nm, 96);
            formatDday(d, a, sizeof(a));
            ansiToWide(a, dd, 32);
            catf(buf, cap, L"    - %s (%s)\n", nm, dd);
            shown++;
        }
    }
    if (total > shown)
        catf(buf, cap, L"    ... 외 %d개\n", total - shown);
}

void guiShowStartupAlert(const Fridge *f)
{
    int expired = 0, soon = 0;
    wchar_t text[1200], day[32];
    Date today = getToday();

    countAlerts(f, &expired, &soon);
    if (expired == 0 && soon == 0) return;

    text[0] = L'\0';
    dateStr(today, day, 32);
    catf(text, 1200, L"오늘 날짜: %s\n\n", day);
    catf(text, 1200, L"유통기한 지난 재료: %d개\n", expired);
    appendAlertItems(text, 1200, f, 1);
    catf(text, 1200, L"\n%d일 이내 임박 재료: %d개\n", ALERT_DAYS, soon);
    appendAlertItems(text, 1200, f, 0);
    MessageBoxW(NULL, text, L"유통기한 알림", MB_OK | MB_ICONWARNING);
}

/* ---------------------------------------------------------------- 실행 */

int guiRun(HINSTANCE hInst, AppData *app, int nShow)
{
    INITCOMMONCONTROLSEX ic;
    WNDCLASSEXW wc;
    RECT r;
    HDC dc;
    MSG m;

    g_app = app;
    SetProcessDPIAware();                                    /* 고해상도 화면에서 흐려지지 않게 */
    dc = GetDC(NULL);
    g_dpi = GetDeviceCaps(dc, LOGPIXELSX);
    ReleaseDC(NULL, dc);

    ic.dwSize = sizeof(ic);
    ic.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&ic);

    g_font = CreateFontW(-S(15), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, HANGUL_CHARSET,
                         OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                         DEFAULT_PITCH | FF_DONTCARE, L"맑은 고딕");

    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.hInstance = hInst;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpfnWndProc = MainProc;
    wc.lpszClassName = MAIN_CLASS;
    if (!RegisterClassExW(&wc)) return 1;
    if (!registerDialogClass(hInst)) return 1;               /* 대화상자 창 클래스 (gui_dialog.c) */

    r.left = 0; r.top = 0; r.right = S(820); r.bottom = S(520);
    AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);
    g_main = CreateWindowExW(0, MAIN_CLASS, APP_TITLE, WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top,
                             NULL, NULL, hInst, NULL);
    if (g_main == NULL) return 1;
    ShowWindow(g_main, nShow);
    UpdateWindow(g_main);

    while (GetMessageW(&m, NULL, 0, 0) > 0) {
        if (!IsDialogMessageW(g_main, &m)) {
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }
    }
    DeleteObject(g_font);
    return (int)m.wParam;
}

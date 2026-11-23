/* gui_common.c
 * 담당: 윤종하
 * 설명: GUI 공통 전역 상태와 도우미 함수 (문자 변환, 메시지 창, 컨트롤 만들기, 표 도우미, 메인 목록 갱신)
 *       데이터 파일은 ANSI(949), 화면은 유니코드이므로 표시·입력 때 변환한다. (README 13.3장)
 */
#include "gui_common.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#include "expiry.h"
#include "food.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

AppData *g_app;
HWND     g_main, g_list, g_status;
HFONT    g_font;
int      g_dpi = 96;
static wchar_t g_note[128];                   /* 상태줄에 보여 줄 마지막 작업 */

/* 96dpi 기준 픽셀 값을 현재 화면 배율로 바꾼다 */
int S(int v) { return MulDiv(v, g_dpi, 96); }
/* ---------------------------------------------------------------- 변환·문자열 도우미 */

/* ANSI(949) → 유니코드 */
void ansiToWide(const char *src, wchar_t *dst, int dstLen)
{
    if (MultiByteToWideChar(DATA_CP, 0, src, -1, dst, dstLen) == 0)
        dst[0] = L'\0';
}

/* 유니코드 → ANSI(949). 길이가 넘치거나 949에 없는 글자가 있으면 0 */
int wideToAnsi(const wchar_t *src, char *dst, int dstLen)
{
    BOOL used = FALSE;
    if (WideCharToMultiByte(DATA_CP, 0, src, -1, dst, dstLen, NULL, &used) == 0)
        return 0;
    return !used;
}

/* 앞뒤 공백 제거 */
void trim(wchar_t *s)
{
    wchar_t *a = s;
    size_t n;
    while (*a && iswspace(*a)) a++;
    if (a != s) memmove(s, a, (wcslen(a) + 1) * sizeof(wchar_t));
    n = wcslen(s);
    while (n > 0 && iswspace(s[n - 1])) s[--n] = L'\0';
}

int hasSpace(const wchar_t *s)
{
    for (; *s; s++)
        if (iswspace(*s)) return 1;
    return 0;
}

/* buf 끝에 서식 문자열을 덧붙인다 (넘치면 잘라 낸다) */
void catf(wchar_t *buf, size_t cap, const wchar_t *fmt, ...)
{
    size_t len = wcslen(buf);
    va_list ap;
    if (len + 1 >= cap) return;
    va_start(ap, fmt);
    _vsnwprintf(buf + len, cap - len - 1, fmt, ap);
    va_end(ap);
    buf[cap - 1] = L'\0';
}

void dateStr(Date d, wchar_t *buf, int n)
{
    swprintf(buf, n, L"%04d-%02d-%02d", d.year, d.month, d.day);
}

/* "YYYY-MM-DD"를 읽어 올바른 날짜면 1 */
int parseDate(const wchar_t *s, Date *d)
{
    int y, m, dd;
    wchar_t tail;
    if (swscanf(s, L"%d-%d-%d%lc", &y, &m, &dd, &tail) != 3) return 0;
    d->year = y;
    d->month = m;
    d->day = dd;
    return isValidDate(*d);
}

/* D-day 문자열 (예: "D-3 (임박)") */
void ddayText(int days, wchar_t *buf, int n)
{
    char a[16];
    wchar_t w[16];
    formatDday(days, a, sizeof(a));
    ansiToWide(a, w, 16);
    if (days < 0)               swprintf(buf, n, L"%s (지남)", w);
    else if (days <= ALERT_DAYS) swprintf(buf, n, L"%s (임박)", w);
    else                        swprintf(buf, n, L"%s", w);
}

/* ---------------------------------------------------------------- 메시지·컨트롤 도우미 */

void msg(HWND owner, const wchar_t *text, UINT icon)
{
    MessageBoxW(owner, text, APP_TITLE, MB_OK | icon);
}

int ask(HWND owner, const wchar_t *text)
{
    return MessageBoxW(owner, text, APP_TITLE, MB_YESNO | MB_ICONQUESTION) == IDYES;
}

/* 입력이 잘못됐을 때: 안내 후 해당 입력칸으로 커서를 옮긴다 */
void bad(HWND dlg, int id, const wchar_t *text)
{
    msg(dlg, text, MB_ICONWARNING);
    SetFocus(GetDlgItem(dlg, id));
}

/* 컨트롤을 만들고 글꼴을 입힌다. 좌표는 96dpi 기준 */
HWND mk(HWND parent, const wchar_t *cls, const wchar_t *text, DWORD style, int id,
               int x, int y, int w, int h)
{
    DWORD ex = (wcscmp(cls, L"EDIT") == 0 || wcscmp(cls, WC_LISTVIEWW) == 0) ? WS_EX_CLIENTEDGE : 0;
    HWND c = CreateWindowExW(ex, cls, text, WS_CHILD | WS_VISIBLE | style,
                             S(x), S(y), S(w), S(h), parent, (HMENU)(INT_PTR)id,
                             GetModuleHandleW(NULL), NULL);
    if (c) SendMessageW(c, WM_SETFONT, (WPARAM)g_font, TRUE);
    return c;
}

void lvCol(HWND lv, int i, const wchar_t *t, int w, int fmt)
{
    LVCOLUMNW c;
    memset(&c, 0, sizeof(c));
    c.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
    c.fmt = fmt;
    c.cx = S(w);
    c.pszText = (LPWSTR)t;
    SendMessageW(lv, LVM_INSERTCOLUMNW, i, (LPARAM)&c);
}

int lvAdd(HWND lv, int row, const wchar_t *t)
{
    LVITEMW it;
    memset(&it, 0, sizeof(it));
    it.mask = LVIF_TEXT;
    it.iItem = row;
    it.pszText = (LPWSTR)t;
    return (int)SendMessageW(lv, LVM_INSERTITEMW, 0, (LPARAM)&it);
}

void lvSet(HWND lv, int row, int col, const wchar_t *t)
{
    LVITEMW it;
    memset(&it, 0, sizeof(it));
    it.iSubItem = col;
    it.pszText = (LPWSTR)t;
    SendMessageW(lv, LVM_SETITEMTEXTW, row, (LPARAM)&it);
}

void lvSelect(HWND lv, int row)
{
    LVITEMW it;
    memset(&it, 0, sizeof(it));
    it.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
    it.state = LVIS_SELECTED | LVIS_FOCUSED;
    SendMessageW(lv, LVM_SETITEMSTATE, row, (LPARAM)&it);
    SendMessageW(lv, LVM_ENSUREVISIBLE, row, FALSE);
}

/* 이름 입력칸을 읽어 ANSI로 바꾼다. 공백이 있거나 비었거나 너무 길면 안내하고 0 */
int readName(HWND dlg, int id, const wchar_t *label, char *out)
{
    wchar_t w[96], m[128];

    GetDlgItemTextW(dlg, id, w, 96);
    trim(w);
    if (w[0] == L'\0') {
        swprintf(m, 128, L"%s을(를) 입력하세요.", label);
        bad(dlg, id, m);
        return 0;
    }
    if (hasSpace(w)) {
        swprintf(m, 128, L"%s에는 공백을 넣을 수 없습니다.", label);
        bad(dlg, id, m);
        return 0;
    }
    if (!wideToAnsi(w, out, NAME_LEN)) {
        swprintf(m, 128, L"%s이(가) 너무 깁니다. (한글 14자 이내)", label);
        bad(dlg, id, m);
        return 0;
    }
    return 1;
}

/* ---------------------------------------------------------------- 메인 창 목록 갱신 */

void setNote(const wchar_t *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    _vsnwprintf(g_note, 127, fmt, ap);
    va_end(ap);
    g_note[127] = L'\0';
}

/* 식재료를 임박순으로 정렬해 표를 다시 채우고 상태줄을 갱신한다. 행 번호 = 냉장고 배열 인덱스 */
void refreshMain(void)
{
    int i, expired, soon, sel;
    wchar_t w[96], st[256];

    sel = (int)SendMessageW(g_list, LVM_GETNEXTITEM, (WPARAM)-1, LVNI_SELECTED);
    sortByExpiry(&g_app->fridge);

    SendMessageW(g_list, WM_SETREDRAW, FALSE, 0);
    SendMessageW(g_list, LVM_DELETEALLITEMS, 0, 0);
    for (i = 0; i < g_app->fridge.count; i++) {
        const Food *x = &g_app->fridge.items[i];
        ansiToWide(x->name, w, 96);
        lvAdd(g_list, i, w);
        swprintf(w, 96, L"%d", x->quantity);
        lvSet(g_list, i, 1, w);
        lvSet(g_list, i, 2, x->storage == FREEZER ? L"냉동" : L"냉장");
        dateStr(x->expire, w, 96);
        lvSet(g_list, i, 3, w);
        ddayText(daysLeft(x->expire), w, 96);
        lvSet(g_list, i, 4, w);
    }
    if (sel >= 0 && g_app->fridge.count > 0) {
        if (sel >= g_app->fridge.count) sel = g_app->fridge.count - 1;
        lvSelect(g_list, sel);
    }
    SendMessageW(g_list, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(g_list, NULL, TRUE);

    countAlerts(&g_app->fridge, &expired, &soon);
    swprintf(st, 256, L"전체 %d개 · 기한 지남 %d개 · 임박 %d개", g_app->fridge.count, expired, soon);
    if (g_note[0]) catf(st, 256, L"     |     %s", g_note);
    SetWindowTextW(g_status, st);
}

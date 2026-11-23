/* dlg_recommend.c
 * 담당: 윤종하
 * 설명: 요리 추천 대화상자 (G4) + 조리법 자동 불러오기 (도전 목표)
 *       요리를 고르면 조리 순서가 없을 때 식품안전나라 API에서 자동으로 불러온다.
 *       인터넷 호출은 일꾼 스레드에서 하고, 결과는 WM_FETCHDONE 메시지로 창에 돌려준다.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "dlg_recommend.h"
#include "dlg_recipe.h"
#include "gui_dialog.h"
#include "expiry.h"
#include "food.h"
#include "recipe.h"
#include "guide.h"
#include "guide_api.h"
#include "apirec.h"

/* ---------------------------------------------------------------- G4: 요리 추천 창 */

#define MAX_REC      20
#define MAX_FAIL     64
#define WM_FETCHDONE (WM_APP + 1)      /* 일꾼 스레드가 "조리법 불러오기 끝"을 알리는 메시지 */
#define WM_APIRECDONE (WM_APP + 2)     /* 일꾼 스레드가 "API 추천 받기 끝"을 알리는 메시지 */
#define TIMER_FETCH  1                 /* 요리를 고른 뒤 잠깐 기다렸다가 불러오기 시작하는 타이머 */

typedef struct {
    DlgBase b;
    Recommendation rec[MAX_REC];
    int n;
    HWND hwnd, list, detail, btnCook, btnFetch, btnMode;
    char fetching[NAME_LEN];           /* 지금 인터넷에서 불러오는 중인 요리 이름 (없으면 "") */
    char noteName[NAME_LEN];           /* 방금 불러온 요리 이름과 */
    char noteMatched[NAME_LEN];        /* 인터넷에서 실제로 검색된 요리 이름 (다를 때 안내용) */
    int  apiMode;                      /* 1이면 목록이 "인터넷(API) 추천", 0이면 "저장된 레시피 추천" */
    int  apiLoading;                   /* 1이면 인터넷 추천을 받아 오는 중 */
    ApiRecommendation *arec;           /* 인터넷 추천 결과 (힙. 창을 닫을 때 해제) */
    int  an;                           /* arec의 개수 */
} RecDlg;

/* ---- API로 조리법 자동 불러오기 (README 도전 목표) ---- */

/* 일꾼 스레드에 넘기는 작업. 스레드가 채워서 창으로 돌려보낸다 */
typedef struct {
    char      name[NAME_LEN];
    char      steps[MAX_STEPS][STEP_LEN];
    char      matched[NAME_LEN];
    int       n;
    ApiResult r;
    HWND      dlg;
} FetchJob;

/* 이번 실행 중 불러오기에 실패한 요리. 같은 요리를 계속 다시 요청하지 않게 기억한다 */
static struct { char name[NAME_LEN]; ApiResult r; } g_fail[MAX_FAIL];
static int g_failCount;

static ApiResult failedResult(const char *name)
{
    int i;
    for (i = 0; i < g_failCount; i++)
        if (strcmp(g_fail[i].name, name) == 0) return g_fail[i].r;
    return API_OK;                                   /* 실패 기록 없음 */
}

static void rememberFail(const char *name, ApiResult r)
{
    int i;
    for (i = 0; i < g_failCount; i++)
        if (strcmp(g_fail[i].name, name) == 0) { g_fail[i].r = r; return; }
    if (g_failCount < MAX_FAIL) {
        strncpy(g_fail[g_failCount].name, name, NAME_LEN - 1);
        g_fail[g_failCount].name[NAME_LEN - 1] = '\0';
        g_fail[g_failCount].r = r;
        g_failCount++;
    }
}

static void forgetFail(const char *name)
{
    int i;
    for (i = 0; i < g_failCount; i++)
        if (strcmp(g_fail[i].name, name) == 0) { g_fail[i] = g_fail[--g_failCount]; return; }
}

static const wchar_t *apiErrorText(ApiResult r)
{
    switch (r) {
    case API_NO_KEY:     return L"인증키를 읽지 못했습니다. 영문·숫자만 한 줄로 적었는지 확인하세요.";
    case API_AUTH_ERROR: return L"인증키가 올바르지 않거나 이 서비스 이용 신청이 안 되어 있습니다.";
    case API_LIMIT:      return L"호출 제한에 걸렸습니다. 잠시 뒤 다시 시도하세요.";
    case API_NET_ERROR:  return L"인터넷에 연결하지 못했습니다.";
    case API_NOT_FOUND:  return L"인터넷에서 이 요리의 조리법을 찾지 못했습니다.";
    default:             return L"응답에서 조리 순서를 읽지 못했습니다.";
    }
}

/* 일꾼 스레드: 인터넷에서 조리 순서를 받아 창에 알린다 (화면은 건드리지 않는다) */
static DWORD WINAPI fetchThread(LPVOID p)
{
    FetchJob *j = (FetchJob *)p;
    j->r = fetchRecipeSteps(j->name, j->steps, MAX_STEPS, &j->n, j->matched, sizeof(j->matched));
    if (!PostMessageW(j->dlg, WM_FETCHDONE, 0, (LPARAM)j))
        free(j);                                     /* 그 사이 창이 닫혔으면 결과를 버린다 */
    return 0;
}

/* name 요리의 조리법을 인터넷에서 불러오기 시작한다 (한 번에 하나만). 시작했으면 1 */
static int recStartFetch(RecDlg *d, const char *name)
{
    FetchJob *j;
    HANDLE t;

    if (d->fetching[0] != '\0' || !hasApiKey()) return 0;
    j = (FetchJob *)calloc(1, sizeof(FetchJob));
    if (j == NULL) return 0;
    strncpy(j->name, name, NAME_LEN - 1);
    j->dlg = d->hwnd;
    t = CreateThread(NULL, 0, fetchThread, j, 0, NULL);
    if (t == NULL) { free(j); return 0; }
    CloseHandle(t);
    strncpy(d->fetching, name, NAME_LEN - 1);
    return 1;
}

/* 기한이 지나지 않은 같은 이름 재료의 수량 합계 (차감 전후 비교용) */
static int totalQuantity(const Fridge *f, const char *name)
{
    int i, sum = 0;
    for (i = 0; i < f->count; i++)
        if (strcmp(f->items[i].name, name) == 0 && daysLeft(f->items[i].expire) >= 0)
            sum += f->items[i].quantity;
    return sum;
}

/* ---- API 추천: 재료 "이름"만 대조해 인터넷 레시피를 추천 (양·개수는 보지 않는다) ---- */

static void recShowDetail(RecDlg *d);                /* 아래에서 정의 */
static void recRefresh(RecDlg *d);                   /* 아래에서 정의 */

/* 일꾼 스레드에 넘기는 작업. 냉장고는 창에서 미리 이름만 복사해 두므로 스레드는 냉장고를 읽지 않는다 */
typedef struct {
    IngredientRef      ings[MAX_ING_REF];
    int                n;
    ApiRecommendation *res;
    int                count;
    ApiResult          r;
    HWND               dlg;
} ApiJob;

static DWORD WINAPI apiRecThread(LPVOID p)
{
    ApiJob *j = (ApiJob *)p;

    j->res = (ApiRecommendation *)calloc(MAX_API_REC, sizeof(ApiRecommendation));
    if (j->res == NULL) j->r = API_NET_ERROR;
    else                j->r = apiRecommend(j->ings, j->n, j->res, MAX_API_REC, &j->count);
    if (!PostMessageW(j->dlg, WM_APIRECDONE, 0, (LPARAM)j)) {
        free(j->res);                                /* 그 사이 창이 닫혔으면 결과를 버린다 */
        free(j);
    }
    return 0;
}

static void recSetModeText(RecDlg *d)
{
    SetWindowTextW(d->btnMode, d->apiMode ? L"저장 레시피로 보기" : L"인터넷 추천 보기");
}

/* API 추천 결과를 표에 채운다 */
static void recApiPopulate(RecDlg *d)
{
    int i, j;
    wchar_t w[320], t[96];

    SendMessageW(d->list, LVM_DELETEALLITEMS, 0, 0);
    for (i = 0; i < d->an; i++) {
        const ApiRecommendation *r = &d->arec[i];

        ansiToWide(r->recipe.name, w, 320);
        lvAdd(d->list, i, w);
        swprintf(w, 320, L"%d개", r->matched);
        lvSet(d->list, i, 1, w);

        w[0] = L'\0';
        catf(w, 320, L"있는 재료:");
        for (j = 0; j < r->haveCount; j++) {
            ansiToWide(r->have[j], t, 96);
            catf(w, 320, L"%s%s%s", j ? L", " : L" ", t, r->haveUrgent[j] ? L"(임박)" : L"");
        }
        if (r->matched > r->haveCount)
            catf(w, 320, L" 외 %d개", r->matched - r->haveCount);
        lvSet(d->list, i, 2, w);
    }
    if (d->an > 0) lvSelect(d->list, 0);
}

/* 인터넷 추천을 받아 오기 시작한다 (재료 이름만 복사해 별도 스레드에서 호출) */
static void recApiStart(RecDlg *d)
{
    ApiJob *j;
    HANDLE t;

    free(d->arec);
    d->arec = NULL;
    d->an = 0;
    d->apiMode = 1;
    d->apiLoading = 0;
    SendMessageW(d->list, LVM_DELETEALLITEMS, 0, 0);

    j = (ApiJob *)calloc(1, sizeof(ApiJob));
    if (j != NULL) {
        j->n = collectIngredients(&g_app->fridge, j->ings, MAX_ING_REF);
        j->dlg = d->hwnd;
        if (j->n > 0 && (t = CreateThread(NULL, 0, apiRecThread, j, 0, NULL)) != NULL) {
            CloseHandle(t);
            d->apiLoading = 1;
            j = NULL;                                /* 스레드가 넘겨받았다 */
        }
    }
    free(j);                                         /* 시작하지 못했을 때만 남아 있다 */
    recShowDetail(d);
}

/* 선택한 인터넷 추천 요리의 재료 정보와 조리 순서를 아래 칸에 보여 준다 */
static void recShowApiDetail(RecDlg *d)
{
    static wchar_t text[8192];
    wchar_t w[2100], a[96];
    int sel, i;
    const ApiRecommendation *r;

    EnableWindow(d->btnCook, FALSE);                 /* 양을 비교하지 않으므로 재료 자동 차감은 하지 않는다 */
    EnableWindow(d->btnFetch, FALSE);
    if (d->apiLoading) {
        SetWindowTextW(d->detail,
            L"인터넷(식품안전나라)에서 내 재료로 만들 수 있는 요리를 찾는 중입니다...\r\n"
            L"재료 이름만 비교하고, 양은 보지 않습니다.");
        return;
    }
    sel = (int)SendMessageW(d->list, LVM_GETNEXTITEM, (WPARAM)-1, LVNI_SELECTED);
    if (sel < 0 || sel >= d->an) {
        SetWindowTextW(d->detail, d->an == 0
            ? L"추천할 요리가 없습니다.\r\n사용할 수 있는(기한이 지나지 않은) 재료를 등록해 보세요."
            : L"요리를 선택하면 재료 정보와 조리 순서가 여기에 표시됩니다.");
        return;
    }

    r = &d->arec[sel];
    text[0] = L'\0';
    ansiToWide(r->recipe.name, a, 96);
    catf(text, 8192, L"[ %s ]  (인터넷 레시피)\r\n\r\n[내 냉장고에 있는 재료]  ", a);
    for (i = 0; i < r->haveCount; i++) {
        ansiToWide(r->have[i], a, 96);
        catf(text, 8192, L"%s%s%s", i ? L", " : L"", a, r->haveUrgent[i] ? L"(임박)" : L"");
    }
    if (r->matched > r->haveCount)
        catf(text, 8192, L" 외 %d개", r->matched - r->haveCount);

    ansiToWide(r->recipe.parts, w, 2100);
    catf(text, 8192, L"\r\n\r\n[재료 정보]\r\n  %s\r\n\r\n[조리 순서]\r\n", w);
    if (r->recipe.stepCount == 0)
        catf(text, 8192, L"  제공된 조리 순서가 없습니다.\r\n");
    for (i = 0; i < r->recipe.stepCount; i++) {
        ansiToWide(r->recipe.steps[i], w, 320);
        catf(text, 8192, L"  %d. %s\r\n", i + 1, w);
    }
    catf(text, 8192, L"\r\n※ 재료의 이름만 비교했습니다. 양과 개수는 요리 전에 직접 확인하세요.");
    SetWindowTextW(d->detail, text);
}

/* 선택한 요리의 재료 현황과 조리 순서를 아래 칸에 보여 주고, 요리하기 버튼을 켜거나 끈다 */
static void recShowDetail(RecDlg *d)
{
    static wchar_t text[8192];
    char steps[MAX_STEPS][STEP_LEN];
    wchar_t a[96], w[320];
    int sel, i, j, n, needFetch = 0;
    ApiResult fr;
    const Recipe *rc;
    const Recommendation *r;

    text[0] = L'\0';
    KillTimer(d->hwnd, TIMER_FETCH);                 /* 선택이 바뀌면 이전 요리의 불러오기 예약을 취소 */
    if (d->apiMode) { recShowApiDetail(d); return; } /* 인터넷 추천 목록이면 그쪽 화면으로 */
    sel = (int)SendMessageW(d->list, LVM_GETNEXTITEM, (WPARAM)-1, LVNI_SELECTED);
    if (sel < 0 || sel >= d->n) {
        SetWindowTextW(d->detail, d->n == 0
            ? L"추천할 요리가 없습니다.\r\n재료를 더 등록하거나 레시피를 추가해 보세요."
            : L"요리를 선택하면 재료와 조리 순서가 여기에 표시됩니다.");
        EnableWindow(d->btnCook, FALSE);
        EnableWindow(d->btnFetch, FALSE);
        return;
    }

    r = &d->rec[sel];
    rc = &g_app->recipes.items[r->recipeIdx];
    ansiToWide(rc->name, a, 96);
    catf(text, 8192, L"[ %s ]\r\n\r\n[재료]\r\n", a);
    for (i = 0; i < rc->ingCount; i++) {
        int lacking = 0;
        for (j = 0; j < r->missingCount; j++)
            if (strcmp(r->missing[j], rc->ingredients[i]) == 0) lacking = 1;
        ansiToWide(rc->ingredients[i], a, 96);
        catf(text, 8192, L"  - %s %d개  %s\r\n", a, rc->amounts[i], lacking ? L"(부족)" : L"(보유)");
    }

    catf(text, 8192, L"\r\n[조리 순서]\r\n");
    n = getRecipeSteps(rc->name, steps, MAX_STEPS);
    if (n == 0) {
        if (!hasApiKey()) {
            catf(text, 8192, L"  아직 등록된 조리 방법이 없습니다.\r\n"
                             L"  (data/api_key.txt에 식품안전나라 인증키를 넣으면 인터넷에서 자동으로 불러옵니다.)\r\n");
        } else if (strcmp(d->fetching, rc->name) == 0) {
            catf(text, 8192, L"  인터넷에서 조리법을 불러오는 중입니다...\r\n");
        } else if ((fr = failedResult(rc->name)) != API_OK) {
            catf(text, 8192, L"  인터넷에서 조리법을 불러오지 못했습니다.\r\n  - %s\r\n"
                             L"  ([조리법 불러오기] 버튼으로 다시 시도할 수 있습니다.)\r\n", apiErrorText(fr));
        } else {
            catf(text, 8192, L"  잠시 뒤 인터넷에서 조리법을 자동으로 불러옵니다...\r\n");
            needFetch = 1;
        }
    }
    for (i = 0; i < n; i++) {
        ansiToWide(steps[i], w, 320);
        catf(text, 8192, L"  %d. %s\r\n", i + 1, w);
    }
    if (n > 0 && strcmp(d->noteName, rc->name) == 0 && strcmp(d->noteMatched, rc->name) != 0) {
        ansiToWide(d->noteMatched, w, 320);
        catf(text, 8192, L"\r\n(인터넷에서 불러왔습니다. 검색된 요리 이름: %s)\r\n", w);
    }
    if (!r->canCook)
        catf(text, 8192, L"\r\n재료가 부족해 지금은 만들 수 없습니다. 장을 본 뒤 위 순서대로 만들어 보세요.");

    SetWindowTextW(d->detail, text);
    EnableWindow(d->btnCook, r->canCook ? TRUE : FALSE);
    EnableWindow(d->btnFetch, (hasApiKey() && d->fetching[0] == '\0') ? TRUE : FALSE);
    if (needFetch)
        SetTimer(d->hwnd, TIMER_FETCH, 600, NULL);   /* 방향키로 훑어볼 때 매번 요청하지 않도록 잠깐 기다린다 */
}

/* 타이머: 선택이 600ms 동안 그대로이고 조리 순서가 없으면 인터넷에서 자동으로 불러온다 */
static void recTimer(HWND h, DlgBase *base, UINT_PTR id)
{
    RecDlg *d = (RecDlg *)base;
    char steps[MAX_STEPS][STEP_LEN];
    const Recipe *rc;
    int sel;

    (void)h;
    if (id != TIMER_FETCH || d->apiMode) return;
    KillTimer(d->hwnd, TIMER_FETCH);
    sel = (int)SendMessageW(d->list, LVM_GETNEXTITEM, (WPARAM)-1, LVNI_SELECTED);
    if (sel < 0 || sel >= d->n) return;
    rc = &g_app->recipes.items[d->rec[sel].recipeIdx];
    if (getRecipeSteps(rc->name, steps, MAX_STEPS) > 0) return;
    if (failedResult(rc->name) != API_OK) return;
    if (recStartFetch(d, rc->name))
        recShowDetail(d);                            /* "불러오는 중..." 표시 */
}

/* 일꾼 스레드가 끝났을 때: 받은 조리 순서를 저장하고 화면을 갱신한다 */
static LRESULT recUser(HWND h, DlgBase *base, UINT umsg, WPARAM w, LPARAM l)
{
    RecDlg *d = (RecDlg *)base;
    FetchJob *j;

    (void)w;
    if (umsg == WM_APIRECDONE) {                     /* 인터넷 추천을 받아 왔다 */
        ApiJob *aj = (ApiJob *)l;
        d->apiLoading = 0;
        if (aj->r == API_OK && aj->count > 0) {
            free(d->arec);
            d->arec = aj->res;                       /* 결과는 창이 넘겨받는다 */
            d->an = aj->count;
            aj->res = NULL;
            if (d->apiMode) {                        /* 사용자가 그사이 다른 화면으로 바꿨으면 저장만 해 둔다 */
                recApiPopulate(d);
                recShowDetail(d);
            }
        } else if (d->apiMode) {                     /* 실패: 저장된 레시피 추천으로 대신 보여 준다 */
            wchar_t m[400];
            swprintf(m, 400, L"인터넷 추천을 받지 못해 저장된 레시피로 추천합니다.\n\n사유: %s",
                     aj->r == API_NOT_FOUND ? L"내 재료로 만들 수 있는 요리를 인터넷에서 찾지 못했습니다."
                                            : apiErrorText(aj->r));
            d->apiMode = 0;
            recSetModeText(d);
            recRefresh(d);
            msg(h, m, MB_ICONINFORMATION);
        }
        free(aj->res);
        free(aj);
        return 0;
    }
    if (umsg != WM_FETCHDONE) return 0;
    j = (FetchJob *)l;
    d->fetching[0] = '\0';
    if (j->r == API_OK && j->n > 0 && replaceRecipeSteps(j->name, j->steps, j->n) == RC_OK) {
        forgetFail(j->name);
        strncpy(d->noteName, j->name, NAME_LEN - 1);
        strncpy(d->noteMatched, j->matched, NAME_LEN - 1);
        setNote(L"조리법을 인터넷에서 불러와 저장했습니다");
        refreshMain();
    } else {
        rememberFail(j->name, (j->r == API_OK) ? API_PARSE_ERROR : j->r);
    }
    free(j);
    recShowDetail(d);                                /* 현재 선택한 요리 기준으로 다시 그린다 (다음 요리 예약 포함) */
    return 0;
}

/* 추천 목록을 다시 계산해서 표를 채운다 */
static void recRefresh(RecDlg *d)
{
    int i, j;
    wchar_t w[320], t[96];

    d->n = recommend(&g_app->fridge, &g_app->recipes, d->rec, MAX_REC);
    SendMessageW(d->list, LVM_DELETEALLITEMS, 0, 0);
    for (i = 0; i < d->n; i++) {
        const Recommendation *r = &d->rec[i];
        const Recipe *rc = &g_app->recipes.items[r->recipeIdx];

        ansiToWide(rc->name, w, 320);
        lvAdd(d->list, i, w);
        swprintf(w, 320, L"%d/%d", r->matched, rc->ingCount);
        lvSet(d->list, i, 1, w);

        w[0] = L'\0';
        if (r->canCook) {
            catf(w, 320, L"바로 가능");
        } else {
            catf(w, 320, L"부족:");
            for (j = 0; j < r->missingCount; j++) {
                ansiToWide(r->missing[j], t, 96);
                catf(w, 320, L"%s%s", j ? L", " : L" ", t);
            }
        }
        if (r->urgentCount > 0) {
            catf(w, 320, L"  (임박 재료:");
            for (j = 0; j < r->urgentCount; j++) {
                ansiToWide(r->urgent[j], t, 96);
                catf(w, 320, L"%s%s", j ? L", " : L" ", t);
            }
            catf(w, 320, L")");
        }
        lvSet(d->list, i, 2, w);
    }
    if (d->n > 0) lvSelect(d->list, 0);
    recShowDetail(d);
}

static void recCreate(HWND h, DlgBase *base)
{
    RecDlg *d = (RecDlg *)base;

    d->hwnd = h;
    d->list = mk(h, WC_LISTVIEWW, L"", LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | WS_TABSTOP,
                 ID_RLIST, 16, 16, 628, 206);
    SendMessageW(d->list, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
    lvCol(d->list, 0, L"요리", 150, LVCFMT_LEFT);
    lvCol(d->list, 1, L"재료", 60, LVCFMT_CENTER);
    lvCol(d->list, 2, L"상태", 390, LVCFMT_LEFT);

    d->detail = mk(h, L"EDIT", L"", ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | WS_VSCROLL,
                   ID_DETAIL, 16, 234, 628, 190);
    d->btnCook = mk(h, L"BUTTON", L"요리하기", BS_DEFPUSHBUTTON | WS_TABSTOP, ID_COOK, 16, 438, 110, 32);
    mk(h, L"BUTTON", L"레시피 추가", BS_PUSHBUTTON | WS_TABSTOP, ID_ADDREC, 136, 438, 110, 32);
    d->btnFetch = mk(h, L"BUTTON", L"조리법 불러오기", BS_PUSHBUTTON | WS_TABSTOP, ID_FETCH, 256, 438, 140, 32);
    d->btnMode = mk(h, L"BUTTON", L"", BS_PUSHBUTTON | WS_TABSTOP, ID_MODE, 406, 438, 120, 32);
    mk(h, L"BUTTON", L"닫기", BS_PUSHBUTTON | WS_TABSTOP, IDCANCEL, 534, 438, 110, 32);
    if (hasApiKey()) {                               /* 인증키가 있으면 인터넷(API) 추천이 기본 */
        d->apiMode = 1;
        recSetModeText(d);
        recApiStart(d);
    } else {                                         /* 없으면 저장된 레시피로 추천 */
        d->apiMode = 0;
        recSetModeText(d);
        EnableWindow(d->btnMode, FALSE);
        recRefresh(d);
    }
    SetFocus(d->list);
}

/* 선택한 요리를 만들고 재료를 차감한다 (F5) */
static void recCook(HWND h, RecDlg *d)
{
    const Recipe *rc;
    int sel, j, before[MAX_ING];
    wchar_t m[800], a[96];

    if (d->apiMode) return;                          /* 인터넷 추천은 양을 비교하지 않아 자동 차감을 하지 않는다 */
    sel = (int)SendMessageW(d->list, LVM_GETNEXTITEM, (WPARAM)-1, LVNI_SELECTED);
    if (sel < 0 || sel >= d->n || !d->rec[sel].canCook) return;
    rc = &g_app->recipes.items[d->rec[sel].recipeIdx];

    m[0] = L'\0';
    for (j = 0; j < rc->ingCount; j++) {
        ansiToWide(rc->ingredients[j], a, 96);
        catf(m, 800, L"%s%s %d개", j ? L", " : L"", a, rc->amounts[j]);
        before[j] = totalQuantity(&g_app->fridge, rc->ingredients[j]);
    }
    catf(m, 800, L"를 사용합니다.\n만들까요?");
    if (!ask(h, m)) return;

    if (cookRecipe(&g_app->fridge, rc) == RC_OK) {
        m[0] = L'\0';
        catf(m, 800, L"재료가 차감되었습니다.\n");
        for (j = 0; j < rc->ingCount; j++) {
            int after = totalQuantity(&g_app->fridge, rc->ingredients[j]);
            ansiToWide(rc->ingredients[j], a, 96);
            catf(m, 800, L"\n%s  %d → %d%s", a, before[j], after, after == 0 ? L" (삭제)" : L"");
        }
        ansiToWide(rc->name, a, 96);
        setNote(L"'%s'을(를) 만들었습니다", a);
        msg(h, m, MB_ICONINFORMATION);
        refreshMain();
        recRefresh(d);
    } else {
        msg(h, L"재료가 부족해 차감하지 못했습니다.", MB_ICONERROR);
    }
}

static void recCommand(HWND h, DlgBase *base, int id, int code)
{
    RecDlg *d = (RecDlg *)base;
    (void)code;

    switch (id) {
    case IDCANCEL: d->b.done = 1; break;
    case ID_COOK:  recCook(h, d); break;
    case ID_FETCH: {                                  /* 직접 눌러서 (다시) 불러오기 */
        char steps[MAX_STEPS][STEP_LEN];
        const Recipe *rc;
        int sel = (int)SendMessageW(d->list, LVM_GETNEXTITEM, (WPARAM)-1, LVNI_SELECTED);

        if (d->apiMode || sel < 0 || sel >= d->n) break;
        rc = &g_app->recipes.items[d->rec[sel].recipeIdx];
        if (getRecipeSteps(rc->name, steps, MAX_STEPS) > 0 &&
            !ask(h, L"이미 저장된 조리 순서가 있습니다.\n인터넷의 조리법으로 바꿀까요?"))
            break;
        forgetFail(rc->name);
        if (recStartFetch(d, rc->name))
            recShowDetail(d);
        break;
    }
    case ID_MODE:                                     /* 인터넷 추천 <-> 저장된 레시피 추천 */
        if (d->apiMode) {
            d->apiMode = 0;
            recSetModeText(d);
            recRefresh(d);
        } else if (!hasApiKey()) {
            msg(h, L"인증키가 없어 인터넷 추천을 쓸 수 없습니다.\ndata/api_key.txt에 식품안전나라 인증키를 넣어 주세요.", MB_ICONINFORMATION);
        } else {
            d->apiMode = 1;
            recSetModeText(d);
            if (d->apiLoading) {                      /* 아직 받아 오는 중이면 기다린다 */
                SendMessageW(d->list, LVM_DELETEALLITEMS, 0, 0);
                recShowDetail(d);
            } else if (d->arec != NULL && d->an > 0) {
                recApiPopulate(d);
                recShowDetail(d);
            } else {
                recApiStart(d);
            }
        }
        break;
    case ID_ADDREC: {
        wchar_t nm[64];
        if (recipeDialog(h, nm, 64)) {
            setNote(L"레시피 '%s'을(를) 추가했습니다", nm);
            refreshMain();
            if (!d->apiMode) recRefresh(d);           /* 인터넷 추천 목록은 그대로 둔다 */
        }
        break;
    }
    }
}

static LRESULT recNotify(HWND h, DlgBase *base, NMHDR *nh)
{
    RecDlg *d = (RecDlg *)base;
    (void)h;

    if (nh->idFrom == ID_RLIST) {
        if (nh->code == LVN_ITEMCHANGED) {
            NMLISTVIEW *lv = (NMLISTVIEW *)nh;
            if ((lv->uChanged & LVIF_STATE) && ((lv->uNewState ^ lv->uOldState) & LVIS_SELECTED))
                recShowDetail(d);
        } else if (nh->code == NM_DBLCLK) {
            recCook(h, d);
        }
    }
    return 0;
}

void recommendDialog(HWND owner)
{
    static RecDlg d;                          /* Recommendation 20개가 커서 스택 대신 정적 영역을 쓴다 */
    memset(&d, 0, sizeof(d));
    d.b.onCreate = recCreate;
    d.b.onCommand = recCommand;
    d.b.onNotify = recNotify;
    d.b.onTimer = recTimer;
    d.b.onUser = recUser;
    runDialog(owner, L"오늘 뭐 먹지?", 660, 486, &d.b);
    free(d.arec);                             /* 인터넷 추천 결과 해제 */
    d.arec = NULL;
}

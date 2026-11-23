/* apirec.c
 * 담당: 윤종하
 * 설명: API 요리 추천. 재료 이름으로 요리를 찾고, 재료의 양은 보지 않고 이름만 대조해 점수를 매긴다. (화면 입출력 없음)
 *
 *   1. 내 재료 이름마다 API에 "이 재료가 들어간 요리"를 묻는다 (임박 재료 먼저, 최대 MAX_QUERY회)
 *   2. 받은 요리를 모아 같은 이름은 하나로 합친다
 *   3. 요리마다 "내 재료 이름이 재료 정보 글에 나오는가"를 센다 (양·개수는 보지 않음)
 *   4. 점수 = 나온 재료 수 × 10 + 그중 임박 재료 수 × 5, 점수 높은 순으로 돌려준다
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apirec.h"
#include "expiry.h"

#define MAX_CAND  100       /* 합치기 전에 모아 두는 요리 최대 개수 */

/* 같은 뜻으로 쓰는 재료 이름. 짝이 있으면 그 이름, 없으면 NULL */
static const char *synonymOf(const char *name)
{
    static const char *pairs[][2] = {
        { "계란", "달걀" },
        { "달걀", "계란" },
    };
    int i;
    for (i = 0; i < (int)(sizeof(pairs) / sizeof(pairs[0])); i++)
        if (strcmp(name, pairs[i][0]) == 0) return pairs[i][1];
    return NULL;
}

/* text 안에 word가 있는지. 한글(2바이트) 글자 경계에서만 비교해 글자 중간에서 우연히 맞는 일을 막는다 */
static int containsWord(const char *text, const char *word)
{
    const unsigned char *t = (const unsigned char *)text;
    size_t wl = strlen(word);

    if (wl == 0) return 0;
    while (*t) {
        if (strncmp((const char *)t, word, wl) == 0) return 1;
        t += (*t >= 0x80 && t[1] != '\0') ? 2 : 1;
    }
    return 0;
}

int ingredientInText(const char *text, const char *name)
{
    const char *syn;

    if (text == NULL || name == NULL) return 0;
    if (containsWord(text, name)) return 1;
    syn = synonymOf(name);
    return syn != NULL && containsWord(text, syn);
}

/* 냉장고에서 쓸 수 있는(기한이 지나지 않은) 재료 이름을 중복 없이 모은다. 임박 재료가 앞에 온다 */
int collectIngredients(const Fridge *f, IngredientRef *out, int max)
{
    int i, j, n = 0, pass;

    if (f == NULL || out == NULL || max <= 0) return 0;
    for (i = 0; i < f->count && n < max; i++) {
        int d = daysLeft(f->items[i].expire);
        int urgent = (d <= ALERT_DAYS) ? 1 : 0;
        if (d < 0) continue;                                   /* 기한 지난 재료는 쓰지 않는다 */
        for (j = 0; j < n; j++)
            if (strcmp(out[j].name, f->items[i].name) == 0) break;
        if (j < n) {
            if (urgent) out[j].urgent = 1;                     /* 같은 이름 중 하나라도 임박이면 임박 */
        } else {
            strncpy(out[n].name, f->items[i].name, NAME_LEN - 1);
            out[n].name[NAME_LEN - 1] = '\0';
            out[n].urgent = urgent;
            n++;
        }
    }
    /* 임박 재료를 앞으로 (순서는 유지) */
    {
        IngredientRef tmp[MAX_ING_REF];
        int k = 0;
        if (n > MAX_ING_REF) n = MAX_ING_REF;
        for (pass = 1; pass >= 0; pass--)
            for (i = 0; i < n; i++)
                if (out[i].urgent == pass) tmp[k++] = out[i];
        for (i = 0; i < n; i++) out[i] = tmp[i];
    }
    return n;
}

/* 점수 계산: 내 재료 중 이름이 재료 정보에 나오는 것을 센다 */
static void scoreRecipe(ApiRecommendation *r, const IngredientRef *ings, int n)
{
    int i;
    r->matched = r->urgentMatched = r->haveCount = 0;
    for (i = 0; i < n; i++) {
        if (!ingredientInText(r->recipe.parts, ings[i].name)) continue;
        r->matched++;
        if (ings[i].urgent) r->urgentMatched++;
        if (r->haveCount < MAX_HAVE) {
            strncpy(r->have[r->haveCount], ings[i].name, NAME_LEN - 1);
            r->have[r->haveCount][NAME_LEN - 1] = '\0';
            r->haveUrgent[r->haveCount] = ings[i].urgent;
            r->haveCount++;
        }
    }
    r->score = r->matched * 10 + r->urgentMatched * 5;
}

ApiResult apiRecommend(const IngredientRef *ings, int n, ApiRecommendation *out, int maxOut, int *outCount)
{
    char queries[MAX_QUERY][NAME_LEN];
    int nq = 0, i, k, count = 0;
    ApiRecipe *page = NULL, *cand = NULL;
    int *taken = NULL;
    int nCand = 0;
    ApiResult err = API_OK;

    if (outCount) *outCount = 0;
    if (ings == NULL || n <= 0 || out == NULL || maxOut <= 0) return API_NOT_FOUND;

    /* 1. 물어볼 이름 정하기: 내 재료 이름(임박 먼저)과 같은 뜻의 이름 */
    for (i = 0; i < n && nq < MAX_QUERY; i++) {
        const char *syn = synonymOf(ings[i].name);
        strncpy(queries[nq], ings[i].name, NAME_LEN - 1);
        queries[nq][NAME_LEN - 1] = '\0';
        nq++;
        if (syn != NULL && nq < MAX_QUERY) {
            strncpy(queries[nq], syn, NAME_LEN - 1);
            queries[nq][NAME_LEN - 1] = '\0';
            nq++;
        }
    }

    page = (ApiRecipe *)malloc(sizeof(ApiRecipe) * API_PAGE_SIZE);
    cand = (ApiRecipe *)malloc(sizeof(ApiRecipe) * MAX_CAND);
    taken = (int *)calloc(MAX_CAND, sizeof(int));
    if (page == NULL || cand == NULL || taken == NULL) {
        free(page); free(cand); free(taken);
        return API_NET_ERROR;
    }

    /* 2. API 호출과 합치기 */
    for (i = 0; i < nq; i++) {
        int got = 0;
        ApiResult r = fetchRecipesByIngredient(queries[i], page, API_PAGE_SIZE, &got);
        if (r != API_OK) {
            if (r != API_NOT_FOUND) { err = r; break; }       /* 키 오류·호출 제한·인터넷 끊김이면 더 부르지 않는다 */
            continue;
        }
        for (k = 0; k < got && nCand < MAX_CAND; k++) {
            int j;
            for (j = 0; j < nCand; j++)
                if (strcmp(cand[j].name, page[k].name) == 0) break;
            if (j == nCand)
                cand[nCand++] = page[k];                       /* 같은 이름이 없을 때만 추가 */
        }
    }
    free(page);

    if (nCand == 0) {
        free(cand); free(taken);
        return (err != API_OK) ? err : API_NOT_FOUND;
    }

    /* 3·4. 이름 대조와 점수, 점수 높은 순으로 maxOut개 뽑기 */
    {
        ApiRecommendation *tmp = (ApiRecommendation *)malloc(sizeof(ApiRecommendation));
        int *score = (int *)malloc(sizeof(int) * nCand);
        int *match = (int *)malloc(sizeof(int) * nCand);
        if (tmp == NULL || score == NULL || match == NULL) {
            free(tmp); free(score); free(match); free(cand); free(taken);
            return API_NET_ERROR;
        }
        for (i = 0; i < nCand; i++) {
            tmp->recipe = cand[i];
            scoreRecipe(tmp, ings, n);
            score[i] = tmp->score;
            match[i] = tmp->matched;
        }
        while (count < maxOut) {
            int best = -1;
            for (i = 0; i < nCand; i++) {
                if (taken[i] || match[i] < 1) continue;        /* 내 재료가 하나도 안 나오는 요리는 제외 */
                if (best < 0 || score[i] > score[best] ||
                    (score[i] == score[best] && strcmp(cand[i].name, cand[best].name) < 0))
                    best = i;
            }
            if (best < 0) break;
            taken[best] = 1;
            out[count].recipe = cand[best];
            scoreRecipe(&out[count], ings, n);
            count++;
        }
        free(tmp); free(score); free(match);
    }
    free(cand);
    free(taken);

    if (outCount) *outCount = count;
    if (count > 0) return API_OK;
    return (err != API_OK) ? err : API_NOT_FOUND;
}

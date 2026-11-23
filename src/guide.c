/* guide.c
 * 담당: 윤종하
 * 설명: 요리별 조리 순서 조회와 저장 (로직 모듈: 화면 입출력 없음)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "guide.h"

typedef struct {
    char name[NAME_LEN];
    char steps[MAX_STEPS][STEP_LEN];
    int  count;
} Guide;

static Guide *g_guides = NULL;     /* 불러온 조리 순서 목록 (동적 배열) */
static int    g_count = 0;
static int    g_capacity = 0;
static int    g_loaded = 0;        /* 파일을 이미 읽었으면 1 */

/* 목록 끝에 하나 추가한다. 가득 차면 2배로 늘린다 */
static ResultCode pushGuide(const Guide *g)
{
    if (g_count == g_capacity) {
        int newCap = (g_capacity == 0) ? 8 : g_capacity * 2;
        Guide *tmp = (Guide *)realloc(g_guides, sizeof(Guide) * newCap);
        if (tmp == NULL)
            return RC_NO_MEMORY;
        g_guides = tmp;
        g_capacity = newCap;
    }
    g_guides[g_count++] = *g;
    return RC_OK;
}

/* 이름으로 찾는다. 없으면 -1 */
static int findGuide(const char *name)
{
    int i;
    for (i = 0; i < g_count; i++)
        if (strcmp(g_guides[i].name, name) == 0)
            return i;
    return -1;
}

/* steps.txt를 한 번만 읽는다. 파일이 없으면 빈 목록으로 둔다 */
static void loadGuides(void)
{
    FILE *fp;
    static char line[MAX_STEPS * (STEP_LEN + 1) + NAME_LEN + 16];   /* 한 줄이 가장 길 때도 담긴다 */

    if (g_loaded) return;
    g_loaded = 1;

    fp = fopen(GUIDE_FILE, "r");
    if (fp == NULL) return;

    while (fgets(line, sizeof(line), fp)) {
        Guide g;
        char *tok;
        size_t n = strlen(line);

        while (n > 0 && (line[n - 1] == '\n' || line[n - 1] == '\r'))
            line[--n] = '\0';
        if (n == 0) continue;

        memset(&g, 0, sizeof(g));
        tok = strtok(line, "|");
        if (tok == NULL) continue;
        strncpy(g.name, tok, NAME_LEN - 1);

        while ((tok = strtok(NULL, "|")) != NULL && g.count < MAX_STEPS) {
            strncpy(g.steps[g.count], tok, STEP_LEN - 1);
            g.count++;
        }
        if (g.count > 0 && findGuide(g.name) < 0)
            pushGuide(&g);
    }
    fclose(fp);
}

/* 요리 이름으로 조리 순서를 찾아 steps에 복사한다. 단계 수를 반환, 없으면 0 */
int getRecipeSteps(const char *recipeName, char steps[][STEP_LEN], int max)
{
    int idx, i, n;

    if (recipeName == NULL || steps == NULL || max <= 0) return 0;
    loadGuides();
    idx = findGuide(recipeName);
    if (idx < 0) return 0;

    n = g_guides[idx].count < max ? g_guides[idx].count : max;
    for (i = 0; i < n; i++)
        strcpy(steps[i], g_guides[idx].steps[i]);
    return n;
}

/* 조리 순서를 메모리와 파일 끝에 추가한다 */
ResultCode addRecipeSteps(const char *recipeName, char steps[][STEP_LEN], int count)
{
    Guide g;
    FILE *fp;
    int i;
    char *p;

    if (recipeName == NULL || steps == NULL || recipeName[0] == '\0') return RC_INVALID;
    if (count < 1 || count > MAX_STEPS) return RC_INVALID;
    loadGuides();
    if (findGuide(recipeName) >= 0) return RC_INVALID;

    memset(&g, 0, sizeof(g));
    strncpy(g.name, recipeName, NAME_LEN - 1);
    for (i = 0; i < count; i++) {
        strncpy(g.steps[i], steps[i], STEP_LEN - 1);
        for (p = g.steps[i]; *p; p++)          /* 구분자와 줄바꿈은 공백으로 바꾼다 */
            if (*p == '|' || *p == '\n' || *p == '\r')
                *p = ' ';
    }
    g.count = count;

    fp = fopen(GUIDE_FILE, "a");
    if (fp == NULL) return RC_FILE_ERROR;
    fprintf(fp, "%s", g.name);
    for (i = 0; i < count; i++)
        fprintf(fp, "|%s", g.steps[i]);
    fprintf(fp, "\n");
    fclose(fp);

    return pushGuide(&g);
}

/* 메모리에 있는 전체 목록을 파일에 다시 쓴다. 임시 파일에 먼저 쓰고 성공하면 교체해서 원본이 깨지지 않게 한다 */
static ResultCode rewriteGuideFile(void)
{
    FILE *fp;
    int i, j;
    const char *tmpPath = "data/steps.tmp";

    fp = fopen(tmpPath, "w");
    if (fp == NULL) return RC_FILE_ERROR;
    for (i = 0; i < g_count; i++) {
        fprintf(fp, "%s", g_guides[i].name);
        for (j = 0; j < g_guides[i].count; j++)
            fprintf(fp, "|%s", g_guides[i].steps[j]);
        fprintf(fp, "\n");
    }
    if (fclose(fp) != 0) { remove(tmpPath); return RC_FILE_ERROR; }

    remove(GUIDE_FILE);
    if (rename(tmpPath, GUIDE_FILE) != 0) return RC_FILE_ERROR;
    return RC_OK;
}

/* 조리 순서를 바꾼다. 같은 이름이 없으면 새로 추가한다 */
ResultCode replaceRecipeSteps(const char *recipeName, char steps[][STEP_LEN], int count)
{
    int idx, i;
    char *p;

    if (recipeName == NULL || steps == NULL || recipeName[0] == '\0') return RC_INVALID;
    if (count < 1 || count > MAX_STEPS) return RC_INVALID;
    loadGuides();

    idx = findGuide(recipeName);
    if (idx < 0)
        return addRecipeSteps(recipeName, steps, count);

    memset(&g_guides[idx].steps, 0, sizeof(g_guides[idx].steps));
    for (i = 0; i < count; i++) {
        strncpy(g_guides[idx].steps[i], steps[i], STEP_LEN - 1);
        for (p = g_guides[idx].steps[i]; *p; p++)
            if (*p == '|' || *p == '\n' || *p == '\r')
                *p = ' ';
    }
    g_guides[idx].count = count;
    return rewriteGuideFile();
}
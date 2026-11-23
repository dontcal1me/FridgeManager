/* fileio.c
 * 담당: 윤종하
 * 설명: 식재료·레시피·폐기 기록 파일의 저장과 불러오기 (형식은 README 12장)
 *       파일이 없으면 RC_OK, 형식이 잘못된 줄은 건너뛴다.
 */
#include <stdio.h>
#include <string.h>
#include "fileio.h"
#include "food.h"
#include "recipe.h"
#include "stats.h"

#define LINE_LEN 1024

/* fridge.txt 불러오기: 이름 유통기한 수량 보관 */
ResultCode loadFridge(const char *path, Fridge *f)
{
    FILE *fp;
    char line[LINE_LEN];

    if (path == NULL || f == NULL) return RC_INVALID;
    fp = fopen(path, "r");
    if (fp == NULL) return RC_OK;                  /* 처음 실행: 파일 없음 */

    while (fgets(line, sizeof(line), fp)) {
        Food item;
        int s;
        if (sscanf(line, "%29s %d-%d-%d %d %d",
                   item.name, &item.expire.year, &item.expire.month, &item.expire.day,
                   &item.quantity, &s) != 6)
            continue;
        item.storage = (Storage)s;
        if (addFood(f, &item) == RC_NO_MEMORY) {
            fclose(fp);
            return RC_NO_MEMORY;
        }
    }
    fclose(fp);
    return RC_OK;
}

/* fridge.txt 저장 */
ResultCode saveFridge(const char *path, const Fridge *f)
{
    FILE *fp;
    int i;

    if (path == NULL || f == NULL) return RC_INVALID;
    fp = fopen(path, "w");
    if (fp == NULL) return RC_FILE_ERROR;
    for (i = 0; i < f->count; i++) {
        const Food *x = &f->items[i];
        fprintf(fp, "%s %04d-%02d-%02d %d %d\n", x->name,
                x->expire.year, x->expire.month, x->expire.day, x->quantity, (int)x->storage);
    }
    fclose(fp);
    return RC_OK;
}

/* recipes.txt 불러오기: 요리이름 사용자추가여부 재료개수 (재료 수량)... */
ResultCode loadRecipes(const char *path, RecipeBook *rb)
{
    FILE *fp;
    char line[LINE_LEN];

    if (path == NULL || rb == NULL) return RC_INVALID;
    fp = fopen(path, "r");
    if (fp == NULL) return RC_OK;

    while (fgets(line, sizeof(line), fp)) {
        Recipe rc;
        const char *p = line;
        int pos = 0, i, ok = 1;

        memset(&rc, 0, sizeof(rc));
        if (sscanf(p, "%29s %d %d%n", rc.name, &rc.isCustom, &rc.ingCount, &pos) != 3)
            continue;
        if (rc.ingCount < 1 || rc.ingCount > MAX_ING)
            continue;
        p += pos;
        for (i = 0; i < rc.ingCount; i++) {
            if (sscanf(p, "%29s %d%n", rc.ingredients[i], &rc.amounts[i], &pos) != 2) {
                ok = 0;
                break;
            }
            p += pos;
        }
        if (!ok) continue;
        if (addRecipe(rb, &rc) == RC_NO_MEMORY) {
            fclose(fp);
            return RC_NO_MEMORY;
        }
    }
    fclose(fp);
    return RC_OK;
}

/* recipes.txt 저장 */
ResultCode saveRecipes(const char *path, const RecipeBook *rb)
{
    FILE *fp;
    int i, j;

    if (path == NULL || rb == NULL) return RC_INVALID;
    fp = fopen(path, "w");
    if (fp == NULL) return RC_FILE_ERROR;
    for (i = 0; i < rb->count; i++) {
        const Recipe *rc = &rb->items[i];
        fprintf(fp, "%s %d %d", rc->name, rc->isCustom, rc->ingCount);
        for (j = 0; j < rc->ingCount; j++)
            fprintf(fp, " %s %d", rc->ingredients[j], rc->amounts[j]);
        fprintf(fp, "\n");
    }
    fclose(fp);
    return RC_OK;
}

/* waste.txt 불러오기: 이름 수량 보관 유통기한 폐기일 */
ResultCode loadWaste(const char *path, WasteList *w)
{
    FILE *fp;
    char line[LINE_LEN];

    if (path == NULL || w == NULL) return RC_INVALID;
    fp = fopen(path, "r");
    if (fp == NULL) return RC_OK;

    while (fgets(line, sizeof(line), fp)) {
        WasteRecord r;
        int s;
        memset(&r, 0, sizeof(r));
        if (sscanf(line, "%29s %d %d %d-%d-%d %d-%d-%d",
                   r.name, &r.quantity, &s,
                   &r.expire.year, &r.expire.month, &r.expire.day,
                   &r.discarded.year, &r.discarded.month, &r.discarded.day) != 9)
            continue;
        r.storage = (Storage)s;
        if (appendWaste(w, &r) == RC_NO_MEMORY) {
            fclose(fp);
            return RC_NO_MEMORY;
        }
    }
    fclose(fp);
    return RC_OK;
}

/* waste.txt 저장 */
ResultCode saveWaste(const char *path, const WasteList *w)
{
    FILE *fp;
    int i;

    if (path == NULL || w == NULL) return RC_INVALID;
    fp = fopen(path, "w");
    if (fp == NULL) return RC_FILE_ERROR;
    for (i = 0; i < w->count; i++) {
        const WasteRecord *r = &w->items[i];
        fprintf(fp, "%s %d %d %04d-%02d-%02d %04d-%02d-%02d\n", r->name, r->quantity, (int)r->storage,
                r->expire.year, r->expire.month, r->expire.day,
                r->discarded.year, r->discarded.month, r->discarded.day);
    }
    fclose(fp);
    return RC_OK;
}

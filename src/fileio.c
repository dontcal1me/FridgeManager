/* fileio.c
 * 담당: 윤종하
 * 설명: 식재료·레시피·폐기 기록 파일 저장·불러오기
 *
 * [뼈대] 컴파일만 되는 빈 껍데기입니다. feature/fileio 브랜치에서 채웁니다.
 */
#include "fileio.h"

ResultCode loadFridge(const char *path, Fridge *f)
{
    (void)path; (void)f;
    return RC_OK;
}

ResultCode saveFridge(const char *path, const Fridge *f)
{
    (void)path; (void)f;
    return RC_OK;
}

ResultCode loadRecipes(const char *path, RecipeBook *rb)
{
    (void)path; (void)rb;
    return RC_OK;
}

ResultCode saveRecipes(const char *path, const RecipeBook *rb)
{
    (void)path; (void)rb;
    return RC_OK;
}

ResultCode loadWaste(const char *path, WasteList *w)
{
    (void)path; (void)w;
    return RC_OK;
}

ResultCode saveWaste(const char *path, const WasteList *w)
{
    (void)path; (void)w;
    return RC_OK;
}

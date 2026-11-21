/* recipe.c
 * 담당: 최민솔
 * 설명: 요리 추천, 재료 자동 차감, 레시피 추가 (F3, F5)
 *
 * [뼈대] 컴파일만 되는 빈 껍데기입니다. 담당자가 README 6.4장을 보고 내용을 채웁니다.
 *        함수 이름과 매개변수, 반환값은 recipe.h 그대로 두세요.
 */
#include "recipe.h"

void initRecipeBook(RecipeBook *rb)
{
    rb->items = 0;
    rb->count = 0;
    rb->capacity = 0;
}

void freeRecipeBook(RecipeBook *rb)
{
    (void)rb;
}

ResultCode addRecipe(RecipeBook *rb, const Recipe *rc)
{
    (void)rb; (void)rc;
    return RC_OK;
}

int recommend(const Fridge *f, const RecipeBook *rb, Recommendation *out, int max)
{
    (void)f; (void)rb; (void)out; (void)max;
    return 0;
}

ResultCode cookRecipe(Fridge *f, const Recipe *rc)
{
    (void)f; (void)rc;
    return RC_OK;
}

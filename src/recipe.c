/* recipe.c
 * 담당: 최민솔
 * 설명: 요리 추천·재료 자동 차감·레시피 추가 (F3, F5)
 */
#include <stdlib.h>
#include <string.h>

#include "recipe.h"

/* TODO(최민솔): 구현 전 뼈대. 컴파일만 되는 빈 껍데기입니다. */

void initRecipeBook(RecipeBook *rb) { (void)rb; }
void freeRecipeBook(RecipeBook *rb) { (void)rb; }
ResultCode addRecipe(RecipeBook *rb, const Recipe *rc) { (void)rb; (void)rc; return RC_OK; }
int recommend(const Fridge *f, const RecipeBook *rb, Recommendation *out, int max) { (void)f; (void)rb; (void)out; (void)max; return 0; }
ResultCode cookRecipe(Fridge *f, const Recipe *rc) { (void)f; (void)rc; return RC_OK; }
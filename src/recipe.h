/* recipe.h
 * 담당: 최민솔
 * 설명: 요리 추천, 재료 자동 차감, 레시피 추가 (F3, F5)
 *       함수 선언은 README 6.4장과 같다. 윤종하가 확정했으므로 바꾸지 않는다.
 */
#ifndef RECIPE_H
#define RECIPE_H

#include "common.h"

void       initRecipeBook(RecipeBook *rb);                /* capacity 8로 동적 배열 생성 */
void       freeRecipeBook(RecipeBook *rb);
ResultCode addRecipe(RecipeBook *rb, const Recipe *rc);   /* F3: 레시피 추가 */
int        recommend(const Fridge *f, const RecipeBook *rb,
                     Recommendation *out, int max);       /* F3: 점수순 추천. 채운 개수 반환 */
ResultCode cookRecipe(Fridge *f, const Recipe *rc);       /* F5: 재료 자동 차감 */

#endif

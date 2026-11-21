/* food.h
 * 담당: 박지연
 * 설명: 식재료 등록·수정·삭제·수량 변경·검색 (F1, F5)
 *       함수 선언은 README 6.5장과 같다. 윤종하가 확정했으므로 바꾸지 않는다.
 */
#ifndef FOOD_H
#define FOOD_H

#include "common.h"

void       initFridge(Fridge *f);                                 /* capacity 4로 동적 배열 생성 */
void       freeFridge(Fridge *f);                                 /* 메모리 해제, items = NULL, count = 0 */
ResultCode addFood(Fridge *f, const Food *item);                  /* F1 */
ResultCode updateFood(Fridge *f, int idx, const Food *item);      /* F5 */
ResultCode removeFood(Fridge *f, int idx);                        /* F5 */
ResultCode useFood(Fridge *f, int idx, int amount);               /* F5: 수량 차감 */
int        findFood(const Fridge *f, const char *name);           /* 이름으로 검색, 없으면 -1 */

#endif

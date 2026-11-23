/* dlg_food.h
 * 담당: 김기범
 * 설명: 식재료 등록·수정 대화상자 (G3)
 */
#ifndef DLG_FOOD_H
#define DLG_FOOD_H

#include "gui_common.h"

/* 입력을 받아 food에 채운다. isEdit=1이면 food의 현재 값을 채워서 보여 준다.
 * 등록/수정을 눌렀으면 1, 취소면 0 */
int foodDialog(HWND owner, Food *food, int isEdit);

#endif

/* dlg_recipe.h
 * 담당: 최민솔
 * 설명: 레시피 추가 대화상자 (G4). 재료 최대 10개와 조리 순서를 입력받아 저장한다.
 */
#ifndef DLG_RECIPE_H
#define DLG_RECIPE_H

#include "gui_common.h"

/* 레시피를 추가했으면 1. addedName에 추가한 요리 이름이 들어간다 (필요 없으면 NULL) */
int recipeDialog(HWND owner, wchar_t *addedName, int nameLen);

#endif

/* ui_console.h
 * 담당: 윤종하
 * 설명: 콘솔 화면 (메뉴, 목록, 입력 화면). 화면 함수는 로직 모듈의 함수만 호출한다.
 */
#ifndef UI_CONSOLE_H
#define UI_CONSOLE_H

#include "common.h"

void showStartupAlert(const Fridge *f);     /* F4: 시작 알림 박스. 마지막에 waitForKey() */
void printMainMenu(void);                   /* 메인 메뉴 출력 */
void runMainMenu(AppData *app);             /* 0을 고를 때까지 메뉴 반복 */

void printFoodTable(const Fridge *f);       /* 식재료 표 출력. 번호는 1부터 (idx + 1) */
void screenAddFood(AppData *app);           /* 메뉴 1: 식재료 등록 */
void screenFoodList(AppData *app);          /* 메뉴 2: 임박순 목록 */
void screenEditFood(AppData *app);          /* 메뉴 3: 수정 / 삭제 / 수량 변경 */
void screenRecommend(AppData *app);         /* 메뉴 4: 요리 추천과 요리하기 */
void screenAddRecipe(AppData *app);         /* 메뉴 5: 레시피 추가 */
void screenDiscardExpired(AppData *app);    /* 메뉴 6: 기한 지난 재료 일괄 폐기 */
void screenWasteStats(const AppData *app);  /* 메뉴 7: 폐기 기록 통계 */

#endif

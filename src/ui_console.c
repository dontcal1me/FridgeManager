/* ui_console.c
 * 담당: 윤종하
 * 설명: 콘솔 화면 (메뉴, 목록, 입력 화면)
 *
 * [뼈대] 메인 메뉴는 동작하고, 나머지 화면은 "준비 중" 안내만 출력합니다.
 *        feature/main 브랜치에서 화면을 하나씩 채웁니다.
 */
#include <stdio.h>
#include "ui_console.h"
#include "util.h"

void showStartupAlert(const Fridge *f)
{
    (void)f;
}

void printMainMenu(void)
{
    printf("\n============ 냉장고 관리 ============\n");
    printf(" 1. 식재료 등록\n");
    printf(" 2. 식재료 목록 (임박순)\n");
    printf(" 3. 식재료 수정 / 삭제 / 수량 변경\n");
    printf(" 4. 오늘 뭐 먹지? (요리 추천)\n");
    printf(" 5. 레시피 추가\n");
    printf(" 6. 기한 지난 재료 일괄 폐기\n");
    printf(" 7. 폐기 기록 통계\n");
    printf(" 0. 저장 후 종료\n");
    printf("=====================================\n");
}

void runMainMenu(AppData *app)
{
    int choice;
    do {
        printMainMenu();
        choice = inputInt("선택 > ", 0, 7);
        switch (choice) {
        case 1: screenAddFood(app);         break;
        case 2: screenFoodList(app);        break;
        case 3: screenEditFood(app);        break;
        case 4: screenRecommend(app);       break;
        case 5: screenAddRecipe(app);       break;
        case 6: screenDiscardExpired(app);  break;
        case 7: screenWasteStats(app);      break;
        default: break;
        }
    } while (choice != 0);
}

void printFoodTable(const Fridge *f)
{
    (void)f;
}

void screenAddFood(AppData *app)
{
    (void)app;
    printf("[준비 중] 식재료 등록\n");
}

void screenFoodList(AppData *app)
{
    (void)app;
    printf("[준비 중] 식재료 목록\n");
}

void screenEditFood(AppData *app)
{
    (void)app;
    printf("[준비 중] 식재료 수정/삭제\n");
}

void screenRecommend(AppData *app)
{
    (void)app;
    printf("[준비 중] 요리 추천\n");
}

void screenAddRecipe(AppData *app)
{
    (void)app;
    printf("[준비 중] 레시피 추가\n");
}

void screenDiscardExpired(AppData *app)
{
    (void)app;
    printf("[준비 중] 일괄 폐기\n");
}

void screenWasteStats(const AppData *app)
{
    (void)app;
    printf("[준비 중] 폐기 통계\n");
}

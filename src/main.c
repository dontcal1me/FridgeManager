/* main.c
 * 담당: 윤종하
 * 설명: 콘솔 진입점. 데이터 불러오기 → 시작 알림 → 메인 메뉴 → 저장 → 메모리 해제
 */
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "food.h"
#include "recipe.h"
#include "stats.h"
#include "fileio.h"
#include "ui_console.h"
#include "test.h"

/* --test 뒤에 온 이름에 맞는 테스트 함수를 호출한다. 성공 0, 모르는 이름 1 */
int runTest(const char *name)
{
    if (strcmp(name, "util") == 0)        testUtil();
    else if (strcmp(name, "expiry") == 0) testExpiry();
    else if (strcmp(name, "recipe") == 0) testRecipe();
    else if (strcmp(name, "food") == 0)   testFood();
    else if (strcmp(name, "stats") == 0)  testStats();
    else if (strcmp(name, "fileio") == 0) testFileio();
    else {
        printf("사용법: --test [util|expiry|recipe|food|stats|fileio]\n");
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    AppData app;

    if (argc == 3 && strcmp(argv[1], "--test") == 0)
        return runTest(argv[2]);                 /* 모듈 단독 테스트 */

    initFridge(&app.fridge);
    initRecipeBook(&app.recipes);
    initWasteList(&app.waste);

    if (loadFridge(FRIDGE_FILE, &app.fridge) != RC_OK)
        printf("[경고] %s를 읽지 못했습니다.\n", FRIDGE_FILE);
    if (loadRecipes(RECIPE_FILE, &app.recipes) != RC_OK)
        printf("[경고] %s를 읽지 못했습니다.\n", RECIPE_FILE);
    if (loadWaste(WASTE_FILE, &app.waste) != RC_OK)
        printf("[경고] %s를 읽지 못했습니다.\n", WASTE_FILE);

    showStartupAlert(&app.fridge);               /* F4 */
    runMainMenu(&app);                           /* 0을 고를 때까지 반복 */

    if (saveFridge(FRIDGE_FILE, &app.fridge) != RC_OK)
        printf("[오류] %s 저장에 실패했습니다. data 폴더가 있는지 확인하세요.\n", FRIDGE_FILE);
    if (saveRecipes(RECIPE_FILE, &app.recipes) != RC_OK)
        printf("[오류] %s 저장에 실패했습니다.\n", RECIPE_FILE);
    if (saveWaste(WASTE_FILE, &app.waste) != RC_OK)
        printf("[오류] %s 저장에 실패했습니다.\n", WASTE_FILE);

    freeFridge(&app.fridge);
    freeRecipeBook(&app.recipes);
    freeWasteList(&app.waste);
    return 0;
}

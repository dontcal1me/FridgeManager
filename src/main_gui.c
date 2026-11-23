/* main_gui.c
 * 담당: 윤종하
 * 설명: GUI 진입점. 데이터 불러오기 → 시작 알림 → 메인 창 → 저장 → 메모리 해제
 */
#include <windows.h>
#include <stdio.h>
#include <wchar.h>

#include "common.h"
#include "food.h"
#include "recipe.h"
#include "stats.h"
#include "fileio.h"
#include "ui_gui.h"

/* 작업 디렉터리에 data 폴더가 없으면, 실행 파일 위치에서 위로 올라가며 data 폴더를 찾아 이동한다.
 * (Visual Studio가 아니라 .exe를 직접 더블클릭해도 data/ 를 찾을 수 있게 한다) */
static void fixWorkingDir(void)
{
    wchar_t dir[MAX_PATH];
    int i;
    DWORD a = GetFileAttributesW(L"data");

    if (a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_DIRECTORY))
        return;
    if (GetModuleFileNameW(NULL, dir, MAX_PATH) == 0)
        return;
    for (i = 0; i < 5; i++) {
        wchar_t probe[MAX_PATH + 8];
        wchar_t *p = wcsrchr(dir, L'\\');
        if (p == NULL) return;
        *p = L'\0';                                   /* 파일 이름, 그다음엔 폴더 한 단계씩 제거 */
        swprintf(probe, MAX_PATH + 8, L"%s\\data", dir);
        a = GetFileAttributesW(probe);
        if (a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_DIRECTORY)) {
            SetCurrentDirectoryW(dir);
            return;
        }
    }
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, PWSTR cmdLine, int nShow)
{
    AppData app;
    int rc;

    (void)hPrev;
    (void)cmdLine;
    fixWorkingDir();

    initFridge(&app.fridge);
    initRecipeBook(&app.recipes);
    initWasteList(&app.waste);

    if (loadFridge(FRIDGE_FILE, &app.fridge) != RC_OK)
        MessageBoxW(NULL, L"data/fridge.txt를 읽지 못했습니다.", L"냉장고 관리", MB_OK | MB_ICONWARNING);
    if (loadRecipes(RECIPE_FILE, &app.recipes) != RC_OK)
        MessageBoxW(NULL, L"data/recipes.txt를 읽지 못했습니다.", L"냉장고 관리", MB_OK | MB_ICONWARNING);
    if (loadWaste(WASTE_FILE, &app.waste) != RC_OK)
        MessageBoxW(NULL, L"data/waste.txt를 읽지 못했습니다.", L"냉장고 관리", MB_OK | MB_ICONWARNING);

    guiShowStartupAlert(&app.fridge);                 /* G1 */
    rc = guiRun(hInst, &app, nShow);                  /* 창을 닫을 때까지 */

    if (saveFridge(FRIDGE_FILE, &app.fridge) != RC_OK ||
        saveRecipes(RECIPE_FILE, &app.recipes) != RC_OK ||
        saveWaste(WASTE_FILE, &app.waste) != RC_OK)
        MessageBoxW(NULL, L"저장에 실패했습니다. data 폴더가 있는지 확인하세요.", L"냉장고 관리", MB_OK | MB_ICONERROR);

    freeFridge(&app.fridge);
    freeRecipeBook(&app.recipes);
    freeWasteList(&app.waste);
    return rc;
}

/* ui_gui.h
 * 담당: 윤종하
 * 설명: GUI 화면 (Win32 API). 로직 모듈의 함수만 호출하고 구조체 내부는 직접 고치지 않는다.
 */
#ifndef UI_GUI_H
#define UI_GUI_H

#include <windows.h>
#include "common.h"

/* G1: 시작 시 유통기한 알림 팝업. 임박·지난 재료가 없으면 아무것도 띄우지 않는다 */
void guiShowStartupAlert(const Fridge *f);

/* G2~G5: 메인 창을 만들고 창이 닫힐 때까지 메시지 루프를 돈다. 종료 코드를 반환한다 */
int  guiRun(HINSTANCE hInst, AppData *app, int nShow);

#endif

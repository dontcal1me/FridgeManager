/* dlg_simple.h
 * 담당: 장태현
 * 설명: 간단한 대화상자 두 개 - 숫자 한 개 입력(수량 변경), 글 보기(폐기 통계)
 */
#ifndef DLG_SIMPLE_H
#define DLG_SIMPLE_H

#include "gui_common.h"

/* 숫자 한 개를 입력받는다 (min~max). 확인을 눌렀으면 1이고 *out에 값이 들어간다 */
int  promptNumber(HWND owner, const wchar_t *title, const wchar_t *label,
                  int initial, int min, int max, int *out);

/* 여러 줄 글을 보여 주는 창 (닫기 버튼 하나) */
void textDialog(HWND owner, const wchar_t *title, const wchar_t *text);

#endif

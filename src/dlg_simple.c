/* dlg_simple.c
 * 담당: 장태현
 * 설명: 간단한 대화상자 - 숫자 한 개 입력(수량 변경), 글 보기(폐기 통계)
 */
#include "dlg_simple.h"
#include "gui_dialog.h"

/* TODO(장태현): 구현 전 뼈대. 컴파일만 되는 빈 껍데기입니다. */

int promptNumber(HWND owner, const wchar_t *title, const wchar_t *label,
                 int initial, int min, int max, int *out)
{
    (void)owner; (void)title; (void)label; (void)initial; (void)min; (void)max; (void)out;
    return 0;
}

void textDialog(HWND owner, const wchar_t *title, const wchar_t *text)
{
    (void)owner; (void)title; (void)text;
}
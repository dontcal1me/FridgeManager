/* gui_dialog.h
 * 담당: 윤종하
 * 설명: 모달 대화상자 틀. 각 대화상자(dlg_*.c)는 DlgBase를 맨 앞에 둔 구조체를 만들고
 *       컨트롤 만들기·명령 처리 함수를 연결한 뒤 runDialog로 띄운다.
 */
#ifndef GUI_DIALOG_H
#define GUI_DIALOG_H

#include "gui_common.h"

/* 대화상자 컨트롤 ID */
enum {
    ID_NAME = 200, ID_DATE, ID_QTYEDIT, ID_FRIDGE, ID_FREEZER, ID_COOK, ID_ADDREC,
    ID_DETAIL, ID_RLIST, ID_RNAME, ID_STEPS, ID_PROMPT, ID_FETCH, ID_MODE,
    ID_ING_NAME = 300,                        /* 300 + 재료 번호 */
    ID_ING_QTY = 320                          /* 320 + 재료 번호 */
};

typedef struct DlgBase {
    int done;                                                    /* 1이면 창을 닫는다 */
    void    (*onCreate)(HWND, struct DlgBase *);                 /* 컨트롤 만들기 */
    void    (*onCommand)(HWND, struct DlgBase *, int id, int code);
    LRESULT (*onNotify)(HWND, struct DlgBase *, NMHDR *);
    void    (*onTimer)(HWND, struct DlgBase *, UINT_PTR id);
    LRESULT (*onUser)(HWND, struct DlgBase *, UINT msg, WPARAM, LPARAM);   /* WM_APP 이상 사용자 메시지 */
} DlgBase;

/* 대화상자 창 클래스를 등록한다 (프로그램 시작 때 한 번). 성공하면 1 */
int  registerDialogClass(HINSTANCE hInst);

/* 대화상자를 owner 위에 띄우고 닫힐 때까지 자기 메시지 루프를 돈다 (cw, ch: 클라이언트 크기, 96dpi) */
void runDialog(HWND owner, const wchar_t *title, int cw, int ch, DlgBase *b);

#endif
